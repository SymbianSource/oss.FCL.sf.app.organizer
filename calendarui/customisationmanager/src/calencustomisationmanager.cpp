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



#include <vwsdef.h>                     // TVwsViewId
#include <apadef.h>                     // TApaCommand
#include "calendarui_debug.h"
#include <hbwidget.h>
#include <hblabel.h>
#include "calencustomisationmanager.h"
#include "calencommandhandler.h"
#include "calenservices.h"
#include "calencustomisation.h"
#include <ecom/ecom.h>
#include <e32lang.h>
#include "calenservices.h"
#include "calenservicesfactory.h"
#include "CalenUid.h"




// ----------------------------------------------------------------------------
// CCalenCustomisationManager::NewL
// Two phased constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
 CCalenCustomisationManager* CCalenCustomisationManager::NewL( 
                                    MCalenServicesFactory& aFactory,
                                    MCalenServices& aServices
                                    )
    {
    TRACE_ENTRY_POINT;

    CCalenCustomisationManager* self = new( ELeave ) CCalenCustomisationManager(
                                                                 aFactory,
                                                                 aServices );
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
                                                        MCalenServices& aServices )
    : iServicesFactory( aFactory ),       
      iServices( aServices )
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
 CCalenCustomisationManager::~CCalenCustomisationManager()
    {
    TRACE_ENTRY_POINT;

    // Reset and destroy the contents of the owned arrays,
    // this will delete the plugins.
    iPlugins.ResetAndDestroy();

    iPluginInfo.ResetAndDestroy();

    iActivePlugins.Reset();
    iRomBasedPlugins.Reset();

    // Close the ECOM interface
    REComSession::FinalClose();
    
    iDefferedUnloadPluginList.Reset();

    TRACE_EXIT_POINT;
    }
 // ----------------------------------------------------------------------------
 // CCalenCustomisationManager::GetCommandHandlerL
 // Get a command handler for a specific command.  Ownership remains with the
 // plugin
 // ----------------------------------------------------------------------------
 MCalenCommandHandler* CCalenCustomisationManager::GetCommandHandlerL( TInt aCommand )
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
                 //if ( !iPlugins[index]->IsDisabled() )
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
// CCalenCustomisationManager::ActivePlugins
// Returns a reference to an array of active plugins uids
// ----------------------------------------------------------------------------
//
 const RArray<TUid>& CCalenCustomisationManager::ActivePlugins() const
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

    REComSession::ListImplementationsL( KCalenCustomisationInterfaceUid, iPluginInfo );
    
    LoadPluginsBasedOnVariantL();
    
    TInt pluginCount = iPluginInfo.Count();
    
    if(pluginCount) 
		{
    	for ( TInt pluginIndex(0); pluginIndex < pluginCount; ++pluginIndex )
			{
    		TUid pluginUid = iPluginInfo[pluginIndex]->ImplementationUid();

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
// CCalenCustomisationManager::LoadPluginsBasedOnVariantL
// Loads the plugin based on the language variant
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::LoadPluginsBasedOnVariantL()
{
	TLanguage languge = User::Language();
	RArray<TUid> needsToBeRemoved;
	
	switch(languge) 
		{
		case ELangTaiwanChinese:
		case ELangPrcChinese:
		case ELangHongKongChinese:
			// TODO: Remove the other regional plugins Thai, Vietnamese etc.
			break;
		case ELangThai:
		case ELangEnglish_Thailand:
			needsToBeRemoved.Append(KCalenChineseImplUid);
			// TODO: Remove the other regional plugin Vietnamese.
			break;
		case ELangVietnamese:
			needsToBeRemoved.Append(KCalenChineseImplUid);
			// TODO: Remove the other regional plugin Thai.
			break;

		default:
			needsToBeRemoved.Append(KCalenChineseImplUid);
			// TODO: Remove the other regional plugins Thai, Vietnamese etc.
			break;
		}
	TInt count = needsToBeRemoved.Count();
	if(count) 
		{
		for(TInt i=0; i< count; i++) 
			{
			TInt index = iPluginInfo.Find(needsToBeRemoved[i],
								CCalenCustomisationManager::PluginInfoFinder);
			if( index != KErrNotFound ) 
				{
				CImplementationInformation* impl = iPluginInfo[index];
				iPluginInfo.Remove(index);
				delete impl;
				}
			}
		}
	needsToBeRemoved.Close();
}

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::PluginInfoFinder(
// Matches an uid in pluginInfo.
// ----------------------------------------------------------------------------
//
TBool CCalenCustomisationManager::PluginInfoFinder( const TUid* aUid,
								const CImplementationInformation&  aArrayItem )
{
	return (*aUid  == aArrayItem.ImplementationUid() );
}

// ----------------------------------------------------------------------------
// CCalenCustomisationManager::SetPluginAvailabilityL
// Enables or disables the given plugin.
// ----------------------------------------------------------------------------
//
 void CCalenCustomisationManager::SetPluginAvailabilityL( TUid aPluginUid,
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
            ASSERT( position != KErrNotFound );
            
            iActivePlugins.Remove( position );         


            // Issue notification of plugin been disabled
            iPluginsEnabledDisabled = ETrue;            
            
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
 void CCalenCustomisationManager::UnloadPluginsL( const RArray<TUid>& aPlugins )
    {
    TRACE_ENTRY_POINT;
    // Check plugin isn't already enabled
    TInt count = aPlugins.Count();
    for (TInt index = 0; index < count; ++index )
        {
        TInt position = iPlugins.Find( aPlugins[index], CPluginInfo::Identifier );
        if ( position != KErrNotFound ) 
	        {
	        	if((iInfoBarProviderUid != iPlugins[position]->Uid()))
		            {
		            CPluginInfo* removedPlugin = iPlugins[ position ];
		            iPlugins.Remove( position );
		            delete removedPlugin;
		            }
	        	else 
		        	{
		        	iPlugins[position]->Disable(ETrue);
					iDefferedUnloadPluginList.AppendL(iPlugins[position]->Uid());
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
    
    TRACE_EXIT_POINT;
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
		case ECalenNotifySystemLanguageChanged:
			{
			CreateActivePluginListL();
			DoImmediatePluginLoadingL();
			}
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
 const RImplInfoPtrArray& CCalenCustomisationManager::PluginInfoArray() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iPluginInfo;
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


    TRACE_EXIT_POINT;
    }

 


// ----------------------------------------------------------------------------
// CCalenCustomisationManager::Infobar
// Returns the infobar.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
 HbWidget* CCalenCustomisationManager::Infobar( )
    {
    TRACE_ENTRY_POINT;

    iInfoBarProviderUid = TUid::Uid(0);
       
    HbWidget* infoBar =NULL;
    // Loop though all the command handlers, within two interlinked
    // for loops. If GetInfoBarL leaves for one plugin, a plugin
    // error message will be displayed and the loop will continue with
    // the next command handler. If none of the plugins leave, there will
    // be only one TRAP used.
    TInt count = iPlugins.Count();
    /*if(count > 0)
        {
        infoBar = iPlugins[0]->Plugin().InfobarL();
        iInfoBarProviderUid = iPlugins[0]->Uid();
        }*/
    
    for ( TInt index = 0; index < count && !infoBar; ++index )
        {
        TRAPD( error,
            // Loop though until an infobar is found
            for (; index < count && !infoBar; ++index )
                {
                //if ( !iPlugins[index]->IsDisabled() )
                    {
                    infoBar = iPlugins[index]->Plugin().InfobarL();
                    iInfoBarProviderUid = iPlugins[index]->Uid();
                    }
                }
             );
        if(error)
        	{
        		// Need to handle the error case
        	}
        }
        
    TRACE_EXIT_POINT;   
    return infoBar;
    }

 // ----------------------------------------------------------------------------
 // CCalenCustomisationManager::InfobarTextL
 // @returns info bar text
 // (other items were commented in a header).
 // ----------------------------------------------------------------------------
 //
 QString* CCalenCustomisationManager::InfobarTextL()
     {
     TRACE_ENTRY_POINT;

         iInfoBarProviderUid = TUid::Uid(0);
            
         QString* infoBarText =NULL;
         // Loop though all the command handlers, within two interlinked
         // for loops. If GetInfoBarL leaves for one plugin, a plugin
         // error message will be displayed and the loop will continue with
         // the next command handler. If none of the plugins leave, there will
         // be only one TRAP used.
         TInt count = iPlugins.Count();
         /*if(count > 0)
             {
             infoBar = iPlugins[0]->Plugin().InfobarL();
             iInfoBarProviderUid = iPlugins[0]->Uid();
             }*/
         
         for ( TInt index = 0; index < count && !infoBarText; ++index )
             {
             TRAPD( error,
                 // Loop though until an infobar is found
                 for (; index < count && !infoBarText; ++index )
                     {
                     //if ( !iPlugins[index]->IsDisabled() )
                         {
                         infoBarText = iPlugins[index]->Plugin().InfobarTextL();
                         iInfoBarProviderUid = iPlugins[index]->Uid();
                         }
                     }
                  );
             if(error)
		        	{
		        		// Need to handle the error case
		        	}
             
             }
             
         TRACE_EXIT_POINT;   
         return infoBarText;
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

    CPluginInfo* newPlugin = new ( ELeave ) CPluginInfo( aPlugin, aUid);
    CleanupStack::PushL( newPlugin );
    iPlugins.AppendL( newPlugin );
    CleanupStack::Pop( newPlugin );

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
// CCalenCustomisationManager::Identifier(
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
// CCalenCustomisationManager::OfferMenu
// Offers a menupane for customisation by plugins
// and the controller.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenCustomisationManager::OfferMenu(HbMenu* aHbMenu )
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
                iPlugins[index]->Plugin().CustomiseMenu(aHbMenu );
                                                             
                // The commands added should be checked to see that
                // they match the expected command range for the plugin
                }
             );
         
        if ( error )
            {
            // Report a problem with plugin.
            }
        }

    TRACE_EXIT_POINT;
    }

// End of File
