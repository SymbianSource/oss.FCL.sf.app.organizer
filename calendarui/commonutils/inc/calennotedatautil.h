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
* Description:  Helper tool to get alarm and repeat data of Calendar note.
 *
*/

#ifndef     CALENNOTEDATAUTIL_H
#define     CALENNOTEDATAUTIL_H

// INCLUDE
#include <e32base.h>
#include <calcommon.h>
#include <calrrule.h>
//  DATA TYPES
enum TCalenRepeatIndex
    {
    ERepeatNotRepeated,
    ERepeatDaily,
    ERepeatWorkdays,
    ERepeatWeekly,
    ERepeatBiWeekly,
    ERepeatMonthly, // Monthly By Date
    ERepeatYearly,  // Yearly By Date
    ERepeatOther
    };

// FORWARD DECLARATIONS
class CCalEntry;

//  CLASS DEFINITIONS

/**
 *  Helper class for getting Calendar note data
 *  This class is wrapping that casts from Agenda entry to Calendar Note.
 */
NONSHARABLE_CLASS( CalenNoteDataUtil )
    {
public: // New function

    enum TRepeatQueryType
    {
    ESave = 1,
    EDelete,
    EEdit
    };

    /**
    * Return index of repeat type
    * @param : aEntry : Agenda note's entry
    * @return : repeat type index
    **/
    IMPORT_C static TCalenRepeatIndex RepeatIndexL(const CCalEntry& aEntry);

    /**
    * If note has alarm, alarm time is set to aAlarmDateTime
    * and return ETrue. aAlarmDateTime has dd/mm/yy, hh:mm part,
    * E.g. 15/Sep/2002, 13:00.
    * If note has no alarm, aAlarmDateTime is not changed
    * and return EFalse.
    * @param : aEntry : Agenda note's entry
    * @return : ETrue : Note has alarm
    *           EFalse: Note has no alarm
    **/
    IMPORT_C static TBool GetAlarmDateTimeL(const CCalEntry& aEntry, TTime& aAlarmDateTime);

    IMPORT_C static TBool ShowRepeatTypeQueryL( CalCommon::TRecurrenceRange& aAnswer,
                                                       const TRepeatQueryType aType );
    /**
     * @brief To check rule is repeated in "workdays"
     * @param aRrule, a TCalRRule
     * @return ETrue if rule is repeated in "workdays"
     *         else EFalse  
     **/
    IMPORT_C static TBool IsWorkdaysRepeatingL( const TCalRRule aRrule );
    };

#endif // CALENNOTEDATAUTIL_H

// End of file
