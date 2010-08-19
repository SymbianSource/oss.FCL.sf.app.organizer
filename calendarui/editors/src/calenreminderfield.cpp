/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description : Class looking after alarm fields for forms.
*
*/

// user includes
#include "calenreminderfield.h"
#include "CalenDefaultEditors.hrh"
#include "calenunifiededitor.h"

// system includes 
#include <AknQueryValueText.h>
#include <AknPopupField.h>
#include <AknUtils.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <eikmfne.h>
#include <StringLoader.h>
#include <eikdialg.h>
#include <centralrepository.h>
#include <calalarm.h>
#include <calentry.h>
#include <calrrule.h>
#include <caltime.h>
#include <calendateutils.h>
#include <CalenDefaultEditorsData.rsg>
#include <calenconstants.h>

//debug
#include "calendarui_debug.h"

// -----------------------------------------------------------------------------
// CCalenReminderField::NewL()
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenReminderField* CCalenReminderField::NewL( CCalenUnifiedEditor& aUnifiedEditor )
    {
    TRACE_ENTRY_POINT;

    CCalenReminderField* self =
        new( ELeave ) CCalenReminderField( aUnifiedEditor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::CCalenReminderField()
// Constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenReminderField::CCalenReminderField( CCalenUnifiedEditor& aUnifiedEditor )
    : iUnifiedEditor( aUnifiedEditor )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::ConstructL()
// Leaving construction.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenReminderField::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    iIsAlarmOnOff = CCalenReminderField::EAlarmNoAlarm;
    isAllDayInitialOffsetSet = EFalse;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::~CCalenReminderField()
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenReminderField::~CCalenReminderField()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::DeleteMeaninglessLinesL()
// Remove any lines not needed by the current entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenReminderField::DeleteMeaninglessLinesL()
    {
    TRACE_ENTRY_POINT;

    if ( !iUnifiedEditor.Edited().IsAlarmActivated() )
        {
        SetAlarmDateTimeLinesDimmedL( ETrue );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::SetAlarmDateTimeLinesDimmedL()
// Delete alarm time and date items from form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenReminderField::SetAlarmDateTimeLinesDimmedL(
    const TBool& aDimmed )
    {
    TRACE_ENTRY_POINT;

    CCoeControl* alarmDateCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorReminderDate );
    CCoeControl* alarmTimeCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorReminderTime );
    		
    if( aDimmed )
        {
        // Alarm Off, Delete alarm date & alarm time fields from Editor
        if( alarmTimeCtrl )
            {
            iUnifiedEditor.DeleteLine( ECalenEditorReminderTime,EFalse );
            }
        if( alarmDateCtrl )
            {
            iUnifiedEditor.DeleteLine( ECalenEditorReminderDate,EFalse );
            }
        }
    else
        {
        // Alarm On, Add alarm time & alarm date fields to Editor
        if(!alarmDateCtrl)
            {		
            iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_REMINDER_TIME_ITEM,
                    ECalenEditorReminderTime, ECalenEditorReminder);
            }
        if(!alarmTimeCtrl)
            {
            iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_REMINDER_DATE_ITEM,
                    ECalenEditorReminderDate, ECalenEditorReminderTime);
            }
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::HandleControlStateChangeL()
// Called when a field is modifed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenReminderField::HandleControlStateChangeL(TInt aControlId )
    {
    TRACE_ENTRY_POINT;

    switch (aControlId)
        {
        case ECalenEditorReminder:
            {
            HandleAlarmActiveFieldChangeL();
            break;
            }
        default:
            break;
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::NotifyChangeIsAlarmActiveL()
// Called when an alarm field is changed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenReminderField::HandleAlarmActiveFieldChangeL( )
    {
    TRACE_ENTRY_POINT;
    // Activate and initialize the alarm field if it is not already on
    if( IsAlarmActiveInForm() )
        {
        if( !iUnifiedEditor.Edited().IsAlarmActivated() )
            {
            // no more info note "Alarm Already Passed"
            // if the note is an old note and it is in past
            iUnifiedEditor.EditorDataHandler().SetDefaultAlarmDateTimeL();
            SetAlarmDateTimeLinesDimmedL( EFalse );
            SetDataToEditorL();
            }
        }
    else
        {
        // Hide alarm date/time field
        iUnifiedEditor.Edited().SetAlarmOffL();
        SetAlarmDateTimeLinesDimmedL( ETrue );
        }

    iUnifiedEditor.UpdateFormL();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::UpdateAlarmDateTimeWhenStartChanged()
// When the start time of the entry is changed, this function
// gets called to keep the time interval between the entry start
// and the alarm constant.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenReminderField::UpdateAlarmTimeWhenStartChangedL()
    {
    TRACE_ENTRY_POINT;

        // Updating operation is executed only if alarm date/time field exists.
       if( iUnifiedEditor.Edited().IsAlarmActivated() )
            {
            TTime start = iUnifiedEditor.Edited().EventDateTime();
            TTime alarm;
		//For a new allday event when the start time is changed for the first time
		//alarm time must be updated correctly 
            if(iUnifiedEditor.Edited().IsAllDayEvent() && !isAllDayInitialOffsetSet 
                    && iUnifiedEditor.EditorDataHandler().IsCreatingNew())
                {
                alarm = CalenDateUtils::DefaultTime(start)+iAlarmOffset;
                isAllDayInitialOffsetSet = ETrue;
                }
            else
                {
                alarm = start + iAlarmOffset;
                }
        alarm = CalenDateUtils::LimitToValidTime( alarm );
        iUnifiedEditor.Edited().SetAlarmDateTimeL( alarm );
        SetDataToEditorL();
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::SetAlarmDateTimeField()
// Sets the alarm date/time to the specified TTime.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenReminderField::SetAlarmDateTimeField( const TTime& aTime )
    {
    TRACE_ENTRY_POINT;

    iUnifiedEditor.EditorFields().SetTimeField( ECalenEditorReminderTime, aTime );
    iUnifiedEditor.EditorFields().SetDateField( ECalenEditorReminderDate, aTime );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::HandleErrorL()
// Handle error codes related to the alarm fields.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenReminderField::HandleErrorL( const TInt& aError )
    {
    TRACE_ENTRY_POINT;
    
    TBool handled = EFalse;
    switch ( aError )
        {
        case CCalenEditorDataHandler::EFormErrAlarmTimeLaterThanNote:
            {
            TTime startDay = CalenDateUtils::BeginningOfDay( iUnifiedEditor.Edited().EventDateTime() );
            TTime alarmDay = CalenDateUtils::BeginningOfDay( iUnifiedEditor.Edited().AlarmDateTime() );

            TInt newFocusId = startDay == alarmDay ? ECalenEditorReminderTime : ECalenEditorReminderDate;
            iUnifiedEditor.TryChangeFocusToL( newFocusId );
            handled = ETrue;
            break;
            }
        case CCalenEditorDataHandler::EFormErrAlarmDateTooManyDaysBeforeNote:
            {
            iUnifiedEditor.TryChangeFocusToL( ECalenEditorReminderDate );
            handled = ETrue;
            break;
            }
        case CCalenEditorDataHandler::EFormErrAlarmTimePast:
            {
            TTime alarmTime = iUnifiedEditor.Edited().AlarmDateTime();

            TTime now;
            now.HomeTime();
            TInt newFocusId = ECalenEditorReminderDate;
            if( CalenDateUtils::OnSameDay( now, alarmTime ) )
                {
                newFocusId = ECalenEditorReminderTime;
                }
            iUnifiedEditor.TryChangeFocusToL( newFocusId );
 
            handled = ETrue;
            break;
            }
        default:
            handled = EFalse;
            break;
        }

     TRACE_EXIT_POINT;
    return handled;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::PrepareForFocusTransitionL()
// Called when focus is moved from the specified line.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenReminderField::PrepareForFocusTransitionL( TInt aFocusedId )
    {
    TRACE_ENTRY_POINT;

    switch( aFocusedId )
        {
        case ECalenEditorStartDate:
        case ECalenEditorStartTime:
            {
            UpdateAlarmTimeWhenStartChangedL();
            break;
            }
        case ECalenEditorReminderDate:
        case ECalenEditorReminderTime:
            {
            const TBool continueOnError = EFalse;
            ReadDataFromFormL( continueOnError );
            TTime start = iUnifiedEditor.Edited().EventDateTime();
            TTime alarm = iUnifiedEditor.Edited().AlarmDateTime();
            User::LeaveIfError( alarm.MinutesFrom( start, iAlarmOffset ) );
            break;
            }
        default:
            {
            break;
            }
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::IsStartTimeInFutureL()
// Checks to see if the current value of the alarm date/time line is in the future.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenReminderField::IsStartTimeInFutureL()
    {
    TRACE_ENTRY_POINT;

    TTime currentTime;
    currentTime.HomeTime();

    TTime start = iUnifiedEditor.Edited().EventDateTime();

    if( iUnifiedEditor.EditorDataHandler().IsTimedEntry() )
        {
        if( currentTime>start )
            {
            TRACE_EXIT_POINT;
            return EFalse;
            }
        }
    else
        {
        TTime startDay = CalenDateUtils::BeginningOfDay( start );
        TTime currentDay = CalenDateUtils::BeginningOfDay( currentTime );
        if( currentDay>startDay )
            {
            TRACE_EXIT_POINT;
            return EFalse;
            }
        }

    TRACE_EXIT_POINT;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::IsAlarmActiveInForm()
// Checks to see if the alarm is active.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenReminderField::IsAlarmActiveInForm()
    {
    TRACE_ENTRY_POINT;

    TInt pos = iIsAlarmOnOff;

    TRACE_EXIT_POINT;
    return pos == CCalenReminderField::EAlarmActive;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::SetDataToEditorL()
// Reads data from the entry and updates the form with the appropriate values.
// -----------------------------------------------------------------------------
//
void CCalenReminderField::SetDataToEditorL()
    {
    TRACE_ENTRY_POINT;

    // Alarm popup choice item
    TInt pos = CCalenReminderField::EAlarmNoAlarm;

    if ( iUnifiedEditor.Edited().IsAlarmActivated() )
        {
        pos = CCalenReminderField::EAlarmActive;

        // FIXME: How about todos with different start and end dates?
        TTime alarm = iUnifiedEditor.Edited().AlarmDateTime();        
        TTime start = iUnifiedEditor.Edited().EventDateTime();

        User::LeaveIfError( alarm.MinutesFrom( start, iAlarmOffset ) );
        SetAlarmDateTimeField( alarm );
        }
    else
        {
        SetAlarmDateTimeLinesDimmedL( ETrue );
        }

    iIsAlarmOnOff = pos;    
    
    if( pos == CCalenReminderField::EAlarmActive )
        {
        iUnifiedEditor.EditorFields().SetAlarmFieldOnOffL( ETrue );               
        }
    else 
        {
        iUnifiedEditor.EditorFields().SetAlarmFieldOnOffL( EFalse );  
        }   

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::ReadDataFromFormL()
// Reads data from the form and updates the entry with the appropriate values.
// -----------------------------------------------------------------------------
//
void CCalenReminderField::ReadDataFromFormL( TBool aContinueOnError )
    {
    TRACE_ENTRY_POINT;

    if( IsAlarmActiveInForm() )
        {
        TTime alarm = iUnifiedEditor.Edited().AlarmDateTime();
        TRAPD(err, alarm = AlarmDateTimeFromFormL() );
        if(!aContinueOnError)
            {
            User::LeaveIfError( err );
            }
        iUnifiedEditor.Edited().SetAlarmDateTimeL( alarm );
        }
    else
        {
        iUnifiedEditor.Edited().SetAlarmOffL();
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::InitReminderFieldLdayoutL()
// Updates fields just before the form is shown.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenReminderField::InitReminderFieldLdayoutL()
    {
    TRACE_ENTRY_POINT;
    
    if(iUnifiedEditor.Edited().IsAlarmActivated() )
        {
        SetAlarmDateTimeLinesDimmedL( EFalse );
        }
    else
        {
        SetAlarmDateTimeLinesDimmedL( ETrue );
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenReminderField::GetTimeField
// Get the time field in touch layouts
// -----------------------------------------------------------------------------
//
TTime CCalenReminderField::GetTimeField( TInt aControlId )
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	
	return iUnifiedEditor.EditorFields().ReadTimeField( aControlId );
	}

// -----------------------------------------------------------------------------
// CCalenReminderField::AlarmDateTimeFromFormL()
// Returns the current date/time value of the alarm field.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTime CCalenReminderField::AlarmDateTimeFromFormL()
	{
    TRACE_ENTRY_POINT;
    
    // get user entered date & time from the editor
    TTime alarm = CalenDateUtils::BeginningOfDay( GetTimeField( ECalenEditorReminderDate ) );
    alarm += CalenDateUtils::TimeOfDay( GetTimeField( ECalenEditorReminderTime ) );
    
    TRACE_EXIT_POINT;
    return alarm;
	}
// -----------------------------------------------------------------------------
// CCalenReminderField::CheckAlarmActive()
// Gets the info whether alarm on/off 
// adds & updates the alarm fields accordingly
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CCalenReminderField::CheckAlarmActive( TBool aAlarmActive )
    {
    TRACE_ENTRY_POINT;     

    if( aAlarmActive )
        {
        iIsAlarmOnOff = CCalenReminderField::EAlarmActive;
        }
    else 
        {
        iIsAlarmOnOff = CCalenReminderField::EAlarmNoAlarm;
        }   
    
    TRAP_IGNORE( HandleAlarmActiveFieldChangeL() );
    
    TRACE_EXIT_POINT;
    }
 
// End of File
