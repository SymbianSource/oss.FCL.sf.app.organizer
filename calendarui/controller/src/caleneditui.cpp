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
* Description:  Controller class for editing related UI behavior
 *
*/

// System includes.
#include <QDateTime>

// User includes.
#include "calendarui_debug.h"
#include "caleneditui.h"
#include "calencontroller.h"
#include "hb_calencommands.hrh"
#include "calenservicesimpl.h"
#include "agendaentry.h"
#include "caleneditor.h"
#include "calendateutils.h"

// ----------------------------------------------------------------------------
// CCalenEditUi::NewL
// First stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenEditUi* CCalenEditUi::NewL( CCalenController& aController )
    {
    TRACE_ENTRY_POINT;

    CCalenEditUi* self = new(ELeave) CCalenEditUi( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::~CCalenEditUi
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenEditUi::~CCalenEditUi()
    {
    TRACE_ENTRY_POINT;
    iController.CancelNotifications(this);
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::CCalenEditUi
// Constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenEditUi::CCalenEditUi( CCalenController& aController )
    : iController( aController )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::ConstructL
// Second stage construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    iController.RegisterForNotificationsL( this, ECalenNotifyDialogClosed );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::HandleActionUiCommandL
// Handles action ui commands
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenEditUi::HandleCommandL( const TCalenCommand& aCommand )
    {
    TRACE_ENTRY_POINT;

    TBool continueCommand(EFalse);
    TInt command = aCommand.Command();
    MCalenContext& context = iController.Services().Context();
    AgendaEntry editedEntry;
    QDateTime newEntryDateTime = iController.context().focusDateAndTime();
   
    // Check if it is not on same day and set the default time and date accordingly.
    bool isSameDay = CalenDateUtils::isOnToday(newEntryDateTime);
    if (!isSameDay) {
    	newEntryDateTime = CalenDateUtils::defaultTime(newEntryDateTime);
    }
    switch ( command )
        {
        case ECalenNewMeeting:
			iEditor= new CalenEditor(iController.Services().agendaInterface());
			iEditor->create(newEntryDateTime, false, CalenEditor::TypeAppointment );
		    connect(iEditor, SIGNAL(entrySaved()), this, SLOT(handleEntrySaved()));
		    connect(iEditor, SIGNAL(dialogClosed()), this, SLOT(handleDialogClosed()));
            break;
        case ECalenEditCurrentEntry:            
            editedEntry = iController.Services().agendaInterface()->fetchById(
                                    context.instanceId().mEntryLocalUid );
            // For repeating entry, we need to update proper start time here
            // from the context
            if(!editedEntry.isNull()) {
            	if (editedEntry.isRepeating()) {
            		// Get the instancde time frm the context
            		QDateTime instanceTime = context.focusDateAndTime();
            		editedEntry.setStartAndEndTime(instanceTime, editedEntry.endTime());
            	}
            	iEditor= new CalenEditor(iController.Services().agendaInterface());
            	iEditor->edit(editedEntry, false);
            	connect(iEditor, SIGNAL(entrySaved()), this, SLOT(handleEntrySaved()));
            	connect(iEditor, SIGNAL(dialogClosed()), this, SLOT(handleDialogClosed()));
            }
            break;
        default:
            // Controller decided this class was the place to handle this
            // command
            break;
        }
    
    TRACE_EXIT_POINT;
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenEditUi::handleEntrySaved
// Issues the entry saved notification after receiving signal from editor. 
// ----------------------------------------------------------------------------
//
void CCalenEditUi::handleEntrySaved()
{
	iController.Services().IssueNotificationL(ECalenNotifyEntrySaved);
}

// ----------------------------------------------------------------------------
// CCalenEditUi::handleEntrySaved
// Issues the dialog closed notification after receiving signal from editor. 
// ----------------------------------------------------------------------------
//
void CCalenEditUi::handleDialogClosed()
{
	iController.Services().IssueNotificationL(ECalenNotifyDialogClosed);
}

// ----------------------------------------------------------------------------
// CCalenEditUi::HandleNotification
// Handle notifications
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEditUi::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    if( aNotification == ECalenNotifyDialogClosed )
		{
    	iEditor->deleteLater();
		}
    TRACE_EXIT_POINT;
    }

// End of file
