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
* Description:  Provides asynchronous wrapper functions for
 *                common InterimAPI operations.
 *
*/


//debug
#include "calendarui_debug.h"

// INCLUDES
#include "CalenAsyncCommands.h"
#include "CalenEngine.h"
#include "calenagendautils.h"
#include "calendateutils.h"
#include "CleanupResetAndDestroy.h"

#include <calsession.h>
#include <calcommon.h>
#include <calinstance.h>
#include <calinstanceview.h>

#include "CalenPaths.h"

////////////////////////////////////////////////////////////////////////
// CMultiStepCommandBase method definitions
///////////////////////////////////////////////////////////////////////

CMultistepCommandBase::CMultistepCommandBase(CCalenEngine& aEngine) :
    CActive( 0 ),
    iEngine( aEngine )
    {
    TRACE_ENTRY_POINT;
    
    CActiveScheduler::Add( this );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CMultistepCommandBase::~CMultistepCommandBase()
    {
    TRACE_ENTRY_POINT;
    
    /* !!! NOTE !!!
     * Do NOT call Cancel() in here.
     * Calling Cancel() will cause the code to panic!
     * Cancel() can only be called in a class that implements
     * the DoCancel() function.
     */
     
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMultistepCommandBase::Start()
    {
    TRACE_ENTRY_POINT;
    
    CompleteSelf();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CMultistepCommandBase::CompleteSelf()
    {
    TRACE_ENTRY_POINT;
    
    TRequestStatus* pStat = &iStatus;
    if( IsAdded() )
        {
        if (!IsActive())
            {
            SetActive();
            }
        User::RequestComplete( pStat, KErrNone );
        }
    
    TRACE_EXIT_POINT;
    }


////////////////////////////////////////////////////////////////////////
// COpenCommand method definitions
///////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
COpenCommand::COpenCommand(CCalenEngine& aEngine)
    : CMultistepCommandBase( aEngine ),
      iState( ECreateSession ),
      iAgendaError( KErrNone )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
COpenCommand::~COpenCommand()
    {
    TRACE_ENTRY_POINT;
    
    Cancel();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void COpenCommand::Panic(TInt aReason)
    {
    TRACE_ENTRY_POINT;
    
    __ASSERT_DEBUG( EFalse, User::Panic( _L("COpenCommand"), aReason ) );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void COpenCommand::RunL()
    {
    TRACE_ENTRY_POINT;
    
    switch( iState )
        {
        case ECreateSession:
            {
            iEngine.UpdateCalSessionsL();
            
            iState = ECreateView;
            CompleteSelf();
            }
            break;

        case ECreateView:
            {
            
            if(iEngine.iInstanceView)
                {
                delete iEngine.iInstanceView;
                iEngine.iInstanceView = NULL;
                }
            
            RPointerArray<CCalSession>& sessionArray  =  iEngine.GetActiveSessionsL();
            // Creating a view will create the index
            // This is an asynch function:
            // -> we don't complete ourselves here, agenda model calls back to the Completed method.
            
            
            if(sessionArray.Count())
                {
                iEngine.iInstanceView = CCalInstanceView::NewL( sessionArray, *this );
                }
            else
                {
                iState = ESignalCaller;
                CompleteSelf();
                }
            }
            break;

        case ESignalCaller:
            {
            User::LeaveIfError( iAgendaError );
            iState = EReady;
            iEngine.OpenCompleted();
            }
            break;

        default:
            {
            Panic( EInvalidState );
            }
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void COpenCommand::DoCancel()
    {
    TRACE_ENTRY_POINT;

    iState = ECreateSession;
    iEngine.ReleaseCalendarDatabase();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void COpenCommand::Completed(TInt aError)
    {
    TRACE_ENTRY_POINT;

    iAgendaError = aError;
    iState = ESignalCaller;
    CompleteSelf();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void COpenCommand::Progress(TInt /*aPercentageCompleted*/)
    {
    TRACE_ENTRY_POINT;
    
    // do not care about progress, only completion matters
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool COpenCommand::NotifyProgress()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt COpenCommand::RunError(TInt aError)
    {
    TRACE_ENTRY_POINT;
    
    // suppress any error
    iEngine.ReleaseCalendarDatabase();
    iEngine.HandleError( aError );
    
    TRACE_EXIT_POINT;
    return KErrNone;
    }

////////////////////////////////////////////////////////////////////////
// CGetEntrisCommand method definitions
///////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetEntriesCommand* CGetEntriesCommand::NewL( CCalenEngine& aEngine,
                                              const TTime& aDay,
                                              RPointerArray<CCalInstance>& aInstanceArray )
    {
    TRACE_ENTRY_POINT;

    CGetEntriesCommand* self = new( ELeave )CGetEntriesCommand( aEngine, aDay, aInstanceArray );
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetEntriesCommand::CGetEntriesCommand( CCalenEngine& aEngine,
                                        const TTime& aDay,
                                        RPointerArray<CCalInstance>& aInstanceArray )
    : CMultistepCommandBase( aEngine ),
      iState( EFetch ),
      iDay( aDay ),
      iInstanceArray( aInstanceArray )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetEntriesCommand::~CGetEntriesCommand()
    {
    TRACE_ENTRY_POINT;

    Cancel();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetEntriesCommand::RunL(void)
    {
    TRACE_ENTRY_POINT;

    switch( iState )
        {
        case EFetch:
            {
            if( !iEngine.iInstanceView )
                {
                iState = EFinalize;
                CompleteSelf();
                }
            else
                {

                    CalCommon::TCalViewFilter filter = CalCommon::EIncludeAppts    | 
                                                       CalCommon::EIncludeReminder | 
                                                       CalCommon::EIncludeEvents   | 
                                                       CalCommon::EIncludeAnnivs;

                    CalenAgendaUtils::FindEventsForDayRangeL( iEngine.iInstanceView, iInstanceArray, filter, iDay, iDay );

                    CalenAgendaUtils::RemoveEntriesEndingAtMidnightL( iInstanceArray, iDay );

                    iState = EFinalize;
                    CompleteSelf();
                }

            }
            break;

        case EFinalize:
            {

                iDay = CalenDateUtils::BeginningOfDay( iDay );

                // Remove all events starting before iDay
                for(TInt i(0); i<iInstanceArray.Count(); )
                    {
                    if( ( iInstanceArray[i]->Entry().EntryTypeL() == CCalEntry::EAppt ||
                          iInstanceArray[i]->Entry().EntryTypeL() == CCalEntry::EReminder )
                        &&
                        iInstanceArray[i]->StartTimeL().TimeLocalL() < iDay )
                        {
                        delete iInstanceArray[i];
                        iInstanceArray.Remove( i );
                        }
                    else
                        {
                        i++;
                        }
                    }

                CalenAgendaUtils::SortInstanceList( iInstanceArray );

                iState = ESignalCaller;
                CompleteSelf();

            }
            break;

        case ESignalCaller:
            {
            iState = EReady;
            iEngine.GetEntriesCompleted();
            }
            break;

        default:
            {
            Panic( EInvalidState );
            }
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetEntriesCommand::DoCancel(void)
    {
    TRACE_ENTRY_POINT;

    iState = EFetch;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CGetEntriesCommand::RunError(TInt aError)
    {
    TRACE_ENTRY_POINT;

    iEngine.HandleError( aError );
    
    TRACE_EXIT_POINT;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetEntriesCommand::Panic(TInt aReason)
    {
    TRACE_ENTRY_POINT;

    __ASSERT_DEBUG( EFalse, User::Panic( _L("CGetEntriesCommand"), aReason ) );
    
    TRACE_EXIT_POINT;
    }


////////////////////////////////////////////////////////////////////////
// CGetNextEventCommand method definitions
///////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetNextEventCommand* CGetNextEventCommand::NewL( CCalenEngine& aEngine, 
                                                  const TTime& aStartDay, 
                                                  const TInt aDaysToSearch, 
                                                  RPointerArray<CCalInstance>& aInstanceArray )
    {
    TRACE_ENTRY_POINT;
    
    CGetNextEventCommand* self = new( ELeave )CGetNextEventCommand( aEngine, aStartDay, aDaysToSearch, aInstanceArray );
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetNextEventCommand::CGetNextEventCommand(CCalenEngine& aEngine, const TTime& aStartDay, const TInt aDaysToSearch, RPointerArray<CCalInstance>& aInstanceArray)
    : CMultistepCommandBase( aEngine ),
      iState( EFetch ),
      iStartDay( aStartDay ),
      iDaysToSearch( aDaysToSearch ),
      iInstanceArray( aInstanceArray )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetNextEventCommand::~CGetNextEventCommand(void)
    {
    TRACE_ENTRY_POINT;
    
    Cancel();
    iTmpArray.ResetAndDestroy();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetNextEventCommand::RunL(void)
    {
    TRACE_ENTRY_POINT;
    
    switch( iState )
        {
        case EFetch:
            {
            if( !iEngine.iInstanceView )
                {
                iState = EFinalize;
                CompleteSelf();
                }
            else
                {
                // create date range and fetch entries
    
                CalCommon::TCalViewFilter filter = CalCommon::EIncludeAppts    | 
                                                   CalCommon::EIncludeReminder;

                TTime endDay = iStartDay +TTimeIntervalDays( iDaysToSearch-1 );

                CalenAgendaUtils::FindEventsForDayRangeL( iEngine.iInstanceView, iTmpArray, filter, iStartDay, endDay );

                iState = EFinalize;
                CompleteSelf();
                }
            }
            break;

        case EFinalize:
            {
                if( iTmpArray.Count() )
                    {
                    CalenAgendaUtils::SortInstanceList( iTmpArray );

                    iStartDay = CalenDateUtils::BeginningOfDay( iStartDay );

                    // Find first event starting on iStartDay or later
                    for(TInt i(0); i<iTmpArray.Count(); i++)
                        {
                        if( iTmpArray[i]->StartTimeL().TimeLocalL() >= iStartDay )
                            {
                            iInstanceArray.AppendL( iTmpArray[i] );
                            iTmpArray.Remove( i );
                            break;
                            }
                        }
                    }

                iTmpArray.ResetAndDestroy();
                iState = ESignalCaller;
                CompleteSelf();
            }
            break;

        case ESignalCaller:
            {
            iState = EReady;
            iEngine.GetFutureEventCompleted();
            }
            break;

        default:
            {
            Panic( EInvalidState );
            }
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetNextEventCommand::DoCancel(void)
    {
    TRACE_ENTRY_POINT;
    
    iState = EFetch;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CGetNextEventCommand::RunError(TInt aError)
    {
    TRACE_ENTRY_POINT;
    
    iTmpArray.ResetAndDestroy();
    iEngine.HandleError( aError );
    
    TRACE_EXIT_POINT;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetNextEventCommand::Panic(TInt aReason)
    {
    TRACE_ENTRY_POINT;
    
    __ASSERT_DEBUG( EFalse, User::Panic( _L("CGetNextEventCommand"), aReason ) );
    
    TRACE_EXIT_POINT;
    }


///////////////////////////////////////////////////////////////
// CGetTodos
///////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetTodosCommand* CGetTodosCommand::NewL( CCalenEngine& aEngine,
                                          RPointerArray<CCalInstance>& aInstanceArray )
    {
    TRACE_ENTRY_POINT;
    
    CGetTodosCommand* self = new( ELeave )CGetTodosCommand( aEngine, aInstanceArray );
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetTodosCommand::CGetTodosCommand(CCalenEngine& aEngine, RPointerArray<CCalInstance>& aInstanceArray)
    : CMultistepCommandBase( aEngine ),
      iState( EFetchInstances ),
      iInstanceArray( aInstanceArray )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetTodosCommand::~CGetTodosCommand()
    {
    TRACE_ENTRY_POINT;
    
    Cancel();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetTodosCommand::RunL(void)
    {
    TRACE_ENTRY_POINT;

    switch( iState )
        {
        case EFetchInstances:
            {
            TCalTime dummy;
            CalCommon::TCalTimeRange dayRange( dummy, dummy );
            CalenDateUtils::GetDayRangeL( TCalTime::MinTime(), TCalTime::MaxTime(), dayRange );

            if( !iEngine.iInstanceView )
                {
                iState = ESignalCaller;
                CompleteSelf();
                }
            else
                {
                iEngine.iInstanceView->FindInstanceL( iInstanceArray, 
                                                      CalCommon::EIncludeIncompletedTodos |
                                                      // only fetch the first instance for repeating to-dos!
                                                      CalCommon::EIncludeRptsNextInstanceOnly,
                                                      dayRange );

                iState = ESignalCaller;
                CompleteSelf();
                }
            }
            break;

        case ESignalCaller:
            {
            iState = EReady;
            iEngine.GetTodosCompleted();
            }
            break;

        default:
            {
            Panic( EInvalidState );
            }
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetTodosCommand::DoCancel(void)
    {
    TRACE_ENTRY_POINT;

    iState = EFetchInstances;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CGetTodosCommand::RunError(TInt aError)
    {
    TRACE_ENTRY_POINT;
    
    iEngine.HandleError( aError );
    
    TRACE_EXIT_POINT;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetTodosCommand::Panic(TInt aReason)
    {
    TRACE_ENTRY_POINT;

    __ASSERT_DEBUG( EFalse, User::Panic( _L("CGetTodosCommand"), aReason ) );
    
    TRACE_EXIT_POINT;
    }


////////////////////////////////////////////////////////////////////////
// CGetEntriesForDaysCommand method definitions
///////////////////////////////////////////////////////////////////////


// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetEntriesForDaysCommand* CGetEntriesForDaysCommand::NewL( CCalenEngine& aEngine,
                                                            RPointerArray<CCalInstance>& aInstanceArray,
                                                            const TTime& aDay,
                                                            TInt aDaysToSearch )
    {
    TRACE_ENTRY_POINT;
    
    CGetEntriesForDaysCommand* self = new( ELeave )CGetEntriesForDaysCommand( aEngine, aInstanceArray, aDay, aDaysToSearch );
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetEntriesForDaysCommand::CGetEntriesForDaysCommand( CCalenEngine& aEngine,
                                                      RPointerArray<CCalInstance>& aInstanceArray,
                                                      const TTime& aDay,
                                                      TInt aDaysToSearch )
    : CMultistepCommandBase( aEngine ),
      iState( EFetch ),
      iInstanceArray( aInstanceArray ),
      iDay( aDay ),
      iDaysToSearch( aDaysToSearch )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CGetEntriesForDaysCommand::~CGetEntriesForDaysCommand()
    {
    TRACE_ENTRY_POINT;

    Cancel();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetEntriesForDaysCommand::RunL(void)
    {
    TRACE_ENTRY_POINT;
    
    switch( iState )
        {
        case EFetch:
            {
            if( !iEngine.iInstanceView )
                {
                iState = EFinalize;
                CompleteSelf();
                }
            else
                {
                    TTime endDay = iDay +TTimeIntervalDays( iDaysToSearch-1 );
                    CalCommon::TCalViewFilter filter = CalCommon::EIncludeAppts    | 
                                                       CalCommon::EIncludeReminder | 
                                                       CalCommon::EIncludeEvents   | 
                                                       CalCommon::EIncludeAnnivs;

                    CalenAgendaUtils::FindTodosForDayRangeL( iEngine.iInstanceView, iInstanceArray, iDay, endDay );
                    CalenAgendaUtils::FindEventsForDayRangeL( iEngine.iInstanceView, iInstanceArray, filter, iDay, endDay );
                    CalenAgendaUtils::RemoveEntriesEndingAtMidnightL( iInstanceArray, iDay );

                    iState = EFinalize;
                    CompleteSelf();
                }
            }
            break;

        case EFinalize:
            {
                // remove all cancelled meetings
                for( TInt i(0); i<iInstanceArray.Count(); )
                    {
                    CCalEntry& entry = iInstanceArray[i]->Entry();

                    if( entry.EntryTypeL() == CCalEntry::EAppt && 
                        entry.StatusL() == CCalEntry::ECancelled )
                        {
                        delete iInstanceArray[i];
                        iInstanceArray.Remove( i );
                        }
                    else
                        {
                        i++;
                        }
                    }

                CalenAgendaUtils::SortInstanceList( iInstanceArray );

                iState = ESignalCaller;
                CompleteSelf();
            }
            break;

        case ESignalCaller:
            {
            iState = EReady;
            iEngine.GetCalendarDataCompleted();
            }
            break;

        default:
            {
            Panic( EInvalidState );
            }
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetEntriesForDaysCommand::DoCancel(void)
    {
    TRACE_ENTRY_POINT;

    iState = EFetch;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CGetEntriesForDaysCommand::RunError(TInt aError)
    {
    TRACE_ENTRY_POINT;
    
    iEngine.HandleError( aError );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CGetEntriesForDaysCommand::Panic(TInt aReason)
    {
    TRACE_ENTRY_POINT;
    
    __ASSERT_DEBUG( EFalse, User::Panic( _L("CGetEntriesForDaysCommand"), aReason ) );
    
    TRACE_EXIT_POINT;
    }


// End of File
