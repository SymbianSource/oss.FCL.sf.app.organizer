/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

//User include
#include "calenbooleansettingitem.h"
#include "calencustomisationmanager.h"
#include "calensetting.h"

//System include
#include <akncheckboxsettingpage.h>
#include <bautils.h>
#include <StringLoader.h>
#include <eikenv.h>
#include <data_caging_path_literals.hrh>
#include <Calendar.rsg>




// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CCalenBooleanSettingItem::CCalenBooleanSettingItem
// Constructor.Phase One
// ----------------------------------------------------------------------------
//
CCalenBooleanSettingItem::CCalenBooleanSettingItem(TInt aId,CCalenCustomisationManager& aCustMan)
    : CAknBinaryPopupSettingItem( aId , iInternalValue),
      iCustMan( aCustMan )
    
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenBooleanSettingItem::NewLC
// Constructor.
// ----------------------------------------------------------------------------
//
CCalenBooleanSettingItem* CCalenBooleanSettingItem::NewLC(TInt aId,
                                                              TInt aOridinal,
                                                              CCalenCustomisationManager& aCustMan)
    {
    TRACE_ENTRY_POINT
    CCalenBooleanSettingItem* self = new(ELeave) CCalenBooleanSettingItem(aId,aCustMan);
    CleanupStack::PushL(self);
    self->ConstructL(aOridinal);
    TRACE_EXIT_POINT
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenBooleanSettingItem::ConstructL
// Constructor.Phase Two
// ----------------------------------------------------------------------------
//
void CCalenBooleanSettingItem::ConstructL(TInt aOrdinal)
    {
    TRACE_ENTRY_POINT
    CCalenSetting* setting = CCalenSetting::InstanceL();
    CleanupReleasePushL( *setting );
        
        
    // Assuming settings list is up to date (it should be).
    const CCalenCustomisationManager::CCalenPluginStatusArray& knownPlugins 
                                                       = setting->PluginAvailability();
    const RImplInfoPtrArray& allPlugins = iCustMan.PluginInfoArray();
    
    for ( TInt i( 0 ); i < knownPlugins.Count(); ++i )
        {
        const TUid pluginUid = knownPlugins[i].iUid;

        // Settings doesn't keep plugin names; they need to be retrieved from
        // the customisation manager.
        TInt implIndex = allPlugins.Find( pluginUid, PluginUidFinder );
        if(implIndex != KErrNotFound)
            {
            HBufC* displayName = allPlugins[implIndex]->DisplayName().AllocLC();
        
            //Construct the boolean setting item
            //R_CALEN_REGIONAL_SETTING_TEXTS -> On/Off array
            CAknSettingItem::ConstructL(EFalse, aOrdinal, *displayName, NULL, 
                                                   R_CALEN_LUNAR_SETTING_PAGE, EAknCtPopupSettingList, 
                                                   NULL, R_CALEN_REGIONAL_SETTING_TEXTS);
            
            
            CleanupStack::PopAndDestroy( displayName );
            }
        }
    
    CleanupStack::PopAndDestroy( setting );
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenBooleanSettingItem::~CCalenBooleanSettingItem
// Destructor
// ----------------------------------------------------------------------------
//
CCalenBooleanSettingItem::~CCalenBooleanSettingItem()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCheckboxSettingItem::EditItemL
// Edits the item
// ----------------------------------------------------------------------------
//
void CCalenBooleanSettingItem::EditItemL( TBool aCalledFromMenu)
    {
    TRACE_ENTRY_POINT;
    
    CCalenSetting* setting = CCalenSetting::InstanceL();
    CleanupReleasePushL( *setting );
    
    
    // Assuming settings list is up to date (it should be).
    const CCalenCustomisationManager::CCalenPluginStatusArray& knownPlugins
                                               = setting->PluginAvailability();

    const RImplInfoPtrArray& allPlugins = iCustMan.PluginInfoArray();

    RArray<TUid> canBeEnabled;
    CleanupClosePushL( canBeEnabled );
    
    for ( TInt i( 0 ); i < knownPlugins.Count(); ++i )
        {
        const TUid pluginUid = knownPlugins[i].iUid;

        // Settings doesn't keep plugin names; they need to be retrieved from
        // the customisation manager.
        TInt implIndex = allPlugins.Find( pluginUid, PluginUidFinder );
        ASSERT( implIndex != KErrNotFound );
        canBeEnabled.AppendL( pluginUid );
        }
    
    //Pass to base class to toggle the item
    CAknBinaryPopupSettingItem::EditItemL(aCalledFromMenu);
    
    //Get the toggle item On/Off status to set the plugin status
    TBool internalValue = InternalValue();
    
    iCustMan.SetPluginAvailabilityL( canBeEnabled[0], internalValue );
    setting->SetPluginStatusL( canBeEnabled[0], internalValue );
    
 
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
TBool CCalenBooleanSettingItem::PluginUidFinder( const TUid* aUid,
                                                  const CImplementationInformation& aArrayItem )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ( *aUid == aArrayItem.ImplementationUid() );
    }

// ----------------------------------------------------------------------------
// CCalenBooleanSettingItem::StoreL
// ----------------------------------------------------------------------------
//
void CCalenBooleanSettingItem::StoreL()
    {
    TRACE_ENTRY_POINT;
    CAknBinaryPopupSettingItem::StoreL();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenBooleanSettingItem::LoadL
// ----------------------------------------------------------------------------
//
void CCalenBooleanSettingItem::LoadL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenSetting* setting = CCalenSetting::InstanceL();
    CleanupReleasePushL( *setting );
    const CCalenCustomisationManager::CCalenPluginStatusArray& knownPlugins
                                                  = setting->PluginAvailability();
   
    TBool enabled = EFalse;
    if(knownPlugins.Count() > 0)
       {
       enabled = knownPlugins[0].iEnabled;
       }
    iInternalValue = enabled;
    CAknBinaryPopupSettingItem::LoadL();
    
    CleanupStack::PopAndDestroy( setting );
    TRACE_EXIT_POINT;
    }
// End of file


