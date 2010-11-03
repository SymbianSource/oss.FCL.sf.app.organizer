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
* Description:  Description:   The model part of preview plugin.
*
*/

#ifndef BCPREVIEWDEFINES_H
#define BCPREVIEWDEFINES_H

/**
 *  Define some const layout data, macro, and so on.
 */
namespace {

// Define macro used in the start of each function.
#define TRACE_ENTRY_POINT
#define TRACE_EXIT_POINT

//
// The max lenght of label string.
//static const TInt KBCNaviLabelSize (32);
static const TInt KMaxItemStringLen (128);
static const TInt KIconArrayCount( 4 );
static const TInt KListboxItemGranularity( 3 );

enum TCalenMaxTime
    {
    ECalenMaxDay = 29,
    ECalenMaxMonth = 11,
    ECalenMaxYear = 2100,
    ECalenMaxSecond = 59,
    ECalenMaxMinute = 59,
    ECalenMaxHour = 23,
    ECalenMaxAnnivYear = 2100
    };

//enum TCalenMinTime
//    {
//    ECalenMinDay = 0,
//    ECalenMinMonth = 0,
//    ECalenMinYear = 1900,
//    ECalenMinSecond = 0,
//    ECalenMinMinute = 0,
//    ECalenMinHour = 0,
//    ECale
//    };

// Define the type of icons for each event type
enum TEventIconType
    {
    /** For MR and Meeting */
    EApptIcon,
    /** For Todo */
    ETodoIcon,
    /** For Memo */
    EMemoIcon,
    /** For Anniversary */
    EAnnivIcon,
    /** For account total type number */
    ETotalIconNumber
    };
}

#endif // BCPREVIEWDEFINES_H

// End of File
