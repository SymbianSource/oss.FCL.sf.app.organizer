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
* Description:  Implementation of Calendar services
*
*/

#include <gulicon.h>

#include "calendarui_debug.h"
#include "calendummyservices.h"
#include "calenglobaldata.h"
#include <mcalenpreview.h>

// ----------------------------------------------------------------------------
// CCalenDummyServices::CCalenDummyServices
// C++ constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDummyServices::CCalenDummyServices( CCalenGlobalData& aGlobalData )
    : iGlobalData( aGlobalData )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::~CCalenDummyServices
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDummyServices::~CCalenDummyServices()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::Release
// Cleans resources
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyServices::Release()
    {
    TRACE_ENTRY_POINT;
    delete this;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::SessionL
// Retrieve the calendar session currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalSession& CCalenDummyServices::SessionL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iGlobalData.CalSessionL();
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::EntryViewL
// Retrieve the calendar entry view currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalEntryView* CCalenDummyServices::EntryViewL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iGlobalData.EntryViewL();
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::InstanceViewL
// Retrieve the calendar instance view currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalInstanceView* CCalenDummyServices::InstanceViewL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iGlobalData.InstanceViewL();
    }
    
// ----------------------------------------------------------------------------
// CCalenDummyServices::SessionL
// Retrieve the calendar session currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalSession& CCalenDummyServices::SessionL(const TDesC& aCalendar )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iGlobalData.CalSessionL(aCalendar);
    }

CCalEntryView* CCalenDummyServices::EntryViewL(const TCalCollectionId aCollectionId)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iGlobalData.EntryViewL(aCollectionId);
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::InstanceViewL
// Retrieve the calendar instance view currently in use by Calendar
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalInstanceView* CCalenDummyServices::InstanceViewL(
                                    const RArray<TInt>& aCollectionIds )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iGlobalData.InstanceViewL(aCollectionIds);
    }
    

// ----------------------------------------------------------------------------
// CCalenDummyServices::RegisterForNotificationsL
// Register for notifications of Calendar events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyServices::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            TCalenNotification /*aNotification*/ )
    {
    TRACE_ENTRY_POINT; 

    iHandler = aHandler; 
  
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::RegisterForNotificationsL
// Register for notifications of Calendar events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyServices::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                           RArray<TCalenNotification>& /*aNotifications*/ )
    {
    TRACE_ENTRY_POINT; 

    iHandler = aHandler; 
  
    TRACE_EXIT_POINT;
    }    

// ----------------------------------------------------------------------------
// CCalenDummyServices::CancelNotifications
// Cancel notifications of Calendar events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyServices::CancelNotifications( MCalenNotificationHandler* /*aHandler*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::IssueCommandL
// Issue a command to be handled by Calendar or a customization
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenDummyServices::IssueCommandL( TInt /*aCommand*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::IssueNotificationL
// Issue a notification to Calendar, which will be broadcast synchronously
// to all registered notification handlers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyServices::IssueNotificationL( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    if ( iHandler )
        {
        iHandler->HandleNotification( aNotification );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::Context
// Returns the context.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
MCalenContext& CCalenDummyServices::Context()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iGlobalData.Context();
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::RequestActivationL
// Request activation of a specific view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyServices::RequestActivationL( const TVwsViewId& /*aViewId*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::ActivationNotificationL
// Notify Calendar that a specific view has been activated
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyServices::ActivationNotificationL( const TVwsViewId& /*aViewId*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::OfferMenuPaneL
// Offers the menu pane to plugins for customisation
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyServices::OfferMenuPaneL( TInt /*aResourceId*/,
                                          CEikMenuPane* /*aMenuPane*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::GetCommandRange
// Gets the command range that has been allocated to this MCalenServices
// instance
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDummyServices::GetCommandRange( TInt& /*aCommandRangeStart*/,
                                           TInt& /*aCommandRangeEnd*/ ) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::Infobar
// Descriptor past to plugins to get customised info bar text
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenDummyServices::Infobar( const TRect& /*aRect*/  )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::Infobar
// Descriptor past to plugins to get customised info bar text
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
const TDesC& CCalenDummyServices::Infobar()
    {
    TRACE_ENTRY_POINT;
    const TDesC& nullDesc = _L(" ");
    TRACE_EXIT_POINT;
    return nullDesc;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::CustomPreviewPaneL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
MCalenPreview* CCalenDummyServices::CustomPreviewPaneL( TRect& /*aRect*/ )
	{
	TRACE_ENTRY_POINT
	TRACE_EXIT_POINT
	return NULL;
	}
// ----------------------------------------------------------------------------
// CCalenDummyServices::PreviewPane
// Descriptor past to plugins to get customised info bar text
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenDummyServices::PreviewPane(  TRect& /*aRect*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::GetIconL
// Get icon of specific type
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenDummyServices::GetIconL( MCalenServices::TCalenIcons /*aIndex*/, const TInt /*aViewId*/  )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::QueueKeyEventL
// Queue key events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenDummyServices::QueueKeyEvent( const TKeyEvent& /*aEvent*/, TEventCode /*aType*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::GetQueuedKeyEventL
// Get queued key events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//    
TBool CCalenDummyServices::GetQueuedKeyEvent( TKeyEvent& /*aEvent*/, TEventCode& /*aType*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::ResetKeyEventQueue
// reset key event queue
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//    
void CCalenDummyServices::ResetKeyEventQueue()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenServicesImpl::InterimUtilsL()
// Retrieve interimutils from globaldata
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenInterimUtils2& CCalenDummyServices::InterimUtilsL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iGlobalData.InterimUtilsL();
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::ToolbarOrNull()
// Always returns NULL, CCalenDummyServices will never have a toolbar
// ----------------------------------------------------------------------------
MCalenToolbar* CCalenDummyServices::ToolbarOrNull()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::MissedAlarmStore()
// Always returns NULL, CCalenDummyServices will not use MissedAlarmStore
// ----------------------------------------------------------------------------
CMissedAlarmStore* CCalenDummyServices::MissedAlarmStore()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }	

// ----------------------------------------------------------------------------
// CCalenDummyServices::GetMissedAlarmsList()
// Always returns NULL, CCalenDummyServices will not use MissedAlarmslist
// ----------------------------------------------------------------------------
void CCalenDummyServices::GetMissedAlarmsList(RArray<TCalenInstanceId>& /*aMissedAlarmsList*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::GetMultipleCalendarData
// (other items were commented in a header).
// ----------------------------------------------------------------------------
// 
CCalenAttachmentModel* CCalenDummyServices::GetAttachmentData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT; 
	return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenDummyServices::GetAllCalendarInfoL
// Get all available calendar info
// ----------------------------------------------------------------------------
//     		
void CCalenDummyServices::GetAllCalendarInfoL( 
        RPointerArray<CCalCalendarInfo>& aCalendarInfoList )
	{
	TRACE_ENTRY_POINT;
	iGlobalData.GetAllCalendarInfoL(aCalendarInfoList);
	TRACE_EXIT_POINT; 
	}	

// ----------------------------------------------------------------------------
// CCalenDummyServices::GetCalFileNameForCollectionId
// Gets calendar filename from collection id
// ----------------------------------------------------------------------------
//     
const TDesC& CCalenDummyServices::GetCalFileNameForCollectionId(
        const TCalCollectionId aCollectionId)
    {
    TRACE_ENTRY_POINT;TRACE_EXIT_POINT;
    return iGlobalData.GetCalFileNameForCollectionId(aCollectionId);
    }
// End of file
