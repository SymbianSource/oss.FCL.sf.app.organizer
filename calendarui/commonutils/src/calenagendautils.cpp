/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Static utility functions. 
 *                - date utils to help comparisions and calculations with
 *                  dates and times. 
 *
*/



#include <calinstance.h>
#include <calinstanceview.h>
#include <calrrule.h>
#include <calenagendautils.h>
#include <calendateutils.h>

//debug
#include "calendarui_debug.h"
#include "vstaticutils.h"

// ============================ MEMBER FUNCTIONS ==============================
// ============================ CalenAgendaUtils ==============================

// -----------------------------------------------------------------------------
// CalenAgendaUtils::FindEventsForDayRangeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CalenAgendaUtils::FindEventsForDayRangeL( CCalInstanceView* aInstanceView, 
                                                        RPointerArray<CCalInstance>& aList, 
                                                        const CalCommon::TCalViewFilter& aFilter, 
                                                        const TTime& aStartDay,
                                                        const TTime& aEndDay )
    {
    TRACE_ENTRY_POINT;
    
    TCalTime dummy;
    CalCommon::TCalTimeRange dayRange( dummy, dummy );

    CalenDateUtils::GetDayRangeL( aStartDay, aEndDay, dayRange );

    if(CalenDateUtils::IsValidDay(aStartDay))
    {
        aInstanceView->FindInstanceL( aList, aFilter, dayRange );    
    }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CalenAgendaUtils::FindTodosForDayRangeL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CalenAgendaUtils::FindTodosForDayRangeL( CCalInstanceView* aInstanceView, 
                                                       RPointerArray<CCalInstance>& aList, 
                                                       const TTime& aStartDay,
                                                       const TTime& aEndDay )
    {
    TRACE_ENTRY_POINT;
    
    const TCalTime dummy;
    CalCommon::TCalTimeRange dayRange( dummy, dummy );
    CalenDateUtils::GetDayRangeL( aStartDay, aEndDay, dayRange );

    TTime today;  today.HomeTime();
    const TTime tomorrow( today + TTimeIntervalDays(1) );

    const TTime rangeStart( dayRange.StartTime().TimeLocalL() );  // at 00:00.00
    const TTime rangeEnd( dayRange.EndTime().TimeLocalL() ); // at 23:59.59

    // fetch past uncompleted to-dos for today if today is included in the range
    if( rangeStart <= today && today <= rangeEnd )
        {
        // today includes today's to-dos + all past incompleted todos
        CalenDateUtils::GetDayRangeL( TCalTime::MinTime(), today, dayRange );

        if(CalenDateUtils::IsValidDay(aStartDay))
        {
        aInstanceView->FindInstanceL( aList, 
                                      CalCommon::EIncludeIncompletedTodos | 
                                      // only fetch the first instance for repeating to-dos!
                                      CalCommon::EIncludeRptsNextInstanceOnly, 
                                      dayRange );
        }
        // reset the time range for the remaining days
        CalenDateUtils::GetDayRangeL( tomorrow, aEndDay, dayRange );
        }

    // today already fetched, fetch the remaining range if any...
    // (including all the instances of repeating to-dos)
    if( rangeEnd > tomorrow )
        {
        aInstanceView->FindInstanceL( aList, 
                                      CalCommon::EIncludeIncompletedTodos, 
                                      dayRange );
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CalenAgendaUtils::RemoveEntriesStartingAtMidnight
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CalenAgendaUtils::RemoveEntriesEndingAtMidnightL( RPointerArray<CCalInstance>& aList, 
                                                                const TTime& aDay )
    {
    TRACE_ENTRY_POINT;

    // Remove events starting before aDay and ending at midnight
    TInt i( 0 );

    while( i < aList.Count() )
        {
        CCalInstance* item = aList[i];

        if( EndsAtStartOfDayL( item, aDay ) )
            {
            aList.Remove( i );
            delete item; // remember to delete before we lose the pointer
            }
        else
            {
            ++i;
            }
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CalenAgendaUtils::CreateEntryIdListForDayL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CalenAgendaUtils::CreateEntryIdListForDayL( RPointerArray<CCalInstance>& aList, 
                                                          CCalInstanceView* aInstanceView, 
                                                          const TTime& aDay, 
                                                          const TBool aSortForPopup, 
                                                          const TBool aIncludeToDos )
    {
    TRACE_ENTRY_POINT;
    
    const CalCommon::TCalViewFilter filter = CalCommon::EIncludeAppts    | 
                                             CalCommon::EIncludeReminder | 
                                             CalCommon::EIncludeEvents   | 
                                             CalCommon::EIncludeAnnivs;

    CalenAgendaUtils::FindEventsForDayRangeL( aInstanceView, aList, filter, aDay, aDay );

    CalenAgendaUtils::RemoveEntriesEndingAtMidnightL( aList, aDay );

    if( aIncludeToDos )
        {
        CalenAgendaUtils::FindTodosForDayRangeL( aInstanceView, aList, aDay, aDay );
        }

    if( aSortForPopup )
        {
        CalenAgendaUtils::SortPopupInstanceList( aList );
        }
    else
        {
        CalenAgendaUtils::SortInstanceList( aList );
        }
     
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CalenAgendaUtils::EndsAtStartOfDayL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CalenAgendaUtils::EndsAtStartOfDayL( CCalInstance* aInstance,
                                                    const TTime& aDay )
    {
    TRACE_ENTRY_POINT;
    
    const TTime dayStart = CalenDateUtils::BeginningOfDay( aDay );
    const TTime startTime( aInstance->Time().TimeLocalL() );
    TTimeIntervalMinutes duration;
    aInstance->Entry().EndTimeL().TimeLocalL().MinutesFrom( aInstance->Entry().StartTimeL().TimeLocalL(), duration );
    const TTime endTime( startTime +duration );

    const TBool result( endTime > startTime && endTime == dayStart );

    TRACE_EXIT_POINT;
    return result;
    }

// -----------------------------------------------------------------------------
// CalenAgendaUtils::SortInstanceList
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CalenAgendaUtils::SortInstanceList( RPointerArray<CCalInstance>& aInstanceList )
    {
    TRACE_ENTRY_POINT;
    
    TLinearOrder<CCalInstance> instanceListOrder( CalenAgendaUtils::EntryCompare );
    aInstanceList.Sort( instanceListOrder );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CalenAgendaUtils::SortInstanceList
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CalenAgendaUtils::SortPopupInstanceList( RPointerArray<CCalInstance>& aInstanceList )
    {
    TRACE_ENTRY_POINT;
    
    TLinearOrder<CCalInstance> instanceListOrder( CalenAgendaUtils::PopupEntryCompare );
    aInstanceList.Sort( instanceListOrder );
    
    TRACE_EXIT_POINT;
    }

TInt CalenAgendaUtils::EntryCompare(const CCalInstance& aInstance1, const CCalInstance& aInstance2)
    {
    TRACE_ENTRY_POINT;
        
    TInt res = KErrArgument;
    TRAP_IGNORE( ( res = DoEntryCompareL( aInstance1, aInstance2 ) ) );
    
    TRACE_EXIT_POINT;    
    return res;
    }
// -----------------------------------------------------------------------------
// CalenAgendaUtils::DoEntryCompareL
// Common compare for all calendar entries. Order as follows:
//  1. ETodo
//  2. EEvent
//  3. EAnniv
//  4. EAppt, EReminder
// -----------------------------------------------------------------------------
//
TInt CalenAgendaUtils::DoEntryCompareL( const CCalInstance& aInstance1,
                                        const CCalInstance& aInstance2 )
    {
    TRACE_ENTRY_POINT;
    
    TInt ret( EEqual );

    const CCalEntry& entry1 = aInstance1.Entry();
    const CCalEntry& entry2 = aInstance2.Entry();
    const CCalEntry::TType type1 = entry1.EntryTypeL();
    const CCalEntry::TType type2 = entry2.EntryTypeL();

    // types are equal (reminders are handled as meetings)
    if( type1 == type2 ||
      ((type1 == CCalEntry::EAppt && type2 == CCalEntry::EReminder)  || 
      (type2 == CCalEntry::EAppt && type1 == CCalEntry::EReminder)) )
        {
        switch( type1 )
            {
            case CCalEntry::ETodo:
                ret = CalenAgendaUtils::CompareToDosL( entry1, entry2 );
                break;

            case CCalEntry::EAnniv:
            case CCalEntry::EEvent:
                ret = CalenAgendaUtils::CompareNonTimedNotesL( aInstance1, aInstance2 );
                break;

            case CCalEntry::EReminder:
            case CCalEntry::EAppt:
                ret = CalenAgendaUtils::CompareTimedNotesL( aInstance1, aInstance2 );
                break;

            default:
                ASSERT(EFalse);
            }
        }
        else // types are different
        {
#ifdef RD_CALENDAR_PREVIEW
        switch( type1 )
            {
            case CCalEntry::ETodo:
                // to-dos come always first...
                ret = ELessThan;
                break;
                
            case CCalEntry::EAnniv:
                // ...and then anniversaries...
                if( type2 == CCalEntry::ETodo )
                    {
                    ret = EGreaterThan;            
                    }
                else
                    {
                    ret = ELessThan;            
                    } 
                break;
                
            case CCalEntry::EEvent:
                // ...then day notes...
                if( (type2 == CCalEntry::ETodo) || (type2 == CCalEntry::EAnniv)) 
                    {
                    ret = EGreaterThan;            
                    }
                else
                    {
                    ret = ELessThan;            
                    }
                break;
                
            case CCalEntry::EReminder:
            case CCalEntry::EAppt:
                // ...and finally timed notes...
                ret = EGreaterThan;
                break;
            default:
                ASSERT(EFalse);
            }                
#else // !RD_CALENDAR_PREVIEW
        switch( type1 )
            {
            case CCalEntry::ETodo:
                // to-dos come always first...
                ret = ELessThan;
                break;

            case CCalEntry::EEvent:
                // ...then day notes...
                if( type2 == CCalEntry::ETodo )
                    {
                    ret = EGreaterThan;
                    }
                else
                    {
                    ret = ELessThan;
                    }                             
                break;

            case CCalEntry::EAnniv:
                // ...and anniversaries...
                if( (type2 == CCalEntry::ETodo) || (type2 == CCalEntry::EEvent)) 
                    {
                    ret = EGreaterThan;                    
                    }
                else
                    {
                    ret = ELessThan;                    
                    }
                break;

            case CCalEntry::EReminder:
            case CCalEntry::EAppt:
                // ...and finally timed notes.
                ret = EGreaterThan;
                break;

            default:
                ASSERT(EFalse);
            }
#endif // RD_CALENDAR_PREVIEW
        }
    
    TRACE_EXIT_POINT;
    return ret;
    }

TInt CalenAgendaUtils::PopupEntryCompare( const CCalInstance& aInstance1,
                                          const CCalInstance& aInstance2 )
    {
    TRACE_ENTRY_POINT;
        
    TInt res = KErrArgument;
    TRAP_IGNORE( ( res = DoPopupEntryCompareL( aInstance1, aInstance2 ) ) );
    
    TRACE_EXIT_POINT;    
    return res;
    }

// -----------------------------------------------------------------------------
// CalenAgendaUtils::DoPopupEntryCompareL
// Compare for calendar pop-up. Order as follows:
//  1. ETodo
//  2. EAnniv
//  3. EEvent
//  4. EAppt, EReminder
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CalenAgendaUtils::DoPopupEntryCompareL( const CCalInstance& aInstance1,
                                             const CCalInstance& aInstance2 )
    {
    TRACE_ENTRY_POINT;
    
    TInt ret( EEqual );
    const CCalEntry& entry1 = aInstance1.Entry();
    const CCalEntry& entry2 = aInstance2.Entry();
    const CCalEntry::TType type1 = entry1.EntryTypeL();
    const CCalEntry::TType type2 = entry2.EntryTypeL();

    // types are equal (reminders are handled as meetings)
    if( type1 == type2 ||
        ((type1 == CCalEntry::EAppt && type2 == CCalEntry::EReminder)  || 
        (type2 == CCalEntry::EAppt && type1 == CCalEntry::EReminder)) )
        {
        switch( type1 )
            {
            case CCalEntry::ETodo:
                ret = CalenAgendaUtils::CompareToDosL( entry1, entry2 );
                break;

            case CCalEntry::EAnniv:
            case CCalEntry::EEvent:
                ret = CalenAgendaUtils::CompareNonTimedNotesL( aInstance1, aInstance2 );
                break;

            case CCalEntry::EReminder:
            case CCalEntry::EAppt:
                ret = CalenAgendaUtils::CompareTimedNotesL( aInstance1, aInstance2 );
                break;

            default:
                ASSERT(EFalse);
            }
        }
     else // types are different
     {
         switch( type1 )
            {
            case CCalEntry::ETodo:
            // to-dos come always first...
            ret = ELessThan;
                break;

            case CCalEntry::EAnniv:
            // ...and then anniversaries...
            if( type2 == CCalEntry::ETodo )
                {
                ret = EGreaterThan;            
                }
            else
                {
                ret = ELessThan;            
                } 
                break;

            case CCalEntry::EEvent:
            // ...then day notes...
            if( (type2 == CCalEntry::ETodo) || (type2 == CCalEntry::EAnniv)) 
                {
                ret = EGreaterThan;            
                }
            else
                {
                ret = ELessThan;            
                }
            break;

            case CCalEntry::EReminder:
            case CCalEntry::EAppt:
                // ...and finally timed notes...
                ret = EGreaterThan;
                break;

            default:
                ASSERT(EFalse);
            }
        }
    
    TRACE_EXIT_POINT;
    return ret;
    }

// -----------------------------------------------------------------------------
// CalenAgendaUtils::CompareToDos
// Compares two to-do entries. Sort criteria:
//  1. Status: ETodoNeedsAction before ETodoCompleted
//  2. Due date: oldest first (1.1.2005 before 2.1.2005 )
//  3. Priority: highest first (1 before 2 )
//  4. Last modified: oldest first (1.1.2005 09:00 before 1.1.2005 10:00 )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CalenAgendaUtils::CompareToDosL( const CCalEntry& aEntry1,
                                      const CCalEntry& aEntry2 )
    {
    TRACE_ENTRY_POINT;

    TInt ret( EEqual );
    CCalEntry::TStatus status1 = aEntry1.StatusL();
    CCalEntry::TStatus status2 = aEntry2.StatusL();

    if( status1 == CCalEntry::ENullStatus )
        {
        status1 = CCalEntry::ETodoNeedsAction;
        }
    
    if( status2 == CCalEntry::ENullStatus )
        {
        status2 = CCalEntry::ETodoNeedsAction;
        }

    if( status1 == status2 )
        {
        TTime time1 = aEntry1.EndTimeL().TimeUtcL();
        TTime time2 = aEntry2.EndTimeL().TimeUtcL();

        if( time1 == time2 )
            {
            const TUint pri1( aEntry1.PriorityL() );
            const TUint pri2( aEntry2.PriorityL() );

            if( pri1 == pri2 )
                {
                time1 = aEntry1.LastModifiedDateL().TimeUtcL();
                time2 = aEntry2.LastModifiedDateL().TimeUtcL();

                if( time1 == time2 )
                    {
                    ret = EEqual;
                    }
                else if( time1 > time2 )
                    {
                    ret = EGreaterThan; // oldest first
                    }
                else
                    {
                    ret = ELessThan;
                    }
                }
            else
                {
                if( pri1 > pri2 )
                    {
                    ret = EGreaterThan;
                    }
                else
                    {
                    ret = ELessThan;
                    }
                }
            }
        else
            {
            if( time1 > time2 )
                {
                ret = EGreaterThan;
                }
            else
                {
                ret = ELessThan;
                }
            }
        }
    else
        {
        if( status1 == CCalEntry::ETodoCompleted )
            {
            ret = EGreaterThan;
            }
        else
            {
            ret = ELessThan;
            }
        }
    
    TRACE_EXIT_POINT;
    return ret;
    }

// -----------------------------------------------------------------------------
// CalenAgendaUtils::CompareNonTimedNotes
// Compares two non-timed notes. Sort criteria:
//  1. start time: oldest first
//  2. last modified:  oldest first
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CalenAgendaUtils::CompareNonTimedNotesL( const CCalInstance& aInstance1,
                                              const CCalInstance& aInstance2 )
    {
    TRACE_ENTRY_POINT;
    
    TInt ret( EEqual );
    TTime time1 = aInstance1.Time().TimeUtcL();
    TTime time2 = aInstance2.Time().TimeUtcL();

    if( time1 == time2 )
        {
        time1 = aInstance1.Entry().LastModifiedDateL().TimeUtcL();
        time2 = aInstance2.Entry().LastModifiedDateL().TimeUtcL();

        if( time1 == time2 )
            {
            ret = EEqual;
            }
        else if( time1 > time2 )
            {
            ret = EGreaterThan; // oldest first
            }
        else
            {
            ret = ELessThan;
            }
        }
    else
        {
        if( time1 < time2 )
            {
            ret = ELessThan;
            }
        else
            {
            ret = EGreaterThan;
            }
        }
    
    TRACE_EXIT_POINT;
    return ret;
    }

// -----------------------------------------------------------------------------
// CalenAgendaUtils::CompareTimedNotes
// Compares two non-timed notes. Sort criteria:
//  1. start time: oldest first
//  2. duration: shortest first
//  3. last modified: oldest first
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CalenAgendaUtils::CompareTimedNotesL( const CCalInstance& aInstance1,
                                           const CCalInstance& aInstance2 )
    {
    TRACE_ENTRY_POINT;
    
    TInt ret( EEqual );
    TTime time1 = aInstance1.Time().TimeUtcL();
    TTime time2 = aInstance2.Time().TimeUtcL();

    if( time1 == time2 )
        {
        TTimeIntervalMinutes duration1;
        TTimeIntervalMinutes duration2;
        aInstance1.EndTimeL().TimeUtcL().MinutesFrom( aInstance1.StartTimeL().TimeUtcL(), duration1 );
        aInstance2.EndTimeL().TimeUtcL().MinutesFrom( aInstance2.StartTimeL().TimeUtcL(), duration2 );

        if( duration1 == duration2 )
            {
            time1 = aInstance1.Entry().LastModifiedDateL().TimeUtcL();
            time2 = aInstance2.Entry().LastModifiedDateL().TimeUtcL();

            if( time1 == time2 )
                {
                ret = EEqual;
                }
            else if( time1 > time2 )
                {
                ret = EGreaterThan; // oldest first
                }
            else
                {
                ret = ELessThan;
                }
            }
        else
            {
            if( duration1 < duration2 )
                {
                ret = ELessThan;
                }
            else
                {
                ret = EGreaterThan;
                }
            }
        }
    else
        {
        if( time1 < time2 )
            {
            ret = ELessThan;
            }
        else
            {
            ret = EGreaterThan;
            }
        }
    
    TRACE_EXIT_POINT;
    return ret;
    }

EXPORT_C TTimeIntervalMinutes CalenAgendaUtils::DurationL( const CCalEntry& aEntry )
    {
    TRACE_ENTRY_POINT;
    
    TTimeIntervalMinutes duration;

    const TTime start = aEntry.StartTimeL().TimeUtcL();
    const TTime end = aEntry.EndTimeL().TimeUtcL(); 
    end.MinutesFrom( start, duration );
    
    TRACE_EXIT_POINT;
    return duration;
    }


EXPORT_C TTime CalenAgendaUtils::EntryTimeL( const CCalEntry& aEntry )
    {
    TRACE_ENTRY_POINT;

    // FIXME: instance time!
    TTime entryTime; 
    
    if( aEntry.EntryTypeL() == CCalEntry::ETodo )
        {
        entryTime = aEntry.EndTimeL().TimeLocalL();
        }
    else
        {
        entryTime = aEntry.StartTimeL().TimeLocalL();
        }
    
    if ( entryTime == Time::NullTTime() )
        {
        TTime today; 
        today.HomeTime();                
        entryTime = today;
        }
    
    //FIXME: should we add one more step
    // entryTime = CalenDateUtils::LimitToValidRange( entryTime );
    TRACE_EXIT_POINT;
    return entryTime;
    }

EXPORT_C TBool CalenAgendaUtils::IsTimedEntryL( CCalEntry::TType aType ) 
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return aType == CCalEntry::EAppt || aType == CCalEntry::EReminder;
    }

// ---------------------------------------------------------
// CCalenDayOnlyEventContainer::IsRepeatingL
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool CalenAgendaUtils::IsRepeatingL( const CCalEntry& aEntry )
    {    
    TRACE_ENTRY_POINT;
    
    TBool isRepeating = EFalse;
    RArray<TCalTime> rdates;
    CleanupClosePushL( rdates );
    aEntry.GetRDatesL( rdates );
    TBool isRDate = rdates.Count() > 0;
    CleanupStack::PopAndDestroy(); // rdates

    TCalRRule newRule;

    if ( ( isRDate ) || ( aEntry.GetRRuleL(newRule) && newRule.Type() != TCalRRule::EInvalid ) )
        {
        isRepeating = ETrue;
        }
    
    TRACE_EXIT_POINT;
    return isRepeating;
    }

EXPORT_C TBool CalenAgendaUtils::IsEmptyText(const TDesC& aDes)
    {
    TRACE_ENTRY_POINT;
    
    TBool empty = ETrue;
    TInt length = aDes.Length();
    for(TUint i = 0; i < length; ++i)
        {
        if( !VersitUtils::IsWhiteSpace(aDes.operator[](i)) )
            {
            empty = EFalse;
            break;
            }
        }
        
    TRACE_EXIT_POINT;
    return empty;
    }

//  End of File
