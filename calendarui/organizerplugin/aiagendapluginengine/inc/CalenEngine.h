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
* Description:  
*
*/

#ifndef CALENENGINE_H
#define CALENENGINE_H

// INCLUDES
#include <e32base.h>
#include <calcommon.h> //TCalCollectionId

// FORWARD DECLARATIONS
class CCalSession;
class CCalInstance;
class CCalAlarm;

class CMultistepCommandBase;
class COpenCommand;
class CCalInstanceView;
class CCalCalendarInfo;

class MCalenEngineGeneralCallback;
class MCalenEngineOpenCallback;
class MCalenEngineEntryFetchCallback;

class TCalInfo
    {
public:
    TFileName iFileName;
    TUint32 iColor;
    TCalCollectionId iColltectionId;
    };


// CLASS DEFINITION
NONSHARABLE_CLASS( CCalenEngine ) : public CBase
{
private:
    friend class CActive;
    friend class COpenCommand;
    friend class CGetEntriesCommand;
    friend class CGetTodosCommand;
    friend class CGetNextEventCommand;
    friend class CGetEntriesForDaysCommand;

private:
    // hidden default constructor
    CCalenEngine();

public: // Factory method and destructor
    static CCalenEngine* NewL();
    virtual ~CCalenEngine();

public: // New methods
    void OpenDatabaseL(MCalenEngineOpenCallback& aCallback);

    void GetEntriesForDayL( MCalenEngineEntryFetchCallback& aCallback, 
                            const TTime& aDay, 
                            RPointerArray<CCalInstance>& aInstanceArray );

    void GetEventForNextSevenDaysL( MCalenEngineEntryFetchCallback& aCallback, 
                                    const TTime& aStartDay, 
                                    const TInt aDaysToCheck, 
                                    RPointerArray<CCalInstance>& aInstanceArray );

    void GetTodosL( MCalenEngineEntryFetchCallback& aCallback, 
                    RPointerArray<CCalInstance>& aInstanceArray );

    void GetCalendarDataL( MCalenEngineEntryFetchCallback& aCallback,
                           RPointerArray<CCalInstance>& aInstanceArray,
                           const TTime& aStartDay,
                           const TInt aDaysToSearch );

    void Close();
    
    RPointerArray<CCalSession>& GetActiveSessionsL();
    
    void UpdateCalSessionsL();
    
    void GetAllCalendarInfoL( );
    
    void Reset();
private:
    void StartCommand( CMultistepCommandBase* aCommand, 
                       MCalenEngineGeneralCallback& aCallback );
    void FinishCommand();

    void OpenCompleted();
    void GetEntriesCompleted();
    void GetTodosCompleted();
    void GetFutureEventCompleted();
    void GetCalendarDataCompleted();
    void HandleError(TInt aError);

    void ReleaseCalendarDatabase();

private: // data members
    CCalInstanceView* iInstanceView;
    MCalenEngineGeneralCallback* iCallback;
    CMultistepCommandBase* iCommand;
    
    RArray<TCalInfo> iCalInfoArray;
    RPointerArray<CCalSession> iSessionReferanceArray;
    RPointerArray<CCalSession> iSessionOwningArray;

};

#endif // CALENENGINE_H


// End of File
