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
*  Description : calendar unified editor fields control
*
*/

// system includes
#include <eikedwin.h>
#include <eikmfne.h>
#include <StringLoader.h>
#include <e32des16.h>
#include <calentry.h>
#include <CalenDefaultEditorsData.rsg>
#include <calendateutils.h>
#include <calenservices.h>

// user includes
#include "calenunifiededitorcontrol.h"
#include "calenunifiededitor.h"
#include "CalenDefaultEditors.hrh"
#include "calenentryutil.h"
#include "caleneventtypefield.h"
#include "calenreminderfield.h"
#include "calenrepeatfield.h"
#include "calenalldayfield.h"
#include "calenpriorityfield.h"
#include "calendbfield.h"
#include "CalenDescription.h"

// debug
#include "calendarui_debug.h"

// Constants
const TInt KOneDay( 1 );


// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::NewL
// Two phased constructor
// -----------------------------------------------------------------------------
//
CCalenUnifiedEditorControl* CCalenUnifiedEditorControl::NewL( CCalenUnifiedEditor& aUnifiedEditor,
                                                    MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;
 
    CCalenUnifiedEditorControl* self = new (ELeave)CCalenUnifiedEditorControl( aUnifiedEditor );
    CleanupStack::PushL( self );
    self->ConstructL(aServices);
    CleanupStack::Pop();

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::CCalenUnifiedEditorControl
// Destructor
// -----------------------------------------------------------------------------
//
CCalenUnifiedEditorControl::~CCalenUnifiedEditorControl()
    {
    TRACE_ENTRY_POINT;
    
    delete iEventTypeField;
    delete iReminderField;
    delete iRepeatField;
    delete iAllDayField;
    delete iPriorityField;
    delete iDbField;
    delete iDescription;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ConstructL
// Second phased construction
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::ConstructL(MCalenServices& aServices)
    {
    TRACE_ENTRY_POINT;
    
    iEventTypeField = CCalenEventTypeField::NewL( iUnifiedEditor );
    iReminderField = CCalenReminderField::NewL( iUnifiedEditor );
    iRepeatField = CCalenRepeatField::NewL( iUnifiedEditor );
    iAllDayField = CCalenAllDayField::NewL( iUnifiedEditor );
    iPriorityField = CCalenPriorityField::NewL( iUnifiedEditor );
    iDbField = CCalenDbField::NewL( iUnifiedEditor,aServices );
    iDescription = CCalenDescription::NewL( iUnifiedEditor );
    
    TRACE_EXIT_POINT;    
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::CCalenUnifiedEditorControl
// Default constructor
// -----------------------------------------------------------------------------
//
CCalenUnifiedEditorControl::CCalenUnifiedEditorControl( CCalenUnifiedEditor& aUnifiedEditor )
    :iUnifiedEditor( aUnifiedEditor )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::MakeUnifiedEditorL
// Initializes the dialog's controls before the dialog is sized
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::MakeUnifiedEditorL()
    {
    TRACE_ENTRY_POINT;

    // Add default editor fields.
    AddDefaultEditorL();
    // Initialise editor fields.
    InitDefaultEditorsL();
    // Set data to editor fields.
    SetDataToEditorL();
    
    // Update MSK string.
    iUnifiedEditor.SetMskFromResourceL();
    iUnifiedEditor.UpdateFormL();
        
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::SetDataToEditorL
// Set data to the collapsed unified editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::SetDataToEditorL()
    {
    TRACE_ENTRY_POINT;
    
    // Set Editor field values
    
    // Set data to EventType fields   
    iEventTypeField->SetDataToEditorL();
    
    TPtrC subject = iUnifiedEditor.Edited().Summary().Left(
                        iUnifiedEditor.MaxTextEditorLength() );
    iUnifiedEditor.SetEditorTextL( ECalenEditorSubject, &subject );
   

    
    // update duration for EndTime update, with StartTime change
    UpdateMeetingDurationL();

    switch( iUnifiedEditor.GetEntryType() )
        {
        case CCalEntry::EEvent:
            {
              iAllDayField->SetDataToEditorL();
            }
            break;
        case CCalEntry::EAnniv:
            {
            TTime birthDayYear = iUnifiedEditor.Edited().EventDateTime();
            SetDateField( ECalenEditorStartDate, birthDayYear, ETrue );
            }
            break;
        case CCalEntry::EAppt:
            {
            if( iUnifiedEditor.Edited().IsAllDayEvent() )
                {
                iAllDayField->SetDataToEditorL();
                }
            else
                {
                // Entry is Non-AllDay event
                TTime firstRDate; 
                TTime rruleEndTime ;             
                TTimeIntervalMinutes duration;                
                
                TTime entryStartTime = iUnifiedEditor.Edited().StartDateTime();
                TTime entryEndTime = iUnifiedEditor.Edited().EndDateTime();
                
                User::LeaveIfError( entryEndTime.MinutesFrom( entryStartTime, duration ) );
                
                ReadRrule(firstRDate,rruleEndTime);                                           
                
                if( firstRDate <= entryStartTime)
                    {                    
                    SetTimeField( ECalenEditorStartTime, firstRDate, ETrue );
                    SetTimeField( ECalenEditorEndTime, rruleEndTime, ETrue );                
                    SetDateField( ECalenEditorStartDate, firstRDate, ETrue);
                    SetDateField( ECalenEditorEndDate, rruleEndTime, ETrue);
                    }
                else{
                    SetTimeField( ECalenEditorStartTime, entryStartTime, ETrue );
                    SetTimeField( ECalenEditorEndTime, entryEndTime, ETrue );                
                    SetDateField( ECalenEditorStartDate, entryStartTime, ETrue);
                    SetDateField( ECalenEditorEndDate, entryEndTime, ETrue);
                    }                
               }
            
            }
            break;
        case CCalEntry::ETodo:
            {
            TTime dueDate = iUnifiedEditor.Edited().EventDateTime();
            
            // update date field with todays date
            SetDateField(ECalenEditorStartDate, dueDate, ETrue);
            // set priority field value 
            iPriorityField->SetDataToEditorL();

            }
            break;
        default:
            break;
        }
    
    iReminderField->SetDataToEditorL();
    
    // Anniversary, To-do, exceptional and single instance of repeat entry 
    // do not have Repeat field.
    if( CCalEntry::EAnniv != iUnifiedEditor.GetEntryType()
            && CCalEntry::ETodo != iUnifiedEditor.GetEntryType() )
        {
        iRepeatField->SetDataToEditorL();
        }
    
    if(!iDbField->IsCalendarEdited())
        {
        iDbField->SetDataToEditorL();
        }

    if( CCalEntry::ETodo != iUnifiedEditor.GetEntryType() )
        {
        TPtrC location = iUnifiedEditor.Edited().Location().Left(
                            iUnifiedEditor.MaxTextEditorLength() );
        iUnifiedEditor.SetEditorTextL( ECalenEditorPlace, &location );
        }
    
    // TODO: Uncomment this when enabling attachment support
      iUnifiedEditor.SetAttachmentNamesToEditorL();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::AddDefaultEditorL
// Adds editor fields
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::AddDefaultEditorL()
    {
    TRACE_ENTRY_POINT;
    
    switch( iUnifiedEditor.GetEntryType() )
        {
        case CCalEntry::EEvent:
            {
            AddDefaultMeetingEditorL();
            }
            break;
        case CCalEntry::EAnniv:
            {
            AddDefaultBirthDayEditorL();
            }
            break;            
        case CCalEntry::EAppt:
            {
            AddDefaultMeetingEditorL();
            }
            break;
        case CCalEntry::ETodo:
            {
            AddDefaultTodoEditorL();
            }            
            break;
        default:
            break;
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::InitDefaultEditorsL
// Initialise default editor contents
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::InitDefaultEditorsL()
    {
    TRACE_ENTRY_POINT;
    
    // Set editor lengths
    CEikEdwin* edwin = static_cast<CEikEdwin*>( iUnifiedEditor.Control( ECalenEditorSubject ) );
    edwin->SetTextLimit( iUnifiedEditor.MaxTextEditorLength() );
    
    iEventTypeField->InitEventTypeFieldLayoutL();
    
    //Currently nothig to be initalized 
    //as its intial value depend on entry
    iAllDayField->InitAllDayFieldLayoutL();
    

    iReminderField->InitReminderFieldLdayoutL();

    // To Initialise priority field for meeting tyrpe entry
    if( CCalEntry::ETodo == iUnifiedEditor.GetEntryType() )
        {
        iPriorityField->InitPriorityFieldLayoutL();
        }

    // Initialise repeat field, if it's not Anniversary.
    // To-do entry don't have repeat field, for now
    if( CCalEntry::EAnniv != iUnifiedEditor.GetEntryType() && 
                CCalEntry::ETodo != iUnifiedEditor.GetEntryType() )
        {
        iRepeatField->InitRepetFieldLayoutL();
        }

    iDbField->InitDbFieldLayoutL();
    iDescription->InitDescritpionFieldLayoutL();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::AddDefaultBirthDayEditorL
// Add collapsed birthday editor contents
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::AddDefaultBirthDayEditorL()
    {
    TRACE_ENTRY_POINT;
    // event type, subject, date & year,more details
     
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_START_DATE_ITEM, 
                  ECalenEditorStartDate, ECalenEditorSubject ); 

    // "Start Date" Label should be "Date of Birth" for Birthday
    iUnifiedEditor.SetControlCaptionL( ECalenEditorStartDate,
                    R_QTN_CALEN_EDITOR_DATE_OF_BIRTH );

    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_REMINDER_ITEM,
                    ECalenEditorReminder, ECalenEditorStartDate );

    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_PLACE_ITEM,
                    ECalenEditorPlace, ECalenEditorReminder );
/*    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_PEOPLE_ITEM,
                    ECalenEditorPeople, ECalenEditorPlace );
*/    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_DB_NAME_ITEM,
                    ECalenEditorDBName, ECalenEditorPlace );

    // TODO: Uncomment this when enabling attachment support
    // Replace ECalenEditorDBName with ECalenEditorAttachment in the next statement
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_ATTACHMENT_ITEM,
                    ECalenEditorAttachment, ECalenEditorDBName );

    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_DESCRIPTION_ITEM,
                    ECalenEditorDescription, ECalenEditorAttachment );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::AddDefaultMeetingEditorL
// Add meeting editor contents
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::AddDefaultMeetingEditorL()
    {
    TRACE_ENTRY_POINT;
    // non all day event 
    // event type,subject,all day,start time,end time,start date,
    // end date,place,more details
    
    // all day event 
    // subject,event type,all day,start date,end date,place,more details 
    
  
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_ALL_DAY_ITEM,
            ECalenEditorAllDayItem, ECalenEditorSubject );

    if( !iUnifiedEditor.Edited().IsAllDayEvent() )
        {
        iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_START_TIME_ITEM, 
                ECalenEditorStartTime, ECalenEditorAllDayItem );
        iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_END_TIME_ITEM,
                ECalenEditorEndTime, ECalenEditorStartTime );
        iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_START_DATE_ITEM,
                    ECalenEditorStartDate, ECalenEditorEndTime );        
        }
     else
         {
         iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_START_DATE_ITEM,
                     ECalenEditorStartDate, ECalenEditorAllDayItem );   
         }
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_END_DATE_ITEM,
            ECalenEditorEndDate, ECalenEditorStartDate );
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_PLACE_ITEM,
                  ECalenEditorPlace, ECalenEditorEndDate );
    
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_REMINDER_ITEM,
                    ECalenEditorReminder, ECalenEditorPlace );
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_REPEAT_ITEM,
                    ECalenEditorRepeat, ECalenEditorReminder );
    /*iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_PEOPLE_ITEM,
                    ECalenEditorPeople, ECalenEditorRepeat );
    */
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_DB_NAME_ITEM,
                    ECalenEditorDBName, ECalenEditorRepeat );
    
    // TODO: Uncomment this when enabling attachment support
    // Replace ECalenEditorDBName with ECalenEditorAttachment in the next statement
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_ATTACHMENT_ITEM,
                    ECalenEditorAttachment, ECalenEditorDBName );
    
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_DESCRIPTION_ITEM,
                    ECalenEditorDescription, ECalenEditorAttachment );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::AddDefaultTodoEditorL
// Add collapsed todo editor contents
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::AddDefaultTodoEditorL()
    {
    TRACE_ENTRY_POINT;
    // event type,subject,due date,more details
    
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_START_DATE_ITEM, 
                    ECalenEditorStartDate, ECalenEditorSubject );

    // "Start Date" Label should be "Due date" for To-Do
    iUnifiedEditor.SetControlCaptionL( ECalenEditorStartDate,
                    R_QTN_CALEN_EDITOR_DUE_DATE );
    
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_REMINDER_ITEM,
                    ECalenEditorReminder, ECalenEditorStartDate );
    
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_PRIORITY_ITEM, 
                    ECalenEditorPriority, ECalenEditorReminder );

    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_DB_NAME_ITEM,
                    ECalenEditorDBName, ECalenEditorPriority );
    
    // TODO: Uncomment this when enabling attachment support
    // Replace ECalenEditorDBName with ECalenEditorAttachment in the next statement
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_ATTACHMENT_ITEM,
                    ECalenEditorAttachment, ECalenEditorDBName );
    
    iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_DESCRIPTION_ITEM,
                    ECalenEditorDescription, ECalenEditorAttachment );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::SetTimeField
// Sets the time field in touch layouts
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::SetTimeField( TInt aControlId, const TTime& aTime ,
                                              TBool aDoDraw )
    {
    TRACE_ENTRY_POINT;    

    CEikTimeEditor* timeField = 
        static_cast<CEikTimeEditor*>( iUnifiedEditor.ControlOrNull( aControlId ) );
    if( timeField )
        {
        timeField->SetTime( aTime );
        if ( aDoDraw )
            {
            timeField->DrawDeferred();
            }
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::SetDateField
// Sets a date field to the given TTime.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::SetDateField( TInt aControlId, const TTime& aTime,
                                              TBool aDoDraw )
    {
    TRACE_ENTRY_POINT;

    CEikDateEditor* dateField = NULL;
    dateField = static_cast<CEikDateEditor*>( iUnifiedEditor.ControlOrNull( aControlId ) );
    if( dateField )
        {
        dateField->SetDate( aTime );
        if( aDoDraw )
            {
            dateField->DrawDeferred();
            }
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::SetDateTimeField
// Update date and time field
// -----------------------------------------------------------------------------
void CCalenUnifiedEditorControl::SetDateTimeField( TInt aControlId, const TTime& aTime,
                                                  TBool aDoDraw )
    {
    TRACE_ENTRY_POINT;

    CEikTimeAndDateEditor* dateTimeField = NULL;
    dateTimeField = static_cast<CEikTimeAndDateEditor*>( iUnifiedEditor.ControlOrNull( aControlId  ) );
    if( dateTimeField )
      {
      dateTimeField->SetTimeAndDate( aTime );
      if( aDoDraw )
          {
          dateTimeField->DrawDeferred();
          }
      }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::SetAlarmFieldOnOffL
// Set alarm field on or off
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::SetAlarmFieldOnOffL( TBool aOnOff )
    {
    TRACE_ENTRY_POINT;
    
    iUnifiedEditor.SetAlarmFieldOnOffL( aOnOff );
    
    TRACE_EXIT_POINT;
    } 

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::SetAlarmFieldOnOffL
// Set alarm field on or off
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::SetAllDayFieldL( TBool aYesNo )
    {
    TRACE_ENTRY_POINT;
    
    iUnifiedEditor.SetAllDayFieldL( aYesNo );
    
    TRACE_EXIT_POINT;
    } 


// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::IsAlarmActiveInForm
// Check for whether alarm is activated or not
// -----------------------------------------------------------------------------
//
TBool CCalenUnifiedEditorControl::IsAlarmActiveInForm()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iReminderField->IsAlarmActiveInForm();
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::CheckAlarmActive
// check alarm active based on the input
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::CheckAlarmActive( TBool aActive )
    {
    TRACE_ENTRY_POINT;
    
    iReminderField->CheckAlarmActive( aActive );
    
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::IsAllDayEvent
// To check value of AllDay field
// -----------------------------------------------------------------------------
//
TBool CCalenUnifiedEditorControl::IsAllDayEvent()
    {
    TRACE_ENTRY_POINT;
    
    return iAllDayField->IsAllDayEvent();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::SetAllDayEvent
// To Set AllDay field value
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::SetAllDayEvent( TBool aActive )
    {
    TRACE_ENTRY_POINT;
    
    iAllDayField->SetAllDayEvent( aActive );
    
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::HandleControlStateChangeL
// Handles a state change in the control with id aControlId. 
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::HandleControlStateChangeL( TInt aControlId )
    {
    TRACE_ENTRY_POINT;
    
    switch( aControlId )
        {
        case ECalenEditorEventType:
            {
            iEventTypeField->HandleControlStateChangeL( aControlId );
            }
            break;
        case ECalenEditorReminder:
            {
            iReminderField->HandleControlStateChangeL( aControlId );
            }
            break;
        case ECalenEditorRepeat:
            {
            iRepeatField->HandleControlStateChangeL( aControlId );
            }
            break;
        case ECalenEditorDBName:
            {
            iDbField->HandleControlStateChangeL( aControlId );
            }
            break;
        default:
            break;
        }
   
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::PrepareForFocusTransitionL
// Tries to change focus to the specified line.
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::PrepareForFocusTransitionL( TInt aFocusedId )
    {
    switch( aFocusedId )
        {
        case ECalenEditorSubject:
            {
            ReadSubjectFromEditorL();
            }
            break;
        case ECalenEditorEventType:
            {
            iEventTypeField->PrepareForFocusTransitionL( aFocusedId );
            }
            break;
            
        case ECalenEditorStartTime:    
        case ECalenEditorStartDate:
            {
            ReadStartDateTimeFromEditorL( ETrue, aFocusedId );
            UpdateMeetingDurationL();
            }
            break;
            
        case ECalenEditorEndDate:
        case ECalenEditorEndTime:
            {
            ReadEndDateTimeFromEditorL( ETrue );
            UpdateMeetingDurationL();
            }
            break;
            
        case ECalenEditorBirthDayYear:
            break;
            
        case ECalenEditorDueDate:   
            break;
        
        // todo: Need to check, ECalenEditorDueDate can be removed?    
        case ECalenEditorAllDayItem:
            {
            iAllDayField->PrepareForFocusTransitionL( aFocusedId );
            }
            break;
            
        case ECalenEditorPriority:
            {
            iPriorityField->PrepareForFocusTransitionL( aFocusedId );
            }
            break;
            
        case ECalenEditorPlace:
            {
            ReadPlaceFieldFromEditorL( ETrue );
            }
            break;
            
        case ECalenEditorRepeat:
            {
            iRepeatField->PrepareForFocusTransitionL( aFocusedId );
            }
            break;
            
        case ECalenEditorDBName:
            {
            iDbField->PrepareForFocusTransitionL( aFocusedId );
            }
            break;
        case ECalenEditorAttachment:
            break;
        case ECalenEditorDescription:
            break;
        default:
            break;
        }
    
    iReminderField->PrepareForFocusTransitionL( aFocusedId );
    
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::OnEventTypeChangedL
// Handles event type changed
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::OnEventTypeChangedL( CCalEntry::TType aNewEventType )
    {
    TRACE_ENTRY_POINT;
    
    // update Entry type of Edited entry
    iUnifiedEditor.Edited().SetEntryType( aNewEventType );

    // Delete, previous entry type fields from editor
    DeletePreviousEntryTypeFieldsL();
    
    // Add new fiedls to editor
    AddNewEntryTypeFieldsL( aNewEventType );
    iUnifiedEditor.SetEntryType( aNewEventType );

    // after changing the event type, update the fields with default values
    InitDefaultEditorsL();
    SetDataToEditorL();
    
    iUnifiedEditor.UpdateFormL();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::DeletePreviousEntryTypeFieldsL
// Delete previous entry type fields on selcting 
// the new entry type
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::DeletePreviousEntryTypeFieldsL()
    {
    TRACE_ENTRY_POINT;
    CCalEntry::TType previousEntryType = iUnifiedEditor.GetEntryType();
    switch( previousEntryType )
        {
        case CCalEntry::EEvent:
            {
            // Delete AllDay, StartDate, EndDate, and Place fields
            iUnifiedEditor.DeleteLine( ECalenEditorAllDayItem, ETrue );
            iUnifiedEditor.DeleteLine( ECalenEditorStartDate, ETrue );
            iUnifiedEditor.DeleteLine( ECalenEditorEndDate, ETrue );
            iUnifiedEditor.DeleteLine( ECalenEditorPlace, ETrue );
            }
            break;
        case CCalEntry::EAnniv:
            {
            iUnifiedEditor.DeleteLine( ECalenEditorStartDate, ETrue );
            }
            break;
        case CCalEntry::EAppt:
            {
            if( iUnifiedEditor.Edited().IsAllDayEvent() ) 
                {
                // Delete, AllDay event fields from Editor
                iUnifiedEditor.DeleteLine( ECalenEditorAllDayItem, ETrue );
                }
            else
                {
                // Delete, Non-AllDay event fields from Editor
                iUnifiedEditor.DeleteLine( ECalenEditorAllDayItem, ETrue );
                iUnifiedEditor.DeleteLine( ECalenEditorStartTime, ETrue );
                iUnifiedEditor.DeleteLine( ECalenEditorEndTime, ETrue );
                }
            iUnifiedEditor.DeleteLine( ECalenEditorStartDate, ETrue );
            iUnifiedEditor.DeleteLine( ECalenEditorEndDate, ETrue );
            iUnifiedEditor.DeleteLine( ECalenEditorPlace, ETrue );
				
				
            }
            break;
        case CCalEntry::ETodo:
            {
            iUnifiedEditor.DeleteLine( ECalenEditorStartDate, ETrue );
            iUnifiedEditor.DeleteLine( ECalenEditorPriority, ETrue );
            }
            break;
        default:
            break;
        }
    
        // Delete, extra fields added to editor for more entry details
        DeleteExtendedEntryFields( previousEntryType );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::DeleteExtendedEntryfields
// Delete previous entry type more information fields 
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::DeleteExtendedEntryFields( CCalEntry::TType aPreviousEntryType )
    {
    
    // If Reminder field is in 'ON' state, delete Reminder Date, time fields
    if( iReminderField->IsAlarmActiveInForm() )
        {
        CCoeControl* alarmDateCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorReminderDate );
        CCoeControl* alarmTimeCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorReminderTime );
                
        if( alarmTimeCtrl )
            {
            iUnifiedEditor.DeleteLine( ECalenEditorReminderTime, ETrue );
            }
        if( alarmDateCtrl )
            {
            iUnifiedEditor.DeleteLine( ECalenEditorReminderDate, ETrue );
            }
        }

    iUnifiedEditor.DeleteLine( ECalenEditorReminder );
    
    /*CCoeControl* PeopleFieldCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorPeople );
    if( PeopleFieldCtrl )
        {
        iUnifiedEditor.DeleteLine( ECalenEditorPeople );
        }
    */
    iUnifiedEditor.DeleteLine( ECalenEditorDBName );
    
    // TODO: Uncomment this when enabling attachment support
    iUnifiedEditor.DeleteLine( ECalenEditorAttachment );
    
    iUnifiedEditor.DeleteLine( ECalenEditorDescription );
    
    // To-Do do not have place field
    CCoeControl* editorPlaceCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorPlace );
    if( editorPlaceCtrl )
        {
        iUnifiedEditor.DeleteLine( ECalenEditorPlace );
        }
    
    // Anniversary & To-Do entry, do not have Repeat field
    if( ( aPreviousEntryType != CCalEntry::EAnniv ) ||
            ( aPreviousEntryType != CCalEntry::ETodo ) )
        {
        // Delete Repeat field.
        CCoeControl* repeatCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorRepeat );
        CCoeControl* repeatUntilCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorRepeatUntil );
        if( repeatCtrl )
            {
            iUnifiedEditor.DeleteLine( ECalenEditorRepeat );
            }
        if( repeatUntilCtrl )
            {
            iUnifiedEditor.DeleteLine( ECalenEditorRepeatUntil );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::AddNewEntryTypeFieldsL
// Add new entry type's editor fields
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::AddNewEntryTypeFieldsL( CCalEntry::TType aNewEventType )
    {
    TRACE_ENTRY_POINT;
    
    switch( aNewEventType )
        {
        case CCalEntry::EEvent:
        case CCalEntry::EAppt:
            {
            AddDefaultMeetingEditorL();
            }
            break;
        case CCalEntry::EAnniv:
            {
            AddDefaultBirthDayEditorL();
            }
            break;
        case CCalEntry::ETodo:
            {
            AddDefaultTodoEditorL();
            }
            break;
        default:
            break;
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::AskRepeatingNoteQueriesL
// Add new entry type's editor fields
// -----------------------------------------------------------------------------
//
CCalenEditorDataHandler::TError CCalenUnifiedEditorControl::AskRepeatingNoteQueriesL(
     CalCommon::TRecurrenceRange& aRepeatType )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iRepeatField->AskRepeatingNoteQueriesL( aRepeatType );
    
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ReadDataFromEditorL
// Reads editor data mainly for collapsed editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::ReadDataFromEditorL( TBool aContinueOnError )
    {
    TRACE_ENTRY_POINT;
    // Read data from the editor
    ReadSubjectFromEditorL();
    ReadStartDateTimeFromEditorL( aContinueOnError );
    ReadEndDateTimeFromEditorL( aContinueOnError );        
    
    // ToDo do not have Place field
    if( iUnifiedEditor.GetEntryType() != CCalEntry::ETodo )
        {
        ReadPlaceFieldFromEditorL( aContinueOnError );
        }
    
    // TODO: Uncomment this when enabling attachment support
    ReadAttachmentFieldFromEditorL( aContinueOnError );
    ReadDescriptionFieldFromEditorL( aContinueOnError );
    
    iReminderField->ReadDataFromFormL( aContinueOnError );
    
    if( iUnifiedEditor.GetEntryType() == CCalEntry::ETodo )
        {
        iUnifiedEditor.Edited().SetNonRepeatingL();
        iPriorityField->ReadDataFromFormL( aContinueOnError );
        }
    else
        {
        // Anniversary, Exceptional and Single instance of repeat enty
        // do not have repeat field in editor
        CCoeControl* repeatCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorRepeat );
        if( repeatCtrl )
            {
            iRepeatField->ReadDataFromFormL( aContinueOnError );
            }
        }
    
    iDbField->ReadDataFromFormL( aContinueOnError );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ReadSubjectFromEditorL
// Reads subject field from editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::ReadSubjectFromEditorL()
    {
    TRACE_ENTRY_POINT;
    iUnifiedEditor.ReadSubjectFromEditorL();
    TRACE_ENTRY_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ReadStartDateTimeFromEditorL
// Reads start date time from the editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::ReadStartDateTimeFromEditorL( TBool aContinueOnError , TInt /* aFocusedId */ )
    {
    TRACE_ENTRY_POINT;
    
    TTime start = iUnifiedEditor.Edited().StartDateTime();
    TDateTime startDT = start.DateTime();
    
    // Reading time field leaves, if it contains empty values.
    // When editors are forced to exit, we want to continue reading
    // other fields, even if one time field contains empty values
    if( aContinueOnError )
        {
        PIM_TRAPD_HANDLE( ( start = GetStartDateTimeL() ) );
        }
    else
        {
        // Allow this function to leave.
        start = GetStartDateTimeL();
        }
    startDT = start.DateTime();
    iUnifiedEditor.Edited().SetStartDateTimeL( start );
    
    UpdateEndTimeL();
		
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ReadStartDateTimeFromEditorL
// Reads End date time from the editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::ReadEndDateTimeFromEditorL( TBool aContinueOnError )
    {
    TRACE_ENTRY_POINT;
    
    TTime end;
    TDateTime endDT;
    
    // Reading time field leaves, if it contains empty values.
    // When editors are forced to exit, we want to continue reading
    // other fields, even if one time field contains empty values
    if( aContinueOnError )
        {
        PIM_TRAPD_HANDLE( ( end = GetEndDateTimeL() ) );
        }
    else
        {
        // Allow this function to leave.
        end = GetEndDateTimeL();
        }
    endDT = end.DateTime();
    
    iUnifiedEditor.Edited().SetEndDateTimeL( end );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::GetStartDateTimeL
// Get start date time from editor
// -----------------------------------------------------------------------------
//
TTime CCalenUnifiedEditorControl::GetStartDateTimeL()
    {
    TRACE_ENTRY_POINT;
    
    TTime result = ReadTimeField( ECalenEditorStartDate );
    TDateTime resDT = result.DateTime();
    CCoeControl* startTimeCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorStartTime );

    // For non-timed event and AllDay event StatTime field 
    // does n't exit, so do not read StartTime.
    if( startTimeCtrl )
        {
        TTime timeOfDate = ReadTimeField( ECalenEditorStartTime );
        if( timeOfDate > 0 )
            {
            TDateTime timeOfDayTime = timeOfDate.DateTime();
            result += TTimeIntervalHours( timeOfDayTime.Hour() );
            result += TTimeIntervalMinutes( timeOfDayTime.Minute() );
            }
        }
    else
        {
        // To set time as beginning of the day
        result = CalenDateUtils::BeginningOfDay( result );
        }
    
    if( iAllDayField->IsAllDayEvent() )
        {
        TTime endDate = iUnifiedEditor.Edited().EndDateTime();
        endDate = CalenDateUtils::BeginningOfDay( endDate );
        iUnifiedEditor.Edited().SetEndDateTimeL(endDate);
        }
    
    resDT = result.DateTime();
    TRACE_EXIT_POINT;
    return result;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::GetEndDateTimeL
// Get end date time from editor
// -----------------------------------------------------------------------------
//
TTime CCalenUnifiedEditorControl::GetEndDateTimeL()
    {
    TRACE_ENTRY_POINT;
    TTime result;
    CCoeControl* endDateCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorEndDate);
    if( endDateCtrl )
        {
        result = ReadDateField( ECalenEditorEndDate );
        CCoeControl* endTimeCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorEndTime );
        // for AllDay event do not have EndTime
        // so, do not read EndTime for them.
        if( endTimeCtrl )
            {
            TTime timeOfDate = ReadTimeField( ECalenEditorEndTime );
            if( timeOfDate > 0 )
                {
                TDateTime timeOfDayTime = timeOfDate.DateTime();
                result += TTimeIntervalHours( timeOfDayTime.Hour() );
                result += TTimeIntervalMinutes( timeOfDayTime.Minute() );
                }
            }
        else
            {
            // To set time as beginning of the day
            result = CalenDateUtils::BeginningOfDay( result );
            }
        
        if( iAllDayField->IsAllDayEvent() )
            {
            // For allday (1 day) event Eg. "Start:- 12:00am, 15-08-2010 &  End:-12:00am, 16-08-2010"
            // In Editor it should be displayed as "StartDate: 15-08-2010 & EndDate:15-08-2010" 
            // But, while saving EndDate is saved as 12:00am, 16-08-2010. 
            TTime startDate = iUnifiedEditor.Edited().StartDateTime();
            startDate = CalenDateUtils::BeginningOfDay( startDate );
            iUnifiedEditor.Edited().SetStartDateTimeL(startDate);
            if( result >= startDate )
                {
                result += TTimeIntervalDays( KOneDay );
                }
            }

        }
    else
        {
        // For entries Anniversary/Todo there is no EndDate field
        // For them EndDate is same as StartDate
        result = ReadDateField( ECalenEditorStartDate );
        }
    
    TRACE_EXIT_POINT;
    return result;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ReadTimeField
// Reads time from editor
// -----------------------------------------------------------------------------
//
TTime CCalenUnifiedEditorControl::ReadTimeField( TInt aControlId )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return static_cast<CEikTTimeEditor*>( iUnifiedEditor.Control( aControlId ) )->GetTTime();
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ReadDateField
// Reads date from editor
// -----------------------------------------------------------------------------
//
TTime CCalenUnifiedEditorControl::ReadDateField( TInt aControlId )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return static_cast<CEikDateEditor*>( iUnifiedEditor.Control( aControlId ) )->Date();
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ReadPlaceFieldFromEditorL
// Reads place field from editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::ReadPlaceFieldFromEditorL( TBool /*aContinueOnError*/ )
    {
    TRACE_ENTRY_POINT;
    
    iUnifiedEditor.ReadPlaceFromEditorL();

    TRACE_ENTRY_POINT; 
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ReadPeopleFieldFromEditorL
// Reads people field from editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::ReadPeopleFieldFromEditorL( TBool /*aContinueOnError*/ )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_ENTRY_POINT;    
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ReadAttachmentFieldFromEditorL
// Reads attachment field from editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::ReadAttachmentFieldFromEditorL( TBool /*aContinueOnError*/ )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_ENTRY_POINT;    
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ReadDescriptionFieldFromEditorL
// Reads description field from editor
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::ReadDescriptionFieldFromEditorL( TBool /*aContinueOnError*/ )
    {
    TRACE_ENTRY_POINT;
    
    TRACE_ENTRY_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::Description
// Access to the decription field
// -----------------------------------------------------------------------------
//
CCalenDescription* CCalenUnifiedEditorControl::Description()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iDescription;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::UpdateLinesOnLocaleChangeL
// Update editor lines on environment change notifications
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::UpdateLinesOnLocaleChangeL()
    {
    TRACE_ENTRY_POINT;
    
    TInt lastFocusedItem = iUnifiedEditor.IdOfFocusedControl();
    switch( iUnifiedEditor.GetEntryType() )
        {
        case CCalEntry::EEvent:
            {
            iUnifiedEditor.DeleteLine( ECalenEditorStartDate, EFalse );
            iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_START_DATE_ITEM,
                        ECalenEditorStartDate, ECalenEditorAllDayItem );
            
            iUnifiedEditor.DeleteLine( ECalenEditorEndDate, EFalse );
            iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_END_DATE_ITEM,
                    ECalenEditorEndDate, ECalenEditorStartDate );
            }
            break;
            
        case CCalEntry::EAnniv:
            {
            iUnifiedEditor.DeleteLine( ECalenEditorStartDate, EFalse );
            iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_BIRTHDAY_YEAR_ITEM, 
                    ECalenEditorStartDate, ECalenEditorEventType );
            
            }
            break;
            
        case CCalEntry::EAppt:
            {
            
            if( iUnifiedEditor.Edited().IsAllDayEvent() )
                {
                iUnifiedEditor.DeleteLine( ECalenEditorStartDate, EFalse );
                iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_START_DATE_ITEM,
                            ECalenEditorStartDate, ECalenEditorAllDayItem );
                
                iUnifiedEditor.DeleteLine( ECalenEditorEndDate, EFalse );
                iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_END_DATE_ITEM,
                        ECalenEditorEndDate, ECalenEditorStartDate );
                }
            else
                {
                iUnifiedEditor.DeleteLine( ECalenEditorStartTime, EFalse );
                iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_START_TIME_ITEM,
                        ECalenEditorStartTime, ECalenEditorAllDayItem );
                
                iUnifiedEditor.DeleteLine( ECalenEditorStartDate, EFalse );
                iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_START_DATE_ITEM,
                        ECalenEditorStartDate, ECalenEditorStartTime );

                iUnifiedEditor.DeleteLine( ECalenEditorEndTime, EFalse );
                iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_END_TIME_ITEM,
                        ECalenEditorEndTime, ECalenEditorStartDate );
                
                iUnifiedEditor.DeleteLine( ECalenEditorEndDate, EFalse );
                iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_END_DATE_ITEM,
                        ECalenEditorEndDate, ECalenEditorEndTime );
                }

            }
            break;
            
        case CCalEntry::ETodo:
            {
            iUnifiedEditor.DeleteLine( ECalenEditorStartDate, EFalse );
            iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_START_DATE_ITEM, 
                          ECalenEditorStartDate, ECalenEditorEventType );
                        
            }
            break;
        default:
            break;
        }
    
    if( iUnifiedEditor.ControlOrNull( ECalenEditorReminderDate ) )
        {
        iUnifiedEditor.DeleteLine( ECalenEditorReminderDate, EFalse );
        iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_REMINDER_DATE_ITEM,
                ECalenEditorReminderDate, ECalenEditorReminder );
        
        }
    if( iUnifiedEditor.ControlOrNull( ECalenEditorReminderTime ) )
        {
        iUnifiedEditor.DeleteLine( ECalenEditorReminderTime, EFalse );
        iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_REMINDER_TIME_ITEM,
                ECalenEditorReminderTime, ECalenEditorReminderDate );                    
        }
    if( iUnifiedEditor.ControlOrNull( ECalenEditorRepeatUntil ) )
        {
        iUnifiedEditor.DeleteLine( ECalenEditorRepeatUntil, EFalse );
        iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_REPEAT_UNTIL_ITEM,
                ECalenEditorRepeatUntil, ECalenEditorRepeat );                    

        }

    iUnifiedEditor.TryChangeFocusToL( lastFocusedItem );
    SetDataToEditorL();
    iUnifiedEditor.UpdateFormL();
    TRACE_EXIT_POINT;
    }

TInt CCalenUnifiedEditorControl::GetCalendarIndexForEntryL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return iDbField->GetCalendarIndexForEntryL();
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::GetCalendarNameForEntryL()
// -----------------------------------------------------------------------------
//
const TDesC& CCalenUnifiedEditorControl::GetCalendarNameForEntryL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return iDbField->GetCalendarNameForEntryL();
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::UpdateMeetingDurationL()
// This function leaves if the end date/time is before the start date/time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::UpdateMeetingDurationL()
    {
    TRACE_ENTRY_POINT;

    TTime start = iUnifiedEditor.Edited().StartDateTime();
    TTime end = iUnifiedEditor.Edited().EndDateTime();
    User::LeaveIfError(
        end.MinutesFrom(start, iMeetingInterval ) );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::UpdateEndTimeL()
// Updates the end date/time when the start date/time is changed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::UpdateEndTimeL()
    {
    TRACE_ENTRY_POINT;

    TTime start = iUnifiedEditor.Edited().StartDateTime();
    
    // Update EndTime in the Editor for meeting Entry
    TTime end = start;
    end += iMeetingInterval;
    end = CalenDateUtils::LimitToValidTime( end );

    // update data model
    iUnifiedEditor.Edited().SetEndDateTimeL( end );

    // update form fields
    UpdateMeetingDurationL();

    if( iAllDayField->IsAllDayEvent() && 
            CCalEntry::EEvent == iUnifiedEditor.Edited().EntryType() )
        {
        // For allday (1 day) event Eg. "Start:- 12:00am, 15-08-2010 &  End:-12:00am, 16-08-2010"
        // In Editor it should be displayed as "StartDate: 15-08-2010 & EndDate:15-08-2010" 
        // No time filed is displayed.
        end -= TTimeIntervalDays( KOneDay );
        if ( end < start )                         
            {
            end = start;
            }
        }
    
    SetDateField( ECalenEditorEndDate, end, ETrue );
    SetTimeField( ECalenEditorEndTime, end, ETrue );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::ReadRrule()
// Checks the Rdates and RRule if there is any with the CCalEntry
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::ReadRrule(TTime& firstRdatestartTime, TTime& endTime)
    {   
    TRACE_ENTRY_POINT;
    
    RArray<TCalTime> rDateList;
    CleanupClosePushL( rDateList );    
    rDateList = iUnifiedEditor.Edited().GetRdatesL();
    
    TInt count = rDateList.Count();    
    TBool hasRDates = ( count > 0 );    
    
    // Get the firstRDate only if the entry has RDates.
    if( hasRDates )
        {
        firstRdatestartTime = rDateList[ 0 ].TimeLocalL();
        TDateTime firstRDateTime = firstRdatestartTime.DateTime();
        
        TTimeIntervalMinutes duration;                            
        TTime entryStartTime = iUnifiedEditor.Edited().StartDateTime();
        TTime entryEndTime = iUnifiedEditor.Edited().EndDateTime();
        
        User::LeaveIfError( entryEndTime.MinutesFrom( entryStartTime, duration ) );                
        endTime= firstRdatestartTime + duration;  
        }       
    else
        {
        firstRdatestartTime = iUnifiedEditor.Edited().StartDateTime();
        endTime =  iUnifiedEditor.Edited().EndDateTime();
        }
    
    CleanupStack::PopAndDestroy();
    
    TRACE_EXIT_POINT;
    }
// -----------------------------------------------------------------------------
// CCalenUnifiedEditorControl::HandleErrorL()
// Handles errors. This function may be called on saving note.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenUnifiedEditorControl::HandleErrorL(const TInt& aError)
    {
    TRACE_ENTRY_POINT;
    if( aError == CCalenEditorDataHandler::EFormErrDurationGreaterThanRepeatInterval )
        {
        iUnifiedEditor.TryChangeFocusToL(ECalenEditorRepeat);
        }
    else if( aError == CCalenEditorDataHandler::EFormErrOutOfSequence )
        {
        iUnifiedEditor.TryChangeFocusToL(ECalenEditorStartDate);
        }
    else if( aError == CCalenEditorDataHandler::EFormErrOverlapsExistingInstance )
        {
        iUnifiedEditor.TryChangeFocusToL(ECalenEditorStartDate);
        }
    else
        {
        if( aError == CCalenEditorDataHandler::EFormErrInstanceAlreadyExistsOnThisDay )
            {   
            iUnifiedEditor.TryChangeFocusToL(ECalenEditorStartDate);
            }    
        }

    if (iReminderField->HandleErrorL(aError) )
        {
        TRACE_EXIT_POINT;
        return;
        }
    if ( iRepeatField->HandleErrorL(aError) )
        {
        TRACE_EXIT_POINT;
        return;
        }

    TRACE_EXIT_POINT;
    }
// End of file


