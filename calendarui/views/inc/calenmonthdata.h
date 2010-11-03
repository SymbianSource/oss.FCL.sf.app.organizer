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
* Description:   For data of MonthView by one day.
 *
*/


#ifndef     CALENMONTHDATA_H
#define     CALENMONTHDATA_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

//  FORWARD DECLARATIONS
class TCalenMonthData;

//  DATA TYPES
typedef CArrayFixFlat<TCalenMonthData> CMonthDataArray;

//  CLASS DEFINITIONS
/**
 *  For data of month view
 */
NONSHARABLE_CLASS( TCalenMonthData )
    {
public :  // Constructors and destructor
    /**
     * C++ default constructor.
     */
    TCalenMonthData();

    /**
     * C++ constructor.
     */
    TCalenMonthData(TTime aDay);

    /**
     * Destructor.
     */
    virtual ~TCalenMonthData();

public : // New funcitons
    /**
     * return day
     * @return day
     */
    TTime   Day();

    void SetHasEvents(TBool aHasEvents);
    TBool HasEvents();

private :  // New data
    TTime   iDay;
    TBool iHasEvents;
    };

#endif      //  CALENMONTHDATA_H


// End of File
