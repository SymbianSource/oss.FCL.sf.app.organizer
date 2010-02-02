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
* Description:   Calendar notifier
*
*/



#ifndef CALENNOTIFIER_H
#define CALENNOTIFIER_H

// INCLUDES
#include <e32hashtab.h>                 // RHashSet
#include <cenrepnotifyhandler.h>        // MCenRepNotifyHandlerCallback
#include <calennotificationhandler.h>   // MCalenNotificationHandler
#include <calprogresscallback.h>        // MCalProgressCallBack
#include <calfilechangenotification.h>  // MCalFileChangeObserver

#include "calendbchangenotifier.h"      // MCalenDBChangeObserver
#include "calencontextchangeobserver.h" // MCalenContextChangeObserver
#include "calenecomchangeobserver.h"    // MCalenEComChangeObserver

// FORWARD DECLARATIONS
class CCalenGlobalData;                 // Calendar global data singleton
class CEnvironmentChangeNotifier;       // Receive system event notifications
class CCalenDbChangeNotifier;           // Receive database change notifications
class CCalenEComWatcher;                // Receives Ecom registry change notifications
class CCalenSetting;                    // Calendar settings
class CCalenController;
class CCalFileChangeInfo;

/**
 * CCalenNotifier observes various system events relevent to Calendar
 * MCalenNotificationHandlers can register for notifications of specific
 * events. This minimises the number of event observers required
 * throughout the Calendar application.
 */
class CCalenNotifier :  public CBase,
                              public MCenRepNotifyHandlerCallback,
                              public MCalenDBChangeObserver,
                              public MCalProgressCallBack,
                              public MCalenContextChangeObserver,
                              public MCalenEComChangeObserver,
                              public MCalFileChangeObserver
                              
    {
    public:
        /**
         * C++ default Constructor
         */
        CCalenNotifier( CCalenController& aController );

        /**
         * 2nd phase of construction.
         */
        void ConstructL();

        /**
         * Destructor
         */
        virtual ~CCalenNotifier();

    public:
        /**
         * Allows MCalenNotificationHandlers to register for system event
         * notifications
         * @param aHandler Handler to register
         * @param aFlag Notification of events to notify aHandler about
         */
        void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                TCalenNotification aNotification );
        
        /**
         * Allows MCalenNotificationHandlers to register for system event
         * notifications
         * @param aHandler Handler to register
         * @param aFlags Array of notifications to  notify aHandler about
         */
        void RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                 RArray<TCalenNotification>& aNotifications );

        /**
         * Allows MCalenNotificationHandlers to cancel system event
         * notifications
         * @param aHandler Handler to deregister
         */
        void CancelNotifications( MCalenNotificationHandler* aHandler );

        /**
         * Notify all interested handlers about an event
         * @param aNotification Notification to broadcast
         */
        void BroadcastNotification( TCalenNotification aNotification );
        
        /**
         * Notify all interested handlers about an event
         * @param aNotification Notification to broadcast
         */
        void BroadcastApprovedNotification( TCalenNotification aNotification );

        /**
         * Check if the system time changed since Calendar was
         * last launched
         */
        TInt SystemTimeChangedL();

        /**
         * After calling this function, any settings changed notifications
         * will not be broadcast until after ResumeSettingsNotifications
         * has been called.
         */
        void DeferSettingsNotifications();

        /**
         * Resumes settings notifications after they have been paused
         * with DeferSettingsNotifications.
         */
        void ResumeSettingsNotifications();
        
        /**
         * Update the cenrep with latest system time change information 
         */
        void UpdateSytemTimeChangeInfoL();
    
    public:
        /**
         * From MCenRepNotifyHandlerCallback.
         * Notification of any change to the Calendar central repository
         * where the main Calendar settings are stored
         * @param aId Id of the cenrep key that changed
         */
        void HandleNotifyGeneric( TUint32 aId );

        /**
         * From MCenRepNotifyHandlerCallback.
         * Notification of central repository observer failure.
         */
        void HandleNotifyError( TUint32 aId,
                                        TInt aError,
                                        CCenRepNotifyHandler* aHandler );

        /**
         * From MCalenDBChangeObserver.
         * Notification of any change to the Calendar database through an
         * external CCalSession.
         * Notifications are limited to a maximum of one per second
         */
        void HandleDBChangeL();

        /**
         * From MCalenContextChangeObserver.
         * Called when the context changes.
         */
        void ContextChanged();

        /**
         * From MCalenEComChangeObserver
         * Called by when the ecom registry gets changed.
         */
        void EComChanged();
        
        /**
         * @brief From MCalFileChangeObserver
         * The callback that will recieve 1 or more file change notifications
         * 
         *  @param aCalendarInfoChangeEntries Holds the information about the 
         *         calendar info changes  
         */
        void CalendarInfoChangeNotificationL(
            RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries);

     public:
        /**
         * From MCallProgressCallBack
         * Called when CCalEntryView creation is complete.
         * @param aStatus completion status
         */
        void Completed( TInt aStatus );

        /**
         * From MCallProgressCallBack
         * Not used by this class
         */
        void Progress( TInt aPercentageCompleted );

        /**
         * From MCallProgressCallBack
         * Returns EFalse to disable progress notifications
         */
        TBool NotifyProgress();

    private:    // Callback functions from observers
        /**
         * Called from CEnvironmentChangeNotifier when the
         * system environment changes
         * @param aThisPtr self pointer
         * @return EFalse
         */
        static TInt EnvChangeCallbackL( TAny* aThisPtr );

        /**
         * Broadcasts the given notification.
         * @param aNotification Notification id to be broadcasted
         */
        void DoBroadcast( TCalenNotification aNotification );
        
        TInt DoEnvChange();
        
        static TInt AsyncRemoveCalendarL(TAny* aThisPtr);
        
        void AsyncRemoveCalendarL();

    private:  // Data
        // Array of handlers to notify
        class TNotificationHandler
            {  
            public:
                TNotificationHandler();
                
                RHashSet<TCalenNotification>  iHashSet;
                MCalenNotificationHandler* iHandler;
            };

        RArray<TNotificationHandler> iHandlers;

        // Notifications about locale and time changes
        CEnvironmentChangeNotifier* iEnvChangeNotifier;

        // Notifications about Calendar settings changes
        CRepository* iRepository;
        CCenRepNotifyHandler* iCenRepChangeNotifier;

        // Calendar Global Data singleton
        CCalenGlobalData* iGlobalData;
        
        // Notifications about changes to the ECom registry
        CCalenEComWatcher* iEComWatcher;
       // CCalenEComWatcher* iEComWatcher1;

        RArray<TCalenNotification> iBroadcastQueue;
        TBool iBroadcastActive;

        CCalenDbChangeNotifier* iDbChangeNotifier;
        CCalenSetting* iSetting;
        TBool iIsSettingsBroadcastDeferred;
        TBool iSettingsNeedsBroadcast;
        TBool iLocaleNeedsBroadcast;
        CCalenController& iController;
        TBool iIgnoreFirstLocaleChange;
        
        // latest time change from agenda server
        TReal iTimeOfChangeUtcReal;
        
        CAsyncCallBack* iAsyncCallback;
        HBufC* iFilnameDeleted;
    };

#endif // CALENNOTIFIER_H

// End of file
