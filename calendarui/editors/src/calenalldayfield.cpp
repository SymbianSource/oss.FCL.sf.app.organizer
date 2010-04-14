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
* Description:  ?Description
*
*/

// user includes
#include "calenalldayfield.h"
#include "calenunifiededitor.h"
#include "CalenDefaultEditors.hrh"
#include <calendateutils.h>

// system includes
#include <CalenDefaultEditorsData.rsg>

// debug
#include "calendarui_debug.h"

// -----------------------------------------------------------------------------
// CCalenAllDayField::NewL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenAllDayField* CCalenAllDayField::NewL( CCalenUnifiedEditor& aUnifiedEditor )
    {
    TRACE_ENTRY_POINT;

    CCalenAllDayField* self =
        new( ELeave ) CCalenAllDayField( aUnifiedEditor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenAllDayField::CCalenAllDayFiled()
// Constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenAllDayField::CCalenAllDayField( CCalenUnifiedEditor& aUnifiedEditor )
    : iUnifiedEditor( aUnifiedEditor )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenAllDayField::ConstructL()
// Leaving construction.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenAllDayField::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    // Set initial value 'ENonAllDayEvent' for AllDay event field
    iIsAllDaySeleceted = CCalenAllDayField::ENonAllDayEvent;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenAllDayFiled::~CCalenAllDayFiled()
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenAllDayField::~CCalenAllDayField()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// CCalenAllDayField::IsAllDayEvent
// Checks to see if event is for all day.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenAllDayField::IsAllDayEvent()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return iIsAllDaySeleceted;
    }

// -----------------------------------------------------------------------------
// CCalenAllDayField::SetAllDayEvent( TBool aAllDaySelected)
// Set AllDay filed value
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenAllDayField::SetAllDayEvent( TBool aAllDaySelected )
    {
    TRACE_ENTRY_POINT;     
    
    iIsAllDaySeleceted = aAllDaySelected;
    
    // Update Allday related fields also.
    TRAP_IGNORE( HandleAllDayFieldChangeL() );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenAllDayField::InitAllDayFieldLayoutL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenAllDayField::InitAllDayFieldLayoutL()
    {
    TRACE_ENTRY_POINT
    
    iUnifiedEditor.EditorFields().SetAllDayFieldL( iIsAllDaySeleceted );
    
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenAllDayField::SetDataToEditorL()
// Reads data from the entry and updates the form with the appropriate values.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenAllDayField::SetDataToEditorL()
    {
    TRACE_ENTRY_POINT;

    iIsAllDaySeleceted = iUnifiedEditor.Edited().IsAllDayEvent();
    
    //To set the Yes/No strings to the alldayfield
    iUnifiedEditor.EditorFields().SetAllDayFieldL( iIsAllDaySeleceted );
    
    // update Time lines as per new AllDay field value.
    InsertAllDayRelatedFieldsL( iIsAllDaySeleceted );
    
    if( iIsAllDaySeleceted )
        {
        TTime startDate = iUnifiedEditor.Edited().StartDateTime();
        TTime endDate = iUnifiedEditor.Edited().EndDateTime();

        // In case AllDay event end at "12:00am" 24/06/2009. 
        // editor should display EndDate as 23/06/2009.
        if( endDate > startDate )
            {
            endDate -= TTimeIntervalDays( 1 );
            if( endDate < startDate )
                {
                endDate = startDate;
                }
            }
		
		iUnifiedEditor.EditorFields().SetDateField( ECalenEditorStartDate, startDate, ETrue );
        iUnifiedEditor.EditorFields().SetDateField( ECalenEditorEndDate, endDate, ETrue );
        }
     else
        { 
        if( iUnifiedEditor.GetEntryType()== CCalEntry::EAppt )
            {
            // update End date field as per Start date field value
            TTime startDate = iUnifiedEditor.Edited().StartDateTime();
            TTime endDate = iUnifiedEditor.Edited().EndDateTime();
            
            iUnifiedEditor.EditorFields().SetDateField( ECalenEditorStartDate, startDate, ETrue );
            iUnifiedEditor.EditorFields().SetTimeField( ECalenEditorStartTime, startDate, ETrue );
            iUnifiedEditor.EditorFields().SetDateField( ECalenEditorEndDate, endDate, ETrue );
            iUnifiedEditor.EditorFields().SetTimeField( ECalenEditorEndTime, endDate, ETrue );
            }
        else if( iUnifiedEditor.GetEntryType()== CCalEntry::EEvent )
            {
            TTime startTime = iUnifiedEditor.Edited().StartDateTime();
            iUnifiedEditor.EditorFields().SetDateField( ECalenEditorStartDate, startTime, ETrue );
            iUnifiedEditor.EditorFields().SetTimeField( ECalenEditorStartTime, startTime, ETrue );
            }
        }    
             
        TRACE_EXIT_POINT
    }


// -----------------------------------------------------------------------------
// CCalenAllDayField::InsertAllDayRelatedFieldsL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenAllDayField::InsertAllDayRelatedFieldsL( TBool aAllDayEvent )
    {
    TRACE_ENTRY_POINT;
    
    CCoeControl* eventStartTimeCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorStartTime );
    CCoeControl* eventEndTimeCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorEndTime );
    
    if( aAllDayEvent )
        {
        // AllDay event, delete Start time & End time fields from form.
        if( eventStartTimeCtrl )
            {
            iUnifiedEditor.DeleteLine( ECalenEditorStartTime,ETrue );
            }
        if( eventEndTimeCtrl )
            {
            iUnifiedEditor.DeleteLine( ECalenEditorEndTime,ETrue );
            }
        }
    else
        {
        // Non AllDay event, add Start time & End time fields to form. 
        if(!eventStartTimeCtrl)
            {       
            iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_START_TIME_ITEM,
                    ECalenEditorStartTime, ECalenEditorAllDayItem );
            }
        if(!eventEndTimeCtrl)
            {
            iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_END_TIME_ITEM,
                    ECalenEditorEndTime, ECalenEditorStartTime );
            }
        }
    
    TRACE_EXIT_POINT;
    }
	
// -----------------------------------------------------------------------------
// CCalenAllDayField::HandleAllDayFieldChangeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenAllDayField::HandleAllDayFieldChangeL()
    {
    TRACE_ENTRY_POINT;

    // Handle AllDay field change
    iUnifiedEditor.Edited().SetAllDayEvent( IsAllDayEvent() );

    if( IsAllDayEvent() )
        {
		// AllDay event set to yes. Change entry type to EEvent
        if(iUnifiedEditor.GetEntryType() == CCalEntry::EAppt)
            {
            iUnifiedEditor.SetEntryType( CCalEntry::EEvent );
            iUnifiedEditor.Edited().SetEntryType( CCalEntry::EEvent );
            }
         }
    else
        {
		// Non AllDay event set to No. Change entry type to EAppt
        if( iUnifiedEditor.GetEntryType() == CCalEntry::EEvent )
            {
            iUnifiedEditor.SetEntryType( CCalEntry::EAppt );
            iUnifiedEditor.Edited().SetEntryType( CCalEntry::EAppt );
            }
        }
     SetDataToEditorL();
     iUnifiedEditor.UpdateFormL();

    TRACE_EXIT_POINT;
    }
 
// -----------------------------------------------------------------------------
// CCalenAllDayField::PrepareForFocusTransitionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenAllDayField::PrepareForFocusTransitionL( TInt /* aFocusedId */ )
    {
    
    TRACE_ENTRY_POINT;
    // Nothing to do 
    TRACE_EXIT_POINT;

    }




