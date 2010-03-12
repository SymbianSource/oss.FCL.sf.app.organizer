/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Calendar settings container
*
*/


//debug
#include "calendarui_debug.h"

// INCLUDES
#include "calensettingsuicontainer.h"

#include "CalenFileListSettingItem.h"
#include "calenglobaldata.h"
#include "CalenInterimUtils2.h"
#include "CalenSliderSettingItem.h"
#include "CalenUid.h"
#include "RImplInfoPtrArrayOwn.inl"
#include "CleanupResetAndDestroy.h"
#include "calencheckboxsettingitem.h"
#include "calennestedsettingitem.h"
#include "calenbooleansettingitem.h"
#include "calenviewinfo.h"

#include <Calendar.rsg>
#include <csxhelp/cale.hlp.hrh>
#include "calencustomisationmanager.h"
//#include <CalenUiPluginInterface.h>

#include <utf.h>
#include <aknlists.h>               // for CAknSettingStyleListBox
#include <eikclbd.h>                // CColumnListBoxData
#include <StringLoader.h>
#include <bacntf.h>                 // CEnvironmentChangeNotifier
#include <centralrepository.h>      // CRepository
#include <CalendarInternalCRKeys.h> // KCRUidCalendar


#ifdef RD_CALEN_EXTERNAL_CAL
#include <aiwservicehandler.h>

// MIME types
_LIT8(KContentTypeCalendar, "text/calendar");

#endif //RD_CALEN_EXTERNAL_CAL


#include <AknUtils.h>



const TInt KDisablePluginEnablement = 2;

void ListMailboxesL( 
		CMRMailboxUtils& aMbUtils,
		RArray<CMRMailboxUtils::TMailboxInfo>& aMailBoxes )
	{
	aMbUtils.ListMailBoxesL( aMailBoxes );
	
    // Get MR Viewer implementations
	RImplInfoPtrArrayOwn implArray;
    CleanupClosePushL( implArray );
    const TUid mrViewersIface = {KMRViewersInterfaceUID};
    REComSession::ListImplementationsL( mrViewersIface, implArray );
    TBuf16<KMaxUidName> mbName;
    
    TInt index(0);
    while (index < aMailBoxes.Count() )
    	{
    	TBool matchingMRViewerFound( EFalse );
    	TInt mrviewerImplCount( implArray.Count() );
        for( TInt j=0; j < mrviewerImplCount && !matchingMRViewerFound; ++j )
            {
            CnvUtfConverter::ConvertToUnicodeFromUtf8( mbName, implArray[j]->DataType() );
            if( aMailBoxes[index].iMtmUid.Name().CompareF(mbName) == 0 )
                {
                // Found a match. This mailbox would have been shown in the settings list.
                matchingMRViewerFound = ETrue;
                }
            }
        
        if ( !matchingMRViewerFound )
        	{
        	aMailBoxes.Remove( index );        	
        	}
        else
        	{
        	++index;
        	}
    	}
    CleanupStack::PopAndDestroy(); // implArray
	}

// ========================= MEMBER FUNCTIONS ================================


// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::NewL
// Symbian two-phased constructor.
// Two-phased constructor. Does NOT take ownership of aParentControl or aListBox.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenSettingsUiContainer* CCalenSettingsUiContainer::NewL(
                                                CCoeControl* aParentControl,
                                                CAknSettingStyleListBox& aListBox,
                                                CCalenGlobalData& aGlobalData,
                                                CCalenCustomisationManager& aCustomisationManager )
    {
    TRACE_ENTRY_POINT;

    CCalenSettingsUiContainer* self = new(ELeave) CCalenSettingsUiContainer( aParentControl,
                                                                             aListBox,
                                                                             aGlobalData,
                                                                             aCustomisationManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::ConstructL
// Symbian OS default constructor
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iSetting = CCalenSetting::InstanceL();

    // Initialize TLocale watcher
    TCallBack callback( LocaleCallbackL, this );
    iLocaleChangeNotifier =
        CEnvironmentChangeNotifier::NewL( EActivePriorityLogonA, callback );

    // Initialize Central Repository watcher
    iRepository = CRepository::NewL( KCRUidCalendar );
    iCenRepNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iRepository );
    
    iSettingItemArray = new( ELeave )CAknSettingItemArray( 2, 0, 0 );

    if( iGlobalData.InterimUtilsL().MRViewersEnabledL() )
        {
        LoadMailboxSettingsL();
        }

    iLocaleChangeNotifier->Start();
    iCenRepNotifyHandler->StartListeningL();

    iPageStatus = EFalse;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::~CCalenSettingsUiContainer
// Destructor
// ----------------------------------------------------------------------------
//
CCalenSettingsUiContainer::~CCalenSettingsUiContainer()
    {
    TRACE_ENTRY_POINT;

    delete iLocaleChangeNotifier;
    if( iCenRepNotifyHandler )
        {
        iCenRepNotifyHandler->StopListening();
        delete iCenRepNotifyHandler;
        }

    delete iRepository;

    iMailBoxes.Close();
    if( iSettingItemArray )
        {
        iSettingItemArray->ResetAndDestroy();
        delete iSettingItemArray;
        }

    if ( iSetting )
        {
        iSetting->Release();
        }

#ifdef RD_CALEN_EXTERNAL_CAL
    delete iServiceHandler;
#endif //RD_CALEN_EXTERNAL_CAL

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::CCalenSettingsUiContainer
// Default constructor
// ----------------------------------------------------------------------------
//
CCalenSettingsUiContainer::CCalenSettingsUiContainer( CCoeControl* aParentControl,
                                                      CAknSettingStyleListBox& aListBox,
                                                      CCalenGlobalData& aGlobalData,
                                                      CCalenCustomisationManager& aCustomisationManager )
    : iGlobalData( aGlobalData ),
      iParentControl( aParentControl ),
      iListBox( aListBox ),
      iCustomisationManager( aCustomisationManager ),
      iIgnoreNextSettingsNotification( EFalse ),
      iIgnoreFirstLocaleChange( ETrue )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::AddCustomSettingsWithLinkL
// Adds all settings from customisations. This will get a link from the front
// settings page. Removes items from the array when ownership transferred
// away from the array.
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::AddCustomSettingsWithLinkL( RPointerArray<CAknSettingItem>& aSettings,
                                                            const TDesC& aName,
                                                            TInt aOrdinal )
    {
    TRACE_ENTRY_POINT;

    // CCalenNestedSettingItem will take items out of aSettings array and
    // take ownership of removed items.
    CCalenNestedSettingItem* item = new( ELeave )CCalenNestedSettingItem( 0,
                                                                          aSettings,
                                                                          aName );
    CleanupStack::PushL( item );

    // The followng text should never actually be displayed on screen
    item->SetEmptyItemTextL( KNullDesC );

    // Construct setting item with parametrized values
    item->ConstructL( EFalse, aOrdinal, aName, NULL,
                      R_CALEN_EMPTY_SETTING_PAGE, EAknSetListBox,
                      NULL, R_CALEN_EMPTY_POPUP_SETTING_TEXTS );

    iSettingItemArray->InsertL( aOrdinal, item );

    CleanupStack::Pop( item );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::AddSingleCustomSettingL
// Adds a setting from customisations. This will go directly into the front
// settings page. Takes ownership of aSetting when the function completes.
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::AddSingleCustomSettingL( CAknSettingItem* aSetting,
                                                         TInt aOrdinal )
    {
    TRACE_ENTRY_POINT;

    iSettingItemArray->InsertL( aOrdinal, aSetting ); // Takes ownership of aSetting.

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::AddCustomSettingsItemsL
// Adds all settings from customisations.
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::AddCustomSettingsItemsL( TInt& aOrdinal )
    {
    TRACE_ENTRY_POINT;

    const RArray<TUid>& plugins = iCustomisationManager.ActivePlugins();

    RPointerArray<CAknSettingItem> settings;
    CleanupResetAndDestroyPushL( settings );

    for ( TInt i( 0 ); i<plugins.Count(); ++i )
        {
        iCustomisationManager.GetPluginSettingsL( plugins[i],
                                                  settings );
        // We have ownership of settings items now.

        // If there is more than one setting per plugin, it gets a link in the main
        // setting page. If there is only one, it goes directly into the main page.
        if ( settings.Count() > 1 )
            {
            const RImplInfoPtrArray& pluginInfo = iCustomisationManager.PluginInfoArray();
            TInt pluginIndex = KErrNotFound;

            for ( TInt j( 0 ); j<pluginInfo.Count(); ++j )
                {
                if ( pluginInfo[j]->ImplementationUid() == plugins[i] )
                    {
                    pluginIndex = j;
                    break;
                    }
                }

            ASSERT( pluginIndex != KErrNotFound );
            const TDesC& pluginName = pluginInfo[pluginIndex]->DisplayName();

            AddCustomSettingsWithLinkL( settings, pluginName, aOrdinal++ );
            }
        else if ( settings.Count() == 1 )
            {
            // This function takes ownership of the setting item we pass it,
            // when the function completes.
            AddSingleCustomSettingL( settings[0], aOrdinal++ );
            settings.Remove( 0 );
            }

        ASSERT( settings.Count() == 0 );
        }

    CleanupStack::PopAndDestroy( &settings );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::AddPluginSettingItemL
// Adds a settings item allowing the user to enable/disable plugins and
// change their priority.
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::AddPluginSettingItemL( TInt& aOrdinal )
    {
    TRACE_ENTRY_POINT;

    const RImplInfoPtrArray& plugins = iCustomisationManager.PluginInfoArray();
    TInt pluginCount = plugins.Count();
    
    if ( pluginCount > 0 )
        {
        // Check to see if any of the plugins are able to be be enable or 
        // disabled
        TBool pluginFound( EFalse );
        TBuf<100> displayName;
        for ( TInt index(0); index < pluginCount && !pluginFound; ++index )
            {
            // if the display name is empty and the plugin is in ROM the
            // plugin can  not be disabled
            TInt version = plugins[index]->Version();
            TBool romBased = plugins[index]->RomBased();
            displayName = plugins[index]->DisplayName();
           
            if ( ( version != KDisablePluginEnablement ) || !romBased )
                {
                pluginFound = ETrue;
                }
            } 
        
        
        
        if ( pluginFound )
            {
            //There will not be any sort of settings item for plugins which are not part of settings 
            TInt countOfAvailability =  iSetting->PluginAvailability().Count();
            
            if(countOfAvailability)
                {
                //If available plugins(which has settings) count is > 1 its check box or toggle button.  
                if(countOfAvailability > 1 )
                    {
                    CCalenCheckboxSettingItem* settingItem
                        = new( ELeave )CCalenCheckboxSettingItem( ECalenPluginListItemId,
                                                                  iCustomisationManager );
        
                    CleanupStack::PushL( settingItem );
        
                    // The followng text should never actually be displayed on screen
                    settingItem->SetEmptyItemTextL( KNullDesC );
                    
        
                    // Construct setting item with parametrized values
                    HBufC* itemTitle = StringLoader::LoadLC( R_CALEN_SETTING_PLUGINLIST_TITLE );
                    settingItem->ConstructL( EFalse, aOrdinal, *itemTitle, NULL,
                                             R_CALEN_SETTING_PLUGINLIST, EAknSetListBox,
                                             NULL, R_CALEN_EMPTY_POPUP_SETTING_TEXTS );
                    
                    iSettingItemArray->InsertL( aOrdinal, settingItem );
        
                    CleanupStack::PopAndDestroy( itemTitle );
                    CleanupStack::Pop( settingItem );
                    aOrdinal++;
                    }
                else
                    {
                    CCalenBooleanSettingItem* settingItem =
                            CCalenBooleanSettingItem::NewLC(ECalenPluginListItemId,aOrdinal,iCustomisationManager);
                    iSettingItemArray->InsertL( aOrdinal, settingItem );
                    CleanupStack::Pop( settingItem );
                    aOrdinal++;
                    }
                }
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::AddDefaultViewItemL
// Adds a setting item to allow selection of the default view. Includes custom
// views in the view cycle array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::AddDefaultViewItemL( TInt& aBuffer,
                                                     TInt aId,
                                                     TInt aTitleResource,
                                                     TInt aSettingPageResource,
                                                     TInt aOrdinal )
    {
    TRACE_ENTRY_POINT;

    // Create new setting item
    CAknEnumeratedTextPopupSettingItem* settingItem =
        new( ELeave )CAknEnumeratedTextPopupSettingItem( aId, aBuffer );
    CleanupStack::PushL( settingItem );

    // The followng text should never actually be displayed on screen
    _LIT( KNone, "None" );
    settingItem->SetEmptyItemTextL( KNone );

    // Construct setting item with parametrized values
    HBufC* itemTitle = StringLoader::LoadLC( aTitleResource );
    settingItem->ConstructL( EFalse, aOrdinal, *itemTitle, NULL,
                             aSettingPageResource, EAknCtPopupSettingList,
                             NULL, R_CALEN_EMPTY_POPUP_SETTING_TEXTS );

    CArrayPtr<CAknEnumeratedText>* items = settingItem->EnumeratedTextArray();

    const RPointerArray<CCalenViewInfo>& views = iCustomisationManager.Views();
    const RArray<TUid>& activePlugins = iCustomisationManager.ActivePlugins();

    TBool isDefViewValid( EFalse );

    TInt numViews = views.Count();
    for ( TInt i( 0 ); i<numViews; ++i )
        {
        CCalenViewInfo* view = views[i];
        TUid pluginUid = view->PluginUid();
	    TInt position = activePlugins.Find( pluginUid );
	    
		// If the view is from plugin or native view with cycle position,
        // then add it to the default view list
        if((position != KErrNotFound) ||
        	( view->CyclePosition() != CCalenView::ENoCyclePosition ))
            {
            TInt viewUidInt = view->ViewUid().iUid;

            if ( viewUidInt == iDefView )
                {
                isDefViewValid = ETrue;
                }

            CAknEnumeratedText* item = new( ELeave )CAknEnumeratedText( viewUidInt,
                                                                        view->SettingsName().AllocL() );
            items->AppendL( item );
            }
        }

    if ( !isDefViewValid )
        {
        // Default view is no longer valid (could be first ever load, or could be
        // a 3rd party plugin's view used to be the default but is not removed).
        // Set default to the first in the view cycle array. 
        TInt defViewIndex( 0 );
        while ( views[defViewIndex]->CyclePosition() == CCalenView::ENoCyclePosition )
            {
            defViewIndex++;
            }
            
        iDefView = views[defViewIndex]->ViewUid().iUid;
        iSetting->SetDefaultView( TUid::Uid( iDefView ) );
        }

    settingItem->HandleTextArrayUpdateL();

    // Append item to settingitem-array
    iSettingItemArray->InsertL( aOrdinal, settingItem );

    CleanupStack::PopAndDestroy( itemTitle );
    CleanupStack::Pop( settingItem );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::AddSnoozeTimeItemL
// Adds an alarm snooze time item
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::AddSnoozeTimeItemL( TInt aOrdinal,
                                                    CArrayPtr<CGulIcon>* aIconArray,
                                                    TInt& aSnoozeTime)
    {
    TRACE_ENTRY_POINT;

    CCalenSliderSettingItem* settingItem =
                new( ELeave )CCalenSliderSettingItem( ECalenSnoozeTimeItemId,
                                                      aSnoozeTime,
                                                      R_SNOOZE_TIME_SLIDER );
    CleanupStack::PushL( settingItem );

    HBufC* itemTitle = StringLoader::LoadLC( R_QTN_CALE_SET_SNOOZE_TIME );

    settingItem->ConstructL( EFalse,
                             aOrdinal,
                             *itemTitle,
                             aIconArray,
                             R_SNOOZE_TIME_SETTING_PAGE,
                             EAknCtSlider );

    iSettingItemArray->InsertL( aOrdinal, settingItem );

    CleanupStack::PopAndDestroy( itemTitle );
    CleanupStack::Pop( settingItem );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// AddEnumerationItemL()
// Add integer type of settings item
// Integer type item can have multiple choices shown as radiobutton list.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::AddEnumerationItemL( TInt& aBuffer,
                                                     TInt aId,
                                                     TInt aTitleResource,
                                                     TInt aSettingPageResource,
                                                     TInt aAssociatedResource,
                                                     TInt aOrdinal )
    {
    TRACE_ENTRY_POINT;

    // Create new setting item
    CAknSettingItem* settingItem =
        new ( ELeave ) CAknEnumeratedTextPopupSettingItem( aId, aBuffer );
    CleanupStack::PushL( settingItem );

    // Construct setting item with parametrized values
    HBufC* itemTitle = StringLoader::LoadLC( aTitleResource );
    settingItem->ConstructL( EFalse, aOrdinal, *itemTitle, NULL,
                             aSettingPageResource, EAknCtPopupSettingList,
                             NULL, aAssociatedResource );

    // Append item to settingitem-array
    iSettingItemArray->InsertL( aOrdinal, settingItem );

    CleanupStack::PopAndDestroy( itemTitle );
    CleanupStack::Pop( settingItem );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSettingsUiContainer::AddBinaryItemL()
// Add binary type (true/false) settings item
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::AddBinaryItemL( TBool& aBuffer,
                                                TInt aId,
                                                TInt aTitleResource,
                                                TInt aSettingPageResource,
                                                TInt aAssociatedResource,
                                                TInt aOrdinal )
    {
    TRACE_ENTRY_POINT;

    // Create new setting item
    CAknSettingItem* settingItem =
        new ( ELeave ) CAknBinaryPopupSettingItem( aId,  aBuffer );
    CleanupStack::PushL( settingItem );

    // Construct setting item with parametrized values
    HBufC* itemTitle = StringLoader::LoadLC( aTitleResource );
    settingItem->ConstructL( EFalse, aOrdinal, *itemTitle, NULL,
                             aSettingPageResource, EAknCtPopupSettingList,
                             NULL, aAssociatedResource );

    // Append item to settingitem-array
    iSettingItemArray->InsertL( aOrdinal, settingItem );

    CleanupStack::PopAndDestroy( itemTitle );
    CleanupStack::Pop( settingItem );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::AddAlarmToneItemL
// Adds an item to the settings page which contains information about
// which tone should be played when an alarm expires.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::AddAlarmToneItemL( TInt aOrdinal )
    {
    TRACE_ENTRY_POINT;

    CCalenFileListSettingItem* settingItem =
        new ( ELeave ) CCalenFileListSettingItem( ECalenAlarmToneItemId );
    CleanupStack::PushL( settingItem );

    // Construct setting item with parametrized values
    HBufC* itemTitle = StringLoader::LoadLC( R_CALEN_SETTING_ALARMTONE_TITLE );
    settingItem->ConstructL( EFalse, aOrdinal, *itemTitle, NULL,
                             NULL, NULL, NULL, NULL );

    // Append item to settingitem-array
    iSettingItemArray->InsertL( aOrdinal, settingItem );

    CleanupStack::PopAndDestroy( itemTitle );
    CleanupStack::Pop( settingItem );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::CreateMailboxSettingsItemL
// Creates the settings item for selection of the default meeting request
// mailbox. This should only be called when MR viewers are enabled.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::CreateMailboxSettingsItemL( TInt aOrdinal )
    {
    TRACE_ENTRY_POINT;

    // Mailbox settings are shown when we have a MR viewers implementation.
    ASSERT( iGlobalData.InterimUtilsL().MRViewersEnabledL() );
    CMRMailboxUtils& mailboxUtils = iGlobalData.MRMailboxUtilsL();
    CAknEnumeratedTextPopupSettingItem* settingItem =
        new ( ELeave ) CAknEnumeratedTextPopupSettingItem( ECalenDefaultMailboxId,
                                                           iDefMailbox );
    CleanupStack::PushL( settingItem );

    // Construct setting item with parametrized values
    HBufC* itemTitle = StringLoader::LoadLC( R_CALEN_SETTING_DEFAULTMAILBOX_TITLE );
    // The resources loaded up here are empty as the items need to be dynamically updated.
    settingItem->ConstructL( EFalse, aOrdinal, *itemTitle, NULL,
                             R_CALEN_SETTING_DEFAULTMAILBOX, EAknCtPopupSettingList,
                             NULL, R_CALEN_EMPTY_POPUP_SETTING_TEXTS );

    CArrayPtr<CAknEnumeratedText>* items = settingItem->EnumeratedTextArray();

    RImplInfoPtrArrayOwn implArray;
    CleanupClosePushL( implArray );
    //Get all MRViewers Implementation
    CMRMailboxUtils::TMailboxInfo defaultMailbox;
    const TUid mrViewersIface = {KMRViewersInterfaceUID};
    REComSession::ListImplementationsL( mrViewersIface, implArray );

    TBuf16<KMaxUidName> mbName;
    TInt enumerationCounter = 0;

    for( TInt i=0; i < iMailBoxes.Count(); ++i )
        {
        for( TInt j=0; j < implArray.Count(); ++j )
            {
            CnvUtfConverter::ConvertToUnicodeFromUtf8( mbName, implArray[j]->DataType() );
            if( iMailBoxes[i].iMtmUid.Name().CompareF(mbName) == 0 )
                {
                 // Found a match
                CAknEnumeratedText* item = new(ELeave) CAknEnumeratedText( enumerationCounter,
                                                                           iMailBoxes[i].iName.AllocL() );
                items->AppendL( item );
                ++enumerationCounter;
                }
            }
        }

    CleanupStack::PopAndDestroy();    //implArray

    settingItem->HandleTextArrayUpdateL();

    // Append item to settingitem-array
    iSettingItemArray->InsertL( aOrdinal, settingItem );

    CleanupStack::PopAndDestroy( itemTitle );
    CleanupStack::Pop( settingItem );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::UpdateListBoxL
// Update listbox item
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenSettingsUiContainer::UpdateListBoxL()
    {
    TRACE_ENTRY_POINT;

    iListBox.HandleItemAdditionL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::ConstructListBoxL
// Update listbox item
// Items are added manually, instead of loading them from resources
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenSettingsUiContainer::ConstructListBoxL()
    {
    TRACE_ENTRY_POINT;

    iListBox.ConstructL( iParentControl, EAknListBoxSelectionList );

    LoadCurrentValuesL();

    // Create items to array
    CreateListBoxItemsL();

    // Set array to listbox
    CTextListBoxModel* model = iListBox.Model();
    model->SetItemTextArray( iSettingItemArray );
    // Ownership retained by us
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );

    iSettingItemArray->RecalculateVisibleIndicesL();
    iListBox.CreateScrollBarFrameL();
    iListBox.ScrollBarFrame()->SetScrollBarVisibilityL(
                        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::CreateListBoxItemsL
// Update listbox item
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::CreateListBoxItemsL()
    {
    TRACE_ENTRY_POINT;

    TInt ordinal = 0;

    AddAlarmToneItemL( ordinal++ );

    CArrayPtr<CGulIcon>* iconArray = iListBox.ItemDrawer()->ColumnData()->IconArray();

    if( !iconArray )
        {
        iconArray = new( ELeave )CArrayPtrFlat<CGulIcon>( 1 );
        iListBox.ItemDrawer()->ColumnData()->SetIconArrayL( iconArray ); // ownership transfered
        }

    AddSnoozeTimeItemL( ordinal++, iconArray, iSnoozeTime );

    AddDefaultViewItemL( iDefView,
                         ECalenDefaultViewItemId,
                         R_CALEN_SETTING_DEFAULTVIEW_TITLE,
                         R_CALEN_SETTING_DEFAULTVIEW,
                         ordinal++ );

    AddEnumerationItemL( iWeekFormat,
                         ECalenWeekFormatItemId,
                         R_CALEN_SETTING_WEEKFORMAT_TITLE,
                         R_CALEN_SETTING_WEEKFORMAT,
                         R_CALEN_WEEKFORMAT_TEXTS,
                         ordinal++ );


    // Always add this in. We will set it hidden when it's not needed (i.e. when the
    // week start is set to anything other than "Monday") at the bottom of this function.
    AddBinaryItemL( iWeekTitle,
                         ECalenWeekTitleItemId,
                         R_CALEN_SETTING_WEEKTITLE_TITLE,
                         R_CALEN_SETTING_WEEKTITLE,
                         R_CALEN_WEEKTITLE_TEXTS,
                         ordinal++ );
    

#ifdef RD_CALEN_EXTERNAL_CAL
    // add external calendar support if available.
    TBool isExt=ExtCalendarAvailableL();
    if (isExt)
        {
        AddEnumerationItemL( iExtCalendar,
                             ECalenExtCalendarItemId,
                             R_CALEN_SETTING_EXT_CALENDAR_TITLE,
                             R_CALEN_EXT_CALENDAR_SETTING_PAGE,
                             R_CALEN_EXT_CALENDAR_SETTING_TEXTS,
                             ordinal++ );
        }

#endif //RD_CALEN_EXTERNAL_CAL

    // Only add the mailbox choice when the Meeting Request functionality is there.
    if( iGlobalData.InterimUtilsL().MRViewersEnabledL() )
        {
        CreateMailboxSettingsItemL( ordinal++ ); // increments ordinal
        }

#ifdef RD_SCALABLE_UI_V2_NO_TOOLBAR_SETTING
    if( AknLayoutUtils::PenEnabled() )
        {
        AddBinaryItemL( iToolbar,
                        ECalenToolbarItemId,
                        R_CALEN_SETTING_TOOLBAR_TITLE,
                        R_CALEN_SETTING_TOOLBAR,
                        R_CALEN_TOOLBAR_TEXTS,
                        ordinal++ );
        }
#endif // RD_SCALABLE_UI_V2_NO_TOOLBAR_SETTING

    AddPluginSettingItemL( ordinal );

    AddCustomSettingsItemsL( ordinal );
    ++ordinal;

    // Hide the week title setting if the week start is anything other than Monday.
    Item( ECalenWeekTitleItemId )->SetHidden( !ShouldShowWeekTitleItem() );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::CurrentFeatureId
// Get the currently selected feature
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CCalenSettingsUiContainer::CurrentFeatureId() const
    {
    TRACE_ENTRY_POINT;

    if ( iSettingItemArray )
        {
        TRACE_EXIT_POINT;
        return iSettingItemArray->MdcaCount();
        }
    else
        {
        TRACE_EXIT_POINT;
        return 0;
        }
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::GetHelpContext
// Get help for this view
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    TRACE_ENTRY_POINT;

    aContext.iMajor = KUidCalendar;
    // This is specified in HRH file.
    aContext.iContext = KCALE_HLP_SETTINGS;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::ShouldShowWeekTitleItem
// Returns ETrue if the start of the week is set to Monday.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenSettingsUiContainer::ShouldShowWeekTitleItem() const
    {
    TRACE_ENTRY_POINT;
    TDay day = static_cast<TDay>( iWeekFormat );
    TRACE_EXIT_POINT;
    return ( day == EMonday );
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::Item
// Returns the settings item associated with the given setting list item id.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CAknSettingItem* CCalenSettingsUiContainer::Item( TSettingListItemId aId )
    {
    TRACE_ENTRY_POINT;
    for( TInt i = 0; i < iSettingItemArray->Count(); ++i )
        {
        if( iSettingItemArray->At(i)->Identifier() == aId )
            {
            TRACE_EXIT_POINT;
            return iSettingItemArray->At( i );
            }
        }
    ASSERT( EFalse );
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::ValidateWeekTitleItemL
// Hides or shows the week title item based on whether the beginning of the
// week is set to Monday or not. This should only be called on the week format item.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::ValidateWeekTitleItemL( CAknSettingItem *aItem )
    {
    TRACE_ENTRY_POINT;

    ASSERT( aItem->Identifier() == ECalenWeekFormatItemId );
    TBool wasShown = ShouldShowWeekTitleItem();
    aItem->StoreL(); // Updates iWeekFormat variable
    TBool doShow = ShouldShowWeekTitleItem();

    if ( wasShown != doShow )
        {
        Item( ECalenWeekTitleItemId )->SetHidden( !doShow );

        if ( !doShow )
            {
            Item( ECalenWeekTitleItemId )->LoadL();
            }

        iSettingItemArray->RecalculateVisibleIndicesL();
        iListBox.HandleItemAdditionL();
        iListBox.UpdateScrollBarsL();
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::ShowOrHideWeekTitleL
// Shows or hides the week title setting as appropriate.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::ShowOrHideWeekTitleL()
    {
    TRACE_ENTRY_POINT;

    CAknSettingItem* wkTitle = Item( ECalenWeekTitleItemId );
    const TInt currentIndex( iListBox.CurrentItemIndex() );
    
    const TBool wasLookingAtWeekTitle( currentIndex == wkTitle->VisibleIndex() );
    const TBool doShow( ShouldShowWeekTitleItem() );

    wkTitle->SetHidden( !doShow );

    // Make sure we're still looking at a setting item after one was removed.
    if ( !doShow && wasLookingAtWeekTitle )
        {
        iListBox.SetCurrentItemIndex( Item( ECalenWeekFormatItemId )->VisibleIndex() );
        }

    iSettingItemArray->RecalculateVisibleIndicesL();
    iListBox.HandleItemAdditionL();
    iListBox.UpdateScrollBarsL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::HandleListBoxSelectionL
// Handles list box selection events.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenSettingsUiContainer::HandleListBoxSelectionL( TInt aCommand ,TBool& aPageStatus)
    {
    TRACE_ENTRY_POINT;

    TInt index( iListBox.CurrentItemIndex() );

    if ( index >= 0 )   // index is -1 if there are no items in the list
        {
        CAknSettingItem* item;
        TDay day = static_cast<TDay>( iWeekFormat );

        // If the starting day of a week is other than Monday, "Title of week view" setting item 
        // is  not displayed. For the item below "Week Format", the index in iSettingItemArray
        // is one more than CurrentItemIndex().
        if( ( day != EMonday && index >= 4 ) )
            {
            item = iSettingItemArray->At( index + 1);
            }
        else
            {
            item = iSettingItemArray->At( index );          
            }
         // Open edit dialog if EAknCmdOpen, invert the value otherwise
        aPageStatus = ETrue;
        iPageStatus = aPageStatus;
        item->EditItemL( aCommand == EAknCmdOpen );
        aPageStatus = EFalse;
        iPageStatus = aPageStatus;
        TBool isNativeSettingChange = ETrue;
        
       if(item->Identifier() > 0 )
            {
             // Save the settings change, so if we have two settings windows open
            // the background one can be updated.
            switch( item->Identifier() )
                {
                case ECalenWeekFormatItemId:
                    // item->StoreL() gets called in ValidateWeekTitleItemL
                    ValidateWeekTitleItemL( item );
                    iSetting->SetWeekFormat( static_cast<TDay>( iWeekFormat ) );
                    break;
                case ECalenAlarmToneItemId:
                    item->StoreL();
                    // CCalenFileListSettingItem::StoreL saves the filename to cenrep.
                    break;
                case ECalenSnoozeTimeItemId:
                    item->StoreL();
                    iSetting->SetSnoozeTime( iSnoozeTime );
                    break;
                case ECalenDefaultViewItemId:
                    item->StoreL();
                    iSetting->SetDefaultView( TUid::Uid( iDefView ) );
                    break;
                case ECalenWeekTitleItemId:
                    item->StoreL();
                    iSetting->SetWeekTitle( static_cast<TCalenWeekTitle>( iWeekTitle ) );
                    break;
                case ECalenDefaultMailboxId:
                    item->StoreL();
                    StoreMailboxSettingsL();
                    break;
    #ifdef RD_CALEN_EXTERNAL_CAL
                case ECalenExtCalendarItemId:
                    item->StoreL();
                    iSetting->SetExtCalendar( iExtCalendar);
                    break;
    #endif // RD_CALEN_EXTERNAL_CAL
    #ifdef RD_SCALABLE_UI_V2_NO_TOOLBAR_SETTING
                case ECalenToolbarItemId:
                    item->StoreL();
                    iSetting->SetToolbar( iToolbar );
                    break;
    #endif // RD_SCALABLE_UI_V2_NO_TOOLBAR_SETTING
                case ECalenPluginListItemId:
                    // No item to store, CCalenCheckboxSettingItem updated
                    // the customisation manager and CCalenSetting. Need to
                    // redraw the listboc
                    item->StoreL();
                    PluginListChangedL();
                    
                    break;
                default:  // Custom setting.
                    item->StoreL();
                    // Custom settings won't generate a repository change, so keep listening.
                    isNativeSettingChange = EFalse;
                    break;
                }
    
            if ( isNativeSettingChange )
                
                {
                // Don't listen for changes to cenrep/TLocale for this settings change.
                iIgnoreNextSettingsNotification = ETrue;
                iSetting->SaveL();
                }
            
          
            iListBox.DrawItem( index );
            }
        }
		TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::PluginListChangedL()
// Cenrep watcher callback
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::PluginListChangedL()
    {
    iSettingItemArray->ResetAndDestroy();

    // Update the default view as it would have changed after enabling/disabling plugins
    iDefView = iSetting->DefaultView().iUid;
    iWeekFormat = iSetting->WeekFormat();
    iWeekTitle = iSetting->WeekTitle();
    iSnoozeTime = iSetting->SnoozeTime();
    CreateListBoxItemsL();
    
    iSettingItemArray->RecalculateVisibleIndicesL();
    iListBox.HandleItemAdditionL();
    iListBox.UpdateScrollBarsL();
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::HandleNotifyGeneric
// Cenrep watcher callback
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::HandleNotifyGeneric( TUint32 aId )
    {
    TRACE_ENTRY_POINT;

    // Make sure it wasn't this object that generated the settings change.    
    if ( iIgnoreNextSettingsNotification )
        {
        iIgnoreNextSettingsNotification = EFalse;        
        }
    else
        {
        PIM_TRAPD_HANDLE( HandleExternalSettingsChangedL(aId));
        }

    // Use another trap to make sure we start listening again, regardless
    // of whether the previous function left or not.
    PIM_TRAPD_HANDLE( iCenRepNotifyHandler->StartListeningL() );

    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::HandleNotifyError
// Cenrep watcher error callback
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::HandleNotifyError( TUint32 /*aId*/,
                                                   TInt /*aError*/,
                                                   CCenRepNotifyHandler* /*aHandler*/ )
    {
    TRACE_ENTRY_POINT;

    PIM_TRAPD_HANDLE( iCenRepNotifyHandler->StartListeningL() );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::LocaleCallbackL
// Locale watcher callback
// ----------------------------------------------------------------------------
//
TInt CCalenSettingsUiContainer::LocaleCallbackL( TAny* aThisPtr )
    {
    TRACE_ENTRY_POINT;
    
    CCalenSettingsUiContainer* ptr = static_cast<CCalenSettingsUiContainer*>( aThisPtr );
    ptr->DoLocaleCallbackL();
    
    TRACE_EXIT_POINT;
    return FALSE;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::DoLocaleCallbackL
// Implementation of locale watcher callback
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::DoLocaleCallbackL()
    {
    TRACE_ENTRY_POINT;

    if ( iLocaleChangeNotifier->Change() & EChangesLocale )
        {
        if( iIgnoreFirstLocaleChange )
            {
            // When initialising the locale change observer, we get a notification
            // when observing starts. We ignore this as we don't need to act on it.
            iIgnoreFirstLocaleChange = EFalse;
            }
        else
            {
            // Make sure it wasn't this object that generated the settings change.
            if ( iIgnoreNextSettingsNotification )
                {
                iIgnoreNextSettingsNotification = EFalse;
                }
            else
                {
                HandleExternalSettingsChangedL();
                ShowOrHideWeekTitleL();
//                ValidateWeekTitleItemL( Item( ECalenWeekFormatItemId ) );
                }
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::HandleExternalSettingsChangedL
// Called when something else changed our settings
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::HandleExternalSettingsChangedL(TUint32 aId)
    {
    TRACE_ENTRY_POINT;
    // Load from cenrep to member data.
    LoadCurrentValuesL();
    
    switch(aId)
        {
        case KCalendarPluginAvailability:
           {
           HandlePluginChange(); 
           if(!iPageStatus)
               {
               PluginListChangedL();
               }
          }    
        break;
        
        default:
            {
            // If the starting day of a week is Monday, "Title of week view" setting item 
            // is also displayed. so the number of default items will be 5.
            TInt maxDefaultSettingItems = ShouldShowWeekTitleItem()?5:4;
            
            for( TInt i=0; i<maxDefaultSettingItems; ++i )
              {
              iSettingItemArray->At( i )->LoadL();
              iSettingItemArray->At( i )->UpdateListBoxTextL();
              }
            }
        }
    iListBox.HandleItemAdditionL();    

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::LoadCurrentValuesL
// Loads current values from cenrep.
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::LoadCurrentValuesL()
    {
    TRACE_ENTRY_POINT;
    iSetting->LoadL();
    
    iDefView = iSetting->DefaultView().iUid;
    iWeekFormat = iSetting->WeekFormat();
    iWeekTitle = iSetting->WeekTitle();
    iSnoozeTime = iSetting->SnoozeTime();
#ifdef RD_SCALABLE_UI_V2_NO_TOOLBAR_SETTING
    iToolbar = iSetting->Toolbar();
#endif

#ifdef RD_CALEN_EXTERNAL_CAL
    iExtCalendar = iSetting->ExtCalendar();
#endif

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::StoreMailboxSettingsL
// Stores the mailbox settings.
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::StoreMailboxSettingsL()
    {
    TRACE_ENTRY_POINT;

    if( iGlobalData.InterimUtilsL().MRViewersEnabledL() )
        {
         // iHasSelectedMailbox will be EFalse if we didn't select a MR MB previously.
         // However, they might have selected one this time.
        if( !iHasSelectedMailbox && iDefMailbox != 0 )
            {
            iHasSelectedMailbox = ETrue;
            --iDefMailbox; // This gets decremented as the first menu item was a blank, unselected choice.
            }

        if( iHasSelectedMailbox )
            {
            CMRMailboxUtils& mbUtils = iGlobalData.MRMailboxUtilsL();

            ListMailboxesL( mbUtils, iMailBoxes );

            RImplInfoPtrArrayOwn implArray;
            CleanupClosePushL( implArray );
            const TUid mrViewersIface = {KMRViewersInterfaceUID};
            REComSession::ListImplementationsL( mrViewersIface, implArray );
            TBuf16<KMaxUidName> mbName;

            TInt enumValue = 0; // How many mailboxes we have been through so far that are visible to the user.
            // Convert from the enumeration to the default mailbox id.
            for( TInt i=0; i < iMailBoxes.Count(); ++i )
                {
                for( TInt j=0; j<implArray.Count(); ++j )
                    {
                    CnvUtfConverter::ConvertToUnicodeFromUtf8( mbName, implArray[j]->DataType() );
                    if( iMailBoxes[i].iMtmUid.Name().CompareF(mbName) == 0 )
                        {
                        // Found a match. This mailbox would have been shown in the settings list.
                        if( enumValue == iDefMailbox )
                            {
                            User::LeaveIfError( mbUtils.SetDefaultMRMailBoxL( iMailBoxes[iDefMailbox].iEntryId ) );
                            break;
                            }
                        ++enumValue;
                        }
                    }
                }
            CleanupStack::PopAndDestroy(); // implArray
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::LoadMailboxSettingsL
// Loads the MR mailbox settings, if MR functionality is enabled.
// ----------------------------------------------------------------------------
//
void CCalenSettingsUiContainer::LoadMailboxSettingsL()
    {
    TRACE_ENTRY_POINT;

    if( iGlobalData.InterimUtilsL().MRViewersEnabledL() )
        {
        iHasSelectedMailbox = EFalse;
        CMRMailboxUtils::TMailboxInfo defaultMailbox;
        CMRMailboxUtils& mbUtils = iGlobalData.MRMailboxUtilsL();
				
        ListMailboxesL( mbUtils, iMailBoxes );

        TInt res = KErrNone;
        TRAPD( err, res = mbUtils.GetDefaultMRMailBoxL( defaultMailbox ) );
        if( res == KErrNone && err == KErrNone )
            {
            // Convert from the default mailbox to an enumeration.
            for( TInt i=0; i<iMailBoxes.Count(); ++i )
                {
                if( iMailBoxes[i].iEntryId == defaultMailbox.iEntryId )
                    {
                    iDefMailbox = i;
                    iHasSelectedMailbox = ETrue;
                    break;
                    }
                }
            }
        else if ( err != KErrNotFound || err != KErrGeneral )
            {
            // KErrNotFound and KErrGeneral are thrown when no mailbox is selected.
            // This is OK as iHasSelectedMailbox is EFalse, so we will show the blank option
            // and allow the user to choose. Any other error needs to leave.
            User::LeaveIfError( err );
            }
        }

    TRACE_EXIT_POINT;
    }

#ifdef RD_CALEN_EXTERNAL_CAL
// ----------------------------------------------------------------------------
// CCalenSettingViewDlg::ExtCalendarAvailableL()
// Return ETrue if we have service handler
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenSettingsUiContainer::ExtCalendarAvailableL()
    {
    TRACE_ENTRY_POINT;

    // Create service handler instance.
    if ( !iServiceHandler )
        {
        iServiceHandler = CAiwServiceHandler::NewL();
        }

    // create criteria item, this is used to specify which provider we are interested in.
    CAiwCriteriaItem* crit = CAiwCriteriaItem::NewLC( ECalenExtAiwCommandId,
                                                      KAiwCmdView,
                                                      KContentTypeCalendar );

    // we have a base service just for finding out if there is our provider available
    // this should be fixed by AIW framework in the future so that we don't need extra service.
    crit->SetServiceClass( TUid::Uid( KAiwClassBase ) );
    crit->SetMaxProviders( 1 );
    //crit->SetId(0x10282e42);
    RCriteriaArray array;
    array.Append( crit );
    // attach to the service provider if available, owenership of array is not transferred.
    iServiceHandler->AttachL( array );

    // if there is a provider here is count of suitable ones, if not it is zero
    TInt num=iServiceHandler->NbrOfProviders( crit );

    // if we detach, dll is unloaded, we don't want that as it is likely that we need it again.
    //    iServiceHandler->DetachL( array );
    array.ResetAndDestroy();
    CleanupStack::Pop( crit );

    TBool res = EFalse;

    if ( num > 0 )
        {
        res = ETrue;
        }

    TRACE_EXIT_POINT;
    return res;
    }

#endif //RD_CALEN_EXTERNAL_CAL

// ----------------------------------------------------------------------------
// CCalenSettingsUiContainer::PluginUidFinder
// Matches a uid and a plugin uid. Used by when finding a uid to match the
// setting's array against the customisation manager's array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenSettingsUiContainer::PluginUidFinder( const TUid* aUid,
                                                  const CImplementationInformation& aArrayItem )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ( *aUid == aArrayItem.ImplementationUid() );
    }

void CCalenSettingsUiContainer::HandlePluginChange()
    {
    TRACE_ENTRY_POINT;
    
    pluginAvail& pluginAvailablity = iSetting->PluginAvailability();
    for(TInt index = 0;index < pluginAvailablity.Count(); index++)
        {
        TRAP_IGNORE(iCustomisationManager.SetPluginAvailabilityL(pluginAvailablity[index].iUid, 
                                                pluginAvailablity[index].iEnabled));
        }
    TRACE_EXIT_POINT;
    }

// End of file

