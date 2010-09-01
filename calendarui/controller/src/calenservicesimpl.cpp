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

#include <gulicon.h>
#include <calcalendarinfo.h>
#include <mcalenpreview.h>

#include "calenglobaldata.h"
#include "calendarui_debug.h"
#include "calenservicesimpl.h"
#include "calencontroller.h"
#include "calenviewmanager.h"
#include "calenicons.h"
#include "calenattachmentmodel.h"

// ----------------------------------------------------------------------------
// CCalenServicesImpl::NewL
// Two phased constructor.
// Taking the command range start and end points
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenServicesImpl* CCalenServicesImpl::NewL( TInt aCommandRangeStart,
                                                                  TInt aCommandRangeEnd )
    {
    TRACE_ENTRY_POINT;
    CCalenServicesImpl* self = new( ELeave ) CCalenServicesImpl(
                                       aCommandRangeStart, aCommandRangeEnd );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::NewL
// 1st phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenServicesImpl* CCalenServicesImpl::NewL()
    {
    TRACE_ENTRY_POINT;
    CCalenServicesImpl* self = new( ELeave ) CCalenServicesImpl( 0, 0 );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::CCalenServicesImpl
// C++ constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenServicesImpl::CCalenServicesImpl( TInt aCommandRangeStart,
                                                        TInt aCommandRangeEnd )
    : iCommandRangeStart( aCommandRangeStart ),
      iCommandRangeEnd( aCommandRangeEnd )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::ConstructL
// 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenServicesImpl::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iGlobalData = CCalenGlobalData::InstanceL();
    iController = CCalenController::InstanceL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::Release
// Performs cleanup of this object.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenServicesImpl::Release()
    {
    TRACE_ENTRY_POINT;

    if( iGlobalData )
        {
        iGlobalData->Release();
        }

    if( iController )
        {
        iController->Release();
        }

    delete this;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::SessionL
// Retrieve the calendar session currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalSession& CCalenServicesImpl::SessionL()
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.

    TRACE_EXIT_POINT;
    return iGlobalData->CalSessionL();
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::EntryViewL
// Retrieve the calendar entry view currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalEntryView* CCalenServicesImpl::EntryViewL()
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.

    TRACE_EXIT_POINT;
    return iGlobalData->EntryViewL();
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::InstanceViewL
// Retrieve the calendar instance view currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalInstanceView* CCalenServicesImpl::InstanceViewL()
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.

    TRACE_EXIT_POINT;
    return iGlobalData->InstanceViewL();
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::InterimUtilsL()
// Retrieve interimutils from globaldata
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenInterimUtils2& CCalenServicesImpl::InterimUtilsL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iGlobalData->InterimUtilsL();
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::RegisterForNotificationsL
// Register for notification of Calendar event
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenServicesImpl::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.
    iController->RegisterForNotificationsL( aHandler, aNotification );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::RegisterForNotificationsL
// Register for array notifications of Calendar events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenServicesImpl::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            RArray<TCalenNotification>& aNotifications )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.
    iController->RegisterForNotificationsL( aHandler, aNotifications );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::CancelNotifications
// Cancel notifications of Calendar events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenServicesImpl::CancelNotifications( MCalenNotificationHandler* aHandler )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.
    iController->CancelNotifications( aHandler );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::IssueCommandL
// Issue a command to be handled by Calendar or a customization
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenServicesImpl::IssueCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;
    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.

    TRACE_EXIT_POINT;
    return iController->IssueCommandL( aCommand );
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::IssueNotificationL
// Issue a notification to Calendar, which will be broadcast synchronously
// to all registered notification handlers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenServicesImpl::IssueNotificationL( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.
    iController->BroadcastNotification( aNotification );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::Context
// Returns the context.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
MCalenContext& CCalenServicesImpl::Context()
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.

    TRACE_EXIT_POINT;
    return iGlobalData->Context();
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::RequestActivationL
// Request activation of a specific view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenServicesImpl::RequestActivationL( const TVwsViewId& aViewId )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.
    iController->RequestActivationL( aViewId );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::ActivationNotificationL
// Notify Calendar that a specific view has been activated
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenServicesImpl::ActivationNotificationL( const TVwsViewId& /*aViewId*/ )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::OfferMenuPaneL
// Offers the menu pane to plugins for customisation
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenServicesImpl::OfferMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    TRACE_ENTRY_POINT;

    iController->OfferMenuPaneL( aResourceId, aMenuPane );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::GetCommandRange
// Returns the assigned command range
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenServicesImpl::GetCommandRange( TInt& aCommandRangeStart, 
        TInt& aCommandRangeEnd ) const
    {
    TRACE_ENTRY_POINT;

    aCommandRangeStart = iCommandRangeStart;
    aCommandRangeEnd = iCommandRangeEnd;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::Infobar
// Descriptor passed to plugins to get customised info bar text
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenServicesImpl::Infobar(const TRect& aRect )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iController->Infobar( aRect );
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::Infobar
// Returns the customized text from plugin
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
const TDesC& CCalenServicesImpl::Infobar()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iController->Infobar();
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::CustomPreviewPaneL
// Returns the customized Preview pane
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
MCalenPreview* CCalenServicesImpl::CustomPreviewPaneL( TRect& aRect )
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iController->CustomPreviewPaneL( aRect );	
	}

// ----------------------------------------------------------------------------
// CCalenServicesImpl::PreviewPane
// Descriptor passed to plugins to get customised info bar text
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenServicesImpl::PreviewPane( TRect& aRect )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iController->PreviewPane( aRect );
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::ToolbarOrNull
// Provides access to the calendar toolbar if one is available
// ----------------------------------------------------------------------------
MCalenToolbar* CCalenServicesImpl::ToolbarOrNull()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iController->ViewManager().ToolbarOrNull();
    }
    
// ----------------------------------------------------------------------------
// CCalenServicesImpl::GetIconL
// Get icon of specific type
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenServicesImpl::GetIconL( MCalenServices::TCalenIcons aIndex, const TInt aViewId ) 
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iController->GetIconL( aIndex, aViewId );
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::QueueKeyEvent
// Queue key events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenServicesImpl::QueueKeyEvent( const TKeyEvent& aEvent, 
                                                           TEventCode aType )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iGlobalData->QueueKeyEvent( aEvent, aType );
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::GetQueuedKeyEventL
// Get queued key events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//    
TBool CCalenServicesImpl::GetQueuedKeyEvent( TKeyEvent& aEvent, 
                                                                  TEventCode& aType )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iGlobalData->GetQueuedKeyEvent( aEvent, aType );
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::ResetKeyEventQueue
// reset key event queue
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//    
void CCalenServicesImpl::ResetKeyEventQueue()
    {
    TRACE_ENTRY_POINT;

    iGlobalData->ResetKeyEventQueue();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::MissedAlarmStore
// Returns a reference to the Missed Alarm Store
// ----------------------------------------------------------------------------
CMissedAlarmStore* CCalenServicesImpl::MissedAlarmStore()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iController->MissedAlarmStore();
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::GetMissedAlarmsList
// Get missed alarms list
// ----------------------------------------------------------------------------
void CCalenServicesImpl::GetMissedAlarmsList(RArray<TCalenInstanceId>& aMissedAlarmsList)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iController->GetMissedAlarmsList(aMissedAlarmsList);
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::GetAttachmentData
// returns ref of CCalenAttachmentModel
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 
CCalenAttachmentModel* CCalenServicesImpl::GetAttachmentData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return &iController->AttachmentData();
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::SessionL
// Retrieve the calendar session currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalSession& CCalenServicesImpl::SessionL(const TDesC& aCalendar )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.

    TRACE_EXIT_POINT;
    return iGlobalData->CalSessionL(aCalendar);
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::EntryViewL
// Retrieve the calendar entry view currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalEntryView* CCalenServicesImpl::EntryViewL(const  TCalCollectionId aCollectionId )
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.

    TRACE_EXIT_POINT;
    return iGlobalData->EntryViewL(aCollectionId);
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::InstanceViewL
// Retrieve the calendar instance view currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalInstanceView* CCalenServicesImpl::InstanceViewL(
                                        const RArray<TInt>& aCollectionIds)
    {
    TRACE_ENTRY_POINT;

    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.

    TRACE_EXIT_POINT;
    return iGlobalData->InstanceViewL(aCollectionIds);
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::GetAllCalendarInfoL
// Get all available calendar info
// -----------------------------------------------------------------------------
//
void CCalenServicesImpl::GetAllCalendarInfoL(
        RPointerArray<CCalCalendarInfo>& aCalendarInfoList )
    {
    TRACE_ENTRY_POINT;
    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.
    iGlobalData->GetAllCalendarInfoL(aCalendarInfoList);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenGlobalData::GetCalFileNameForCollectionId
// Get calendar file name for the given collection id
// -----------------------------------------------------------------------------
//
const TDesC& CCalenServicesImpl::GetCalFileNameForCollectionId(
        const TCalCollectionId aCollectionId)
    {
    TRACE_ENTRY_POINT;
    ASSERT( iGlobalData && iController ); // Make sure ConstructL has been called.
    TRACE_EXIT_POINT;
    return iGlobalData->GetCalFileNameForCollectionId(aCollectionId);
    }

// End of file
