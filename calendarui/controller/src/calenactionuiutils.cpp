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
* Description:  Implementation of UI utils
*
*/

// System Includes
#include <QtGui>
#include <hblabel.h>
#include <hbaction.h>
#include <hbinputdialog.h>
#include <HbMessageBox>
#include <agendautil.h>
#include <agendaentry.h>

// User includes
#include "calendarui_debug.h"
#include "calenactionuiutils.h"
#include "calendar.hrh"
#include "hb_calencommands.hrh"
#include "caleninstanceid.h"
#include "CleanupResetAndDestroy.h"
#include "calendateutils.h"

// -----------------------------------------------------------------------------
// CCalenCommonUI::FindPossibleInstanceL
// Finds an instance with the given instance id and instance view.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
AgendaEntry CalenActionUiUtils::findPossibleInstanceL(const TCalenInstanceId& id,
                                                         AgendaUtil* agendaUtil )
    {
    TRACE_ENTRY_POINT;
    
    QList<AgendaEntry> instances = agendaUtil->createEntryIdListForDay(id.mInstanceTime,
                                                                       AgendaUtil::FilterFlags(AgendaUtil::IncludeAll));
    AgendaEntry result;

    // For instances finishing the next day (now possible with unified DateTime editor),
    // we have to do our best to match the instance time exactly - otherwise we could
    // match the LocalUid to the incorrect instance in a series.
    for ( TInt i=0; i < instances.count() && (result.isNull()); ++i )
        {
        if( instances[i].id() == id.mEntryLocalUid )
            {
            // Check the instance time matches.
            if( instances[i].startTime() == id.mInstanceTime )
                {
                result = instances[i];
                instances.removeAt(i);
                }
            }
        }

    if( result.isNull() )
        {
        // Couldn't match the instance time exactly - just use the instance
        // with the same LocalUid as the one we're looking for.
        for ( TInt i=0; i < instances.count() && (result.isNull()); ++i )
            {
            if( instances[i].id() == id.mEntryLocalUid )
                {
                result = instances[i];
                instances.removeAt(i);
                }
            }
        }

    TRACE_EXIT_POINT;
    return result;
    }


// ----------------------------------------------------------------------------
// CalenActionUiUtils::SetToDoCompleteStatusL
// Mark to-do entry as completed or restore a completed to-do and save.
// This is not in the engine layer because it shows an error mesage on failure.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenActionUiUtils::setToDoCompleteStatus( AgendaUtil* agendaUtil,
                                                 AgendaEntry& entry,
                                                 const bool status )
    {
    TRACE_ENTRY_POINT;

    ASSERT( !entry.isNull() );
    QDateTime now = QDateTime::currentDateTime();

    // set as completed or restore and update the entry in the database
    agendaUtil->setCompleted(entry, status, now)

    TRACE_EXIT_POINT;
    }

int CalenActionUiUtils::showDeleteConfirmationQueryL(const TDeleteConfirmationType type,
                                                     const int count)
    {
    TRACE_ENTRY_POINT;
    int retStatus = 0;

    
    HbMessageBox popup(HbMessageBox::MessageTypeQuestion);
    popup.setIconVisible(true);
    popup.setTimeout(HbPopup::NoTimeout);
       
    QString text = 0;
    
    switch(type)
        {
        case CalenActionUiUtils::EDeleteEntry:
            {
            // TODO: Add the text id
            text.append("Delete entry?");
            break;
            }
        case CalenActionUiUtils::EDeleteToDo:
            {
            text.append(hbTrId("txt_calendar_info_delete_todo_note"));
            break;
            }
        case CalenActionUiUtils::EDeleteToDos:
            {//"Delete %N to-do notes?"
            // TODO: Add the text id
            text.append("Delete %N to-do's?").arg(count);
            break;
            }
        case CalenActionUiUtils::EDeleteAll:
            {
            text.append(hbTrId("txt_calendar_info_delete_all_calendar_entries"));
            break;
            }
        default:
            break;
        }
    
    popup.setText(text);
    		
	popup.setPrimaryAction(new HbAction(
								hbTrId("txt_calendar_button_delete"), &popup));
	popup.setSecondaryAction(new HbAction(
								hbTrId("txt_calendar_button_cancel"), &popup));
	HbAction *selected = popup.exec();
	if (selected == popup.primaryAction()) { 
		retStatus = 1;
	}

    TRACE_EXIT_POINT
    return retStatus;
    }



// End of file
