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
* Description: main Caldav class, all magic happens here
*
*/

#ifndef _CALSTORE_H
#define _CALSTORE_H

#include <e32base.h>
#include <BADESCA.H>

#include <calchangecallback.h>
#include <calprogresscallback.h>
#include <calfilechangenotification.h>

#include <xmlengdomimplementation.h>
#include <xmlengdomparser.h>

#include "caldavutils.h"

class CCalSession;
class CCalEntry;
class CCalIter;
class CCalEntryView;
class CCalenExporter;
class CCalenImporter;
class CCalenInterimUtils2;

class RXmlEngDOMImplementation;
class RXmlEngDOMParser;
class RXmlEngDocument;

class CHttpClient;
class CalDavTest;

// sync interval of 0 correspons to one minute
// 0 actually means to enable push connectivity once supported
#define DEFAULT_SYNC_MINUTES 		0
#define DEFAULT_PAST_DAYS			14
#define DEFAULT_IMMEDIATE_SYNC		ETrue
#define DEFAULT_KEEP_SERVER_ENTRY	EFalse


enum TLocalLoopAction
	{
		ELoopActionUpload, ELoopActionFillArray, ELoopActionDeleteLocal
	};

/**
 *  CCalDavEngine
 * 
 */
class CCalDavEngine : public CBase, MCalProgressCallBack, MCalChangeCallBack2, MCalFileChangeObserver
	{
public:

	friend class CalDavTest;

	~CCalDavEngine();

	static CCalDavEngine* NewL(const TDesC& aCalendar);
	static CCalDavEngine* NewLC(const TDesC& aCalendar);

	TInt EnableL();
	void DisableL();
	TBool EnabledSync();

	TInt SyncL();

	TInt MkcalendarL(const TDesC8 &aName);
	TInt DeleteCalendarL(const TDesC8 &aName);

	TPtrC CalendarName() const;
	TPtrC8 Home() const;

	TPtrC8 Url() const;
	void SetUrlL(const TDesC8 &aUrl);

	TPtrC8 User() const;
	void SetUserL(const TDesC8 &aUser);

	TPtrC8 Password() const;
	void SetPasswordL(const TDesC8 &aPassword);

	TTimeIntervalMinutes SyncInterval() const;
	void SetSyncIntervalL(TTimeIntervalMinutes aSyncInterval);

	TTimeIntervalDays PastDays() const;
	void SetPastDaysL(TTimeIntervalDays aDays);

	TBool ImmediateSync() const;
	void SetImmediateSyncL(TBool aImmediateSyc);

	TBool KeepServerEntry() const;
	void SetKeepServerEntryL(TBool aKeepServerEntry);

	CPeriodic* Timer();

	TPtrC8 SyncToken();
	TPtrC8 CTag();

private:

	CCalDavEngine();
	void ConstructL(const TDesC& aCalendar);

	void RegisterL();
	HBufC8* GetCTagL();
	HBufC8* GetSyncTokenL();

	TInt InitL();

	// Called during calendar entry view creation
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress();
	void CalChangeNotification(RArray<TCalChangeEntry> &aChangeItems);
	void CalendarInfoChangeNotificationL(RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries);

	void CalendarInfoL();
	void SetCalendarInfoL(const TDesC8 &aKey, const TDesC8 &aValue);

	void HandleCalendarInfoChangeL();
	void HandleChangeL(MCalChangeCallBack2::TChangeType &aChangeType,
			MCalChangeCallBack2::TChangeEntryType &aEntryType,
			TCalLocalUid &aUid);

	TBool ManualSyncL();
	TBool WebDavSyncL();
	TInt WebDavSyncReportL(TBool aSynctoken);
	TBool UploadModifiedSinceDateL();
	TBool ClientChangesL();
	TBool ServerChangesL();
	TInt DeleteRemovedEntriesOnServerL();
	TInt LocalLoopL(TLocalLoopAction aAction);
	
	TInt ListL();
	TInt TimeReportL(TBool VEVENT, const TDesC8 &aStart, TBool aDelete = EFalse);

	// server actions
	TInt UploadEntryL(const TCalLocalUid &aUid,
			MCalChangeCallBack2::TChangeType aChangeType,
			MCalChangeCallBack2::TChangeEntryType aEntryType);
	TInt UploadEntryL(CCalEntry* aEntry,
			MCalChangeCallBack2::TChangeType aChangeType,
			MCalChangeCallBack2::TChangeEntryType aEntryType);
	TInt DownloadEntryL(const TDesC8 &aUrl);
	TInt DownloadEntryL(CDesC8Array* aArray);
	TInt DeleteEntryL(const TCalLocalUid &aUid);
	TInt DeleteEntryL(const TDesC8 &aUid);
	TInt HeadL(const TDesC8 &aUID);

	// local actions
	TInt AddModifyLocalEntryL(const TDesC8 &aUrl, const TDesC8 &aETag,
			CDesC8ArrayFlat* aArray);
	TInt DeleteLocalEntryL(const TDesC8 &aUID);

	TInt ParseResponsesDeleteL(const TDesC8 &aDocument);
	TInt ParseResponsesL(RXmlEngDocument &aDocument, TBool aMultiget = EFalse);
	TInt ParseResponsesL(const TDesC8 &aDocument, TBool aMultiget = EFalse);
	
	unsigned long DoesEntryExistL(const TDesC8 &aUrl);
	TPtrC8 GetUIDByUrl(const TDesC8 &aUrl);
	TBool ETagMatchL(const TDesC8& aUrl, const TDesC8& aETag);
	TInt StoreEntryL(const TDesC8 &aBuf, const TDesC8 &aEtag);

	void SyncFailedL();
	void SetLastSyncTimeL();
	void SetSyncTokenL(HBufC8* aToken);
	void SetCTagL(HBufC8* aToken);

	void CheckCalendarInfoL(RXmlEngDocument &aDocument);
	
	TBool GetOptionsL();
	TInt GetCalendarUrlsL(CDesC8ArrayFlat *aArray);
	void FindUrlsL(const TDesC8 &aDes, HBufC8 *&home, HBufC8 *&inbox,
			HBufC8 *&outbox);
	void GetBaseUrl(const TDesC8 &aUrl);
	HBufC8 * FindCalendarCollectionL(const TDesC8 &aUrl,
			CDesC8ArrayFlat *aArray);
	
	void DeleteCalObjects();
	TInt CreateCalObjectsL();
	
	CCalSession* iCalSession;
	CCalIter* iCalIter;
	CCalEntryView* iCalEntryView;
	CCalenExporter* iCalExporter;
	CCalenImporter* iCalImporter;
	CCalenInterimUtils2* iCalIntermimUtils2;
	HBufC *iCalendar;
	HBufC8 *iUrl;
	HBufC8 *iBaseUrl;
	HBufC8 *iHome;

	TCalDAVOptions iOptions;
	CHttpClient* iHttp;

	RXmlEngDOMImplementation iDomImpl;
	RXmlEngDOMParser iDomParser;

	// when we get a callback with a localuid, the corresponding entry is already deleted
	// we therefore need a different way to map from localuid to global uid/filename
	// this can be completly removed when introducing a new callback,
	// which includes the to be deleted localuid
	RArray<TCalLocalUid> iLocalUidArray;
	RArray<TBuf8<100> > iGlobalUidArray;
	//
	RArray<TCalLocalUid> iDeletedEntries;

	HBufC8 *iSynctoken;
	HBufC8 *iCTag;
	TBool iManualSync;
	TCalTime iLastSyncTime;
	TBool iFirstInit;

	// Options
	TTimeIntervalMinutes iSyncInterval;
	TTimeIntervalDays iPastDays;
	TBool iImmediateSync;
	TBool iKeepServerEntry;
	TBool iEnabled;

	CPeriodic* iTimer;
	};

#endif // CALSTORE_H
