/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar view manager
*
*/


#ifndef CALENVIEWMANAGER_H
#define CALENVIEWMANAGER_H

// INCLUDES
#include <vwsdef.h>
#include <coeview.h>                    // MCoeViewActivationObserver
#include <calencommandhandler.h>
#include <calennotificationhandler.h>   // MCalenNotificationHandler

// FORWARD DECLARATIONS
class CAknViewAppUi;
class CCalenGlobalData;
class CCalenViewPopulator;
class CCalenController;
class CCalenView;
class CEikMenuPane;
class CCalenToolbarImpl;
class MCalenToolbar;
class CCalenSetting;
class CCalenViewInfo;
class CCalenCustomisationManager;
class MCalenServices;
class CCalenIcons; 
class CCalenToolbarImpl;

typedef CCalenView* (*NativeView)(MCalenServices&);

/**
 * The action ui handles events passed from the controller and delegates
 * them to the appropriate place (i.e. the specific action ui classes).
 */
class CCalenViewManager : public CBase,
                          public MCoeViewActivationObserver,
                          public MCalenNotificationHandler,
                          public MCalenCommandHandler
    {
    public:  // Construction and destruction
        /**
         * Symbian 1st phase constructor
         * @param aAppUi Reference to the CAknViewAppUi
         * @param aController Reference to the CCalenController
         * @return Pointer to CCalenViewManager
         */
        static CCalenViewManager* NewL( CAknViewAppUi& aAppUi,  
                                        CCalenController& aController );
        /**
         * Destructor
         */                             
        virtual ~CCalenViewManager();

    public:  // New functions
        /**
         * Constructs the custom views
         * @param aCustomisationmManager Reference to CCalenCustomisationManager
         */
        void ConstructCustomViewsL(
                           CCalenCustomisationManager& aCustomisationManager );

        /**
         * Get info array
         * @return Reference to RPointerArray<CCalenViewInfo>
         */
        RPointerArray<CCalenViewInfo>& ViewInfoArray();

        /**
         * From MCalenCommandHandler.
         * Handles key presses.
         * 
         * @param aCommand The command Id of the key.
         */
        TBool HandleCommandL( const TCalenCommand& aCommand );

        /**
         * From MCalenCommandHandler.
         * Allows extending this API without breaking BC.
         * 
         * @param aExtensionUid specifies
         * @return extension of the requested type
         */
        TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid );

        /**
         * Returns the current view.
         * @return TUid of current view
         */
        TUid CurrentView() const;

        /**
         * Removes the current view from the switch menu
         * @param aMenuPane Where the currenct view should be removed from
         */
        void RemoveCurrentViewFromMenu( CEikMenuPane* aMenuPane );

        /**
         * Request activation of a specific view
         * @param aViewId Reference to TVwsViewId of the view to be activated
         */
        void RequestActivationL( const TVwsViewId& aViewId );
        
        /**
         * Get a pointer to the Calendar toolbar, or NULL if none is available.
         * Ownership is not transferred.
         * @return Calendar toolbar if available
         */
        MCalenToolbar* ToolbarOrNull();
        
        /**
         * Activates the default view, as retrieved from settings.
         */
        void ActivateDefaultViewL( TUid aDefaultView );
        
        /**
         * Returns if the first view activation on start-up has taken place
         */
        TBool ViewsActivated() const;
        
        /** 
         * Interupts the population of the current view.
         */
        void InterruptPopulationL();
        
        /**
         * Try to activate a view
         * @param aViewUid View id to be activated
         * @param aMessageId Message id
         * @param aMessage Message send along activation query
         */
        void RequestActivationL( const TUid& aViewUid,
                                 const TUid& aMessageId = KNullUid,
                                 const TDesC8& aMessage = KNullDesC8 );
        
        /**
         * Started population of the current view
         */
        void StartActiveStepL();
        
        /**
         * Returns a reference to the calendar icon array
         * @return reference to a CCalenIcons array
         */
        CCalenIcons& IconsL();  
         
        /**
         * Gets next view icon
         */
        CGulIcon* GetNextViewIconL();
        
        /**
         * Repopulate the view.
         */
        void SetRepopulation(TBool aRePopulate);

        /**
         * Returns calendar toolbar
         */
        CCalenToolbarImpl* CalenToolbar();
        
        /**
         * Unload all plugins on fake exit
         */
        void UnLoadAllPluginsL();
        
        /**
         * Reload all plugins when application comes to
         * foreground after fake exit
         */
        void ReloadAllPluginsL();
        
		/**
         * Gets the view Id of previous view.
         * @return TUid, Uid of previous view.
         */
        TUid GetPreviousViewUid();
		
        /**
         * Check for native view.
         * @param aViewUid Uid of the View  
         * @returns ETrue if aViewUid is any of the native views
         */
        TBool IsNativeView(TUid aViewUid);
        
        /**
         * Activate view depending upon the current default view
		 * If plugin views is default view, then dummy view will be activated
		 * else the native view
		 * @param	aDefView	current default view frm calendar settings
         */
        void ActivateViewOnFakeExitL(TUid aDefView);
        
        /**
         * Activate the local view if application already in back ground
         * @param   aDefView    current default view which needs to be activated.
         */
        void ActivateLocalViewL(TUid aDefView);
		
        /**
         * set the view Id of previous view.
         * @param TUid, aPreviousUid of previous view.
         */
        void SetPreviousViewUid(TUid aPreviousUid);
        
    private:  // New functions

        /**
         * From MCoeViewActivationObserver
         * Called just before a view in this application is activated by the view server
         * @param aNewlyActivatedViewId View id to be activated
         * @param aViewIdToBeDeactivated View id to be deactivated
         */
        void HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId,
                                   const TVwsViewId& aViewIdToBeDeactivated );
                                                                                          
        /**
         * Requests activation for the next view in the view cycle
         */
        void CycleNextViewL();
        
        /**
         * Requests activation for the previous view in the view cycle
         */
        void CyclePrevViewL();
        
        /**
         * Constructs the s60 native views and registers them with the view server
         */
        void ConstructNativeViewsL();
        
        /**
         * Constructs any custom views from plugins and registers them with the view server
         */
        void ConstructCustomViewsL();
        
         /**
         * Constructs any custom views from a plugin and registers them with the view server
         */
        void ConstructCustomViewL( TUid aPluginUid );
          
        /**
         * Prompts the user to choose a view to switch to. If cancelled returns 
         * KErrCancel, otherwise one of the ECalenView*
         */
        void ShowSwitchViewQueryL();
        
        /**
         * Adds a view to the array
         * @param aView Pointer to the view that is going to be added
         */
        void AddNativeViewL( CCalenView* aView );
        
        /** 
         * Removes a native view from the view manager
         * @param aViewUid  view uid of view that is going to be removed
         */
        void RemoveNativeViewL( TUid aViewUid );
        
        /**
         * Creates native view 
         */
        TBool CreateNativeViewL( TUid aViewUid );

        /**
         * Return reference to the customisation manager
         * @return Reference to CCalenCustomisationManager
         */
        CCalenCustomisationManager& CustomisationManager();

        /**
         * Compares the list of active plugins and plugins the view manager
         * knows about, to discover which plugins have been enabled or
         * disabled
         */
        void UpdatePluginListL();
        
        /**
         * Removes the custom views supplied by the plugin list from the view 
         * server.
         * @param   uids 
         */
        void RemoveDisabledPluginsViewsL( RArray<TUid>& aDisabledPlugins );
                                 
        /**
         * Asyncronous callback function to remove the current view after 
         * it has been disabled.
         */
        static TInt RemoveDeActivatedView( TAny* aObject );
        
        /**
         * Asyncronous callback function to remove the current view after 
         * it has been disabled. Leaving version
         */
        void RemoveDeActivatedViewL();
        
        /**
         * Handles notifications (Leaving)
         */
        void HandleNotificationL( TCalenNotification aNotification );
        
        /**
         * Updates the nextview icon on the toolbar when default view is not month view
         */
        void UpdateToolbarNextViewIconL(TUid defViewUid);
        
        /**
         * Handles Faster app exit
         */
        void HandleFasterAppExitCommandL();
        
        /**
         * Handles ECalenNotifySettingsChanged notification 
         */
        void HandleSettingsChangeNotificationL();
        
        /**
         * Handles ECalenNotifyEntryDeleted and ECalenNotifyInstanceDeleted
         * notifications 
         */
        void HandleEntryDeleteNotificationL();
        
        /**
         * Handles ECalenNotifyEntryClosed notification
         */
        void HandleEntryClosedNotificationL();
        
        /**
         * Handles ECalenNotifySystemTimeChanged notification
         */ 
        void HandleSystemTimeChangeNotificationL();
        
        /**
         * Changes day when swipped
         */
        void ChangeDayL( TBool aNextDay );
        
    private: // from MCalenNotificationHandler
   
        /**
         * Handles notifications
         */
        void HandleNotification( const TCalenNotification aNotification );
        
    private:  // Construction and destruction
        /**
         * C++ constructor
         * @param aAppUi Reference to CAknViewAppUi
         * @param aController Reference to CCalenController
         */
        CCalenViewManager( CAknViewAppUi& aAppUi,
                                     CCalenController& aController  );

        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();

    private:  // Data
        TVwsViewId              iCurrentViewId;
        CCalenGlobalData*       iGlobalData;
        CAknViewAppUi&          iAppUi;
        TInt                    iViewCycleIndex;
        CCalenViewPopulator*    iPopulator;
        CCalenSetting*          iSetting;
        CCalenToolbarImpl*      iToolbar;
        CCalenCustomisationManager* iCustomisationManager;
        RPointerArray<CCalenViewInfo> iViewInfoArray;
        CCalenController&           iController;

        // Stores the uids of the plugins, the view manager is
        // aware of.
        RArray<TUid> iKnownPlugins;
        // Pointer to CCalenViewInfo for an active view which has
        // been disabled. 
        CCalenViewInfo* iRemovedActiveView;
        // Remove the current view asynchronously
        CAsyncCallBack*  iViewRemovalCallback;   
        CCalenIcons* iIcons;
        
        // The first view on start-up has been activated.
        TBool iViewsActivated; 
        TVwsViewId  iPreviousViewId;
        TVwsViewId  iPreviousToDayView;
        TVwsViewId iPreviousToWeekView;
        TBool iAvoidRepopulation;
        TBool iStartupComplete;
        TBool iFakeExit;
    };

#endif // CALENVIEWMANAGER_H

// End of file
