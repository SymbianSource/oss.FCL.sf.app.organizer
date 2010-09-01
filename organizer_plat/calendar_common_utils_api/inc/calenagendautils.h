/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Static utility functions. 
* Date utils to help comparisions and calculations with dates and times. 
*
*/

#ifndef CALENUTILS_H
#define CALENUTILS_H

//  System includes
#include <calcommon.h>
#include <calentry.h>

// Forward declarations
class CCalInstance;
class CCalEntryView;
class CCalInstanceView;

// Class declaration
/**
 * @class CalenAgendaUtils
 * @brief Static utility functions to create/edit/remove entries
 * Also provides other utility functions
 */
NONSHARABLE_CLASS( CalenAgendaUtils )
    {
	public:
		
		/**
		 * @brief Intializes aList with all the particular instance of the entries for a specified day mentioned in aDay
		 *
		 * @param aList The list will contain all the entries for the specified day once the function completes
		 * @param aInstanceView The instance view from which all the instances of the entries will be fetched
		 * @param aDay The day for which the instances of the entries have to be fetched
		 * @param aSortForPopup EFalse by default, does not sort the list. Must be ETrue if the entries need to be sorted by start time
		 * @param aIncludeToDos ETrue by default, includes To-do's also in the list. Must be EFalse if To-do's must be excluded from the list
		 */
        IMPORT_C static void CreateEntryIdListForDayL( RPointerArray<CCalInstance>& aList, 
                                                       CCalInstanceView* aInstanceView, 
                                                       const TTime& aDay, 
                                                       const TBool aSortForPopup = EFalse,
                                                       const TBool aIncludeToDos = ETrue );

		/**
		 * @brief Finds all the events for a specified day range
		 *
		 * @param aInstanceView The instance view from which all the instances of the entries will be fetched
		 * @param aList The list will contain all the entries for the specified day range once the function completes
		 * @param aFilter Mentions the filtering criteria as specified by CalCommon::TCalViewFilter
		 * @param aStartDay The start day
		 * @param aEndDay The end day
		 */
        IMPORT_C static void FindEventsForDayRangeL( CCalInstanceView* aInstanceView, 
                                                     RPointerArray<CCalInstance>& aList, 
                                                     const CalCommon::TCalViewFilter& aFilter, 
                                                     const TTime& aStartDay,
                                                     const TTime& aEndDay );

		/**
		 * @brief Finds all the To-Do's for the specified day range
		 *
		 * @param aInstanceView The instance view from which all the instances of the To-Do's will be fetched
		 * @param aList The list will contain all the To-Do's for the specified day range once the function completes
		 * @param aStartDay The start day
		 * @param aEndDay The end day
		 */
        IMPORT_C static void FindTodosForDayRangeL( CCalInstanceView* aInstanceView, 
                                                    RPointerArray<CCalInstance>& aList, 
                                                    const TTime& aStartDay,
                                                    const TTime& aEndDay );

		/**
		 * @brief Removes all entries from the list, ending at 12:00 am on aDay
		 *
		 * @param aList The list of all the instances from which the entries ending at midnight needs to be removed
		 * @param aDay The day for which the information is requested
		 */
        IMPORT_C static void RemoveEntriesEndingAtMidnightL( RPointerArray<CCalInstance>& aList, 
                                                             const TTime& aDay );

		/**
		 * @brief Sorts the entries in aInstanceList based on start time
		 *
		 * @param aInstanceList Will contain the sorted list of entries once the function completes
		 */
        IMPORT_C static void SortInstanceList( RPointerArray<CCalInstance>& aInstanceList );

		/**
		 * @brief Specific sort for preview popups.
		 * Similiar to SortInstanceList but anniversary and event order is reversed in this case.
		 *
		 * @param aInstanceList Will contain the sorted list of entries once the function completes
		 */
        IMPORT_C static void SortPopupInstanceList( RPointerArray<CCalInstance>& aInstanceList );

		/**
		 * @brief Checks if aInstance ends at starting midnight of the day,
		 * but has started earlier. Such entries are not shown in day that they end, 
		 * because end time is just marking the end of previous day. 
		 *
		 * @param aInstance The instance for which the check needs to be done
		 * @param aDay The day in question
		 * @return TBool ETrue if entry ends at starting midnight of day, but starts before that, EFalse otherwise
		 */
        IMPORT_C static TBool EndsAtStartOfDayL( CCalInstance* aInstance,
                                                 const TTime& aDay );

		/**
		 * @brief Returns the duration of an entry in minutes
		 *
		 * @param aEntry The entry for which duration is required
		 * @return TTimeIntervalMinutes The duration of the entry in minutes
		 */
        IMPORT_C static TTimeIntervalMinutes DurationL( const CCalEntry& aEntry );


		/**
		 * @brief Gets the start time for aEntry
		 *
		 * @param aEntry The entry for which the start time is needed
		 * @return TTime Start time for meetings, day notes and anniversaries,
		 * end time (due time) for todos, today 00:00 for undated todos (and for entries with null start time) 
		 */
        IMPORT_C static TTime EntryTimeL( const CCalEntry& aEntry );
        
		/**
		 * @brief Check if an entry with aType is a timed entry
		 *
		 * @param aType The entry type
		 * @return TBool ETrue if the entry is timed, EFalse otherwise
		 */
        IMPORT_C static TBool IsTimedEntryL( CCalEntry::TType aType );

		/**
		 * @brief Checks if aEntry is repeating
		 *
		 * @param aEntry The entry to be checked
		 * @return ETrue if aEntry is repeating, EFalse otherwise        
		 */
        IMPORT_C static TBool IsRepeatingL( const CCalEntry& aEntry );

		/**
		 *@brief Checks if aDes is empty
		 *
		 * @param aDes The text to check
		 * @return TBool ETrue if aDes contains only whitespace characters, EFalse otherwise
		 */
		IMPORT_C static TBool IsEmptyText( const TDesC& aDes );

    private:
		
		/**
		 * @brief Common compare for all calendar entries. Order as follows:
		 *  1. ETodo     : done/undone, date, priority, last modified(oldest 1st)
		 *  2. EEvent    : date/time, last modified(oldest 1st)
		 *  3. EAnniv    : date/time, last modified(oldest 1st)
		 *  4. EAppt     : start time, duration(shortest 1st), last modified(oldest 1st)
		 *  5. EReminder : handled as EAppts
		 *
		 * @param aInstance1
		 * @param aInstance2
		 * @return TInt One of the TCompareResult values
		 */
        static TInt EntryCompare( const CCalInstance& aInstance1,
                                  const CCalInstance& aInstance2 );

		/**
		 * @brief Special order handling for popup
		 *  1. ETodo     : done/undone, date, priority, last modified(oldest 1st)
		 *  2. EAnniv    : date/time, last modified(oldest 1st)
		 *  3. EEvent    : date/time, last modified(oldest 1st)
		 *  4. EAppts    : start time, duration(shortest 1st), last modified(oldest 1st)
		 *  5. EReminder : handled as EAppts
		 *
		 * @param aInstance1
		 * @param aInstance2
		 * @return TInt One of the TCompareResult values
		 */
        static TInt PopupEntryCompare( const CCalInstance& aInstance1,
                                       const CCalInstance& aInstance2 );

		/**
		 * @brief Special order handling for popup
		 *  1. ETodo     : done/undone, date, priority, last modified(oldest 1st)
		 *  2. EAnniv    : date/time, last modified(oldest 1st)
		 *  3. EEvent    : date/time, last modified(oldest 1st)
		 *  4. EAppts    : start time, duration(shortest 1st), last modified(oldest 1st)
		 *  5. EReminder : handled as EAppts
		 *
		 * @param aInstance1
		 * @param aInstance2
		 * @return TInt One of the TCompareResult values
		 */
        static TInt DoPopupEntryCompareL( const CCalInstance& aInstance1,
                                          const CCalInstance& aInstance2 );

		/**
		 * @brief Common compare for all To-Do's
		 *
		 * @param aEntry1
		 * @param aEntry2
		 * @return TInt One of the TCompareResult values
		 */
        static TInt CompareToDosL( const CCalEntry& aEntry1,
                                   const CCalEntry& aEntry2 );

		/**
		 * @brief Common compare for all non-timed notes
		 *
		 * @param aInstance1
		 * @param aInstance2
		 * @return TInt One of the TCompareResult values
		 */
        static TInt CompareNonTimedNotesL( const CCalInstance& aInstance1,
                                           const CCalInstance& aInstance2 );

		/**
		 * @brief Common compare for all timed notes
		 *
		 * @param aInstance1
		 * @param aInstance2
		 * @return TInt One of the TCompareResult values
		 */
        static TInt CompareTimedNotesL( const CCalInstance& aInstance1,
                                        const CCalInstance& aInstance2 );

		/**
		 * @brief Common compare for all entries
		 *
		 * @param aInstance1
		 * @param aInstance2
		 * @return TInt One of the TCompareResult values
		 */
        static TInt DoEntryCompareL( const CCalInstance& aInstance1,
                                     const CCalInstance& aInstance2 );

		/**
		 * @enum TCompareResult
		 * @brief The different comparison results
		 */
        enum TCompareResult
            {
            EEqual       =  0, // 1. == 2.
            ELessThan    = -1, // 1. < 2.
            EGreaterThan =  1  // 1. > 2.
            };
    };

#endif // CALENUTILS_H

// End of File
