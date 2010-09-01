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
* Description:   Calendar checkbox setting item
*
*/


//debug
#include "calendarui_debug.h"

// INCLUDES
#include "calencheckboxsettingitem.h"

#include "calencustomisationmanager.h"
#include "calensetting.h"
#include <akncheckboxsettingpage.h>

const TInt KDisablePluginEnablement = 2;

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CCalenCheckboxSettingItem::CCalenCheckboxSettingItem
// Constructor.
// ----------------------------------------------------------------------------
//
CCalenCheckboxSettingItem::CCalenCheckboxSettingItem(
                                        TInt aId,
                                        CCalenCustomisationManager& aCustMan )
    : CAknSettingItem( aId ),
      iCustMan( aCustMan )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCheckboxSettingItem::~CCalenCheckboxSettingItem
// Destructor
// ----------------------------------------------------------------------------
//
CCalenCheckboxSettingItem::~CCalenCheckboxSettingItem()
    {
    TRACE_ENTRY_POINT;

    if ( iSelectionList )
        {
        iSelectionList->ResetAndDestroy();
        delete iSelectionList;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCheckboxSettingItem::EditItemL
// Edits the item
// ----------------------------------------------------------------------------
//
void CCalenCheckboxSettingItem::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    TRACE_ENTRY_POINT;

    CCalenSetting* setting = CCalenSetting::InstanceL();
    CleanupReleasePushL( *setting );

    if ( iSelectionList )
        {
        iSelectionList->ResetAndDestroy();
        delete iSelectionList;
        iSelectionList = NULL;
        }

    iSelectionList = new( ELeave )CSelectionItemList( 2 );

    // Assuming settings list is up to date (it should be).
    const CCalenCustomisationManager::CCalenPluginStatusArray& knownPlugins
                                               = setting->PluginAvailability();

    const RImplInfoPtrArray& allPlugins = iCustMan.PluginInfoArray();

    RArray<TUid> canBeEnabled;
    CleanupClosePushL( canBeEnabled );

    for ( TInt i( 0 ); i<knownPlugins.Count(); ++i )
        {
        const TUid pluginUid = knownPlugins[i].iUid;

        // Settings doesn't keep plugin names; they need to be retrieved from
        // the customisation manager.
        TInt implIndex = allPlugins.Find( pluginUid, PluginUidFinder );
        ASSERT( implIndex != KErrNotFound );

        TInt version = allPlugins[implIndex]->Version();
        TBool romBased = allPlugins[implIndex]->RomBased();
        // if the display name is empty and the plugin is in ROM the
        // plugin can  not be disabled
        if ( ( version != KDisablePluginEnablement ) || !romBased )
            {
             // CSelectableItem takes a non-const TDesC& but CImplementationInformation
            // only gives us a const reference. We'll have to make a copy :(
            const TDesC& displayName = allPlugins[implIndex]->DisplayName();
           
            HBufC* nameBuf = displayName.AllocLC();

            TBool enabled = knownPlugins[i].iEnabled;
            CSelectableItem* item
                        = new( ELeave )CSelectableItem( *nameBuf, enabled );
            CleanupStack::PushL( item );
            
            item->ConstructL();
            item->SetSelectionStatus( enabled );
            iSelectionList->AppendL( item );
            canBeEnabled.AppendL( pluginUid );
            
            CleanupStack::Pop( item );
            CleanupStack::PopAndDestroy( nameBuf );
            }     
        }

    CAknCheckBoxSettingPage* page
        = new( ELeave )CAknCheckBoxSettingPage( SettingPageResourceId(),
                                                iSelectionList );
    CleanupStack::PushL( page );
    SetSettingPage( page );
    SettingPage()->SetSettingPageObserver( this );
    SetUpStandardSettingPageL();
    CleanupStack::Pop( page );
    TBool success = SettingPage()->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );
    SetSettingPage( NULL );

    if ( success )
        {
        // knownPlugins is in the same order as iSelectionList.
        TInt selectionCount = iSelectionList->Count();
        for ( TInt index( 0 ); index < selectionCount; ++index )
            {
            TInt selection = iSelectionList->At( index )->SelectionStatus();
                                       
            iCustMan.SetPluginAvailabilityL( canBeEnabled[index], selection );
            
            const RImplInfoPtrArray& newPlugins = iCustMan.PluginInfoArray();
            if(newPlugins.Find(canBeEnabled[index],PluginUidFinder)!=KErrNotFound)
                {
                setting->SetPluginStatusL( canBeEnabled[index], selection );
                }
            
            }
        }
    
    CleanupStack::PopAndDestroy(); // canBeEnabled
    CleanupStack::PopAndDestroy( setting );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCheckboxSettingItem::PluginUidFinder
// Matches a uid and a plugin uid. Used by when finding a uid to match the
// setting's array against the customisation manager's array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenCheckboxSettingItem::PluginUidFinder( const TUid* aUid,
                                                  const CImplementationInformation& aArrayItem )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ( *aUid == aArrayItem.ImplementationUid() );
    }

// End of file
