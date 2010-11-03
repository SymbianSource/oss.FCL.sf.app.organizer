/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
 *
*/

#ifndef CBCALENDEFS_H
#define CBCALENDEFS_H

#include "calenconstants.h"

namespace {

// Default start hour in day and week view
static const TInt KFSCalStartingHour = 8;

// Time slots in one hour, day and week view
static const TInt KFSCalSlotsInHour = 2;
static const TInt KFSCalHourSlotCount = KFSCalSlotsInHour * KCalenHoursInDay;

// Time it takes to complete a scroll operation
static const TInt KLayoutTime = 500;

// Maximum event title length displayed in day and week view.
static const TInt KFSCalMaxDescriptionLength = 100;

// Time text length in day and week view. Space for 12:00 + one for Czech am/pm
static const TInt KDayWeekViewTimeTextLength = 6;

// Length of visual tag
static const TInt KTagTextLength = 64;

// Freestyle calendar mif file
_LIT( KFSCalenMifFile, "z:\\resource\\apps\\businesscalendar.mif");

// Time text length for a string formatted with R_QTN_TIME_USUAL_WITH_ZERO
static const TInt KTimeUsualWithZeroLength = 10;

// Date text lenght for a string formatted with R_QTN_DATE_USUAL
static const TInt KDateUsualWithZeroLength = 15;

}
#endif  // CBCALENDEFS_H


// End of File
