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
* Description:  Handles deletion
*
*/

// System includes
#include <QString>
#include <QtGui>
#include <hbdatetimepicker.h>
#include <hbdialog.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbradiobuttonlist.h>
#include <centralrepository.h>
#include <agendaentry.h>

// User includes
#include "calendarui_debug.h"
#include "calendeleteui.h"
#include "hb_calencommands.hrh"
#include "calencontext.h"
#include "calencontroller.h"
#include "CleanupResetAndDestroy.h"
#include "caleninstanceid.h"
#include "calenactionuiutils.h"
#include "CalendarPrivateCRKeys.h"  // includes CalendarInternalCRKeys.h
#include "calendateutils.h"
#include "calenagendautils.h"


// Local constants
const TInt KEntriesToDelete = 1;

// ----------------------------------------------------------------------------
// CalenDeleteUi::NewL
// Two phased constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenDeleteUi* CalenDeleteUi::NewL( CCalenController& aController )
    {
    TRACE_ENTRY_POINT;

    CalenDeleteUi* self = new( ELeave ) CalenDeleteUi( aController, NULL );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::CalenDeleteUi
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenDeleteUi::CalenDeleteUi( CCalenController& aController, QObject *parent )
    :QObject(parent), iController( aController )
    {
    TRACE_ENTRY_POINT;
    iIsDeleting = false;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::~CalenDeleteUi
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenDeleteUi::~CalenDeleteUi()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::ConstructL
// Second phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::ConstructL()
    {
    TRACE_ENTRY_POINT;
    iController.RegisterForNotificationsL( this, ECalenNotifyCancelDelete );
    iMoreEntriesToDelete = EFalse;
    iDisplayQuery = EFalse;
    iEntriesToDelete = KEntriesToDelete;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::HandleNotification
// Handles notifications.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    if( aNotification == ECalenNotifyCancelDelete)
        {
        if(iMutlipleContextIdsCount)
            {
            // get the context
            MCalenContext& context = iController.context();
            // reset the multiple contexts
            context.resetMultipleContextIds();          
           
            }
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::HandleCommandL
// Handles action ui commands
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CalenDeleteUi::HandleCommandL( const TCalenCommand& aCommand )
    {
    TRACE_ENTRY_POINT;
    TBool continueCommand(EFalse);
   
    switch( aCommand.Command() )
        {
        case ECalenDeleteCurrentEntry:
            
            DeleteCurrentEntryL(); // Entry & instance
            break;

        case ECalenDeleteSeries:
            DeleteThisOrAllL( AgendaUtil::ThisAndAll );
            break;
            
        case ECalenDeleteCurrentOccurrence:
            DeleteThisOrAllL( AgendaUtil::ThisOnly );
            break;
            
        case ECalenDeleteEntryWithoutQuery:
            continueCommand = DeleteEntryWithoutQueryL();
            break;

        case ECalenDeleteAllEntries:
            DeleteAllEntriesL(); // EntryView & instance
            break;

        case ECalenDeleteEntriesBeforeDate:
            DeleteEntriesBeforeDateL(); // EntryView & instance
            break;

        default:
            // Controller decided this class was the place to handle this
            // command but it wasn't in our list; something has gone wrong.
            //ASSERT( EFalse );
            break;
        }
        
    TRACE_EXIT_POINT;
    return continueCommand;
    }


// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteThisOrAllL
// Deletes series repeating entry
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteThisOrAllL( AgendaUtil::RecurrenceRange aRepeatType )
    {
    TRACE_ENTRY_POINT;
    
    bool isDeleted( false );
    
    if( iController.context().instanceId().mEntryLocalUid )
        {
        AgendaEntry instance = 
            CalenActionUiUtils::findPossibleInstanceL(iController.context().instanceId(),
                                                   iController.Services().agendaInterface() );
         if( !instance.isNull() )
            {
            isDeleted = DeleteSingleInstanceL( instance, aRepeatType );
            }
        }
    iController.BroadcastNotification( isDeleted? ECalenNotifyEntryDeleted :
                                                               ECalenNotifyDeleteFailed );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteEntryWithoutQueryL()
// Deletes the current entry
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CalenDeleteUi::DeleteEntryWithoutQueryL()
    {
    TRACE_ENTRY_POINT;
	bool continueCommand(EFalse);
    
	// get the context
	MCalenContext& context = iController.context();
	
	// get the multliple context ids count
	iMutlipleContextIdsCount = context.mutlipleContextIdsCount();
	
	ASSERT( iMutlipleContextIdsCount );

	    if(!iMoreEntriesToDelete)
		    {
		    iDisplayQuery = ShowMultipleEntriesDeleteQueryL(iMutlipleContextIdsCount);
		    }
		
		if(iDisplayQuery)
			{
			if(!iMoreEntriesToDelete)	
			    {
			    DisplayWaitDialogL();
			    }
			// get the multiple context instance ids
			QList<TCalenInstanceId>& multipleContextIds = context.getMutlipleContextIds();
			
			if(iMutlipleContextIdsCount <= iEntriesToDelete )
			    {
			    iMoreEntriesToDelete = EFalse; 
			    iEntriesToDelete = iMutlipleContextIdsCount;
			    }
			else
			    {
			    iMoreEntriesToDelete = ETrue;
	            // set the continue command flag if more entries are there to delete
			    continueCommand = ETrue;
			    }
			    
			TInt index(0);
			while(index<iEntriesToDelete)
			    {
			    // get the local uid of the entry through multiple context list
			    ulong entryLocalUid = multipleContextIds[0].mEntryLocalUid;
			    if(entryLocalUid)
			        {
			        DeleteEntryL(entryLocalUid);
			        }
			    // remove mutliple context based on the instanceid
			    context.removeMultipleContextId(multipleContextIds[0]);
			    index++;
			   }

			if(!iMoreEntriesToDelete)
			    {
			    MarkedEntriesDeletedL();    
			    }
			}
        else
            {
            context.resetMultipleContextIds();
            // notify delete failed
            iController.BroadcastNotification(ECalenNotifyDeleteFailed);    
            }
	
    TRACE_EXIT_POINT;
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteCurrentEntryL
// Deletes the current entry
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteCurrentEntryL()
{
    TRACE_ENTRY_POINT;

    bool deleted( false );
    TCalenNotification notification = ECalenNotifyDeleteFailed;
    
    // Make sure we're focused on an entry.
    if (iController.context().instanceId().mEntryLocalUid) {
        // Fetch the entry
        AgendaEntry entry = iController.Services().agendaInterface()->fetchById(
                                iController.context().instanceId().mEntryLocalUid);
        
        // Check if the entry is a To-Do
        if (AgendaEntry::TypeTodo == entry.type()) {
            if(!entry.isNull()) {
                deleted = DeleteEntryL(entry);
                if(deleted) {
                    // Delete is successful
                    notification = ECalenNotifyEntryDeleted;
                }
            }
        }
        else {
            if (entry.isRepeating() || !entry.recurrenceId().isNull()) {
                // Show a confirmation note whether the user
                // wants to delete the single instance or all of them
                showRepeatingEntryDeleteQuery();
            } else {
                // If the entry is not a repeating entry,
                // delete it directly
                deleted = DeleteEntryL(entry);
                if (deleted) {
                    // Delete is successful
                    notification = ECalenNotifyEntryDeleted;
                }
            }
        }
    }

    iController.BroadcastNotification( notification );

    TRACE_EXIT_POINT;
}

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteAllEntriesL
// Deletes all entries
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteAllEntriesL()
    {
    TRACE_ENTRY_POINT;

    ASSERT( !iIsDeleting );

    const int buttonId = CalenActionUiUtils::showDeleteConfirmationQueryL( 
                                                                        CalenActionUiUtils::EDeleteAll );   
    if( buttonId )
        {
        HandleDeleteMultipleEventsL( AgendaUtil::minTime(), AgendaUtil::maxTime(),1 );
        }
    else
        {
        // notify delete failed
        iController.BroadcastNotification(ECalenNotifyDeleteFailed); 
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteEntriesBeforeDateL
// Deletes all entries before a set date.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteEntriesBeforeDateL()
    {
    TRACE_ENTRY_POINT;

    ASSERT( !iIsDeleting );

    TCalenNotification notification = ECalenNotifyDeleteFailed;
    
    // launch the datepicker
    mIsDateValid = false;
    dateQuery();

    if( mIsDateValid )
        {
        // Do delete only if inputted day is after beginning of range
        if( mDateTime > AgendaUtil::minTime() )
            {
            // Two pass delete:
            // 1. pass
            // To prevent destroying entries starting and ending midnight
            // subtract one second and do delete on that range.
            mDateTime = mDateTime.addSecs(-1);
            mDateTime = ( mDateTime > AgendaUtil::minTime()? mDateTime :  AgendaUtil::minTime());

            HandleDeleteMultipleEventsL( AgendaUtil::minTime(),
                                                    mDateTime,1 );
            notification = ECalenNotifyMultipleEntriesDeleted;
            }
        }

    iController.BroadcastNotification( notification );
    TRACE_EXIT_POINT;
    }
    
void CalenDeleteUi::dateQuery()
	{
	// Create a popup with datepicker to select the date.
	QDateTime currentDateTime(CalenDateUtils::today());
	QDate currentDate(currentDateTime.date());
	mDatePicker = new  HbDateTimePicker(currentDate);
	mDatePicker->setMinimumDate(CalenDateUtils::minTime().date());
	mDatePicker->setMaximumDate(currentDate);
	HbDialog popUp;
	popUp.setDismissPolicy(HbDialog::NoDismiss);
	popUp.setTimeout(HbDialog::NoTimeout );
	popUp.setHeadingWidget(new HbLabel(hbTrId("txt_calendar_title_set_date")));
	popUp.setContentWidget(mDatePicker);  
	HbAction *okAction = new HbAction(hbTrId("txt_calendar_button_dialog_delete"));
	popUp.setPrimaryAction(okAction);
	connect(okAction,SIGNAL(triggered()),this,SLOT(getSelectedDate()));
	connect(okAction,SIGNAL(triggered()),&popUp,SLOT(close()));
	popUp.setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"),&popUp));
	popUp.exec();
	}

void CalenDeleteUi::showRepeatingEntryDeleteQuery()
{
    HbDialog popUp;
    popUp.setDismissPolicy(HbDialog::NoDismiss);
    popUp.setTimeout(HbDialog::NoTimeout);
    
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    HbWidget *editWidget = new HbWidget();
    editWidget->setLayout(layout);
    
    HbRadioButtonList *editButtonList = new HbRadioButtonList();
    
    QStringList list;
    list << hbTrId("txt_calendar_info_this_occurrence_only")
    		<< hbTrId("txt_calendar_info_all_occurences");

    editButtonList->setItems(list);

    layout->addItem(editButtonList);
    
    popUp.setContentWidget(editWidget);
    popUp.setHeadingWidget(new HbLabel(
						hbTrId("txt_calendar_title_delete_repeated_entry")));

    connect(editButtonList, SIGNAL(itemSelected(int)), this,
            SLOT(handleRepeatedEntryDelete(int)));
    connect(editButtonList, SIGNAL(itemSelected(int)), &popUp, SLOT(close()));

    // Create secondary action
    HbAction *cancelAction = new HbAction(
							hbTrId("txt_calendar_button_softkey1_cancel"));
    popUp.setSecondaryAction(cancelAction);
    connect(cancelAction, SIGNAL(triggered()), this, SLOT(handleCancel()));
    connect(cancelAction, SIGNAL(triggered()), &popUp, SLOT(close()));
    
    // Show the popup
    popUp.exec();
}

void CalenDeleteUi::getSelectedDate()
	{  
    QDate selectedDate(mDatePicker->date());
        
	// Check if the date is within the range.
	if(selectedDate.isValid())
		{
		QTime time(0,0,0,0);
		mDateTime.setDate(selectedDate);
		mDateTime.setTime(time);
		mIsDateValid = true;
		}
    }

void CalenDeleteUi::handleRepeatedEntryDelete(int index)
{
    // Fetch the entry
    // Find all possible instances
    AgendaEntry instance = CalenActionUiUtils::findPossibleInstanceL(
                                iController.context().instanceId(),
                                iController.Services().agendaInterface());

    if (!instance.isNull()) {
        connect(iController.Services().agendaInterface(), SIGNAL(entryDeleted(ulong)),
                this, SLOT(entryDeleted(ulong)));
        switch(index) {
            case 0:
                // User wants to delete only this occurence
                iController.Services().agendaInterface()->deleteRepeatedEntry(instance, AgendaUtil::ThisOnly);
                break;
            case 1:
                // User wants to delete all the occurences
                iController.Services().agendaInterface()->deleteRepeatedEntry(instance, AgendaUtil::ThisAndAll);
                break;
        }
    }
}

void CalenDeleteUi::entryDeleted(ulong id)
{
    if (iController.context().instanceId().mEntryLocalUid == id) {
        iController.BroadcastNotification(ECalenNotifyEntryDeleted);
    }
    disconnect(iController.Services().agendaInterface(), SIGNAL(entryDeleted(ulong)),
               this, SLOT(entryDeleted(ulong)));
}

// ----------------------------------------------------------------------------
// CalenDeleteUi::HandleDeleteMultipleEventsL
// Handles multiple delete events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::HandleDeleteMultipleEventsL( const QDateTime& aFirstDay,
                                                 const QDateTime& aLastDay,
                                                 int aConfNoteId )
    {
    TRACE_ENTRY_POINT;

    
    ASSERT( !iIsDeleting );
    iConfirmationNoteId = aConfNoteId;
    
    DeleteDayRangeL( aFirstDay, aLastDay );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteDayRangeL
// Deletes all entries in a given range.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteDayRangeL( const QDateTime& aStart,
                                                      const QDateTime& aEnd )
    {
    TRACE_ENTRY_POINT;

    iStartTime = aStart;
    iEndTime = aEnd;
    
    iIsDeleting = ETrue;
    
    //AgendaUtil& agendaInterface = iController.agendaInterface();
    // Connect to the signal that gets generated when deletion is completed
    connect(iController.agendaInterface(), SIGNAL(entriesDeleted(int)), this,
            SLOT(doCompleted(int)));
    AgendaUtil::FilterFlags filter =
    	        AgendaUtil::FilterFlags(AgendaUtil::IncludeAnniversaries
    	                | AgendaUtil::IncludeAppointments
    	                | AgendaUtil::IncludeEvents
    	                | AgendaUtil::IncludeReminders
    	                | AgendaUtil::IncludeIncompletedTodos);
    // 1: First pass, delete all entries.
    iController.agendaInterface()->deleteEntries(iStartTime, iEndTime, filter);

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::DoCompletedL
// Handles delete callback
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::doCompleted( int aFirstPassError )
    {
    TRACE_ENTRY_POINT;

    if( aFirstPassError == KErrNone )
        {
        // 2: Second pass, delete notes that end 00:00 of next day of iEndTime
        // We didn't delete them in first pass
        QDateTime nextMidnight = CalenDateUtils::beginningOfDay( iEndTime.addDays(1) );
        nextMidnight = ( nextMidnight < AgendaUtil::maxTime()? nextMidnight : AgendaUtil::maxTime() );
        PIM_TRAPD_HANDLE( deleteEntriesEndingAtMidnight( nextMidnight ) );
        }

    // 3. End deleting, close wait dialog, and show confirmation or error note
    iIsDeleting = EFalse;
    
    iController.BroadcastNotification( ECalenNotifyMultipleEntriesDeleted );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenDeleteUi::DeleteEntriesEndingAtMidnightL
// Deletes entries ending at midnight on the given day
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenDeleteUi::deleteEntriesEndingAtMidnight( QDateTime aMidnight )
    {
    TRACE_ENTRY_POINT;

    QDateTime start, end;
    QTime startTime(aMidnight.time());
    startTime.setHMS(startTime.hour(), startTime.minute() - 1,
                    startTime.second(), startTime.msec());
    start.setDate(aMidnight.date());
    start.setTime(startTime);
    
    end = aMidnight;
    
    QList<AgendaEntry> instances = iController.Services().agendaInterface()->fetchEntriesInRange( start, end );

    // 2. loop through them and delete those entries that end at midnight
    for( int i=0; i < instances.count(); ++i )
        {
        AgendaEntry entry = instances[i];

        // Checking that if entry ends at midnight, is quite clumsy, but here goes:
        // EndsAtStartOfDay takes only CCalInstance, but here we mimic EndsAtStartOfDay
        // for CCalEntry type.

        // First check that if _instance_ ends at midnight, but starts earlier
        if( CalenAgendaUtils::endsAtStartOfDayL( entry, aMidnight ) )
            {
            // Second, check that _entry's_ endtime is exactly the midnight
            // This prevents us from destroying repeating entries, that has one
            // instance falling on given midnight.
            if( entry.endTime() == aMidnight )
                {
                iController.Services().agendaInterface()->deleteRepeatedEntry(entry, AgendaUtil::ThisAndAll);
                }
            }
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::DeleteSingleInstanceL
// Delete the given instance. Ask the user whether to delete the series or the instance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CalenDeleteUi::DeleteSingleInstanceL( AgendaEntry& aInstance )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return DoDeleteSingleInstanceL( aInstance, EFalse, AgendaUtil::ThisAndAll );
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::DeleteSingleInstanceL
// Delete the given instance. Delete the entry range given by aRepeatType.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CalenDeleteUi::DeleteSingleInstanceL( AgendaEntry& aInstance, 
                                AgendaUtil::RecurrenceRange aRepeatType )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return DoDeleteSingleInstanceL( aInstance, ETrue, aRepeatType );
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::DoDeleteSingleInstanceL
// Performs the deletion of the instance. If aHasRepeatType is EFalse, the user
// is prompted to delete either the instance or the entire series. In this case,
// aRepeatType is ignored. If aHasRepeatType is ETrue, aRepeatType determines
// whether to delete the instance or the entire series.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CalenDeleteUi::DoDeleteSingleInstanceL( AgendaEntry& aInstance,
                                               bool aHasRepeatType,
                                               AgendaUtil::RecurrenceRange aRepeatType )
    {
    TRACE_ENTRY_POINT;

    bool repeating = aInstance.isRepeating();

    if( !repeating )
        {
        // Even though there is no RRule, the entry might
        // have a list of rdates.
       QList<QDate> rDates = aInstance.rDates();
        repeating = ( rDates.count() > 0 );
        }
    QDateTime recId = aInstance.recurrenceId().toUTC();
    const bool child = recId.isNull();

    if( !aHasRepeatType )
        {
        aRepeatType = AgendaUtil::ThisAndAll;
        }

    bool doDelete( ETrue );

    if( !aHasRepeatType && ( child || repeating ) && ( aInstance.type() != AgendaEntry::TypeAnniversary ) )
        {
        //doDelete = CalenActionUiUtils::ShowRepeatTypeQueryL( aRepeatType,
          //                                                 CalenActionUiUtils::EDelete );
        }
    else
        {
        doDelete = CalenActionUiUtils::showDeleteConfirmationQueryL(aInstance.type() == AgendaEntry::TypeTodo ?
                                                                   CalenActionUiUtils::EDeleteToDo :
                                                                   CalenActionUiUtils::EDeleteEntry );
        }
        
    if( doDelete )
        {
        if( !child || aRepeatType == AgendaUtil::ThisOnly )
            {
            iController.Services().agendaInterface()->deleteRepeatedEntry( aInstance, aRepeatType );
            }
        else if( aRepeatType == AgendaUtil::ThisAndAll )
            {
            iController.Services().agendaInterface()->deleteRepeatedEntry( aInstance, aRepeatType );
            }
        }

    TRACE_EXIT_POINT;
    return doDelete;
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::DeleteEntryL
// Deletes an entry from the database
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CalenDeleteUi::DeleteEntryL( AgendaEntry& entry )
    {
    TRACE_ENTRY_POINT;

    TBool doDelete = CalenActionUiUtils::showDeleteConfirmationQueryL(entry.type() == AgendaEntry::TypeTodo ?
                                                                      CalenActionUiUtils::EDeleteToDo :
                                                                      CalenActionUiUtils::EDeleteEntry );
    
    if( doDelete )
        {
        iController.Services().agendaInterface()->deleteEntry(entry.id());
        }

    TRACE_EXIT_POINT;
    return doDelete;
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::ShowMultipleEntriesDeleteQueryL
// For displaying multiple entries deletion confirmation query
// -----------------------------------------------------------------------------
//
TInt CalenDeleteUi::ShowMultipleEntriesDeleteQueryL(int aCount)
	{
    TRACE_ENTRY_POINT;
    if( aCount > 1 )
        {
        TRACE_EXIT_POINT;
        return CalenActionUiUtils::showDeleteConfirmationQueryL(CalenActionUiUtils::EDeleteToDos, aCount );
        }
    else if( aCount ==  1 )
        {
        TRACE_EXIT_POINT;
        return CalenActionUiUtils::showDeleteConfirmationQueryL(CalenActionUiUtils::EDeleteToDo);
        }
    else
        {        
        TRACE_EXIT_POINT;
        return 0;   //return 0 for other invalid aCount value ( < 0 )
        }   
    
	}

// -----------------------------------------------------------------------------
// CalenDeleteUi::DialogDismissedL
// From MProgressDialogCallback
// Callback method
// called when a dialog is dismissed.
// -----------------------------------------------------------------------------
//
void CalenDeleteUi::DialogDismissedL( const TInt /*aButtonId*/ )
    {
    TRACE_ENTRY_POINT;
    
    // no more entries to delete
    iMoreEntriesToDelete = EFalse;
    iDisplayQuery = EFalse;
    
    // issue notification cancel delete
    iController.BroadcastNotification(ECalenNotifyCancelDelete);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::DeleteEntryL
// Delete entry using entry local uid
// -----------------------------------------------------------------------------
//
void CalenDeleteUi::DeleteEntryL(ulong& aEntryLocalUid)
    {
    TRACE_ENTRY_POINT;
    
    iController.Services().agendaInterface()->deleteEntry(aEntryLocalUid);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::DisplayWaitDialogL
// Display wait dialog
// -----------------------------------------------------------------------------
//
void CalenDeleteUi::DisplayWaitDialogL()
    {
    TRACE_ENTRY_POINT;   
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CalenDeleteUi::MarkedEntriesDeletedL
// Dismiss wait dialog and show information note
// -----------------------------------------------------------------------------
//
void CalenDeleteUi::MarkedEntriesDeletedL()
    {
    TRACE_ENTRY_POINT;    
    // notify marked entries deleted
    iController.BroadcastNotification( ECalenNotifyMarkedEntryDeleted );
    
    TRACE_EXIT_POINT;
    }

// End of File
