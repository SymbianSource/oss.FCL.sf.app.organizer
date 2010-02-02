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
* Description:   Calendar alarm manager
*
*/



#ifndef CALENALARMMANAGER_H
#define CALENALARMMANAGER_H

// INCLUDES
#include <vwsdef.h>
#include <coeview.h>                    // MCoeViewActivationObserver
#include <missedalarmstore.h>
#include <cenrepnotifyhandler.h>        // MCenRepNotifyHandlerCallback
#include <calencommandhandler.h>
#include <calennotificationhandler.h>   // MCalenNotificationHandler
#include <calentoolbar.h>  

// FORWARD DECLARATIONS
class CCalenController;
class CMissedAlarmStore;
class CCalenContextFWListener;
class CCalenViewManager;

// CLASS DEFINITIONS

class CCalenAlarmManager : public CBase,
                           public MCalenNotificationHandler,
                           public MCalenCommandHandler,
                           public MCenRepNotifyHandlerCallback
    {
    public:
        /**
         * Symbian 1st phase constructor
         * @param aViewManager Reference to the CCalenViewManager
         * @return Pointer to CCalenViewManager
         */
        static CCalenAlarmManager* NewL( CCalenController& aController );
        
        /**
         * Destructor
         */                             
        virtual ~CCalenAlarmManager();
        
    private:
        /**
         * C++ constructor
         * @param aViewManager Reference to CCalenViewManager
         */
        CCalenAlarmManager( CCalenController& aController );

        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();    

    public:
        
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
         * Handles notifications (Leaving)
         */
        void HandleNotificationL( TCalenNotification aNotification );
    		
        /**
         * From MCenRepNotifyHandlerCallback.
         * Notification of any change to the Missed Alarm Store central
         * repository
         * @param aId Id of the cenrep key that changed
         */
        void HandleNotifyGeneric( TUint32 aId );

    public:
        
        /**
         * Get Missed alarm store
         * @return reference to Missed alarm store
         */
        CMissedAlarmStore* MissedAlarmStore();
        
        /**
         * Get Missed alarms list with viewed info
         * @return array of missed alarms
         */
        void GetMissedAlarmsList(RArray<TCalenInstanceId>& aMissedAlarmList);
        
        /**
         * Destroys CCalenContextFWListener object for Alarm
         * @param aCloseEventView
         */        
        void StopAlarmContextListener(TBool aCloseEventView=EFalse);
        
        /**
         * Destroys CCalenContextFWListener object for Auto snooze case.
         */        
        void StopContextListenerForAutoSnooze();
        
        /**
         * Creates CCalenContextFWListener object for Alarm
         */          
        void StartAlarmContextListenerL(); 
        
    private:
        
        /**
         * from MCalenNotificationHandler
         */
        void HandleNotification( const TCalenNotification aNotification );
        
        /**
         * Creates missed alarms list
         */
        void CreateMissedAlarmsListL();
        
        /**
         * Handles the command ECalenMissedAlarmsView
         * for launching missed alarms view
         */
        void OnCmdMissedAlarmViewL();

        /**
         * Handles the command ECalenMissedEventView
         * for launching missed event view
         */
        void OnCmdMissedEventViewL();

        /**
         * Handles the command ECalenCmdClear
         * for clearing a missed alarm
         */
        void OnCmdClearMissedAlarmL();
        
        /**
         * Handles the command ECalenCmdClearAll
         * for clearing all missed alarms
         */
        void OnCmdClearAllMissedAlarmsL();
        
        /**
         * Handles the command ECalenCmdGotoCalendar
         * for going back to native view from missed alarms view
         */ 
        void OnCmdGoToCalendarL();
        
        /**
         * Handles the command ECalenMissedEventViewFromIdle
         * for intialising the data before launching the
         * missed event view from Idle(soft notification/indicator)
         */
        void OnCmdLaunchFromIdleL();
        
        /**
         * For taking action whenever an entry is deleted 
         */
        void HandleEntryDeleteNotificationL();
        
        /**
         * For updating the missed alarms list when an entry is deleted 
         */
        void UpdateMissedAlarmsListOnDeleteL();
        
        /**
         * For handling notification ECalenNotifyLostAlarms
         * which updates missed alarms list and missed alarms count
         */
        void HandleNotifyLostAlarmsL();
        
        /**
         * For handling notification ECalenNotifyMissedAlarmsViewClosed
         * which activates the previous view or exits the application 
         * if launched from Idle
         */
        void HandleMissedAlarmViewClosedL();
        
        /**
         * For handling notification ECalenNotifyMissedEventViewClosed
         * which activates the previous view or exits the application 
         * if launched from Idle
         */
        void HandleMissedEventViewClosedL();
        
        /**
         * For handling notification ECalenNotifySystemTimeChanged
         * Listen for the system time change notification.
         * Clear all the future alarms which are missed due to 
         * time change
         */
        void HandleSystemTimeChangedL();

        /**
         * Sets the context based on the missed alarm 
         */
        void SetContextForMissedEventViewL();
        
        /**
         * Mark missed alarm event as viewed
         */
        void SetMissedAlarmEventAsViewed();
        
        /**
         * Remove all viewed events
         */
        void RemoveAllViewedEventsL();
        
        /**
         * Clear one missed alarm
         */
        TBool ClearOneMissedAlarmL(TInt aEntryLocalUid, TCalCollectionId aColId );
        
        /**
         * Update missed alarms list
         */
        void UpdateMissedAlarmsListL();
        
        /**
         * Get the alarm time for the entry
         */
        void GetAlarmDateTimeL(const CCalEntry& aEntry, TTime& aAlarmDateTime);
        
        /**
        * Launch the event viewer after pressing MSK.
        */
        void LaunchEventViewerL();
        
        /**
         * Handles Back and Delete event in event viewer in MSK case.
         */
         void HandleBackEventL();
        
    private:  // Data        
				
		CCalenController& iController;
 		CCalenViewManager& iViewManager;
		CMissedAlarmStore* iMissedAlarmStore;
        
        CRepository* iMissedAlarmStoreRepository;
        CCenRepNotifyHandler* iCenRepChangeNotifier;
        
        RArray<TCalenInstanceId> iMissedAlarmList;
        
        TVwsViewId  iPreviousToMissedAlarmView;
        TVwsViewId  iPreviousToMissedEventView;

        TUint32 iMissedAlarmsCount;
 		TBool iLaunchedFromIdle;
 		TBool iViewerLaunchedFromIdle;   // For opening alarm through MSK.
 		
 		TInt iOrigWGPos; // original window group position 
        TInt iOrigWGPrio; // original window group priority
        CCalenContextFWListener*  iContextFWListener; //Context Framework handler 
        TUid iPreviousToEventViewUid;  // getting the view id previous to event view.
    };

#endif // CALENALARMMANAGER_H

// End of file
