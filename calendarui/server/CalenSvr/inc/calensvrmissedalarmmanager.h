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
* Description:   An alarm UI class, CCalenSvrMissedAlarmManager.
*                This class takes care of handling the UI and Cenrep updation
*								 for missed alarms.
*
*/


#ifndef __CALENSVRMISSEDALARMMANAGER_H
#define __CALENSVRMISSEDALARMMANAGER_H

// INCLUDES
#include <e32base.h>
#include <AknSoftNotifier.h>
#include <AknSoftNotificationParameters.h>
#include <cenrepnotifyhandler.h>        // MCenRepNotifyHandlerCallback
#include <missedalarmstore.h>

// FORWARD DECLARATIONS
class CAknSoftNotifier;
class CAknSoftNotificationParameters;
class CAknSmallIndicator;

// CLASS DECLARATION

/**
 *  Class declaration for handling missed alarms 
 */

NONSHARABLE_CLASS( CCalenSvrMissedAlarmManager ) : public CActive,
                                                   public MCenRepNotifyHandlerCallback
     {
	 public: // Constructors and destructor
	 
	     /**
	      * Constructor
	      */
	     static CCalenSvrMissedAlarmManager* NewL();

	     /**
	      * Destructor
	      */
	     ~CCalenSvrMissedAlarmManager();

    private:   
        
        /**
         * C++ default constructor
         */
        CCalenSvrMissedAlarmManager();

        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();
        
    public: 
        
        void Start();
    
        /**
         * Handles the change in missed alarm store.
         * updates soft notification and indicator plugins
         */ 
        void HandleMissedAlarmL();
    
    private: // From CActive
    
        void DoCancel();
        TInt RunError(TInt aError);
        void RunL();
    
    public:
        /**
         * From MCenRepNotifyHandlerCallback.
         * Notification of any change to the Missed Alarm Store central repository
         * @param aId Id of the cenrep key that changed
         */
        void HandleNotifyGeneric( TUint32 aId );
        
    private:
    
        /**
         * Method to handle the Missed Alarm Soft Notification 
         * Implements a Customized Soft Notification.   
         */
        void HandleMissedAlarmSoftNotificationL();
        
        /**
         * Method to display the Missed Alarm Notification 
         * on Cover UI
         */
        void HandleMissedAlarmCoverUISoftNotificationL();
        
        /**
         * Method to create the Group Soft Notification Parameters     
         */
        CAknSoftNotificationParameters* CreateNotificationParametersLC( 
                                const TAknSoftNotificationType aNotificationType );
                                                                  
        /**
         * Method to handle the Missed Alarm Small Indicator     
         */
        void HandleMissedAlarmSmallIndicatorL(TBool aArg);
        
        /**
         * Method to update the ScreenSaver Icon for missed alarms
         */
        TInt SetMissedAlarmScreenSaverIcon();
        
        /**
         * Method to update the CentralRepository Key value for Missed Calendar Alarms 
         */
        TInt SetMissedAlarmCenRepValueL();
        
        /**
         * Method to update the count of the missed alarms
         * by reading the latest count from the central repository
         */
        void GetCountOfMissedAlarmsL();
        
    private:
        //Pointer for CMissedAlarmsStore class
        CMissedAlarmStore* iMissedAlarmStore;
        
        //Pointer for Customised Notification parameters
        CAknSoftNotifier* iSoftNotifier;
        
        CRepository* iMissedAlarmStoreRepository;
        CCenRepNotifyHandler* iCenRepChangeNotifier;
        
        //Number of missed alarms read from Calendar repository
        TInt iNumOfMissedAlarms;    
        
        TBool iShowSoftNotification;
 
     };

#endif  // __CALENSVRMISSEDALARMMANAGER_H

// End of File