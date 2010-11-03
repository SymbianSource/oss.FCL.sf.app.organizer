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

#ifndef  CBCSLOTINFOSTORAGE_H
#define  CBCSLOTINFOSTORAGE_H

//  INCLUDES
#include <e32std.h>
#include "calendar.hrh"
#include "caleninstanceid.h"
#include "cbcalendar.hrh" // for TScrollDirection
#include "cbcalendefs.h"

//

/**
 * An interval containing a start and end slot.
 * The start slot belongs to the interval, the end slot
 * is the first slot outside of the interval. If the end slot
 * is before or at the same slot as the start slot, the interval
 * is considered empty.
 */
class TCalenSlotInterval
    {
public:
    /**
     * Check if this interval overlaps the second interval.
     */
    TBool Overlaps( const TCalenSlotInterval& aInterval ) const;

    /**
     * Add aOffset to all slot coordinates later than aPos
     */
    void AddOffset( TInt aOffset, TInt aPos );

    /**
     * Set this interval to be the minimum interval
     * containing both this original interval and aInterval.
     */
    void Union( const TCalenSlotInterval& aInterval );

    /**
     * Check if aInterval lies directly next to this interval.
     */
    TBool Adjacent( const TCalenSlotInterval& aInterval ) const;

    /**
     * Check if this interval is empty.
     */
    TBool IsEmpty() const;

    /**
     * Set this interval to be the area contained in both
     * this interval and to aInterval.
     */
    void Intersect( const TCalenSlotInterval& aInterval );

    /**
     * Remove aInterval from this interval. If aInterval lies
     * within this interval, the result is two separate intervals.
     * This object contains one of them, aSecondPart contains the other one.
     * If the result is just one single interval, this interval contains that
     * and aSecondPart is set to an empty interval.
     */
    void Subtract( const TCalenSlotInterval& aInterval, TCalenSlotInterval& aSecondPart );

    /**
     * Check if this interval starts later than aInterval.
     */
    TBool operator>( const TCalenSlotInterval& aInterval ) const;

    /**
     * The starting slot of the interval. This is the first slot
     * that belongs to the interval.
     */
    TInt iStartSlot;
    /**
     * The ending slot of the interval. This is the first slot
     * that doesn't belong to the interval.
     */
    TInt iEndSlot;
    };


/**
 * Class for storing a calendar instance and the range it occupies.
 */
struct TCalenTimedEventInfo : public TCalenSlotInterval
    {
public:
    /**
     * The id of the calendar instance
     */
    TCalenInstanceId iId;
    /**
     * Status of the entry, needed for setting the displayed color later
     */
    CCalEntry::TStatus iStatus;
    /**
     * Replication status of the entry, needed for setting the displayed color
     * later.
     */
    CCalEntry::TReplicationStatus iReplicationStatus;
    };

/**
 * Class for storing general time intervals and their associated
 * status (needed for displaying the interval).
 */
struct TCalenEventInterval : public TCalenSlotInterval
    {
public:
    /**
     * The status of this interval, if it represents only one calendar
     * instance.
     */
    CCalEntry::TStatus iStatus;
    /**
     * The replication status of this interval, if it represents only one
     * calendar instance.
     */
    CCalEntry::TReplicationStatus iReplicationStatus;
    /**
     * A flag saying that this interval represents a conflict between two or
     * more calendar instances.
     */
    TBool iOverlap;
    };



/**
 * A class containing a sequence of non-overlapping events,
 * visualised as a column.
 */
class TCalenTimeColumn : public TCalenSlotInterval
    {
public:

    /**
     * Explicitly frees the memory used by the event array.
     */
    void Close();

    /**
     * Add a new event to this column. Events must be added sequentially,
     * and must not overlap earlier events in this column.
     */
    void AddEvent( const TCalenTimedEventInfo& aEvent );

    /**
     * Check if a new event can be added to this column.
     */
    TBool CanFitEvent( const TCalenTimedEventInfo& aEvent );

    /**
     * Check if this column contains an event with the id aId.
     */
    TBool ContainsEvent( const TCalenInstanceId& aId );

    /**
     * Add aOffset to all slot coordinates later than aPos
     */
    void AddOffset( TInt aOffset, TInt aPos );

    RArray<TCalenTimedEventInfo> iEventArray;
    };


/**
 * A class containing one or more columns with events,
 * where every event overlaps at least one event in some other
 * column. (Otherwise that event should be added to a separate region.)
 */
class TCalenTimeRegion : public TCalenSlotInterval
    {
public:
    /**
     * Explicitly frees the memory used by data structures.
     */
    void Close();

    /**
     * Check if the given interval overlaps with this region.
     */
    TBool Overlaps( const TCalenSlotInterval& aInterval ) const;

    /**
     * Add an event to this region. Events must be added sequentially,
     * and must overlap this region (unless it's the first event of the region).
     */
    void AddEvent( const TCalenTimedEventInfo& aEvent );

    /**
     * Add aOffset to all slot coordinates later than aPos
     */
    void AddOffset( TInt aOffset, TInt aPos );

private:
    /**
     * Add the event to the bookkeeping of overlapping/nonoverlapping
     * intervals.
     */
    void AddInterval( const TCalenTimedEventInfo& aEvent );
public:

    RArray<TCalenTimeColumn> iColumns;
    RArray<TCalenEventInterval> iIntervals;
    };


/**
 * A container struct, used by the clients of the slot info storage,
 * to provide data in.
 */
struct SCalenApptInfo
    {
    TTime iStartTime;
    TTime iEndTime;
    TBool iAllDay;
    TCalenInstanceId iId;
    CCalEntry::TStatus iStatus;
    CCalEntry::TReplicationStatus iReplicationStatus;
    TBufC<KFSCalMaxDescriptionLength> iSummary;
    TUint32 iColor;
    };


/**
 * Storage class for storing all calendar instances within one day. This
 * class organises the data according to the way it will be needed in the
 * day and week view.
 */
class CBCSlotInfoStorage: public CBase
    {
public:

    enum TSlotsInHour
        {
        EOne = 1,
        ETwo,
        EThree,
        EFour
        };

public:  // Constructors and destructor


    /**
     * Two-phase constructor
     */
    static CBCSlotInfoStorage* NewL( TSlotsInHour aSlotsInHour );
    /**
     * Two-phase constructor
     */
    static CBCSlotInfoStorage* NewLC( TSlotsInHour aSlotsInHour );

    /**
     * Destructor
     */
    virtual ~CBCSlotInfoStorage();


public:     // New functions

    /**
     * Reset the storage, remove all data and set the state back to normal.
     */
    void Reset();

    /**
     * Add a timed event. All timed events must be added in increasing
     * order (sorted by starting time).
     */
    void InsertTimedEvent( const SCalenApptInfo& aItemInfo );
    /**
     * Add an untimed event.
     */
    void InsertUntimedEventL( const CCalInstance& aInstance );
    /**
     * Add an untimed event. (Nonleaving version, useful for testing.)
     */
    void InsertUntimedEvent( CCalEntry::TType aType,
                             const TCalenInstanceId& aId );
    /**
     * Add an allday event.
     */
    void InsertAlldayEvent( const SCalenApptInfo& aItemInfo );

    /**
     * Is the given event allday event
     * @param aStart time to be checked
     * @param aEnd time to be checked
     * @return ETrue if this is allday event, EFalse otherwise
     */
    static TBool IsAlldayEvent( TTime aStart, TTime aEnd );

    /**
     * Is the given event allday event
     * @param aInstance Instance to be checked
     * @return ETrue if this is allday event, EFalse otherwise
     */
    static TBool IsAlldayEvent( const CCalInstance& aInstance );

    /**
     * Return the slot number where this class would insert the
     * untimed events if nothing else is specified.
     */
    TInt SuggestedUntimedSlotPos();
    /**
     * Return how many untimed slots is needed for this day.
     */
    TInt NeededUntimedSlotCount();

    /**
     * Update the indexing to take the current amount of untimed slots
     * into account. This must be called after all untimed events
     * have been added.
     *
     * @param aSlot the slot where the untimed events are to be added.
     *              If negative, uses the default, otherwise aSlot must
     *              be less than or equal to the default position as
     *              returned by SuggestedUntimedSlotPos().
     * @param aUntimedCount the number of slots to insert for untimed events. If
     *                      aSlot is specified, this must be larger or equal
     *                      to NeededUntimedSlotCount().
     */
    TInt UpdateUntimedPos( TInt aSlot = -1, TInt aUntimedCount = 0 );

    /**
     * Return the first slot containing a non-allday event.
     * If this class doesn't contain any data, returns KErrNotFound.
     */
    TInt FirstOccupiedSlot();
    /**
     * Return the last slot containing a non-allday event.
     * If this class doesn't contain any data, returns KErrNotFound.
     */
    TInt LastOccupiedSlot();

    TInt EarliestEndSlot();
    TInt LastStartSlot();


    /**
     * Convert a starting time into a slot index.
     */
    TInt SlotIndexForStartTime( TTime aStartTime );
    /**
     * Convert an ending time into a slot index.
     */
    TInt SlotIndexForEndTime( TTime aStartTime );

    /**
     * Get information about where the item aItemInfo
     * should be displayed. The parameters are filled
     * on return.
     *
     * @param aStartSlot    the first slot of the event
     * @param aEndSlot      the first slot after the event
     * @param aColumnIndex  the column in which this event is located
     * @param aColumns      the total number of columns in the region
     *                      this event belongs to
     */
    void GetLocation( const SCalenApptInfo& aItemInfo,
                      TInt& aStartSlot,
                      TInt& aEndSlot,
                      TInt& aColumnIndex,
                      TInt& aColumns );

    /**
     * Get the number of allday events
     */
    TInt AlldayCount();
    /**
     * Get the number of todo events
     */
    TInt TodoCount();

    /**
     * Check if a slot is the first slot of an hour.
     */
    TBool IsHourStartSlot( const TInt& aSlotIndex ) const;
    /**
     * Check if a slot is an extra slot (for untimed events).
     */
    TBool IsExtraSlot( const TInt& aSlotIndex ) const;
    /**
     * Convert a slot index into a hour
     */
    TInt HourFromSlotIndex( const TInt& aSlotIndex ) const;
    /**
     * Convert a hour into a slot index.
     */
    TInt SlotIndexFromHour( TInt aHour );

    /**
     * Rounds the slot number up (towards earlier hours) to an even hour
     */
    TInt RoundHourUp( TInt aSlot );

    /**
     * Rounds the slot number down (towards later hours) to an even hour
     */
    TInt RoundHourDown( TInt aSlot );

    /**
     * Get the starting slot of the current selection
     */
    void GetSelectedSlot( TInt& aSlot, TInt& aRegion, TInt& aColumnIndex, TInt& aColumns );
    /**
     * Try to move the selection in the given direction
     *
     * @return ETrue if able to move the selection, EFalse if
     *         unable to move (indicating that the selection should move
     *         to the next/previous day).
     */
    TBool MoveSelection( TScrollDirection aDirection );

    /**
     * Move the selected slot within the currently selected event.
     */
    void MoveSelectionInEvent( TScrollDirection aDirection );

    /**
     * Make sure the selected slot within the currently selected event is valid.
     */
    void UpdateSelectionInEvent();

    /**
     * Check if any event currently is selected.
     */
    TBool IsEventSelected() const;
    /**
     * Check if the current selection actually denotes
     * more than one event (the todo event slot is selected,
     * containing more than one todo).
     */
    TBool IsMultipleEventsSelected() const;
    /**
     * Check if an allday event currently is selected.
     */
    TBool IsAlldayEventSelected() const;

    /**
     * Get the instance id of the currently selected event.
     */
    TCalenInstanceId SelectedEvent();

    /**
     * Update the state to make the given calendar instance selected
     *
     * @return KErrNotFound if the instance isn't found, KErrNone otherwise.
     */
    TInt SelectEvent( const TCalenInstanceId& aId );

    /**
     * Get the instance id of an untimed event. Maximally one
     * todo event is counted into this, i.e. aIndex = 1 never returns
     * a todo event even though there are more than one.
     */
    TCalenInstanceId UntimedEvent( TInt aIndex );

    /**
     * Get info about an allday event.
     */
    const TCalenTimedEventInfo& AlldayEvent( TInt aIndex );

    /**
     * Move the selection to the given slot, possibly selecting
     * an event.
     */
    void SelectSlot( TInt aSlot );

    /**
     * Return the list of regions.
     */
    const RArray<TCalenTimeRegion>& RegionList() const;

    /**
     * Get the list of event intervals (for use in week view and ribbon).
     */
    void GetEventIntervals( RArray<TCalenEventInterval>& aArray ) const;

    /**
     * Return the interval which is selected currently.
     */
    TCalenSlotInterval SelectedInterval();
    
    /**
     * Sets selection within a region
     * 
     * @param aRegion Region index.
     * @param aColumn Column index.
     * @param aSlot   Slot number (has to be aligned to full hour).
     */
    TBool SetSelectionInRegion( TInt aRegion, TInt aColumn, TInt aSlot );

private:

    enum TMoveDirection
        {
        EMoveDirectionUp = -1,
        EMoveDirectionDown = 1
        };

    /**
     * C++ default constructor.
     */
    CBCSlotInfoStorage( TSlotsInHour aSlotsInHour );

    /**
     * Second-phase constructor
     */
    void ConstructL();

    /**
     * See if any region overlaps the given interval. Regions are searched
     * in the direction specified by aDirection, e.g. if aDirection < 0,
     * this returns the last overlapping region, if aDirection > 0, returns
     * the first overlapping instead.
     *
     * @return the index of the found overlapping region, or -1 if no
     *         matching region was found.
     */
    TInt FindRegion( const TCalenSlotInterval& aInterval, TInt aDirection );

    /**
     * See if any event overlaps the given interval within the current column.
     * Events are searched in the direction specified by aDirection,
     * e.g. if aDirection < 0, this returns the last overlapping event,
     * if aDirection > 0, returns the first overlapping instead.
     *
     * @return the index within the column of the overlapping event, or -1 if no
     *         matching event was found.
     */
    TInt FindEvent( const TCalenSlotInterval& aInterval, TInt aDirection );

    /**
     * Update the selection state by selecting the first event which ends at
     * the end of the current region.
     */
    void EnterRegionFromBelow();
    /**
     * Update the selection state by selecting the first event which starts
     * at the start of the current region.
     */
    void EnterRegionFromAbove();

    /**
     * Try to move the selection in the given direction, when an
     * empty area is selected.
     *
     * @return ETrue if able to move the selection, EFalse if
     *         unable to move (indicating that the selection should move
     *         to the next/previous day).
     */
    TBool MoveInEmptyArea( TScrollDirection aDirection );
    /**
     * Try to move the selection in the given direction, when the
     * selection is in a region.
     *
     * @return ETrue if able to move the selection, EFalse if
     *         unable to move (indicating that the selection should move
     *         to the next/previous day).
     */
    TBool MoveInRegion( TScrollDirection aDirection );
    /**
     * Try to move the selection in the given direction, when an
     * allday event is selected
     *
     * @return ETrue if able to move the selection, EFalse if
     *         unable to move (indicating that the selection should move
     *         to the next/previous day).
     */
    TBool MoveInAlldayEvent( TScrollDirection aDirection );

    /**
     * Update the selection state by moving from one ordinary event column
     * to another, in the given direction
     */
    void MoveBetweenColumns( TScrollDirection aDirection );

    /**
     * Update the selection state by moving in the given direction
     */
    void MoveInColumn( TInt aDirection );
    /**
     * The selection should be moved out of the current region (in the given
     * direction), update the selection state according to what there is
     * outside of the region.
     */
    void MoveOutFromRegion( TInt aDirection );

    /**
     * Set the selected slot within the current event according to the selection
     * direction.
     */
    void SetSelectionInEvent( TInt aDirection );

    /**
     * Determines how large area to scan for new events/regions when moving in the
     * given direction.
     *
     * I.e., if moving upwards, returns the whole interval from the start of aInterval
     * up to the next slot which can be focused as an empty slot. If aInterval doesn't
     * start on an even hour, the returned interval is from the start of the first whole
     * hour before the interval, to the start of aInterval.
     *
     * If moving downwards, returns the whole interval from the end of aInterval to the
     * end of the next whole hour after the interval.
     *
     */
    TCalenSlotInterval NextFocusArea( const TCalenSlotInterval& aInterval, TInt aDirection );

    /**
     * Return the next slot to focus if moving in the given direction from the interval
     * and focusing an empty slot.
     */
    TInt NextEmptyFocusSlot( const TCalenSlotInterval& aInterval, TInt aDirection );
    /**
     * Return the interval which the current selection state represents, if
     * an empty area is selected.
     */
    TCalenSlotInterval EmptySelectionInterval();

    /**
     * Backup the whole selection state.
     */
    void StoreOrigSelection();
    /**
     * Check if the current selection state is valid, if not, reset it
     * to the backed up copy
     * @return ETrue if selection state is valid, EFalse if not.
     */
    TBool ValidateSelection();


private:    // New data

    RArray<TCalenTimeRegion> iRegionList;
    RArray<TCalenInstanceId> iUntimedEvents;
    RArray<TCalenInstanceId> iTodoEvents;
    RArray<TCalenTimedEventInfo> iAlldayEvents;

    TInt iLastStartSlot;
    TInt iEarliestEndSlot;
    /**
     * The total number of untimed slots.
     */
    TInt iUntimedSlotCount;
    /**
     * The slot index of the first untimed slot.
     */
    TInt iFirstUntimedSlot;
    /**
     * The number of empty untimed slots.
     */
    TInt iEmptyUntimedSlots;
    /**
     * The number of slots per hour.
     */
    TSlotsInHour iSlotsInHour;


    /**
     * Chooses which allday event is selected. If none currently is selected,
     * this is negative. This variable overrides the rest selection variables.
     * If this points to an allday event, the other variables are ignored,
     * except iSelectedSlot which is used for returning to the original place
     * if moving back to the ordinary events.
     */
    TInt iSelectedAlldayEvent;

    /**
     * Chooses which region currently is selected. If none currently is selected,
     * this is negative. If this points to a region, iSelectedColumn and
     * iSelectedColumnEventIndex are taken into account.
     */
    TInt iSelectedRegion;

    /**
     * Chooses which column is selected within the currently selected region.
     * If this is equal to the number of columns, the last, implicit, empty column
     * is selected.
     */
    TInt iSelectedColumn;
    /**
     * Chooses which event is selected within the currently selected column. If
     * none is selected, this is negative.
     */
    TInt iSelectedColumnEventIndex;
    /**
     * Chooses which slot in the day currently is in focus. This must always point
     * to a valid slot. If an event is selected within a column, it points to an
     * slot within that event. If no event is selected, it points to the start
     * of the currently selected empty time region (which is iSlotsInHour slots long).
     * If an allday event is selected, this points to the last selected slot in the
     * ordinary day area.
     */
    TInt iSelectedSlot;

    // copies of the current selection state, to be used for reverting to the original
    // state if the selection is moved to an invalid position
    TInt iOrigSelectedAlldayEvent;
    TInt iOrigSelectedRegion;
    TInt iOrigSelectedColumn;
    TInt iOrigSelectedSlot;
    TInt iOrigSelectedColumnEventIndex;

    };

#endif      //  CBCSlotInfoStorage_H


// End of File
