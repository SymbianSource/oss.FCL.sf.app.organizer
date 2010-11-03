/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The Source file for the CCalProgressCallBackListener class.
*
*/


// INCLUDES
#include <calsession.h>
#include <calentryview.h>
#include <calinstanceview.h>

#include "CalProgressCallBackListener.h"

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalProgressCallBackListener* CCalProgressCallBackListener::NewL()
    {
    CCalProgressCallBackListener* self = new (ELeave) CCalProgressCallBackListener;
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::~CCalProgressCallBackListener
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalProgressCallBackListener::~CCalProgressCallBackListener()
    {
    
    if( iInstanceView )
        {
        delete iInstanceView;
        iInstanceView = NULL;
        }
    if( iEntryView )
        {
        delete iEntryView;
        iEntryView = NULL;
        }
    if( iAsyncWait )
        {
        if( iAsyncWait->IsStarted() )
            {
            iAsyncWait->AsyncStop();
            }
        delete iAsyncWait;
        iAsyncWait = NULL;
        }
    if( iSession )
        {
        delete iSession;
        iSession = NULL;
        }
    
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::Progress
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalProgressCallBackListener::Progress( TInt /*aPercentageCompleted*/ )
    {
    // Nothing.
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::Completed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalProgressCallBackListener::Completed( TInt aError )
    {
    // Stop the scheduler.
    if( iAsyncWait->IsStarted() )
        {
        iAsyncWait->AsyncStop();
        }
        
    // Update the status.
    if( KErrNone == aError )
        {
        iReqStatus = ECalViewCreateCompleted;
        }
    else
        {
        iReqStatus = ECalViewCreateError;
        }
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::NotifyProgress
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalProgressCallBackListener::NotifyProgress()
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::CalSession
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalSession* CCalProgressCallBackListener::CalSession()
    {
    
    if( iSession )
        {
        return iSession;
        }
    return NULL;
    
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::CalEntryView
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalEntryView* CCalProgressCallBackListener::CalEntryView()
    {
    
    if( iEntryView )
        {
        return iEntryView;
        }
    return NULL;
    
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::CalInstanceView
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalInstanceView* CCalProgressCallBackListener::CalInstanceView()
    {
    
    if( iInstanceView )
        {
        return iInstanceView;
        }
    return NULL;
    
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::DeleteAllEntryL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalProgressCallBackListener::DeleteAllEntriesL()
    {
    
    if( iEntryView && !( iAsyncWait->IsStarted() ) )
        {
        TCalTime startTime;
        TCalTime endTime;
        startTime.SetTimeLocalL( TCalTime::MinTime() );
        endTime.SetTimeLocalL( TCalTime::MaxTime() );
        CalCommon::TCalTimeRange timeRange( startTime, endTime );
        
        // Delete the entries.
        iReqStatus = ECalViewCreateReqPending;
        iEntryView->DeleteL( timeRange, CalCommon::EIncludeAll , *this);
        
        // Start the scheduler wait.
        iAsyncWait->Start();
        }
    
    // Panic if there was an error.
    User::LeaveIfError( iReqStatus );
    
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::CCalProgressCallBackListener
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalProgressCallBackListener::CCalProgressCallBackListener()
    {
    // Nothing.
    }

// -----------------------------------------------------------------------------
// CCalProgressCallBackListener::ConstructL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalProgressCallBackListener::ConstructL()
    {
    
    // Construct the CCalSession object and open a calendar file.
    iSession = CCalSession::NewL();
    TRAPD( error, iSession->OpenL( _L( "c:Calendar" ) ) );
    if( KErrNotFound == error )
        {
        iSession->CreateCalFileL( _L( "c:Calendar" ) );
        // No need to TRAP now, we've already created the file.
        iSession->OpenL( _L( "c:Calendar" ) );
        }
    else
        {
        User::LeaveIfError( error );
        }
    
    // Active scheduler wait object to wait for alendar entry view creation.
    iAsyncWait = new(ELeave) CActiveSchedulerWait();
    if( !iEntryView && !iAsyncWait->IsStarted() )
        {
        // Construct the CCalEntryView object.
        iEntryView = CCalEntryView::NewL( *iSession, *this );
        iReqStatus = ECalViewCreateReqPending;
        
        // Start the scheduler.
        iAsyncWait->Start();
        }
    // Check for Entry view creation status.
    User::LeaveIfError( iReqStatus );
    
    if( !iInstanceView && !iAsyncWait->IsStarted() )
        {
        // Construct the CCalInstanceView object.
        iInstanceView = CCalInstanceView::NewL( *iSession, *this );
        iReqStatus = ECalViewCreateReqPending;
        
        // Start the scheduler.
        iAsyncWait->Start();
        }
    // Check for Instance view creation status.
    User::LeaveIfError( iReqStatus );
    
    }

// End of file
