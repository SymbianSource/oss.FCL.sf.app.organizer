/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class manages Calendar Server's connection to Agenda Server
*                and Calendar database. 
*
*/



#ifndef CALENSVRDBMANAGER_H
#define CALENSVRDBMANAGER_H

// INCLUDES
#include "PropertyObserver.h"
#include <calprogresscallback.h>    // MCalProgressCallBack

#include <e32base.h>

// FORWARD DECLARATIONS
class CCalenGlobalData;

/**
 * CCalenSvrDBManager manages Calendar Server's connection to Agenda Server
 * and Calendar database. It optimizes behavior so that Agenda Server
 * is not opened and closed unnecessarily often in short period of time, as
 * opening can be costly operation as it requires
 * 1) Starting Agenda Server process
 * 2) Opening database and creating indexes. Indexes are stored in
 *   Agenda Server, and reused by all clients of same database. 
 * 
 * It uses Calendar Engine to do actual opening. It owns Calendar Engine.
 */
NONSHARABLE_CLASS( CCalenSvrDBManager ) : 
    public CBase, 
    public MPropertyChangeHandler,
    public MCalProgressCallBack

    {
public:
    NONSHARABLE_CLASS( MCalenDBUser )
        {
    public:
        virtual void DatabaseOpened() = 0;
        virtual void DatabaseTemporarilyClosed() = 0;
    	virtual void HandleError() = 0;  
        };

public: // Construction and destruction
    static CCalenSvrDBManager* NewL();
    virtual ~CCalenSvrDBManager();

public: // New methods 
    /** Called by Calendar server, when booting is ready 
     */
    void BootReadyL();

    /** Register new user for Calendar database. Database is kept 
     * open as long as there are users. 
     */
    void RegisterUserL(MCalenDBUser& aUser); 

    /** Unregister user from Calendar database. When there are no more 
     * users, CCalenSvrDBManager sets small delay and closes database
     * session. 
     */
    void UnregisterUserL(MCalenDBUser& aUser);

public: // From PropertyObserver
    void HandlePropertyChange(const TUid& aCategory, const TUint& aKey, const TInt& aValue);
   
public: // from   MCalProgressCallBack
    void Completed(TInt aError);
    TBool NotifyProgress();
    void Progress(TInt aPercentageCompleted);

private:
    void OpenDatabaseCompletedL(TInt aErrorVal = KErrNone);

private: // Timer handling
    void StartClosingTimer();
    void ClosingTimeOut();
    static TInt ClosingTimeOutCallback(TAny* aAny);

private: // Restore handling
    void RestoreStarted();
    void RestoreFinishedL();

private: // construction 
    CCalenSvrDBManager();
    void ConstructL();

private: // new functions
    /**
     *
     **/
    void NotifyUsersL(TInt aErrorVal = KErrNone);

private:
    enum TState 
    {
        EStateBooting, // FIXME: if unregister works correctly this is not needed
        EStateOpeningDB,
        EStateDBOpen,
        EStateGoingToCloseDB, 
        EStateDBClosed,
        EStateRestoring
    };

    TState iState;
    RPointerArray<MCalenDBUser> iUsers;
    CPeriodic* iClosingTimer;

    CCalenGlobalData* iGlobalData;

    // ?one_line_short_description_of_data
    CPropertyObserver* iBackupObserver;

    };

#endif // CALENSVRDBMANAGER_H


// End of File
