/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar Lunar Plugin 
 *
*/


 
#ifndef __CALENLUNARLOCALIZER_H__
#define __CALENLUNARLOCALIZER_H__

#include <e32base.h>
#include <badesca.h>
#include "CalenLunarLocalizedInfo.h"
#include "CalendarVariant.hrh"
#include "calendarui_debug.h"

class CEikonEnv;
class TCalenLunarInfo;
class TChineseDate;
class CFont;
class CCalenExtraRowFormatter;

class CCalenLunarLocalizer : public CBase
    {
public: // public API 
    IMPORT_C static CCalenLunarLocalizer* NewL();

    IMPORT_C virtual ~CCalenLunarLocalizer();
    
    IMPORT_C virtual CCalenLunarLocalizedInfo* LocalizeL( TCalenLunarInfo& aInfo );
    
    IMPORT_C virtual TPtrC GetExtraRowTextL( CCalenLunarLocalizedInfo& aLocInfo, 
                                    TInt aMaxWidth, 
                                    const CFont& aFont,
									TBool aTwoLines
                                  );
    

protected:
    IMPORT_C CCalenLunarLocalizer();

    IMPORT_C void ConstructL();

    TBool TryToFitL( const TDesC& aStr );
    virtual void LocalizeMonthAndDayL(CCalenLunarLocalizedInfo* aLocInfo,
                                      TCalenLunarInfo& aInfo);
private:
    void GetChineseDigits(TInt aNum, TDes& aDes);
    void GetChineseDay(const TChineseDate& aChineseDate, TDes& aDes);
    void GetChineseMonth(const TChineseDate& aChineseDate, TDes& aDes);

protected: // data
    CEikonEnv* iEikEnv;

    /**
     * Localized names of lunar festivals
     * Own.
     */
    CDesCArray* iFestivalNames;
    
    /**
     * Localized names of solar terms 
     * Own.
     */
    CDesCArray* iSolarTermNames;

    /**
     * Localized names of animal years
     * Own.
     */
    CDesCArray* iAnimalYearNames;

    /**
     * Localized names of heavenly stems
     * Own.
     */
    CDesCArray* iHeavenlyStemNames;

    /**
     * Localized names of terrestial branches
     * Own.
     */
    CDesCArray* iTerrestialBranchNames;


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
    
    TInt  iResourceFileOffset;

    };

#endif // __CALENLUNARLOCALIZER_H__
