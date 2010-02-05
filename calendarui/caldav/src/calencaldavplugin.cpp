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
* Description: CalDav ECOM plugin
*
*/

#include <eikenv.h>
#include <aknutils.h>
#include <eiklabel.h>
#include <avkon.hrh>
#include <StringLoader.h>
#include <AknMessageQueryDialog.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <aknbiditextutils.h>
#include <aknsettingitemlist.h>

#include <caldavsession.h>
#include <calcalendarinfo.h>
#include <calenservices.h>
#include <calendar.rsg>

#include <CalenCaldavPluginData.rsg>
#include <calencommands.hrh>
#include "caldavsettingitems.h"
#include "calennestedsettingitem.h"
#include "calendarui_debug.h" 
#include "CalenCaldavPlugin.h"

_LIT( KResourceCCalDavPluginFile, "calencaldavplugindata.rsc" );

#define KResourcePath KDC_RESOURCE_FILES_DIR

/**
 * CCalenCalDavPlugin::CCalenCalDavPlugin
 * default constructor
 */
CCalenCalDavPlugin::CCalenCalDavPlugin(MCalenServices* aServices) :
	iServices(aServices), iResourceFileOffset(NULL)

	{
	TRACE_ENTRY_POINT;TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::NewL
 * first phase construction
 */
CCalenCalDavPlugin* CCalenCalDavPlugin::NewL(MCalenServices* aServices)
	{
	TRACE_ENTRY_POINT;
	CCalenCalDavPlugin* self = new (ELeave) CCalenCalDavPlugin(aServices);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);TRACE_EXIT_POINT;
	return self;
	}

/**
 * CCalenCalDavPlugin::~CCalenCalDavPlugin
 * default destructor
 */
CCalenCalDavPlugin::~CCalenCalDavPlugin()
	{
	TRACE_ENTRY_POINT;

	delete iSession;

	if (iServices)
		{
		iServices->CancelNotifications(this);
		iServices->Release();
		}

	if (iResourceFileOffset)
		{
		CCoeEnv::Static()->DeleteResourceFile(iResourceFileOffset);
		}TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::ConstructL
 * second phase construction
 */
void CCalenCalDavPlugin::ConstructL()
	{
	TRACE_ENTRY_POINT;

	TFileName dllName;
	// Get the complate path of the DLL from where it is currently loaded
	Dll::FileName(dllName);

	TFileName resourceFilename;
	resourceFilename.Append(dllName.Mid(0, 2));
	resourceFilename.Append(KResourcePath);
	resourceFilename.Append(KResourceCCalDavPluginFile);
	BaflUtils::NearestLanguageFile(CEikonEnv::Static()->FsSession(),
			resourceFilename);
	// Add the resource file.
	iResourceFileOffset = CEikonEnv::Static()->AddResourceFileL(
			resourceFilename);

	iServices->GetCommandRange(iStart, iEnd);

	iSession = CCalDavSession::NewL();

	TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::InfobarL
 * show an additional infobar
 */
CCoeControl* CCalenCalDavPlugin::InfobarL(const TRect& /*aRect*/)
	{
	TRACE_ENTRY_POINT;
	return NULL;TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::InfobarL
 * show an additional infobar
 */
const TDesC& CCalenCalDavPlugin::InfobarL()
	{
	TRACE_ENTRY_POINT;
	return KNullDesC;
	TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::PreviewPaneL
 * show an additional preview pabe
 */
CCoeControl* CCalenCalDavPlugin::PreviewPaneL(TRect& /* aRect */)
	{
	TRACE_ENTRY_POINT;
	return NULL;
	TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::CustomPreviewPaneL
 * show an additional custom preview pane
 */
MCalenPreview* CCalenCalDavPlugin::CustomPreviewPaneL(TRect& /* aRect */)
	{
	TRACE_ENTRY_POINT;
	return NULL;
	TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::CustomiseMenuPaneL
 * customize menu pane
 */
TBool CCalenCalDavPlugin::CustomiseMenuPaneL(TInt /*aResourceId*/,
		CEikMenuPane* /*aMenuPane*/)
	{
	TRACE_ENTRY_POINT;
	return EFalse;
	TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::HandleCommandL
 * let this plugin handle its own commands
 */
TBool CCalenCalDavPlugin::HandleCommandL(const TCalenCommand& /*aCommand*/)
	{
	TRACE_ENTRY_POINT;TRACE_EXIT_POINT;
	return EFalse;
	}

/**
 * CCalenCalDavPlugin::CommandHandlerL
 * let this plugin handle its own commands
 */
MCalenCommandHandler* CCalenCalDavPlugin::CommandHandlerL(TInt /*aCommand*/)
	{
	TRACE_ENTRY_POINT;
	MCalenCommandHandler* commandHandler = NULL;
	return commandHandler;TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::RemoveViewsFromCycle
 * remove added views from cycle
 */
void CCalenCalDavPlugin::RemoveViewsFromCycle(RArray<TInt>& /*aViews*/)
	{
	TRACE_ENTRY_POINT;TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::GetCustomSettingsL
 * add custom settings to calendar setting pane
 * all CalDav settings are added here into UI
 */
void CCalenCalDavPlugin::GetCustomSettingsL(
		RPointerArray<CAknSettingItem>& aCustomSettingArray)
	{
	TRACE_ENTRY_POINT

	RPointerArray<CCalCalendarInfo> calendarInfoList;
	iServices->GetAllCalendarInfoL(calendarInfoList);
	CleanupClosePushL(calendarInfoList);

	TInt count = calendarInfoList.Count();
	for (TInt i = 0; i < count; i++)
		{
		TPtrC calendar = calendarInfoList[i]->FileNameL();

		RPointerArray<CAknSettingItem> settings;

		CCalDavBooleanSettingItem * enable = CCalDavBooleanSettingItem::NewL(
				ECalDavSettingEnable, iSession, calendar);

		CCalDavTextSettingItem* url = CCalDavTextSettingItem::NewL(
				ECalDavSettingUrl, iSession, calendar);
		url->SetEnableSettingItem(enable);
		settings.Append(url);

		CCalDavTextSettingItem* user = CCalDavTextSettingItem::NewL(
				ECalDavSettingUser, iSession, calendar);
		user->SetEnableSettingItem(enable);
		settings.Append(user);

		CCalDavPasswordSettingItem* password = CCalDavPasswordSettingItem::NewL(
				ECalDavSettingPassword, iSession, calendar);
		password->SetEnableSettingItem(enable);
		settings.Append(password);

		CCalDavBooleanSettingItem * keepserver =
				CCalDavBooleanSettingItem::NewL(ECalDavSettingKeepServer,
						iSession, calendar);
		settings.Append(keepserver);

		//    CCalDavBooleanSettingItem * immediatesync = CCalDavBooleanSettingItem::NewL(ECalDavSettingSyncImmediate,iSession, calendar);
		//    settings.Append(immediatesync);

		CCalDavEnumeratedTextSettingItem* range =
				CCalDavEnumeratedTextSettingItem::NewL(ECalDavSettingSyncRange,
						iSession, calendar);
		settings.Append(range);

		CCalDavEnumeratedTextSettingItem* time =
				CCalDavEnumeratedTextSettingItem::NewL(
						ECalDavSettingSyncInterval, iSession, calendar);
		settings.Append(time);
		settings.Append(enable);

		CCalenNestedSettingItem* item = new (ELeave) CCalenNestedSettingItem(0,
				settings, calendarInfoList[i]->NameL());
		CleanupStack::PushL(item);

		item->SetEmptyItemTextL(KNullDesC);

		if (count == 1)
			{
			HBufC16* name = StringLoader::LoadLC(R_CALDAV);
			item->ConstructL(EFalse, 0, *name, NULL,
			R_CALEN_EMPTY_SETTING_PAGE, EAknSetListBox, NULL,
			R_CALEN_EMPTY_POPUP_SETTING_TEXTS );
			CleanupStack::PopAndDestroy(name);
			}
		else
			{
			item->ConstructL(EFalse, 0, calendarInfoList[i]->NameL(), NULL,
			R_CALEN_EMPTY_SETTING_PAGE, EAknSetListBox, NULL,
			R_CALEN_EMPTY_POPUP_SETTING_TEXTS );
			}
		aCustomSettingArray.Append(item);
		CleanupStack::Pop(item);
		}
	CleanupStack::PopAndDestroy(&calendarInfoList);

	TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::GetCustomViewsL
 * add custom views
 */
void CCalenCalDavPlugin::GetCustomViewsL(RPointerArray<CCalenView>&
/*aCustomViewArray*/)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::HandleNotification
 * handle received notifications in this plugin
 */
void CCalenCalDavPlugin::HandleNotification(const TCalenNotification /*aNotification*/)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
 * CCalenCalDavPlugin::CanBeEnabledDisabled
 * can the plugin be disabled or enabled, called by customization manager
 */
TBool CCalenCalDavPlugin::CanBeEnabledDisabled()
	{
	return EFalse;
	}

/**
 * CCalenCalDavPlugin::CalenCustomisationExtensionL
 * add custom extensions
 */
TAny* CCalenCalDavPlugin::CalenCustomisationExtensionL(TUid /*aExtensionUid*/)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return NULL;
	}

/**
 * CCalenCalDavPlugin::CalenCommandHandlerExtensionL
 * CalenCommandHandlerExtensionL
 */
TAny* CCalenCalDavPlugin::CalenCommandHandlerExtensionL(TUid /*aExtensionUid*/)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return NULL;
	}
//EOF
