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
 *                - date utils to help comparisions and calculations witg
 *                  dates and times. 
 *
*/


#ifndef CALENUTILS_H
#define CALENUTILS_H

//  INCLUDES
#include <calcommon.h> // for CalCommon::TCalViewFilter

#include <calentry.h>

// FORWARD DECLARATIONS
class CCalInstance;
class CCalEntryView;
class CCalInstanceView;

// CLASS DECLARATION
NONSHARABLE_CLASS( CalenAgendaUtils )
    {
    public:
        /**
         *
         **/
        IMPORT_C static void CreateEntryIdListForDayL( RPointerArray<CCalInstance>& aList, 
                                                       CCalInstanceView* aInstanceView, 
                                                       const TTime& aDay, 
                                                       const TBool aSortForPopup = EFalse,
                                                       const TBool aIncludeToDos = ETrue );

        /**
         * Finds all events for a day range.
         **/
        IMPORT_C static void FindEventsForDayRangeL( CCalInstanceView* aInstanceView, 
                                                     RPointerArray<CCalInstance>& aList, 
                                                     const CalCommon::TCalViewFilter& aFilter, 
                                                     const TTime& aStartDay,
                                                     const TTime& aEndDay );

        /**
         * Finds all todos for the specified day range.
         **/
        IMPORT_C static void FindTodosForDayRangeL( CCalInstanceView* aInstanceView, 
                                                    RPointerArray<CCalInstance>& aList, 
                                                    const TTime& aStartDay,
                                                    const TTime& aEndDay );

        /**
         * Removes entries ending aDay at midnight.
         **/
        IMPORT_C static void RemoveEntriesEndingAtMidnightL( RPointerArray<CCalInstance>& aList, 
                                                            const TTime& aDay );

        /**
         * Common instance list sort.
         **/
        IMPORT_C static void SortInstanceList( RPointerArray<CCalInstance>& aInstanceList );

        /**
         * Specific sort for calendar popup (EAnniv and EEvent reversed)
         **/
        IMPORT_C static void SortPopupInstanceList( RPointerArray<CCalInstance>& aInstanceList );

        /**
         * Checks that if entry ends at starting midnight of the day,
         * but has started earlier.
         * Such entries are not shown in day that they end, because end time 
         * is just marking the end of previous day. 
         * @returns ETrue, if entry ends at starting midnight of day, but starts before that
         *          EFalse, otherwise
         **/
        IMPORT_C static TBool EndsAtStartOfDayL( CCalInstance* aInstance,
                                                 const TTime& aDay );

        /**
         * @return duration of entry as minutes 
         **/
        IMPORT_C static TTimeIntervalMinutes DurationL( const CCalEntry& aEntry );


        /**
         * Bullet-proof way to read entry time and get real ttime back
         * @return start time for meetings, day notes and anniversaries 
         *         end time (due time) for todos
         *         today 00:00 for undated todos (and for entries with null start time) 
         */
        IMPORT_C static TTime EntryTimeL( const CCalEntry& aEntry );
        
        /** 
         * @return ETrue for appointments and reminders
         *         EFalse otherwise
         */
        IMPORT_C static TBool IsTimedEntryL( CCalEntry::TType aType );

        /**
         * Checks if the entry is repeating
         * @param aEntry : A Calendar Entry
         * @return ETrue if it has an RRule (Meeting/ToDo) or RDate (Meeting)
         * @return EFalse otherwise        
         */
        IMPORT_C static TBool IsRepeatingL( const CCalEntry& aEntry );

         /*
         Returns ETrue if aDes contains only whitespace characters.
         Returns EFalse otherwise
         */
         IMPORT_C static TBool IsEmptyText(const TDesC& aDes);

    private: // class internal utility functions
        /**
         * Common compare for all calendar entries. Order as follows:
         *  1. ETodo     : done/undone, date, priority, last modified(oldest 1st)
         *  2. EEvent    : date/time, last modified(oldest 1st)
         *  3. EAnniv    : date/time, last modified(oldest 1st)
         *  4. EAppt     : start time, duration(shortest 1st), last modified(oldest 1st)
         * (5. EReminder : handled as EAppts )
         **/
        static TInt EntryCompare( const CCalInstance& aInstance1,
                                  const CCalInstance& aInstance2 );

        /**
         * Special order handling for popup
         *  1. ETodo     : done/undone, date, priority, last modified(oldest 1st)
         *  2. EAnniv    : date/time, last modified(oldest 1st)
         *  3. EEvent    : date/time, last modified(oldest 1st)
         *  4. EAppts    : start time, duration(shortest 1st), last modified(oldest 1st)
         * (5. EReminder : handled as EAppts )
         **/
        static TInt PopupEntryCompare( const CCalInstance& aInstance1,
                                       const CCalInstance& aInstance2 );

        static TInt DoPopupEntryCompareL( const CCalInstance& aInstance1,
                                          const CCalInstance& aInstance2 );

        /**
         * Specific entry comparison functions
         **/
        static TInt CompareToDosL( const CCalEntry& aEntry1,
                                   const CCalEntry& aEntry2 );
        static TInt CompareNonTimedNotesL( const CCalInstance& aInstance1,
                                           const CCalInstance& aInstance2 );
        static TInt CompareTimedNotesL( const CCalInstance& aInstance1,
                                        const CCalInstance& aInstance2 );
        static TInt DoEntryCompareL( const CCalInstance& aInstance1,
                                     const CCalInstance& aInstance2 );

        enum TCompareResult
            {
            EEqual       =  0, // 1. == 2.
            ELessThan    = -1, // 1. < 2.
            EGreaterThan =  1  // 1. > 2.
            };

    };

#endif // CALENUTILS_H


// End of File
