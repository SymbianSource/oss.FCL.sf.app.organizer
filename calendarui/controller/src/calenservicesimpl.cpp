/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of Calendar services
*
*/

// System includes
#include <agendautil.h>
#include <hbwidget.h>

#include "calendarui_debug.h"
#include "calenservicesimpl.h"
#include "calencontroller.h"
#include "calenviewmanager.h"



// ----------------------------------------------------------------------------
// CalenServicesImpl::NewL
// Two phased constructor.
// Taking the command range start and end points
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenServicesImpl* CalenServicesImpl::NewL( TInt aCommandRangeStart,
                                                                  TInt aCommandRangeEnd )
    {
    TRACE_ENTRY_POINT;
    CalenServicesImpl* self = new( ELeave ) CalenServicesImpl(
                                       aCommandRangeStart, aCommandRangeEnd );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::NewL
// 1st phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenServicesImpl* CalenServicesImpl::NewL()
    {
    TRACE_ENTRY_POINT;
    CalenServicesImpl* self = new( ELeave ) CalenServicesImpl( 0, 0 );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::CalenServicesImpl
// C++ constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenServicesImpl::CalenServicesImpl( TInt aCommandRangeStart,
                                                        TInt aCommandRangeEnd )
    : iCommandRangeStart( aCommandRangeStart ),
      iCommandRangeEnd( aCommandRangeEnd )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::ConstructL
// 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iController = CCalenController::InstanceL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::Release
// Performs cleanup of this object.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::Release()
    {
    TRACE_ENTRY_POINT;

    if( iController )
        {
        iController->Release();
        }  
    
    delete this;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::SessionL
// Retrieve the calendar session currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
HbMainWindow& CalenServicesImpl::MainWindow()
    {
	TRACE_ENTRY_POINT;
    ASSERT( iController ); // Make sure ConstructL has been called.
	TRACE_EXIT_POINT;
    return iController->MainWindow();
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::RegisterForNotificationsL
// Register for notification of Calendar event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iController ); // Make sure ConstructL has been called.
    iController->RegisterForNotificationsL( aHandler, aNotification );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::RegisterForNotificationsL
// Register for array notifications of Calendar events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            RArray<TCalenNotification>& aNotifications )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iController ); // Make sure ConstructL has been called.
    iController->RegisterForNotificationsL( aHandler, aNotifications );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::CancelNotifications
// Cancel notifications of Calendar events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::CancelNotifications( MCalenNotificationHandler* aHandler )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iController ); // Make sure ConstructL has been called.
    iController->CancelNotifications( aHandler );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::IssueCommandL
// Issue a command to be handled by Calendar or a customization
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CalenServicesImpl::IssueCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;
    ASSERT( iController ); // Make sure ConstructL has been called.

    TRACE_EXIT_POINT;
    return iController->IssueCommandL( aCommand );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::IssueNotificationL
// Issue a notification to Calendar, which will be broadcast synchronously
// to all registered notification handlers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::IssueNotificationL( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iController ); // Make sure ConstructL has been called.
    iController->BroadcastNotification( aNotification );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::RequestActivationL
// Request activation of a specific view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::RequestActivationL( const TVwsViewId& /*aViewId*/ )
    {
    //ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.
   // iController->RequestActivationL( aViewId );
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::ActivationNotificationL
// Notify Calendar that a specific view has been activated
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::ActivationNotificationL( const TVwsViewId& /*aViewId*/ )
    {
    //ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::Context
// Returns the context.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
MCalenContext& CalenServicesImpl::Context()
    {
	TRACE_ENTRY_POINT;
    ASSERT( iController ); // Make sure ConstructL has been called.
    TRACE_EXIT_POINT;
    return iController->context();
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::Infobar
// Descriptor passed to plugins to get customised info bar text
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
HbWidget* CalenServicesImpl::Infobar( )
    {    
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iController->Infobar();
    }
// ----------------------------------------------------------------------------
// CCalenServicesImpl::InfobarTextL
// @returns info bar text
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
QString* CalenServicesImpl::InfobarTextL()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iController->InfobarTextL();
    }
// ----------------------------------------------------------------------------
// CalenServicesImpl::GetCommandRange
// Returns the assigned command range
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::GetCommandRange( TInt& aCommandRangeStart, 
                                                              TInt& aCommandRangeEnd ) const
    {
    TRACE_ENTRY_POINT;

    aCommandRangeStart = iCommandRangeStart;
    aCommandRangeEnd = iCommandRangeEnd;

    TRACE_EXIT_POINT;
    }
// ----------------------------------------------------------------------------
// CalenServicesImpl::OfferMenu
// Offers the menu to plugins for customisation
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenServicesImpl::OfferMenu(HbMenu* aHbMenu )
    {
    TRACE_ENTRY_POINT;

    iController->OfferMenu(aHbMenu);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::agendaInterface
// returns the interface to the agenda database
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
AgendaUtil* CalenServicesImpl::agendaInterface()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return iController->agendaInterface();
    
    }

// ----------------------------------------------------------------------------
// CalenServicesImpl::getFirstView
// returns the first view with which calendar has been launched
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
int CalenServicesImpl::getFirstView()
{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	
	return iController->getFirstView();
}

// End of file
