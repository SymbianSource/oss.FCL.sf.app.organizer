/*
 * Copyright (c) 2010 Sun Microsystems, Inc. and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributor:
 * Maximilian Odendahl
 *
 * Contributors:
 * 
 * Description:  main Caldav class, all magic happens here 
 *				one engine for each configured Caldav calendar
 */
#include "caldavengine.h"
#include <f32file.h> 
#include <S32FILE.H>
#include <S32MEM.H> 
#include <avkon.rsg>
#include <UTF.H>
#include <agnexportobserver.h>
#include <calsession.h>
#include <caliterator.h>
#include <calentry.h>
#include <calentryview.h>
#include <caltime.h>
#include <calalarm.h>
#include <calcategory.h>
#include <caluser.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caleninterimutils2.h> 
#include <calenexporter.h>
#include <calenimporter.h>
#include <calcalendarinfo.h>
#include "calenglobaldata.h"
#include <xmlengdocument.h>
#include <xmlengnode.h>
#include <xmlengelement.h>
#include <xmlengnodelist.h>
#include <xmlengtext.h>
#include "httpclient.h"
#include <e32math.h>	// divmod
#define MULTIGETSPLIT 			100
#define ARRAYEXPAND				4
#define	URLMAX					500
#define EXPANDSIZE_SMALL		512
#define EXPANDSIZE_BIG			1024
_LIT(KFormatString,"\"%F%Y%M%DT000000Z\"");

/**
 * SyncTickL
 * timer for basic sync interval
 */
TInt SyncTickL(TAny *aObject)
	{
	CCalDavEngine* engine = ((CCalDavEngine*) aObject);
	CPeriodic* timer = engine->Timer();
	if (engine && timer)
		{
		timer->Cancel();
		TRAP_IGNORE(engine->SyncL());
		TInt ticktime = engine->SyncInterval().Int() == 0 ? 1
				: engine->SyncInterval().Int();
		const TUint64 tickInterval = 1000000 * 60 * ticktime;
		timer->Start(tickInterval, tickInterval, TCallBack(SyncTickL, engine));
		}
	return 0;
	}

#ifdef _DEBUG
void ExportToFileNameL(const TDesC8& aDes, const TDesC &aFilename)
	{
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);

	TInt err = fsSession.MkDirAll(aFilename);

	RFile file;
	User::LeaveIfError(file.Replace(fsSession, aFilename, EFileWrite));
	CleanupClosePushL(file);

	RFileWriteStream writeStream2;
	writeStream2.Attach(file);
	CleanupClosePushL(writeStream2);

	writeStream2 << aDes;
	writeStream2.CommitL();
	CleanupStack::PopAndDestroy(&writeStream2);
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&fsSession);
	}

void ExportToFileL(CCalEntry* aEntry, CCalenExporter *iCalExporter)
	{
	if (aEntry)
		{
		_LIT(KFileName, "C:\\logs\\caldav\\testing_export.txt");

		RFs fsSession;
		User::LeaveIfError(fsSession.Connect());
		CleanupClosePushL(fsSession);

		TInt err = fsSession.MkDirAll(KFileName);

		RFile file;
		User::LeaveIfError(file.Replace(fsSession, KFileName, EFileWrite));
		CleanupClosePushL(file);

		RFileWriteStream writeStream2;
		writeStream2.Attach(file);
		CleanupClosePushL(writeStream2);

		iCalExporter->ExportICalL(*aEntry, writeStream2);
		writeStream2.CommitL();
		CleanupStack::PopAndDestroy(&writeStream2);
		CleanupStack::PopAndDestroy(&file);
		CleanupStack::PopAndDestroy(&fsSession);
		}
	}
#endif

/**
 * SearchL
 * search inside xml tree for a specific node
 */
void SearchL(TXmlEngNode &aTopNode, const TDesC8 &aName,
		const TDesC8 &aNamespace, TXmlEngNode & aReturnNode)
	{
	RXmlEngNodeList<TXmlEngNode> List;
	aTopNode.GetChildNodes(List);
	while (List.HasNext() && aReturnNode.IsNull())
		{
		TXmlEngNode Node = List.Next();
		TXmlEngNode::TXmlEngDOMNodeType Type = Node.NodeType();
		if (Type == TXmlEngNode::EElement)
			{
			//todo: compare namespace ?
			if (Node.Name() == aName)
				{
				aReturnNode = Node.CopyL();
				return;
				}
			else
				{
				SearchL(Node, aName, aNamespace, aReturnNode);
				if (!aReturnNode.IsNull())
					return;
				}
			}
		}
	}

/**
 * SearchL
 * search inside xml tree for a specific node
 */
void SearchL(TXmlEngNode &aTopNode, const TDesC8 &aName,
		const TDesC8 &aNamespace, RBuf8 &aBuf)
	{
	RXmlEngNodeList<TXmlEngNode> List;
	aTopNode.GetChildNodes(List);
	while (List.HasNext())
		{
		TXmlEngNode Node = List.Next();
		TXmlEngNode::TXmlEngDOMNodeType Type = Node.NodeType();
		if (Type == TXmlEngNode::EElement)
			{
			//todo: compare namespace ?
			if (Node.Name() == aName)
				{
				if (Node.IsSimpleTextContents())
					aBuf.Create(Node.Value());
				else
					Node.WholeTextContentsCopyL(aBuf);
				return;
				}
			else
				{
				SearchL(Node, aName, aNamespace, aBuf);
				if (aBuf.Length())
					return;
				}
			}
		}
	}

/**
 * SearchL
 * search inside xml tree for a specific node
 */
TPtrC8 SearchL(TXmlEngNode &aTopNode, const TDesC8 &aName,
		const TDesC8 &aNamespace)
	{
	RXmlEngNodeList<TXmlEngNode> List;
	aTopNode.GetChildNodes(List);
	while (List.HasNext())
		{
		TXmlEngNode Node = List.Next();
		TXmlEngNode::TXmlEngDOMNodeType Type = Node.NodeType();
		if (Type == TXmlEngNode::EElement)
			{
			//todo: compare namespace ?
			if (Node.Name() == aName)
				return Node.Value();
			else
				{
				TPtrC8 Return = SearchL(Node, aName, aNamespace);
				if (Return != KNullDesC8)
					return Return;
				}
			}
		}
	return KNullDesC8();
	}

/**
 * GetBoolFromPropertiesL
 * get a boolean value from CCalCalendarInfo
 */
TBool GetBoolFromPropertiesL(CCalCalendarInfo* info, const TDesC8 &aKey)
	{
	TBool boolean;
	TPckgC<TBool> pckgboolean(boolean);
	pckgboolean.Set(info->PropertyValueL(aKey));
	return pckgboolean();
	}

/**
 * GetTimeFromPropertiesL
 * get a time value from CCalCalendarInfo
 */
TCalTime GetTimeFromPropertiesL(CCalCalendarInfo* info, const TDesC8 &aKey)
	{
	TCalTime time;
	TPckgC<TCalTime> pckgtime(time);
	pckgtime.Set(info->PropertyValueL(aKey));
	return pckgtime();
	}

/**
 * PropertyExists
 * finds a property from an array
 */
TBool PropertyExists(CDesC8Array* aArray, const TDesC8 &aKey)
	{
	TInt pos = 0;
	return aArray->Find(aKey, pos, ECmpNormal) == KErrNone;
	}

/**
 * CCalDavEngine::SetCalendarInfoL
 * set key and value pair at calendar session info
 */
void CCalDavEngine::SetCalendarInfoL(const TDesC8 &aKey, const TDesC8 &aValue)
	{
	TBool createdelete = !iCalSession;
	if (createdelete)
		{
		iCalSession = CCalSession::NewL();
		TRAP_IGNORE(iCalSession->OpenL(*iCalendar));
		}
	CCalCalendarInfo* calendarInfo = iCalSession->CalendarInfoL();
	CleanupStack::PushL(calendarInfo);
	TPckgC<TBool> enabled(iEnabled);
	calendarInfo->SetPropertyL(KCaldavEnabled, enabled);
	if (aValue != KNullDesC8)
		calendarInfo->SetPropertyL(aKey, aValue);
	iCalSession->SetCalendarInfoL(*calendarInfo);
	CleanupStack::PopAndDestroy(calendarInfo);

	if (createdelete)
		{
		delete iCalSession;
		iCalSession = NULL;
		}
	}

/**
 * CCalDavEngine::CCalDavEngine
 * default constructor
 */
CCalDavEngine::CCalDavEngine() :
	iCalSession(0), iCalIter(0), iCalEntryView(0), iCalExporter(0),
			iCalImporter(0), iCalIntermimUtils2(0), iCalendar(0), iUrl(0),
			iBaseUrl(0), iHome(0), iHttp(0), iSynctoken(0), iCTag(0),
			iSyncInterval(DEFAULT_SYNC_MINUTES), iPastDays(DEFAULT_PAST_DAYS),
			iImmediateSync(DEFAULT_IMMEDIATE_SYNC), iKeepServerEntry(
					DEFAULT_KEEP_SERVER_ENTRY), iEnabled(EFalse), iTimer(0)
	{
	}

/**
 * CCalDavEngine::~CCalDavEngine
 * default destructor
 */
CCalDavEngine::~CCalDavEngine()
	{
	iLocalUidArray.Close();
	iGlobalUidArray.Close();
	iDeletedEntries.Close();

	iDomParser.Close();
	iDomImpl.Close();

	if (iCalendar)
		delete iCalendar;

	if (iUrl)
		delete iUrl;

	if (iBaseUrl)
		delete iBaseUrl;

	if (iHttp)
		delete iHttp;

	DeleteCalObjects();

	if (iCalIntermimUtils2)
		delete iCalIntermimUtils2;

	if (iSynctoken)
		delete iSynctoken;

	if (iCTag)
		delete iCTag;

	if (iHome)
		delete iHome;

	if (iTimer)
		delete iTimer;
	}

/**
 * CCalDavEngine::NewLC
 * first phase construction
 */
CCalDavEngine* CCalDavEngine::NewLC(const TDesC& aCalendar)
	{
	CCalDavEngine* self = new (ELeave) CCalDavEngine();
	CleanupStack::PushL(self);
	self->ConstructL(aCalendar);
	return self;
	}

/**
 * CCalDavEngine::NewL
 * first phase construction
 */
CCalDavEngine* CCalDavEngine::NewL(const TDesC& aCalendar)
	{
	CCalDavEngine* self = CCalDavEngine::NewLC(aCalendar);
	CleanupStack::Pop(self); // self;
	return self;
	}

/**
 * CCalDavEngine::
 * second phase construction
 */
void CCalDavEngine::ConstructL(const TDesC& aCalendar)
	{
	iManualSync = EFalse;
	iFirstInit = ETrue;
	iCalendar = aCalendar.AllocL();

	iHttp = CHttpClient::NewL();

	iDomImpl.OpenL();
	TInt err = iDomParser.Open(iDomImpl);
	if (KErrNone != err)
		User::Leave(err);

	iTimer = CPeriodic::NewL(EPriorityNormal);
	iCalIntermimUtils2 = CCalenInterimUtils2::NewL();
	iCalSession = CCalSession::NewL();
	TRAP_IGNORE(iCalSession->OpenL(aCalendar));
	CalendarInfoL();
	// we can't close the file, so delete it completly
	delete iCalSession;
	iCalSession = NULL;
	}

/**
 * CCalDavEngine::Progress
 * Progress callback
 */
void CCalDavEngine::Progress(TInt /*aPercentageCompleted*/)
	{
	}

/**
 * CCalDavEngine::RegisterL
 * register all neccessary notification callback
 */
void CCalDavEngine::RegisterL()
	{
	TDateTime Start;
	TDateTime End;
	End.SetYear(2200);

	TCalTime StartCal;
	TCalTime EndCal;
	StartCal.SetTimeLocalL(Start);
	EndCal.SetTimeLocalL(End);
	CalCommon::TCalTimeRange Range(StartCal, EndCal);
	CCalChangeNotificationFilter *Filter = CCalChangeNotificationFilter::NewL(
			MCalChangeCallBack2::EChangeEntryAll, true, Range);
	iCalSession->StartChangeNotification(*this, *Filter);
	iCalSession->StartFileChangeNotificationL(*this);
	delete Filter;
	}

/**
 * CCalDavEngine::CalendarInfoL
 * load all properties from CalendarInfo
 */
void CCalDavEngine::CalendarInfoL()
	{
	CCalCalendarInfo* calendarInfo = iCalSession->CalendarInfoL();
	if (calendarInfo->IsValid())
		{
		CleanupStack::PushL(calendarInfo);
		CDesC8Array* propertyKeys = calendarInfo->PropertyKeysL();
		CleanupStack::PushL(propertyKeys);
		TInt pos = 0;
		if (propertyKeys->Find(KCaldavEnabled, pos, ECmpNormal) == KErrNone)
			{
			iEnabled = GetBoolFromPropertiesL(calendarInfo, KCaldavEnabled);
			if (PropertyExists(propertyKeys, KCaldavFirstInit))
				iFirstInit = GetBoolFromPropertiesL(calendarInfo,
						KCaldavFirstInit);
			if (PropertyExists(propertyKeys, KCaldavSynctoken))
				iSynctoken
						= calendarInfo->PropertyValueL(KCaldavSynctoken).AllocL();
			if (PropertyExists(propertyKeys, KCaldavCtag))
				iCTag = calendarInfo->PropertyValueL(KCaldavCtag).AllocL();
			if (PropertyExists(propertyKeys, KCaldavManualSync))
				iManualSync = GetBoolFromPropertiesL(calendarInfo,
						KCaldavManualSync);
			if (PropertyExists(propertyKeys, KCaldavTime))
				iLastSyncTime = GetTimeFromPropertiesL(calendarInfo,
						KCaldavTime);
			if (PropertyExists(propertyKeys, KCaldavUrl))
				iUrl = calendarInfo->PropertyValueL(KCaldavUrl).AllocL();
			if (PropertyExists(propertyKeys, KCaldavUser))
				iHttp->SetUserL(calendarInfo->PropertyValueL(KCaldavUser));
			if (PropertyExists(propertyKeys, KCaldavPassword))
				iHttp->SetPasswordL(calendarInfo->PropertyValueL(
						KCaldavPassword));
			if (PropertyExists(propertyKeys, KCaldavKeepServer))
				iKeepServerEntry = GetBoolFromPropertiesL(calendarInfo,
						KCaldavKeepServer);
			if (PropertyExists(propertyKeys, KCaldavImmediateSync))
				iImmediateSync = GetBoolFromPropertiesL(calendarInfo,
						KCaldavImmediateSync);
			if (PropertyExists(propertyKeys, KCaldavPastDays))
				{
				TPckgC<TTimeIntervalDays> pastdays(iPastDays);
				pastdays.Set(calendarInfo->PropertyValueL(KCaldavPastDays));
				iPastDays = pastdays();
				}
			if (PropertyExists(propertyKeys, KCaldavSyncInterval))
				{
				TPckgC<TTimeIntervalMinutes> syncinterval(iSyncInterval);
				syncinterval.Set(calendarInfo->PropertyValueL(
						KCaldavSyncInterval));
				iSyncInterval = syncinterval();
				}
			// access point	    
			}

		CleanupStack::PopAndDestroy(propertyKeys);
		CleanupStack::Pop(calendarInfo);
		}
	delete calendarInfo;
	}

/**
 * CCalDavEngine::InitL
 * check for correct url
 * load most Caldav url properties
 * do inital sync
 */
TInt CCalDavEngine::InitL()
	{
	// this is only needed to find a GlobalUID from a LocalUID, used after an event was deleted
	// also used now for uploading of local events when only GlobalUID is know 
	LocalLoopL(ELoopActionFillArray);

	if (iFirstInit)
		{
		TInt err = GetCalendarUrlsL(NULL);
		if (err == KErrArgument)
			return KErrArgument;
		GetOptionsL();
		SetLastSyncTimeL();

		TBool success;
		// get all server items
		if (iOptions.sync_collection)
			success = WebDavSyncL();
		else
			success = ListL() == KErrNone;

		if (!success)
			return KErrGeneral;

		// upload all local entries of this calendar to server
		LocalLoopL(ELoopActionUpload);

		if (iOptions.sync_collection)
			SetSyncTokenL(GetSyncTokenL());
		else
			SetCTagL(GetCTagL());

		iFirstInit = EFalse;
		iEnabled = ETrue;
		TPckgC<TBool> firstInit(iFirstInit);
		SetCalendarInfoL(KCaldavFirstInit, firstInit); // this will set iEnabled as well
		}
	else
		{
		TInt err = GetCalendarUrlsL(NULL);
		if (err == KErrArgument)
			return KErrArgument;
		GetOptionsL();
		SetLastSyncTimeL();
		iEnabled = ETrue;
		SetCalendarInfoL(KCaldavEnabled, KNullDesC8);
		SyncL();
		}

	return KErrNone;
	}

/**
 * CCalDavEngine::Completed
 * Completed callback
 */
void CCalDavEngine::Completed(TInt aError)
	{
	if (aError == KErrNone)
		{
		CActiveScheduler::Stop();
		}
	else
		iManualSync = true;
	}

/**
 * CCalDavEngine::NotifyProgress
 * NotifyProgress callback
 */
TBool CCalDavEngine::NotifyProgress()
	{
	return EFalse;
	}

/**
 * CCalDavEngine::CalChangeNotification
 * change item callback, sync to server
 */
void CCalDavEngine::CalChangeNotification(RArray<TCalChangeEntry> &aChangeItems)
	{
	for (TInt i = 0; i < aChangeItems.Count(); i++)
		{
		TRAP_IGNORE(HandleChangeL(aChangeItems[i].iChangeType, aChangeItems[i].iEntryType, aChangeItems[i].iEntryId));
		}
	}

/**
 * CCalDavEngine::CalendarInfoChangeNotificationL
 * change callback, sync changed color or name to server
 */
void CCalDavEngine::CalendarInfoChangeNotificationL(RPointerArray<
		CCalFileChangeInfo>& aCalendarInfoChangeEntries)
	{
	for (TInt i = 0; i < aCalendarInfoChangeEntries.Count(); i++)
		{
		if ((aCalendarInfoChangeEntries[i]->FileNameL() == *iCalendar)
				&& (aCalendarInfoChangeEntries[i]->ChangeType()
						== MCalFileChangeObserver::ECalendarInfoUpdated))
			{
			TRAP_IGNORE(HandleCalendarInfoChangeL());
			}
		}
	}

/**
 * CCalDavEngine::HandleCalendarInfoChangeL
 * sync changed color or name to server
 */
void CCalDavEngine::HandleCalendarInfoChangeL()
	{
	if (iHttp)
		{
		CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
		CleanupStack::PushL(response);
		CCalCalendarInfo* info = iCalSession->CalendarInfoL();
		CleanupStack::PushL(info);

		HBufC8* name =
				CnvUtfConverter::ConvertFromUnicodeToUtf8L(info->NameL());
		CleanupStack::PushL(name);

		TRgb color = info->Color();
		_LIT8(KColorformat,"#%02x%02x%02xFF");
		TBuf8<9> colorbuf;
		colorbuf.Format(KColorformat, color.Red(), color.Green(), color.Blue());

		HBufC8* patch = HBufC8::NewLC(KColorDisplayStart().Length()
				+ colorbuf.Length() + KColorDisplayMiddle().Length()
				+ name->Length() + KColorDisplayEnd().Length());
		patch->Des().Append(KColorDisplayStart);
		patch->Des().Append(colorbuf);
		patch->Des().Append(KColorDisplayMiddle);
		patch->Des().Append(*name);
		patch->Des().Append(KColorDisplayEnd);
		iHttp->ProppatchL(*iUrl, *patch, response);
		CleanupStack::PopAndDestroy(4); // response, info, name, patch
		}
	}

/**
 * CCalDavEngine::HandleChangeL
 * change item callback, sync to server
 */
void CCalDavEngine::HandleChangeL(
		MCalChangeCallBack2::TChangeType &aChangeType,
		MCalChangeCallBack2::TChangeEntryType &aEntryType, TCalLocalUid &aUid)
	{
	switch (aChangeType)
		{
		case MCalChangeCallBack2::EChangeAdd:
		case MCalChangeCallBack2::EChangeModify:
			{
			if (iImmediateSync)
				UploadEntryL(aUid, aChangeType, aEntryType);
			else
				// enable manual sync for the next sync interval
				SyncFailedL();
			break;
			}
		case MCalChangeCallBack2::EChangeDelete:
			{
			if (iImmediateSync)
				DeleteEntryL(aUid);
			else
				{
				iDeletedEntries.Append(aUid);
				SyncFailedL();
				}
			break;
			}
		case MCalChangeCallBack2::EChangeUndefined:
			{
			// upload new and modified entries to server
			UploadModifiedSinceDateL();

			// Find locally deleted ones and delete on server
			DeleteRemovedEntriesOnServerL();

			break;
			}
		}
	}

/**
 * CCalDavEngine::EnableL
 * enable Caldav sync
 */
TInt CCalDavEngine::EnableL()
	{
	if (!iCalEntryView) //do not use iEnabled here,might be set already in ConstructL()
		{
		TInt aErr = CreateCalObjectsL();
		if ((aErr == KErrNone) && (InitL() == KErrNone))
			{
			iTimer->Cancel();
			TInt ticktime = iSyncInterval.Int() == 0 ? 1 : iSyncInterval.Int();
			const TUint64 tickInterval = 1000000 * 60 * ticktime;
			iTimer->Start(tickInterval, tickInterval,
					TCallBack(SyncTickL, this));
			// register change notification
			RegisterL();
			return KErrNone;
			}
		else
			{
			iEnabled = EFalse;
			SetCalendarInfoL(KCaldavEnabled, KNullDesC8);
			DeleteCalObjects();
			return KErrArgument;
			}
		}
	return KErrNone;
	}

/**
 * CCalDavEngine::DeleteCalObjects
 * delete all calendar objects
 */
void CCalDavEngine::DeleteCalObjects()
	{
	delete iCalIter;
	iCalIter = NULL;
	delete iCalEntryView;
	iCalEntryView = NULL;
	delete iCalImporter;
	iCalImporter = NULL;
	delete iCalExporter;
	iCalExporter = NULL;
	if (iCalSession)
		{
		iCalSession->StopChangeNotification();
		iCalSession->StopFileChangeNotification();
		delete iCalSession;
		}
	iCalSession = NULL;
	}

/**
 * CCalDavEngine::CreateCalObjectsL
 * create all calendar objects
 */
TInt CCalDavEngine::CreateCalObjectsL()
	{
	iCalSession = CCalSession::NewL();
	TRAPD(aErr,iCalSession->OpenL(*iCalendar));
	iCalExporter = CCalenExporter::NewL(*iCalSession);
	iCalImporter = CCalenImporter::NewL(*iCalSession);
	iCalEntryView = CCalEntryView::NewL(*iCalSession);
	iCalIter = CCalIter::NewL(*iCalSession);
	return aErr;
	}

/**
 * CCalDavEngine::DisableL
 * disable sync
 */
void CCalDavEngine::DisableL()
	{
	if (iEnabled)
		{
		iTimer->Cancel();
		iEnabled = EFalse;
		SetCalendarInfoL(KCaldavEnabled, KNullDesC8);
		DeleteCalObjects();
		}
	}

/**
 * CCalDavEngine::EnabledSync
 * check for enabled sync
 */
TBool CCalDavEngine::EnabledSync()
	{
	return iEnabled;
	}

/**
 * CCalDavEngine::TimeReportL
 * do a CalDav time report
 */
TInt CCalDavEngine::TimeReportL(TBool VEVENT, const TDesC8 &aStart,
		TBool aDelete)
	{
	CBufFlat* body = CBufFlat::NewL(EXPANDSIZE_BIG);
	CleanupStack::PushL(body);

	body->InsertL(body->Size(), VEVENT ? KTimeStartEVENT() : KTimeStartTODO());
	body->InsertL(body->Size(), aStart); // "20090509T220000Z"/>
	body->InsertL(body->Size(), KTimeEnd);

	CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_BIG);
	CleanupStack::PushL(response);
	TInt ret = iHttp->ReportL(*iUrl, body->Ptr(0), response);

	if (ret == MULTISTATUS)
		ret = aDelete ? ParseResponsesDeleteL(response->Ptr(0))
				: ParseResponsesL(response->Ptr(0));
	else
		ret = KErrGeneral;

	CleanupStack::PopAndDestroy(response);
	CleanupStack::PopAndDestroy(body);
	return ret;
	}

/**
 * CCalDavEngine::ListL
 * get events either using time report or basic propfind
 */
TInt CCalDavEngine::ListL()
	{
	if (iOptions.calendar_access)
		{
		TTime syncstart;
		syncstart.HomeTime();
		syncstart = syncstart - iPastDays;
		TBuf<100> nowStr;
		syncstart.FormatL(nowStr, KFormatString);
		TBuf8<100> nowStrAdd;
		nowStrAdd.Append(nowStr);

		TInt eventreturn = KErrNone;
		if (iOptions.VEVENT)
			eventreturn = TimeReportL(ETrue, nowStrAdd);

		TInt todoreturn = KErrNone;
		if (iOptions.VTODO)
			todoreturn = TimeReportL(EFalse, nowStrAdd);

		return (eventreturn == KErrNone) && (todoreturn == KErrNone) ? KErrNone
				: KErrGeneral;
		}
	else
		{
		// use PROPFIND report
		CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
		CleanupStack::PushL(response);
		TInt ret = iHttp->PropfindL(*iUrl, KPropList, response, EFalse);
		if (ret == MULTISTATUS)
			ret = ParseResponsesL(response->Ptr(0));
		else
			ret = KErrGeneral;
		CleanupStack::PopAndDestroy(response);
		return ret;
		}
	}

/**
 * CCalDavEngine::SyncL
 * sync a calendar
 */
TInt CCalDavEngine::SyncL()
	{
	if (iUrl)
		{
		if (iOptions.sync_collection)
			return WebDavSyncL();
		else
			return ManualSyncL();
		}
	return KErrGeneral;
	}

/**
 * CCalDavEngine::ManualSyncL
 * if sync failed previously, try again manually
 */
TBool CCalDavEngine::ManualSyncL()
	{
	TBool client = ClientChangesL();
	if (iOptions.sync_ctag)
		{
		HBufC8 *newCTag = GetCTagL();
		TBool server = ETrue;
		if ((CTag() != *newCTag) && (*newCTag != KNullDesC8))
			{
			server = ServerChangesL();
			if (server)
				SetCTagL(newCTag);
			else
				delete newCTag;
			}
		else
			delete newCTag;

		return client || server;
		}
	else
		{
		TBool server = ServerChangesL();
		return client || server;
		}
	}

/**
 * CCalDavEngine::ServerChangesL
 * sync server changes
 */
TBool CCalDavEngine::ServerChangesL()
	{
	// loop over all server items to find new and modified entries
	// uses either propfind or calendar-query
	TInt ret = ListL();

	// loop over all local items to find deleted ones on the server
	LocalLoopL(ELoopActionDeleteLocal);

	return ret == KErrNone;
	}

/**
 * CCalDavEngine::LocalLoopL
 * loop over local calendar store
 */
TInt CCalDavEngine::LocalLoopL(TLocalLoopAction aAction)
	{
	TBuf8<URLMAX> iter = iCalIter->FirstL();
	TBuf8<URLMAX> url;
	while (iter != KNullDesC8)
		{
		url.Append(*iUrl);
		url.Append(iter);
		url.Append(KIcs);
		switch (aAction)
			{
			case ELoopActionDeleteLocal:
				{
				if (HeadL(iter) == NOTFOUND)
					DeleteLocalEntryL(url);
				break;
				}
			case ELoopActionFillArray:
				{
				RPointerArray<CCalEntry> entryArray;
				CleanupClosePushL(entryArray);
				iCalEntryView->FetchL(iter, entryArray);
				if (entryArray.Count())
					{
					iLocalUidArray.Append(entryArray[0]->LocalUidL());
					iGlobalUidArray.Append(entryArray[0]->UidL());
					}
				entryArray.ResetAndDestroy();
				CleanupStack::PopAndDestroy(&entryArray);

				break;
				}
			case ELoopActionUpload:
				{
				TInt pos = iGlobalUidArray.Find(iter);
				if ((pos != KErrNotFound) && (iLocalUidArray.Count() > pos))
					UploadEntryL(iLocalUidArray[pos],
							MCalChangeCallBack2::EChangeAdd,
							MCalChangeCallBack2::EChangeEntryAll);
				break;
				}
			}
		url.Delete(0, url.Length());
		iter = iCalIter->NextL();
		}
	return KErrNone;
	}

/**
 * CCalDavEngine::ParseResponsesDeleteL
 * process a recieved multistatus response
 */
TInt CCalDavEngine::ParseResponsesDeleteL(const TDesC8 &aDocument)
	{
	TInt ret = KErrNone;
	RXmlEngDocument document = iDomParser.ParseL(aDocument);
	CleanupClosePushL(document);
	if (document.NotNull())
		{
		// this method works for response as well as sync-response
		// do not use GetElementsByTagNameL for one specific responses directly	
		TXmlEngElement ResponseListTop;
		SearchL(document, KMultistatus, KNullDesC8, ResponseListTop);

		RXmlEngNodeList<TXmlEngNode> ResponseList;
		CleanupClosePushL(ResponseList);
		ResponseListTop.GetChildNodes(ResponseList);

		while (ResponseList.HasNext())
			{
			TXmlEngNode node = ResponseList.Next();
			if (node.NodeType() == TXmlEngNode::EElement)
				{
				TPtrC8 href = SearchL(node, KHref, KNullDesC8);
				// don't do anything with home itself
				if ((href.Right(KIcs().Length()) == KIcs))
					{
					if (!DoesEntryExistL(href))
						DeleteEntryL(href);
					}
				}
			}
		CleanupStack::PopAndDestroy(&ResponseList);

		}
	else
		ret = KErrArgument;
	CleanupStack::PopAndDestroy(&document);
	return ret;
	}

/**
 * CCalDavEngine::ParseResponsesL
 * process a recieved multistatus response
 */
TInt CCalDavEngine::ParseResponsesL(RXmlEngDocument &aDocument, TBool aMultiget)
	{
	TInt ret = KErrNone;
	if (aDocument.NotNull())
		{
		CDesC8ArrayFlat *multiget = NULL;
		if (iOptions.MULTIGET)
			{
			multiget = new (ELeave) CDesC8ArrayFlat(ARRAYEXPAND);
			CleanupStack::PushL(multiget);
			}

		// this method works for response as well as sync-response
		// do not use GetElementsByTagNameL for one specific responses directly	
		TXmlEngElement ResponseListTop;
		SearchL(aDocument, KMultistatus, KNullDesC8, ResponseListTop);

		RXmlEngNodeList<TXmlEngNode> ResponseList;
		CleanupClosePushL(ResponseList);
		ResponseListTop.GetChildNodes(ResponseList);

		while (ResponseList.HasNext())
			{
			TXmlEngNode node = ResponseList.Next();
			if (node.NodeType() == TXmlEngNode::EElement)
				{
				TPtrC8 href = SearchL(node, KHref, KNullDesC8);
				//do not use Search, only looking for first childs,
				//as D:propstat has D:status as well
				RXmlEngNodeList<TXmlEngElement> statuslist;
				CleanupClosePushL(statuslist);
				node.AsElement().GetElementsByTagNameL(statuslist, KStatus,
						KDav);
				//only one or zero item
				HBufC8* status =
						statuslist.Count() ? statuslist.Next().Value().AllocL()
								: KNullDesC8().AllocL();
				CleanupStack::PopAndDestroy(&statuslist);
				CleanupStack::PushL(status);
				status->Des().LowerCase();
				TPtrC8 etag = SearchL(node, KEtag, KNullDesC8);
				RBuf8 calendardata;
				SearchL(node, KCalendarData, KNullDesC8, calendardata);
				calendardata.CleanupClosePushL();

				// don't do anything with home itself
				if (href.Right(KIcs().Length()) == KIcs)
					{
					if ((*status == KHTTP200) || (*status == KHTTP201) || (*status == KNullDesC8))
						{
						if ((calendardata == KNullDesC8))
							{
							if (aMultiget)
								{
								// ATTENTION: an empty response to a multiget should never happen
								// data wrapped inside CDATA, e.g. bedework ??
								}
							else
								//TOOD: if this is a webdav sync response, we should skip the etag check
								AddModifyLocalEntryL(href, etag, multiget);
							}
						else
							{
							// response to a multiget or time-range report, we now already have everything we need
							StoreEntryL(calendardata, etag);
							}
						}
					else if (*status == KHTTP404)
						{
						if (iOptions.sync_collection)
							{
							// if this is an initial sync without token, 
							// this should be ignored, Sun Server bug!!!
							if (SyncToken() != KNullDesC8)
								DeleteLocalEntryL(href);
							}
						else
							{
							//multiget answer, but deleted in the meantime, should delete locally as well
							DeleteLocalEntryL(href);
							}
						}
					}
				CleanupStack::PopAndDestroy(&calendardata);
				CleanupStack::PopAndDestroy(status);
				}
			}
		CleanupStack::PopAndDestroy(&ResponseList);

		if (iOptions.MULTIGET)
			{
			if (multiget->Count())
				{
				DownloadEntryL(multiget);
				multiget->Reset();
				}
			CleanupStack::PopAndDestroy(multiget);
			}
		}
	else
		ret = KErrArgument;

	return ret;
	}

/**
 * CCalDavEngine::ParseResponsesL
 * process a recieved multistatus response
 */
TInt CCalDavEngine::ParseResponsesL(const TDesC8 &aDocument, TBool aMultiget)
	{
#ifdef _DEBUG
	_LIT(KFilename,"c:\\logs\\caldav\\parseresonseslatest.txt");
	ExportToFileNameL(aDocument, KFilename);
#endif

	RXmlEngDocument document = iDomParser.ParseL(aDocument);
	CleanupClosePushL(document);
	TInt ret = ParseResponsesL(document, aMultiget);
	CleanupStack::PopAndDestroy(&document);
	return ret;
	}

/**
 * CCalDavEngine::StoreEntryL
 * store event in local store
 */
#ifdef ETAG
TInt CCalDavEngine::StoreEntryL(const TDesC8 &aBuf, const TDesC8 &aEtag)
#else
TInt CCalDavEngine::StoreEntryL(const TDesC8 &aBuf, const TDesC8 &/*aEtag*/)
#endif
	{
#ifdef _DEBUG
	_LIT(KFileName2, "C:\\logs\\caldav\\testing_import.txt");
	ExportToFileNameL(aBuf, KFileName2);
#endif

	HBufC8* buffer = HBufC8::NewL(aBuf.Length() + 500);
	buffer->Des().Append(aBuf);
	TPtr8 ptr = buffer->Des();
	CalDavUtils::FixImportIssues(ptr);

	RPointerArray<CCalEntry> Array;
	CleanupClosePushL(Array);
	RDesReadStream ReadStream;
	ReadStream.Open(ptr);
	CleanupClosePushL(ReadStream);
#ifdef _DEBUG
	_LIT(KFileName, "C:\\logs\\caldav\\testing_import_fixed.txt");
	ExportToFileNameL(ptr, KFileName);
#endif
	TRAPD(error, iCalImporter->ImportICalendarL(ReadStream,Array));
	CleanupStack::PopAndDestroy(&ReadStream); // calls close on rSteam												
	if ((error == KErrNone) && (Array.Count()))
		{
		iCalIntermimUtils2->StoreL(*iCalEntryView, *Array[0], ETrue); // or should last one be EFalse??
		TInt pos = iLocalUidArray.Find(Array[0]->LocalUidL());
		if (pos == KErrNotFound)
			{
			iLocalUidArray.Append(Array[0]->LocalUidL());
			iGlobalUidArray.Append(Array[0]->UidL());
			}
#ifdef ETAG
		Array[0]->SetETag(aEtag);
#endif
		}
	Array.ResetAndDestroy();
	CleanupStack::PopAndDestroy(&Array);

	delete buffer;
	return error;
	}

/**
 * CCalDavEngine::WebDavSyncReportL
 * webdav sync report
 * http://tools.ietf.org/html/draft-daboo-webdav-sync-02
 */
TInt CCalDavEngine::WebDavSyncReportL(TBool aSynctoken)
	{
	HBufC8 *Buf = HBufC8::NewL(KSync().Length() + SyncToken().Length());
	TPtrC8 token = SyncToken();
	if (aSynctoken)
		Buf->Des().Format(KSync, &token);
	else
		Buf->Des().Format(KSync, &KNullDesC8());
	CleanupStack::PushL(Buf);

	CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_BIG);
	CleanupStack::PushL(response);
	TInt ret = iHttp->ReportL(*iUrl, *Buf, response);

	if (ret == MULTISTATUS)
		{
		RXmlEngDocument document = iDomParser.ParseL(response->Ptr(0));
		CleanupClosePushL(document);
		if (document.NotNull())
			{
			TPtrC8 Token = SearchL(document, KSynctoken, KNullDesC8);
			if ((Token != KNullDesC8) && (Token != SyncToken()))
				{
				ret = ParseResponsesL(document);
				//store newest token
				if (ret == KErrNone)
					SetSyncTokenL(Token.AllocL());
				}
			else
				ret = KErrNone;
			}
		else
			ret = KErrGeneral;
		CleanupStack::PopAndDestroy(&document);
		}
	CleanupStack::PopAndDestroy(response);
	CleanupStack::PopAndDestroy(Buf);
	return ret;
	}

/**
 * CCalDavEngine::WebDavSyncL
 * sync using webdav sync
 * http://tools.ietf.org/html/draft-daboo-webdav-sync-02
 */
TBool CCalDavEngine::WebDavSyncL()
	{
	if (iHttp)
		{
		// commit any left over client changes
		TBool RetClient = ClientChangesL();
		// get all changes from server
		TInt ret = WebDavSyncReportL(ETrue);
		if (ret == CONFLICT)
			ret = WebDavSyncReportL(EFalse);
		return (ret == KErrNone) && RetClient;
		}
	return EFalse;
	}

/**
 * CCalDavEngine::GetUIDByUrl
 * parse url to find UID
 */
TPtrC8 CCalDavEngine::GetUIDByUrl(const TDesC8 &aUrl)
	{
	TPtrC8 UID;
	TInt Pos = aUrl.LocateReverse('/');
	TInt Pos2 = aUrl.Find(KIcs);

	if ((Pos != KErrNotFound) && (Pos2 != KErrNotFound))
		UID.Set(aUrl.Mid(Pos + 1, Pos2 - Pos - 1));
	else
		{
		if (Pos != KErrNotFound)
			UID.Set(aUrl.Mid(Pos + 1, aUrl.Length() - Pos - 1));
		else if (Pos2 != KErrNotFound)
			UID.Set(aUrl.Left(aUrl.Length() - KIcs().Length()));
		else
			UID.Set(aUrl);
		}

	return UID;
	}

/**
 * CCalDavEngine::DoesEntryExistL
 * check if entry exists in local store
 */
unsigned long CCalDavEngine::DoesEntryExistL(const TDesC8 &aUrl)
	{
	// check if we already have it locally by uid
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	iCalEntryView->FetchL(GetUIDByUrl(aUrl), entryArray);
	// get parent 
	CCalEntry *entry = entryArray.Count() ? entryArray[0] : NULL;
	TInt ret = entry ? entry->LocalUidL() : 0;
	entryArray.ResetAndDestroy();
	CleanupStack::PopAndDestroy(&entryArray);
	return ret;
	}

/**
 * CCalDavEngine::ETagMatchL
 * checks for equal ETag
 */
TBool CCalDavEngine::ETagMatchL(const TDesC8& /*aUrl*/, const TDesC8& /*aETag*/)
	{
#ifdef ETAG
	// check if we already have it locally by uid
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	iCalEntryView->FetchL(GetUIDByUrl(aUrl), entryArray);
	// get parent 
	CCalEntry *entry = entryArray.Count() ? entryArray[0] : NULL;
	TBool ret = entry ? entry->ETag() == aETag : EFalse;
	entryArray.ResetAndDestroy();
	CleanupStack::PopAndDestroy(&entryArray);
	return ret;
#else
	return EFalse;
#endif
	}

/**
 * CCalDavEngine::AddModifyLocalEntryL
 * add or modify existing event
 */
TInt CCalDavEngine::AddModifyLocalEntryL(const TDesC8 &aUrl,
		const TDesC8 &aETag, CDesC8ArrayFlat* aArray)
	{
	// check if we have the entry locally
	// check for etag if we have the latest version, if not, download and import or add to multiget request
	if (!ETagMatchL(aUrl, aETag))
		{
		if (aArray)
			aArray->AppendL(aUrl);
		else
			DownloadEntryL(aUrl);
		}
	return KErrNone;
	}

/**
 * CCalDavEngine::DownloadEntryL
 * download entries using multiget from server
 */
TInt CCalDavEngine::DownloadEntryL(CDesC8Array* aArray)
	{
	TInt ret = KErrNone;
	TInt64 remainder;
	TInt64 result = Math::DivMod64(aArray->Count(), MULTIGETSPLIT, remainder);

	// split large multigets request into several smaller ones
	for (TInt64 l = 0; l <= result; l++)
		{
		// do the multiget request and pass it to parserepsonses again to read in the data
		CBufFlat* body = CBufFlat::NewL(EXPANDSIZE_BIG);
		CleanupStack::PushL(body);

		body->InsertL(body->Size(), KMultistart);
		for (TInt64 i = 0; i <= ((l == result) ? remainder - 1 : MULTIGETSPLIT
				- 1); i++)
			{
			body->InsertL(body->Size(), KHrefstart);
			body->InsertL(body->Size(), (*aArray)[MULTIGETSPLIT * l + i]);
			body->InsertL(body->Size(), KHrefend);
			}
		body->InsertL(body->Size(), KMultiend);

		CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_BIG);
		CleanupStack::PushL(response);
		TInt Return = iHttp->ReportL(*iUrl, body->Ptr(0), response);

		if (Return == MULTISTATUS)
			{
			TInt parsereturn = ParseResponsesL(response->Ptr(0), ETrue);
			// if it failed before, we do not want to override this error
			ret = (ret == KErrNone) ? parsereturn : ret;
			}
		else
			ret = KErrGeneral;
		CleanupStack::PopAndDestroy(response);
		CleanupStack::PopAndDestroy(body);

		}
	return ret;
	}

/**
 * CCalDavEngine::DownloadEntryL
 * download entry from server
 */
TInt CCalDavEngine::DownloadEntryL(const TDesC8 &aUrl)
	{
	TBuf8<URLMAX> url;
	url.Append(*iUrl);
	url.Append(GetUIDByUrl(aUrl));
	url.Append(KIcs);

	CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
	CleanupStack::PushL(response);
	TInt ret = iHttp->GetL(url, response);
	if (ret == OK)
		ret = StoreEntryL(response->Ptr(0), iHttp->ETag());
	else
		ret = KErrGeneral;
	CleanupStack::PopAndDestroy(response);
	return ret;
	}

/**
 * CCalDavEngine::DeleteLocalEntryL
 * delete an event from local store
 */
TInt CCalDavEngine::DeleteLocalEntryL(const TDesC8 &aUID)
	{
	CDesC8ArrayFlat * Array = new (ELeave) CDesC8ArrayFlat(ARRAYEXPAND);
	CleanupStack::PushL(Array);
	Array->AppendL(GetUIDByUrl(aUID));
	// we could have delete it ourselves, so it is already gone
	TRAPD(error, iCalEntryView->DeleteL(*Array));
	Array->Reset();
	CleanupStack::PopAndDestroy(Array);
	return error;
	}

/**
 * CCalDavEngine::DeleteRemovedEntriesOnServerL
 * check for removed entries on server
 */
TInt CCalDavEngine::DeleteRemovedEntriesOnServerL()
	{
	if (iOptions.calendar_access)
		{
		TTime syncstart;
		syncstart.HomeTime();
		syncstart = syncstart - iPastDays;
		TBuf<100> nowStr;
		syncstart.FormatL(nowStr, KFormatString);
		TBuf8<100> nowStrAdd;
		nowStrAdd.Append(nowStr);

		TInt eventreturn = KErrNone;
		if (iOptions.VEVENT)
			eventreturn = TimeReportL(ETrue, nowStrAdd, ETrue);

		TInt todoreturn = KErrNone;
		if (iOptions.VTODO)
			todoreturn = TimeReportL(EFalse, nowStrAdd, ETrue);

		return (eventreturn == KErrNone) && (todoreturn == KErrNone) ? KErrNone
				: KErrGeneral;
		}
	else
		{
		// use PROPFIND report
		CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
		CleanupStack::PushL(response);
		TInt ret = iHttp->PropfindL(*iUrl, KPropList, response, EFalse);
		if (ret == MULTISTATUS)
			ret = ParseResponsesDeleteL(response->Ptr(0));
		else
			ret = KErrGeneral;
		CleanupStack::PopAndDestroy(response);
		return ret;
		}

	}

/**
 * CCalDavEngine::UploadModifiedSinceDateL
 * check for any modified data after last sync time
 */
TBool CCalDavEngine::UploadModifiedSinceDateL()
	{
	TBool manualsync = EFalse;
	// upload modified and newly create ones 
	RArray<TCalLocalUid> *Ids = new (ELeave) RArray<TCalLocalUid> (ARRAYEXPAND);
	iCalEntryView->GetIdsModifiedSinceDateL(iLastSyncTime, *Ids);
	for (TInt i = 0; i < Ids->Count(); i++)
		{
		TCalLocalUid id = (*Ids)[i];
		TInt ret = UploadEntryL(id, MCalChangeCallBack2::EChangeUndefined,
				MCalChangeCallBack2::EChangeEntryAll);
		// TOOD: if it fails during upload, ignore
		// if it fails due to internet connection, try again
		if (ret != KErrNone)
			manualsync = ETrue;
		}
	Ids->Reset();
	delete Ids;
	return manualsync;
	}

/**
 * CCalDavEngine::ClientChangesL
 * check for left over local client changes
 */
TBool CCalDavEngine::ClientChangesL()
	{
	if (iCalEntryView && iManualSync)
		{
		iManualSync = EFalse;

		// upload modified and newly create ones 
		iManualSync = UploadModifiedSinceDateL();

		// delete locally deleted entries on server 
		for (TInt i = iDeletedEntries.Count() - 1; i >= 0; --i)
			{
			TInt ret = DeleteEntryL(iDeletedEntries[i]);
			if (ret == KErrNone)
				iDeletedEntries.Remove(i);

			}

		iManualSync = iDeletedEntries.Count() ? ETrue : EFalse;

		TPckgC<TBool> manualSync(iManualSync);
		SetCalendarInfoL(KCaldavManualSync, manualSync);
		}

	return ETrue;
	}

/**
 * CCalDavEngine::MkcalendarL
 * create a new calendar on the server
 */
TInt CCalDavEngine::MkcalendarL(const TDesC8 &aName)
	{
	if (iOptions.MKCALENDAR)
		{
		TBuf8<URLMAX> url;
		url.Append(*iHome);
		url.Append(aName);
		url.Append(KSlash);

		CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
		CleanupStack::PushL(response);

		// TOOD: initialize with name, body etc.
		TInt ret = iHttp->MkCalendarL(url, KNullDesC8, response);

		if ((ret == CREATED) || (ret == OK))
			ret = KErrNone;
		else if ((ret == NOTALLOWED) || (ret == FORBIDDEN))
			ret = KErrArgument;
		else
			ret = KErrGeneral;
		CleanupStack::PopAndDestroy(response);
		return ret;
		}
	else
		return KErrNotSupported;
	}

/**
 * CCalDavEngine::DeleteCalendarL
 * delete a calendar on the server
 */
TInt CCalDavEngine::DeleteCalendarL(const TDesC8 &aName)
	{
	if (iOptions.MKCALENDAR)
		{
		TBuf8<URLMAX> url;
		url.Append(*iHome);
		url.Append(aName);
		url.Append(KSlash);

		CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
		CleanupStack::PushL(response);
		TInt ret = iHttp->DeleteL(url);
		if ((ret == NOCONTENT) || (ret == OK))
			ret = KErrNone;
		else
			ret = KErrGeneral;
		CleanupStack::PopAndDestroy(response);
		return ret;
		}
	else
		return KErrNotSupported;
	}

/**
 * CCalDavEngine::HeadL
 * check for existence of an entry on server
 */
TInt CCalDavEngine::HeadL(const TDesC8 &aUID)
	{
	// special handing for yahoo neccessary
	// after deleting an event, it is still there and findable with HEAD
	_LIT8(KYahoo,"yahoo");
	_LIT8(KTrash,"trash");

	TBuf8<URLMAX> url;
	url.Append(*iUrl);
	url.Append(aUID);
	url.Append(KIcs);
	if (iUrl->Find(KYahoo) == KErrNotFound)
		{
		TInt head = iHttp->HeadL(url);
		return (head == NOCONTENT) || (head == OK) ? OK : head;
		}
	else
		{
		CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
		CleanupStack::PushL(response);
		TInt ret = iHttp->PropfindL(url, KNullDesC8, response);

		if (ret == MULTISTATUS)
			{
			RXmlEngDocument document = iDomParser.ParseL(response->Ptr(0));
			CleanupClosePushL(document);
			if (document.NotNull())
				{
				HBufC8* href = SearchL(document, KHref, KNullDesC8).AllocL();
				href->Des().LowerCase();
				ret = href->Find(KTrash) == KErrNotFound ? OK : NOTFOUND;
				delete href;
				}
			CleanupStack::PopAndDestroy(&document);
			CleanupStack::PopAndDestroy(response);
			return ret;
			}
		else
			{
			CleanupStack::PopAndDestroy(response);
			return NOTFOUND;
			}
		}
	}

/**
 * CCalDavEngine::DeleteEntryL
 * delete entry on server
 */
TInt CCalDavEngine::DeleteEntryL(const TDesC8 &aUid)
	{
	TBuf8<URLMAX> url;
	url.Append(*iBaseUrl);
	url.Append(aUid);
	return iHttp->DeleteL(url);
	}

/**
 * CCalDavEngine::DeleteEntryL
 * delete entry on server
 */
TInt CCalDavEngine::DeleteEntryL(const TCalLocalUid &aUid)
	{
	TInt Ret = KErrNone;
	// find the filename for a given local UID
	TInt aPos = iLocalUidArray.Find(aUid);
	if (aPos != KErrNotFound)
		{
		TBuf8<URLMAX> url;
		url.Append(*iUrl);
		url.Append(iGlobalUidArray[aPos]);
		url.Append(KIcs);

#ifdef ETAG
		CCalEntry* entry = iCalEntryView->FetchL(aUid);
		CleanupStack::PushL(entry);
		TInt Return = entry ? iHttp->DeleteL(url, entry->ETag())
		: iHttp->DeleteL(url);
		CleanupStack::PopAndDestroy(entry);
#else
		TInt Return = iHttp->DeleteL(url);
#endif

		if ((Return == NOCONTENT) || (Return == OK))
			{
			SetLastSyncTimeL();
			}
		else if (Return == PRECONDFAILED)
			{
			// someone modified this in the meantime
			// ask user if he wants the new event or still delete it
			TBool modify = EFalse;
			if (modify)
				DownloadEntryL(url);
			else
				iHttp->DeleteL(url);
			}
		else if (Return == NOTFOUND)
			{
			// someone deleted this already
			Ret = KErrGeneral;
			}
		else
			{
			Ret = KErrGeneral;
			SyncFailedL();
			TInt pos = iDeletedEntries.Find(aUid);
			if (pos == KErrNotFound)
				iDeletedEntries.Append(aUid);
			}
		}
	else
		Ret = KErrGeneral;
	return Ret;
	}

/**
 * CCalDavEngine::UploadEntryL
 * upload entry to server
 */
TInt CCalDavEngine::UploadEntryL(CCalEntry* aEntry,
		MCalChangeCallBack2::TChangeType aChangeType,
		MCalChangeCallBack2::TChangeEntryType aEntryType)
	{
	if (aEntry)
		{
		TInt ret = KErrNone;
		TBool upload = EFalse;
		switch (aEntryType)
			{
			case MCalChangeCallBack2::EChangeEntryEvent:
			case MCalChangeCallBack2::EChangeEntryTodo:
				{
				upload = aEntry && ((MCalChangeCallBack2::EChangeEntryEvent
						&& iOptions.VEVENT)
						|| (MCalChangeCallBack2::EChangeEntryTodo
								&& iOptions.VTODO));
				break;
				}
			case MCalChangeCallBack2::EChangeEntryAll:
				{
				if (aEntry)
					{
					switch (aEntry->EntryTypeL())
						{
						case CCalEntry::EAppt:
						case CCalEntry::EAnniv:
						case CCalEntry::EEvent:
						case CCalEntry::EReminder:
							{
							upload = iOptions.VEVENT;
							break;
							}
						case CCalEntry::ETodo:
							{
							upload = iOptions.VTODO;
							break;
							}
						}
					}

				}
			}
		if (upload)
			{
			CBufFlat* BufFlat = CBufFlat::NewL(EXPANDSIZE_SMALL);
			CleanupStack::PushL(BufFlat);
			RBufWriteStream writeStream(*BufFlat);
			CleanupClosePushL(writeStream);
			iCalExporter->ExportICalL(*aEntry, writeStream);
			writeStream.CommitL();
			CleanupStack::PopAndDestroy(&writeStream);

			HBufC8* buffer = BufFlat->Ptr(0).AllocL();
			CleanupStack::PopAndDestroy(BufFlat);
			CleanupStack::PushL(buffer);
			TPtr8 ptr = buffer->Des();
			CalDavUtils::FixExportIssues(ptr);

#ifdef _DEBUG
			ExportToFileL(aEntry, iCalExporter);
			_LIT(KFileName, "C:\\logs\\caldav\\testing_export_fixed.txt");
			ExportToFileNameL(ptr, KFileName);
#endif

			TBuf8<URLMAX> url;
			url.Append(*iUrl);
			url.Append(aEntry->UidL());
			url.Append(KIcs);

			CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
			CleanupStack::PushL(response);

#ifdef ETAG
			TPtrC8 etag = aEntry->GetETag();
			TBool newentry = (aChangeType == MCalChangeCallBack2::EChangeAdd)
			|| ((aChangeType == MCalChangeCallBack2::EChangeUndefined)
					&& (etag == KNullDesC8));
			TInt Ret = newentry ? iHttp->PutL(url, *buffer, response)
			: iHttp->PutL(url, *buffer, response, etag);
#else
			TBool newentry = (aChangeType == MCalChangeCallBack2::EChangeAdd)
					|| (aChangeType == MCalChangeCallBack2::EChangeUndefined);
			TInt Ret = newentry ? iHttp->PutL(url, *buffer, response)
					: iHttp->PutL(url, *buffer, response, _L8("ETAG"));
#endif
			if ((Ret == CREATED) || (Ret == NOCONTENT) || (Ret == OK))
				{
				if (newentry)
					{
					iLocalUidArray.Append(aEntry->LocalUidL());
					iGlobalUidArray.Append(aEntry->UidL());
					}
#ifdef ETAG
				aEntry->SetETag(iHttp->ETag());
#endif
				SetLastSyncTimeL();
				}
			else if (Ret == PRECONDFAILED)
				{
				if (newentry)// same filename already exists, use a different one and upload again
					{
					TBuf8<URLMAX> nextUrl;
					nextUrl.Append(*iUrl);
					nextUrl.Append(aEntry->UidL());
					TTime time;
					time.HomeTime();
					_LIT(KTimeFormat,"%H%T%S");
					TBuf<20> StringTime;
					time.FormatL(StringTime, KTimeFormat);
					nextUrl.Append(StringTime);
					nextUrl.Append(_L8(".ics"));
					response->Reset();
					TInt Ret = iHttp->PutL(nextUrl, *buffer, response);
					if ((Ret == CREATED) || (Ret == OK))
						{
						iLocalUidArray.Append(aEntry->LocalUidL());
						iGlobalUidArray.Append(aEntry->UidL());
#ifdef ETAG
						aEntry->SetETag(iHttp->ETag());
#endif
						SetLastSyncTimeL();
						}
					else
						{
						SyncFailedL();
						ret = KErrAbort;
						}
					}
				else
					{
					if (!iKeepServerEntry)
						{
						response->Reset();
						// upload again without ETAG to overwrite server entry
						TInt Ret = iHttp->PutL(url, *buffer, response);
						if ((Ret == CREATED) || (Ret == OK))
							{
#ifdef ETAG
							aEntry->SetETag(iHttp->ETag());
#endif
							SetLastSyncTimeL();
							}
						else
							{
							SyncFailedL();
							ret = KErrAbort;
							}
						}
					else
						{
						// download the server event and update local store
						ret = DownloadEntryL(url);
						if (ret == KErrNone)
							SetLastSyncTimeL();
						else
							{
							SyncFailedL();
							ret = KErrAbort;
							}
						}
					}
				}
			else
				{
				SyncFailedL();
				ret = KErrAbort;
				}
			CleanupStack::PopAndDestroy(response);
			CleanupStack::PopAndDestroy(buffer);
			}
		return ret;
		}
	return KErrArgument;
	}

/**
 * CCalDavEngine::UploadEntryL
 * upload entry to server
 */
TInt CCalDavEngine::UploadEntryL(const TCalLocalUid &aUid,
		MCalChangeCallBack2::TChangeType aChangeType,
		MCalChangeCallBack2::TChangeEntryType aEntryType)
	{
	CCalEntry * aEntry = iCalEntryView->FetchL(aUid);
	CleanupStack::PushL(aEntry);
	TInt ret = UploadEntryL(aEntry, aChangeType, aEntryType);
	CleanupStack::PopAndDestroy(aEntry);
	return ret;
	}

/**
 * CCalDavEngine::GetSyncTokenL
 * get latest Webdav Sync token
 */
HBufC8* CCalDavEngine::GetSyncTokenL()
	{
	HBufC8 *aBuf = HBufC8::NewL(KSync().Length());
	aBuf->Des().Format(KSync, &KNullDesC8());
	CleanupStack::PushL(aBuf);

	CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
	CleanupStack::PushL(response);
	TInt RetServer = iHttp->ReportL(*iUrl, *aBuf, response);

	if (RetServer)
		{
		RXmlEngDocument document = iDomParser.ParseL(response->Ptr(0));
		CleanupClosePushL(document);
		if (document.NotNull())
			{
			HBufC8* ret = SearchL(document, KSynctoken, KNullDesC8).AllocL();
			CleanupStack::PopAndDestroy(&document);
			CleanupStack::PopAndDestroy(response);
			CleanupStack::PopAndDestroy(aBuf);
			return ret;
			}
		CleanupStack::PopAndDestroy(&document);
		}
	CleanupStack::PopAndDestroy(response);
	CleanupStack::PopAndDestroy(aBuf);
	return KNullDesC8().AllocL();
	}

/**
 * CCalDavEngine::CheckCalendarInfoL
 * check for new calendar displayname and color
 */
void CCalDavEngine::CheckCalendarInfoL(RXmlEngDocument &aDocument)
	{
	TBool change = EFalse;
	CCalCalendarInfo* info = iCalSession->CalendarInfoL();
	CleanupStack::PushL(info);

	HBufC8* color = SearchL(aDocument, KCalendar_Color, KNullDesC8).AllocLC();
	if ((*color != KNullDesC8) && (color->Length() > 6))
		{
		TLex8 lexred(color->Des().Mid(1, 2));
		TInt red;
		lexred.Val(red);
		TLex8 lexgreen(color->Des().Mid(3, 2));
		TInt green;
		lexgreen.Val(green);
		TLex8 lexblue(color->Des().Mid(5, 2));
		TInt blue;
		lexblue.Val(blue);
		TRgb newcolor(red, green, blue);
		if (info->Color() != newcolor)
			{
			info->SetColor(newcolor);
			change = ETrue;
			}
		}
	CleanupStack::PopAndDestroy(color);

	HBufC8* displayname =
			SearchL(aDocument, KDisplayname, KNullDesC8).AllocLC();
	if (*displayname != KNullDesC8)
		{
		HBufC16* name =
				CnvUtfConverter::ConvertToUnicodeFromUtf8L(*displayname);
		CleanupStack::PushL(name);
		if (info->NameL() != *name)
			{
			info->SetNameL(*name);
			change = ETrue;
			}
		CleanupStack::PopAndDestroy(name);
		change = ETrue;
		}
	CleanupStack::PopAndDestroy(displayname);

	if (change)
		iCalSession->SetCalendarInfoL(*info);
	CleanupStack::PopAndDestroy(info);

	}

/**
 * CCalDavEngine::GetCTagL
 * get latest CTag
 * https://trac.calendarserver.org/browser/CalendarServer/trunk/doc/Extensions/caldav-ctag.txt
 */
HBufC8* CCalDavEngine::GetCTagL()
	{
	if (iHttp)
		{
		CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
		CleanupStack::PushL(response);
		TInt ret = iHttp->PropfindL(*iUrl, KCtag, response);

		if (ret == MULTISTATUS)
			{
			RXmlEngDocument document = iDomParser.ParseL(response->Ptr(0));
			CleanupClosePushL(document);
			if (document.NotNull())
				{
				HBufC8* status =
						SearchL(document, KStatus, KNullDesC8).AllocLC();
				HBufC8* ctag =
						SearchL(document, KGetctag, KNullDesC8).AllocLC();
				status->Des().LowerCase();
				if ((*ctag != KNullDesC8) && (*status == KHTTP200))
					{
					CleanupStack::Pop(ctag);
					CleanupStack::PopAndDestroy(status);
					CleanupStack::PopAndDestroy(&document);
					CleanupStack::PopAndDestroy(response);
					return ctag;
					}
				else
					CleanupStack::PopAndDestroy(ctag);
				CleanupStack::PopAndDestroy(status);
				}
			CleanupStack::PopAndDestroy(&document);

			}
		CleanupStack::PopAndDestroy(response);
		}
	return KNullDesC8().AllocL();
	}

/**
 * CCalDavEngine::GetOptionsL
 * get OPTIONS from server
 */
TBool CCalDavEngine::GetOptionsL()
	{
	if (iHttp)
		{
		// check DAV and allow headers
		iHttp->GetServerOptionsL(*iUrl, iOptions);

		// check ctag extension
		HBufC8* ctag = GetCTagL();
		if (*ctag != KNullDesC8)
			iOptions.sync_ctag = true;
		delete ctag;

		// check supported elements
		CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
		CleanupStack::PushL(response);
		TInt Ret = iHttp->PropfindL(*iUrl, KSupportedSet, response);

		if (Ret == MULTISTATUS)
			{
			RXmlEngDocument document = iDomParser.ParseL(response->Ptr(0));
			CleanupClosePushL(document);
			if (document.NotNull())
				{
				CheckCalendarInfoL(document);

				//<C:supported-calendar-component-set/>
				TXmlEngElement supportedelement;
				SearchL(document, KSupportedCalendarComponentSet, KNullDesC8,
						supportedelement);
				if (supportedelement.NotNull())
					{
					RXmlEngNodeList<TXmlEngElement> supportedelements;
					CleanupClosePushL(supportedelements);
					supportedelement.GetChildElements(supportedelements);
					while (supportedelements.HasNext())
						{
						TXmlEngElement element = supportedelements.Next();
						TPtrC8 value = element.AttributeValueL(KName);
						if (value == KNullDesC8)
							value.Set(element.AttributeValueL(KName, KCalDav));
						if (value == KVEVENT)
							iOptions.VEVENT = ETrue;
						else if (value == KVTODO)
							iOptions.VTODO = ETrue;
						else if (value == KVFREBUSY)
							iOptions.VFREEBUSY = ETrue;
						else if (value == KVJOURNAL)
							iOptions.VJOURNAL = ETrue;
						}
					CleanupStack::PopAndDestroy(&supportedelements);
					}
				}
			CleanupStack::PopAndDestroy(&document);
			}
		CleanupStack::PopAndDestroy(response);
		}
	return ETrue;
	}

/**
 * CCalDavEngine::SetSyncTokenL
 * set sync token
 */
void CCalDavEngine::SetSyncTokenL(HBufC8* aToken)
	{
	if (iSynctoken)
		{
		delete iSynctoken;
		iSynctoken = NULL;
		}
	iSynctoken = aToken;
	SetCalendarInfoL(KCaldavSynctoken, *iSynctoken);
	}

/**
 * CCalDavEngine::SyncToken
 * get synctoken
 */
TPtrC8 CCalDavEngine::SyncToken()
	{
	return iSynctoken ? *iSynctoken : KNullDesC8();
	}

/**
 * CCalDavEngine::SetCTagL
 * set ctag
 */
void CCalDavEngine::SetCTagL(HBufC8* aToken)
	{
	if (iCTag)
		{
		delete iCTag;
		iCTag = NULL;
		}
	iCTag = aToken;
	SetCalendarInfoL(KCaldavCtag, *iCTag);
	}

/**
 * CCalDavEngine::CTag
 * get ctag
 */
TPtrC8 CCalDavEngine::CTag()
	{
	return iCTag ? *iCTag : KNullDesC8();
	}

/**
 * CCalDavEngine::SetLastSyncTimeL
 * set last sync time
 */
void CCalDavEngine::SetLastSyncTimeL()
	{
	// only set a new last sync time, if we did not have a failed one before
	// otherwise, the old one would be lost
	if (!iManualSync)
		{
		TTime time;
		time.UniversalTime();
		iLastSyncTime.SetTimeUtcL(time);
		TPckgC<TCalTime> lasttime(iLastSyncTime);
		SetCalendarInfoL(KCaldavTime, lasttime);
		}
	}

/**
 * CCalDavEngine::SyncFailedL
 * sync failed, enable manual sync
 */
void CCalDavEngine::SyncFailedL()
	{
	if (!iManualSync)
		{
		iManualSync = ETrue;
		TPckgC<TBool> manualSync(iManualSync);
		SetCalendarInfoL(KCaldavManualSync, manualSync);
		}
	}

/**
 * CCalDavEngine::GetBaseUrl
 * get base domain url
 */
void CCalDavEngine::GetBaseUrl(const TDesC8 &aUrl)
	{
	_LIT8(http,"http://");
	_LIT8(https,"https://");

	if (iBaseUrl)
		{
		delete iBaseUrl;
		iBaseUrl = NULL;
		}

	if (aUrl.Length() > http().Length())
		{
		TInt length = aUrl.Find(https) != KErrNotFound ? https().Length()
				: http().Length();
		TInt pos = aUrl.Mid(length).Locate('/');
		iBaseUrl = aUrl.Left(pos + length).Alloc();
		}
	}

/**
 * CCalDavEngine::FindUrlsL
 * find home, inbox and outbox property
 */
void CCalDavEngine::FindUrlsL(const TDesC8 &aDes, HBufC8 *&home,
		HBufC8 *&inbox, HBufC8 *&outbox)
	{
	RXmlEngDocument document = iDomParser.ParseL(aDes);
	CleanupClosePushL(document);
	if (document.NotNull())
		{
		HBufC8* status = SearchL(document, KStatus, KNullDesC8).AllocLC();
		status->Des().LowerCase();
		if (*status == KHTTP200)
			{
			TXmlEngElement calendarhome, inboxhome, outboxhome;

			SearchL(document, KCalendarHomeSet, KNullDesC8, calendarhome);
			if (calendarhome.NotNull())
				{
				TPtrC8 homeend = SearchL(calendarhome, KHref, KNullDesC8);
				home = HBufC8::NewL(iBaseUrl->Length() + homeend.Length());
				home->Des().Append(*iBaseUrl);
				home->Des().Append(homeend);
				}

			SearchL(document, KInbox, KNullDesC8, inboxhome);
			if (inboxhome.NotNull())
				{
				TPtrC8 inboxend = SearchL(inboxhome, KHref, KNullDesC8);
				inbox = HBufC8::NewL(iBaseUrl->Length() + inboxend.Length());
				inbox->Des().Append(*iBaseUrl);
				inbox->Des().Append(inboxend);
				}

			SearchL(document, KOutbox, KNullDesC8, outboxhome);
			if (outboxhome.NotNull())
				{
				TPtrC8 outboxend = SearchL(outboxhome, KHref, KNullDesC8);
				outbox = HBufC8::NewL(iBaseUrl->Length() + outboxend.Length());
				outbox->Des().Append(*iBaseUrl);
				outbox->Des().Append(outboxend);
				}
			}
		CleanupStack::PopAndDestroy(status);
		}
	CleanupStack::PopAndDestroy(&document);
	}

/**
 * CCalDavEngine::FindCalendarCollectionL
 * find all calendar collections under home url
 */
HBufC8* CCalDavEngine::FindCalendarCollectionL(const TDesC8 &aUrl,
		CDesC8ArrayFlat *aArray)
	{
	HBufC8* homecalendar = 0;

	// do propfind depth:1 and find all calendar collections
	// right now, take the first one as default
	CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
	CleanupStack::PushL(response);
	TInt retcode = iHttp->PropfindL(aUrl, KCalendarurl, response, EFalse);
	if (retcode == MULTISTATUS)
		{
		RXmlEngDocument document = iDomParser.ParseL(response->Ptr(0));
		CleanupClosePushL(document);
		if (document.NotNull() && document.DocumentElement().NotNull())
			{
			RXmlEngNodeList<TXmlEngElement> ResponseList;
			CleanupClosePushL(ResponseList);
			document.DocumentElement().GetElementsByTagNameL(ResponseList,
					KResponse, KDav);
			TBool FirstOneDone = EFalse;
			while (ResponseList.HasNext())
				{
				TXmlEngElement node = ResponseList.Next();
				TPtrC8 href = SearchL(node, KHref, KNullDesC8);
				TPtrC8 status = SearchL(node, KStatus, KNullDesC8);

				TXmlEngElement calendar;
				TXmlEngElement vevent_collection;
				TXmlEngElement vtodo_collection;
				SearchL(node, KCalendar, KNullDesC8, calendar);
				SearchL(node, KVEventCollection, KNullDesC8, vevent_collection);
				SearchL(node, KVTodoCollection, KNullDesC8, vtodo_collection);

				if (calendar.NotNull() || vevent_collection.NotNull()
						|| vtodo_collection.NotNull())
					{
					if (!FirstOneDone)
						{
						homecalendar = HBufC8::NewL(iBaseUrl->Length()
								+ href.Length());
						homecalendar->Des().Append(*iBaseUrl);
						homecalendar->Des().Append(href);

						iOptions.VEVENT = vevent_collection.NotNull();
						iOptions.VTODO = vtodo_collection.NotNull();

						FirstOneDone = ETrue;
						}

					if (aArray)
						{
						TBuf8<URLMAX> url;
						url.Append(*iBaseUrl);
						url.Append(href);
						aArray->AppendL(url);
						}
					}
				}
			CleanupStack::PopAndDestroy(&ResponseList);
			}
		CleanupStack::PopAndDestroy(&document);
		}
	CleanupStack::PopAndDestroy(response);
	return homecalendar;
	}

/**
 * CCalDavEngine::GetCalendarUrlsL
 * find calendar url based on any url
 * could be principal url, home or direct calendar url
 */
TInt CCalDavEngine::GetCalendarUrlsL(CDesC8ArrayFlat *aArray)
	{
	if (iHttp && iUrl)
		{
		HBufC8 *principal = 0;
		HBufC8 *home = 0;
		HBufC8 *homecalendar = 0;
		HBufC8 *inbox = 0;
		HBufC8 *outbox = 0;

		GetBaseUrl(*iUrl);

		// TODO: does this really find groupdav collection?

		// find out if this is a caldav or groupdav calendar collection
		CBufFlat* response = CBufFlat::NewL(EXPANDSIZE_SMALL);
		CleanupStack::PushL(response);
		TInt retcode = iHttp->PropfindL(*iUrl, KCalendarurl, response);
		if (retcode == MULTISTATUS)
			{
			RXmlEngDocument document = iDomParser.ParseL(response->Ptr(0));
			CleanupClosePushL(document);
			if (document.NotNull())
				{
				HBufC8* status =
						SearchL(document, KStatus, KNullDesC8).AllocLC();
				status->Des().LowerCase();
				TXmlEngElement calendar;
				SearchL(document, KCalendar, KNullDesC8, calendar);
				// it should be <owner><href>value</href></owner>
				// but oracle beehive server does <owner>value</owner>
				TXmlEngElement owner;
				SearchL(document, KOwner, KNullDesC8, owner);
				TPtrC8 ownerref1 = SearchL(owner, KHref, KNullDesC8);
				TPtrC8 ownerref2 = owner.Value();
				TPtrC8 ownerref;
				ownerref.Set(ownerref1 != KNullDesC8 ? ownerref1 : ownerref2);

				if (calendar.NotNull() && (ownerref != KNullDesC8) && (*status
						== KHTTP200))
					{
					// this is a calendar collection and we know the principal as well now
					homecalendar = iUrl->AllocL();
					_LIT8(KHTTP,"http");
					HBufC8* http = ownerref.Left(KHTTP().Length()).AllocLC();
					if (*http == KHTTP)
						{
						// sogo server does not return relative, but principal url
						principal = ownerref.AllocL();
						}
					else
						{
						principal = HBufC8::NewL(iBaseUrl->Length()
								+ ownerref.Length());
						principal->Des().Append(*iBaseUrl);
						principal->Des().Append(ownerref);
						}
					CleanupStack::PopAndDestroy(http);
					}
				CleanupStack::PopAndDestroy(status);
				}
			CleanupStack::PopAndDestroy(&document);
			}

		// if we have principal, ask for home, otherwise see if principal was given in the first place
		if (principal)
			{
			response->Reset();
			TInt retcode =
					iHttp->PropfindL(*principal, KPrincipalurl, response);
			if (retcode == MULTISTATUS)
				{
				FindUrlsL(response->Ptr(0), home, inbox, outbox);
				}
			}
		else
			{
			response->Reset();
			TInt retcode = iHttp->PropfindL(*iUrl, KPrincipalurl, response);
			if (retcode == MULTISTATUS)
				{
				FindUrlsL(response->Ptr(0), home, inbox, outbox);
				}
			}

		home = CalDavUtils::EnsureSlashL(home);
		inbox = CalDavUtils::EnsureSlashL(inbox);
		outbox = CalDavUtils::EnsureSlashL(outbox);

		// find out all calendar collections under home
		if (home)
			{
			// TODO: temporary? we already have homecalendar...
			if (!homecalendar)
				{
				homecalendar = FindCalendarCollectionL(*home, aArray);
				}
			}
		else
			{
			// have not found out home nor a groupdav collection, maybe we are home ourselves
			homecalendar = FindCalendarCollectionL(*iUrl, aArray);
			if (homecalendar)
				home = iUrl->AllocL();
			}

		CleanupStack::PopAndDestroy(response);
		delete principal;
		delete inbox;
		delete outbox;

		if (home)
			iHome = CalDavUtils::EnsureSlashL(home);
		if (homecalendar)
			{
			delete iUrl;
			iUrl = NULL;
			iUrl = CalDavUtils::EnsureSlashL(homecalendar);
			return KErrNone;
			}
		else
			return KErrArgument;
		}
	return KErrArgument;
	}

/**
 * CCalDavEngine::CalendarName
 * get calendar name
 */
TPtrC CCalDavEngine::CalendarName() const
	{
	return iCalendar ? *iCalendar : KNullDesC();
	}

/**
 * CCalDavEngine::Home
 * get home
 */
TPtrC8 CCalDavEngine::Home() const
	{
	return iHome ? *iHome : KNullDesC8();
	}

/**
 * CCalDavEngine::Url
 * get url
 */
TPtrC8 CCalDavEngine::Url() const
	{
	return iUrl ? *iUrl : KNullDesC8();
	}

/**
 * CCalDavEngine::SetUrlL
 * set url
 */
void CCalDavEngine::SetUrlL(const TDesC8 &aUrl)
	{
	if (iUrl ? *iUrl != aUrl : ETrue)
		{
		DisableL();

		if (iUrl)
			{
			delete iUrl;
			iUrl = NULL;
			}

		iUrl = CalDavUtils::EnsureSlashL(aUrl);
		SetCalendarInfoL(KCaldavUrl, *iUrl);
		}
	}

/**
 * CCalDavEngine::User
 * get user
 */
TPtrC8 CCalDavEngine::User() const
	{
	return iHttp->User();
	}

/**
 * CCalDavEngine::SetUserL
 * set user
 */
void CCalDavEngine::SetUserL(const TDesC8 &aUser)
	{
	if (iHttp->User() != aUser)
		{
		DisableL();
		SetCalendarInfoL(KCaldavUser, aUser);
		iHttp->SetUserL(aUser);
		}
	}

/**
 * CCalDavEngine::Password
 * get password
 */
TPtrC8 CCalDavEngine::Password() const
	{
	return iHttp->Password();
	}

/**
 * CCalDavEngine::SetPasswordL
 * set password
 */
void CCalDavEngine::SetPasswordL(const TDesC8 &aPassword)
	{
	if (iHttp->Password() != aPassword)
		{
		DisableL();
		iHttp->SetPasswordL(aPassword);
		SetCalendarInfoL(KCaldavPassword, aPassword);
		}
	}

/**
 * CCalDavEngine::SyncInterval
 * get SyncInterval
 */
TTimeIntervalMinutes CCalDavEngine::SyncInterval() const
	{
	return iSyncInterval;
	}

/**
 * CCalDavEngine::SetSyncIntervalL
 * set SetSyncIntervalL
 */
void CCalDavEngine::SetSyncIntervalL(TTimeIntervalMinutes aSyncInterval)
	{
	iSyncInterval = aSyncInterval;
	TPckgC<TTimeIntervalMinutes> minutes(iSyncInterval);
	SetCalendarInfoL(KCaldavSyncInterval, minutes);
	}

/**
 * CCalDavEngine::PastDays
 * get past days
 */
TTimeIntervalDays CCalDavEngine::PastDays() const
	{
	return iPastDays;
	}

/**
 * CCalDavEngine::SetPastDaysL
 * Set PastDaysL
 */
void CCalDavEngine::SetPastDaysL(TTimeIntervalDays aDays)
	{
	iPastDays = aDays;
	TPckgC<TTimeIntervalDays> days(iPastDays);
	SetCalendarInfoL(KCaldavPastDays, days);
	}

/**
 * CCalDavEngine::ImmediateSync
 * get ImmediateSyncL
 */
TBool CCalDavEngine::ImmediateSync() const
	{
	return iImmediateSync;
	}

/**
 * CCalDavEngine::SetImmediateSyncL
 * Set ImmediateSyncL
 */
void CCalDavEngine::SetImmediateSyncL(TBool aImmediateSyc)
	{
	iImmediateSync = aImmediateSyc;
	TPckgC<TBool> immediatesync(iImmediateSync);
	SetCalendarInfoL(KCaldavImmediateSync, immediatesync);
	}

/**
 * CCalDavEngine::KeepServerEntry
 * get KeepServerEntryL
 */
TBool CCalDavEngine::KeepServerEntry() const
	{
	return iKeepServerEntry;
	}

/**
 * CCalDavEngine::SetKeepServerEntryL
 * Set KeepServerEntryL
 */
void CCalDavEngine::SetKeepServerEntryL(TBool aKeepServerEntry)
	{
	iKeepServerEntry = aKeepServerEntry;
	TPckgC<TBool> keepserver(iKeepServerEntry);
	SetCalendarInfoL(KCaldavKeepServer, keepserver);
	}

/**
 * CCalDavEngine::Timer
 * get timer
 */
CPeriodic* CCalDavEngine::Timer()
	{
	return iTimer;
	}
