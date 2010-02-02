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
* Description:   CalenEditorData stores data from edited Symbian CCalEntry
*                to format that match Calendar Editors fields. Editor fields
*                update CalenEditorData.
*
*/


//debug
#include "calendarui_debug.h"

#include "CalenEditorData.h"

#include <calendateutils.h>

#include <calalarm.h>
#include <calentry.h>
#include <calrrule.h>
// Utility functions 

// ======== LOCAL FUNCTIONS ======== 

// -----------------------------------------------------------------------------
// ThrowAwaySecondsAndMicroSecondsL
// Remove the seconds and micro seconds part from a TTime.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
static TTime ThrowAwaySecondsAndMicroSecondsL( const TTime& aTime )
    {
    TRACE_ENTRY_POINT;

    TDateTime dt = aTime.DateTime();
    dt.SetSecond(0);
    dt.SetMicroSecond(0);
    TTime result( dt );

    TRACE_EXIT_POINT;
    return result;
    }

// -----------------------------------------------------------------------------
// DurationL
// Returns the difference between the start and end time of an entry.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
static TTimeIntervalMinutes DurationL( const CCalEntry& aEntry )
    {
    TRACE_ENTRY_POINT;

    TTimeIntervalMinutes duration;

    TTime start = aEntry.StartTimeL().TimeLocalL();
    TTime end = aEntry.EndTimeL().TimeLocalL(); 
    end.MinutesFrom( start, duration );

    TRACE_EXIT_POINT;
    return duration;
    }

// -----------------------------------------------------------------------------
// StoreDescriptorL
// Gets reference to target HBufC& and attempts to copy data to it.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
static void StoreDescriptorL(const TDesC& aSrc, HBufC*& aTgt)
    {
    TRACE_ENTRY_POINT;

    // Simplistic implementation
    delete aTgt; // it's safe to delete null pointers
    aTgt = NULL;
    aTgt = aSrc.AllocL();
    // Complex and more efficient implementation
//     if ( aSrc.Length() <= aTgt->Des().MaxLength() )
//         {
//         aTgt->Des().Copy( aSrc );
//         }
//     else
//         {
//         delete aTgt;
//         aTgt = NULL;
//         aTgt = aSrc.AllocL();
//         }

    TRACE_EXIT_POINT;
    }

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CCalenEditorData::NewL
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorData* CCalenEditorData::NewL(CCalEntry& aEntry, 
const TCalTime& aInstanceDateTime )
    {
    TRACE_ENTRY_POINT;

    CCalenEditorData* self = new (ELeave) CCalenEditorData;
    CleanupStack::PushL( self );
    self->ConstructL(aEntry, aInstanceDateTime);
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::~CCalenEditorData
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorData::~CCalenEditorData()
    {
    TRACE_ENTRY_POINT;

    delete iSummary;
    delete iLocation;
    delete iDescription;

    TRACE_EXIT_POINT;
    }
		
// Getters
const CCalEntry::TType& CCalenEditorData::EntryType() const
    {
     TRACE_ENTRY_POINT;
   
     TRACE_EXIT_POINT;
     return iEntryType;   
    }

const CCalEntry::TStatus& CCalenEditorData::Status() const
	{
	TRACE_ENTRY_POINT; 
	
	TRACE_EXIT_POINT;
	return iEntryStatus; 
	}

// -----------------------------------------------------------------------------
// CCalenEditorData::Summary
// Returns the summary.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CCalenEditorData::Summary() const
    {
    TRACE_ENTRY_POINT;

    ASSERT( iSummary );

    TRACE_EXIT_POINT;
    return *iSummary;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::Location
// Returns the location.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CCalenEditorData::Location() const
    {
    TRACE_ENTRY_POINT;

    ASSERT( iLocation );

    TRACE_EXIT_POINT;
    return *iLocation;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::Description
// Returns the description.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CCalenEditorData::Description() const
    {
    TRACE_ENTRY_POINT;

    ASSERT( iDescription );

    TRACE_EXIT_POINT;
    return *iDescription;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::StartDateTime
// Returns the start date time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TTime& CCalenEditorData::StartDateTime() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iStartDateTime;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::EndDateTime
// Returns the end date time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TTime& CCalenEditorData::EndDateTime() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iEndDateTime;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::EventDateTime
// Returns the event date time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TTime& CCalenEditorData::EventDateTime() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return StartDateTime();
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::IsAlarmActivated
// Returns ETrue if the alarm is activated, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorData::IsAlarmActivated() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iIsAlarmActivated;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::AlarmDateTime
// Returns the alarm date time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TTime& CCalenEditorData::AlarmDateTime() const
    {
    TRACE_ENTRY_POINT;

    ASSERT( iIsAlarmActivated );

    TRACE_EXIT_POINT;
    return iAlarmDateTime;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::RepeatType
// Returns the repeat type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TCalenRepeatIndex CCalenEditorData::RepeatType() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iRepeatType;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::RepeatUntilDateTime
// Returns the date/time until which this entry repeats.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TTime& CCalenEditorData::RepeatUntilDateTime() const
    {
    TRACE_ENTRY_POINT;

    ASSERT( IsRepeating() );

    TRACE_EXIT_POINT;
    return iRepeatUntilDateTime;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::Priority
// Returns the priority.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorData::TTodoPriority CCalenEditorData::Priority() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iPriority;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SynchType
// Returns the synchronisation type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorData::TSynchType CCalenEditorData::SynchType() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iSynchType;
    }

// Setters

// -----------------------------------------------------------------------------
// CCalenEditorData::SetSummaryL
// Sets the summary.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetSummaryL( const TDesC& aSummary )
    {
    TRACE_ENTRY_POINT;

    StoreDescriptorL( aSummary, iSummary );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetLocationL
// Sets the location.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetLocationL( const TDesC& aLocation )
    {
    TRACE_ENTRY_POINT;

    StoreDescriptorL( aLocation, iLocation );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetDescriptionL
// Sets the description.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetDescriptionL( const TDesC& aDescription )
    {
    TRACE_ENTRY_POINT;

    StoreDescriptorL( aDescription, iDescription );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetDescriptionTransferOwnershipL
// Sets the description, passing ownership of aDescription.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetDescriptionTransferOwnershipL( HBufC* aDescription )
    {
    TRACE_ENTRY_POINT;

    delete iDescription;
    iDescription = aDescription;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetStartDateTimeL
// Sets the start date/time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetStartDateTimeL( const TTime& aStart )
    {
    TRACE_ENTRY_POINT;

    iStartDateTime = ThrowAwaySecondsAndMicroSecondsL( aStart );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetEndDateTimeL
// Sets the end date/time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetEndDateTimeL( const TTime& aEnd )
    {
    TRACE_ENTRY_POINT;

    iEndDateTime = ThrowAwaySecondsAndMicroSecondsL( aEnd );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetEventDateL
// Sets the event date.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetEventDateL( const TTime& aDate)
    {
    TRACE_ENTRY_POINT;

    TTime midnight = CalenDateUtils::BeginningOfDay( aDate );
    SetStartDateTimeL( midnight );
    SetEndDateTimeL( midnight );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetAlarmOnL
// Sets the alarm to be active at the given date/time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetAlarmOnL( const TTime& aAlarmTime )
    {
    TRACE_ENTRY_POINT;

    iIsAlarmActivated = ETrue;
    iAlarmDateTime = ThrowAwaySecondsAndMicroSecondsL( aAlarmTime );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetAlarmOffL
// Turns off the alarm.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetAlarmOffL()
    {
    TRACE_ENTRY_POINT;

    iIsAlarmActivated = EFalse;
    iAlarmDateTime = Time::NullTTime();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetAlarmDateTimeL
// Sets the alarm to be at the given time. Note that you must call SetAlarmOnL()
// before calling this function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetAlarmDateTimeL( const TTime& aTime )
    {
    TRACE_ENTRY_POINT;

    __ASSERT_ALWAYS( iIsAlarmActivated, User::Leave( KErrNotReady ) );
    iAlarmDateTime = ThrowAwaySecondsAndMicroSecondsL( aTime );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetNonRepeatingL
// Sets the event to be non-repeating.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetNonRepeatingL()
    {
    TRACE_ENTRY_POINT;

    iRepeatType = ERepeatNotRepeated; 
    iRepeatUntilDateTime = Time::NullTTime();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetRepeatingL
// Sets the event to be repeating, based on repeat type and until date/time
// passed in.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetRepeatingL(TCalenRepeatIndex aRepeatType, const TTime& aUntilDateTime )
    {
    TRACE_ENTRY_POINT;

    iRepeatType = aRepeatType;
    iRepeatUntilDateTime = CalenDateUtils::BeginningOfDay( aUntilDateTime );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetRepeatUntilDateTimeL
// Sets the event to repeat until the given date/time. Note that you must call
// SetRepeatingL before calling this function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetRepeatUntilDateTimeL(const TTime& aTime)
    {
    TRACE_ENTRY_POINT;

    __ASSERT_ALWAYS( IsRepeating(), User::Leave( KErrNotReady ) );
    SetRepeatingL( iRepeatType, aTime );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetPriorityL
// Sets the priority.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetPriorityL(CCalenEditorData::TTodoPriority aPriority)
    {
    TRACE_ENTRY_POINT;

    iPriority = aPriority;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::SetSynchTypeL
// Sets the sychronisation type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::SetSynchTypeL(CCalenEditorData::TSynchType aSynchType)
    {
    TRACE_ENTRY_POINT;

    iSynchType = aSynchType;

    TRACE_EXIT_POINT;
    }

// Utility functions 

// -----------------------------------------------------------------------------
// CCalenEditorData::IsRepeating
// Returns ETrue if the entry is repeating, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CCalenEditorData::IsRepeating() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iRepeatType != ERepeatNotRepeated; 
    }

// Private functions 

// -----------------------------------------------------------------------------
// CCalenEditorData::CCalenEditorData
// Constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEditorData::CCalenEditorData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::ConstructL
// Leaving construction.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::ConstructL( CCalEntry& aEntry, 
                                   const TCalTime& aInstanceDateTime )
    {
    TRACE_ENTRY_POINT;
    iEntryType = aEntry.EntryTypeL();
    iEntryStatus = aEntry.StatusL();
    CopyDataFromEntryL( aEntry, aInstanceDateTime );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEditorData::CopyDataFromEntryL
// Copy data from the given entry to this class.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEditorData::CopyDataFromEntryL( CCalEntry& aEntry, const TCalTime& aInstanceDateTime )
    {
    TRACE_ENTRY_POINT;
    
    SetSummaryL( aEntry.SummaryL() );
    SetLocationL( aEntry.LocationL() );
    SetDescriptionL( aEntry.DescriptionL() );

    switch ( aEntry.EntryTypeL() )
        {
        case CCalEntry::EAppt:
            {
            TTime start = aInstanceDateTime.TimeLocalL();
            TTime end = start + DurationL( aEntry );
            SetStartDateTimeL( start );
            SetEndDateTimeL( end );
            }
            break;

        case CCalEntry::EReminder:
            {
            TTime start = aInstanceDateTime.TimeLocalL();
            TTime end = start;
            SetStartDateTimeL( start );
            SetEndDateTimeL( end );
            }
            break;

        case CCalEntry::EEvent:
            {
            TTime start = CalenDateUtils::BeginningOfDay( aInstanceDateTime.TimeLocalL() );
            TTime end = start + DurationL( aEntry );
            end -= TTimeIntervalDays(1);
            end = CalenDateUtils::BeginningOfDay( end );
            // ensure that for all-day events shorter than day we use sensible value
            if (end < start) 
                {
                end = start;
                }
            SetStartDateTimeL( start );
            SetEndDateTimeL( end );
            }
            break;

        case CCalEntry::EAnniv:
            {
            TTime event = aInstanceDateTime.TimeLocalL();
            SetEventDateL( event );
            }
            break;

        case CCalEntry::ETodo:
            {
            // Undated todos
            TTime dueDate = aEntry.EndTimeL().TimeLocalL();
            if ( dueDate == Time::NullTTime() )
                {
                TTime today;
                today.HomeTime();
                today = CalenDateUtils::BeginningOfDay( today ); 
                dueDate = today;
                }
            SetEventDateL( dueDate );
            }
            break;

        default:
            ASSERT( EFalse );
            break;
        }

    // Repeat rule 
    TCalRRule rrule;
    TBool isRepeating = aEntry.GetRRuleL( rrule );

    RArray<TCalTime> rdates;
    CleanupClosePushL( rdates );
    aEntry.GetRDatesL( rdates );
    TBool hasRDates = ( rdates.Count() > 0 );
 
    // If the entry has rdates and/or got rrules,
    // need to find and set the repeat until date
    if ( hasRDates || isRepeating )
        {
        TTime repeatUntil = aEntry.StartTimeL().TimeLocalL();

        if ( isRepeating )
            {
            // If the entry is repeating find the rrule 
            // repeat until time.
            TTime rruleUntil = rrule.Until().TimeLocalL();
            if ( rruleUntil > repeatUntil )
                {
                repeatUntil = rruleUntil;
                }
            }

        if ( hasRDates )
            {
            // Find the last rdate date, using the fact that
            // the rdates are sorted in date order
            TTime lastRDate = rdates [ rdates.Count() - 1 ].TimeLocalL();
            
            // Compare the date of the last rdate with 
            // the current repat until time.
            if ( lastRDate > repeatUntil )
                {
                repeatUntil = lastRDate;
                }

            // Set the repeat type as other.
            SetRepeatingL( ERepeatOther, repeatUntil  );

            // If the first rDate is before the start of any rRule,
            // the first rDate will have to be disguised as 
            // the start of the series
            TTime firstRDate = rdates[ 0 ].TimeLocalL();
            if ( firstRDate <  StartDateTime() )
                {
                TTimeIntervalMinutes duration;

                User::LeaveIfError( EndDateTime().MinutesFrom( StartDateTime(), duration ) );
                SetStartDateTimeL( firstRDate );
                TTime endDate = firstRDate + duration;
                SetEndDateTimeL( endDate );
                }
            }
        else 
            {
            // Find the repeat type and set the repeat type.
            TCalenRepeatIndex repeatType = CalenNoteDataUtil::RepeatIndexL( aEntry );
            ASSERT( repeatType != ERepeatNotRepeated );

            SetRepeatingL( repeatType, repeatUntil  );
            }
        }
    else
        {
        SetNonRepeatingL();
        }

    CleanupStack::PopAndDestroy(); // rdates

    // Alarm
    TTime alarmDateTime;
    TBool hasAlarm = CalenNoteDataUtil::GetAlarmDateTimeL( aEntry, 
                                                          alarmDateTime );

    if ( hasAlarm ) 
        {
        if ( isRepeating )
            {
            // alarm time should be offset from instance date, not entry date.
            // As instance date time might have different time of day (repeating entries defined by RDATEs) 
            // than entry start, we need to nudge only days to instance date, and keep time of alarm same. 
            TTimeIntervalDays deltaDays = aInstanceDateTime.TimeLocalL().DaysFrom( aEntry.StartTimeL().TimeLocalL() );
            alarmDateTime += deltaDays;
            }
        SetAlarmOnL( alarmDateTime );
        }
    else
        {
        SetAlarmOffL();
        }

    // Priority - same behaviour as todo/day views.
    TUint priority( aEntry.PriorityL() );

    // Keep priority inside enumeration range
    TTodoPriority todoPriority;
    if( !priority )
        {
        todoPriority = ETodoPriorityHigh;
        }
    else if( priority > ETodoPriorityLow )
        {   
        todoPriority = ETodoPriorityLow;
        }
    else
        {
        todoPriority = TTodoPriority(priority);
        }

    SetPriorityL( todoPriority );

    // Synch type
    // Note: EOpen is Symbian default for new entries
    TSynchType synchType = CCalenEditorData::ESynchPrivate;
    switch ( aEntry.ReplicationStatusL() )
        {
        case CCalEntry::EOpen:
            synchType = CCalenEditorData::ESynchPublic;
            break;
        case CCalEntry::EPrivate:
            synchType = CCalenEditorData::ESynchPrivate;
            break;
        case CCalEntry::ERestricted:
            synchType = CCalenEditorData::ESynchNoSync;
            break;
        default:
            __ASSERT_ALWAYS( EFalse, User::Invariant() );
            synchType = CCalenEditorData::ESynchPrivate; // to please compiler; 
            break;
        }
    SetSynchTypeL( synchType );

    TRACE_EXIT_POINT;
    }

// End of file
