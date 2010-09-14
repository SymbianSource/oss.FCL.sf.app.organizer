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

#ifndef __CALENLUNARLOCALIZEDINFO_H__
#define __CALENLUNARLOCALIZEDINFO_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATION
class CCalenLunarLocalizer;

/**
* Class declaration for Lunar localized info
*/ 
class CCalenLunarLocalizedInfo : public CBase
    {
public:
    enum TField
        {
        ELunarFestival,
        ESolarFestival,
        ESolarTerm,
        ELunarMonthAndDay,
        EGregorianDate
        };


public: // public API
    static CCalenLunarLocalizedInfo* NewL();

    virtual ~CCalenLunarLocalizedInfo();

    /**
     * Get specific field. This is useful, when you have ordered array of TFields
     * and you want to process them in a loop.
     * If day didn't had e.g. festival or solar term, KNullDesC is returned for those.
     */
    const TDesC& GetField( TField aField );

    const TDesC& LunarFestival() { return iLunarFestival; }
    const TDesC& SolarFestival() { return iSolarFestival; }
    const TDesC& SolarTerm() { return iSolarTerm; }
    const TDesC& LunarMonthAndDay() { return iLunarMonthAndDay; }
    const TDesC& GregorianDate() { return iGregorianDate; }

private:
    CCalenLunarLocalizedInfo();

    void ConstructL();

private:

    TPtrC iLunarFestival;
    TPtrC iSolarFestival;
    TPtrC iSolarTerm;

    TBuf<50> iLunarMonthAndDay;
    TBuf<50> iGregorianDate;
private:
    friend class CCalenLunarLocalizer;
    };

#endif // __CALENLUNARLOCALIZEDINFO_H__
