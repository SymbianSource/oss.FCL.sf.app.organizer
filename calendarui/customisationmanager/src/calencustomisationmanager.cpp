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
* Description:  Handles loading of Calendar customisation plugins
*
*/


#include <eikmenup.h>
#include <ecom/ecom.h>
#include <Calendar.rsg>
#include <aknsettingitemlist.h>          // CAknSettingItem
#include <calencommandhandler.h>
#include <calencustomisation.h>
#include <calenservices.h>
#include <mcalenpreview.h>

#include "calensend.h"
#include "calenviewinfo.h"
#include "calendarui_debug.h"
#include "calencustomisationmanager.h"
#include "calencontroller.h"
#include "calensetting.h"
#include "CalenUid.h"


// ----------------------------------------------------------------------------
// CCalenCustomisationManager::NewL
// Two phased constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenCustomisationManager* CCalenCustomisationManager::NewL( 
                                    MCalenServicesFactory& aFactory,
                                    CCalenPluginStatusArray& aPluginSettingsStatus,
                                    MCalenServices& aServices,
                                    RPointerArray<CCalenViewInfo>& aViewArray )
    {
    TRACE_ENTRY_POINT;

    CCalenCustomisationManager* self = new( ELeave ) CCalenCustomisationManager(
                                                                 aFactory,
                                                                 aServices,
                                                                 aViewArray,
                                                                 aPluginSettingsStatus );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CCalenCustomisationManager
// C++ constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenCustomisationManager::CCalenCustomisationManager( MCalenServicesFactory& aFactory,
                                                        MCalenServices& aServices,
                                                        RPointerArray<CCalenViewInfo>& aViewArray,
                                                        CCalenPluginStatusArray& aPluginSettingsStatus )
    : iServicesFactory( aFactory ),
      iViewInfoArray( aViewArray ),
      iSortOrder( CCalenViewInfo::CyclePositionComparison ), 
      iServices( aServices ),
      iPluginSettingsStatus( aPluginSettingsStatus )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::ConstructL
// Second stage constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::ConstructL()
    {
    TRACE_ENTRY_POINT;
    iInfoBarProviderUid = TUid::Uid(0);
    iPreviewPaneProviderUid = TUid::Uid(0);
    
    // Get a pointer to the global data and controller
    iGlobalData = CCalenGlobalData::InstanceL();
    
    // Get the setting singleton. We update it when settings change.
    iSetting = CCalenSetting::InstanceL();
    
    CreateActivePluginListL();

    // Load any enabled plugins
    DoImmediatePluginLoadingL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::~CCalenCustomisationManager
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenCustomisationManager::~CCalenCustomisationManager()
    {
    TRACE_ENTRY_POINT;

    // Reset and destroy the contents of the owned arrays,
    // this will delete the plugins.
    iPlugins.ResetAndDestroy();

    iPluginInfo.ResetAndDestroy();

    iActivePlugins.Reset();
    iRomBasedPlugins.Reset();

    // reset deferred unload plugins list
    iDefferedUnloadPluginList.Reset();
	
	
    // Close the ECOM interface
    REComSession::FinalClose();

    // Release the global data and controller
    if ( iGlobalData )
        {
        iGlobalData->Release();
        }
    
    if(iSetting)
        {
        iSetting->Release();
        }
    iHiddenViews.Reset();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::GetCommandHandlerL
// Get a command handler for a specific command.  Ownership remains with the
// plugin
// ----------------------------------------------------------------------------
EXPORT_C MCalenCommandHandler* CCalenCustomisationManager::GetCommandHandlerL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;

    MCalenCommandHandler* handler( NULL );
    
    // Loop though all the enabled plugins asking for a command handler
    // to handle this command with.  Returns NULL if no plugin wants to 
    // handle the command.
    
    TInt count = iPlugins.Count();
    for ( TInt index = 0; index < count && !handler; ++index )
        {
        TRAPD( error,
            // Loop though until a command handler is found
            for (; index < count && !handler; ++index )
                {
                if ( !iPlugins[index]->IsDisabled() )
                    {
                    handler = iPlugins[index]->Plugin().CommandHandlerL( aCommand );
                    }
                }
             );

        if ( error )
            {
            // Report a problem with plugin.
            }
        }
    
    TRACE_EXIT_POINT;
    return handler;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::GetPluginSettingsL
// Gets custom settings from the specified plugin
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenCustomisationManager::GetPluginSettingsL( TUid aPluginUid,
                                                              RPointerArray<CAknSettingItem>& aSettingArray )
    {
    TRACE_ENTRY_POINT;

    CCalenCustomisation* plugin = FindPluginL( aPluginUid );
    User::LeaveIfNull( plugin );

    TRAPD( error, plugin->GetCustomSettingsL( aSettingArray ));
    if ( error )
            {
            // Report a problem with plugin.
            }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::ActivePlugins
// Returns a reference to an array of active plugins uids
// ----------------------------------------------------------------------------
//
EXPORT_C const RArray<TUid>& CCalenCustomisationManager::ActivePlugins() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return iActivePlugins;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CreateActivePluginListL
// Generates the list of active plugins
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::CreateActivePluginListL()
    {
    TRACE_ENTRY_POINT;
    
    iPluginInfo.ResetAndDestroy();
    iActivePlugins.Reset();
    iRomBasedPlugins.Reset();
    
    //Added resolver for corolla release
    // Set resolver params
    /*TLanguage lang = User::Language();
    TBuf8<40> langStr;
    langStr.Format( _L8("language(%d)"), lang );

    TEComResolverParams resolverParams;
    resolverParams.SetDataType( langStr );
    resolverParams.SetGenericMatch( ETrue );*/
    
    REComSession::ListImplementationsL( KCalenCustomisationInterfaceUid,/* resolverParams,*/ iPluginInfo );
    
    LoadPluginsBasedOnVariantL();
    
    TInt pluginCount = iPluginInfo.Count();
    
    for ( TInt pluginIndex(0); pluginIndex < pluginCount; ++pluginIndex )
        {
        TUid pluginUid = iPluginInfo[pluginIndex]->ImplementationUid();
        
        // Find the plugin in the list of known plugins 
        TInt position = iPluginSettingsStatus.Find( pluginUid, PluginAvailabilityFinder );
        if ( ( position == KErrNotFound ) || ( iPluginSettingsStatus[position].iEnabled ) )
            {
            // If the plugin can not be found or if it is enabled
            // add it to the list of active plugins
            iActivePlugins.AppendL( pluginUid );
            
            // If the plugin is rom-based, store its uid
            if ( iPluginInfo[pluginIndex]->RomBased() )
                {
                iRomBasedPlugins.AppendL( pluginUid );
                }
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::GetCustomViewsL
// GetCustomViews returns custom views info in an array for a given
// plugin. 
// NOTE: The contents of the array are owned by the caller - caller
// must guarantee to call ResetAndDestroy on the array in all cases.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenCustomisationManager::GetCustomViewsL( TUid aPluginUid, 
                                                           RPointerArray<CCalenView>& aViews )
    {
    TRACE_ENTRY_POINT;

    CCalenCustomisation* plugin = FindPluginL( aPluginUid );
    User::LeaveIfNull( plugin );
    
    TRAPD( error, plugin->GetCustomViewsL( aViews ));
    if ( error )
        {
        // Report a problem with plugin.
        }

    TInt numViews = aViews.Count();
    for ( TInt index( 0 ); index < numViews; ++index )
        {
        CCalenView* customView = aViews[index];

        // Add to view info array
        const TUid viewId = customView->Id();
        
         // Check for duplicate view ids. 
        TInt position = iViewInfoArray.Find( viewId , 
                                     CCalenViewInfo::ViewInfoIdentifier );
                                     
        if ( position == KErrNotFound )
            {
            const TDesC& menuName = customView->LocalisedViewNameL( CCalenView::EMenuName );
            const TDesC& settingsName = customView->LocalisedViewNameL( CCalenView::ESettingsName );
            const CCalenView::TCyclePosition cyclePos = customView->CyclePosition();

            CCalenViewInfo* viewInfo = CCalenViewInfo::NewL( viewId,
                                                             aPluginUid, 
                                                             menuName,
                                                             settingsName,
                                                             cyclePos );
                                                             
            // Check if the view has already been hidden.
            TBool hidden = ( iHiddenViews.Find( viewId.iUid ) != KErrNotFound );
            viewInfo->Hide( hidden );                     
                                                             
            ViewInfoArray().InsertInOrderAllowRepeatsL( viewInfo, iSortOrder );
            }
         else
            {
            // A plugin with the same id already exists. ( It could be a replacement
            // view provided by a rom plugin, as currently preventing rom plugins from
            // being disabled if required is not implemented, it needs to be handled)
            delete customView;
            aViews.Remove( index );
            index--; 
            numViews--;
            
            // Report a problem with plugin.
            }
         }
	RArray<TInt> hiddenViews;
    CleanupClosePushL( hiddenViews );
    plugin->RemoveViewsFromCycle( hiddenViews );
    TInt hiddenViewCount = hiddenViews.Count();
    if ( hiddenViewCount )
       {
       // Get the default view form the settings to check if it is going to be hidden
       // by the installed plugin
       TUid defaultView = iSetting->DefaultView();
       for ( TInt index(0); index < hiddenViewCount; ++index )
           {
           TInt hiddenViewId = hiddenViews[index];
          // iHiddenViews.InsertInOrderAllowRepeatsL( hiddenViewId );
                       
           // If this view is already on the view cycle list it needs
           // to be hidden.
           TUid hiddenViewUid = TUid::Uid( hiddenViewId );
                       
           // If the current default view is hidden, then set the newly
           // installed plugin view as default view
           if(defaultView == hiddenViewUid)
               {
               TInt position = iViewInfoArray.Find( aPluginUid, 
                                                    CCalenViewInfo::ViewPluginIdentifier );
               if(position != -1)
                   {
                   for(TInt i = position; i < iViewInfoArray.Count(); i++)
                       {
                       if((iViewInfoArray[i]->PluginUid() == aPluginUid) &&
                               (iViewInfoArray[i]->CyclePosition() != CCalenView::ENoCyclePosition))
                           {
                           iSetting->SetDefaultView(iViewInfoArray[i]->ViewUid());
                           break;
                           }
                       }
                   }
               }
           }
       }
   CleanupStack::PopAndDestroy(); // removedViews

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::SetPluginAvailabilityL
// Enables or disables the given plugin.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenCustomisationManager::SetPluginAvailabilityL( TUid aPluginUid,
                                                                  TBool aEnabled )
    {
    TRACE_ENTRY_POINT;
    
    if ( aEnabled )
        {
        EnablePluginL( aPluginUid );
        }
    else
        {
        DisablePluginL( aPluginUid );
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::DisablePluginL
// Disables the given plugin.
// ----------------------------------------------------------------------------
//  
 void CCalenCustomisationManager::DisablePluginL( TUid aPluginUid )
    {
    TRACE_ENTRY_POINT;

    // Find the plugins
    TInt index = iPlugins.Find( aPluginUid, CPluginInfo::Identifier );
    if ( index != KErrNotFound )
        {
        CPluginInfo* pluginInfo = iPlugins[index];
        
        // Check if the plugin has not already been disabled
        if ( !( pluginInfo->IsDisabled() ) )
            {
            // Mark the plugin as being disabled
            pluginInfo->Disable( ETrue );
             
            // Remove the plugin from the active plugin list
            TInt position = iActivePlugins.Find( aPluginUid );
            if(position == KErrNotFound ) // plugin has already been removed
                { 
                return ;
                }
            
            iActivePlugins.Remove( position );
             
            // Does this plugin hide any views
            RArray<TInt> removedViews;
            CleanupClosePushL( removedViews );
            CCalenCustomisation& plugin = pluginInfo->Plugin();
            plugin.RemoveViewsFromCycle( removedViews );
            TInt hiddenViewsCount = removedViews.Count();
            if ( hiddenViewsCount )
                {
                // Need to recalculate which views have been hidden, as 
                // more than one plugin might have hidden the same view.
                for ( TInt index( 0 ); index < hiddenViewsCount; ++index )
                    {
                    TUid viewUid = TUid::Uid( removedViews[index] );
                    CheckHiddenViewL( viewUid, aPluginUid );
                    }
                }

            // If the plugin does not offer any views, or the current
            // info or preview bar it can be deleted immediately,
            // otherwise the customisation needs to wait until it
            // is told it is safe to do so.

            // Does this plugin does not offer any views and the plugin
            // doesn't offer the current infobar or preview pane
            // bar it can be deleted immediately
            TInt offerViews = iViewInfoArray.Find( aPluginUid, 
                                       CCalenViewInfo::ViewPluginIdentifier );
                                              
            if ( offerViews == KErrNotFound &&
                 iInfoBarProviderUid != aPluginUid
                 && iPreviewPaneProviderUid != aPluginUid )
                {
                RArray<TUid> pluginArray;
                CleanupClosePushL( pluginArray );
                pluginArray.AppendL( aPluginUid );
                UnloadPluginsL( pluginArray );
                CleanupStack::PopAndDestroy(); // pluginArray
                }

            // Issue notification of plugin been disabled
            iPluginsEnabledDisabled = ETrue;
            iServices.IssueNotificationL( ECalenNotifyPluginEnabledDisabled );
            CleanupStack::PopAndDestroy(); // removedViews
            }
        }
        

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::UnloadPluginsL
// When a plugin is disabled it can not be unloaded until
// any views etc it has provided have been deleted. This function
// is called when it is safe to unload the plugin.
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenCustomisationManager::UnloadPluginsL( const RArray<TUid>& aPlugins )
    {
    TRACE_ENTRY_POINT;
    // Check plugin isn't already enabled
    TInt count = aPlugins.Count();
    for (TInt index = 0; index < count; ++index )
        {
        TInt position = iPlugins.Find( aPlugins[index], CPluginInfo::Identifier );
        if ( position != KErrNotFound ) 
	        {
            TUid pluginUid = iPlugins[ position]->Uid();
            if(!iPluginInfo[position]->RomBased())           
                {
                    if((iInfoBarProviderUid != iPlugins[position]->Uid())
                        && (iPreviewPaneProviderUid != iPlugins[ position]->Uid()))
                        {
                        CPluginInfo* removedPlugin = iPlugins[ position ];                   
                        iPlugins.Remove( position );
                        delete removedPlugin;
                        }
                    else 
                        {
                        iPlugins[position]->Disable(ETrue);
                        iDefferedUnloadPluginList.AppendL(iPlugins[position]->Uid());
                        iInfoBarProviderUid = KNullUid;
                        iPreviewPaneProviderUid = KNullUid;
                        }
                }     
		   }
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::EnablePluginL
// Enables the given plugin. Returns KErrNotFound if
// the plugin can't be found, KErrNone otherwise.
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::EnablePluginL( TUid aPluginUid )
    {
    TRACE_ENTRY_POINT;
    
    // Check plugin isn't already enabled
    TInt index = iPlugins.Find( aPluginUid, CPluginInfo::Identifier);
    
    if ( index == KErrNotFound )
        {
        // Load plugin
        TRAPD( error, LoadPluginL( aPluginUid ) );
        if ( error )
            {
            // report a problem loading the plugin
            
            }
            
        iActivePlugins.AppendL( aPluginUid );
    
        // Issue notification of plugin been added
        iPluginsEnabledDisabled = ETrue;
        iServices.IssueNotificationL( ECalenNotifyPluginEnabledDisabled );
        }
		else
    	{
		// Ensure plugin is enabled
		if(iPlugins[index]->IsDisabled())
			{
			iPlugins[index]->Disable( EFalse );
			iServices.IssueNotificationL( ECalenNotifyPluginEnabledDisabled );
			}
        }
    
    TRACE_EXIT_POINT;
    }
	
	
// ----------------------------------------------------------------------------
// CCalenCustomisationManager::LoadPluginsBasedOnVariant
// From MCalenNotificationHandler. Handles notifications.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::LoadPluginsBasedOnVariantL()
    {
    TRACE_ENTRY_POINT
    
    TLanguage lang = User::Language();
    
    // Based on the variant the plugins will be loaded
    // Eg. for 01 Euro regional plugins are not needed so all regional plugins will be removed 
    // other than non regional plugins.
    // for Chinese variant Thai and vietnamese plugins will be removed 
    // leaving chinese plugin and other non regional plugins.
    RArray<TUid> needsToBeRemoved;  
    
    switch(lang)
        {
        case ELangThai:
        case ELangEnglish_Thailand:
            needsToBeRemoved.Append(KCalenChineseImplUid);
            needsToBeRemoved.Append(KCalenVietnameseImplUid);
            break;
        case ELangTaiwanChinese:
        case ELangHongKongChinese:
        case ELangPrcChinese:
            needsToBeRemoved.Append(KCalenThaiImplUid);
            needsToBeRemoved.Append(KCalenVietnameseImplUid);
            break;
        case ELangVietnamese:
            needsToBeRemoved.Append(KCalenThaiImplUid);
            needsToBeRemoved.Append(KCalenChineseImplUid);
            break;
        default:
            needsToBeRemoved.Append(KCalenThaiImplUid);
            needsToBeRemoved.Append(KCalenChineseImplUid);
            needsToBeRemoved.Append(KCalenVietnameseImplUid);
            break;
        }
    
    if(needsToBeRemoved.Count())
        {
        for(TInt i = 0 ; i < needsToBeRemoved.Count() ; i++)
            {
            TInt index = iPluginInfo.Find(needsToBeRemoved[i],CCalenCustomisationManager::PluginInfoFinder);
            if( index != KErrNotFound )
                {
                CImplementationInformation* impl = iPluginInfo[index];
                iPluginInfo.Remove(index);
                delete impl;
                }
            }
        }
    
    needsToBeRemoved.Close();
    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::PluginInfoFinder(
// Matches an uid in pluginInfo.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenCustomisationManager::PluginInfoFinder( const TUid* aUid  , const CImplementationInformation&  aArrayItem)
    {
    TRACE_ENTRY_POINT; 
    TRACE_EXIT_POINT;
    return (*aUid  == aArrayItem.ImplementationUid() );
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::HandleNotificationL
// From MCalenNotificationHandler. Handles notifications.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    PIM_TRAPD_HANDLE( HandleNotificationL(aNotification) );
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenCustomisationManager::HandleNotificationL
// Handles notifications that the list of available plugins had been changed,
// either by GS or by the ecom registry.
// When plugins are enabled or disabled inside the calendar application, this
// function will be called after the plugin enablement changes have already been 
// made.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::HandleNotificationL(TCalenNotification aNotification)
    {
    TRACE_ENTRY_POINT;
    
     switch( aNotification )
        {
        case ECalenNotifyCheckPluginUnloading:
        	{
        	if(iDefferedUnloadPluginList.Count() > 0)
	        	{
	        	UnloadPluginsL(iDefferedUnloadPluginList);
	        	}
        	}
        	break;
        case ECalenNotifySettingsChanged:
	        {
		    // Make sure that any plugins marked as enabled are enabled, and
		    // any plugins marked as disabled are disabled.
		    iPluginsEnabledDisabled = EFalse;
		    TInt pluginCount = iPluginSettingsStatus.Count();
		    for ( TInt pluginIndex(0); pluginIndex < pluginCount; ++pluginIndex )
		        {
		        TUid pluginUid = iPluginSettingsStatus[pluginIndex].iUid;
		        TBool enabled = iPluginSettingsStatus[pluginIndex].iEnabled;
		 
		        SetPluginAvailabilityL( pluginUid, enabled );
		        }
		        
		    if ( iPluginsEnabledDisabled )
		        {
		        iServices.IssueNotificationL( ECalenNotifySettingsChanged );
		        }
	        }
	        break;
        case ECalenNotifyEComRegistryChanged:
            {
            CreateActivePluginListL();
            iSetting->UpdatePluginListL(*this);
            DoImmediatePluginLoadingL();
            iServices.IssueNotificationL(ECalenNotifyPluginEnabledDisabled);
            iServices.IssueNotificationL(ECalenNotifySettingsChanged );
            }
	  default:
            break; 
        }

	  TRACE_EXIT_POINT;
    }  

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::PluginInfoArray
// Returns a const reference to the plugin information array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C const RImplInfoPtrArray& CCalenCustomisationManager::PluginInfoArray() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iPluginInfo;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::Views
// Returns a const reference to the views array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<CCalenViewInfo>& CCalenCustomisationManager::Views() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iViewInfoArray;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::DoImmediatePluginLoadingL
// Examines the properties of the plugin list and
// loads those that need to loaded on start-up
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::DoImmediatePluginLoadingL()
    {
    TRACE_ENTRY_POINT;

    
    for(TInt i = 0 ; i < iPlugins.Count() ; i++)
        {
        RArray<TUid> pluginArray;
        CleanupClosePushL( pluginArray );
        pluginArray.AppendL( iPlugins[i]->Uid() );
        UnloadPluginsL( pluginArray );
        CleanupStack::PopAndDestroy(); // pluginArray
        }
    
    // Get the number plugins
    TInt pluginCount = iPluginInfo.Count();

    for ( TInt index( 0 ); index < pluginCount; ++index )
        {
        TUid pluginUid = iPluginInfo[index]->ImplementationUid();
        
        if ( iActivePlugins.Find( pluginUid ) != KErrNotFound )        
        
            {
            TRAPD( error, LoadPluginL( pluginUid ) );
            if ( error )
                {
                // report a problem loading the plugin
                
                }
            }
        }
   
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::LoadPluginL
// Loads a plugin
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::LoadPluginL( TUid aPluginUid )
    {
    TRACE_ENTRY_POINT;

    // Get a new services object from global data
    MCalenServices* services = iServicesFactory.NewServicesL();
    
    // Creates the plugin and transfers ownership of the services
    // object to the plugin.
    CCalenCustomisation* plugin = 
        CCalenCustomisation::CreateImplementationL( aPluginUid, services );
    
   CleanupStack::PushL( plugin );

    // the plugin array takes ownership of the plugin
    AddPluginL( plugin, aPluginUid  );
    CleanupStack::Pop( plugin );

        
    RArray<TInt> hiddenViews;
    CleanupClosePushL( hiddenViews );
    plugin->RemoveViewsFromCycle( hiddenViews );
    TInt hiddenViewCount = hiddenViews.Count();
    if ( hiddenViewCount )
        {
        // Get the default view form the settings to check if it is going to be hidden
        // by the installed plugin
        TUid defaultView = iSetting->DefaultView();
        for ( TInt index(0); index < hiddenViewCount; ++index )
            {
            TInt hiddenViewId = hiddenViews[index];
            iHiddenViews.InsertInOrderAllowRepeatsL( hiddenViewId );
            
            // If this view is already on the view cycle list it needs
            // to be hidden.
            TUid hiddenViewUid = TUid::Uid( hiddenViewId );
            
            // If the current default view is hidden, then set the newly
            // installed plugin view as default view
            if(defaultView == hiddenViewUid)
            	iSetting->SetDefaultView(aPluginUid);
            
            TInt position = iViewInfoArray.Find( hiddenViewUid, 
                                     CCalenViewInfo::ViewInfoIdentifier );
                                     
            if ( position != KErrNotFound )
                {
                CCalenViewInfo* viewInfo = iViewInfoArray[position];
                viewInfo->Hide( ETrue );
                
                // By hiding the view, the cycleposition order may be changed
                // so the view info array needs to be resorted.
                iViewInfoArray.Sort( iSortOrder );
                }
            }
        }
    CleanupStack::PopAndDestroy(); // removedViews

    TRACE_EXIT_POINT;
    }

 // ----------------------------------------------------------------------------
// CCalenCustomisationManager::OfferMenuPaneL
// Offers a menupane for customisation by plugins
// and the controller.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenCustomisationManager::OfferMenuPaneL( TInt aResourceId,
                                                          CEikMenuPane* aMenuPane )
    {
    TRACE_ENTRY_POINT;

    // Offer the menu to registered plugins
    OfferMenuPaneToPluginsL( aResourceId, aMenuPane );

    // If the menupane is a main menu, a check needs to be made that the
    // exit and switch view commands exist

    // Replace the send menu if it exists
    TInt position( 0 );
    if ( aMenuPane->MenuItemExists( ECalenSend, position) )
        {
        // Delete the existing send menu option
        aMenuPane->DeleteMenuItem(ECalenSend);

        // Insert the calensend menu item
        iGlobalData->CalenSendL().DisplaySendMenuItemL( *aMenuPane, position );
        } 

    // If the menu has a view switch menu item, it is up to the 
    // customisation manager to decide if the menu should be 
    // cascading or a selection list box. 
    if ( aMenuPane->MenuItemExists( ECalenSwitchView, position) )
        {
        CEikMenuPaneItem::SData& switchItem =  aMenuPane->ItemData( ECalenSwitchView );
        if ( iActivePlugins.Count() == 0 )
            {
            // Create a cascading menu
            switchItem.iCascadeId = R_CALENDAR_CHANGE_VIEW_MENUPANE;
            }
        else
            {
            switchItem.iCascadeId = 0;
            }
        }

    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CheckHiddenViewL(
// A check needs to be made to see if any other plugins hide the same view.
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::CheckHiddenViewL( TUid aViewUid, TUid aPluginUid )
    {
    TRACE_ENTRY_POINT;
    
    // Find the viewId in the hidden view list, and remove it
    TInt viewId = aViewUid.iUid;
    TInt position = iHiddenViews.Find( viewId );
    if ( position != KErrNotFound )
        {
        iHiddenViews.Remove( position );
        
        // Try to find the view again, in case it is hidden by another view
        position = iHiddenViews.Find( viewId );
        if ( position == KErrNotFound )
            {
            // If it can't be found in the hidden view list,
            // it can be unhidden. 
            TInt viewInfoPos = iViewInfoArray.Find( aViewUid , 
                                     CCalenViewInfo::ViewInfoIdentifier );
                                     
            if ( viewInfoPos != KErrNotFound )
                {
                // Unhide the view
                CCalenViewInfo* viewInfo = iViewInfoArray[viewInfoPos];
                viewInfo->Hide( EFalse );
                
                // Set hidden view as default view if the currently disabled plugin
                // is the default view
                TInt position = iViewInfoArray.Find( aPluginUid, 
                                                           CCalenViewInfo::ViewPluginIdentifier );
                if(position != -1)
                  {
                  for(TInt i = position; i < iViewInfoArray.Count(); i++)
                      {
                      if((iViewInfoArray[i]->PluginUid() == aPluginUid) &&
                              iViewInfoArray[i]->ViewUid() == iSetting->DefaultView())
                          {
                          iSetting->SetDefaultView(aViewUid);
                          break;
                          }
                      }
                  }
                                
                // By unhiding the view, the cycleposition order may be changed
                // so the view info array needs to be resorted.
                iViewInfoArray.Sort( iSortOrder );
                }
            }
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::GetReplacePluginViewIdL
// Returns the view ID of the plugin that hides a particular view
// ----------------------------------------------------------------------------
//
EXPORT_C TUid CCalenCustomisationManager::GetReplacePluginViewIdL( TUid aHiddenViewId )
	{
	TRACE_ENTRY_POINT;
	
	RArray<TInt> removedViews;
	CCalenCustomisation* plugin = NULL;
	TInt i = 0;
	for( ;i < iActivePlugins.Count(); i++)
	{
        plugin = FindPluginL(iActivePlugins[i]);
		plugin->RemoveViewsFromCycle(removedViews);
		if(removedViews.Count())
		{
			TInt position = removedViews.Find(aHiddenViewId.iUid);
			if(position != KErrNotFound)
			    break;
			else
				continue;
		    }
		else
		    {
			continue;
		    }	
	    }
	removedViews.Reset();
	
	// Get all the views provided by the plugin
	//plugin->GetCustomViewsL()
	TInt position = iViewInfoArray.Find( iActivePlugins[i], 
	                                       CCalenViewInfo::ViewPluginIdentifier );
	while((position < iViewInfoArray.Count()) && ((iViewInfoArray[position]->CyclePosition() == CCalenView::ENoCyclePosition) ||
	        (iActivePlugins[i] != (iViewInfoArray[position]->PluginUid()))))
	    {
	    // go to the next view position
	    position++;
	    }
    TRACE_EXIT_POINT;   

    return (iViewInfoArray[position]->ViewUid());
	}

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::GetReplacePluginViewIdL
// Returns the hidden views list 
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenCustomisationManager::GetHiddenViewIdL( TUid aPluginUid, RArray<TInt>& aHiddenViews )
	{
	TRACE_ENTRY_POINT;
	
	// Get the plugin instance
	CCalenCustomisation* plugin = FindPluginL(aPluginUid);
	
	plugin->RemoveViewsFromCycle(aHiddenViews);
	
	TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CustomPreviewPaneL
// Returns the preview pane
// (other items were commented in a header).
// ----------------------------------------------------------------------------
EXPORT_C MCalenPreview* CCalenCustomisationManager::CustomPreviewPaneL( TRect& aRect )
	{
	MCalenPreview* previewPane = NULL;
   
   
    // Loop though all the command handlers, within two interlinked
    // for loops. If PreviewPaneL leaves for one plugin, a plugin
    // error message will be displayed and the loop will continue with
    // the next command handler. If none of the plugins leave, there will
    // be only one TRAP used.
    TInt count = iPlugins.Count();
    for ( TInt index = 0; index < count && !previewPane; ++index )
        {
        TRAPD( error,
                {
                 // Loop though until an infobar is found
                for (; index < count && !previewPane; ++index )
                    {
                    if ( !iPlugins[index]->IsDisabled() )
                        {
                        previewPane = iPlugins[index]->Plugin().CustomPreviewPaneL( aRect );
                        }
                    }
                }
             );
        if ( error )
            {
            // Report a problem with plugin.
            }
        }
    
    TRACE_EXIT_POINT;
    return previewPane;
	}
// ----------------------------------------------------------------------------
// CCalenCustomisationManager::PreviewPane
// Returns the preview pane
// (other items were commented in a header).
// ----------------------------------------------------------------------------
EXPORT_C CCoeControl* CCalenCustomisationManager::PreviewPane(   TRect& aRect  )
    {
    TRACE_ENTRY_POINT;
	
    iInfoBarProviderUid = TUid::Uid(0);;		
    iPreviewPaneProviderUid = TUid::Uid(0);
    
    CCoeControl* previewPane = NULL;
   
    // Loop though all the command handlers, within two interlinked
    // for loops. If PreviewPaneL leaves for one plugin, a plugin
    // error message will be displayed and the loop will continue with
    // the next command handler. If none of the plugins leave, there will
    // be only one TRAP used.
    TInt count = iPlugins.Count();
    for ( TInt index = 0; index < count && !previewPane; ++index )
        {
        TRAPD( error,
            // Loop though until an infobar is found
            for (; index < count && !previewPane; ++index )
                {
                
                if ( !iPlugins[index]->IsDisabled() )
                    {
                    previewPane = iPlugins[index]->Plugin().PreviewPaneL( aRect );
                    if(previewPane)
						{
                    	iPreviewPaneProviderUid = iPlugins[index]->Uid();
						}
                    }
                
                }
             );

        if ( error )
            {
            // Report a problem with plugin.
            }
        }
    
    TRACE_EXIT_POINT;
    return previewPane;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::Infobar
// Returns the infobar.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
EXPORT_C CCoeControl* CCalenCustomisationManager::Infobar(const  TRect& aRect )
    {
    TRACE_ENTRY_POINT;

    iInfoBarProviderUid = TUid::Uid(0);
    iPreviewPaneProviderUid = TUid::Uid(0);
    
    CCoeControl* infoBar = NULL;
    // Loop though all the command handlers, within two interlinked
    // for loops. If GetInfoBarL leaves for one plugin, a plugin
    // error message will be displayed and the loop will continue with
    // the next command handler. If none of the plugins leave, there will
    // be only one TRAP used.
    TInt count = iPlugins.Count();
    for ( TInt index = 0; index < count && !infoBar; ++index )
        {
        TRAPD( error,
            // Loop though until an infobar is found
            for (; index < count && !infoBar; ++index )
                {
                if ( !iPlugins[index]->IsDisabled() )
                    {
                    infoBar = iPlugins[index]->Plugin().InfobarL( aRect  );
                    iInfoBarProviderUid = iPlugins[index]->Uid();
                    }
                }
             );

        if ( error )
            {
            // Report a problem with plugin.
            }
        }
        
    TRACE_EXIT_POINT;   
    return infoBar;
    }
    
// ----------------------------------------------------------------------------
// CCalenCustomisationManager::Infobar
// Returns the infobar.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
EXPORT_C const TDesC& CCalenCustomisationManager::Infobar()
    {
    TRACE_ENTRY_POINT;

    iInfoBarProviderUid = TUid::Uid(0);
    iPreviewPaneProviderUid = TUid::Uid(0);
    
    iInfoBarStr.Set(KNullDesC);
    // Loop though all the command handlers, within two interlinked
    // for loops. If GetInfoBarL leaves for one plugin, a plugin
    // error message will be displayed and the loop will continue with
    // the next command handler. If none of the plugins leave, there will
    // be only one TRAP used.
    TBool infoBarFound = EFalse;
    TInt count = iPlugins.Count();
    for ( TInt index = 0; index < count && !infoBarFound; ++index )
        {
        TRAPD( error,
            // Loop though until an infobar is found
            for (; index < count && !infoBarFound; ++index )
                {
                if ( !iPlugins[index]->IsDisabled() )
                    {
                    iInfoBarStr.Set( iPlugins[index]->Plugin().InfobarL() );
                    if(iInfoBarStr.Length())
                        {
                        infoBarFound = ETrue;
                        }
                    iInfoBarProviderUid = iPlugins[index]->Uid();
                    }
                }
             );

        if ( error )
            {
            // Report a problem with plugin.
            }
        }
        
    TRACE_EXIT_POINT;   
    return iInfoBarStr;
    }
    
// ----------------------------------------------------------------------------
// CCalenCustomisationManager::HiddenView
// Return ETrue if a view has been hidden by a plugin
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//         
EXPORT_C TBool CCalenCustomisationManager::HiddenView( TUid aHiddenView) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ( iHiddenViews.Find( aHiddenView.iUid ) != KErrNotFound );
    }
    
    
// ----------------------------------------------------------------------------
// CCalenCustomisationManager::RomBased(
// Return ETrue if a view has been provided by a rom-based plugin
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//  
EXPORT_C TBool CCalenCustomisationManager::IsViewRomBased( TUid aViewUid ) const
    {
    TRACE_ENTRY_POINT;
    
    TBool isRomBased = EFalse;
    
    TInt viewInfoPos = iViewInfoArray.Find( aViewUid , 
                                     CCalenViewInfo::ViewInfoIdentifier );
    if ( viewInfoPos != KErrNotFound )
        {
        TUid plugUid = iViewInfoArray[viewInfoPos]->PluginUid();
        TInt position = iRomBasedPlugins.Find( plugUid );
        if ( position != KErrNotFound )
            {
            isRomBased = ETrue;
            }
        }
    
    TRACE_EXIT_POINT;
    return isRomBased;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CanBeEnabledDisabled(
// Return ETrue if plugins can be enabled/disabled from settings
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenCustomisationManager::CanBeEnabledDisabledL(TUid aPluginUid)
    {
    TRACE_ENTRY_POINT;
    TBool enabledDisabled = EFalse;
    
    CCalenCustomisation* plugin = FindPluginL(aPluginUid);
    if(plugin)
		{
        enabledDisabled = plugin->CanBeEnabledDisabled();
		}
    
    TRACE_EXIT_POINT;
    return enabledDisabled;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::OfferMenuPaneL
// Offers a menupane to all interested plugins
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::OfferMenuPaneToPluginsL( TInt aResourceId,
                                                          CEikMenuPane* aMenuPane )
    {
    TRACE_ENTRY_POINT;

    // Get the number of command handlers
 //   TInt count = iCommandHandlers.Count();
    const TInt count = iPlugins.Count();

    // Loop though all the command handlers, within two interlinked
    // for loops. If customiseMenuPane leaves for one plugin, a plugin
    // error message will be displayed and the loop will continue with
    // the next command handler. If none of the plugins leave, there will
    // be only one TRAP used.
    for ( TInt index = 0; index < count; ++index )
        {
        TRAPD( error,
            for (; index < count; ++index )
                {
                if ( !iPlugins[index]->IsDisabled() )
                    {
                iPlugins[index]->Plugin().CustomiseMenuPaneL( aResourceId,
                                                             aMenuPane );
                                                             
                // The commands added should be checked to see that
                // they match the expected command range for the plugin
                    }
                }
             );
         
        if ( error )
            {
            // Report a problem with plugin.
            }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::FindPluginL
// Finds a plugin and returns a plugin with a given uid, returns NULL
// if the uid can not be found.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenCustomisation* CCalenCustomisationManager::FindPluginL( TUid aUid )
    {
    TRACE_ENTRY_POINT;

    TInt index = iPlugins.Find( aUid, CPluginInfo::Identifier );
    
    CCalenCustomisation* plugin = NULL;
    if ( index != KErrNotFound )
        {
        plugin = &( iPlugins[index]->Plugin() );
        }

    TRACE_EXIT_POINT; 
    return plugin;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CPluginInfo::CPluginInfo(
// C++ constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenCustomisationManager::CPluginInfo::CPluginInfo( 
                                    CCalenCustomisation* aPlugin,
                                    TUid aUid )
    : iPlugin( aPlugin ),
      iUid( aUid )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CPluginInfo::~CPluginInfo(
// C++ destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenCustomisationManager::CPluginInfo::~CPluginInfo() 
    {
    TRACE_ENTRY_POINT;

    delete iPlugin;

    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CPluginInfo::Plugin
// returns a reference to the plugin
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenCustomisation& CCalenCustomisationManager::CPluginInfo::Plugin()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return *iPlugin;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CPluginInfo::Uid
// returns the plugin uid
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 
TUid CCalenCustomisationManager::CPluginInfo::Uid() const 
    {
    TRACE_ENTRY_POINT; 
    TRACE_EXIT_POINT;

    return iUid;
    }
   
// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CPluginInfo::Disable
// Marks the plugin as being disabled
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//  
void CCalenCustomisationManager::CPluginInfo::Disable( TBool aDisable)
    {
    TRACE_ENTRY_POINT;
    
    iDisabled = aDisable;
     
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CPluginInfo::IsDisabled
// Returns if the plugin has been marked as being disabled
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//  
TBool CCalenCustomisationManager::CPluginInfo::IsDisabled()
    {
    TRACE_ENTRY_POINT;
    
    return iDisabled;
     
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::AddPluginL
// Adds a plugin to the array of plugins
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//           
void CCalenCustomisationManager::AddPluginL( CCalenCustomisation* aPlugin,
                                             TUid aUid ) 
    {
    TRACE_ENTRY_POINT;
    TInt index = iPlugins.Find( aUid, CPluginInfo::Identifier );
    if ( index == KErrNotFound )
        {
    CPluginInfo* newPlugin = new ( ELeave ) CPluginInfo( aPlugin, aUid);
    CleanupStack::PushL( newPlugin );
    iPlugins.AppendL( newPlugin );
    CleanupStack::Pop( newPlugin );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::CPluginInfo::Identifier(
// Matches an uid and a plugin uid. Used by FindPluginL to
// find a plugin with a given uid. 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenCustomisationManager::CPluginInfo::Identifier( const TUid* aUid,
                                                           const CPluginInfo& aArray )
    {
    TRACE_ENTRY_POINT; 
    TRACE_EXIT_POINT;

    return ( *aUid == aArray.Uid() ); 
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::ViewInfoArray
// returns a reference to the view info array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------      
//
RPointerArray<CCalenViewInfo>& CCalenCustomisationManager::ViewInfoArray()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iViewInfoArray;
    }    
    
// ----------------------------------------------------------------------------
// CCalenCustomisationManager::PluginAvailabilityFinder
// Matches an uid and a plugin uid. Used by when finding the disabled plugins
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//  
TBool CCalenCustomisationManager::PluginAvailabilityFinder( const TUid* aUid, 
                                 const TCalenPluginAvailability& aArrayItem )
    {
    TRACE_ENTRY_POINT; 
    TRACE_EXIT_POINT;
    
    return ( *aUid == aArrayItem.iUid); 
    }    

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::DoPluginLoadingL
// Load all plugins 
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenCustomisationManager::DoPluginLoadingL()
    {
    TRACE_ENTRY_POINT;
    // Reset and destroy the contents of the owned arrays    
    //iPlugins.ResetAndDestroy();
    iHiddenViews.Reset();
    iDefferedUnloadPluginList.Reset();
    // create active plugin list
    CreateActivePluginListL();
    
    DoImmediatePluginLoadingL();
    iSetting->LoadL();
    iSetting->UpdatePluginListL(*this);
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::DisableAllPluginsL
// Disable all plugins 
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenCustomisationManager::DisableAllPluginsL()
    {
    TRACE_ENTRY_POINT;
    
    TInt pluginCount = iPluginInfo.Count();
    for(TInt index = 0;index<pluginCount;index++)
        {
        TUid pluginUid = iPluginInfo[index]->ImplementationUid();
        if ((iActivePlugins.Find(pluginUid) != KErrNotFound)
                && !(iRomBasedPlugins.Find(pluginUid) != KErrNotFound))
            {
            DisablePluginOnFakeExitL(pluginUid);
            }
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::DisablePluginOnFakeExitL
// Disable plugin on fake exit
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::DisablePluginOnFakeExitL(TUid aPluginUid)
    {
    TRACE_ENTRY_POINT;
    
    TInt index = iPlugins.Find( aPluginUid, CPluginInfo::Identifier );
    if ( index != KErrNotFound )
        {
        CPluginInfo* pluginInfo = iPlugins[index];
    
        // Remove the plugin from the active plugin list
        TInt position = iActivePlugins.Find( aPluginUid );
        ASSERT( position != KErrNotFound );
    
        if(iActivePlugins.Count())
            {
            iActivePlugins.Remove( position );            
            }

        // Does this plugin hide any views
        RArray<TInt> removedViews;
        CleanupClosePushL( removedViews );
        CCalenCustomisation& plugin = pluginInfo->Plugin();
        plugin.RemoveViewsFromCycle( removedViews );
        TInt hiddenViewsCount = removedViews.Count();
        if ( hiddenViewsCount )
            {
            // Need to recalculate which views have been hidden, as 
            // more than one plugin might have hidden the same view.
            for ( TInt index( 0 ); index < hiddenViewsCount; ++index )
                {
                TUid viewUid = TUid::Uid( removedViews[index] );
                CheckHiddenViewL( viewUid, aPluginUid );
                }
            }
    
        // If the plugin does not offer any views, or the current
        // info or preview bar it can be deleted immediately,
        // otherwise the customisation needs to wait until it
        // is told it is safe to do so.
    
        // Does this plugin does not offer any views and the plugin
        // doesn't offer the current infobar or preview pane
        // bar it can be deleted immediately
        TInt offerViews = iViewInfoArray.Find( aPluginUid, 
                                   CCalenViewInfo::ViewPluginIdentifier );
                                      
        if ( offerViews == KErrNotFound &&
             iInfoBarProviderUid != aPluginUid
             && iPreviewPaneProviderUid != aPluginUid )
            {
            RArray<TUid> pluginArray;
            CleanupClosePushL( pluginArray );
            pluginArray.AppendL( aPluginUid );
            UnloadPluginsL( pluginArray );
            CleanupStack::PopAndDestroy(); // pluginArray
            }
    
        // Issue notification of plugin been disabled
        iPluginsEnabledDisabled = ETrue;
        iServices.IssueNotificationL( ECalenNotifyPluginEnabledDisabled );
        CleanupStack::PopAndDestroy(); // removedViews
        }
    TRACE_EXIT_POINT;
    }

// End of File

