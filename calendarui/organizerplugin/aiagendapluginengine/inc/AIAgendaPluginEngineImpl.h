/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class contains the actual implementation of the engine.
 *
*/


#ifndef CAIAGENDAPLUGINENGINEIMPL_H
#define CAIAGENDAPLUGINENGINEIMPL_H

// INCLUDES
#include "CalEngineCallback.h"
#include "CalenEngine.h"
#include "PropertyObserver.h"

#include <calfilechangenotification.h>

// FORWARD DECLARATIONS
class CEnvironmentChangeNotifier;
class CEventExpirationTimer;
class CCalInstance;
class MPluginDataChangeObserver;
class CCalSession;

// CLASS DECLARATION
NONSHARABLE_CLASS( CAIAgendaPluginEngineImpl ) : public CBase,
                                                 public MCalenEngineListener,
                                                 public MPropertyChangeHandler,
                                                 public MCalFileChangeObserver
                                                 
{
    // Allow CAIAgendaPluginEngine to access the SafeRefresh()
    friend class CAIAgendaPluginEngine;

 public:
    /**
     * Two-phased constructor.
     * @since Series 60 2.6
     * @param ?arg1 ?description
     **/
    static CAIAgendaPluginEngineImpl* NewL(MPluginDataChangeObserver& aDataChangeObserver, TObserverType aObserverType);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void ConstructL(void);

    /**
     * Destructor.
     * @since Series 60 2.6
     **/
    virtual ~CAIAgendaPluginEngineImpl(void);

 private:
    /**
     * Hide default constructor.
     * @since Series 60 2.6
     **/
    CAIAgendaPluginEngineImpl(MPluginDataChangeObserver& aDataChangeObserver, TObserverType aObserverType);

 public:  // From MCalenEngineListener
    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void OpenDatabaseCompleted(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void GetEntriesCompleted(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void DoGetEntriesCompletedL(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void GetTodosCompleted(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void GetFutureEventCompleted(void);

    /**
     * ?member_description.
     * @since S60 3.1
     **/
    void GetCalendarDataCompleted(void);

    /**
     * ?member_description.
     * @since S60 3.0
     **/
    void HandleError(TInt aError);

public:  // From MPropertyChangeHandler
    /**
     * ?member_description.
     * @since Series 60 3.0
     **/
    void HandlePropertyChange(const TUid& /*aCategory*/, const TUint& /*aKey*/, const TInt& /*aValue*/);
    
 private: //from MCalFileChangeObserver
     /*
      * The callback that will recieve 1 or more file change notifications
      * @param aCalendarInfoChangeEntries Array of CCalFileChangeInfo
      */
    void CalendarInfoChangeNotificationL(RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries);

 private:  // New functions
   
    /**
     * ?member_description.
     * @since S60 3.1
     **/
    void DoGetCalendarDataCompletedL(void);
   
    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void OpenDataBase(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void DoOpenDataBaseL(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void GetTodayData(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void DoGetTodayDataL(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void GetFutureData(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void DoGetFutureDataL(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     * @return ?description
     **/
    TBool HasEventsForTodayL(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void NotifyObserverAndCloseDB(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     * @param ?arg1 ?description
     * @return ?description
     **/
    static TInt EnvironmentChangeCallBack(TAny* aPtr);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void CloseCalendarEngine(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void RemoveExpiredEntries(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void DoRemoveExpiredEntriesL(void);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void SetTimerForNextExpiringEventL(void);

    /** 
     * Find the difference (in minutes) between meeting in aIndex and next meeting (if exist) with different start time.
     * @since S60 3.1
     * @param aIndex  Index of the current meeting.
     * @param aDifference Reference to the result.
     **/
    void DiffToNextEventL(const TInt aIndex, TTimeIntervalMinutes& aDifference);

    /**
     * ?member_description.
     * @since Series 60 2.6
     * @param ?arg1 ?description
     **/
    void StateMachine(void);

 protected: // for CAIAgendaPluginEngine
    /**
     * ?member_description.
     * @since S60 3.0
     **/
    void SafeRefresh(void);

 private:  // From MEventExpirationObserver
    /**
     * Callback for iEventExpirationTimer.
     * @since Series 60 2.6
     **/
    static TInt EventExpiredCallBack(TAny* aPtr);

    /**
     * Callback for iRefreshTimer.
     * @since Series 60 3.0
     **/
    static TInt RefreshTimerCallBack(TAny* aPtr);

 private:  // Data members
    // Calendar agenda engine
    CCalenEngine* iCalendarEngine;

    // ?one_line_short_description_of_data
    RPointerArray<CCalInstance> iInstanceArray;

    // ?one_line_short_description_of_data
    MPluginDataChangeObserver& iDataChangeObserver;

    // ?one_line_short_description_of_data
    TObserverType iObserverType;

    // ?one_line_short_description_of_data
    CEnvironmentChangeNotifier* iEnvironmentChangeNotifier;

    // ?one_line_short_description_of_data
    enum TDBState
    {
        EDBOffline,
        EDBInitializing,
        EDBInitialized,
        EDBOpening,
        EDBOpen,
        EDBReading
    };

    // ?one_line_short_description_of_data
    enum TFetchState
    {
        EFetchNormal,
        EFetchFuture
    };

    // ?one_line_short_description_of_data
    TDBState iDBState;

    // ?one_line_short_description_of_data
    TFetchState iFetchState;

    // ?one_line_short_description_of_data
    TBool iNeedsUpdate;

    // ?one_line_short_description_of_data
    TBool iHadEvents;

    // Timer for event expirations
    CEventExpirationTimer* iExpirationTimer;

    // timer for SafeRefresh
    CEventExpirationTimer* iRefreshTimer;

    // ?one_line_short_description_of_data
    CPropertyObserver* iEventChangeObserver;

    // ?one_line_short_description_of_data
    CPropertyObserver* iTodoChangeObserver;

    // Counter for SafeRefresh calls
    TUint iRefreshCounter;
    
    // Calendar session for receving notification
    CCalSession* iCalendarSession;

};

#endif  // CAIAGENDAPLUGINENGINEIMPL_H


// End of File
