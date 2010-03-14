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
* Description: provides a manager to keep track of all available engines
*
*/

#include "caldavenginemgr.h"
#include "caldavengine.h"

#include <calsession.h>
#include <calcalendarinfo.h>
#include "calenglobaldata.h"

/**
 * CCalDavEngineMgr::CCalDavEngineMgr()
 * default constructor
 */
CCalDavEngineMgr::CCalDavEngineMgr()
	{
	}

/**
 * CCalDavEngineMgr::~CCalDavEngineMgr()
 * default destructor
 */
CCalDavEngineMgr::~CCalDavEngineMgr()
	{
	for (TInt i = iEngines.Count() - 1; i >= 0; --i)
		delete iEngines[i];
	iEngines.Close();
	}

/**
 * CCalDavEngineMgr::NewLC()
 * first phase construction
 */
CCalDavEngineMgr* CCalDavEngineMgr::NewLC()
	{
	CCalDavEngineMgr* self = new (ELeave) CCalDavEngineMgr();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
 * CCalDavEngineMgr::NewL()
 * first phase construction
 */
CCalDavEngineMgr* CCalDavEngineMgr::NewL()
	{
	CCalDavEngineMgr* self = CCalDavEngineMgr::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

/**
 * CCalDavEngineMgr::Progress()
 * progress callback
 */
void CCalDavEngineMgr::Progress(TInt /*aPercentageCompleted*/)
	{
	}

/**
 * CCalDavEngineMgr::Completed()
 * completed callback
 */
void CCalDavEngineMgr::Completed(TInt /*aError*/)
	{
	}

/**
 * CCalDavEngineMgr::NotifyProgress()
 * NotifyProgress callback
 */
TBool CCalDavEngineMgr::NotifyProgress()
	{
	return EFalse;
	}

/**
 * CCalDavEngineMgr::ConstructL()
 * second phase construction
 * check all calendar files, for each one which has caldav sync enabled, 
 * create a CCalDavengine and enable sync
 */
void CCalDavEngineMgr::ConstructL()
	{
	RPointerArray<CCalCalendarInfo> calendarInfoList;
	CCalenGlobalData* data = CCalenGlobalData::NewL(*this);
	data->InitializeGlobalDataL();
	CleanupStack::PushL(data);
	data->GetAllCalendarInfoL(calendarInfoList);
	CleanupClosePushL(calendarInfoList);

	for (TInt i = 0; i < calendarInfoList.Count(); i++)
		{
		CDesC8Array* propertyKeys = calendarInfoList[i]->PropertyKeysL();
		CleanupStack::PushL(propertyKeys);
		TInt pos = 0;
		if (propertyKeys->Find(KCaldavEnabled, pos, ECmpNormal) == KErrNone)
			{
			CCalDavEngine* engine = CCalDavEngine::NewL(
					calendarInfoList[i]->FileNameL());
			iEngines.Append(engine);
			if (engine->EnabledSync())
				engine->EnableL();
			}
		CleanupStack::PopAndDestroy(propertyKeys);
		}
	CleanupStack::PopAndDestroy(&calendarInfoList);
	CleanupStack::PopAndDestroy(data);
	}

/**
 * CCalDavEngineMgr::FindEngineL()
 * find engines inside engine array, create if neccessary
 */
TInt CCalDavEngineMgr::FindEngineL(const TDesC &aName, TBool aCreate)
	{
	CCalSession* session = CCalSession::NewL();
	TRAPD(error,session->OpenL(aName));
	delete session;
	if (error != KErrNone)
		return KErrNotFound;
		
	for (TInt i = 0; i <= iEngines.Count() - 1; i++)
		{
		if (iEngines[i]->CalendarName() == aName)
			return i;
		}

	if (aCreate)
		{
		iEngines.Append(CCalDavEngine::NewL(aName));
		return iEngines.Count() - 1;
		}
	else
		return KErrNotFound;
	}

/**
 * CCalDavEngineMgr::SyncL()
 * sync specific calendar
 */
TInt CCalDavEngineMgr::SyncL(const TDesC &aCalendar)
	{
	TInt pos = FindEngineL(aCalendar);
	if (pos != KErrNotFound)
		return iEngines[pos]->SyncL();
	else
		return KErrNotFound;
	}

/**
 * CCalDavEngineMgr::SyncAllL()
 * sync all calendars
 */
TInt CCalDavEngineMgr::SyncAllL()
	{
	// sync all caldav enabled calendars
	for (TInt i = 0; i <= iEngines.Count() - 1; i++)
		iEngines[i]->SyncL();
	return KErrNone;
	}

/**
 * CCalDavEngineMgr::DisableL()
 * 
 */
TInt CCalDavEngineMgr::DisableL(const TDesC &aCalendar)
	{
	TInt pos = FindEngineL(aCalendar);

	if (pos != KErrNotFound)
		iEngines[pos]->DisableL();

	return pos == KErrNotFound ? KErrNotFound : KErrNone;
	}

/**
 * CCalDavEngineMgr::EnableL()
 * 
 */
TInt CCalDavEngineMgr::EnableL(const TDesC &aCalendar)
	{
	TInt pos = FindEngineL(aCalendar);
	if (pos != KErrNotFound)
		{
		return iEngines[pos]->EnableL();
		}

	return KErrArgument;
	}

/**
 * CCalDavEngineMgr::UrlL()
 * 
 */
TPtrC8 CCalDavEngineMgr::UrlL(const TDesC &aCalendar)
	{
	TInt pos = FindEngineL(aCalendar);
	return (pos != KErrNotFound) ? iEngines[pos]->Url() : KNullDesC8();
	}

/**
 * CCalDavEngineMgr::SetUrlL()
 * 
 */
void CCalDavEngineMgr::SetUrlL(const TDesC &aCalendar, const TDesC8 &aUrl)
	{
	TInt pos = FindEngineL(aCalendar, ETrue);
	if (pos != KErrNotFound)
		iEngines[pos]->SetUrlL(aUrl);
	}

/**
 * CCalDavEngineMgr::UsernameL()
 * 
 */
TPtrC8 CCalDavEngineMgr::UsernameL(const TDesC &aCalendar)
	{
	TInt pos = FindEngineL(aCalendar);
	return (pos != KErrNotFound) ? iEngines[pos]->User() : KNullDesC8();
	}

/**
 * CCalDavEngineMgr::SetUsernameL()
 * 
 */
void CCalDavEngineMgr::SetUsernameL(const TDesC &aCalendar,
		const TDesC8 &aUsername)
	{
	TInt pos = FindEngineL(aCalendar, ETrue);
	if (pos != KErrNotFound)
		iEngines[pos]->SetUserL(aUsername);
	}

/**
 * CCalDavEngineMgr::PasswordL()
 * 
 */
TPtrC8 CCalDavEngineMgr::PasswordL(const TDesC &aCalendar)
	{
	TInt pos = FindEngineL(aCalendar);
	return (pos != KErrNotFound) ? iEngines[pos]->Password() : KNullDesC8();
	}

/**
 * CCalDavEngineMgr::SetPasswordL()
 * 
 */
void CCalDavEngineMgr::SetPasswordL(const TDesC &aCalendar,
		const TDesC8 &aPassword)
	{
	TInt pos = FindEngineL(aCalendar, ETrue);
	if (pos != KErrNotFound)
		iEngines[pos]->SetPasswordL(aPassword);
	}

/**
 * CCalDavEngineMgr::SyncIntervalL()
 * 
 */
TTimeIntervalMinutes CCalDavEngineMgr::SyncIntervalL(const TDesC &aCalendar)
	{
	TInt pos = FindEngineL(aCalendar);
	return (pos != KErrNotFound) ? iEngines[pos]->SyncInterval()
			: TTimeIntervalMinutes(DEFAULT_SYNC_MINUTES);
	}

/**
 * CCalDavEngineMgr::SetSyncIntervalL()
 * 
 */
void CCalDavEngineMgr::SetSyncIntervalL(const TDesC &aCalendar,
		TTimeIntervalMinutes aSyncInterval)
	{
	TInt pos = FindEngineL(aCalendar, ETrue);
	if (pos != KErrNotFound)
		iEngines[pos]->SetSyncIntervalL(aSyncInterval);
	}

/**
 * CCalDavEngineMgr::PastDaysL()
 * 
 */
TTimeIntervalDays CCalDavEngineMgr::PastDaysL(const TDesC &aCalendar)
	{
	TInt pos = FindEngineL(aCalendar);
	return (pos != KErrNotFound) ? iEngines[pos]->PastDays()
			: TTimeIntervalDays(DEFAULT_PAST_DAYS);
	}

/**
 * CCalDavEngineMgr::SetPastDaysL()
 * 
 */
void CCalDavEngineMgr::SetPastDaysL(const TDesC &aCalendar,
		TTimeIntervalDays aDays)
	{
	TInt pos = FindEngineL(aCalendar, ETrue);
	if (pos != KErrNotFound)
		iEngines[pos]->SetPastDaysL(aDays);
	}

/**
 * CCalDavEngineMgr::ImmediateSyncL()
 * 
 */
TBool CCalDavEngineMgr::ImmediateSyncL(const TDesC &aCalendar)
	{
	TInt pos = FindEngineL(aCalendar);
	return pos != KErrNotFound ? iEngines[pos]->ImmediateSync()
			: DEFAULT_IMMEDIATE_SYNC;
	}

/**
 * CCalDavEngineMgr::SetImmediateSyncL()
 * 
 */
void CCalDavEngineMgr::SetImmediateSyncL(const TDesC &aCalendar,
		TBool aImmediateSyc)
	{
	TInt pos = FindEngineL(aCalendar, ETrue);
	if (pos != KErrNotFound)
		iEngines[pos]->SetImmediateSyncL(aImmediateSyc);
	}

/**
 * CCalDavEngineMgr::KeepServerEntryL()
 * 
 */
TBool CCalDavEngineMgr::KeepServerEntryL(const TDesC &aCalendar)
	{
	TInt pos = FindEngineL(aCalendar);
	return pos != KErrNotFound ? iEngines[pos]->KeepServerEntry()
			: DEFAULT_KEEP_SERVER_ENTRY;
	}

/**
 * CCalDavEngineMgr::SetKeepServerEntryL()
 * 
 */
void CCalDavEngineMgr::SetKeepServerEntryL(const TDesC &aCalendar,
		TBool aKeepServerEntry)
	{
	TInt pos = FindEngineL(aCalendar, ETrue);
	if (pos != KErrNotFound)
		iEngines[pos]->SetKeepServerEntryL(aKeepServerEntry);
	}

/**
 * CCalDavEngineMgr::EnabledSyncL()
 * 
 */
TBool CCalDavEngineMgr::EnabledSyncL(const TDesC &aCalendar)
	{
	TInt pos = FindEngineL(aCalendar);
	return (pos != KErrNotFound) ? iEngines[pos]->EnabledSync() : EFalse;
	}
