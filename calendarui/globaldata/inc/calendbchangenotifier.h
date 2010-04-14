/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Notifies observers of external changes to the calendar database
*
*/


#ifndef __CALENDBCHANGENOTIFIER_H
#define __CALENDBCHANGENOTIFIER_H

//  INCLUDES
#include <e32base.h>

#include <calchangecallback.h>      //MCalChangeCallBack

// FORWARD DECLARATIONS
class CCalSession;                  //Calendar session
class CCalenGlobalData;             //Calendar global data

// CLASS DECLARATION

/**
* Observer class for database changes.  Forwards callback from 
* MCalChangeCallBack2
*/
class MCalenDBChangeObserver
    {
    public:
        virtual void HandleDBChangeL() = 0;
    };

/**
*  CCalenDbChangeNotifier buffers notifications from MCalChangeCallBack2 and 
*  only notifies its observers after a set period has elapsed.  This prevents
*  Calendar views refreshing more often than necessary during a sync operation
*/
NONSHARABLE_CLASS(CCalenDbChangeNotifier) : public CActive,
                                                                  public MCalChangeCallBack2
    {
    public:  // Constructors and destructor
        /**
        * Constructor.
        * @param aGlobalData global data reference
        * @return a pointer to the new CCalenDbChangeNotifier instance
        */
        static CCalenDbChangeNotifier* NewL( CCalSession& aSession );

       /**
        * Destructor.
        */
        ~CCalenDbChangeNotifier();
        
    public:
        /**
        * Allow CCalenViews to register for database change notifications
        * @param aDBObserver Observer to register
        */
        void RegisterObserverL( MCalenDBChangeObserver& aDBObserver );
        
        /**
        * Allow CCalenViews to deregister for database change notifications
        * @param aDBObserver Observer to deregister
        */
        void DeRegisterObserverL( MCalenDBChangeObserver& aDBObserver );
        
        /**
        * Returns the time of the last call to MCalChangeCallBack2::CalChangeNotification
        * This is not necessarily the same time as the last notification issued by 
        * this class
        * @return Time of the last database modification as TTime
        */
        TTime LastDBModificationTime() const;
        
    public: 
        /**
        * From MCalChangeCallBack2
        * Called when the calendar database is changed through another
        * CCalSession
        * @param aChangeItems array of database items changed 
        */
        void CalChangeNotification( RArray<TCalChangeEntry>& aChangeItems );

    private:
        /**
        * C++ default constructor.
        * @param aGlobalData global data reference
        */
        CCalenDbChangeNotifier( CCalSession& aSession );

        /**
        * By default Symbian 2nd phase constructor is private.
        * Performs any construction which may leave
        */
        void ConstructL();

        /**
        * From CActive
        * Called when outstanding asynchronous request completes
        * This will be called when iNotificationTimer either completes
        * or is cancelled
        */
        void RunL();
        
        /**
        * From CActive
        * Called by the active scheduler if RunL leaves
        * Ensures we are ready to receive the next database event
        * @param aError System wide error code
        */
        TInt RunError( TInt aError );

        /**
        * From CActive
        * Implements cancellation of outstanding asynchronous requests
        * Cancels iNotificationTimer if started
        */
        void DoCancel();

    private:    // Data

        //Database change observer filter
        CCalChangeNotificationFilter* iCalChangeFilter;      
        
        //Observer array
        RPointerArray<MCalenDBChangeObserver> iDBObservers;
        
        //Timer to limit the amount of notifications issued by this class
        RTimer iNotificationTimer;
        
        //The time of the last received notification
        TTime iLastDbChangeNotification;
        
        //Flag to restart the timer after cancelling last tiemr request
        TBool iRestartTimer;
        
        CCalSession& iSession;
    };

#endif      // __CALENDBCHANGENOTIFIER_H

// End of File
