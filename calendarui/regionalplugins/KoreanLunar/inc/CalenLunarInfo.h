/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description : Class looking after alarm fields for forms.
*
*/

#ifndef __CALENLUNARINFO_H__
#define __CALENLUNARINFO_H__

// INCLUDES
#include <e32def.h>
#include <calendarconverter.h>

/**
* Class declaration for Lunar info
*/ 
class TCalenLunarInfo
    {
public:
    enum TLunarFestival
        {
        ENoFestival = KErrNotFound,
        EFestivalNewYearDay,
        EFestivalBuddhaBirthDay,
        EFestivalHarvest,
        EFestivalFullMoonDay,
        EFestival3rdMarchDay,
        EFestivalDano,
        EFestivalUnknown_1,
        EFestival7thJulyDay,
        EFestivalUnknown_2,
        EFestivalUnknown_3,
        EFestival105thDayAfterWS    
        };

    enum TSolarFestival
        {
        ENoSolarFestival = KErrNotFound,
        ESolarFestivalNewYearDay,
        ESolarFestivalIndependenceMovement,
        ESolarFestivalChildrensDay,
        ESolarFestivalMemorialDay,
        ESolarFestivalConstitutionDay,
        ESolarFestivalLiberationDay,
        ESolarFestivalFoundationDay,
        ESolarFestivalChristmasDay,
        ESolarFestivalHangulDay
        };

    typedef TInt TSolarTerm;

public:
    TBool HasLunarFestival();
    TBool HasSolarFestival();
    TBool HasSolarTerm();

public:
        /**
        * Lunar Festival.
        */
    TLunarFestival  iLunarFestival;
        /**
        * Lunar Festival.
        */
    TSolarFestival  iSolarFestival;
        /**
        * Lunar Festival.
        */
    TSolarTerm      iSolarTerm;
        /**
        * Lunar Festival.
        */
    TChineseDate    iLunarDate;
        /**
        * Lunar Festival.
        */
    TTime           iGregorianDate;
    };


#endif // __CALENLUNARINFO_H__
