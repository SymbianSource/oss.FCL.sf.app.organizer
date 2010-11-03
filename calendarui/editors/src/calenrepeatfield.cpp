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
*  Description : 
*
*/

// user includes
#include "calenrepeatfield.h"
#include "CalenDefaultEditors.hrh"
#include "CalenDescription.h"
#include "calennotedatautil.h"
#include "calenentryutil.h"
#include "CalenEditorDataHandler.h"

// system includes
#include <AknPopupField.h>
#include <AknQueryValueText.h>
#include <calentry.h>
#include <calrrule.h>
#include <calendateutils.h>
#include <CalenDefaultEditorsData.rsg>

// debug
#include "calendarui_debug.h"

//Feb 29th will be read as 28 as indexing of days starts from 0,1,2....
const TInt KLastDayInFebLeapYear = 28;

// CONSTRUCTION AND DESTRUCTION METHODS

// -----------------------------------------------------------------------------
// CCalenRepeatField::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenRepeatField* CCalenRepeatField::NewL( CCalenUnifiedEditor& aUnifiedEditor )
    {
    TRACE_ENTRY_POINT;

    CCalenRepeatField* self =
        new( ELeave ) CCalenRepeatField( aUnifiedEditor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::CCalenRepeatField
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenRepeatField::CCalenRepeatField( CCalenUnifiedEditor& aUnifiedEditor )
    : iUnifiedEditor( aUnifiedEditor )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::ConstructL
// second phase constructor
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenRepeatField::ConstructL()
    {
    TRACE_ENTRY_POINT;

    // read resource for confideitiality popup list
    iRepeatArrayText = iUnifiedEditor.ControlEnv()->ReadDesCArrayResourceL(
            R_CALEN_EDITOR_REPEAT_POPUP_ITEMS);

    // create textarray
    iRepeatArray = CAknQueryValueTextArray::NewL();
    iRepeatTextValues = CAknQueryValueText::NewL();
    iRepeatArray->SetArray(*iRepeatArrayText);
    iRepeatTextValues->SetArrayL(iRepeatArray);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::~CCalenRepeatField
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenRepeatField::~CCalenRepeatField()
    {
    TRACE_ENTRY_POINT;

    delete iRepeatArrayText;
    delete iRepeatArray;
    delete iRepeatTextValues;

    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// CCalenRepeatField::HandleControlStateChangedL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenRepeatField::HandleControlStateChangeL( TInt aControlId )
    {
    TRACE_ENTRY_POINT;

    switch (aControlId)
        {
        case ECalenEditorRepeat:
            {
            NotifyChangeRepeatChoiceL();
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
// CCalenRepeatField::HandleErrorL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenRepeatField::HandleErrorL( const TInt& aError )
    {
    TRACE_ENTRY_POINT;

    TBool handled = EFalse;
    switch ( aError )
        {
        case CCalenEditorDataHandler::EFormErrRepeatUntilEarlierThanNote:
            {
            // ReapetUntill is earlier that note instance
            iUnifiedEditor.TryChangeFocusToL( ECalenEditorRepeatUntil );
            handled = ETrue;
            break;
            }
        case CCalenEditorDataHandler::EFormErrStopTimeEarlierThanStartTime:
            {
            TInt newFocusId = ECalenEditorEndDate;
            
            // Stop time erarliar that satrt time of event
            if( CalenDateUtils::OnSameDay( iUnifiedEditor.Edited().StartDateTime(),
                    iUnifiedEditor.Edited().EndDateTime() ) )
                {
                newFocusId = ECalenEditorEndTime;
                }
            iUnifiedEditor.TryChangeFocusToL( newFocusId );

            handled = ETrue;
            break;
            }
        case CCalenEditorDataHandler::EFormErrCancelSaveRepNoteWhenStartChange:
            {
            iUnifiedEditor.TryChangeFocusToL( ECalenEditorStartDate );
            handled = ETrue;
            break;
            }
        default:
            {
            handled = EFalse;
            break;
            }
        }

    TRACE_EXIT_POINT;
    return handled;
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::FormRepeatType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCalenRepeatIndex CCalenRepeatField::FormRepeatType()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return static_cast<TCalenRepeatIndex>( iRepeatTextValues->CurrentValueIndex() );
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::FormRepeatUntilDateTimeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTime CCalenRepeatField::FormRepeatUntilDateTimeL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iUnifiedEditor.EditorFields().ReadDateField( ECalenEditorRepeatUntil );
    }

// ----------------------------------------------------
// CCalenMeetingForm::DeleteOtherItemFromRepeatField
// Delete "Other" item from repeat field.
// It MUST be called in following case.
// 1. Default repeat value is other then "Other".
// 2. Repeat is changed from "Other" to another value.
//
// If "Other" item has already been deleted, nothing is executed.
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenRepeatField::DeleteOtherItemFromRepeatField()
    {
    TRACE_ENTRY_POINT;

    TInt itemCount = iRepeatArrayText->Count();

    if ( itemCount > ERepeatOther )
        // ERepeatOther is the last item index,
        // which is zero origin.
        {
        iRepeatArrayText->Delete( ERepeatOther );
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenRepeatField::NotifyChangeRepeatChoiceL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenRepeatField::NotifyChangeRepeatChoiceL()
    {
    TRACE_ENTRY_POINT;

    TInt repeatType = iRepeatTextValues->CurrentValueIndex();

    if ( repeatType == ERepeatNotRepeated )
        {
        // Repeat type field value selected as 'Non repeating'
        iUnifiedEditor.Edited().SetNonRepeatingL();
        SetRepeatUntilLineDimmedL( ETrue );
        UpdateEndDateTimeFields();
        }
    else
        {
        TTime start = iUnifiedEditor.Edited().StartDateTime();
        TTime until;

        if ( iUnifiedEditor.Edited().IsRepeating() )
            {
            until = iUnifiedEditor.Edited().RepeatUntilDateTime();
            }
        else
            {
			//Fix for repeated event date change:
            TDateTime startDate = start.DateTime();
            
            //Checking for leap year feb.29th, next year it should be 28.
            TInt untilDay;
            if(startDate.Month() == EFebruary && startDate.Day() == KLastDayInFebLeapYear)
                {
                untilDay = KLastDayInFebLeapYear - 1;
                }
            else
                {
                untilDay = startDate.Day();
                }
            
            TDateTime repOneYear(startDate.Year()+1,startDate.Month(), untilDay, 0, 0, 0, 0);
            TTime aTime( repOneYear );
            until = aTime;
            }

        iUnifiedEditor.Edited().SetRepeatingL( static_cast<TCalenRepeatIndex>( repeatType ), until );
        SetRepeatUntilLineDimmedL( EFalse );
        SetDataToEditorL();
        }

    if ( repeatType != ERepeatOther )
        {
        // Nothing is executed if "Other" item has already been deleted.
        DeleteOtherItemFromRepeatField();
        }

    iUnifiedEditor.UpdateFormL();

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenRepeatField::SetRepeatUntilLineDimmedL
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenRepeatField::SetRepeatUntilLineDimmedL( const TBool& aDimmed )
    {
    TRACE_ENTRY_POINT;

    CCoeControl* repUntilCtrl = iUnifiedEditor.ControlOrNull( ECalenEditorRepeatUntil );

    if(aDimmed)
        {
        if( repUntilCtrl )
            {
            // Delete RepeatUntil line from From
            iUnifiedEditor.DeleteLine( ECalenEditorRepeatUntil, EFalse );
            }
        }
    else
        {
        if( !repUntilCtrl )
            {
            // Insert RepeatUntil line to From
            iUnifiedEditor.InsertFieldL( R_CALEN_EDITOR_REPEAT_UNTIL_ITEM,
                              ECalenEditorRepeatUntil, ECalenEditorRepeat );
            }
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenRepeatField::UpdateEndDateTimeFields
// Called when "Repeat" is updated from any repeat value
// to "Not Repeated".
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenRepeatField::UpdateEndDateTimeFields()
    {
    TRACE_ENTRY_POINT;

    TTime end = iUnifiedEditor.Edited().EndDateTime();

	if( iUnifiedEditor.HasField(ECalenEditorEndDate) )
        {
        // Set End Date field value in Editor
        iUnifiedEditor.EditorFields().SetDateField( ECalenEditorEndDate, end, EFalse );
        }
	if( iUnifiedEditor.HasField( ECalenEditorEndTime ) )
        {
        // Set End Time field value in Editor
        iUnifiedEditor.EditorFields().SetTimeField( ECalenEditorEndTime, end );
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::AskRepeatingNoteQueriesL
// Ask the user of the repeat type when neccessary
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorDataHandler::TError CCalenRepeatField::AskRepeatingNoteQueriesL(
    CalCommon::TRecurrenceRange& aRepeatType)
    {
    TRACE_ENTRY_POINT;

    CCalenEditorDataHandler::TError error = CCalenEditorDataHandler::EFormErrNone;

    if( ShouldQueryRepeatTargetL() )
        {
        // Query whether user want to edit This/all instanced of repeat entry.
        error = QueryRepeatTargetL( aRepeatType );
        }

    TRACE_EXIT_POINT;
    return error;
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::ShouldQueryRepeatTargetL
// Check if the repeat query should be shown
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenRepeatField::ShouldQueryRepeatTargetL()
    {
    TRACE_ENTRY_POINT;

    /**
     * Show save to this/all query if:
     *   Editing a repeating entry (not for child entries!!!)
     *    AND
     *   any of the following fields have changed:
     *    subject, location, description,
     *    start time, end time, (not for date changes)
     *    alarm state, alarm time, alarm date
     *
     **/

    if( iUnifiedEditor.IsCreatingNewEntry() ||
        ! iUnifiedEditor.Original().IsRepeating() ||
        ! iUnifiedEditor.Edited().IsRepeating() ||
        iUnifiedEditor.EditorDataHandler().IsChildL() )
        {
        TRACE_EXIT_POINT;
        return EFalse;
        }

    // This should take care of checking start date editing:
    if ( iUnifiedEditor.EditorDataHandler().IsRepeatRuleEdited() )
        {
        TRACE_EXIT_POINT;
        return EFalse;
        }

    TBool isInstanceFieldEdited =
    iUnifiedEditor.EditorDataHandler().IsSummaryEdited() ||
    iUnifiedEditor.EditorDataHandler().IsLocationEdited() ||
    iUnifiedEditor.EditorDataHandler().IsDescriptionEdited() ||
    iUnifiedEditor.EditorDataHandler().IsStartDateTimeEdited() ||
    iUnifiedEditor.EditorDataHandler().IsEndDateTimeEdited() ||
    iUnifiedEditor.EditorDataHandler().IsAlarmEditedL();

    TRACE_EXIT_POINT;
    return isInstanceFieldEdited;
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::QueryRepeatTargetL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorDataHandler::TError CCalenRepeatField::QueryRepeatTargetL(
    CalCommon::TRecurrenceRange& aRepeatType )
    {
    TRACE_ENTRY_POINT;

    const TBool ok = CalenNoteDataUtil::ShowRepeatTypeQueryL( aRepeatType,
                                                CalenNoteDataUtil::ESave );

    if( !ok )    // Repeat dialog is canceled
        {
        TRACE_EXIT_POINT;
        return CCalenEditorDataHandler::EFormErrCancelChangeRepeatingNote;
        }

    TRACE_EXIT_POINT;
    return CCalenEditorDataHandler::EFormErrNone;
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::ReadDataFromFormL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenRepeatField::ReadDataFromFormL( TBool /*aContinueOnError*/ )
    {
    TRACE_ENTRY_POINT;

    // read repeat type data from form 
    TCalenRepeatIndex repeatType = FormRepeatType();
    if ( repeatType == ERepeatNotRepeated )
        {
        iUnifiedEditor.Edited().SetNonRepeatingL();
        }
    else
        {
        TTime until = iUnifiedEditor.Edited().RepeatUntilDateTime();
        until = FormRepeatUntilDateTimeL();
        iUnifiedEditor.Edited().SetRepeatingL( repeatType, until );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::SetDataToEditorL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenRepeatField::SetDataToEditorL()
    {
    TRACE_ENTRY_POINT;
    
    // Update repeat field data in Form
    TCalenRepeatIndex pos = iUnifiedEditor.Edited().RepeatType();
    iRepeatTextValues->SetCurrentValueIndex( static_cast<TInt>( pos ) );
    if ( iUnifiedEditor.Edited().IsRepeating() )
        {
        // Set RepeatUntil date in Form
        TTime until = iUnifiedEditor.Edited().RepeatUntilDateTime();
        iUnifiedEditor.EditorFields().SetDateField( ECalenEditorRepeatUntil, until );
        }
    else
        {
        // Delete RepeatUntil line from Form
        SetRepeatUntilLineDimmedL( ETrue );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::InitRepetFieldLayoutL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenRepeatField::InitRepetFieldLayoutL()
    {
    TRACE_ENTRY_POINT;

    CAknPopupField* pops =
        static_cast<CAknPopupField*>( iUnifiedEditor.Control( ECalenEditorRepeat ) );
    pops->SetQueryValueL(iRepeatTextValues);

    if( iUnifiedEditor.Edited().RepeatType() != ERepeatOther )
        {
        DeleteOtherItemFromRepeatField();
        }

    if ( iUnifiedEditor.Edited().IsRepeating() )
        {
        SetRepeatUntilLineDimmedL( EFalse );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenRepeatField::PrepareForFocusTransitionL
// Prepare for focus change
// -----------------------------------------------------------------------------
//
void CCalenRepeatField::PrepareForFocusTransitionL( TInt aFocusedId )
    {
    TRACE_ENTRY_POINT;

    switch( aFocusedId )
        {
        case ECalenEditorRepeatUntil:
            {
            TBool continueOnError = EFalse;
            ReadDataFromFormL( continueOnError );
            break;
            }
        default:
            {
            break;
            }
        }

    TRACE_EXIT_POINT;
    }

// End of File
