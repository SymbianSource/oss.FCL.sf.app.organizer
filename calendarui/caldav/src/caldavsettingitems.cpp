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
* Description: Caldav setting items
*
*/

#include "caldavsettingitems.h"

#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <cmapplicationsettingsui.h>
#include <CommDbConnPref.h>
#include <UTF.H>

#include <caldavsession.h>
#include "calencaldavplugin.h"
#include <calencaldavplugindata.rsg>

#define KMAXSETTINGLENGTH	500

#define KTIMETWOWEEKS 	14
#define KTIMETHREEMONTH	30*3
#define KTIMESIXMONTH	30*6
#define KTIMEALL		1000

#define KTIMEALWAYSON		0
#define	KTIMEONEMINUTE		1
#define	KTIMEFIVEMINUTES	5
#define	KTIMEFIFTEENMINUTES	15
#define KTIMETHIRTYMINUTES	30

/**
 * show a global info note
 */
void ShowNoteL(TInt aResource)
	{
	HBufC* text = StringLoader::LoadLC(aResource);
	CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote();
	note->ExecuteLD(*text);
	CleanupStack::PopAndDestroy(text);
	}

/**
 * CCalDavTextSettingItem::CCalDavTextSettingItem 
 * default constructor
 */
CCalDavTextSettingItem::CCalDavTextSettingItem(TInt aId,
		CCalDavSession* aSession) :
	CAknTextSettingItem(aId, iInternalValue), iSession(aSession)
	{

	}

/**
 * CCalDavTextSettingItem::~CCalDavTextSettingItem 
 * destructor
 */
CCalDavTextSettingItem::~CCalDavTextSettingItem()
	{
	delete iCalendar;
	}

/**
 * CCalDavTextSettingItem::NewLC 
 * 1st-phase construction
 */
CCalDavTextSettingItem* CCalDavTextSettingItem::NewLC(TInt aId,
		CCalDavSession* aSession, const TDesC &aCalendar)
	{
	CCalDavTextSettingItem* self = new (ELeave) CCalDavTextSettingItem(aId,
			aSession);
	CleanupStack::PushL(self);
	self->ConstructL(aCalendar);
	return self;
	}

/**
 * CCalDavTextSettingItem::NewL
 *  1st-phase construction
 */
CCalDavTextSettingItem* CCalDavTextSettingItem::NewL(TInt aId,
		CCalDavSession* aSession, const TDesC &aCalendar)
	{
	CCalDavTextSettingItem* self = CCalDavTextSettingItem::NewLC(aId, aSession,
			aCalendar);
	CleanupStack::Pop(); // self;
	return self;
	}

/**
 * CCalDavTextSettingItem::ConstructL
 * 2nd-phase construction
 */
void CCalDavTextSettingItem::ConstructL(const TDesC &aCalendar)
	{
	iCalendar = aCalendar.AllocL();
	HBufC* title = NULL;
	TInt page = 0;
	switch (Identifier())
		{
		case ECalDavSettingUrl:
			title = StringLoader::LoadLC(R_CALDAV_URL);
			page = R_URL_SETTING_PAGE;
			break;
		case ECalDavSettingUser:
			title = StringLoader::LoadLC(R_CALDAV_USERNAME);
			page = R_USERNAME_SETTING_PAGE;
			break;
		case ECalDavSettingPassword:
			title = StringLoader::LoadLC(R_CALDAV_PASSWORD);
			page = R_PASSWORD_SETTING_PAGE;
			break;
		}
	SetEmptyItemTextL(KNullDesC);
	CAknTextSettingItem::ConstructL(EFalse, 0, *title, NULL,page, -1);
	CleanupStack::PopAndDestroy(title);
	}

/**
 * CCalDavTextSettingItem::EditItemL
 * Edits the item
 */
void CCalDavTextSettingItem::EditItemL(TBool aCalledFromMenu)
	{
	TBool enabled;
	iSession->EnabledSync(*iCalendar, enabled);
	if (enabled)
		{
		HBufC* text = StringLoader::LoadLC(R_CALDAV_QUERY_DISCONNECT);
		CAknQueryDialog* dlg = CAknQueryDialog::NewL();
		if (dlg->ExecuteLD(R_QUERY, *text))
			{
			CAknTextSettingItem::EditItemL(aCalledFromMenu);
			// TODO: disable Enabled-Setting-Item	
			}
		CleanupStack::PopAndDestroy(text);
		}
	else
		CAknTextSettingItem::EditItemL(aCalledFromMenu);
	}

/**
 * CCalDavTextSettingItem::StoreL
 * Stores the item
 */
void CCalDavTextSettingItem::StoreL()
	{
	CAknTextSettingItem::StoreL();

	HBufC8* text = CnvUtfConverter::ConvertFromUnicodeToUtf8L(iInternalValue);
	CleanupStack::PushL(text);
	
	switch (Identifier())
		{
		case ECalDavSettingUrl:
			iSession->SetUrl(*iCalendar, *text);
			break;
		case ECalDavSettingUser:
			iSession->SetUsername(*iCalendar, *text);
			break;
		case ECalDavSettingPassword:
			iSession->SetPassword(*iCalendar, *text);
			break;
		}
	CleanupStack::PopAndDestroy(text);
	
	if (iEnableSettingItem)
		{
		iEnableSettingItem->LoadL();
		iEnableSettingItem->UpdateListBoxTextL();
		}
	}

/**
 * CCalDavTextSettingItem::LoadL
 * loades the item
 */
void CCalDavTextSettingItem::LoadL()
	{
	HBufC8* buf = HBufC8::NewL(KMAXSETTINGLENGTH);
	TPtr8 ptr(buf->Des());
	switch (Identifier())
		{
		case ECalDavSettingUrl:
			iSession->Url(*iCalendar, ptr);
			break;
		case ECalDavSettingUser:
			iSession->Username(*iCalendar, ptr);
			break;
		case ECalDavSettingPassword:
			iSession->Password(*iCalendar, ptr);
			break;
		}
	CnvUtfConverter::ConvertToUnicodeFromUtf8(iInternalValue, ptr);
	delete buf;
	CAknTextSettingItem::LoadL();
	}

/**
 * CCalDavTextSettingItem::StoreL
 * Save a pointer to the Enable Item, this is used to set current
 * enable/disable state
 */
void CCalDavTextSettingItem::SetEnableSettingItem(CAknSettingItem* aItem)
	{
	iEnableSettingItem = aItem;
	}

/**
 * CCalDavPasswordSettingItem::CCalDavPasswordSettingItem
 * default constructor
 */
CCalDavPasswordSettingItem::CCalDavPasswordSettingItem(TInt aId,
		CCalDavSession* aSession) :
	CAknPasswordSettingItem(aId, CAknPasswordSettingItem::EAlpha,
			iInternalValue), iSession(aSession)
	{

	}

/**
 * CCalDavPasswordSettingItem::~CCalDavPasswordSettingItem
 * default destructor
 */
CCalDavPasswordSettingItem::~CCalDavPasswordSettingItem()
	{
	delete iCalendar;
	}

/**
 * CCalDavPasswordSettingItem::NewLC
 * first phase construction
 */
CCalDavPasswordSettingItem* CCalDavPasswordSettingItem::NewLC(TInt aId,
		CCalDavSession* aSession, const TDesC &aCalendar)
	{
	CCalDavPasswordSettingItem* self = new (ELeave) CCalDavPasswordSettingItem(
			aId, aSession);
	CleanupStack::PushL(self);
	self->ConstructL(aCalendar);
	return self;
	}

/**
 * CCalDavPasswordSettingItem::NewL
 * first phase construction
 */
CCalDavPasswordSettingItem* CCalDavPasswordSettingItem::NewL(TInt aId,
		CCalDavSession* aSession, const TDesC &aCalendar)
	{
	CCalDavPasswordSettingItem* self = CCalDavPasswordSettingItem::NewLC(aId,
			aSession, aCalendar);
	CleanupStack::Pop(); // self;
	return self;
	}

/**
 * CCalDavPasswordSettingItem::SetEnableSettingItem
 * Save a pointer to the Enable Item, this is used to set current
 * enable/disable state
 */
void CCalDavPasswordSettingItem::SetEnableSettingItem(CAknSettingItem* aItem)
	{
	iEnableSettingItem = aItem;
	}

/**
 * CCalDavPasswordSettingItem::ConstructL
 * second phase construction
 */
void CCalDavPasswordSettingItem::ConstructL(const TDesC &aCalendar)
	{
	iCalendar = aCalendar.AllocL();
	HBufC* title = NULL;
	TInt page = 0;
	switch (Identifier())
		{
		case ECalDavSettingPassword:
			title = StringLoader::LoadLC(R_CALDAV_PASSWORD);
			page = R_PASSWORD_SETTING_PAGE;
			break;
		}
	SetEmptyItemTextL(KNullDesC);
	CAknPasswordSettingItem::ConstructL(EFalse, 0, *title, NULL,page, -1);
	CleanupStack::PopAndDestroy(title);
	}

/**
 * CCalDavPasswordSettingItem::EditItemL
 * edits the item
 */
void CCalDavPasswordSettingItem::EditItemL(TBool aCalledFromMenu)
	{
	TBool enabled;
	iSession->EnabledSync(*iCalendar, enabled);
	if (enabled)
		{
		HBufC* text = StringLoader::LoadLC(R_CALDAV_QUERY_DISCONNECT);
		CAknQueryDialog* dlg = CAknQueryDialog::NewL();
		if (dlg->ExecuteLD(R_QUERY, *text))
			CAknPasswordSettingItem::EditItemL(aCalledFromMenu);
		CleanupStack::PopAndDestroy(text);
		}
	else
		CAknPasswordSettingItem::EditItemL(aCalledFromMenu);
	CAknSettingItem::UpdateListBoxTextL();
	}

/**
 * CCalDavPasswordSettingItem::StoreL
 * stores the item
 */
void CCalDavPasswordSettingItem::StoreL()
	{
	CAknPasswordSettingItem::StoreL();
	
	HBufC8* text = CnvUtfConverter::ConvertFromUnicodeToUtf8L(iInternalValue);
	CleanupStack::PushL(text);

	switch (Identifier())
		{
		case ECalDavSettingPassword:
			iSession->SetPassword(*iCalendar, *text);
			break;
		}
	CleanupStack::PopAndDestroy(text);
	}

/**
 * CCalDavPasswordSettingItem::LoadL
 * loads the item
 */
void CCalDavPasswordSettingItem::LoadL()
	{
	HBufC8* buf = HBufC8::NewL(KMAXSETTINGLENGTH);
	TPtr8 ptr(buf->Des());
	switch (Identifier())
		{
		case ECalDavSettingPassword:
			iSession->Password(*iCalendar, ptr);
			break;
		}
	CnvUtfConverter::ConvertToUnicodeFromUtf8(iInternalValue, ptr);
	delete buf;
	CAknPasswordSettingItem::LoadL();
	}

/**
 * CCalDavBooleanSettingItem::CCalDavBooleanSettingItem
 * default constructor
 */
CCalDavBooleanSettingItem::CCalDavBooleanSettingItem(TInt aId,
		CCalDavSession* aSession) :
	CAknBinaryPopupSettingItem(aId, iInternalValue), iSession(aSession)
	{

	}

/**
 * CCalDavBooleanSettingItem::~CCalDavBooleanSettingItem
 * default destructor
 */
CCalDavBooleanSettingItem::~CCalDavBooleanSettingItem()
	{
	delete iCalendar;
	}

/**
 * CCalDavBooleanSettingItem::NewLC
 * first phase construction
 */
CCalDavBooleanSettingItem* CCalDavBooleanSettingItem::NewLC(TInt aId,
		CCalDavSession* aSession, const TDesC &aCalendar)
	{
	CCalDavBooleanSettingItem* self = new (ELeave) CCalDavBooleanSettingItem(
			aId, aSession);
	CleanupStack::PushL(self);
	self->ConstructL(aCalendar);
	return self;
	}

/**
 * CCalDavBooleanSettingItem::NewLC
 * first phase construction
 */
CCalDavBooleanSettingItem* CCalDavBooleanSettingItem::NewL(TInt aId,
		CCalDavSession* aSession, const TDesC &aCalendar)
	{
	CCalDavBooleanSettingItem* self = CCalDavBooleanSettingItem::NewLC(aId,
			aSession, aCalendar);
	CleanupStack::Pop(); // self;
	return self;
	}

/**
 * CCalDavBooleanSettingItem::ConstructL
 * second phase construction
 */
void CCalDavBooleanSettingItem::ConstructL(const TDesC &aCalendar)
	{
	iCalendar = aCalendar.AllocL();
	HBufC* title = NULL;
	TInt text = 0;
	TInt page = R_CALDAV_BINARY_PAGE;
	switch (Identifier())
		{
		case ECalDavSettingKeepServer:
			title = StringLoader::LoadLC(R_CALDAV_CONFLICTACTION);
			text = R_CALDAV_CONFLICTACTION_TEXTS;
			break;
		case ECalDavSettingSyncImmediate:
			title = StringLoader::LoadLC(R_CALDAV_IMMEDIATESYNC);
			text = R_CALDAV_IMMEDIATESYNC_TEXTS;
			break;
		case ECalDavSettingEnable:
			title = StringLoader::LoadLC(R_CALDAV_ENABLE);
			text = R_CALDAV_ENABLE_TEXTS;
			break;
		}
	SetEmptyItemTextL(KNullDesC);
	CAknBinaryPopupSettingItem::ConstructL(EFalse, 0, *title, NULL,page,
			EAknCtPopupSettingList, NULL, text);
	CleanupStack::PopAndDestroy(title);
	}

/**
 * CCalDavBooleanSettingItem::EditItemL
 * edits the item
 */
void CCalDavBooleanSettingItem::EditItemL(TBool aCalledFromMenu)
	{
	if (Identifier() == ECalDavSettingEnable)
		{
		TBool enabled_old;
		iSession->EnabledSync(*iCalendar, enabled_old);
		if (enabled_old)
			{
			HBufC* text = StringLoader::LoadLC(R_CALDAV_QUERY_DISCONNECT);
			CAknQueryDialog* dlg = CAknQueryDialog::NewL();
			if (dlg->ExecuteLD(R_QUERY, *text))
				{
				iSession->Disable(*iCalendar);
				CAknBinaryPopupSettingItem::EditItemL(aCalledFromMenu);
				}
			CleanupStack::PopAndDestroy(text);
			}
		else
			{
			iSession->Enable(*iCalendar);
			// TODO: progressbar
			TBool enabled_new;
			iSession->EnabledSync(*iCalendar, enabled_new);
			if (!enabled_new)
				{
				ShowNoteL(R_CALDAV_QUERY_FAIL);
				}
			else
				{
				ShowNoteL(R_CALDAV_QUERY_SUCCESS);
				CAknBinaryPopupSettingItem::EditItemL(aCalledFromMenu);
				}
			}
		}
	else
		CAknBinaryPopupSettingItem::EditItemL(aCalledFromMenu);
	}

/**
 * CCalDavBooleanSettingItem::StoreL
 * stores the item
 */
void CCalDavBooleanSettingItem::StoreL()
	{
	CAknBinaryPopupSettingItem::StoreL();

	switch (Identifier())
		{
		case ECalDavSettingKeepServer:
			iSession->SetKeepServerEntry(*iCalendar, iInternalValue);
			break;
		case ECalDavSettingSyncImmediate:
			iSession->SetImmediateSync(*iCalendar, iInternalValue);
			break;
		}
	}

/**
 * CCalDavBooleanSettingItem::LoadL
 * loads the item
 */
void CCalDavBooleanSettingItem::LoadL()
	{
	switch (Identifier())
		{
		case ECalDavSettingKeepServer:
			iSession->KeepServerEntry(*iCalendar, iInternalValue);
			break;
		case ECalDavSettingSyncImmediate:
			iSession->ImmediateSync(*iCalendar, iInternalValue);
			break;
		case ECalDavSettingEnable:
			iSession->EnabledSync(*iCalendar, iInternalValue);
			break;
		}
	CAknBinaryPopupSettingItem::LoadL();
	}

/**
 * CCalDavEnumeratedTextSettingItem::CCalDavEnumeratedTextSettingItem
 * default constructor
 */
CCalDavEnumeratedTextSettingItem::CCalDavEnumeratedTextSettingItem(TInt aId,
		CCalDavSession* aSession) :
	CAknEnumeratedTextPopupSettingItem(aId, iInternalValue), iSession(aSession)
	{

	}

/**
 * CCalDavEnumeratedTextSettingItem::~CCalDavEnumeratedTextSettingItem
 * default destructor
 */
CCalDavEnumeratedTextSettingItem::~CCalDavEnumeratedTextSettingItem()
	{
	delete iCalendar;
	}

/**
 * CCalDavEnumeratedTextSettingItem::NewLC
 * first phase construction
 */
CCalDavEnumeratedTextSettingItem* CCalDavEnumeratedTextSettingItem::NewLC(
		TInt aId, CCalDavSession* aSession, const TDesC &aCalendar)
	{
	CCalDavEnumeratedTextSettingItem* self =
			new (ELeave) CCalDavEnumeratedTextSettingItem(aId, aSession);
	CleanupStack::PushL(self);
	self->ConstructL(aCalendar);
	return self;
	}

/**
 * CCalDavEnumeratedTextSettingItem::NewL
 * first phase construction
 */
CCalDavEnumeratedTextSettingItem* CCalDavEnumeratedTextSettingItem::NewL(
		TInt aId, CCalDavSession* aSession, const TDesC &aCalendar)
	{
	CCalDavEnumeratedTextSettingItem* self =
			CCalDavEnumeratedTextSettingItem::NewLC(aId, aSession, aCalendar);
	CleanupStack::Pop(); // self;
	return self;
	}

/**
 * CCalDavEnumeratedTextSettingItem::ConstructL
 * second phase construction
 */
void CCalDavEnumeratedTextSettingItem::ConstructL(const TDesC &aCalendar)
	{
	iCalendar = aCalendar.AllocL();
	HBufC* title = NULL;
	TInt text = 0;
	TInt page = R_CALDAV_BINARY_PAGE;
	switch (Identifier())
		{
		case ECalDavSettingSyncRange:
			title = StringLoader::LoadLC(R_CALDAV_RANGE);
			text = R_CALDAV_RANGE_TEXTS;
			break;
		case ECalDavSettingSyncInterval:
			title = StringLoader::LoadLC(R_CALDAV_TIME);
			text = R_CALDAV_TIME_TEXTS;
			break;

		}
	SetEmptyItemTextL(KNullDesC);
	CAknEnumeratedTextSettingItem::ConstructL(EFalse, 0, *title, NULL,page,
			EAknCtPopupSettingList, NULL, text);
	CleanupStack::PopAndDestroy(title);
	}

/**
 * CCalDavEnumeratedTextSettingItem::EditItemL
 * edits the item
 */
void CCalDavEnumeratedTextSettingItem::EditItemL(TBool aCalledFromMenu)
	{
	CAknEnumeratedTextPopupSettingItem::EditItemL(aCalledFromMenu);
	}

/**
 * CCalDavEnumeratedTextSettingItem::StoreL
 * stores the item
 */
void CCalDavEnumeratedTextSettingItem::StoreL()
	{
	CAknEnumeratedTextPopupSettingItem::StoreL();

	switch (iInternalValue)
		{
		case 0:
			{
			if (Identifier() == ECalDavSettingSyncRange)
				iSession->SetPastDays(*iCalendar, TTimeIntervalDays(
						KTIMETWOWEEKS));
			else
				iSession->SetSyncInterval(*iCalendar, TTimeIntervalMinutes(
						KTIMEALWAYSON));
			break;
			}
		case 1:
			{
			if (Identifier() == ECalDavSettingSyncRange)
				iSession->SetPastDays(*iCalendar, TTimeIntervalDays(
						KTIMETHREEMONTH));
			else
				iSession->SetSyncInterval(*iCalendar, TTimeIntervalMinutes(
						KTIMEONEMINUTE));
			break;
			}
		case 2:
			{
			if (Identifier() == ECalDavSettingSyncRange)
				iSession->SetPastDays(*iCalendar, TTimeIntervalDays(
						KTIMESIXMONTH));
			else
				iSession->SetSyncInterval(*iCalendar, TTimeIntervalMinutes(
						KTIMEFIVEMINUTES));
			break;
			}
		case 3:
			{
			if (Identifier() == ECalDavSettingSyncRange)
				iSession->SetPastDays(*iCalendar, TTimeIntervalDays(KTIMEALL));
			else
				iSession->SetSyncInterval(*iCalendar, TTimeIntervalMinutes(
						KTIMEFIFTEENMINUTES));
			break;
			}
		case 4:
			{
			if (Identifier() == ECalDavSettingSyncInterval)
				iSession->SetSyncInterval(*iCalendar, TTimeIntervalMinutes(
						KTIMETHIRTYMINUTES));
			break;
			}
		default:
			if (Identifier() == ECalDavSettingSyncRange)
				iSession->SetPastDays(*iCalendar, TTimeIntervalDays(
						KTIMETWOWEEKS));
			else
				iSession->SetSyncInterval(*iCalendar, TTimeIntervalMinutes(
						KTIMEALWAYSON));

			break;
		}
	}

/**
 * CCalDavEnumeratedTextSettingItem::LoadL
 * loads the item
 */
void CCalDavEnumeratedTextSettingItem::LoadL()
	{
	switch (Identifier())
		{
		case ECalDavSettingSyncRange:
			{
			TTimeIntervalDays days;
			iSession->PastDays(*iCalendar, days);
			switch (days.Int())
				{
				case KTIMETWOWEEKS:
					{
					iInternalValue = 0;
					break;
					}
				case KTIMETHREEMONTH:
					{
					iInternalValue = 1;
					break;
					}
				case KTIMESIXMONTH:
					{
					iInternalValue = 2;
					break;
					}
				case KTIMEALL:
					{
					iInternalValue = 3;
					break;
					}
				default:
					iInternalValue = 0;
					break;
				}
			break;
			}
		case ECalDavSettingSyncInterval:
			{
			TTimeIntervalMinutes minutes;
			iSession->SyncInterval(*iCalendar, minutes);
			switch (minutes.Int())
				{
				case KTIMEALWAYSON:
					{
					iInternalValue = 0;
					break;
					}
				case KTIMEONEMINUTE:
					{
					iInternalValue = 1;
					break;
					}
				case KTIMEFIVEMINUTES:
					{
					iInternalValue = 2;
					break;
					}
				case KTIMEFIFTEENMINUTES:
					{
					iInternalValue = 3;
					break;
					}
				case KTIMETHIRTYMINUTES:
					{
					iInternalValue = 4;
					break;
					}
				default:
					iInternalValue = 0;
					break;
				}
			break;
			}
		}
	CAknEnumeratedTextPopupSettingItem::LoadL();
	}

/**
 * CCalDavAccessPointSettingItem::NewL
 * first phase construction
 */
CCalDavAccessPointSettingItem* CCalDavAccessPointSettingItem::NewL(
		TInt aSettingId, CCalDavSession* aSession, const TDesC &aCalendar)
	{
	CCalDavAccessPointSettingItem * item =
			new (ELeave) CCalDavAccessPointSettingItem(aSettingId, aSession);
	CleanupStack::PushL(item);
	item->ConstructL(aCalendar);
	CleanupStack::Pop(item);
	return item;
	}

/**
 * CCalDavAccessPointSettingItem::CCalDavAccessPointSettingItem
 * default constructor
 */
CCalDavAccessPointSettingItem::CCalDavAccessPointSettingItem(TInt aSettingId,
		CCalDavSession* aSession) :
	CAknSettingItem(aSettingId), iSession(aSession)
	{
	}

/**
 * CCalDavAccessPointSettingItem::~CCalDavAccessPointSettingItem
 * default destructor
 */
CCalDavAccessPointSettingItem::~CCalDavAccessPointSettingItem()
	{
	delete iSettingText;
	}

/**
 * CCalDavAccessPointSettingItem::ConstructL
 * second phase construction
 */
void CCalDavAccessPointSettingItem::ConstructL(const TDesC &/*aCalendar*/)
	{
	SetEmptyItemTextL(KNullDesC);
	HBufC* title = StringLoader::LoadLC(R_CALDAV_ACCESS_POINT);
	CAknSettingItem::ConstructL(EFalse, 0, *title, NULL,0, 0);
	CleanupStack::PopAndDestroy(title);

	_LIT(hi,"Default Connection");
	iSettingText = hi().Alloc();
	}

/**
 * CCalDavAccessPointSettingItem::EditItemL
 * edits the item
 */
void CCalDavAccessPointSettingItem::EditItemL(TBool /*aCalledFromMenu*/)
	{
	delete iSettingText;
	iSettingText = NULL;

	TCmSettingSelection userSelection;
	userSelection.iId = 0;
	userSelection.iResult = CMManager::EDefaultConnection;

	CCmApplicationSettingsUi* settings = CCmApplicationSettingsUi::NewL();
	CleanupStack::PushL(settings);
	TUint listedItems = CMManager::EShowDefaultConnection
			| CMManager::EShowDestinations | CMManager::EShowConnectionMethods;
	TBearerFilterArray filter;
	settings->RunApplicationSettingsL(userSelection, listedItems, filter);
	CleanupStack::PopAndDestroy(settings);

	switch (userSelection.iResult)
		{
		case CMManager::EDestination:
			{
			TConnSnapPref prefs;
			prefs.SetSnap(userSelection.iId);
			// iSettingText =
			// iConnection.Start( prefs, iStatus );
			break;
			}
		case CMManager::EConnectionMethod:
			{
			TCommDbConnPref prefs;
			prefs.SetIapId(userSelection.iId);
			prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);

			// iConnection.Start( prefs, iStatus );
			// iSettingText =
			break;
			}
		case CMManager::EDefaultConnection:
			{
			// iConnection.Start( iStatus );
			// iSettingText = 
			break;
			}
		}
	UpdateListBoxTextL();
	}

/**
 * CCalDavAccessPointSettingItem::SettingTextL
 * returns the setting text
 */
const TDesC& CCalDavAccessPointSettingItem::SettingTextL()
	{
	if (!iSettingText)
		{
		return CAknSettingItem::SettingTextL();
		}
	else if (iSettingText->Length() == 0)
		{
		return CAknSettingItem::SettingTextL();
		}

	return *iSettingText;
	}

/**
 * CCalDavAccessPointSettingItem::StoreL
 * stores the item
 */
void CCalDavAccessPointSettingItem::StoreL()
	{
	CAknSettingItem::StoreL();
	}

/**
 * CCalDavAccessPointSettingItem::LoadL
 * loads the item
 */
void CCalDavAccessPointSettingItem::LoadL()
	{
	//CAknSettingItem::Load();
	}
//  End of File  
