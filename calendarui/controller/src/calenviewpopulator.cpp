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
* Description:  Calendar view populator active object
*
*/


// INCLUDES
#include <calencontext.h>

//debug
#include "calendarui_debug.h"
#include "calenviewpopulator.h"
#include "calencontroller.h"
#include "calenglobaldata.h"
#include "calennotifier.h"

#include <eikenv.h> // For CEikonEnv
#include <apgtask.h>

// ----------------------------------------------------------------------------
// CCalenViewPopulator::NewL
// 1st phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenViewPopulator* CCalenViewPopulator::NewL( CCalenController& aController )
    {
    TRACE_ENTRY_POINT;

    CCalenViewPopulator* self = new( ELeave ) CCalenViewPopulator( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenViewPopulator::CCalenViewPopulator
// C++ constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenViewPopulator::CCalenViewPopulator( CCalenController& aController )
    : CActive( EPriorityStandard ),
      iController( aController ),
      iNeedsInstanceView( EFalse ),
      iNeedsEntryView( EFalse )
    {
    TRACE_ENTRY_POINT;

    CActiveScheduler::Add( this );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewPopulator::ConstructL
// 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewPopulator::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iController.RegisterForNotificationsL( this, ECalenNotifyEntryInstanceViewCreated );
    iGlobalData = CCalenGlobalData::InstanceL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewPopulator::~CCalenViewPopulator
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenViewPopulator::~CCalenViewPopulator()
    {
    TRACE_ENTRY_POINT;

    Cancel();
    iController.CancelNotifications( this );

    if( iGlobalData )
        {
        iGlobalData->Release();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewPopulator::DoCancel
// From CActive. Do nothing.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewPopulator::DoCancel()
    {
    TRACE_ENTRY_POINT;

    iStatus = KErrCancel;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewPopulator::RunL
// From CActive.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewPopulator::RunL()
    {
    TRACE_ENTRY_POINT;

    ASSERT( iPopulatingView );
    iPopulationStep = iPopulatingView->ActiveStepL();

    switch( iPopulationStep )
        {
        case CCalenView::EKeepGoing:
        	{
        	SetActive();
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
        	}
            break;
        case CCalenView::EWaitForInstanceView:
        	{
        	RArray<TInt> colIdArray;
        	iController.GetActiveCollectionidsL(colIdArray);
        	if(colIdArray.Count() > 0)
        	    {
        	    if( iGlobalData->InstanceViewL(colIdArray) )
        	        {
        	        SetActive();
        	        TRequestStatus* status = &iStatus;
        	        User::RequestComplete( status, KErrNone );
        	        }
        	    else
        	        {
        	        iNeedsInstanceView = ETrue;
        	        // Wait for notification.
        	        }
        	    }
        	else 
        	    {
        	    if( iGlobalData->InstanceViewL() )
                    {
                    SetActive();
                    TRequestStatus* status = &iStatus;
                    User::RequestComplete( status, KErrNone );
                    }
                else
                    {
                    iNeedsInstanceView = ETrue;
                    // Wait for notification.
                    }
        	    }
        	colIdArray.Reset();
        	 }
            break;
        case CCalenView::EWaitForEntryView:
            {
            if( iGlobalData->EntryViewL() )
                {
                SetActive();
                TRequestStatus* status = &iStatus;
                User::RequestComplete( status, KErrNone );
                }
            else
                {
                iNeedsEntryView = ETrue;
                // Wait for notification.
                }
            }
            break;
        case CCalenView::EDone:
        	{
        	iPopulatingView = NULL;
            iController.Notifier().BroadcastNotification( ECalenNotifyViewPopulationComplete );
            
            if( iController.IsLaunchFromExternalApp() )
                {
                CEikonEnv* eikenv = CEikonEnv::Static();
                iController.SetLaunchFromExternalApp( EFalse );
                const TUid KCalendarAppUID = { 0x10005901 };
                TApaTaskList taskList( eikenv->WsSession() );
                TApaTask task = taskList.FindApp( KCalendarAppUID );
                task.BringToForeground();
                }
        	}
            break;
        default:
            ASSERT( EFalse );
            break;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewPopulator::BeginPopulationL
// Starts populating the given view.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewPopulator::BeginPopulationL( CCalenView* aView )
    {
    TRACE_ENTRY_POINT;

    ASSERT( !iPopulatingView );
    iPopulatingView = aView;
    iPopulatingView->CancelPopulation();
    iPopulationStep = CCalenView::EKeepGoing;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewPopulator::InterruptPopulationL
// Stops current population.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewPopulator::InterruptPopulationL()
    {
    TRACE_ENTRY_POINT;

    if( iPopulatingView )
        {
        Cancel();
        // This doesn't go in DoCancel because the active object might not be
        // active during population (i.e. when waiting for instance/entryview).
        iPopulatingView->CancelPopulation();
        iPopulatingView = NULL;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewPopulator::HandleECalenNotifyViewCreatedL
// Leaving function that handles ECalenNotifyViewCreated
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewPopulator::HandleECalenNotifyViewCreatedL()
    {
    TRACE_ENTRY_POINT;
    
    RArray<TInt> colIdArray;
    iController.GetActiveCollectionidsL(colIdArray);

    if( iPopulatingView )
        {//When commandline is compatiable with new globaldata functionalities, the collectionid 
        // to EntryView can be given
        if( ( iNeedsEntryView && iGlobalData->EntryViewL(/*iGlobalData->Context().InstanceId().iColId*/) ) ||
            ( iNeedsInstanceView && iGlobalData->InstanceViewL(colIdArray) ) )
            {
            SetActive();
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
            }
        }

    if( iGlobalData->EntryViewL(/*iGlobalData->Context().InstanceId().iColId*/) 
                                            && iGlobalData->InstanceViewL(colIdArray) )
        {
        iController.CancelNotifications( this );
        }
    
    colIdArray.Reset();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewPopulator::HandleNotification
// Calls back when the entry view or instance view has been created.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewPopulator::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    if ( aNotification == ECalenNotifyEntryInstanceViewCreated )
        {
        PIM_TRAPD_HANDLE( HandleECalenNotifyViewCreatedL() );
        }

    TRACE_EXIT_POINT;
    }

// End of file
