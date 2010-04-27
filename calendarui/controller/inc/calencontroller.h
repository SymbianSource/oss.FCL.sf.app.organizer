/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar controller
*
*/



#ifndef CALENCONTROLLER_H
#define CALENCONTROLLER_H

// INCLUDES
#include <e32base.h>                    // CAsyncCallBack
#include <vwsdef.h>                     // TVwsViewId
#include <apadef.h>                     // TApaCommand
#include <calencommands.hrh>            // Calendar commands
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calennotificationhandler.h>   // TCalenNotification
#include <calenservices.h>
class MCalenPreview;
#include "calenglobaldata.h"            // TCalenContext
#include "calenservicesfactory.h"       // MCalenServicesFactory

// FORWARD DECLARATIONS
class CCalenActionUi;
class CCalenNotifier;
class CCalenViewManager;
class CCalenAlarmManager;
class CCalenView;
class CAknViewAppUi;
class CCalenStateMachine;
class CCalenCmdLineLauncher;
class CCalenCustomisationManager;
class CEikMenuPane;
class CCoeControl;
class CCalenViewInfo;
class CCalenSetting;
class CCalenServicesImpl;
class MCalenCommandHandler;
class TCalenCommand;
class CMissedAlarmStore;
class CCalenMultipleDbManager;
class CCalenInfo;
class CCalenAttachmentModel;

// CLASS DEFINITIONS

/**
 * The controller handles events from the rest of Calendar and delegates
 * them to the appropriate place (i.e. the action ui classes).
 */
class CCalenController : public CBase, 
                         public MCalenServicesFactory
    {
    public:  // Construction and destruction
        /**
         * Symbian 1st phase constructor
         * @param aAppUi A reference to the CAknViewAppUi
         * @return self pointer to CCalenController
         */
        IMPORT_C static CCalenController* NewL( CAknViewAppUi& aAppUi );
        
        /**
         * Constructs CCalenController with existing CAknViewAppUi. If the
         * controller has been previously initialized with the same CAknViewAppUi,
         * the existing instance will be returned.
         * @param aAppUi Reference to CAknViewAppUi
         * @return CCalenController pointer
         */
        IMPORT_C static CCalenController* InstanceL();

        /**
         * CCalenController is a reference counting singleton. Call Release()
         * when you are done with it, it will clean itself when it needs to
         */
        IMPORT_C void Release();

    public:  // New functions
        /**
         * Adds the passed command to the command queue. Command are handled
         * asynchronously in HandleCommandL
         * @param aCommand Command that is passed in.
         * @return EFalse if the passed command is not in the issuers command range
         * ETrue otherwise
         */
        IMPORT_C TBool IssueCommandL( TInt aCommand );

        /**
         * Passes the notification to the calendar notifier. The notification will be
         * broadcast to all observers
         * @param aNotification Notification to be broadcast
         */
        IMPORT_C void BroadcastNotification( TCalenNotification aNotification );
        
        /**
         * Register the passed notification handler with the calendar notifier
         * @param aHandler Notification handler
         * @param aNotification Notification that handler wants to be notified
         */ 
        IMPORT_C void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                 TCalenNotification aNotification );
        
        
        /**
         * Register the passed notification handler with the calendar notifier
         * @param aHandler Notification handler
         * @param aNotifications Array of Notifications that handler wants to be notified
         */ 
        IMPORT_C void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                 RArray<TCalenNotification>& aNotifications );
        
        /**
         * Unregister the passed handler with the notifier
         * @param aHandler Notification handler
         */
        IMPORT_C void CancelNotifications( MCalenNotificationHandler* aHandler );

        /**
         * Process comamndline parameters
         * @param aCommand Not in use in this case
         * @param aDocumentName Not in use in this case
         * @param aTail Descriptor contains the command
         */
        IMPORT_C void ProcessCommandParametersL( TApaCommand aCommand,
                                                 TFileName& aDocumentName,
                                                 const TDesC8& aTail );
        /**
         * Offer the menu pane to Plugins to customisation
         * @param aMenuPane Where the customisation apply to
         */
        IMPORT_C void OfferMenuPaneL( TInt aResourceId, 
                                      CEikMenuPane* aMenuPane );
       
        /**
         * Get infobar based on the available area for infobar
         * @param aRect The available area for Infobar
         * @return pointer to a Infobar control
         */
        IMPORT_C CCoeControl* Infobar( const TRect& aRect  );

        /**
         * Get infobar based on the available area for infobar
         * @param aRect The available area for Infobar
         * @return pointer to a Infobar control
         */
		IMPORT_C const TDesC& Infobar();
       
        /**
         * Get preview pane based on the available area
         * @param aRect The available area for preview pane
         * @return pointer to a preview pane control
         */
        IMPORT_C CCoeControl* PreviewPane(  TRect& aRect );
        
         /**
         * Releases all plugins, should only be called when 
         * the application is exiting.
         */
        IMPORT_C void ReleaseCustomisations();
        
        /**
         * From MCalenServicesFactory
         */
        IMPORT_C MCalenServices* NewServicesL();
        
        /*
         * Get custom preview pane to replace the native one.
         * @param aRect TRect of preview pane area.
         * @return MCalenPreview* pointer to MCalenPreview class 
         */
        IMPORT_C MCalenPreview* CustomPreviewPaneL( TRect& aRect );

    public:  // public but not exported
        /**
         * Get reference to the CCalenNotiffier
         */
        CCalenNotifier& Notifier();
        
        /**
         * Get a command handler for a specific command
         * @param aCommand Command to find a command handler for
         */
        MCalenCommandHandler* GetCommandHandlerL( TInt aCommand );
        
        /**
         * Request activation of a specific view
         * @param aViewId View id
         */
        void RequestActivationL( const TVwsViewId& aViewId );
        
        /**
         * Get customisationmanager
         * @return reference to CCalenCustomisationManager
         */
        CCalenCustomisationManager& CustomisationManager();
        
        /**
         * Get viewmanager
         * @return reference to CCalenViewManager
         */
        CCalenViewManager& ViewManager();
        
        /**
         * Get services
         * @return reference to controller's services object
         */
        MCalenServices& Services();
        
        /**
         * Checks to see if the system time was changed while Calendar was
         * not running, potentially causing alarms to be missed
         */
        void HandleSystemTimeChangeL();
        
        /**
         * Check for faster app flag 
         */
        TBool IsFasterAppFlagEnabled();
        
        /**
         * Set faster app flag
         */
        void SetFasterAppFlag( TBool aFlag );
        
        /**
         * Get Missed alarm store
         * @return reference to Missed alarm store
         */
        CMissedAlarmStore* MissedAlarmStore();
		
		/**
		 * Get missed alarms list
		 * @return list of missed alarms
		 */
		void GetMissedAlarmsList(RArray<TCalenInstanceId>& aMissedAlarmList);

		/**
		 * returns a reference to the calendar settings
		 */
		CCalenSetting& Settings();
		
        /**
         * Get calendar icon of specific type
         * @return CGulIcon*
         */
		CGulIcon* GetIconL(MCalenServices::TCalenIcons aIndex, const TInt aViewId = 0 ); 

        /* Get multipledb manager
        * @return reference to CCalenMultipleDbManager
        */
        CCalenMultipleDbManager& MultipleDbManager();
        
        /*
         * Get all active collection id 
         * @param aCollectionIds array for collection on return
         * @return void
         */
        void GetActiveCollectionidsL(RArray<TInt>& aCollectionIds);
        
        /*
         * Form calendar name based dbid for eg:for 
         * c:Calendar append dbid 1 as c:Calendar1
         * @return HBufC* calendar name ownership return to caller.
         * @param TInt aDbId db id for which calendar name should be formed.
         */
        HBufC* GetCalendarNameFromDbIdL(const TInt aDbId);

       
        /**
         * Get State Machine
         * @return reference to CCalenStateMachine
         */
        CCalenStateMachine& StateMachine();
        
        /**
         * Get action ui
         * 
         * @return reference to CCalenActionUi
         */
        CCalenAttachmentModel& AttachmentData();

		 /**
         * Function to tell whether editor is active or not
         * @return ETrue if editor is active else EFalse
         */
        TBool IsEditorActive();

        /**
         * @brief adds a new calendar info to the calendar file
         * 
         * @param aCalendarInfo meatadata to be set to calendar file
         */
        void AddCalendarL(CCalCalendarInfo* aCalendarInfo);
        
        /**
         * @brief removes a calendar
         * 
         * @param aCalendarFileName name of the calendar file to be deleted
         */
        void RemoveCalendarL(const TDesC& aCalendarFileName);

        /**
         * @brief removes all dead calendars from the file system.
         */
		void RemoveDeadCalendarsL();

        /**
         * @brief updates a calendar file with new metadata
         * 
         * @param aCalendarInfo updated metadata to be set to calendar file
         */
        void UpdateCalendarL(CCalCalendarInfo* aCalendarInfo);
        
        /**
         * @brief get all available calendar info
         * 
         * @param aCalendarInfoList reference to the calendar info list
         */
        void GetAllCalendarInfoL( 
                    RPointerArray<CCalCalendarInfo>& aCalendarInfoList );
        
    private:  // Construction and destruction
        /**
         * C++ constructor
         */
        CCalenController( CAknViewAppUi& aAppUi );
        
        /**
         * Symbian 2nd phase constructor
	     */
        void ConstructL();
        
        /**
         * destructor
         */
         ~CCalenController(); 
        
    private:  // New functions     
        /**
         * Leaving form of HandleCommandL
         * @param aCommand Command to be handled
         */
        void HandleCommandL( TCalenCommand& aCommand );
          
        /**
         * Displays an information note if the system time changed 
         * while Calendar was inactive
         * @param aResourceId resource id for information note
         */
        void ShowSystemChangeInfoNoteL( TInt aResourceId );

        /**
         * This function is called when a System time is changed.
         * only at the startup
         * @param aThisPtr Specified to be TCallBack()
         * @return EFalse
         */
        static TInt SystemTimeChangeCallback(TAny* aThisPtr);

    public:   
        /**
         * Check system time change at startup 
         */
        void CheckSystemTimeAtStartUpL();
        
        /**
         * Returns a reference to the appui
         */
        CAknViewAppUi& AppUi();
        
        /**
         * Set the iIsExitOnDialogClose flag status from command line launcher.
         * @param aFlag , Sets the status 
         */
        void SetExitOnDialogFlag( TBool aFlag ); //updateflag
        
        /**
         * Get the iIsExitOnDialogClose flag status from command line launcher.
         * @return TBool, Status of the flag.
         */
        TBool GetExitOnDialogFlag(); // getflagstatus
        
    private:  // Data
        CAknViewAppUi&      iAppUi;      // Calendar AppUi
        CCalenActionUi*     iActionUi;   // Default internal command handling
        CCalenNotifier*     iNotifier;   // Broadcasts calendar events
        CCalenViewManager*  iViewManager;// Activate / deactivate views
        CCalenGlobalData*   iGlobalData; // Calendar Global Data
        CCalenSetting*      iSetting;    // Calendar settings
        CCalenServicesImpl* iServices;   // Services.
        CCalenAlarmManager* iAlarmManager; //Alarm Manager.
        CCalenMultipleDbManager* iMultipleDbmanager; //Multiple calendar
        CCalenAttachmentModel* iAttachmentData;
        TInt iRefCount;  // Reference counting singleton

        // Handles the cmd line launching of calendar
        CCalenCmdLineLauncher* iCmdLineLauncher;
        // Manages any customisation plugins
        CCalenCustomisationManager* iCustomisationManager;     
        CCalenStateMachine* iStateMachine;

        // Start point of the next command range
        // to be assigned to a services instance.
        TInt iNextServicesCommandBase; 
        TInt iResourceFileOffset;
        CAsyncCallBack* iSystemTimeChangedMsgDelayer;   
        TBool iFasterApp;
    };

#endif // CALENCONTROLLER_H

// End of file
