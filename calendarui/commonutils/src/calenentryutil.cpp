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
* Description:   CalenEntryUtil stores data from edited Symbian CCalEntry
*                to format that match Calendar Editors fields. Editor fields
*                update CalenEntryUtil.
*
*/



//debug
#include "calendarui_debug.h"

#include "calenentryutil.h"

#include <calendateutils.h>

#include <calalarm.h>
#include <calentry.h>
#include <calrrule.h>
#include <calattachment.h>
#include <bldvariant.hrh> // for feature definitions
#include <featmgr.h>


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
// CCalenEntryUtil::NewL
// Two-phased constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenEntryUtil* CCalenEntryUtil::NewL(CCalEntry& aEntry, 
const TCalTime& aInstanceDateTime )
    {
    TRACE_ENTRY_POINT;

    CCalenEntryUtil* self = new (ELeave) CCalenEntryUtil(aEntry);
    CleanupStack::PushL( self );
    self->ConstructL(aEntry, aInstanceDateTime);
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::~CCalenEntryUtil
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEntryUtil::~CCalenEntryUtil()
    {
    TRACE_ENTRY_POINT;

    delete iSummary;
    delete iLocation;
    delete iDescription;
    
    // Do not call UnInitializeLib() if InitalizeLib() leaves.
    if ( iFeatMgrInitialized )
        {
        // Frees the TLS. Must be done after FeatureManager is used.
        FeatureManager::UnInitializeLib();  
        }  
    


    TRACE_EXIT_POINT;
    }
		
// Getters
EXPORT_C const CCalEntry::TType& CCalenEntryUtil::EntryType() const
    {
     TRACE_ENTRY_POINT;
   
     TRACE_EXIT_POINT;
     return iEntryType;   
    }

EXPORT_C const CCalEntry::TStatus& CCalenEntryUtil::Status() const
	{
	TRACE_ENTRY_POINT; 
	
	TRACE_EXIT_POINT;
	return iEntryStatus; 
	}

// -----------------------------------------------------------------------------
// CCalenEntryUtil::Summary
// Returns the summary.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCalenEntryUtil::Summary() const
    {
    TRACE_ENTRY_POINT;

    ASSERT( iSummary );

    TRACE_EXIT_POINT;
    return *iSummary;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::Location
// Returns the location.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCalenEntryUtil::Location() const
    {
    TRACE_ENTRY_POINT;

    ASSERT( iLocation );

    TRACE_EXIT_POINT;
    return *iLocation;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::Description
// Returns the description.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCalenEntryUtil::Description() const
    {
    TRACE_ENTRY_POINT;

    ASSERT( iDescription );

    TRACE_EXIT_POINT;
    return *iDescription;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::StartDateTime
// Returns the start date time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& CCalenEntryUtil::StartDateTime() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iStartDateTime;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::EndDateTime
// Returns the end date time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& CCalenEntryUtil::EndDateTime() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iEndDateTime;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::EventDateTime
// Returns the event date time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& CCalenEntryUtil::EventDateTime() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return StartDateTime();
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::IsAlarmActivated
// Returns ETrue if the alarm is activated, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenEntryUtil::IsAlarmActivated() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iIsAlarmActivated;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::IsAllDayEvent
// Returns ETrue if meeting is AllDay event, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenEntryUtil::IsAllDayEvent()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return iIsAllDayEvent;
    }


// -----------------------------------------------------------------------------
// CCalenEntryUtil::AlarmDateTime
// Returns the alarm date time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& CCalenEntryUtil::AlarmDateTime() const
    {
    TRACE_ENTRY_POINT;

    ASSERT( iIsAlarmActivated );

    TRACE_EXIT_POINT;
    return iAlarmDateTime;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::RepeatType
// Returns the repeat type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TCalenRepeatIndex CCalenEntryUtil::RepeatType() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iRepeatType;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::RepeatUntilDateTime
// Returns the date/time until which this entry repeats.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C const TTime& CCalenEntryUtil::RepeatUntilDateTime() const
    {
    TRACE_ENTRY_POINT;

    ASSERT( IsRepeating() );

    TRACE_EXIT_POINT;
    return iRepeatUntilDateTime;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::Priority
// Returns the priority.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenEntryUtil::TTodoPriority CCalenEntryUtil::Priority() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iPriority;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SynchType
// Returns the synchronisation type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenEntryUtil::TSynchType CCalenEntryUtil::SynchType() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iSynchType;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::AttachmentCount
// Returns the attachment's count
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CCalenEntryUtil::AttachmentCount() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iAttachmentCount;
    }

// Setters

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetSummaryL
// Sets the summary.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetSummaryL( const TDesC& aSummary )
    {
    TRACE_ENTRY_POINT;

    StoreDescriptorL( aSummary, iSummary );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetLocationL
// Sets the location.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetLocationL( const TDesC& aLocation )
    {
    TRACE_ENTRY_POINT;

    StoreDescriptorL( aLocation, iLocation );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetDescriptionL
// Sets the description.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetDescriptionL( const TDesC& aDescription )
    {
    TRACE_ENTRY_POINT;

    StoreDescriptorL( aDescription, iDescription );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetDescriptionTransferOwnershipL
// Sets the description, passing ownership of aDescription.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetDescriptionTransferOwnershipL( HBufC* aDescription )
    {
    TRACE_ENTRY_POINT;

    delete iDescription;
    iDescription = aDescription;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetStartDateTimeL
// Sets the start date/time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetStartDateTimeL( const TTime& aStart )
    {
    TRACE_ENTRY_POINT;

    iStartDateTime = ThrowAwaySecondsAndMicroSecondsL( aStart );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetEndDateTimeL
// Sets the end date/time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetEndDateTimeL( const TTime& aEnd )
    {
    TRACE_ENTRY_POINT;

    iEndDateTime = ThrowAwaySecondsAndMicroSecondsL( aEnd );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetEventDateL
// Sets the event date.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetEventDateL( const TTime& aDate)
    {
    TRACE_ENTRY_POINT;

    TTime midnight = CalenDateUtils::BeginningOfDay( aDate );
    SetStartDateTimeL( midnight );
    SetEndDateTimeL( midnight );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetAlarmOnL
// Sets the alarm to be active at the given date/time.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetAlarmOnL( const TTime& aAlarmTime )
    {
    TRACE_ENTRY_POINT;

    iIsAlarmActivated = ETrue;
    iAlarmDateTime = ThrowAwaySecondsAndMicroSecondsL( aAlarmTime );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetAlarmOffL
// Turns off the alarm.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetAlarmOffL()
    {
    TRACE_ENTRY_POINT;

    iIsAlarmActivated = EFalse;
    iAlarmDateTime = Time::NullTTime();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetAlarmDateTimeL
// Sets the alarm to be at the given time. Note that you must call SetAlarmOnL()
// before calling this function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetAlarmDateTimeL( const TTime& aTime )
    {
    TRACE_ENTRY_POINT;

    __ASSERT_ALWAYS( iIsAlarmActivated, User::Leave( KErrNotReady ) );
    iAlarmDateTime = ThrowAwaySecondsAndMicroSecondsL( aTime );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetNonRepeatingL
// Sets the event to be non-repeating.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetNonRepeatingL()
    {
    TRACE_ENTRY_POINT;

    iRepeatType = ERepeatNotRepeated; 
    iRepeatUntilDateTime = Time::NullTTime();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetRepeatingL
// Sets the event to be repeating, based on repeat type and until date/time
// passed in.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetRepeatingL(TCalenRepeatIndex aRepeatType, const TTime& aUntilDateTime )
    {
    TRACE_ENTRY_POINT;

    iRepeatType = aRepeatType;
    iRepeatUntilDateTime = CalenDateUtils::LimitToValidTime(
                                   CalenDateUtils::BeginningOfDay( aUntilDateTime ));
    

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetRepeatUntilDateTimeL
// Sets the event to repeat until the given date/time. Note that you must call
// SetRepeatingL before calling this function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetRepeatUntilDateTimeL(const TTime& aTime)
    {
    TRACE_ENTRY_POINT;

    __ASSERT_ALWAYS( IsRepeating(), User::Leave( KErrNotReady ) );
    SetRepeatingL( iRepeatType, aTime );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetPriorityL
// Sets the priority.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetPriorityL(CCalenEntryUtil::TTodoPriority aPriority)
    {
    TRACE_ENTRY_POINT;

    iPriority = aPriority;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetSynchTypeL
// Sets the sychronisation type.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetSynchTypeL(CCalenEntryUtil::TSynchType aSynchType)
    {
    TRACE_ENTRY_POINT;

    iSynchType = aSynchType;

    TRACE_EXIT_POINT;
    }
	
// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetEntryType
// Sets the unsigned user int.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetEntryType( CCalEntry::TType aEntryType )
    {
    TRACE_ENTRY_POINT;

    iEntryType = aEntryType;

    TRACE_EXIT_POINT;
 
    }
// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetUserIntL
// Sets the attachments count
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetAttachmentCount(TInt aAttachmentCount)
    {
    TRACE_ENTRY_POINT;
    
    iAttachmentCount = aAttachmentCount;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::SetAllDayEvent
// Sets the unsigned user int.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetAllDayEvent(TBool aAllDayEvent )
    {
    TRACE_ENTRY_POINT;
    
    iIsAllDayEvent = aAllDayEvent;
    
    TRACE_EXIT_POINT;
    }
// Utility functions 

// -----------------------------------------------------------------------------
// CCalenEntryUtil::IsRepeating
// Returns ETrue if the entry is repeating, EFalse otherwise.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenEntryUtil::IsRepeating() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iRepeatType != ERepeatNotRepeated; 
    }

// Private functions 

// -----------------------------------------------------------------------------
// CCalenEntryUtil::CCalenEntryUtil
// Constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenEntryUtil::CCalenEntryUtil(CCalEntry& aEntry)
: iEntry(aEntry),iAttachmentCount(0)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::ConstructL
// Leaving construction.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEntryUtil::ConstructL( CCalEntry& aEntry, 
                                   const TCalTime& aInstanceDateTime )
    {
    TRACE_ENTRY_POINT;
    // Sets up TLS, must be done before FeatureManager is used.
    FeatureManager::InitializeLibL();
    // Used in destructor. 
    iFeatMgrInitialized = ETrue;
     

    iEntryType = aEntry.EntryTypeL();
    iEntryStatus = aEntry.StatusL();
    CopyDataFromEntryL( aEntry, aInstanceDateTime );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::CopyDataFromEntryL
// Copy data from the given entry to this class.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenEntryUtil::CopyDataFromEntryL( CCalEntry& aEntry, const TCalTime& aInstanceDateTime )
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
            
            // Appt. is AllDay Event
            SetAllDayEvent( CheckForAlldayEventL( start,end ) );
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
            // similar to reminder
            TTime start = aInstanceDateTime.TimeLocalL();
            TTime end = start + DurationL( aEntry );
            SetStartDateTimeL( start );
            SetEndDateTimeL( end );
            // event. is AllDay Event
            SetAllDayEvent( CheckForAlldayEventL( start,end ) );
            }
            break;

        case CCalEntry::EAnniv:
            {
            TTime event = aInstanceDateTime.TimeLocalL();
            SetEventDateL( event );
            if ( FeatureManager::FeatureSupported( KFeatureIdKorean ) )
                {
				SetCalendarTypeL( ( TLunarCalendarType )aEntry.UserInt32L() );
                }
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
    TInt count = rdates.Count();
    TBool hasRDates = ( count > 0 );
    TTime firstRDate;
    
    // Get the firstRDate only if the entry has RDates.
    if( hasRDates )
    	{
    	firstRDate = rdates[ 0 ].TimeLocalL();
    	}
 	
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
            if ( firstRDate <= StartDateTime() )
                {
                // Only if I am editing the ENTRY, i need the first instance date.
                // Also if the entry has an RDate before the first instance date,
                // make the StartDay as the first RDate.
                if( iRecurrenceType == CalCommon::EThisAndAll )
                	{
	                TTimeIntervalMinutes duration;
	                
	                User::LeaveIfError( EndDateTime().MinutesFrom( StartDateTime(), duration ) );
	                SetStartDateTimeL( firstRDate );
	                TTime endDate = firstRDate + duration;
	                SetEndDateTimeL( endDate );
                	}
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
        
    // Alarm
    TTime alarmDateTime;
    TBool hasAlarm = CalenNoteDataUtil::GetAlarmDateTimeL( aEntry, 
                                                          alarmDateTime );

    if ( hasAlarm ) 
        {
        TTimeIntervalDays deltaDays;
        TTimeIntervalDays alarmOffset = alarmDateTime.DaysFrom( aEntry.StartTimeL().TimeLocalL() );
        TTimeIntervalDays alarmDeltaDays;
        TTimeIntervalDays instanceDeltaDays;
        
        // alarm time should be offset from instance date, not entry date.
        // As instance date time might have different time of day (repeating entries defined by RDATEs) 
        // than entry start, we need to nudge only days to instance date, and keep time of alarm same. 
        if ( isRepeating )
            {
            if( hasRDates )
            	{
            	if( (CalCommon::EThisAndAll == iRecurrenceType) && (firstRDate <= ( aEntry.StartTimeL().TimeLocalL()) ) )
            		{
                	// User editing the series and there are RDates and the firstRDate is less than the 
                	// entry start date, the alarmtime for the first entry date (firstRDate) is just the
                	// offset.
            		instanceDeltaDays = firstRDate.DaysFrom( aEntry.StartTimeL().TimeLocalL() );
            		deltaDays = instanceDeltaDays;
            		alarmDateTime += deltaDays;
            		}
            	else 
            		{
            		// If i am viewing or editing only an instance, the alarm date and time
		            // should be with respect to the instance only. This could also be the case where
		            // there are no RDates before the start date and the user could be editing the series
		            // in that case.
            		alarmDeltaDays = alarmDateTime.DaysFrom( aInstanceDateTime.TimeLocalL() );
            		            		
            		deltaDays = alarmDeltaDays.Int() - alarmOffset.Int();
            		alarmDateTime -= deltaDays;
            		}
            	}
            else // If there are no RDates, the behaviour is obvious.
            	{
	            deltaDays = aInstanceDateTime.TimeLocalL().DaysFrom( aEntry.StartTimeL().TimeLocalL() );
	            alarmDateTime += deltaDays;
            	}
            }
        
        // For an entry which is not repeating but has RDates
        else if( (iRepeatType == ERepeatOther) && hasRDates )
        	{
        	// It doesn't matter if the user is viewing or editing the non-RDate
        	// instance. This is the default case. But when the user is viewing or 
        	// editing a RDate instance, the alarmDateTime has to be adjusted.
        	for( TInt index = 0; index < count; index++ )
        		{
        		// If the instance matches one of the RDates
        		if( (aInstanceDateTime.TimeLocalL()) == (rdates[ index ].TimeLocalL()) )
                    {
                    // instanceAlarmDay = actualAlarmDay + (alarmOffsetStart - alarmOffsetInstance)
                    deltaDays =  alarmDateTime.DaysFrom( aEntry.StartTimeL().TimeLocalL() ).Int()
                    - alarmDateTime.DaysFrom( aInstanceDateTime.TimeLocalL() ).Int();
                    alarmDateTime += deltaDays;
                    if ( FeatureManager::FeatureSupported( KFeatureIdKorean ) )
                        {
                        if ( aEntry.EntryTypeL() == CCalEntry::EAnniv && CalendarType() != ESolar )
                            {
                            alarmDateTime = aEntry.StartTimeL().TimeLocalL(); 
                            CCalAlarm* alarm = aEntry.AlarmL();
                            if(alarm)
                                {
                                alarmDateTime = aInstanceDateTime.TimeLocalL() - alarm->TimeOffset();
                                }
                            }
                        }
                    break;
                    }
        		}
        	}        	
        SetAlarmOnL( alarmDateTime );
        }
    else
        {
        SetAlarmOffL();
        }
	CleanupStack::PopAndDestroy(); // rdates
	
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
    TSynchType synchType = CCalenEntryUtil::ESynchPrivate;
    switch ( aEntry.ReplicationStatusL() )
        {
        case CCalEntry::EOpen:
            synchType = CCalenEntryUtil::ESynchPublic;
            break;
        case CCalEntry::EPrivate:
            synchType = CCalenEntryUtil::ESynchPrivate;
            break;
        case CCalEntry::ERestricted:
            synchType = CCalenEntryUtil::ESynchNoSync;
            break;
        default:
            __ASSERT_ALWAYS( EFalse, User::Invariant() );
            synchType = CCalenEntryUtil::ESynchPrivate; // to please compiler; 
            break;
        }
    SetSynchTypeL( synchType );
    SetAttachmentCount(aEntry.AttachmentCountL());
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEntryUtil::AttachmentNamesL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::AttachmentNamesL(RPointerArray<HBufC>& aAttachmentNames)
    {
    TRACE_ENTRY_POINT;
    TInt attachmentCount = iEntry.AttachmentCountL();
    CCalAttachment* attachment;
    TInt attachmentLength(0);
    for(TInt index=0;index<attachmentCount;index++)
        {
        attachment = iEntry.AttachmentL(index);
        attachmentLength = attachment->Label().Length();
        HBufC* attachmentName = HBufC::NewL(attachmentLength);
        attachmentName->Des().Copy(attachment->Label());
        aAttachmentNames.Append(attachmentName);
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CalenViewUtils::CheckForAlldayEventL
// Allday event is an event  with a duration of n*24h.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCalenEntryUtil::CheckForAlldayEventL( TTime aStartTime, TTime aStopTime )
    {
    TRACE_ENTRY_POINT;

    TBool allDayEvent(EFalse);
    
    TTimeIntervalDays differenceInTime = aStopTime.DaysFrom(aStartTime); // fix for AllDayEntry issue

    if( aStartTime == CalenDateUtils::BeginningOfDay( aStartTime ) 
            && aStopTime == CalenDateUtils::BeginningOfDay( aStopTime ) 
            && aStartTime != aStopTime && differenceInTime.Int() >= 1) // fix for AllDayEntry issue
        {
        allDayEvent = ETrue;
        }
    
    TRACE_EXIT_POINT;
    return allDayEvent;
    }

// ---------------------------------------------------------------------------
// CalenViewUtils::GetRdatesL
// Get the RDates for the current CCalEntry
// ---------------------------------------------------------------------------
//
EXPORT_C const RArray<TCalTime> CCalenEntryUtil::GetRdatesL()
    {
    TRACE_ENTRY_POINT;
    
    RArray<TCalTime> rDateList;
    iEntry.GetRDatesL(rDateList);
    return rDateList;
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CalenViewUtils::CalendarType
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
EXPORT_C TLunarCalendarType CCalenEntryUtil::CalendarType() const
    { 
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;

    return iCalendarType;
    }

// ---------------------------------------------------------------------------
// CalenViewUtils::SetCalendarTypeL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
EXPORT_C void CCalenEntryUtil::SetCalendarTypeL(TLunarCalendarType aCalendarType)
    {
	TRACE_ENTRY_POINT;

    iCalendarType = aCalendarType;
	TRACE_EXIT_POINT;

    }

// End of file
