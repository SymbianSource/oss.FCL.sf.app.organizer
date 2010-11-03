/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Storage class for day and week views.
*
*/


//  INCLUDES
#include "cbcslotinfostorage.h"
#include "cbcalendefs.h"
#include "calendateutils.h"
#include "calenagendautils.h"
#include "calenconstants.h"

#include <eikenv.h>

namespace
{
const TInt KUntimedDefaultSlot(16);
}

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::NewL
// ---------------------------------------------------------------------------
//
CBCSlotInfoStorage* CBCSlotInfoStorage::NewL( TSlotsInHour aSlotsInHour )
    {
    CBCSlotInfoStorage* self = CBCSlotInfoStorage::NewLC( aSlotsInHour );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::NewLC
// ---------------------------------------------------------------------------
//
CBCSlotInfoStorage* CBCSlotInfoStorage::NewLC( TSlotsInHour aSlotsInHour )
    {
    CBCSlotInfoStorage* self = new( ELeave ) CBCSlotInfoStorage( aSlotsInHour );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::CBCSlotInfoStorage
// ---------------------------------------------------------------------------
//
CBCSlotInfoStorage::CBCSlotInfoStorage( TSlotsInHour aSlotsInHour )
    :
    iFirstUntimedSlot(KUntimedDefaultSlot),
    iSlotsInHour(aSlotsInHour)
    {
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::ConstructL
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::ConstructL()
    {
    iSelectedSlot = KFSCalStartingHour * iSlotsInHour;
    iSelectedRegion = KErrNotFound;
    iSelectedColumn = 0;
    iSelectedAlldayEvent = KErrNotFound;
    iEarliestEndSlot = KErrNotFound;
    iLastStartSlot = KErrNotFound;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CBCSlotInfoStorage::~CBCSlotInfoStorage()
    {
    for ( TInt i = 0; i < iRegionList.Count(); i++ )
        {
        iRegionList[i].Close();
        }
    iRegionList.Close();
    iUntimedEvents.Close();
    iTodoEvents.Close();
    iAlldayEvents.Close();
    }
// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::Reset
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::Reset()
    {
    for ( TInt i = 0; i < iRegionList.Count(); i++ )
        {
        iRegionList[i].Close();
        }
    iRegionList.Reset();
    iUntimedEvents.Reset();
    iTodoEvents.Reset();
    iAlldayEvents.Reset();

    iUntimedSlotCount=0;
    iFirstUntimedSlot= KFSCalStartingHour * iSlotsInHour;

    iSelectedSlot = KFSCalStartingHour * iSlotsInHour;
    iSelectedRegion = KErrNotFound;
    iSelectedColumn = 0;
    iSelectedAlldayEvent = KErrNotFound;
    iEarliestEndSlot = KErrNotFound;
    iLastStartSlot = KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::InsertTimedEvent
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::InsertTimedEvent( const SCalenApptInfo& aItemInfo )
    {

    TInt startIndex = SlotIndexForStartTime( aItemInfo.iStartTime );
    TInt endIndex = SlotIndexForEndTime( aItemInfo.iEndTime );
    if ( endIndex == startIndex )
        {
        endIndex++;
        }
    if ( iRegionList.Count() > 0 )
        {
        // the timed events must be added in order
        ASSERT( startIndex >= iLastStartSlot );
        }

    TCalenTimedEventInfo info;
    info.iStartSlot = startIndex;
    info.iEndSlot = endIndex;
    info.iId = aItemInfo.iId;
    info.iStatus = aItemInfo.iStatus;
    info.iReplicationStatus = aItemInfo.iReplicationStatus;
    iLastStartSlot = startIndex;
    if ( iEarliestEndSlot == KErrNotFound || endIndex < iEarliestEndSlot )
        {
        iEarliestEndSlot = endIndex;
        }

    TBool added = EFalse;
    if ( iRegionList.Count() > 0 )
        {
        // Since events are added in order, the event either overlaps the
        // last region or doesn't overlap any region at all
        TCalenTimeRegion& region = iRegionList[iRegionList.Count() - 1];
        if ( region.Overlaps( info ) )
            {
            region.AddEvent( info );
            added = ETrue;
            }
        }

    if ( !added )
        {
        // If it didn't overlap the last region, add a new region for this event.
        iRegionList.Append( TCalenTimeRegion() );
        TCalenTimeRegion& region = iRegionList[iRegionList.Count() - 1];
        region.AddEvent( info );

        // Do rounding of the region ends
        if ( iRegionList.Count() >= 2 )
            {
            // Check if the previous region end time can be rounded down
            TCalenTimeRegion& prevRegion = iRegionList[iRegionList.Count() - 2];
            TInt end = RoundHourDown( prevRegion.iEndSlot );
            if ( end <= region.iStartSlot )
                {
                prevRegion.iEndSlot = end;
                }
            // Check if the start time of the new region can be rounded up
            TInt start = RoundHourUp( region.iStartSlot );
            if ( start >= prevRegion.iEndSlot )
                {
                region.iStartSlot = start;
                }
            }
        else
            {
            // No previous region, round the start time up
            region.iStartSlot = RoundHourUp( region.iStartSlot );
            }
        }
    }

void CBCSlotInfoStorage::InsertAlldayEvent( const SCalenApptInfo& aItemInfo )
    {
    TCalenTimedEventInfo info;
    info.iStartSlot = 0;
    info.iEndSlot = iSlotsInHour*KCalenHoursInDay + iUntimedSlotCount;
    info.iId = aItemInfo.iId;
    info.iStatus = aItemInfo.iStatus;
    info.iReplicationStatus = aItemInfo.iReplicationStatus;

    iAlldayEvents.Append( info );
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::IsAllDayEvent
// Allday event is an event starting and ending at midnight,
// with a duration of n*24h.
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::IsAlldayEvent( TTime aStart, TTime aEnd )
    {
    TBool isAllDay( EFalse );

    if ( !CalenDateUtils::OnSameDay( aStart, aEnd ) )
        {
        TTime startMidnight = CalenDateUtils::BeginningOfDay(aStart);
        TTime endMidnight = CalenDateUtils::BeginningOfDay(aEnd);

        if ( ( aStart == startMidnight ) && ( aEnd == endMidnight ) )
            {
            isAllDay = ETrue;
            }
        }
    return isAllDay;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::IsAllDayEvent
// Allday event is an event starting and ending at midnight,
// with a duration of n*24h.
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::IsAlldayEvent( const CCalInstance& aInstance )
    {
    TBool isAllDay( EFalse );

    // If getting the start or end time fails, return EFalse.
    TTime start = 0;
    TTime end = 0;
    TRAPD( error,
        {
        start = aInstance.StartTimeL().TimeLocalL();
        end = aInstance.EndTimeL().TimeLocalL();
        });

    if ( error == KErrNone && !CalenDateUtils::OnSameDay( start, end ) )
        {
        TTime startMidnight = CalenDateUtils::BeginningOfDay(start);
        TTime endMidnight = CalenDateUtils::BeginningOfDay(end);

        if ( ( start == startMidnight ) && ( end == endMidnight ) )
            {
            isAllDay = ETrue;
            }
        }

    if ( error != KErrNone )
        {
        CEikonEnv::Static()->HandleError( error );//codescanner::eikonenvstatic
        }

    return isAllDay;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::AlldayCount
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::AlldayCount()
    {
    return iAlldayEvents.Count();
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::TodoCount
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::TodoCount()
    {
    return iTodoEvents.Count();
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::GetLocation
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::GetLocation( const SCalenApptInfo& aItemInfo,
                                           TInt& aStartSlot,
                                           TInt& aEndSlot,
                                           TInt& aColumnIndex,
                                           TInt& aColumns )
    {
    TInt startIndex = SlotIndexForStartTime( aItemInfo.iStartTime );
    TInt endIndex = SlotIndexForEndTime( aItemInfo.iEndTime );
    if ( endIndex == startIndex )
        {
        endIndex++;
        }

    aStartSlot = startIndex;
    aEndSlot = endIndex;

    TCalenSlotInterval interval;
    interval.iStartSlot = startIndex;
    interval.iEndSlot = endIndex;

    TBool found = EFalse;
    // Scan through all allday events and see if it matches any of them
    for ( TInt i = 0; i < iAlldayEvents.Count() && !found; i++ )
        {
        if ( aItemInfo.iId == iAlldayEvents[i].iId )
            {
            aColumns = iAlldayEvents.Count();
            aColumnIndex = i;
            found = ETrue;
            }
        }

    // Scan through all regions and see if the event overlaps any region.
    // If it overlaps a region, the event must be in that region.
    for ( TInt i = 0; i < iRegionList.Count() && !found; i++ )
        {
        TCalenTimeRegion& region = iRegionList[i];
        if ( region.Overlaps( interval ) )
            {
            // Scan through all columns and look for the event
            aColumns = region.iColumns.Count();
            for ( TInt j = 0; j < aColumns && !found; j++ )
                {
                if ( region.iColumns[j].ContainsEvent( aItemInfo.iId ) )
                    {
                    aColumnIndex = j;
                    found = ETrue;
                    }
                }
            // the event should be in this region if it overlaps the region
            ASSERT( found );
            }
        }
    // the event should have been found somewhere
    ASSERT( found );
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::GetSelectedSlot
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::GetSelectedSlot( TInt& aSlot,
                                          TInt& aRegion,
                                          TInt& aColumnIndex,
                                          TInt& aColumns )
    {

    aSlot = iSelectedSlot;
    aColumnIndex = iSelectedColumn;
    if ( iSelectedRegion >= 0 )
        {
        aColumns = iRegionList[iSelectedRegion].iColumns.Count();
        aRegion = iSelectedRegion;
        }
    else
        {
        aColumns = 0;
        aRegion = KErrNotFound;
        }
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::FindRegion
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::FindRegion( const TCalenSlotInterval& aInterval,
                                          TInt aDirection )
    {
    // Setup the start and end region indices for iterating
    TInt start, end;
    if ( aDirection > 0 )
        {
        start = 0;
        end = iRegionList.Count();
        }
    else
        {
        start = iRegionList.Count() - 1;
        end = -1;
        }

    TInt region = KErrNotFound;

    // Iterate over the regions, in the order given, looking for a region
    // overlapping this interval
    for ( TInt i = start; i != end && region < 0; i += aDirection )
        {
        if ( iRegionList[i].Overlaps( aInterval ) )
            {
            region = i;
            }
        }
    return region;
    }

// ---------------------------------------------------------------------------
// Find an event within the given interval of the current column,
// searching in the given direction.
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::FindEvent( const TCalenSlotInterval& aInterval,
                                         TInt aDirection )
    {
    ASSERT( iSelectedRegion >= 0 && iSelectedRegion < iRegionList.Count() );

    TCalenTimeRegion& region = iRegionList[iSelectedRegion];
    TInt index = KErrNotFound;

    if ( iSelectedColumn < region.iColumns.Count() )
        {
        TCalenTimeColumn& column = region.iColumns[iSelectedColumn];
        TInt start, end;
        if ( aDirection > 0 )
            {
            start = 0;
            end = column.iEventArray.Count();
            }
        else
            {
            start = column.iEventArray.Count() - 1;
            end = -1;
            }

        // Iterate over the events in the chosen column in the order given,
        // looking for an event overlapping this interval
        for ( TInt i = start; i != end && index < 0; i += aDirection )
            {
            if ( column.iEventArray[i].Overlaps( aInterval ) )
                {
                index = i;
                }
            }
        }
    return index;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::IsEventSelected
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::IsEventSelected() const
    {
    TBool selected = EFalse;
    if ( iSelectedAlldayEvent >= 0 )
        {
        selected = ETrue;
        }
    else if ( iSelectedRegion >= 0 &&
         iSelectedColumn < iRegionList[iSelectedRegion].iColumns.Count() &&
         iSelectedColumnEventIndex >= 0 )
        {
        selected = ETrue;
        }
    else if ( IsExtraSlot( iSelectedSlot ) &&
         iSelectedSlot - iFirstUntimedSlot >= iEmptyUntimedSlots )
        {
        selected = ETrue;
        }
    else
        {
        // if no event is selected, the selection slot must be a hour starting slot or an extra slot
        ASSERT( IsHourStartSlot( iSelectedSlot ) || IsExtraSlot( iSelectedSlot ) );
        selected = EFalse;
        }
    return selected;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::IsMultipleEventsSelected
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::IsMultipleEventsSelected() const
    {
    TBool isMultiple = EFalse;
    if ( iSelectedAlldayEvent >= 0 )
        {
        // If an allday event is selected, disregard the value of
        // iSelectedSlot
        isMultiple = EFalse;
        }
    else if ( IsExtraSlot( iSelectedSlot ) )
        {
        TInt index = iSelectedSlot - iFirstUntimedSlot - iEmptyUntimedSlots;
        isMultiple = ( index == 0 ) && iTodoEvents.Count() > 1;
        }
    return isMultiple;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::IsAlldayEventSelected
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::IsAlldayEventSelected() const
    {
    TBool selected = EFalse;
    if ( iSelectedAlldayEvent >= 0 )
        {
        ASSERT( iSelectedAlldayEvent < iAlldayEvents.Count() );
        selected = ETrue;
        }
    return selected;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::UntimedEvent
// ---------------------------------------------------------------------------
//
TCalenInstanceId CBCSlotInfoStorage::UntimedEvent( TInt aIndex )
    {
    TCalenInstanceId id;
    if ( iTodoEvents.Count() > 0 && aIndex == 0 )
        {
        id = iTodoEvents[0];
        }
    else
        {
        // If we have one or more todo items, the first slot is used for them
        // (but not more than one slot)
        if ( iTodoEvents.Count() > 0 )
            {
            aIndex--;
            }

        if ( aIndex >= 0 && aIndex < iUntimedEvents.Count() )
            {
            id = iUntimedEvents[aIndex];
            }
        else
            {
            // should not happen
            User::Invariant();
            }
        }

    return id;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::AlldayEvent
// ---------------------------------------------------------------------------
//
const TCalenTimedEventInfo& CBCSlotInfoStorage::AlldayEvent( TInt aIndex )
    {
    ASSERT( aIndex >= 0 && aIndex < iAlldayEvents.Count() );
    return iAlldayEvents[aIndex];
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::SelectedEvent
// ---------------------------------------------------------------------------
//
TCalenInstanceId CBCSlotInfoStorage::SelectedEvent()
    {
    TCalenInstanceId id;
    if ( iSelectedAlldayEvent >= 0 )
        {
        ASSERT( iSelectedAlldayEvent < iAlldayEvents.Count() );
        id = iAlldayEvents[iSelectedAlldayEvent].iId;
        }
    else if ( iSelectedRegion >= 0 )
        {
        ASSERT( iSelectedRegion < iRegionList.Count() );

        TCalenTimeRegion& region = iRegionList[iSelectedRegion];
        if ( iSelectedColumn < region.iColumns.Count() )
            {
            TCalenTimeColumn& column = region.iColumns[iSelectedColumn];
            if ( iSelectedColumnEventIndex >= 0 &&
                 iSelectedColumnEventIndex < column.iEventArray.Count() )
                {
                TCalenTimedEventInfo& info = column.iEventArray[iSelectedColumnEventIndex];
                id = info.iId;
                }
            else
                {
                // should not happen, no event selected in the column
                User::Invariant();
                }
            }
        else
            {
            // should not happen, the empty column was selected
            User::Invariant();
            }
        }
    else if ( IsExtraSlot( iSelectedSlot ) )
        {
        TInt index = iSelectedSlot - iFirstUntimedSlot - iEmptyUntimedSlots;
        id = UntimedEvent( index );
        }
    else
        {
        // should not happen
        User::Invariant();
        }

    return id;
    }



// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::SelectEvent
// Return KErrNotFound if not found, otherwise KErrNone
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::SelectEvent( const TCalenInstanceId& aId )
    {
    // Id can be of allday event, untimed event or other events

    // Look for this event id in all event data structures and set
    // the state accordingly if it was found

    for ( TInt i(0); i < iAlldayEvents.Count(); i++ )
        {
        TCalenInstanceId id = iAlldayEvents[i].iId;
        if ( id == aId )
            {
            iSelectedAlldayEvent = i;
            iSelectedRegion = KErrNotFound;
            return KErrNone;
            }
        }


    for ( TInt i(0); i < iTodoEvents.Count(); i++ )
        {
        TCalenInstanceId id = iTodoEvents[i];
        if ( id == aId )
            {
            iSelectedAlldayEvent = KErrNotFound;
            iSelectedRegion = KErrNotFound;
            iSelectedSlot = iFirstUntimedSlot + iEmptyUntimedSlots;
            return KErrNone;
            }
        }


    for ( TInt i(0); i < iUntimedEvents.Count(); i++ )
        {
        TCalenInstanceId id = iUntimedEvents[i];
        if ( id == aId )
            {
            iSelectedAlldayEvent = KErrNotFound;
            iSelectedRegion = KErrNotFound;
            iSelectedSlot = iFirstUntimedSlot + iEmptyUntimedSlots + i;
            if ( iTodoEvents.Count() > 0 )
                {
                iSelectedSlot++;
                }
            return KErrNone;
            }
        }

    for ( TInt i(0); i < iRegionList.Count(); i++ )
        {
        TCalenTimeRegion& region = iRegionList[i];
        for ( TInt col(0); col < region.iColumns.Count(); col++ )
            {
            for ( TInt ind(0); ind < region.iColumns[col].iEventArray.Count(); ind++ )
                {
                if ( region.iColumns[col].iEventArray[ind].iId == aId )
                    {
                    iSelectedAlldayEvent = KErrNotFound;
                    iSelectedRegion = i;
                    iSelectedColumn = col;
                    iSelectedColumnEventIndex = ind;
                    SetSelectionInEvent( EMoveDirectionDown );
                    return KErrNone;
                    }
                }
            }
        }

    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::EnterRegionFromBelow
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::EnterRegionFromBelow()
    {
    ASSERT( iSelectedRegion >= 0 && iSelectedRegion < iRegionList.Count() );
    iSelectedColumn = KErrNotFound;
    TCalenTimeRegion& region = iRegionList[iSelectedRegion];
    TInt latestEnd = region.iStartSlot;
    // Look for the column with the latest end
    for ( TInt i = 0; i < region.iColumns.Count(); i++ )
        {
        TCalenTimeColumn& column = region.iColumns[i];
        ASSERT( column.iEventArray.Count() > 0 );
        if ( column.iEndSlot > latestEnd )
            {
            // found a column with an event touching the end of the region
            latestEnd = column.iEndSlot;
            iSelectedColumn = i;
            iSelectedColumnEventIndex = column.iEventArray.Count() - 1;
            SetSelectionInEvent( EMoveDirectionUp );
            }
        }
    ASSERT( iSelectedColumn >= 0 );
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::EnterRegionFromAbove
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::EnterRegionFromAbove()
    {
    ASSERT( iSelectedRegion >= 0 && iSelectedRegion < iRegionList.Count() );
    iSelectedColumn = KErrNotFound;
    TCalenTimeRegion& region = iRegionList[iSelectedRegion];
    TInt earliestStart = region.iEndSlot;
    // Look for the column with the earliest start
    for ( TInt i = 0; i < region.iColumns.Count(); i++ )
        {
        TCalenTimeColumn& column = region.iColumns[i];
        ASSERT( column.iEventArray.Count() > 0 );
        if ( column.iStartSlot < earliestStart )
            {
            // found a column with an event
            earliestStart = column.iStartSlot;
            iSelectedColumn = i;
            iSelectedColumnEventIndex = 0;
            SetSelectionInEvent( EMoveDirectionDown );
            }
        }
    ASSERT( iSelectedColumn >= 0 );
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::NextFocusArea
// Determines how large area to scan for new events/regions when moving in
// the given direction.  If no events are found in this area, focus an
// empty slot instead
// ---------------------------------------------------------------------------
//
TCalenSlotInterval CBCSlotInfoStorage::NextFocusArea(
                                            const TCalenSlotInterval& aInterval,
                                            TInt aDirection )
    {
    // NOTE: the comments are written with the visual layout in mind.
    // upwards == up in the display, towards earlier times
    // downwards == down in the display, towards later times
    TCalenSlotInterval target;
    target.iEndSlot = 0;
    target.iStartSlot = 0;

    if ( IsExtraSlot( aInterval.iStartSlot ) )
        {
        // Special case logic for moving within the extra slots.

        // The index within the extra slot area
        TInt extraIndex = aInterval.iStartSlot - iFirstUntimedSlot;
        // Generally, move just one slot at a time in this area
        TInt newIndex = extraIndex + aDirection;

        if ( newIndex < iEmptyUntimedSlots )
            {
            // Moved upwards past the first slot, return
            // the whole last hour before the untimed slot area
            target.iEndSlot = iFirstUntimedSlot;
            target.iStartSlot = target.iEndSlot - iSlotsInHour;
            }
        else if ( newIndex >= iUntimedSlotCount )
            {
            // Moved downwards past the last untimed slot, return
            // the whole first hour after the untimed slot area
            target.iStartSlot = iFirstUntimedSlot + iUntimedSlotCount;
            target.iEndSlot = target.iStartSlot + iSlotsInHour;
            }
        else
            {
            // Moved normally within the untimed slot area
            target.iStartSlot = aInterval.iStartSlot + aDirection;
            target.iEndSlot = target.iStartSlot + 1;
            }
        return target;
        }

    // Helper interval for the whole untimed area
    TCalenSlotInterval untimedInterval;
    untimedInterval.iStartSlot = iFirstUntimedSlot;
    untimedInterval.iEndSlot = untimedInterval.iStartSlot + iUntimedSlotCount;

    if ( aDirection < 0 )
        {
        // Moving upwards
        // Create a target interval of one hour upwards
        target.iEndSlot = aInterval.iStartSlot;
        target.iStartSlot = aInterval.iStartSlot - iSlotsInHour;

        if ( iUntimedSlotCount > 0 && untimedInterval.Overlaps( target ))
            {
            // The target interval overlaps the untimed area
            if ( iUntimedSlotCount > iEmptyUntimedSlots )
                {
                // Make the interval start at the last untimed slot
                target.iStartSlot = untimedInterval.iEndSlot - 1;
                }
            else
                {
                // Untimed area containing no actual events.
                // Include one whole hour before the untimed area.
                target.iStartSlot = iFirstUntimedSlot - iSlotsInHour;
                }
            return target;
            }

        // Round the start slot upwards to an even hour
        target.iStartSlot = RoundHourUp( target.iStartSlot );
        }
    else
        {
        // Moving downwards
        // Create a target interval of one hour downwards
        target.iStartSlot = aInterval.iEndSlot;
        target.iEndSlot = aInterval.iEndSlot + iSlotsInHour;

        if ( iUntimedSlotCount > 0 && untimedInterval.Overlaps( target ) )
            {
            // The target interval overlaps the untimed area

            // Assumption: There can't be any regions before the untimed area
            if ( iUntimedSlotCount > iEmptyUntimedSlots )
                {
                // Make the interval end at the first untimed slot containing
                // an event
                target.iStartSlot = untimedInterval.iStartSlot + iEmptyUntimedSlots;
                target.iEndSlot = target.iStartSlot + 1;
                }
            else
                {
                // Untimed area containing no actual events.
                // Include one whole hour after the untimed area.
                target.iStartSlot = untimedInterval.iStartSlot + iUntimedSlotCount;
                target.iEndSlot = target.iStartSlot + iSlotsInHour;
                }
            return target;
            }

        target.iEndSlot = RoundHourDown( target.iEndSlot );
        }

    return target;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::NextEmptyFocusSlot
// If no events are found in an empty area scanned, focus this slot instead
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::NextEmptyFocusSlot( const TCalenSlotInterval& aInterval,
                                                  TInt aDirection )
    {
    // NOTE: the comments are written with the visual layout in mind.
    // upwards == up in the display, towards earlier times
    // downwards == down in the display, towards later times

    TInt target( 0 );

    // If the next focus area logic says we should go to an untimed slot,
    // skip the rest of this method
    TCalenSlotInterval testInterval = NextFocusArea( aInterval, aDirection );
    if ( IsExtraSlot( testInterval.iStartSlot ) )
        {
        target = testInterval.iStartSlot;
        }
    else
        {
        if ( aDirection < 0 )
            {
            // Moving upwards, target slot is the start of the area returned by
            // NextFocusArea. This always is an even hour.
            target = testInterval.iStartSlot;
            ASSERT( IsHourStartSlot( target ) );
            }
        else
            {
            target = testInterval.iStartSlot;
            if ( !IsHourStartSlot( target ) )
                {
                // If the interval doesn't start on an even hour, round downwards.
                target = RoundHourDown( target );
                }
            }
        }

    return target;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::EmptySelectionInterval
// Create an interval representation of the current focus area
// ---------------------------------------------------------------------------
//
TCalenSlotInterval CBCSlotInfoStorage::EmptySelectionInterval()
    {
    TCalenSlotInterval target;
    target.iEndSlot = 0;
    target.iStartSlot = 0;

    if ( IsExtraSlot( iSelectedSlot ) )
        {
        // Return an interval consisting of one single slot
        target.iStartSlot = iSelectedSlot;
        target.iEndSlot = target.iStartSlot + 1;
        }
    else
        {
        // Round the start slot to an even hour if it isn't.
        // NOTE: This actually updates the selection state!
        // Only call this method if no event actually is selected.
        if ( !IsHourStartSlot( iSelectedSlot ) )
            {
            iSelectedSlot = RoundHourUp( iSelectedSlot );
            }
        target.iStartSlot = iSelectedSlot;
        target.iEndSlot = iSelectedSlot + iSlotsInHour;
        }

    return target;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::SelectedInterval
// ---------------------------------------------------------------------------
//
TCalenSlotInterval CBCSlotInfoStorage::SelectedInterval()
    {
    TCalenSlotInterval selection;
    selection.iStartSlot = selection.iEndSlot = 0;

    if ( iSelectedAlldayEvent >= 0 )
        {
        selection.iStartSlot = 0;
        selection.iEndSlot = iSlotsInHour*KCalenHoursInDay + iUntimedSlotCount;
        }
    else if ( iSelectedRegion >= 0 )
        {

        TCalenTimeRegion& region = iRegionList[iSelectedRegion];
        if ( iSelectedColumn < region.iColumns.Count() )
            {
            TCalenTimeColumn& column = region.iColumns[iSelectedColumn];
            if ( iSelectedColumnEventIndex >= 0 &&
                 iSelectedColumnEventIndex < column.iEventArray.Count() )
                {
                TCalenTimedEventInfo& info = column.iEventArray[iSelectedColumnEventIndex];
                // gets only the TCalenSlotInterval part from the TCalenTimedEventInfo struct
                selection = info;
                }
            }
        }

    if ( selection.IsEmpty() )
        {
        // No selection was set in the cases above, no event is selected, thus
        // use the empty area selection interval instead.
        selection = EmptySelectionInterval();
        }
    return selection;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::SetSelectionInRegion
// Sets selection within a region
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::SetSelectionInRegion( TInt aRegion, TInt aColumn, TInt aSlot )
    {
    TBool selected(EFalse);
    StoreOrigSelection();
    //validate given values
    if ( aRegion >= 0 && aRegion < iRegionList.Count() )
        {//ASSERT( iSelectedRegion >= 0 && iSelectedRegion < iRegionList.Count() );
        TCalenSlotInterval interval;
        //TODO: round aSlot hour to full hour down????
        interval.iStartSlot = aSlot;
        interval.iEndSlot = interval.iStartSlot + 1;
        
        TCalenTimeRegion& region = iRegionList[aRegion];
        if ( region.Overlaps( interval ) )
            {
            if ( aColumn >= 0 && aColumn < region.iColumns.Count() )
                {
                TBool eventFound(EFalse);
                TCalenTimeColumn& column = region.iColumns[ aColumn ];
                if ( column.Overlaps( interval ) )
                    {
                    ASSERT( column.iEventArray.Count() > 0 );
                    for ( TInt i = 0; i < column.iEventArray.Count(); i++ )
                        {
                        TCalenTimedEventInfo& event = column.iEventArray[i];
                        if ( event.Overlaps( interval ) )
                            {
                            iSelectedColumnEventIndex = i;
                            
                            iSelectedAlldayEvent = KErrNotFound;
                            iSelectedRegion = aRegion;
                            iSelectedColumn = aColumn;
                            iSelectedSlot = aSlot;
                            selected = ETrue;
                            eventFound = ETrue;
                            break;
                            }
                        
                        }
                    }
                
                if (!eventFound)
                    {
                    iSelectedAlldayEvent = KErrNotFound;
                    iSelectedRegion = aRegion;
                    iSelectedColumn = aColumn;
                    iSelectedSlot = aSlot;
                    iSelectedColumnEventIndex = KErrNotFound;
                    selected = ETrue;
                    }             
                }
            else if ( aColumn == iRegionList[aRegion].iColumns.Count() )
                {//empty selection on right selected
                iSelectedAlldayEvent = KErrNotFound;
                iSelectedRegion = aRegion;
                iSelectedColumn = aColumn;
                iSelectedSlot = aSlot;
                iSelectedColumnEventIndex = KErrNotFound;
                selected = ETrue;
                }
            }
        }
    
    return selected;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::MoveOutFromRegion
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::MoveOutFromRegion( TInt aDirection )
    {
    TCalenTimeRegion& region = iRegionList[iSelectedRegion];
    TCalenSlotInterval targetInterval = NextFocusArea( region, aDirection );
    TInt next = iSelectedRegion + aDirection;
    // Check what the next region would be and if we should move into that
    // or into an empty area
    if ( next >= 0 && next < iRegionList.Count() )
        {
        if ( iRegionList[next].Overlaps( targetInterval ) )
            {
            iSelectedRegion = next;
            if ( aDirection < 0 )
                {
                EnterRegionFromBelow();
                }
            else
                {
                EnterRegionFromAbove();
                }
            }
        else
            {
            iSelectedRegion = KErrNotFound;
            iSelectedSlot = NextEmptyFocusSlot( region, aDirection );
            }
        }
    else
        {
        iSelectedRegion = KErrNotFound;
        iSelectedSlot = NextEmptyFocusSlot( region, aDirection );
        }
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::MoveInColumn
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::MoveInColumn( TInt aDirection )
    {
    TCalenTimeRegion& region = iRegionList[iSelectedRegion];
    if ( iSelectedColumn < region.iColumns.Count() )
        {
        TCalenTimeColumn& column = region.iColumns[iSelectedColumn];
        if ( iSelectedColumnEventIndex >= 0 )
            {
            // We currently have an event in a column selected
            TCalenSlotInterval curInterval = column.iEventArray[iSelectedColumnEventIndex];
            TCalenSlotInterval targetInterval = NextFocusArea( curInterval, aDirection );

            TInt next = iSelectedColumnEventIndex + aDirection;
            // Check if the target interval overlaps the next event in this column
            if ( next >= 0 && next < column.iEventArray.Count() )
                {
                if ( column.iEventArray[next].Overlaps( targetInterval ) )
                    {
                    iSelectedColumnEventIndex = next;
                    SetSelectionInEvent( aDirection );
                    }
                else
                    {
                    iSelectedColumnEventIndex = KErrNotFound;
                    iSelectedSlot = NextEmptyFocusSlot( curInterval, aDirection );
                    }
                }
            else
                {
                // There's no next event in this column, focus an empty area instead.
                iSelectedColumnEventIndex = KErrNotFound;
                iSelectedSlot = NextEmptyFocusSlot( curInterval, aDirection );
                }
            }
        else
            {
            // No event selected in the current column, find the target interval
            // and check if there's any event there
            TCalenSlotInterval curInterval = EmptySelectionInterval();
            TCalenSlotInterval targetInterval = NextFocusArea( curInterval, aDirection );
            iSelectedColumnEventIndex = FindEvent( targetInterval, aDirection );
            if ( iSelectedColumnEventIndex >= 0 )
                {
                SetSelectionInEvent( aDirection );
                }
            else
                {
                iSelectedSlot = NextEmptyFocusSlot( curInterval, aDirection );
                }
            }

        // If the new selection lies outside of the region, move out from the region.
        TCalenSlotInterval selection = SelectedInterval();
        if ( selection.iStartSlot < region.iStartSlot || selection.iEndSlot > region.iEndSlot )
            {
            MoveOutFromRegion( aDirection );
            }

        }
    else
        {
        // The empty column was selected, just move up/down in that column
        iSelectedSlot = NextEmptyFocusSlot( EmptySelectionInterval(), aDirection );

        // When moving in the empty column, move out from the region only if
        // the selection moved completely out from the region.
        TCalenSlotInterval selection = SelectedInterval();
        if ( !selection.Overlaps( region ) )
            {
            // The target selection is completely outside of the current region
            TInt next = iSelectedRegion + aDirection;
            if ( next >= 0 && next < iRegionList.Count() &&
                 iRegionList[next].Overlaps( selection ) )
                {
                // The target selection overlaps the next region, move into that,
                // stay within the rightmost empty column
                iSelectedRegion = next;
                iSelectedColumn = iRegionList[next].iColumns.Count();
                iSelectedColumnEventIndex = KErrNotFound;
                }
            else
                {
                // Move out from the current region
                iSelectedRegion = KErrNotFound;
                iSelectedColumn = KErrNotFound;
                iSelectedColumnEventIndex = KErrNotFound;
                // iSelectedSlot was update above, keep the same value
                }
            }

        }

    }


// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::MoveInAlldayEvent
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::MoveInAlldayEvent( TScrollDirection aDirection )
    {
    ASSERT( iSelectedAlldayEvent >= 0 );
    TBool moved = ETrue;
    switch ( aDirection )
        {
        case EScrollUp:
            {
            // don't do anything
            }
            break;
        case EScrollDown:
            {
            // don't do anything
            }
            break;
        case EScrollLeft:
            {
            if ( iSelectedAlldayEvent > 0 )
                {
                // there are more allday events to the left, select next
                iSelectedAlldayEvent--;
                }
            else
                {
                // move to previous day
                moved = EFalse;
                }
            }
            break;
        case EScrollRight:
            {
            if ( iSelectedAlldayEvent < iAlldayEvents.Count() - 1 )
                {
                // there are more allday events to the right, select next
                iSelectedAlldayEvent++;
                }
            else
                {
                iSelectedAlldayEvent = KErrNotFound;
                // find a region to select
                if ( iSelectedRegion >= 0 )
                    {
                    // keep using the old selection state
                    }
                else
                    {
                    // find a new suitable selection

                    // if iSelectedSlot was moved to the empty untimed slot area,
                    // move it down to a real slot

                    if ( iSelectedSlot >= iFirstUntimedSlot &&
                         iSelectedSlot < iFirstUntimedSlot + iEmptyUntimedSlots )
                        {
                        iSelectedSlot = iFirstUntimedSlot + iEmptyUntimedSlots;
                        }

                    TCalenSlotInterval interval = EmptySelectionInterval();

                    iSelectedRegion = FindRegion( interval, EMoveDirectionDown );
                    TInt firstRegion = iSelectedRegion;
                    TBool found = EFalse;
                    // Iterate over all regions overlapping this interval, see
                    // if any of them has events in this column
                    while ( iSelectedRegion >= 0 &&
                            iSelectedRegion < iRegionList.Count() &&
                            iRegionList[iSelectedRegion].Overlaps( interval ) &&
                            !found )
                        {

                        ASSERT( iSelectedRegion < iRegionList.Count() );
                        TCalenTimeRegion& region = iRegionList[iSelectedRegion];

                        ASSERT( region.iColumns.Count() > 0 );
                        iSelectedColumn = 0;
                        TCalenTimeColumn& column = region.iColumns[iSelectedColumn];

                        iSelectedColumnEventIndex = FindEvent( interval, EMoveDirectionDown );
                        if ( iSelectedColumnEventIndex >= 0 )
                            {
                            ASSERT( iSelectedColumnEventIndex < column.iEventArray.Count() );
                            // Keep using the old selected slot also here, just
                            // update it to make sure it's within the actual event
                            UpdateSelectionInEvent();
                            found = ETrue;
                            }
                        else
                            {
                            // Check the next region
                            iSelectedRegion++;
                            }
                        }

                    if ( !found )
                        {
                        iSelectedRegion = firstRegion;
                        iSelectedColumn = 0;
                        iSelectedSlot = interval.iStartSlot;
                        }

                    }
                }
            }
            break;
        default:
            // Do nothing
            break;
        }

    return moved;
    }


// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::MoveBetweenColumns
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::MoveBetweenColumns( TScrollDirection aDirection )
    {
    // Moving to another column
    TCalenSlotInterval selection = SelectedInterval();
    if ( IsEventSelected() )
        {
        // If moving from an event, just use the selected
        // slot pos within the event, not the whole event
        // interval.
        selection.iStartSlot = iSelectedSlot;
        selection.iEndSlot = selection.iStartSlot + iSlotsInHour;
        // Crop the selection to be within the event
        selection.Intersect( SelectedInterval() );
        }

    switch ( aDirection )
        {
        case EScrollLeft:
            {
            iSelectedColumn--;
            break;
            }
        case EScrollRight:
            {
            iSelectedColumn++;
            break;
            }
        default:
            // Do nothing
            break;
        }

    // First try finding events exactly in the selection area
    iSelectedColumnEventIndex = FindEvent( selection, EMoveDirectionDown );
    if ( iSelectedColumnEventIndex >= 0 )
        {
        // An event was found in the given interval
        // Keep using the old selection position, just update it to make
        // sure it's within the event
        UpdateSelectionInEvent();
        }
    else
        {
        // No event found in the interval

        // Expand the interval to even hour endings
        selection.iStartSlot = RoundHourUp( selection.iStartSlot );
        selection.iEndSlot = RoundHourDown( selection.iEndSlot );
        TCalenTimeRegion& region = iRegionList[iSelectedRegion];
        // Crop the selection to the current region
        selection.Intersect( region );

        // Try again with the possibly larger interval
        iSelectedColumnEventIndex = FindEvent( selection, EMoveDirectionDown );
        if ( iSelectedColumnEventIndex >= 0 )
            {
            // An event was found in the given interval
            // Keep using the old selection position, just update it to make
            // sure it's within the event
            UpdateSelectionInEvent();
            }
        else
            {
            // Still no events found. Select an empty interval if possible.

            if ( !IsHourStartSlot( selection.iStartSlot ) )
                {
                // The starting slot isn't a hour start slot. Rounding directly
                // would move the interval outside of the region. See if we can
                // round the starting slot down instead and still be within
                // the region.
                TInt start = RoundHourDown( selection.iStartSlot );
                if ( start + iSlotsInHour <= region.iEndSlot )
                    {
                    // Ok, an empty selection fits within the region.
                    iSelectedSlot = start;
                    }
                }
            else if ( !IsHourStartSlot( selection.iEndSlot ) )
                {
                // The ending slot isn't a hour start slot. Rounding directly
                // could move the interval outside of the region. See if we can
                // round the upwards instead and still be within
                // the region.
                TInt start = RoundHourUp( selection.iEndSlot - iSlotsInHour );
                if ( start >= region.iStartSlot )
                    {
                    // Ok, an empty selection fits within the region.
                    iSelectedSlot = start;
                    }
                }

            // Make sure the selected interval really is a valid
            // empty interval selection (this forces rounding if needed)
            selection = EmptySelectionInterval();

            // Check the final target interval once more.
            iSelectedColumnEventIndex = FindEvent( selection, EMoveDirectionDown );
            if ( iSelectedColumnEventIndex >= 0 )
                {
                // An event was found in the given interval
                UpdateSelectionInEvent();
                }
            else
                {
                // No even found, use the empty selection

                // If not within the region, move out
                if ( selection.iStartSlot < region.iStartSlot )
                    {
                    MoveOutFromRegion( EMoveDirectionUp );
                    }
                else if ( selection.iEndSlot > region.iEndSlot )
                    {
                    MoveOutFromRegion( EMoveDirectionDown );
                    }
                }
            }

        }

    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::MoveInRegion
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::MoveInRegion( TScrollDirection aDirection )
    {
    ASSERT( iSelectedAlldayEvent < 0 && iSelectedRegion >= 0 );
    TBool moved = ETrue;
    switch ( aDirection )
        {
        case EScrollUp:
            {
            MoveInColumn( EMoveDirectionUp );
            }
            break;
        case EScrollDown:
            {
            MoveInColumn( EMoveDirectionDown );
            }
            break;

        case EScrollLeft:
            {
            ASSERT( iSelectedRegion < iRegionList.Count() );
            TCalenTimeRegion& region = iRegionList[iSelectedRegion];
            ASSERT( iSelectedColumn >= 0 && iSelectedColumn <= region.iColumns.Count() );
            if ( iSelectedColumn == 0 )
                {
                if ( iAlldayEvents.Count() > 0 )
                    {
                    // remember the last selection state
                    iSelectedAlldayEvent = iAlldayEvents.Count() - 1;
                    }
                else
                    {
                    // move to previous day
                    moved = EFalse;
                    }
                }
            else
                {
                MoveBetweenColumns( aDirection );
                }
            }
            break;

        case EScrollRight:
            {
            ASSERT( iSelectedRegion < iRegionList.Count() );
            TCalenTimeRegion& region = iRegionList[iSelectedRegion];
            if ( iSelectedColumn < region.iColumns.Count() - 1 )
                {
                MoveBetweenColumns( aDirection );
                }
            else if ( iSelectedColumn == region.iColumns.Count() - 1 )
                {
                // In the last column, moving to the empty column
                iSelectedColumn++;
                iSelectedColumnEventIndex = KErrNotFound;

                // Round the selection to an empty interval
                EmptySelectionInterval();
                }
            else
                {
                // in the last, empty column
                // move to next day
                moved = EFalse;
                }
            }
            break;

        default:
            // Do nothing
            break;
        }

    return moved;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::MoveInEmptyArea
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::MoveInEmptyArea( TScrollDirection aDirection )
    {
    ASSERT( iSelectedAlldayEvent < 0 && iSelectedRegion < 0 );
    TBool moved = ETrue;
    switch ( aDirection )
        {
        case EScrollUp:
            {
            TCalenSlotInterval curInterval = EmptySelectionInterval();
            TCalenSlotInterval targetInterval = NextFocusArea( curInterval, EMoveDirectionUp );
            iSelectedRegion = FindRegion( targetInterval, EMoveDirectionUp );
            if ( iSelectedRegion >= 0 )
                {
                EnterRegionFromBelow();
                }
            else
                {
                iSelectedSlot = NextEmptyFocusSlot( curInterval, EMoveDirectionUp );
                }

            }
            break;
        case EScrollDown:
            {
            TCalenSlotInterval curInterval = EmptySelectionInterval();
            TCalenSlotInterval targetInterval = NextFocusArea( curInterval, EMoveDirectionDown );
            iSelectedRegion = FindRegion( targetInterval, EMoveDirectionDown );
            if ( iSelectedRegion >= 0 )
                {
                EnterRegionFromAbove();
                }
            else
                {
                iSelectedSlot = NextEmptyFocusSlot( curInterval, EMoveDirectionDown );
                }
            }
            break;
        case EScrollLeft:
            {
            if ( iAlldayEvents.Count() > 0 )
                {
                iSelectedAlldayEvent = iAlldayEvents.Count() - 1;
                }
            else
                {
                // move to previous day
                moved = EFalse;
                }
            }
            break;
        case EScrollRight:
            {
            // move to next day
            moved = EFalse;
            }
            break;
        default:
            // Do nothing
            break;
        }

    return moved;
    }


// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::MoveSelection
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::MoveSelection( TScrollDirection aDirection )
    {
    TBool moved = ETrue;
    StoreOrigSelection();
    if ( iSelectedAlldayEvent >= 0 )
        {
        // focused on an allday event
        moved = MoveInAlldayEvent( aDirection );
        }
    else if ( iSelectedRegion >= 0 )
        {
        // focused on some event region
        moved = MoveInRegion( aDirection );
        }
    else
        {
        // focused on an empty, plain area
        moved = MoveInEmptyArea( aDirection );
        }
    if ( !ValidateSelection() )
        {
        moved = EFalse;
        }
    return moved;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::MoveSelectionInEvent
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::MoveSelectionInEvent( TScrollDirection aDirection )
    {
    ASSERT( IsEventSelected() );

    switch ( aDirection )
        {
        case EScrollUp:
            {
            iSelectedSlot -= KFSCalSlotsInHour;
            }
            break;
        case EScrollDown:
            {
            iSelectedSlot += KFSCalSlotsInHour;
            }
            break;

        default:
            // Do nothing
            break;
        }

    UpdateSelectionInEvent();

    if ( iSelectedAlldayEvent >= 0 )
        {
        // if an allday event is selected, invalidate
        // the old region/event selection
        iSelectedRegion = KErrNotFound;
        }
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::UpdateSelectionInEvent
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::UpdateSelectionInEvent()
    {
    ASSERT( IsEventSelected() );
    TCalenSlotInterval event = SelectedInterval();

    if ( iSelectedSlot < event.iStartSlot )
        {
        iSelectedSlot = event.iStartSlot;
        }
    if ( iSelectedSlot >= event.iEndSlot )
        {
        iSelectedSlot = event.iEndSlot - 1;
        }

    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::SetSelectionInEvent
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::SetSelectionInEvent( TInt aDirection )
    {
    TCalenSlotInterval event = SelectedInterval();
    if ( aDirection > 0 )
        {
        iSelectedSlot = event.iStartSlot;
        }
    else
        {
        iSelectedSlot = event.iEndSlot - 1;
        }
    }


// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::StoreOrigSelection
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::StoreOrigSelection()
    {
    iOrigSelectedAlldayEvent = iSelectedAlldayEvent;
    iOrigSelectedRegion = iSelectedRegion;
    iOrigSelectedColumn = iSelectedColumn;
    iOrigSelectedSlot = iSelectedSlot;
    iOrigSelectedColumnEventIndex = iSelectedColumnEventIndex;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::ValidateSelection
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::ValidateSelection()
    {
    TBool validate( ETrue );
    if ( iSelectedSlot < 0 ||
         iSelectedSlot >= iSlotsInHour*KCalenHoursInDay + iUntimedSlotCount )
        {
        // tried to move outside of the day
        // revert to the original coords
        iSelectedAlldayEvent = iOrigSelectedAlldayEvent;
        iSelectedRegion = iOrigSelectedRegion;
        iSelectedColumn = iOrigSelectedColumn;
        iSelectedSlot = iOrigSelectedSlot;
        iSelectedColumnEventIndex = iOrigSelectedColumnEventIndex;
        validate = EFalse;
        }
    return validate;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::SelectSlot
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::SelectSlot( TInt aSlot )
    {
    // Check for special case: empty untimed slot is selected
    if ( aSlot >= iFirstUntimedSlot &&
         aSlot < iFirstUntimedSlot + iEmptyUntimedSlots )
        {
        aSlot = iFirstUntimedSlot + iEmptyUntimedSlots;
        }


    iSelectedAlldayEvent = KErrNotFound;
    iSelectedSlot = aSlot;
    TCalenSlotInterval interval = EmptySelectionInterval();
    iSelectedRegion = FindRegion( interval, EMoveDirectionDown );
    if ( iSelectedRegion >= 0 )
        {
        TCalenTimeRegion& region = iRegionList[iSelectedRegion];
        TInt regCount( region.iColumns.Count() );
        for ( TInt i = 0; i < regCount; i++ )
            {
            TCalenTimeColumn& column = region.iColumns[i];
            TInt colCount( column.iEventArray.Count() );
            for ( TInt j = 0; j < colCount; j++ )
                {
                if ( column.iEventArray[j].Overlaps( interval ) )
                    {
                    iSelectedColumn = i;
                    iSelectedColumnEventIndex = j;
                    iSelectedSlot = aSlot;
                    UpdateSelectionInEvent();
                    return;
                    }
                }
            }
        // Regions can have empty areas where there aren't any events,
        // due to rounding of the ends of the region to whole hours.
        // A region cannot, however, have a empty complete whole hour.
        // Therefore, this cannot occur.
        User::Invariant();
        }
    }



// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::RegionList
// ---------------------------------------------------------------------------
//
const RArray<TCalenTimeRegion>& CBCSlotInfoStorage::RegionList() const
    {
    return iRegionList;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::GetEventIntervals
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::GetEventIntervals( RArray<TCalenEventInterval>& aArray ) const
    {
    TInt regCount( iRegionList.Count() );
    for ( TInt i = 0; i < regCount; i++ )
        {
        const TCalenTimeRegion& region = iRegionList[i];
        for ( TInt j = 0; j < region.iIntervals.Count(); j++ )
            {
            aArray.Append( region.iIntervals[j] );
            }
        }
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::InsertUntimedEventL
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::InsertUntimedEventL( const CCalInstance& aInstance )
    {
    if ( CalenAgendaUtils::IsTimedEntryL( aInstance.Entry().EntryTypeL() ) )
        {
        // timed entry added as untimed event
        User::Invariant();
        }

    TCalenInstanceId id = TCalenInstanceId::CreateL( aInstance );
    InsertUntimedEvent( aInstance.Entry().EntryTypeL(), id );
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::InsertUntimedEvent
// ---------------------------------------------------------------------------
//
void CBCSlotInfoStorage::InsertUntimedEvent( CCalEntry::TType aType,
                                                  const TCalenInstanceId& aId )
    {
    if ( aType == CCalEntry::ETodo )
        {
        iTodoEvents.Append( aId );
        }
    else
        {
        iUntimedEvents.Append( aId );
        }
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::SuggestedUntimedSlotPos
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::SuggestedUntimedSlotPos()
    {
    TInt slot = KFSCalStartingHour * iSlotsInHour;
    if ( iRegionList.Count() > 0 )
        {
        TCalenTimeRegion& region = iRegionList[0];
        if ( region.iStartSlot < slot )
            {
            slot = RoundHourUp( region.iStartSlot );
            }
        }
    return slot;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::NeededUntimedSlotCount
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::NeededUntimedSlotCount()
    {
    TInt count = iUntimedEvents.Count();
    if ( iTodoEvents.Count() > 0 )
        {
        count++;
        }
    return count;
    }


// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::NeededUntimedSlotCount
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::UpdateUntimedPos( TInt aSlot, TInt aUntimedCount )
    {
    TInt newValue = NeededUntimedSlotCount();

    // If this method is called many times, first undo the previous modifications
    TInt regCount( iRegionList.Count() );
    for ( TInt i = 0; i < regCount; i++ )
        {
        iRegionList[i].AddOffset( -iUntimedSlotCount, iFirstUntimedSlot );
        }
    if ( iSelectedSlot >= iFirstUntimedSlot )
        {
        iSelectedSlot -= iUntimedSlotCount;
        }

    // Reset the untimed slot count
    iUntimedSlotCount = 0;

    // Get the default values
    iFirstUntimedSlot = SuggestedUntimedSlotPos();
    iUntimedSlotCount = newValue;

    // If parameters were given, use them instead of the defaults
    if ( aSlot >= 0 )
        {
        ASSERT( aSlot <= iFirstUntimedSlot );
        ASSERT( iUntimedSlotCount <= aUntimedCount );

        iFirstUntimedSlot = aSlot;
        iUntimedSlotCount = aUntimedCount;
        }

    iEmptyUntimedSlots = iUntimedSlotCount - NeededUntimedSlotCount();

    // Add the new offset to all regions and update the selected slot
    regCount = iRegionList.Count();
    for ( TInt i = 0; i < regCount; i++ )
        {
        iRegionList[i].AddOffset( iUntimedSlotCount, iFirstUntimedSlot );
        }
    if ( iSelectedSlot >= iFirstUntimedSlot )
        {
        iSelectedSlot += iUntimedSlotCount;
        }
    TInt eventCount( iAlldayEvents.Count() );
    for ( TInt i = 0; i < eventCount; i++ )
        {
        iAlldayEvents[i].iEndSlot = iSlotsInHour*KCalenHoursInDay + iUntimedSlotCount;
        }


    // Do rounding of the end of the last region. This should be done when no
    // more events will be added, but there's no real harm if new events are
    // added after this.
    if ( iRegionList.Count() > 0 )
        {
        // Round the end of the last region down
        TCalenTimeRegion& lastRegion = iRegionList[iRegionList.Count() - 1];
        lastRegion.iEndSlot = RoundHourDown( lastRegion.iEndSlot );
        }

    return iFirstUntimedSlot;
    }


// -----------------------------------------------------------------------------
// Returns the index of the first occupied slot
// -----------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::FirstOccupiedSlot()
    {

    TInt firstNonemptySlot ( KErrNotFound );

    if ( iUntimedSlotCount > 0 )
        {
        firstNonemptySlot = iFirstUntimedSlot;
        }
    else
        {
        if ( iRegionList.Count() > 0 )
            {
            TCalenTimeRegion& region = iRegionList[0];
            firstNonemptySlot = region.iStartSlot;
            }
        }

    return firstNonemptySlot;
    }

// -----------------------------------------------------------------------------
// Returns the index of the last occupied slot
// -----------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::LastOccupiedSlot()
    {
    TInt lastNonemptySlot ( KErrNotFound );
    if ( iRegionList.Count() > 0 )
        {
        TInt lastIndex = iRegionList.Count() - 1;
        TCalenTimeRegion& region = iRegionList[lastIndex];
        lastNonemptySlot = region.iEndSlot;
        }
    else
        {
        if ( iUntimedSlotCount > 0 )
            {
            lastNonemptySlot = iFirstUntimedSlot + iUntimedSlotCount -1;
            }
        }
    return lastNonemptySlot;
    }

// -----------------------------------------------------------------------------
// Returns the index of the earliest end slot
// -----------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::EarliestEndSlot()
    {
    TInt earliestEndSlot( KErrNotFound );
    TInt untimedEventCount = iUntimedSlotCount - iEmptyUntimedSlots;
    if ( untimedEventCount > 0 )
        {
        earliestEndSlot = iFirstUntimedSlot + iEmptyUntimedSlots;
        // add 1, since end slot is actually the one that is right after..
        earliestEndSlot++;
        }
    else
        {
        if ( iRegionList.Count() > 0 )
            {
            earliestEndSlot = iEarliestEndSlot + iEmptyUntimedSlots;
            }
        }
    return earliestEndSlot;
    }

// -----------------------------------------------------------------------------
// Returns the index of the last start slot
// -----------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::LastStartSlot()
    {
    TInt lastStartSlot( KErrNotFound );

    if ( iRegionList.Count() > 0 )
        {
        lastStartSlot = iLastStartSlot + iUntimedSlotCount;
        }
    else
        {
        if ( iUntimedSlotCount - iEmptyUntimedSlots > 0 )
            {
            lastStartSlot = iFirstUntimedSlot + iUntimedSlotCount -1;
            }
        }
    return lastStartSlot;
    }



// -----------------------------------------------------------------------------
// Returns the index of a slot where this event should start drawing,
// based on the start time
// -----------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::SlotIndexForStartTime( TTime aStartTime )
    {
    // For example, 1/2 hour accuracy (iSlotsInHour == 2):
    // Start drawing with the previus half hour: 9:15 -> 9:00, 9:59-> 9:30.


    TDateTime dt = aStartTime.DateTime();
    TInt minutes = dt.Minute();
    TInt hours = dt.Hour();

    // calculate index based on the hour.
    TInt slotIndex = hours * iSlotsInHour;

    switch ( iSlotsInHour )
        {
        case EOne: // do nothing
            break;
        case ETwo:
            {
            if ( minutes >= 30)
                {
                slotIndex++;
                }
            }
            break;
        case EThree:
            {
            if ( minutes >= 20 )
                {
                slotIndex++;
                }
            if ( minutes >= 40 )
                {
                slotIndex++;
                }
            }
            break;
        case EFour: // followthrough
        default:
            {
            if ( minutes >= 15 )
                {
                slotIndex++;
                }
            if ( minutes >= 30 )
                {
                slotIndex++;
                }
            if ( minutes >= 45 )
                {
                slotIndex++;
                }

            }
        }

        if ( slotIndex >= iFirstUntimedSlot )
            {
            slotIndex += iUntimedSlotCount;
            }
    return slotIndex;
    }

// -----------------------------------------------------------------------------
// Returns the index of a slot where this event should end drawing,
// based on the end time
// -----------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::SlotIndexForEndTime( TTime aEndTime )
    {
    // For example, 1/2 hour accuracy (iSlotsInHour == 2):
    // End drawing with the next half hour: 9:10 -> 9:30, 9:59-> 10:00.

    TDateTime dt = aEndTime.DateTime();
    TInt minutes = dt.Minute();
    TInt hours = dt.Hour();

    // calculate index based on the hour.
    TInt slotIndex = hours * iSlotsInHour;

    switch ( iSlotsInHour )
        {
        case EOne:
            {
            if ( minutes > 0)
                {
                slotIndex++;
                }
            }

            break;
        case ETwo:
            {
            if ( minutes > 0)
                {
                slotIndex++;
                }
            if ( minutes > 30 )
                {
                slotIndex++;
                }
            }
            break;
        case EThree:
            {
            if ( minutes > 0)
                {
                slotIndex++;
                }
            if ( minutes > 20 )
                {
                slotIndex++;
                }
            if ( minutes > 40 )
                {
                slotIndex++;
                }
            }
            break;
        case EFour: // followthrough
        default:
            {
            if ( minutes > 0)
                {
                slotIndex++;
                }

            if ( minutes > 15 )
                {
                slotIndex++;
                }
            if ( minutes > 30 )
                {
                slotIndex++;
                }
            if ( minutes > 45 )
                {
                slotIndex++;
                }

            }
        }


        if ( slotIndex >= iFirstUntimedSlot )
            {
            slotIndex += iUntimedSlotCount;
            }

    return slotIndex;


    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::IsHourStartSlot
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::IsHourStartSlot( const TInt& aSlotIndex ) const
    {
    TBool isHourStartSlot = EFalse;
    if ( IsExtraSlot( aSlotIndex ) )
        {
        isHourStartSlot = EFalse;
        }
    else
        {
        TInt hourIndex( aSlotIndex );
        if ( aSlotIndex >= iFirstUntimedSlot + iUntimedSlotCount )
            {
            hourIndex -= iUntimedSlotCount;
            }
        TInt hour = hourIndex / iSlotsInHour;
        TInt startIndOfHour = hour * iSlotsInHour;
        if ( hourIndex - startIndOfHour > 0 )
            {
            isHourStartSlot = EFalse;
            }
        else
            {
            isHourStartSlot = ETrue;
            }
        }
    return isHourStartSlot;

    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::IsExtraSlot
// ---------------------------------------------------------------------------
//
TBool CBCSlotInfoStorage::IsExtraSlot( const TInt& aSlotIndex ) const
    {
    return ( aSlotIndex >= iFirstUntimedSlot ) &&
        ( aSlotIndex < ( iFirstUntimedSlot + iUntimedSlotCount ) );
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::HourFromSlotIndex
// Returns the corresponding hour, or KErrNone if index is not hour slot index
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::HourFromSlotIndex( const TInt& aSlotInd ) const
    {

    TInt hour( KErrNotFound );

    if ( !IsExtraSlot( aSlotInd ) )
        {

        if ( aSlotInd < 0 )
            {
            // round downwards, to the previous starting hour
            // e.g. iSlotsInHour = 2, aSlotInd = -1, hour = -1,
            // which by SlotIndexFromHour corresponds to slot -2
            hour = ( aSlotInd - iSlotsInHour + 1 ) / iSlotsInHour;
            }
        else if ( aSlotInd < iFirstUntimedSlot )
            {
            hour = aSlotInd / iSlotsInHour;
            }
        else if ( aSlotInd >= iFirstUntimedSlot + iUntimedSlotCount )
            {
            hour = ( aSlotInd - iUntimedSlotCount ) / iSlotsInHour;
            }
        }
    return hour;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::SlotIndexFromHour
// Returns the corresponding hour, or KErrNone if index is not hour slot index
// ---------------------------------------------------------------------------
//

TInt CBCSlotInfoStorage::SlotIndexFromHour( TInt aHour )
    {

    TInt slotIndex( KErrNotFound );

    if ( aHour >= iFirstUntimedSlot / iSlotsInHour )
        {
        slotIndex = aHour * iSlotsInHour + iUntimedSlotCount;
        }
    else
        {
        slotIndex = aHour * iSlotsInHour;
        }


    return slotIndex;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::RoundHourUp
// Rounds the slot number up (towards earlier hours) to an even hour
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::RoundHourUp( TInt aSlot )
    {
    if ( !IsExtraSlot( aSlot ) )
        {
        aSlot = SlotIndexFromHour( HourFromSlotIndex( aSlot ) );
        }
    return aSlot;
    }

// ---------------------------------------------------------------------------
// CBCSlotInfoStorage::RoundHourDown
// Rounds the slot number down (towards later hours) to an even hour
// ---------------------------------------------------------------------------
//
TInt CBCSlotInfoStorage::RoundHourDown( TInt aSlot )
    {
    if ( !IsExtraSlot( aSlot ) && !IsHourStartSlot( aSlot ) )
        {
        aSlot = SlotIndexFromHour( HourFromSlotIndex( aSlot + iSlotsInHour ) );
        }
    return aSlot;
    }




// ---------------------------------------------------------------------------
// TCalenSlotInterval::Overlaps
// ---------------------------------------------------------------------------
//
TBool TCalenSlotInterval::Overlaps( const TCalenSlotInterval& aInterval ) const
    {
    return aInterval.iStartSlot < iEndSlot && aInterval.iEndSlot > iStartSlot;
    }

// ---------------------------------------------------------------------------
// TCalenSlotInterval::AddOffset
// ---------------------------------------------------------------------------
//
void TCalenSlotInterval::AddOffset( TInt aOffset, TInt aPos )
    {
    if ( iStartSlot >= aPos)
        {
        iStartSlot += aOffset;
        }
    if ( iEndSlot >= aPos)
        {
        iEndSlot += aOffset;
        }
    }

// ---------------------------------------------------------------------------
// TCalenSlotInterval::Union
// ---------------------------------------------------------------------------
//
void TCalenSlotInterval::Union( const TCalenSlotInterval& aInterval )
    {
    if ( !aInterval.IsEmpty() )
        {
        iStartSlot = Min( iStartSlot, aInterval.iStartSlot );
        iEndSlot = Max( iEndSlot, aInterval.iEndSlot );
        }
    }

// ---------------------------------------------------------------------------
// TCalenSlotInterval::Adjacent
// ---------------------------------------------------------------------------
//
TBool TCalenSlotInterval::Adjacent( const TCalenSlotInterval& aInterval ) const
    {
    TBool adjacent = EFalse;
    if ( Overlaps( aInterval ) )
        {
        adjacent = ETrue;
        }
    else
        {
        adjacent = iStartSlot == aInterval.iEndSlot || iEndSlot == aInterval.iStartSlot;
        }
    return adjacent;
    }

// ---------------------------------------------------------------------------
// TCalenSlotInterval::IsEmpty
// ---------------------------------------------------------------------------
//
TBool TCalenSlotInterval::IsEmpty() const
    {
    return iStartSlot >= iEndSlot;
    }

// ---------------------------------------------------------------------------
// TCalenSlotInterval::Intersect
// ---------------------------------------------------------------------------
//
void TCalenSlotInterval::Intersect( const TCalenSlotInterval& aInterval )
    {
    if ( aInterval.IsEmpty() )
        {
        iEndSlot = iStartSlot;
        }
    else
        {
        iStartSlot = Max( iStartSlot, aInterval.iStartSlot );
        iEndSlot = Min( iEndSlot, aInterval.iEndSlot );
        }
    }

// ---------------------------------------------------------------------------
// TCalenSlotInterval::Subtract
// ---------------------------------------------------------------------------
//
void TCalenSlotInterval::Subtract( const TCalenSlotInterval& aInterval,
                                   TCalenSlotInterval& aSecondPart )
    {
    aSecondPart.iStartSlot = aSecondPart.iEndSlot = 0;
    if ( !aInterval.IsEmpty() && Overlaps( aInterval ) )
        {
        if ( aInterval.iStartSlot <= iStartSlot )
            {
            iStartSlot = aInterval.iEndSlot;
            }
        else if ( aInterval.iEndSlot >= iEndSlot )
            {
            iEndSlot = aInterval.iStartSlot;
            }
        else
            {
            aSecondPart.iStartSlot = aInterval.iEndSlot;
            aSecondPart.iEndSlot = iEndSlot;
            iEndSlot = aInterval.iStartSlot;
            }
        }
    }

// ---------------------------------------------------------------------------
// TCalenSlotInterval::operator>
// ---------------------------------------------------------------------------
//
TBool TCalenSlotInterval::operator>( const TCalenSlotInterval& aInterval ) const
    {
    return iStartSlot > aInterval.iStartSlot;
    }



// ---------------------------------------------------------------------------
// TCalenTimeColumn::Close
// ---------------------------------------------------------------------------
//
void TCalenTimeColumn::Close()
    {
    iEventArray.Close();
    }

// ---------------------------------------------------------------------------
// TCalenTimeColumn::AddEvent
// ---------------------------------------------------------------------------
//
void TCalenTimeColumn::AddEvent( const TCalenTimedEventInfo& aEvent )
    {
    ASSERT( aEvent.iStartSlot < aEvent.iEndSlot );
    ASSERT( CanFitEvent( aEvent ) );

    if ( iEventArray.Count() == 0 )
        {
        iStartSlot = aEvent.iStartSlot;
        }
    iEndSlot = aEvent.iEndSlot;
    iEventArray.Append( aEvent );
    }

// ---------------------------------------------------------------------------
// TCalenTimeColumn::CanFitEvent
// ---------------------------------------------------------------------------
//
TBool TCalenTimeColumn::CanFitEvent( const TCalenTimedEventInfo& aEvent )
    {
    return ( aEvent.iStartSlot >= iEndSlot )
        || ( iEventArray.Count() == 0 );
    }

// ---------------------------------------------------------------------------
// TCalenTimeColumn::ContainsEvent
// ---------------------------------------------------------------------------
//
TBool TCalenTimeColumn::ContainsEvent( const TCalenInstanceId& aId )
    {
    TBool contains = EFalse;
    TInt eventCount( iEventArray.Count() );
    for ( TInt i = 0; i < eventCount && !contains; i++ )
        {
        if ( iEventArray[i].iId == aId )
            {
            contains = ETrue;
            }
        }
    return contains;
    }

// ---------------------------------------------------------------------------
// TCalenTimeColumn::AddOffset
// ---------------------------------------------------------------------------
//
void TCalenTimeColumn::AddOffset( TInt aOffset, TInt aPos )
    {
    if ( aOffset != 0 )
        {
        TCalenSlotInterval::AddOffset( aOffset, aPos );
        TInt eventCount( iEventArray.Count() );
        for ( TInt i = 0; i < eventCount; i++ )
            {
            iEventArray[i].AddOffset( aOffset, aPos );
            }
        }
    }


// ---------------------------------------------------------------------------
// TCalenTimeRegion::Close
// ---------------------------------------------------------------------------
//
void TCalenTimeRegion::Close()
    {
    TInt colCount( iColumns.Count() );
    for ( TInt i = 0; i < colCount; i++ )
        {
        iColumns[i].Close();
        }
    iColumns.Close();
    iIntervals.Close();
    }

// ---------------------------------------------------------------------------
// TCalenTimeRegion::Overlaps
// ---------------------------------------------------------------------------
//
TBool TCalenTimeRegion::Overlaps( const TCalenSlotInterval& aInterval ) const
    {
    // the base class implementation would be ok, but we might want the assertion here
    ASSERT( iColumns.Count() > 0 );
    return TCalenSlotInterval::Overlaps( aInterval );
    }

// ---------------------------------------------------------------------------
// TCalenTimeRegion::AddOffset
// ---------------------------------------------------------------------------
//
void TCalenTimeRegion::AddOffset( TInt aOffset, TInt aPos )
    {
    if ( aOffset != 0 )
        {
        TCalenSlotInterval::AddOffset( aOffset, aPos );
        TInt colCount( iColumns.Count() );
        for ( TInt i = 0; i < colCount; i++ )
            {
            iColumns[i].AddOffset( aOffset, aPos );
            }
        TInt intervalCount( iIntervals.Count() );
        for ( TInt i = 0; i < intervalCount; i++ )
            {
            iIntervals[i].AddOffset( aOffset, aPos );
            }
        }

    }

// ---------------------------------------------------------------------------
// TCalenTimeRegion::AddEvent
// ---------------------------------------------------------------------------
//
void TCalenTimeRegion::AddEvent( const TCalenTimedEventInfo& aEvent )
    {
    if ( iColumns.Count() == 0 )
        {
        // This is the first event added to this region
        iStartSlot = aEvent.iStartSlot;
        iEndSlot = aEvent.iEndSlot;
        }
    else
        {
        // Check that the events actually are added in the correct order
        ASSERT( aEvent.iStartSlot >= iStartSlot );
        ASSERT( aEvent.iStartSlot < iEndSlot );

        if ( aEvent.iEndSlot > iEndSlot )
            {
            iEndSlot = aEvent.iEndSlot;
            }
        }

    AddInterval( aEvent );

    // If the event fits in one of the existing columns, add it to that one
    TBool added = EFalse;
    TInt colCount( iColumns.Count() );
    for ( TInt i = 0; i < colCount && !added; i++ )
        {
        if ( iColumns[i].CanFitEvent( aEvent ) )
            {
            iColumns[i].AddEvent( aEvent );
            added = ETrue;
            }
        }

    if ( !added )
        {
        // otherwise create a new column for it
        iColumns.Append( TCalenTimeColumn() );
        iColumns[iColumns.Count() - 1].AddEvent( aEvent );
        }
    }

// ---------------------------------------------------------------------------
// TCalenTimeRegion::AddInterval
// ---------------------------------------------------------------------------
//
void TCalenTimeRegion::AddInterval( const TCalenTimedEventInfo& aEvent )
    {
    /*
     * Here are a few examples of different possible cases for this method.
     * The first picture of each example shows the initial situation in the
     * array, and the new event to be added. The transformations performed
     * are shown with a few intermediate steps.
     *
     * nO = newOverlap, nE = newEvent
     *
     * Example one:
     *
     *               ------------------
     *               | newEvent       |
     * --------------------------------
     * ... | lastOverlap | tail |
     * --------------------------
     *
     *                   --------------
     *                   | newEvent   |
     * --------------------------------
     * ... | lastOverlap | tail |
     * --------------------------
     *
     *                   --------------
     *                   | nO   | nE  |
     * --------------------------------
     * ... | lastOverlap | tail |
     * --------------------------
     *
     * Result:
     * --------------------------------
     * ... | lastOverlap        | nE  |
     * --------------------------------
     *
     *
     * Example two:
     *                          ------------------
     *                          | newEvent       |
     * -------------------------------------------
     * ... | lastOverlap | tail      |
     * -------------------------------
     *
     *                          ------------------
     *                          | nO | newEvent  |
     * -------------------------------------------
     * ... | lastOverlap | tail      |
     * -------------------------------
     *
     * Result:
     * -------------------------------------------
     * ... | lastOverlap | tail | nO | newEvent  |
     * -------------------------------------------
     *
     *
     * Example three:
     *                          --------------
     *                          | newEvent   |
     * -----------------------------------------------------
     * ... | lastOverlap | tail                            |
     * -----------------------------------------------------
     *
     *                          --------------
     *                          | newOverlap |
     * -----------------------------------------------------
     * ... | lastOverlap | tail                            |
     * -----------------------------------------------------
     *
     *                          --------------
     *                          | newOverlap |
     * -----------------------------------------------------
     * ... | lastOverlap | tail |            | tailPart2   |
     * -----------------------------------------------------
     *
     * Result:
     * -----------------------------------------------------
     * ... | lastOverlap | tail | newOverlap | tailPart2   |
     * -----------------------------------------------------
     */



    // Fill out the new event
    TCalenEventInterval newEvent;
    newEvent.iStartSlot = aEvent.iStartSlot;
    newEvent.iEndSlot = aEvent.iEndSlot;
    newEvent.iStatus = aEvent.iStatus;
    newEvent.iReplicationStatus = aEvent.iReplicationStatus;
    newEvent.iOverlap = EFalse;

    // a pointer to the last interval which is an overlap interval
    TCalenEventInterval* lastOverlap = NULL;
    // If nonempty, this is the last interval after the last overlap
    TCalenEventInterval tail;
    tail.iStartSlot = tail.iEndSlot = 0;

    // Find lastOverlap and tail.
    if ( iIntervals.Count() >= 1 )
        {
        // lastInterval is a pointer to the last interval in the array
        TCalenEventInterval* lastInterval = &iIntervals[iIntervals.Count() - 1];

        // If this is an overlap interval, we haven't got any tail.
        if ( lastInterval->iOverlap )
            {
            lastOverlap = lastInterval;
            }
        else
            {
            tail = *lastInterval;
            }

        // If there's at least two intervals, and the last one wasn't an overlap,
        // the second last one must be an overlap.
        if ( iIntervals.Count() >= 2 && !lastOverlap )
            {
            lastOverlap = &iIntervals[iIntervals.Count() - 2];
            ASSERT( lastOverlap->iOverlap );
            }
        }

    // If we got a tail, remove it from the array (it will be readded
    // at the end if needed)
    if ( !tail.IsEmpty() )
        {
        iIntervals.Remove( iIntervals.Count() - 1 );
        }

    TCalenEventInterval empty;
    if ( lastOverlap )
        {
        // Remove the part which already is marked as an overlap
        // from the new event. The new event can't start before the
        // last overlap starts since events are added in order, therefore
        // the second subtraction result interval will remain empty.
        newEvent.Subtract( *lastOverlap, empty );
        ASSERT( empty.IsEmpty() );
        }

    // Create a new interval, representing the overlap between the old tail
    // and the new event
    TCalenEventInterval newOverlap = newEvent;
    newOverlap.iOverlap = ETrue;
    newOverlap.Intersect( tail );

    TCalenEventInterval tailPart2 = tail; // initialize iOverlap and iStatus from tail
    // Remove the new overlap from the old tail, possibly creating two separate intervals.
    tail.Subtract( newOverlap, tailPart2 );

    // If the subtraction only yielded one single interval, but it's
    // after newOverlap, make tailPart2 contain that and make tail empty.
    if ( tail > newOverlap )
        {
        tailPart2 = tail;
        tail.iEndSlot = tail.iStartSlot;
        }



    // Remove the new overlap from the new event. Since we already removed the old
    // overlap, this subtraction can't produce two intervals either.
    newEvent.Subtract( newOverlap, empty );
    ASSERT( empty.IsEmpty() );

    // If the new overlap is adjacent to the old one, expand the old one
    // and set the new overlap to be empty.
    if ( lastOverlap && newOverlap.Adjacent( *lastOverlap ) )
        {
        lastOverlap->Union( newOverlap );
        newOverlap.iEndSlot = newOverlap.iStartSlot;
        }


    // Add all the new intervals, if they're non-empty.
    if ( !tail.IsEmpty() )
        {
        iIntervals.Append( tail );
        }
    if ( !newOverlap.IsEmpty() )
        {
        iIntervals.Append( newOverlap );
        }
    if ( !tailPart2.IsEmpty() )
        {
        iIntervals.Append( tailPart2 );
        }
    if ( !newEvent.IsEmpty() )
        {
        iIntervals.Append( newEvent );
        }

    }



// End of File
