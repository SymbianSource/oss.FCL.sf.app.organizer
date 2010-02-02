/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CalenEditorDataHandler implements all logic of checking and
*                validating editor data
*
*/

// System includes
#include <calendateutils.h>
#include <calenagendautils.h>
#include <calinstance.h>
#include <calalarm.h>
#include <calentry.h>
#include <calrrule.h>
#include <centralrepository.h>
#include <calattachment.h>
#include <eikenv.h>
#include <CalenInterimUtils2.h>
#include <calencontext.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenservices.h>

// User includes
#include "CalenEditorDataHandler.h"
#include "CalendarPrivateCRKeys.h"
#include "CalenDefaultEditors.hrh"
#include "calenentryutil.h"
#include "CleanupResetAndDestroy.h"
#include "calenattachmentutils.h"
#include "calenattachmentinfo.h"
#include "calenattachmentmodel.h"
#include "calenunifiededitor.h"
#include "calendarui_debug.h"

// Constants
#define KNoOfDaysInWeek  7

// ======== LOCAL FUNCTIONS ========

// -----------------------------------------------------------------------------
// IsTimed
// Returns ETrue if the entry is timed (i.e. an appointment or a
// reminder), EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
static TBool IsTimed( CCalEntry::TType aEntryType )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return aEntryType == CCalEntry::EAppt || aEntryType == CCalEntry::EReminder;  
    }
       
// -----------------------------------------------------------------------------
// ForeverDate
// Returns the date which Calendar presumes to be infinitely far in the future.
// Used, for example, when setting a repeating meeting to repeat forever.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
static TTime ForeverDate()
    {
    TRACE_ENTRY_POINT;

    TTime forever = TTime( TDateTime( ECalenForeverYear, static_cast<TMonth>( ECalenForeverMonth ), 
                           ECalenForeverDay, 0, 0, 0, 0) );

    TRACE_EXIT_POINT;
    return forever;
    }

// -----------------------------------------------------------------------------
// IsForeverDate
// Returns ETrue if the given date is considered by Calendar to be infinitely
// far in the future.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
static TBool IsForeverDate( const TTime& aDate )
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return CalenDateUtils::OnSameDay( aDate, ForeverDate() );
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::CCalenEditorDataHandler
// Constructor.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenEditorDataHandler::CCalenEditorDataHandler(
                                            CCalEntry& aEntry,
                                            const TCalTime& aInstanceDateTime,
                                            TBool aIsCreatingNew,
                                            TInt aMaxTextEditorLength,
                                            MCalenServices& aServices,
                                            RArray<TInt>& aCollectionIds)
    : iEntry( aEntry ),
      iInstanceDateTime( aInstanceDateTime ),
      iIsCreatingNew( aIsCreatingNew ),
      iMaxTextEditorLength( aMaxTextEditorLength ),
      iServices( aServices ) , iCalendarFieldEdited(EFalse) , iCollectionIds(aCollectionIds)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::ConstructL
// Two phase construction.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenEditorDataHandler::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iEntryType = iEntry.EntryTypeL();
    iRecurrenceId = iEntry.RecurrenceIdL();

    iOriginal = CCalenEntryUtil::NewL( iEntry, iInstanceDateTime );
    iEdited = CCalenEntryUtil::NewL( iEntry, iInstanceDateTime );
    if ( IsCreatingNew() )
        {
        SetDefaultNewValuesL( *iOriginal ); // modify iOriginal also
        SetDefaultNewValuesL( *iEdited );
        }
    else
        {
        iInstanceId = iServices.Context().InstanceId();
        }
    

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::NewL
// Two-phased constructor.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenEditorDataHandler* CCalenEditorDataHandler::NewL(
                                            CCalEntry& aEntry,
                                            const TCalTime& aInstanceDateTime,
                                            TBool aIsCreatingNew,
                                            TInt aMaxTextEditorLength,
                                            MCalenServices& aServices,
                                            RArray<TInt>& aCollectionIds)
    {
    TRACE_ENTRY_POINT;

    CCalenEditorDataHandler* self = new( ELeave ) CCalenEditorDataHandler(
        aEntry, aInstanceDateTime, aIsCreatingNew, aMaxTextEditorLength, aServices,aCollectionIds );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::~CCalenEditorDataHandler
// Destructor.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenEditorDataHandler::~CCalenEditorDataHandler()
    {
    TRACE_ENTRY_POINT;

    delete iOriginal;
    delete iEdited;

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::Edited
// Returns the CCalenEditorDataHandler used to represent the entry the
// user is currently editing.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenEntryUtil& CCalenEditorDataHandler::Edited()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return *iEdited;
    }

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::Original
// Returns the CCalenEditorDataHandler used to represent the entry the
// user originally opened.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
const CCalenEntryUtil& CCalenEditorDataHandler::Original() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return *iOriginal;
    }

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::Entry
// Returns the entry this object is wrapping round.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalEntry& CCalenEditorDataHandler::Entry()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iEntry;
    }

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::InstanceDateTime
// Returns the instance date/time.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
const TCalTime& CCalenEditorDataHandler::InstanceDateTime() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iInstanceDateTime;
    }


// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::ShouldSaveOrDeleteOrDoNothingL
// Works out whether the entry should be deleted, saved, 
// or whether no action should be taken.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenEditorDataHandler::TAction CCalenEditorDataHandler::ShouldSaveOrDeleteOrDoNothingL()
    {
    TRACE_ENTRY_POINT;

    if ( !IsEditedL() )
        {
        // Not edited at all OR
        // Only added space characters to text fields but not
        // edited the non-text fields
        // no need to save the note
        TRACE_EXIT_POINT;
        return CCalenEditorDataHandler::EActionNothing;        
        }

    // *************** THE NOTE IS EDITED ********************
    if(IsCreatingNew())
        {
        // ************* EDITED NEW NOTE *********   
          
        // Subject && Location && Description are text fields                   
        // If text fields as a whole is not empty, we can save the note
        // If text fields as a whole IS empty, we can STILL save the note
        // since we edited "non-text" fields
        if ( !NonTextFieldsEditedL() && AreTextFieldsEmptyL() )
            {
            TRACE_EXIT_POINT;
            return CCalenEditorDataHandler::EActionNothing;
            }
        else
            {
            TRACE_EXIT_POINT;
            return CCalenEditorDataHandler::EActionSave;
            }
        }

     // **************** EDITED OLD NOTE **********    

     if(AreTextFieldsClearedL() && !NonTextFieldsEditedL()) // subject && location && description
        {
        // ***** OLD NOTE + TEXT FIELDS EMPTIED + NON-TEXT FIELDS NOT EDITED
        // Even if user may have edited non-text fields, 
        // delete the note 
        TRACE_EXIT_POINT;
        return CCalenEditorDataHandler::EActionDelete;        
        }

    // ****** OLD NOTE WITH TEXT FIELDS **********
    // Save the note, since the text fields contain something
    TRACE_EXIT_POINT;
    return CCalenEditorDataHandler::EActionSave;
    }

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::NonTextFieldsEdited
// Returns ETrue if any of the non-text fields (e.g. time fields) of the entry
// have been edited, EFalse otherwise.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::NonTextFieldsEditedL()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    
    return ( IsStartDateTimeEdited() ||
             IsEndDateTimeEdited()   ||
             IsAlarmEditedL()        ||
             IsRepeatRuleEdited()    ||
             IsPriorityEdited()      ||
             IsCalendarEditedL()     ||
             IsAttachmentExists()    ||
             IsEventTypeEdited()     ||
             AllDayFieldEdited() );
    }

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::TextFieldsEdited
// Returns ETrue if any of the text fields (e.g. summary) of the entry
// have been edited, EFalse otherwise.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::TextFieldsEdited()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;

    return ( IsSummaryEdited()  || IsLocationEdited() || IsDescriptionEdited()
            || IsAttachmentExists() );
    }

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::IsEditedL
// Returns ETrue if the entry has been modified, EFalse otherwise.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsEditedL()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;

    return ( TextFieldsEdited() || NonTextFieldsEditedL() );
    }

// ---------------------------------------------------------------------------
// CCalenEditorDataHandler::WriteChangesToEntryL
// Read the data from the form and set it to the entry.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenEditorDataHandler::WriteChangesToEntryL( CalCommon::TRecurrenceRange aRepeatTarget )
    {
    TRACE_ENTRY_POINT;

    // This is only place where we can read data from CCalEntry.
    // E.g. when we calculate alarm offset, we need to read start time from
    // entry, not anymore from CCalenEntryUtil.
    TBool isNew = IsCreatingNew();

    WriteStartAndEndTimesToEntryL( aRepeatTarget );

    if ( isNew || IsSummaryEdited() )
        {
        TPtrC summary = Edited().Summary();
        // If text is considered empty, always store as KNullDesC
        if ( CalenAgendaUtils::IsEmptyText( summary ) )
            {
            summary.Set( KNullDesC );
            }
        iEntry.SetSummaryL( summary );
        }

    if ( isNew || IsLocationEdited() )
        {
        TPtrC location = Edited().Location();
        // If text is considered empty, always store as KNullDesC
        if ( CalenAgendaUtils::IsEmptyText( location ) )
            {
            location.Set(KNullDesC);
            }
        iEntry.SetLocationL( location );
        }

    if ( isNew || IsDescriptionEdited() )
        {
        iEntry.SetDescriptionL( Edited().Description() );
        }

    // Repeat rule has to be modified in both cases
    if ( isNew || IsStartDateTimeEdited() || IsRepeatRuleEdited() )
        {

        if ( aRepeatTarget == CalCommon::EThisOnly)
            {
            iEntry.ClearRepeatingPropertiesL();
            }
        else if ( Edited().IsRepeating() )
            {
            // If we're an RDate (with repeat type ERepeatOther), don't try to set an RRule,
            // but don't clear the repeat properties either.
            if( Edited().RepeatType() != ERepeatOther )
                {
                TCalRRule rrule;

                CalculateRepeatRuleL( Edited().StartDateTime(),
                                      Edited().RepeatType(),
                                      Edited().RepeatUntilDateTime(),
                                      rrule );

                iEntry.SetRRuleL( rrule );
                
                // As there should not be any rdates, remove any that are
                // still present
                RArray<TCalTime> emptyList;
                CleanupClosePushL( emptyList );
                iEntry.SetRDatesL( emptyList );
                CleanupStack::PopAndDestroy(); // emptyList
                }
            else
                {
                // The repeat type is ERepeatOther, therefore as 
                // it is impossible to create an entry of type
                // ERepeat other using the editor either the repeat until
                // date or the start date must have been changed

                // The start date/time has changed, in order for the
                // series to maintain its pattern, any rDates and if
                // present rRule are moved by the same offset
                // The repeat type is ERepeatOther, so check if we have any rdates
                RArray<TCalTime> rDateList;
                CleanupClosePushL( rDateList );
                iEntry.GetRDatesL( rDateList );
                TInt count = rDateList.Count();

                if ( count == 0 )
                    {
                    // There are no rdates so the new until and start date can be applied
                    // directly.
                    TCalRRule rrule;

                    CalculateRepeatRuleL( Edited().StartDateTime(),
                                      Edited().RepeatType(),
                                      Edited().RepeatUntilDateTime(),
                                      rrule );

                    iEntry.SetRRuleL( rrule );
                    }
                else
                    {
                    // There are rDates which need to be checked.
                    if ( IsStartDateTimeEdited() )
                        {
                        // Need to shift any rdates
                        TTime editedStart = iEdited->StartDateTime();
                        TTime origStart = iOriginal->StartDateTime();
                        TTimeIntervalMicroSeconds offSet = editedStart.MicroSecondsFrom( origStart );
                        for ( TInt index = 0; index < count; index++ )
                            {
                            TCalTime& rDateTime = rDateList[ index ];
                            TTime shiftedTime = rDateTime.TimeUtcL();
                            TDateTime before = shiftedTime.DateTime();
                            shiftedTime += offSet;
                            TDateTime after = shiftedTime.DateTime();
                            rDateTime.SetTimeUtcL( shiftedTime ); 
                            }
                        }

                    //  Check and fix the rDates and rRules match the 
                    //  repeat until date and time.
                    TTime untilTime = Edited().RepeatUntilDateTime();
                    
                    // Remove any rdates that are after the the repeat until date
                    TInt count = rDateList.Count();
                    if ( count > 0 )
                        {
                        TInt index = count - 1;
                        do 
                            {
                            TTime lastRDate = CalenDateUtils::BeginningOfDay( rDateList[ index ].TimeLocalL() );
                            TDateTime before = lastRDate.DateTime();
                            if ( lastRDate > untilTime )
                                {
                                rDateList.Remove( index-- );
                                }
                            else
                                {
                                index = KErrNotFound;
                                }
                            } while ( index != KErrNotFound );
                        }
                    
                    // Need to check if the end date of the
                    // rrule needs adjusting if it exists.
                    TCalRRule rRule;
                    if ( iEntry.GetRRuleL( rRule ) )
                        {
                        count = rDateList.Count();
                        if ( count > 0 )
                            {
                            // There still exists some rdates, so only need to trim 
                            // the rrule if it exists
                            TTime lastRDate = CalenDateUtils::BeginningOfDay( rDateList[ count - 1 ].TimeLocalL() );
                            const TTime& origUntilDate = Original().RepeatUntilDateTime();
                            TTime startDT = rRule.DtStart().TimeLocalL();
                            
                            if ( lastRDate <= origUntilDate && startDT > lastRDate )
                                { 
                                if ( startDT < untilTime)
                                    {
                                    if( origUntilDate != untilTime)
                                    	{
										ApplyUntilDateToRRuleL( rRule, untilTime);
                                    	iEntry.SetRRuleL( rRule );
                                    	}
                                    }
                                 else
                                    {
                                    // The repeat start is after the until date
                                    // so remove any repeat information.
                                    iEntry.ClearRepeatingPropertiesL();

                                    // If the entry date has been moved past the until
                                    // date, need to swap the an rDate for the entry.
                                    TTime startTime = iEntry.StartTimeL().TimeLocalL();

                                    if ( startTime > untilTime )
                                        {
                                        // Find the duration of the entry
                                        TTime endTime = iEntry.EndTimeL().TimeLocalL();
                                        TTimeIntervalMinutes duration;
                                        //startTime.MinutesFrom( endTime, duration );// for bug: CMCA-745CZ4
                                        endTime.MinutesFrom( startTime, duration );
                                        
                                        // Choose the first rDate as the new start time
                                        TCalTime newStartTime = rDateList[ 0 ];
                                        endTime = newStartTime.TimeLocalL() + duration;

                                        // FIXME.
                                        // If there is only one rDate left, the agenda model
                                        // will crash if it is deleted.
                                        if ( count != 0 )
                                            {
                                            rDateList.Remove( 0 );
                                            }

                                        TCalTime newEndTime;
                                        if ( UseFloatingTimeL() )
                                            {
                                            newEndTime.SetTimeLocalFloatingL( endTime );
                                            }
                                        else
                                            {
                                            newEndTime.SetTimeLocalL( endTime );
                                            }

                                        iEntry.SetStartAndEndTimeL( newStartTime, newEndTime );
                                        }    
                                    }
                                }
                            }
                        }

                    iEntry.SetRDatesL( rDateList );
                    }

                CleanupStack::PopAndDestroy(); // rDateList
                }
            }
        else
            {
            iEntry.ClearRepeatingPropertiesL();

            // FIXME  As the entry is supposedly not repeating
            // any rDates should be removed. Unforunately this
            // is not possible at the moment because removing the 
            // rdates will cause the agenda model to panic
            //
            // RArray<TCalTime> emptyList;
            // CleanupClosePushL( emptyList );
            // iEntry.SetRDatesL( emptyList );
            // CleanupStack::PopAndDestroy(); // emptyList
            }
        }

    if ( isNew || IsAlarmEditedL() && ( iEntry.StatusL() != CCalEntry::ETodoCompleted ) )
        {
        if ( Edited().IsAlarmActivated() )
            {
            CCalAlarm* alarm  = iEntry.AlarmL();
            if ( ! alarm )
                {
                alarm = CCalAlarm::NewL();
                }
            CleanupStack::PushL( alarm );

            TTime startTime;
            //  ToDos don't have start time! 
            // If we have undated todo with alarm that was taken care earlier
            if (iEntry.EntryTypeL()==CCalEntry::ETodo)
                {
                startTime = iEntry.EndTimeL().TimeLocalL();
                }

            // if this is not todo we use starttime normal way
             else
                {
                startTime = iEntry.StartTimeL().TimeLocalL();
                }
            TCalRRule ignore;
            TBool isRepeating = iEntry.GetRRuleL( ignore );

            // If edit alarm of repeating entry, we have to nudge start
            // time to instance date
            if ( isRepeating &&  aRepeatTarget == CalCommon::EThisAndAll )
                {
                // nudge to instance date;
                TTime instanceDate = iInstanceDateTime.TimeLocalL();
                if ( IsStartDateTimeEdited() )
                    {
                    instanceDate = Edited().EventDateTime();
                    }

                TDateTime insDt = instanceDate.DateTime();
                TDateTime startDt = startTime.DateTime();
                // Don't use individual setters (SetMonth, SetDay ...), because
                // they break if month doesn't contain old  date
                // or date to be set.
                startDt.Set( insDt.Year(), insDt.Month(), insDt.Day(),
                             startDt.Hour(), startDt.Minute(),
                             startDt.Second(), startDt.MicroSecond() );
                startTime = TTime( startDt );
                }

            TTime alarmTime = Edited().AlarmDateTime();

            TTimeIntervalMinutes offset;
            startTime.MinutesFrom( alarmTime, offset );
            alarm->SetTimeOffset( offset );

            iEntry.SetAlarmL( alarm );
            CleanupStack::PopAndDestroy( alarm );
            }
        else
            {
            iEntry.SetAlarmL( NULL );
            }
        }

    if ( isNew || IsPriorityEdited() )
        {
        // map priority here
        TUint priority = 0;
        switch ( Edited().Priority() )
            {
            case CCalenEntryUtil::ETodoPriorityHigh:
                priority = 1;
                break;
            case CCalenEntryUtil::ETodoPriorityNormal:
                priority = 2;
                break;
            case CCalenEntryUtil::ETodoPriorityLow:
                priority = 3;
                break;
            default:
                ASSERT( EFalse );
                break;
            }
        iEntry.SetPriorityL( priority );
        }

    if ( isNew )
        {
        CCalEntry::TReplicationStatus status = CCalEntry::EOpen;
        switch ( Edited().SynchType() )
            {
            case CCalenEntryUtil::ESynchPrivate:
                status = CCalEntry::EPrivate;
                break;
            case CCalenEntryUtil::ESynchPublic:
                status = CCalEntry::EOpen;
                break;
            case CCalenEntryUtil::ESynchNoSync:
                status = CCalEntry::ERestricted;
                break;
            default:
                ASSERT( EFalse );
                break;
            }
        iEntry.SetReplicationStatusL( status );
        }
    
    /**
     * S60 settings for new entries
     */
    if ( IsCreatingNew() )
        {
        if ( Edited().EntryType() == CCalEntry::EAnniv )
            {
            TCalRRule rrule( TCalRRule::EYearly );
            rrule.SetDtStart( iEntry.StartTimeL() );
            rrule.SetInterval( 1 );
            rrule.SetCount( 0 );
            iEntry.SetRRuleL( rrule );
            }
        }
    
    // check for attachments
    if( isNew || IsAttachmentExists() )
        {
        // TODO: Uncomment this when enabling attachment support
        AddAttachementsToEntryL();
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::ApplyUntilDateToRRuleL
// Applies an new until date to a rrule
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorDataHandler::ApplyUntilDateToRRuleL( TCalRRule& aRRule, const TTime& aRepeatUntil ) const
    {
    TRACE_ENTRY_POINT;
     // note repeat is set to forever.
    if ( ! CalenDateUtils::IsValidDay( aRepeatUntil ) || IsForeverDate( aRepeatUntil ) )
        {
        // Set repeat to forever:
        aRRule.SetCount( 0 );
        }
    else
        {
        // Change repeatuntil so that hours, minutes, ... are same as in start date
        TDateTime dtUntil = aRepeatUntil.DateTime();
        TDateTime dtStart= iEntry.StartTimeL().TimeLocalL().DateTime();

        dtUntil.Set( dtUntil.Year(), dtUntil.Month(), dtUntil.Day(),
                     dtStart.Hour(), dtStart.Minute(), dtStart.Second(), dtStart.MicroSecond() );

        TCalTime calRepeatUntil;
        if ( UseFloatingTimeL() )
            {
            calRepeatUntil.SetTimeLocalFloatingL( TTime( dtUntil ) );
            }
        else
            {
            calRepeatUntil.SetTimeLocalL( TTime( dtUntil ) );
            }
        aRRule.SetUntil( calRepeatUntil );
        }
    TRACE_EXIT_POINT;
    }
// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::DurationGreaterThanRepeatIntervalError
// Returns ETrue if the duration of instances of the meeting is greater than
// the repeat period of the series, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::DurationGreaterThanRepeatIntervalError()
    {
    TRACE_ENTRY_POINT;
    TBool isError = EFalse;
    switch( Edited().RepeatType() )
        {
        case ERepeatDaily:
        case ERepeatWorkdays:
            {
            TTimeIntervalDays durDays = Edited().EndDateTime().DaysFrom( Edited().StartDateTime() );
            isError = durDays >= TTimeIntervalDays(1);
            break;
            }
        case ERepeatWeekly:
            {
            TTimeIntervalDays durDays = Edited().EndDateTime().DaysFrom( Edited().StartDateTime() );
            isError = durDays >= TTimeIntervalDays(7);
            break;
            }
        case ERepeatBiWeekly:
            {
            TTimeIntervalDays durDays = Edited().EndDateTime().DaysFrom( Edited().StartDateTime() );
            isError = durDays >= TTimeIntervalDays(14);
            break;
            }
        case ERepeatMonthly:
            {
            TTimeIntervalMonths durMonths = Edited().EndDateTime().MonthsFrom( Edited().StartDateTime() );
            isError = durMonths >= TTimeIntervalMonths(1);
            break;
            }
        case ERepeatYearly:
            {
            TTimeIntervalYears durYears = Edited().EndDateTime().YearsFrom( Edited().StartDateTime() );
            isError = durYears >= TTimeIntervalYears(1);
            break;
            }
        case ERepeatOther:
		    {
            isError = EFalse;
            break;
			}
        default:
            // Not repeating, no error
            isError = EFalse;
            break;
        }
    TRACE_EXIT_POINT;
    return isError;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::CheckErrorsForThisAndAllL
// Check for errors in the form. This function should only be called when the
// user has decided they want to edit "This and all" from a prompt, or when the
// entry is not repeating.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorDataHandler::TError CCalenEditorDataHandler::CheckErrorsForThisAndAllL()
    {
    TRACE_ENTRY_POINT;

    // From UI spec:
    //   5 checks is specified, only one not needed in this phase is 2.1

    const TTime start = Edited().StartDateTime();
    const TTime end = Edited().EndDateTime();

    // 1) Check that entry doesn't end earlier than it starts
    if ( end < start )
        {
        TRACE_EXIT_POINT;
        return CCalenEditorDataHandler::EFormErrStopTimeEarlierThanStartTime;
        }

    // 2) Repeating entry checks:
    if ( Edited().IsRepeating() )
        {
        //    2.1) Check that repeat until date is a) later than start date (for new notes)
        //                                         b) not before start date (for existing notes)
        TTime repeatUntilDay = CalenDateUtils::BeginningOfDay(
            Edited().RepeatUntilDateTime() );

        TTime repeatStartDay;

        // if new note or old note isnt repeating
        //   edited.repeatUntil date must be greater than edited.start date
        // else
        //   if IsRepeatRuleEdited or IsStartDateTimeEdited 
        //     //(either one above will make a new rule in which edited.startdate is the start date)
        //     edited.repeatUntil must be greater than edited.start date
        //   else
        //     edited.repeatUntil must be greater than start date on disk

        TCalRRule rrule;
        if( IsCreatingNew() || 
            !Entry().GetRRuleL(rrule) ||
            IsRepeatRuleEdited() || 
            IsStartDateTimeEdited() )
            {
            // We don't have an rrule so we can't get the rrule start date,
            // or user has edited a field that will cause new start date to be used in the new rule.
            // Use the edited entry's start date.
            repeatStartDay = CalenDateUtils::BeginningOfDay( start );
            }
        else
            {
            // original rrule is valid and new rule will not be created
            repeatStartDay = CalenDateUtils::BeginningOfDay( rrule.DtStart().TimeLocalL() );
            }

        if ( repeatUntilDay <= repeatStartDay )
            {
            TRACE_EXIT_POINT;
            return CCalenEditorDataHandler::EFormErrRepeatUntilEarlierThanNote;
            }

        if( DurationGreaterThanRepeatIntervalError() )
            {
            TRACE_EXIT_POINT;
            return CCalenEditorDataHandler::EFormErrDurationGreaterThanRepeatInterval;
            }
        // 3) Alarm field checks:
        TRACE_EXIT_POINT;
        return CheckAlarmFieldsForErrorsL(ETrue); // user editing the series
        }

    // 3) Alarm field checks:
    TRACE_EXIT_POINT;
    return CheckAlarmFieldsForErrorsL(); // user editing the series
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::CheckErrorsForThisOnlyL
// Check for errors in the form. This function should only be called when the
// user has decided they want to edit "This only" from a prompt, or when the
// entry is an exception.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorDataHandler::TError CCalenEditorDataHandler::CheckErrorsForThisOnlyL( CCalEntry& /*aParentEntry*/,
                                                                                  CCalInstanceView& /*aInstanceView*/ )
    {
    TRACE_ENTRY_POINT;

    const TTime start = Edited().StartDateTime();
    const TTime end = Edited().EndDateTime();

    // Check that entry doesn't end earlier than it starts
    if ( end < start )
        {
        TRACE_EXIT_POINT;
        return CCalenEditorDataHandler::EFormErrStopTimeEarlierThanStartTime;
        }

    TError error = CheckForOutOfSequenceL();

    if( error != EFormErrNone )
        {
        return error;
        }

    // Alarm field checks:
    TRACE_EXIT_POINT;
    return CheckAlarmFieldsForErrorsL(); // user creating/editing an exception
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::CheckForOutOfSequenceL
// Checks to see if the instance is being moved out of sequence, relative to
// the rest of the series.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorDataHandler::TError CCalenEditorDataHandler::CheckForOutOfSequenceL()
    {
    TRACE_ENTRY_POINT;
    // Only perform the checks when we change the start or end time of the meeting.
    if( !IsStartDateTimeEdited() && !IsEndDateTimeEdited() )
        {
        TRACE_EXIT_POINT;
        return EFormErrNone;
        }

    TCalTime prevStart, prevEnd;

    GetPreviousInstanceTimeL( prevStart, prevEnd );

    if( prevStart.TimeLocalL() != Time::NullTTime() )
        {
        // Does the exception end on/after prevEnd?
        if( Edited().StartDateTime() < prevEnd.TimeLocalL() )
            {
            // Does the exception start after prevStart?
            if( Edited().StartDateTime() > prevStart.TimeLocalL() )
                {
                TRACE_EXIT_POINT;
                return EFormErrOverlapsExistingInstance;
                }
            else
                {
                // Does the exception finish after prevStart?
                if( Edited().EndDateTime() > prevStart.TimeLocalL() )
                    {
                    TRACE_EXIT_POINT;
                    return EFormErrOverlapsExistingInstance;
                    }
                else
                    {
                    TRACE_EXIT_POINT;
                    return EFormErrOutOfSequence;
                    }
                }
            }

        // Does the exception start on the same day as prevStart?
        if( CalenDateUtils::OnSameDay( Edited().StartDateTime(), prevStart.TimeLocalL() ) )
            {
            TRACE_EXIT_POINT;
            return EFormErrInstanceAlreadyExistsOnThisDay;
            }
        }

    TCalTime nextStart, nextEnd;

    GetNextInstanceTimeL( nextStart, nextEnd );

    if( nextStart.TimeLocalL() != Time::NullTTime() )
        {
        // Does the exception finish on/before nextStart?
        if( Edited().EndDateTime() > nextStart.TimeLocalL() )
            {
            // Does the exception finish before nextFinish?
            if( Edited().EndDateTime() < nextEnd.TimeLocalL() )
                {
                TRACE_EXIT_POINT;
                return EFormErrOverlapsExistingInstance;
                }
            else
                {
                // Does the exception start before nextFinish?
                if( Edited().StartDateTime() < nextEnd.TimeLocalL() )
                    {
                    TRACE_EXIT_POINT;
                    return EFormErrOverlapsExistingInstance;
                    }
                else
                    {
                    TRACE_EXIT_POINT;
                    return EFormErrOutOfSequence;
                    }
                }
            }

        // Does the exception start on the same day as nextStart?
        if( CalenDateUtils::OnSameDay( Edited().StartDateTime(), nextStart.TimeLocalL() ) )
            {
            TRACE_EXIT_POINT;
            return EFormErrInstanceAlreadyExistsOnThisDay;
            }
        }

    // Does the series have any rdates
    RArray<TCalTime> rDateArray;
    CleanupClosePushL( rDateArray );
    Entry().GetRDatesL( rDateArray );
    TInt rDateCount = rDateArray.Count();

    if ( rDateCount > 0 )
        {
        // If the series has rdates, check that the exception
        // does not overlap or start on the same day
        const TTime& startDateTime = Edited().StartDateTime();
        const TTime& endDateTime = Edited().EndDateTime();
        TTimeIntervalMinutes duration;
        startDateTime.MinutesFrom( endDateTime, duration );

        for ( TInt index = 0; index < rDateCount; index++ )
            {
            const TTime& rDate = rDateArray[ index ].TimeLocalL();

            if ( !CalenDateUtils::OnSameDay( Original().StartDateTime(), rDate ) )
                {
                // Does the exception start or end on the same day as a rdate.
                if ( CalenDateUtils::OnSameDay( startDateTime, rDate ) )
                    {
                    CleanupStack::PopAndDestroy(); // rDateArray
                    TRACE_EXIT_POINT;
                    return EFormErrInstanceAlreadyExistsOnThisDay;
                    }

                // Does the exception overlap an rdate?
                TTime rDateEnd = rDate + duration;
                if ( endDateTime > rDateEnd && startDateTime < rDate )
                    {
                    CleanupStack::PopAndDestroy(); // rDateArray
                    TRACE_EXIT_POINT;
                    return EFormErrOverlapsExistingInstance;
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy(); // rDateArray
    TRACE_EXIT_POINT;
    return EFormErrNone;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::GetPreviousInstanceTimeL
// Gets the start and end times of the previous instance. Sets both to a null
// time if there is no instance before the current one
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorDataHandler::GetPreviousInstanceTimeL( TCalTime& aPreviousStartTime,
                                                        TCalTime& aPreviousEndTime )
    {
    TRACE_ENTRY_POINT;

    aPreviousStartTime.SetTimeLocalL( Time::NullTTime() );
    aPreviousEndTime.SetTimeLocalL( Time::NullTTime() );

    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);

    EntryViewL().FetchL( Entry().UidL(), entries );

    TCalTime currentInstanceDate = Entry().RecurrenceIdL();
    if( currentInstanceDate.TimeUtcL() == Time::NullTTime() )
        {
        // We must be creating a new exception. Calculate the recurrence id.
        TTimeIntervalMinutes timeOfDay = CalenDateUtils::TimeOfDay( entries[0]->StartTimeL().TimeLocalL() );
        TTime beginningOfDay = CalenDateUtils::BeginningOfDay( InstanceDateTime().TimeLocalL() );
        currentInstanceDate.SetTimeLocalL( beginningOfDay + timeOfDay );
        }

    TCalRRule rrule;
    if( entries[0]->GetRRuleL(rrule) )
        {
        TCalenRepeatIndex repeatIndex = CalenNoteDataUtil::RepeatIndexL( *entries[0] );
        TBool keepLooking = ETrue;
        RArray<TCalTime> exdates;
        CleanupClosePushL( exdates );
        entries[0]->GetExceptionDatesL(exdates);
        
        // Needed for case ERepeatOther
        TCalRRule::TType type( rrule.Type() );
        TInt repeatInterval( rrule.Interval() );
        TCalTime start, end;
        TTime previousInstanceTime = Time::NullTTime(); 
        
        while( keepLooking )
            {
            // Subtract the repeat interval of the parent.
            switch( repeatIndex )
                {
                case ERepeatDaily:
                    currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalDays(1) );
                    break;
                case ERepeatWeekly:
                    currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalDays(7) );
                    break;
                case ERepeatBiWeekly:
                    currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalDays(14) );
                    break;
                case ERepeatMonthly:
                    currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalMonths(1) );
                    break;
                case ERepeatYearly:
                    currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalYears(1) );
                    break;
                case ERepeatWorkdays:
                    currentInstanceDate.SetTimeLocalL(currentInstanceDate.TimeLocalL()-TTimeIntervalDays(7) );
                case ERepeatOther:
                	/* This case includes repeating events like: 3 days every week, 3rd weekday of everymonth
                	   that does not fall in any cases above but still have repeating rule*/
                	   
                	 // Check if the current entry being edited is child entry
          			 // If yes, then put back the child entry time to currentInstanceDate  
				     if(Entry().RecurrenceIdL().TimeUtcL() != Time::NullTTime() )
                	 	{
                	 	 TTimeIntervalMinutes timeOfDay = CalenDateUtils::TimeOfDay( Entry().StartTimeL().TimeLocalL() );
        				 TTime beginningOfDay = CalenDateUtils::BeginningOfDay( InstanceDateTime().TimeLocalL() );
        				 currentInstanceDate.SetTimeLocalL( beginningOfDay + timeOfDay );
                	 	}
             
                	 switch( type )
                	 {
                	 	case TCalRRule::EDaily:
                	 		start.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalDays(1 * repeatInterval));
                	 		break;
                	 	case TCalRRule::EWeekly:
                	 		start.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalDays(7 * repeatInterval));
                	 		break;
                	 	case TCalRRule::EMonthly: 
                	 		// Add 7 days of buffer to cover the cases were gap b/w two instances of the event 
                	 		// can go beuong 30 days. Ex: Every third wednesday of every month 
                	 		start.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalMonths(repeatInterval)-TTimeIntervalDays(7 * repeatInterval));
                	 		break;
                	 	case TCalRRule::EYearly:  
                	 		// Add 7 days of buffer to cover the cases were gap b/w two instances of the event 
                	 		// can go beuong 365 days. Ex: Every third wednesday of September of every year
                	 		start.SetTimeLocalL( currentInstanceDate.TimeLocalL()-TTimeIntervalYears(repeatInterval)-TTimeIntervalDays(7 * repeatInterval));
                	 		break;
                	 }
                	 
                	 end.SetTimeLocalL(CalenDateUtils::BeginningOfDay(currentInstanceDate.TimeLocalL()));
                     previousInstanceTime = GetPreviousInstanceForRepeatOtherL(*entries[0], CalCommon::TCalTimeRange( start, end));
                     currentInstanceDate.SetTimeLocalL( previousInstanceTime);
                     break;
                default:
                case ERepeatNotRepeated:
                    keepLooking = EFalse;
                    break;
                }

            // Is currentInstanceDate before parent dt start?
            if( currentInstanceDate.TimeLocalL() < entries[0]->StartTimeL().TimeLocalL() )
                {
                // There are no instances before the exception
                keepLooking = EFalse;
                }
            else
                {
                // Is there an exdate on currentInstanceDate?
                TBool isExdateOnDay = EFalse;
                for(TInt i=0; i<exdates.Count(); ++i)
                    {
                    if( exdates[i].TimeLocalL() == currentInstanceDate.TimeLocalL() )
                        {
                        isExdateOnDay = ETrue;
                        // There is an exdate - is there a child associated with the exdate?
                        for(TInt j=1; j<entries.Count(); ++j)
                            {
                            if( entries[j]->RecurrenceIdL().TimeLocalL() == currentInstanceDate.TimeLocalL() )
                                {
                                // This child is the previous instance.
                                aPreviousStartTime = entries[j]->StartTimeL();
                                aPreviousEndTime = entries[j]->EndTimeL();
                                keepLooking = EFalse;
                                }
                            }
                        break;
                        }
                    }

                if( !isExdateOnDay )
                    {
                    // The instance exists and hasn't been deleted or made into an exception.
                    // Use information from the parent to set the start/end times.
                    aPreviousStartTime = currentInstanceDate;

                    TTimeIntervalMinutes duration;
                    TTime start = entries[0]->StartTimeL().TimeLocalL();
                    TTime end = entries[0]->EndTimeL().TimeLocalL(); 
                    end.MinutesFrom( start, duration );
                    aPreviousEndTime.SetTimeLocalL( currentInstanceDate.TimeLocalL() + duration );
                    keepLooking = EFalse;
                    }
                }
            }
        CleanupStack::PopAndDestroy( &exdates );
        }

    CleanupStack::PopAndDestroy(&entries);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::GetNextInstanceTimeL
// Gets the start and end times of the next instance. Sets both to a null
// time if there is no instance after the current one.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorDataHandler::GetNextInstanceTimeL( TCalTime& aNextStartTime,
                                                    TCalTime& aNextEndTime )
    {
    TRACE_ENTRY_POINT;

    aNextStartTime.SetTimeLocalL( Time::NullTTime() );
    aNextEndTime.SetTimeLocalL( Time::NullTTime() );

    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);

    EntryViewL().FetchL( Entry().UidL(), entries );

    TCalTime currentInstanceDate = Entry().RecurrenceIdL();
    if( currentInstanceDate.TimeUtcL() == Time::NullTTime() )
        {
        // We must be creating a new exception. Calculate the recurrence id.
        TTimeIntervalMinutes timeOfDay = CalenDateUtils::TimeOfDay( entries[0]->StartTimeL().TimeLocalL() );
        TTime beginningOfDay = CalenDateUtils::BeginningOfDay( InstanceDateTime().TimeLocalL() );
        currentInstanceDate.SetTimeLocalL( beginningOfDay + timeOfDay );
        }

    TCalRRule rrule;
    if( entries[0]->GetRRuleL(rrule) )
        {
        TCalenRepeatIndex repeatIndex = CalenNoteDataUtil::RepeatIndexL( *entries[0] );
        TBool keepLooking = ETrue;
        RArray<TCalTime> exdates;
        CleanupClosePushL( exdates );
        entries[0]->GetExceptionDatesL(exdates);
        
        // Needed for case ERepeatOther
        TCalRRule::TType type( rrule.Type() );
        TInt repeatInterval( rrule.Interval() );
        TCalTime start, end;
        TTime nextInstanceTime = Time::NullTTime(); 
        
        while( keepLooking )
            {
            // Subtract the repeat interval of the parent.
            switch( repeatIndex )
                {
                case ERepeatDaily:
                    currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalDays(1) );
                    break;
                case ERepeatWeekly:
                    currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalDays(7) );
                    break;
                case ERepeatBiWeekly:
                    currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalDays(14) );
                    break;
                case ERepeatMonthly:
                    currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalMonths(1) );
                    break;
                case ERepeatYearly:
                    currentInstanceDate.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalYears(1) );
                    break;
                case ERepeatWorkdays:
                    currentInstanceDate.SetTimeLocalL(currentInstanceDate.TimeLocalL()+TTimeIntervalDays(7) );
                    break;
                case ERepeatOther:
                    {
                	/* This case includes repeating events like: 3 days every week, 3rd weekday of everymonth
                	   that does not fall in any cases above but still have repeating rule*/
          			 
          			 // Check if the current entry being edited is child entry
          			 // If yes, then put back the child entry time to currentInstanceDate
				     if(Entry().RecurrenceIdL().TimeUtcL() != Time::NullTTime())
                	 	{
                	 	 TTimeIntervalMinutes timeOfDay = CalenDateUtils::TimeOfDay( Entry().StartTimeL().TimeLocalL() );
        				 TTime beginningOfDay = CalenDateUtils::BeginningOfDay( InstanceDateTime().TimeLocalL() );
        				 currentInstanceDate.SetTimeLocalL( beginningOfDay + timeOfDay );
                	 	}
                	 switch( type )
                	     {
                	 	case TCalRRule::EDaily:
                	 		end.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalDays(1 * repeatInterval));
                	 		break;
                	 	case TCalRRule::EWeekly:
                	 		end.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalDays(7 * repeatInterval));
                	 		break;
                	 	case TCalRRule::EMonthly:  
                	 		// Add 7 days of buffer to cover the cases were gap b/w two instances of the event 
                	 		// can go beuong 30 days. Ex: Every third wednesday of every month
                	 		end.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalMonths(repeatInterval)+TTimeIntervalDays(7 * repeatInterval));
                	 		break;
                	 	case TCalRRule::EYearly:  
                	 		// Add 7 days of buffer to cover the cases were gap b/w two instances of the event 
                	 		// can go beuong 365 days. Ex: Every third wednesday of September of every year
                	 		end.SetTimeLocalL( currentInstanceDate.TimeLocalL()+TTimeIntervalYears(repeatInterval)+TTimeIntervalDays(7 * repeatInterval));
                	 		break;
                	     }
                	 
                	 start.SetTimeLocalL(CalenDateUtils::BeginningOfDay(currentInstanceDate.TimeLocalL()+TTimeIntervalDays(1)));
                	 
                     nextInstanceTime = GetNextInstanceForRepeatOtherL(*entries[0], CalCommon::TCalTimeRange( start, end));
                     currentInstanceDate.SetTimeLocalL( nextInstanceTime);
                     break;
                    }
                case ERepeatNotRepeated:
                    keepLooking = EFalse;
                    break;
                default:
                    break;
                }

            // Is currentInstanceDate after parent dt end?
            if( currentInstanceDate.TimeLocalL() > rrule.Until().TimeLocalL() )
                {
                // There are no instances before the exception
                keepLooking = EFalse;
                }
            else
                {
                // Is there an exdate on currentInstanceDate?
                TBool isExdateOnDay = EFalse;
                for(TInt i=0; i<exdates.Count(); ++i)
                    {
                    if( exdates[i].TimeLocalL() == currentInstanceDate.TimeLocalL() )
                        {
                        isExdateOnDay = ETrue;
                        // There is an exdate - is there a child associated with the exdate?
                        for(TInt j=1; j<entries.Count(); ++j)
                            {
                            if( entries[j]->RecurrenceIdL().TimeLocalL() == currentInstanceDate.TimeLocalL() )
                                {
                                // This child is the previous instance.
                                aNextStartTime = entries[j]->StartTimeL();
                                aNextEndTime = entries[j]->EndTimeL();
                                keepLooking = EFalse;
                                }
                            }
                        break;
                        }
                    }
                
                if( !isExdateOnDay )
                    {
                    // The instance exists and hasn't been deleted or made into an exception.
                    // Use information from the parent to set the start/end times.
                    aNextStartTime = currentInstanceDate;

                    TTimeIntervalMinutes duration;
                    TTime start = entries[0]->StartTimeL().TimeLocalL();
                    TTime end = entries[0]->EndTimeL().TimeLocalL();
                    end.MinutesFrom( start, duration );
                    aNextEndTime.SetTimeLocalL( currentInstanceDate.TimeLocalL() + duration );
                    keepLooking = EFalse;
                    }
                }
            }
        CleanupStack::PopAndDestroy( &exdates );
        }

    CleanupStack::PopAndDestroy(&entries);

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::CheckAlarmFieldsForErrorsL
// Check the alarm fields for errors. Returns the error if found, or
// EFormErrNone if no error found.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorDataHandler::TError CCalenEditorDataHandler::CheckAlarmFieldsForErrorsL(TBool aSeries)
    {
    TRACE_ENTRY_POINT;

    TError error = EFormErrNone;
    // If alarm not active, no check
    if ( Edited().IsAlarmActivated() )
        {
        TTime alarm     = Edited().AlarmDateTime();
        TTime startTime = Edited().StartDateTime();
        TTime now(Time::NullTTime());
        now.HomeTime();

        if(IsAlarmInAcceptablePeriod(error, alarm, startTime))
            {
            // ask form (not iEntry) if it is repeating
            // if we launched an old exception, then form has no repeat field
            // if creating a new exception, then form has repeat field
            TBool EventIsInFuture = (IsTimedEntry() && (startTime > now)) ||
                                    (!IsTimedEntry() && (startTime.DaysFrom(now) >= TTimeIntervalDays(0)));
            if( !aSeries && (alarm < now) && ( iEdited->EntryType()!=CCalEntry::EAnniv ) )
                {
                // dont let non-repeating future entries have alarms in past
            	error = EFormErrAlarmTimePast;
                }
            /*  otherwise, save alarm to past:
             *   new/old non-repeating & in past
             *  repeating new/old repeating in past/future
             */
            }
        }

    TRACE_EXIT_POINT;
    return error;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsAlarmInAcceptablePeriod
// Check alarm time validity
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsAlarmInAcceptablePeriod(TError& aError, 
const TTime& aAlarmTime, const TTime& aStartTime) const
    {
    TRACE_ENTRY_POINT;

    TTime upperLimit = IsTimedEntry() ? 
                       aStartTime : 
                       CalenDateUtils::BeginningOfDay(aStartTime) + TTimeIntervalDays(1);
    TTime lowerLimit = aStartTime - TTimeIntervalDays(31);
    TBool acceptable = ETrue;
    if(aAlarmTime < lowerLimit)
        {
        acceptable = EFalse;
        aError = EFormErrAlarmDateTooManyDaysBeforeNote;
        }
    else if(aAlarmTime > upperLimit)
        {
        acceptable = EFalse;
        aError = EFormErrAlarmTimeLaterThanNote;
        }

    TRACE_EXIT_POINT;
    return acceptable;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::ForceValidValuesL
// Force the form to use valid values. This function gets called when the user
// forces the application to exit, for example by pressing the off button.
// We attempt to save as much data as possible from the form.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorDataHandler::ForceValidValuesL( CalCommon::TRecurrenceRange aRepeatTarget )
    {
    TRACE_ENTRY_POINT;

    // Checks and valid values are
    // from S60 Calendar UI spec, chapter 4.6.4.1 Exiting Note Editor:

    // We modify values as we go, thus we can't pre-read fields.
    // E.g. when start time is needed in check, it should be always read
    // from model, because previous check might have modified it.

    //  2) if ends earlier than starts
    if ( Edited().EndDateTime() < Edited().StartDateTime() )
        {
        //  a) (for new entry) set end time equal to start time
        if ( IsCreatingNew() )
            {
            Edited().SetEndDateTimeL( Edited().StartDateTime() );
            }
        //  b) (for existing entries) replace with original end time
        else
            {
            Edited().SetEndDateTimeL( Original().EndDateTime() );
            // if still wrong, replace start time also
            Edited().SetStartDateTimeL( Original().StartDateTime() );
            }
        } // END end < start check

    // 1) Alarm validation
    if ( Edited().IsAlarmActivated() )
        {
        TTime now;
        now.HomeTime();

        // Note: all three checks are done for each alarm.
        // Alarm time is read always again, because modifications are done after each check

        //  1.1) a) if alarm more than 31 days before start, alarm is set 31 days before start
        //       b) (for todos) - "" - before due date
        TTime maxAlarmBeforeStart = Edited().StartDateTime() - TTimeIntervalDays( 31 );
        if ( Edited().AlarmDateTime() < maxAlarmBeforeStart  )
            {
            Edited().SetAlarmDateTimeL( maxAlarmBeforeStart );
            }

        //  1.2) a) (meetings) if alarm later than start, set it to default alarm time
        //       b) (for todos & annivs) - "" - later than due date
        TTime atLatest = Edited().StartDateTime();
        if(!IsTimedEntry())
            {
            atLatest += TTimeIntervalDays(1); 
            atLatest -= TTimeIntervalSeconds(1);
            // at Latest for ToDo/Anniv would be DD @ 23:59:59
            }
        if( Edited().AlarmDateTime() > atLatest )
            {            
            SetDefaultAlarmDateTimeL();
            }

        //  1.3) if alarm is set to passed time,
        if ( Edited().AlarmDateTime() < now )
            {
            // a) it is set off for
            //    - new/old NON-REPEATING entries
            if (!Edited().IsRepeating() && Edited().EntryType() != CCalEntry::EAnniv )
                {
                Edited().SetAlarmOffL();
                }
            // b) (for existing entries) it is saved, unless it has failed 1.1 or 1.2
            else
                {
                // keep, if it is repeating entry, since repeating entries 
                // can have alarms in past
                }
            }
        } // END alarm checks

    //  3) Repeat rule checks
    if ( Edited().IsRepeating() )
        {
        // if repeating note's repeat until date
        // a) (for new entries)  isn't one day later than start date
        // b) (for existing entries) is before start date
        // set it to one year later from start date
        TTime repeatUntilDay = CalenDateUtils::BeginningOfDay(
            Edited().RepeatUntilDateTime() );
        TTime startDay = CalenDateUtils::BeginningOfDay( Edited().StartDateTime() );
        if ( (  IsCreatingNew() && repeatUntilDay <= startDay) ||
             (! IsCreatingNew() && repeatUntilDay < startDay) )
            {
            Edited().SetRepeatUntilDateTimeL( startDay + TTimeIntervalYears(1) );
            }
        }

    //  4) if due date of todo is passed or invalid, save it with entered date
    // no operation (this is just clarification in UI spec


    //  5) Check for out of sequence for exceptions
    //  6) Check for duration > rpt interval for series
    
    if( aRepeatTarget == CalCommon::EThisOnly )
        {
        TError error = CheckForOutOfSequenceL();

        if( error != EFormErrNone )
            {
            // Revert start/end times
            Edited().SetStartDateTimeL( Original().StartDateTime() );
            Edited().SetEndDateTimeL( Original().EndDateTime() );
            }
        }
    else if( aRepeatTarget == CalCommon::EThisAndAll )
        {
        if( DurationGreaterThanRepeatIntervalError() )
            {
            // Revert start/end times
            Edited().SetStartDateTimeL( Original().StartDateTime() );
            Edited().SetEndDateTimeL( Original().EndDateTime() );
            }
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsOldEntry
// Returns ETrue if the entry was already created previously so that
// we are now editing it, returns EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsOldEntry()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return !IsCreatingNew();
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::AreTextFieldsEmptyL
// Returns ETrue if summary && location && description text fields are all empty
// Returns EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::AreTextFieldsEmptyL()
    {
    TRACE_ENTRY_POINT;

    // We have to consider texts empty, if only summary is empty, in case
    // location is not supported, because location can still exists in database.
    TBool summaryAndDescriptionEmpty = CalenAgendaUtils::IsEmptyText( iEdited->Summary() ) && 
    CalenAgendaUtils::IsEmptyText( iEdited->Description() );

    if(LocationEnabledInUiL())
        {
        TRACE_EXIT_POINT;
        return summaryAndDescriptionEmpty && CalenAgendaUtils::IsEmptyText( iEdited->Location() );        
        }
    
    TRACE_EXIT_POINT;

    return summaryAndDescriptionEmpty; // and location is empty implicitly
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::AreTextFieldsClearedL
// Returns ETrue if the user cleared the text in the location and summary fields.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::AreTextFieldsClearedL()
    {
    TRACE_ENTRY_POINT;

    if ( CalenAgendaUtils::IsEmptyText( iEdited->Summary() ) &&
        CalenAgendaUtils::IsEmptyText( iEdited->Description() ) )
        {
        if ( LocationEnabledInUiL() )
            {
            if ( CalenAgendaUtils::IsEmptyText( iEdited->Location() ) )
                {
                if ( IsSummaryEmptied() || IsDescriptionEmptied() || IsLocationEmptied() )
                    {
                    TRACE_EXIT_POINT;
                    return ETrue;
                    }
                }
            }

        else if ( IsSummaryEmptied() || IsDescriptionEmptied() )
            {
            TRACE_EXIT_POINT;
            return ETrue;
            }
        }

    TRACE_EXIT_POINT;

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsDescriptionEmptied
// Returns ETrue, the description was not empty in original && is empty
// in the edited note
// Returns EFalse, otherwise
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CCalenEditorDataHandler::IsDescriptionEmptied()
    {
    TRACE_ENTRY_POINT;

    if( !CalenAgendaUtils::IsEmptyText( iOriginal->Description() ) &&
        CalenAgendaUtils::IsEmptyText( iEdited->Description() ) )
        {
        TRACE_EXIT_POINT;
        return ETrue;
        }

    TRACE_EXIT_POINT;

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsSummaryEmptied
// Returns ETrue, the summary was not empty in original && is empty
// in the edited note
// Returns EFalse, otherwise
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CCalenEditorDataHandler::IsSummaryEmptied()
    {
    TRACE_ENTRY_POINT;

    if( !CalenAgendaUtils::IsEmptyText( iOriginal->Summary() ) &&
        CalenAgendaUtils::IsEmptyText( iEdited->Summary() ) )
        {
        TRACE_EXIT_POINT;
        return ETrue;
        }

    TRACE_EXIT_POINT;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsLocationEmptied
// Returns ETrue, the summary was not empty in original && is empty
// in the edited note
// Returns EFalse, otherwise
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CCalenEditorDataHandler::IsLocationEmptied()
    {
    TRACE_ENTRY_POINT;

    if( !CalenAgendaUtils::IsEmptyText( iOriginal->Location() ) &&
        CalenAgendaUtils::IsEmptyText( iEdited->Location() ) )
        {
        TRACE_EXIT_POINT;
        return ETrue;
        }

    TRACE_EXIT_POINT;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsEntryTypeEdited
// Returns ETrue if the 'Entry Type' is edited, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsEntryTypeEdited()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iOriginal->EntryType() != iEdited->EntryType();
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsStartDateTimeEdited
// Returns ETrue if the start date/time has been edited, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsStartDateTimeEdited()
    {
    TRACE_ENTRY_POINT;
    
    TBool isEdited = EFalse;
    
    TTime orgTTime = iOriginal->StartDateTime();
    TTime editedTTime = iEdited->StartDateTime();
    
    // Get TDateTime of Original & Edited entries.
    TDateTime orgDT = orgTTime.DateTime();
    TDateTime editedDT = editedTTime.DateTime();
    
    if( !IsTimedEntry() && IsCreatingNew() )
        {
        // For non-timed event,on while creating new entry.
        // check for only Date field change only.
        if( orgDT.Day() != editedDT.Day() ||
                orgDT.Month() != editedDT.Month() ||
                orgDT.Year() != editedDT.Year() )
            {
            isEdited = ETrue;
            }
        }
    else
        {
        
        // For timed entry, check for Entry Date/Time Changes
        if( orgTTime != editedTTime )
            {
            isEdited = ETrue;
            }
        }
		
    TRACE_EXIT_POINT;
    return isEdited;
    
    //return iOriginal->StartDateTime() != iEdited->StartDateTime();
     
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsEndDateTimeEdited
// Returns ETrue if the end date/time has been edited, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsEndDateTimeEdited()
    {
    TRACE_ENTRY_POINT;
    TBool isEdited = EFalse;
    
    TTime orgTTime = iOriginal->EndDateTime();
    TTime editedTTime = iEdited->EndDateTime();
    
    // Get TDateTime of Original & Edited entries.
    TDateTime orgDT = orgTTime.DateTime();
    TDateTime editedDT = editedTTime.DateTime();
    
    if( !IsTimedEntry() && IsCreatingNew() )
        {
        
        // For non-timed event, check for only Date field change.
        // No need to check for Entry time change.
        if( orgDT.Day() != editedDT.Day() ||
                orgDT.Month() != editedDT.Month() ||
                orgDT.Year() != editedDT.Year() )
            {
            isEdited = ETrue;
            }
        }
    else
        {
        // For timed entry, check for Entry Date/Time Changes
        if( orgTTime != editedTTime )
            {
            isEdited = ETrue;
            }
        }
    
	TRACE_EXIT_POINT;
    return isEdited;

    //return iOriginal->EndDateTime() != iEdited->EndDateTime();
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsDescriptionEdited
// Returns ETrue if the description has been modified, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsDescriptionEdited()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iOriginal->Description() != iEdited->Description();
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsPriorityEdited
// Returns ETrue if the priotity has been modified, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsPriorityEdited()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iOriginal->Priority() != iEdited->Priority();
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsCalendarEditedL
// Returns ETrue if the calendar filed has been edited, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsCalendarEditedL()
    {
    TRACE_ENTRY_POINT;
      
    TRACE_EXIT_POINT;
    return iCalendarFieldEdited;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsSummaryEdited
// Returns ETrue if the summary has been edited, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsSummaryEdited()
    {
    TRACE_ENTRY_POINT;

    // For summary and location, we compare only that part of text
    // that fits to editor. If it's equal, then we don't go and unnecessarily
    // truncate existing data.
    TRACE_EXIT_POINT;
    return ( iOriginal->Summary().Left( iMaxTextEditorLength ) 
                != iEdited->Summary().Left( iMaxTextEditorLength ) );
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsLocationEdited
// Returns ETrue if the location has been edited, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsLocationEdited()
    {
    TRACE_ENTRY_POINT;

    // For summary and location, we compare only that part of text
    // that fits to editor. If it's equal, then we don't go and unnecessarily
    // truncate existing data.
    TRACE_EXIT_POINT;
    return ( iOriginal->Location().Left( iMaxTextEditorLength ) 
                != iEdited->Location().Left( iMaxTextEditorLength ) );
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsAlarmEditedL
// Returns ETrue if the alarm has been edited, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsAlarmEditedL()
    {
    TRACE_ENTRY_POINT;

    if ( iOriginal->IsAlarmActivated() && iEdited->IsAlarmActivated() )
        {
        // The alarm must be flagged as changed if the interval between the entry start
        // and the alarm is different. (We can't just compare alarm times directly because
        // the start date of the entry may have changed.)
        TTimeIntervalMinutes origOffset, editedOffset;
        
        User::LeaveIfError( iOriginal->AlarmDateTime().MinutesFrom( iOriginal->StartDateTime(), origOffset ) );
        User::LeaveIfError( iEdited->AlarmDateTime().MinutesFrom( iEdited->StartDateTime(), editedOffset ) );
        
        TBool alarmTimeChanged = origOffset != editedOffset;

        TRACE_EXIT_POINT;
        return alarmTimeChanged;
        }
    else
        {
        TBool statusChanged = iOriginal->IsAlarmActivated() != iEdited->IsAlarmActivated();

        TRACE_EXIT_POINT;
        return statusChanged;
        }
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsRepeatRuleEdited
// Returns ETrue if the repeat rule has been edited, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsRepeatRuleEdited()
    {
    TRACE_ENTRY_POINT;

    if ( iOriginal->IsRepeating() && iEdited->IsRepeating() )
        {
        TBool repeatTypeChanged = iOriginal->RepeatType() != iEdited->RepeatType();
        TBool untilDateChanged = iOriginal->RepeatUntilDateTime() != iEdited->RepeatUntilDateTime();

        TRACE_EXIT_POINT;
        return repeatTypeChanged || untilDateChanged;
        }
    else
        {
        TBool statusChanged = iOriginal->IsRepeating() != iEdited->IsRepeating();

        TRACE_EXIT_POINT;
        return statusChanged;
        }
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::LocationEnabledInUiL
// Returns ETrue if the location is a valid field for this type of entry
// (i.e. meetings & reminders only), EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::LocationEnabledInUiL()
    {
    TRACE_ENTRY_POINT;

    // Location is enabled for timed entries (i.e. an appointment or a
    // reminder)
    TRACE_EXIT_POINT;
    return IsTimedEntry();
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsTimedEntry
// Returns ETrue if this is a timed entry, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsTimedEntry() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return IsTimed( iEdited->EntryType() );
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsCreatingNew
// Returns ETrue if the user is creating a new entry, EFalse if the user is
// editing an existing entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsCreatingNew() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iIsCreatingNew;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsStartDateChanged
// Returns ETrue if the start date of the entry has been changed, 
// EFalse otherwise. Note that this function only checks the date, 
// and not the time of day, of the entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsStartDateChanged()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return ! CalenDateUtils::OnSameDay( iOriginal->StartDateTime(),
                                        iEdited->StartDateTime() );
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsAttachmentExists
// Returns ETrue calendar entry is having attachments
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsAttachmentExists()
    {
    TRACE_ENTRY_POINT;
    TBool attachmentExits(EFalse);
    
    // TODO: Uncomment this when enabling attachment support
    
    if( (iOriginal->AttachmentCount() != iEdited->AttachmentCount())
         || iServices.GetAttachmentData()->NumberOfItems() )
        {
        attachmentExits = ETrue;
        }
    
    TRACE_EXIT_POINT;
    return attachmentExits;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsEventTypeEdited
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsEventTypeEdited()
    {
    TRACE_ENTRY_POINT;
    TBool fieldModified = EFalse;
    
    // Check EventType field is edited for already saved Entry
    // For Newly created entry, ignore the EventTypeChange
    if( (iOriginal->EntryType() != iEdited->EntryType()) && !IsCreatingNew() )
        {
        fieldModified = ETrue;
        }
    
    TRACE_EXIT_POINT;
    return fieldModified;
    }
	
// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::AllDayFieldEdited
// Returns ETrue if `All day event' line has been modified.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::AllDayFieldEdited()
    {
    TRACE_ENTRY_POINT;
    TBool fieldModified = EFalse;
    
    if( iOriginal->IsAllDayEvent() != iEdited->IsAllDayEvent() )
        {
        fieldModified = ETrue;
        }
    
    TRACE_EXIT_POINT;
    return fieldModified;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::SetDefaultAlarmDateTimeL
// Sets the alarm to the default value. This function is called after the user
// forces an exit, e.g. when pressing the power off button.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorDataHandler::SetDefaultAlarmDateTimeL()
    {
    TRACE_ENTRY_POINT;

    /**
     * Default alarm time is UI spec logic.
     * It is important that this is here, because there shouldn't be UI spec logic in CCalenEntryUtil,
     * only in CCalenEditorDataHandler.
     *
     * It is still a bit odd, that other set methods are in CCalenEntryUtil but not here.
     * Maybe I add wrapper set methods for edited data also in *Handler.
     *   e.g. void SetAlarmOffL() { Edited().SetAlarmOffL(); }
     *
     */
    TTime defaultAlarm;

    if ( IsTimedEntry() )
        {
        TTimeIntervalMinutes defaultOffsetM( 15 ); // FIXME: should be read from central repository
        defaultAlarm = Edited().StartDateTime() - defaultOffsetM;
        }
    else
        {
        TTimeIntervalHours defaultOffsetH( -8 );
        defaultAlarm = CalenDateUtils::BeginningOfDay( Edited().EventDateTime() ) - defaultOffsetH;
        }

    Edited().SetAlarmOnL( defaultAlarm );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::CalculateRepeatRuleL
// Calculates the repeat rule for a given entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorDataHandler::CalculateRepeatRuleL(
    const TTime& aStartDateTime,
    TCalenRepeatIndex aNewRepeatType,
    const TTime& aRepeatUntil,
    TCalRRule& aRRule)
    {
    TRACE_ENTRY_POINT;

    // FIXME: This is copy-pasted from old implementation.
    // This should be rewritten with care.
        //If it ain't broke...?
    TTime startDateTime = aStartDateTime;
    TTime repeatUntil = aRepeatUntil;

    TCalRRule origRRule;
    TBool hasOrigRRule = iEntry.GetRRuleL( origRRule );

    TBool isStartDateSame = CalenDateUtils::OnSameDay(
        Original().StartDateTime(), Edited().StartDateTime() );

    //
    // If repeat type is changed, repeat start date
    // is replaced to the date of "Start date" field.
    //
    if ( hasOrigRRule &&
         isStartDateSame &&
         aNewRepeatType == Original().RepeatType() )
        {
        aRRule = origRRule; // keep the original setting...
        // until date changed?
        }
    else if (aNewRepeatType == ERepeatOther)
        {
        // prevRptDef is ALWAYS non-NULL in this block,
        // because previous note's repeat ALWAYS exists.
        aRRule = origRRule;
        TCalTime repeatStart;
        if ( UseFloatingTimeL() )
            {
            repeatStart.SetTimeLocalFloatingL( startDateTime );
            }
        else
            {
            repeatStart.SetTimeLocalL( startDateTime );
            }
        aRRule.SetDtStart( repeatStart );
        }
    else
        {
        TCalTime repeatSt;
        if ( UseFloatingTimeL() )
            {
            repeatSt.SetTimeLocalFloatingL( startDateTime );
            }
        else
            {
            repeatSt.SetTimeLocalL( startDateTime );
            }
        aRRule.SetDtStart( repeatSt );
        aRRule.SetInterval( 1 );

        TInt interval(1);

        switch ( aNewRepeatType )
            {
            case (ERepeatDaily):  // Daily
                {
                aRRule.SetType( TCalRRule::EDaily );
                break;
                }
            case (ERepeatWeekly):  // Weekly
                {
                if (hasOrigRRule && origRRule.Type() == TCalRRule::EDaily &&
                    origRRule.Interval() == 7)
                    {
                    // If old note's repeat is every 7 days,
                    // set repeat to every 7 days to Agenda, NOT every week.
                    //
                    aRRule.SetType( TCalRRule::EDaily );
                    interval = 7;
                    }
                else
                    {
                    aRRule.SetType( TCalRRule::EWeekly );

                    RArray<TDay> dayArray;
                    CleanupClosePushL( dayArray );
                    dayArray.Append( aRRule.DtStart().TimeLocalL().DayNoInWeek() );
                    aRRule.SetByDay( dayArray );
                    CleanupStack::PopAndDestroy( &dayArray );
                    }
               break;
                }
            case (ERepeatBiWeekly):  // Bi-weekly
                {
                if ( hasOrigRRule && origRRule.Type() == TCalRRule::EDaily &&
                     origRRule.Interval() == 14)
                    {
                    // If old note's repeat is every 14 days,
                    // set repeat to every 14 days to Agenda, NOT every 2 weeks.
                    //
                    aRRule.SetType( TCalRRule::EDaily );
                    interval = 14;
                    }
                else
                    {
                    aRRule.SetType( TCalRRule::EWeekly );
                    interval = 2;

                    RArray<TDay> dayArray;
                    CleanupClosePushL( dayArray );
                    dayArray.Append( aRRule.DtStart().TimeLocalL().DayNoInWeek() );
                    aRRule.SetByDay( dayArray );
                    CleanupStack::PopAndDestroy( &dayArray );
                    }
                break;
                }
                case ERepeatWorkdays:// Weekly
                    {
                    TLocale iLocale;
                    TUint iWorkDays = iLocale.WorkDays();
                    if (hasOrigRRule && origRRule.Type() == TCalRRule::EDaily
                        && origRRule.Interval() == 7)
                        {
                        // If old note's repeat is every 7 days,
                        // set repeat to every 7 days to Agenda, NOT every week.
                        //
                        aRRule.SetType(TCalRRule::EDaily);
                        interval = 7;
                        }
                    else
                        {
                        aRRule.SetType(TCalRRule::EWeekly);
                        RArray<TDay> dayArray;
                        CleanupClosePushL(dayArray); 
                        TInt fixedNum = 1;
                        TInt ruleday = 0;
                        for (TInt i =0 ; i < KNoOfDaysInWeek ; i++)
                            {
                            ruleday = fixedNum << i;
                            if(iWorkDays & ruleday)
                                {
                                dayArray.Append((TDay)i);
                                aRRule.SetByDay(dayArray);
                                }   
                            } 
                         CleanupStack::PopAndDestroy(&dayArray);
                         }
                    break;
                                  
                    }
            case (ERepeatMonthly):
                // Monthly By Position
                {
                aRRule.SetType( TCalRRule::EMonthly );

                RArray<TInt> dateArray;
                CleanupClosePushL( dateArray );
                dateArray.Append( aRRule.DtStart().TimeLocalL().DayNoInMonth() );
                aRRule.SetByMonthDay( dateArray );
                CleanupStack::PopAndDestroy( &dateArray );

                break;
                }
            case (ERepeatYearly):  // Yearly
                {
                // If old note's repeat is every 12 months,
                // set repeat to every 12 months to Agenda, NOT every year.
                //
                if ( hasOrigRRule && origRRule.Type() == TCalRRule::EMonthly &&
                     origRRule.Interval() == 12)
                    {
                    aRRule.SetType( TCalRRule::EMonthly );
                    interval = 12;
                    }
                else
                    {
                    aRRule.SetType( TCalRRule::EYearly );
                    }
                break;
                }
            default: // Repeat item is "Other"
                {
                interval = aRRule.Interval(); // To copy previous note's interval
                break;
                }
            }

        aRRule.SetInterval( interval );
        }

    // If repeat until is max date of Calendar,
    // note repeat is set to forever.
    if ( ! CalenDateUtils::IsValidDay(repeatUntil) || IsForeverDate( repeatUntil ) )
        {
        // Set repeat to forever:
        aRRule.SetCount( 0 );
        }
    else
        {
        // Change repeatuntil so that hours, minutes, ... are same as in start date
        TDateTime dtUntil = repeatUntil.DateTime();
        TDateTime dtStart= iEntry.StartTimeL().TimeLocalL().DateTime();

        dtUntil.Set( dtUntil.Year(), dtUntil.Month(), dtUntil.Day(),
                     dtStart.Hour(), dtStart.Minute(), dtStart.Second(), dtStart.MicroSecond());

        TCalTime calRepeatUntil;
        if ( UseFloatingTimeL() )
            {
            calRepeatUntil.SetTimeLocalFloatingL( TTime( dtUntil ) );
            }
        else
            {
            calRepeatUntil.SetTimeLocalL( TTime( dtUntil ) );
            }
        aRRule.SetUntil( calRepeatUntil );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::SetDefaultNewValuesL
// Sets the default values when creating a new entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorDataHandler::SetDefaultNewValuesL(CCalenEntryUtil& aData)
    {
    TRACE_ENTRY_POINT;

    // FIXME: I don't know if this method should be needed or not.
    // It's a bit unclear that if pass entry that is new,
    // should we set default data in caller (application) or
    // here.
    // But to replicate old editor functionality, we set in editor
    // side

    ASSERT( IsCreatingNew() );
    // start time and end time are taken from passed entry.
    // They have to be initialized outside of editors

    // subject, location, description are by default empty

    // repeat rule is by default off

    // alarm is by default off

    // synch value
    aData.SetSynchTypeL( CCalenEntryUtil::ESynchPrivate );

    // priority
    aData.SetPriorityL( CCalenEntryUtil::ETodoPriorityNormal );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::IsChildL
// Returns ETrue if the entry is a child, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::IsChildL() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iRecurrenceId.TimeUtcL() != Time::NullTTime();
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::UseFloatingTimeL
// Returns ETrue if the entry should use floating time, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorDataHandler::UseFloatingTimeL() const
    {
    TRACE_ENTRY_POINT;

    // Use floating time based on type
    // This way we ensure that entries _created_ with Calendar Editors
    // are consistent regarding time mode.
    //
    TRACE_EXIT_POINT;
    return (! IsTimedEntry()); // use floating time for non-timed entries        
        
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::WriteStartAndEndTimesToEntryL
// Writes the start and end times from the form to the CCalEntry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorDataHandler::WriteStartAndEndTimesToEntryL( CalCommon::TRecurrenceRange aRepeatTarget )
    {
    TRACE_ENTRY_POINT;

    const TBool unDatedTodo=(iEntry.EntryTypeL()==CCalEntry::ETodo) && (iEntry.EndTimeL().TimeLocalL()==Time::NullTTime());
    const TBool unDatedTodoWithAlarm=unDatedTodo && (Edited().IsAlarmActivated());

    TBool shouldWriteBothTimes =
        // For new entries
        IsCreatingNew() ||
        // When creating exception entry (exception entry's time are read from editors)
        ( aRepeatTarget != CalCommon::EThisAndAll ) ||
        // when repeating entry is modified to be non-repeating (entry's times are read from editors)
        ( Original().IsRepeating() && ! Edited().IsRepeating() );

    if ( shouldWriteBothTimes || IsStartDateTimeEdited() || IsEndDateTimeEdited() || unDatedTodoWithAlarm )
        {

        TCalTime entryStart;
        TCalTime entryEnd;

        // 1) Initialize entryStart and entryEnd with original times.
        // But for todos we need to be careful, because there might not be them originally
        // In S60 UI, todos don't have starttime, so we use same for both start and end time.
        if (iEntry.EntryTypeL()==CCalEntry::ETodo)
            {
             // if undated todo and it has alarm activated
            if ((iEntry.EndTimeL().TimeLocalL()==Time::NullTTime()) && (Edited().IsAlarmActivated()))
                {
                entryStart.SetTimeLocalL(Edited().EventDateTime());
                entryEnd.SetTimeLocalL(Edited().EventDateTime());
                shouldWriteBothTimes=ETrue; // Ensures that times are written later
                }
            else
                {
                 entryStart = iEntry.EndTimeL();
                 entryEnd = iEntry.EndTimeL();
                }
            }
        else
            {
            entryStart = iEntry.StartTimeL();
            entryEnd   = iEntry.EndTimeL();
            }

        // 2) Decide start time
        if ( shouldWriteBothTimes || IsStartDateTimeEdited() )
            {
            TTime newStart = Edited().StartDateTime();
            TDateTime newStartDT = newStart.DateTime();
            // Special case:
            // In case c) only start time (hh:mm) of d) existing a) repeating b) series is edited,
            // we need to preserve start date (dd.mm.yyy), because start date in editors
            // is instance's start date.
            // (Example of problematic cases that brings up, when UI represents
            // two different concepts in same start date field)
            //   a) repeating              b) series is edited                        c) only start time is edited     d) existing
            if ( Edited().IsRepeating() && aRepeatTarget == CalCommon::EThisAndAll && ! IsStartDateChanged()        && ! IsCreatingNew() )
                {
                TDateTime oldDt = iEntry.StartTimeL().TimeLocalL().DateTime();
                TDateTime editDt = newStart.DateTime();
                newStart = TTime( TDateTime( oldDt.Year(), oldDt.Month(), oldDt.Day(),
                                             editDt.Hour(), editDt.Minute(),
                                             oldDt.Second(), oldDt.MicroSecond() ) );
                }
            else if ( ( Edited().RepeatType() == ERepeatOther ) && IsStartDateChanged() )
                {
                // The start time might have been disguised because the entry
                // consists of an rRule and rDates. The actual start time
                // needs to be extracted
                RArray<TCalTime> rDates;
                CleanupClosePushL( rDates );
                
                iEntry.GetRDatesL( rDates );
                if ( rDates.Count() > 0 )
                    {
                    // As we have rDates, check to see if there is an
                    // rRule
                    TCalRRule rRule;
                    if ( iEntry.GetRRuleL( rRule ) )
                        {
                        TTime firstRDate = rDates[ 0 ].TimeLocalL();
                        TTime rRuleStart = iEntry.StartTimeL().TimeLocalL();
                        TTime lastRDate = rDates[ rDates.Count() - 1 ].TimeLocalL();
                        TTimeIntervalMinutes startDateShiftInterval = 0;
                        if ( firstRDate < rRuleStart )
                            {
                            // The first rDate is before the start of the rRule
                            // So adjust the start date accordingly
                            
                            User::LeaveIfError( newStart.MinutesFrom( firstRDate, startDateShiftInterval ) );
                            }
                        else
                            {
                            // The start of the rRule comes first
                            User::LeaveIfError( newStart.MinutesFrom( rRuleStart, startDateShiftInterval ) );
                            }
                            
                        newStart = rRuleStart + startDateShiftInterval;
                        
                        TInt rRuleCount = rRule.Count();
                        rRule.SetCount( rRuleCount );
                        
                        TCalTime rRuleDT;
                        if ( UseFloatingTimeL() )
                            {
                            rRuleDT.SetTimeLocalFloatingL( newStart );
                            }
                        else
                            {
                            rRuleDT.SetTimeLocalL( newStart );
                            }
                        
                        rRule.SetDtStart( rRuleDT );
						
						// For rearranging the recurrence pattern for an 
						// entry having rdates
						
						TInt repeatType = rRule.Type();	
       					switch ( repeatType )
            				{
               				case ( TCalRRule::EWeekly ):  //For Weekly repeating event
                				{
                				RArray<TDay> dayArray;
                    			CleanupClosePushL( dayArray );
                    			dayArray.Append( rRule.DtStart().TimeLocalL().DayNoInWeek() );
                    			rRule.SetByDay( dayArray );
                    			CleanupStack::PopAndDestroy( &dayArray );
                    			
                    			// Adjust the RepeatUntil date
                    			ApplyUntilDateToRRuleL( rRule, Edited().RepeatUntilDateTime() );            
								break;
                				}
                            case ( TCalRRule::EMonthly ):// For Monthly repeating event
                                {
				                RArray<TInt> dateArray;
                				CleanupClosePushL( dateArray );
                				dateArray.Append( rRule.DtStart().TimeLocalL().DayNoInMonth() );
                				rRule.SetByMonthDay( dateArray );
                				CleanupStack::PopAndDestroy( &dateArray );
							
								// Adjust the RepeatUntil date
								ApplyUntilDateToRRuleL( rRule, Edited().RepeatUntilDateTime() );            
				                break;
                				}
                				
                			 case ( TCalRRule::EDaily ):// For Daily repeating event
                                {
				                TCalRRule curRule;				
								iEntry.GetRRuleL( curRule );
								
								// Adjust the RepeatUntil date
								if(lastRDate < curRule.Until().TimeLocalL() )
									{
									ApplyUntilDateToRRuleL( rRule, Edited().RepeatUntilDateTime() );  	
									}
								
								
				                break;
                				}
				            default:
                				{
				                break;
                				}
            				}

                        
                        iEntry.SetRRuleL( rRule );
                        }
                    }
                
                CleanupStack::PopAndDestroy(); // rDates
                }

            if ( UseFloatingTimeL() )
                {
                entryStart.SetTimeLocalFloatingL( newStart );
                }
            else
                {
                entryStart.SetTimeLocalL( newStart );
                }
            }

        // 3) Decide end time
        if ( shouldWriteBothTimes || IsEndDateTimeEdited() )
            {
            TTime start = Edited().StartDateTime();
            TTime end = Edited().EndDateTime();
            
            // For all-day events, we add 24 hours to end date
            // to make it full day long
            /*if ( iEntryType == CCalEntry::EEvent)
                {
                end += TTimeIntervalDays(1);
                }*/

            TTimeIntervalMinutes duration;
            TInt err = end.MinutesFrom( start, duration );
            ASSERT( err == KErrNone );
            ASSERT( duration >= TTimeIntervalMinutes(0) );

            if ( UseFloatingTimeL() )
                {
                entryEnd.SetTimeLocalFloatingL( entryStart.TimeLocalL() + duration );
                }
            else
                {
                entryEnd.SetTimeLocalL( entryStart.TimeLocalL() + duration );
                }
            }
        
        // If EntryType is changed as Non-timed/Timed entry, along with StartTime(AND/OR)EndTime,
        // Both Start/End time TimeMode should be changed accordingly.
        if ( UseFloatingTimeL() )
            {
            entryStart.SetTimeLocalFloatingL( entryStart.TimeLocalL() );
            entryEnd.SetTimeLocalFloatingL( entryEnd.TimeLocalL() );
            }
        else
            {
            entryStart.SetTimeLocalL( entryStart.TimeLocalL() );
            entryEnd.SetTimeLocalL( entryEnd.TimeLocalL() );
            }
        
        // Write times to entry
        iEntry.SetStartAndEndTimeL( entryStart, entryEnd );
        }

    TRACE_EXIT_POINT;
    }

//----------------------------------------------------------------------
// CCalenEditorDataHandler::ResetOriginalDataL
// Reset original editor data
// (other items were commented in a header).
//----------------------------------------------------------------------
void CCalenEditorDataHandler::ResetOriginalDataL()
    {
    TRACE_ENTRY_POINT;
		
    if(iOriginal)
		{
		delete iOriginal;
		iOriginal=NULL;
		}
    iOriginal = CCalenEntryUtil::NewL( iEntry, iInstanceDateTime );

    TRACE_EXIT_POINT;
    }

//----------------------------------------------------------------------
// CCalenEditorDataHandler::EntryViewL
// Temp function to get to work 
// (other items were commented in a header).
//----------------------------------------------------------------------
CCalEntryView&  CCalenEditorDataHandler::EntryViewL()
    {
    TRACE_ENTRY_POINT;

    CCalEntryView* entryView = iServices.EntryViewL(CurrentDbCollectionId());

    TRACE_EXIT_POINT;
    return *entryView;
    }

//----------------------------------------------------------------------
// CCalenEditorDataHandler::InstanceViewL
// Temp function to get to work
// (other items were commented in a header).
//----------------------------------------------------------------------
CCalInstanceView&  CCalenEditorDataHandler::InstanceViewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalInstanceView* instanceView = iServices.InstanceViewL(iCollectionIds);
    
    TRACE_EXIT_POINT;
    return *instanceView;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::GetPreviousInstanceForRepeatOtherL()
// Returns the time of the previous instance of a particular repeating entry
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTime CCalenEditorDataHandler::GetPreviousInstanceForRepeatOtherL(CCalEntry& aEntry, const CalCommon::TCalTimeRange& timeRange)
    {
	RPointerArray<CCalInstance> allInstances;
    CleanupResetAndDestroyPushL( allInstances );
    
    TInt filter;
    // Get the entry type to be filtered
    switch(aEntry.EntryTypeL())
        {
    	case CCalEntry::EAppt:
    		filter = CalCommon::EIncludeAppts;
    		break;
    	case CCalEntry::ETodo:
    		filter = CalCommon::EIncludeCompletedTodos | CalCommon::EIncludeIncompletedTodos;
    		break;
    	case CCalEntry::EEvent:
    		filter = CalCommon::EIncludeEvents;
    		break;
    	case CCalEntry::EReminder:
    		filter = CalCommon::EIncludeReminder;
    		break;
    	case CCalEntry::EAnniv:
    		filter = CalCommon::EIncludeAnnivs;
    		break;
    	default:
    		filter = CalCommon::EIncludeAll;
    		break;
        };

    iServices.InstanceViewL()->FindInstanceL( allInstances, 
                                     (CalCommon::TCalViewFilterFlags)filter,
                                     timeRange);

	TTime previousTime = Time::NullTTime();
	
    for( TInt i = allInstances.Count() - 1; i >= 0; i-- )
        {
        if( allInstances[i]->Entry().UidL() == aEntry.UidL() )
            {
            TRACE_EXIT_POINT;
            previousTime = allInstances[i]->Time().TimeLocalL();
            break;
            }
        }

    CleanupStack::PopAndDestroy( &allInstances );  
    return previousTime;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::GetNextInstanceForRepeatOtherL()
// Returns the time of the next instance of a particular repeating entry
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTime CCalenEditorDataHandler::GetNextInstanceForRepeatOtherL( CCalEntry& aEntry, const CalCommon::TCalTimeRange& timeRange )
    {
	RPointerArray<CCalInstance> allInstances;
    CleanupResetAndDestroyPushL( allInstances );
    
    TInt filter;
    // Get the entry type to be filtered
    switch( aEntry.EntryTypeL() )
        {
    	case CCalEntry::EAppt:
    		filter = CalCommon::EIncludeAppts;
    		break;
    	case CCalEntry::ETodo:
    		filter = CalCommon::EIncludeCompletedTodos | CalCommon::EIncludeIncompletedTodos;
    		break;
    	case CCalEntry::EEvent:
    		filter = CalCommon::EIncludeEvents;
    		break;
    	case CCalEntry::EReminder:
    		filter = CalCommon::EIncludeReminder;
    		break;
    	case CCalEntry::EAnniv:
    		filter = CalCommon::EIncludeAnnivs;
    		break;
    	default:
    		filter = CalCommon::EIncludeAll;
    		break;
        };
    
    iServices.InstanceViewL()->FindInstanceL( allInstances, 
                                     ( CalCommon::TCalViewFilterFlags )filter,
                                     timeRange);
                                     
    TTime nextTime = Time::NullTTime();
    
	TInt i( 0 );
    for( ; i < allInstances.Count(); i++ )
        {
        if( allInstances[i]->Entry().UidL() == aEntry.UidL() )
            {
            TRACE_EXIT_POINT;
            nextTime = allInstances[i]->Time().TimeLocalL();
            break;
            }
        }
  
    CleanupStack::PopAndDestroy( &allInstances );  
    return nextTime;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::SetEntryType
// add attachments to the entry as ccalattachments
// -----------------------------------------------------------------------------
//
void CCalenEditorDataHandler::SetEntryType( CCalEntry::TType aNewEntryType )
    {
    iEntryType = aNewEntryType;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::AddAttachementsToEntryL
// add attachments to the entry as ccalattachments
// -----------------------------------------------------------------------------
//
void CCalenEditorDataHandler::AddAttachementsToEntryL()
    {
    TRACE_ENTRY_POINT;
    
    // Get the list of attachments from model and add them to entry
    RPointerArray<CCalenAttachmentInfo> attachmentInfoList;
    iServices.GetAttachmentData()->GetAttachmentListL( attachmentInfoList );
    TInt attachmentCount = iEntry.AttachmentCountL();
    TInt modelCount = attachmentInfoList.Count();
    
    if( attachmentCount > 0 && modelCount == 0 )
        {
        // This means that all the attachments in the entry have been deleted
        // by the user. So remove all attchments from the entry
        for( int index = 0 ; index < attachmentCount ; index++ )
            {
            iEntry.DeleteAttachmentL( *iEntry.AttachmentL( 0 ) );
            }
        
        // No attachments need to be added at this point, we can simply return.
        attachmentInfoList.Close();
        TRACE_EXIT_POINT;
        return;
        }
    
    // Delete all the attachmenst of the entry which are not present in the model
    TInt currentIndex = 0;
    for( TInt index=0; index < attachmentCount; index++ )
       {
       CCalAttachment* attachment = iEntry.AttachmentL( currentIndex );
       TBool matchFound = EFalse;
       for( int modelIndex = 0 ; modelIndex < modelCount ; modelIndex++ )
           {
           CCalenAttachmentInfo* attachmentInfo = attachmentInfoList[ modelIndex ];
           if( attachmentInfo->StoreType() == 
               CCalenAttachmentInfo::ECalenAttachmentFetchedFromEntry )
               {
               // We are only interested in those attachments which have been
               // fetched from the entry because any newly added attachments
               // will not yet be updated in the entry
               RFile attachmentHandle;
               CleanupClosePushL( attachmentHandle );
               attachment->FileAttachment()->FetchFileHandleL( attachmentHandle );
               
               TFileName systemFileName;
               attachmentHandle.FullName( systemFileName );
               
               if( systemFileName == attachmentInfo->SystemFileName() )
                   {
                   // There is an attachment in the model which matches the
                   // attachment in the entry. So do not delete it
                   matchFound = ETrue;
                   CleanupStack::PopAndDestroy( &attachmentHandle );
                   break;
                   }
               CleanupStack::PopAndDestroy( &attachmentHandle );
               }
           }
       if( !matchFound )
           {
           // There is no matching attachment in the model. Delete it from entry
           // This will compress the attachment array. Hence do not increment the currentIndex
           iEntry.DeleteAttachmentL( *iEntry.AttachmentL( currentIndex ) );
           }
       else
           {
           // Move to the next attachment
           currentIndex++;
           }
       }
    
    // Now add the remaining newly added attachments to the entry
    for( TInt index=0 ; index< modelCount ; index++ )
        {
        CCalenAttachmentInfo* attachmentInfo = attachmentInfoList[ index ];
        if( attachmentInfo->StoreType() == CCalenAttachmentInfo::ECalenNewAttachment )
            {
            TParsePtrC fileNameParser(attachmentInfo->FileName());
            RFile fileHandle;
            CEikonEnv* eikonEnv = CEikonEnv::Static();
            RFs& fs = eikonEnv->FsSession();
            User::LeaveIfError(fs.ShareProtected());
            
            TInt err = fileHandle.Open( fs, attachmentInfo->FileName(), EFileWrite );
            if( KErrNone != err )
                {
                attachmentInfoList.Close();
                TRACE_EXIT_POINT;
                return;
                }
            CleanupClosePushL(fileHandle);
            
            CCalAttachment* attachment = CCalAttachment::NewFileL(fileHandle);
            CleanupStack::PopAndDestroy(&fileHandle); 
            
            CleanupStack::PushL( attachment );
            // Sets the label for the attachment
            attachment->SetLabelL( fileNameParser.NameAndExt() );
            // Sets mime type for the attachment
            attachment->SetMimeTypeL( attachmentInfo->DataType().Des8() );
            attachment->SetAttribute(CCalAttachment::EExportInline);
            iEntry.AddAttachmentL( *attachment );
            CleanupStack::Pop( attachment );
            }                
        }
    attachmentInfoList.Close();
   
    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::SetCalendarFieldEditedL
// Set if calendar is been edited or not with previous and current collection id
// -----------------------------------------------------------------------------
//
void CCalenEditorDataHandler::SetCalendarFieldEditedL(TBool aEdited , 
                                                            TCalCollectionId aPreviousDbColId,
                                                            TCalCollectionId aCurrentDbColId)
    {
    TRACE_ENTRY_POINT
    iCalendarFieldEdited = aEdited;
    iPreviousDbColId = aPreviousDbColId;
    iCurrentDbColId = aCurrentDbColId;
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::PreviousDbCollectionId
// Get previous collection id
// -----------------------------------------------------------------------------
//
TCalCollectionId CCalenEditorDataHandler::PreviousDbCollectionId()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return iPreviousDbColId;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::CurrentDbCollectionId
// Get current collection id 
// -----------------------------------------------------------------------------
//
TCalCollectionId CCalenEditorDataHandler::CurrentDbCollectionId()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return iCurrentDbColId;
    }

// -----------------------------------------------------------------------------
// CCalenEditorDataHandler::CalenInstanceId
// Returns the instance id.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TCalenInstanceId CCalenEditorDataHandler::CalenInstanceId()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    
    return iInstanceId;
    }
	
// End of file	--Don't remove this.
