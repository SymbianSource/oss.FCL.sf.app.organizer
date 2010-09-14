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

#ifndef __CALENLUNARLOCALIZER_H__
#define __CALENLUNARLOCALIZER_H__

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "CalenLunarLocalizedInfo.h"
#include "CalendarVariant.hrh"

// FORWARD DECLARATION
class CEikonEnv;
class TCalenLunarInfo;
class TChineseDate;
class CFont;
class CCalenExtraRowFormatter;

/**
* Class declaration for Lunar localizer
*/ 
class CCalenLunarLocalizer : public CBase
    {
public: // public API
    static CCalenLunarLocalizer* NewL();

    virtual ~CCalenLunarLocalizer();

    virtual CCalenLunarLocalizedInfo* LocalizeL( TCalenLunarInfo& aInfo );

    virtual TPtrC GetExtraRowTextL( CCalenLunarLocalizedInfo& aLocInfo,
                                    TInt aMaxWidth,
                                    const CFont& aFont
#ifdef RD_CALENDAR_PREVIEW
                                    , TBool aTwoLines
#endif // RD_CALENDAR_PREVIEW
                                    );

protected:
    CCalenLunarLocalizer();

    void ConstructL();

    TBool TryToFitL( const TDesC& aStr );
    virtual void LocalizeMonthAndDayL(CCalenLunarLocalizedInfo* aLocInfo,
                                      TCalenLunarInfo& aInfo);
private:

protected: // data
    CEikonEnv* iEikEnv;

    /**
     * Localized names of lunar festivals
     * Own.
     */
    CDesCArray* iLunarFestivalNames;

    /**
     * Localized names of solar festivals
     * Own.
     */
    CDesCArray* iSolarFestivalNames;

    /**
     * Localized names of solar terms
     * Own.
     */
    CDesCArray* iSolarTermNames;

    /**
     * Format string for western date.
     * Own.
     */
    HBufC* iGregorianDateFormat;

    TBuf<1000> iLunarExtraRowText;


    /**
     * Language independent formatter of extra row information.
     */
    CCalenExtraRowFormatter* iRowFormatter;

    };

#endif // __CALENLUNARLOCALIZER_H__
