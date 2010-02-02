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
* Description:  This class is wrapper class of agenda engine.
*
*/


//debug
#include "calendarui_debug.h"

// INCLUDES
#include "CalenEngine.h"
#include "CalenAsyncCommands.h"
#include "CalEngineCallback.h"
#include "CleanupResetAndDestroy.h"


#include <calinstanceview.h>
#include <calsession.h>
#include <calcalendarinfo.h>
#include <calcalendariterator.h>

enum TCalenEnginePanic
    {
    EMultipleCommands = 0
    };



// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
static void Panic(TInt aReason) 
    {
    TRACE_ENTRY_POINT;
    
    _LIT(KPanicCategory, "CalenEngine");
    User::Panic(KPanicCategory, aReason);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
CCalenEngine::CCalenEngine() 
    { 
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
CCalenEngine* CCalenEngine::NewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenEngine* self = new (ELeave) CCalenEngine;
    //CleanupStack::PushL(self);
    //self->ConstructL();
    //CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
CCalenEngine::~CCalenEngine()
    {
    TRACE_ENTRY_POINT;
    
    Close();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::Close()
    {
    TRACE_ENTRY_POINT;
    
    if( iCommand )
        {
        // cancel all asynchronous operations before(!) deleting the object!
        iCommand->Cancel();
        delete iCommand;
        iCommand = NULL;
        }
    ReleaseCalendarDatabase();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEngine::GetActiveSessionsL
// -----------------------------------------------------------------------------
//
RPointerArray<CCalSession>& CCalenEngine::GetActiveSessionsL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iSessionReferanceArray;
    }

// -----------------------------------------------------------------------------
// CCalenEngine::UpdateCalSessionsL
// -----------------------------------------------------------------------------
//
void CCalenEngine::UpdateCalSessionsL()
    {
    TRACE_ENTRY_POINT;

    Reset();

    GetAllCalendarInfoL();

    CCalSession* defaultSession = CCalSession::NewL(); //Default session  
    iSessionOwningArray.AppendL(defaultSession);

    CCalSession* tempSession = NULL; //Temp session 
    for (TInt index = 0; index < iCalInfoArray.Count(); index++)
        {
        TPtrC calFileName = iCalInfoArray[index].iFileName;
        
        RDebug::Print(calFileName);
        if (!calFileName.CompareF(defaultSession->DefaultFileNameL()))
            {
            CleanupStack::PushL(defaultSession);
            defaultSession->OpenL(calFileName);
            CleanupStack::Pop(defaultSession);

            tempSession = defaultSession;
            iCalInfoArray[index].iColltectionId
                    = tempSession->CollectionIdL();
            }
        else
            {
            tempSession = CCalSession::NewL(*defaultSession);
            CleanupStack::PushL(tempSession);
            tempSession->OpenL(calFileName);
            CleanupStack::Pop(tempSession);
            iCalInfoArray[index].iColltectionId
                    = tempSession->CollectionIdL();

            iSessionOwningArray.AppendL(tempSession);
            }
        iSessionReferanceArray.AppendL(tempSession);
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEngine::GetAllCalendarsL
// -----------------------------------------------------------------------------
//
void CCalenEngine::GetAllCalendarInfoL( )
    {
    TRACE_ENTRY_POINT
    CCalSession* sessionIter = CCalSession::NewL();
    CleanupStack::PushL(sessionIter);
    CCalCalendarIterator* calIter = CCalCalendarIterator::NewL(*sessionIter);
    CleanupStack::PushL(calIter);

    for(CCalCalendarInfo* calendarInfo = calIter->FirstL() ;
        calendarInfo != NULL ; calendarInfo = calIter->NextL() )
        {
        if(calendarInfo->Enabled())
            {
            TCalInfo calInfo;
            calInfo.iFileName = calendarInfo->FileNameL();
            calInfo.iColor = calendarInfo->Color().Value();
            iCalInfoArray.AppendL(calInfo);
            }
        delete calendarInfo; 
        }
    
    CleanupStack::PopAndDestroy(calIter);
    CleanupStack::PopAndDestroy(sessionIter);
    TRACE_EXIT_POINT    
    }

// -----------------------------------------------------------------------------
// CCalenEngine::Reset
// -----------------------------------------------------------------------------
//
void CCalenEngine::Reset()
    {
    TRACE_ENTRY_POINT
    if(iCalInfoArray.Count())
        {
        iCalInfoArray.Reset();
        }
    
    if (iSessionReferanceArray.Count())
        {
        iSessionReferanceArray.Reset();
        }
    
    if (iSessionOwningArray.Count())
        {
        iSessionOwningArray.ResetAndDestroy();
        }
    TRACE_EXIT_POINT    
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::StartCommand( CMultistepCommandBase* aCommand,
                                 MCalenEngineGeneralCallback& aCallback )
    {
    TRACE_ENTRY_POINT;
    
    __ASSERT_ALWAYS(iCommand == NULL, Panic(EMultipleCommands));
    iCallback = &aCallback;
    iCommand = aCommand;
    iCommand->Start();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::FinishCommand()
    {
    TRACE_ENTRY_POINT;
    
    __ASSERT_ALWAYS(iCommand != NULL, User::Invariant());
    delete iCommand;
    iCommand = NULL;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::OpenDatabaseL(MCalenEngineOpenCallback& aCallback)
    {
    TRACE_ENTRY_POINT;
    
    StartCommand(new (ELeave) COpenCommand(*this), aCallback);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::OpenCompleted()
    {
    TRACE_ENTRY_POINT;
    
    FinishCommand();
    static_cast<MCalenEngineOpenCallback*>(iCallback)->OpenDatabaseCompleted();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::GetEntriesForDayL( MCalenEngineEntryFetchCallback& aCallback, 
                                               const TTime& aDay, 
                                               RPointerArray<CCalInstance>& aInstanceArray )
    {
    TRACE_ENTRY_POINT;
    
    StartCommand(
        CGetEntriesCommand::NewL( *this, aDay, aInstanceArray ), 
        aCallback );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::GetEventForNextSevenDaysL( MCalenEngineEntryFetchCallback& aCallback, 
                                                       const TTime& aStartDay, const TInt aDaysToSearch,
                                                       RPointerArray<CCalInstance>& aInstanceArray )
    {
    TRACE_ENTRY_POINT;
    
    StartCommand( 
        CGetNextEventCommand::NewL( *this, aStartDay, aDaysToSearch, aInstanceArray ), 
        aCallback );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::GetTodosL( MCalenEngineEntryFetchCallback& aCallback, 
                                       RPointerArray<CCalInstance>& aInstanceArray )
    {
    TRACE_ENTRY_POINT;

    StartCommand( 
        CGetTodosCommand::NewL( *this, aInstanceArray ), 
        aCallback );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::GetCalendarDataL( MCalenEngineEntryFetchCallback& aCallback,
                                    RPointerArray<CCalInstance>& aInstanceArray,
                                    const TTime& aStartDay,
                                    const TInt aDaysToSearch )
    {
    TRACE_ENTRY_POINT;

    StartCommand( 
        CGetEntriesForDaysCommand::NewL( *this, aInstanceArray, aStartDay, aDaysToSearch ), 
        aCallback );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::GetEntriesCompleted()
    {
    TRACE_ENTRY_POINT;

    FinishCommand();
    static_cast<MCalenEngineEntryFetchCallback*>(iCallback)->GetEntriesCompleted();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::GetTodosCompleted()
    {
    TRACE_ENTRY_POINT;

    FinishCommand();
    static_cast<MCalenEngineEntryFetchCallback*>(iCallback)->GetTodosCompleted();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::GetFutureEventCompleted(void)
    {
    TRACE_ENTRY_POINT;

    FinishCommand();
    static_cast<MCalenEngineEntryFetchCallback*>(iCallback)->GetFutureEventCompleted();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::GetCalendarDataCompleted(void)
    {
    TRACE_ENTRY_POINT;
    
    FinishCommand();
    static_cast<MCalenEngineEntryFetchCallback*>(iCallback)->GetCalendarDataCompleted();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::HandleError(TInt aError)
    {
    TRACE_ENTRY_POINT;

    FinishCommand();
    static_cast<MCalenEngineGeneralCallback*>(iCallback)->HandleError( aError );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?implementation_description
// -----------------------------------------------------------------------------
//
void CCalenEngine::ReleaseCalendarDatabase()
    {
    TRACE_ENTRY_POINT;

    delete iInstanceView;
    iInstanceView = NULL;
    
    Reset();
    
    TRACE_EXIT_POINT;
    }


// End of File
