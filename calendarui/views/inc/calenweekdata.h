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
* Description:  Week view hour's data definition
 *
*/


#ifndef     CALENWEEKDATA_H
#define     CALENWEEKDATA_H

//  INCLUDES
#include "calenweekhour.h"

#include <calenconstants.h>
#include <calinstance.h>
#include <e32base.h>

//  DATA TYPES

NONSHARABLE_STRUCT( SDisplayData )
    {
    TBuf<10> iHourDes;  // 10 should be sufficient for all locales...
    TCalenWeekHour iHourData[KCalenDaysInWeek];
    };

// ListBox cell order
enum TWeekLBCellOrder
    {
    ECalenWeekHour,
    ECalenWeekFirstDay,
    ECalenWeekSecondDay,
    ECalenWeekThirdDay,
    ECalenWeekForthDay,
    ECalenWeekFifthDay,
    ECalenWeekSixthDay,
    ECalenWeekSeventhDay
    };

#endif      //  CALENWEEKDATA_H


// End of File
