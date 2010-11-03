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

//debug
#include "calendarui_debug.h"
#include "CalenLunarLocalizer.h"
#include "CalenLunarInfo.h"
#include "CalenLunarLocalizedInfo.h"
#include "CalenExtraRowFormatter.h"
#include <calenkoreanlunarplugindata.rsg>

//system includes
#include <AknBidiTextUtils.h>
#include <avkon.rsg>
#include <badesca.h>
#include <eikenv.h>
#include <StringLoader.h>

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CCalenLunarLocalizer::NewL()
// ---------------------------------------------------------------------------
//
CCalenLunarLocalizer* CCalenLunarLocalizer::NewL()
    {
    TRACE_ENTRY_POINT;

    CCalenLunarLocalizer* self = new (ELeave) CCalenLunarLocalizer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// CCalenLunarLocalizer::~CCalenLunarLocalizer()
// ---------------------------------------------------------------------------
//
CCalenLunarLocalizer::~CCalenLunarLocalizer()
    {
    TRACE_ENTRY_POINT;

    delete iLunarFestivalNames;
    delete iSolarFestivalNames;
    delete iSolarTermNames;
    delete iGregorianDateFormat;
    delete iRowFormatter;

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarLocalizer::CCalenLunarLocalizer()
// ---------------------------------------------------------------------------
//
CCalenLunarLocalizer::CCalenLunarLocalizer() : iEikEnv( CEikonEnv::Static() )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarLocalizer::ConstructL()
// ---------------------------------------------------------------------------
//
void CCalenLunarLocalizer::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iRowFormatter = CCalenExtraRowFormatter::NewL();

    iLunarFestivalNames = iEikEnv->ReadDesCArrayResourceL(R_CALEN_KOR_FESTIVALS);
    iSolarFestivalNames = iEikEnv->ReadDesCArrayResourceL(R_CALEN_KOR_SOLAR_ANNIVERSARY_ITEMS);
    iSolarTermNames = iEikEnv->ReadDesCArrayResourceL(R_CALEN_KOR_SOLAR_ITEMS);
    iGregorianDateFormat = StringLoader::LoadL(R_QTN_DATE_USUAL_WITH_ZERO);

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarLocalizer::LocalizeL()
// ---------------------------------------------------------------------------
//
CCalenLunarLocalizedInfo* CCalenLunarLocalizer::LocalizeL( TCalenLunarInfo& aInfo )
    {
    TRACE_ENTRY_POINT;

    CCalenLunarLocalizedInfo* localized = CCalenLunarLocalizedInfo::NewL();
    CleanupStack::PushL( localized );

    if ( aInfo.HasLunarFestival() )
        {
        localized->iLunarFestival.Set( iLunarFestivalNames->MdcaPoint( aInfo.iLunarFestival ) );
        }

    if ( aInfo.HasSolarFestival() )
        {
        localized->iSolarFestival.Set( iSolarFestivalNames->MdcaPoint( aInfo.iSolarFestival ) );
        }

    if ( aInfo.HasSolarTerm() )
        {
        localized->iSolarTerm.Set( iSolarTermNames->MdcaPoint( aInfo.iSolarTerm ) );
        }

    LocalizeMonthAndDayL(localized, aInfo);

    aInfo.iGregorianDate.FormatL( localized->iGregorianDate,
                                 *iGregorianDateFormat );
    CleanupStack::Pop( localized );

    TRACE_EXIT_POINT;
    return localized;
    }

// ---------------------------------------------------------------------------
// CCalenLunarLocalizer::GetExtraRowTextL()
// ---------------------------------------------------------------------------
//
TPtrC CCalenLunarLocalizer::GetExtraRowTextL( CCalenLunarLocalizedInfo& aLocInfo, 
        TInt aMaxWidth, const CFont& aFont
#ifdef RD_CALENDAR_PREVIEW
                                              , TBool aTwoLines
#endif // RD_CALENDAR_PREVIEW
                                              )
    {
    TRACE_ENTRY_POINT;

    RArray<CCalenLunarLocalizedInfo::TField> fields;
    CleanupClosePushL( fields );

    fields.AppendL( CCalenLunarLocalizedInfo::ELunarMonthAndDay );  //1st priority
    fields.AppendL( CCalenLunarLocalizedInfo::ESolarFestival );     //2nd priority
    fields.AppendL( CCalenLunarLocalizedInfo::ELunarFestival );     //3rd priority
    fields.AppendL( CCalenLunarLocalizedInfo::ESolarTerm );         //4th priority

    TPtrC text = iRowFormatter->FormatExtraRowInformationL( aLocInfo, fields, aMaxWidth, aFont
#ifdef RD_CALENDAR_PREVIEW
                                                            , aTwoLines
#endif // RD_CALENDAR_PREVIEW
                                                            );
    CleanupStack::PopAndDestroy( &fields );

    TRACE_EXIT_POINT;
    return text;
    }

// ---------------------------------------------------------------------------
// CCalenLunarLocalizer::LocalizeMonthAndDayL()
// ---------------------------------------------------------------------------
//
void CCalenLunarLocalizer::LocalizeMonthAndDayL( CCalenLunarLocalizedInfo* aLocInfo,
        TCalenLunarInfo& aInfo )
    {
    TRACE_ENTRY_POINT;
    
    TInt dateResource = R_CALE_KOR_LUNAR_DATE;    
    HBufC* lunar_indicator_year = StringLoader::LoadLC( R_CALE_KOR_LUNAR_INDICATOR );    
    HBufC* lunar_leap_year = StringLoader::LoadLC( R_CALE_KOR_LEAP_YEAR_INDICATOR );

    TBuf16<2> month;
    TBuf16<2> day;
    HBufC* lunar_indicator_buf = HBufC::NewLC(lunar_indicator_year->Length()+lunar_leap_year->Length());
    lunar_indicator_buf->Des().Append(*lunar_indicator_year);
    TPtr lunar_indicator = lunar_indicator_buf->Des(); 
        
    if ( aInfo.iLunarDate.iLeapMonth )
        {
        lunar_indicator.Append(*lunar_leap_year);
        }

    month.AppendNum( aInfo.iLunarDate.iMonth );
    day.AppendNum( aInfo.iLunarDate.iDay );

    CPtrCArray* monthAndDaySubs = new (ELeave) CPtrCArray(3);
    CleanupStack::PushL( monthAndDaySubs );

    monthAndDaySubs->AppendL( lunar_indicator );
    monthAndDaySubs->AppendL( month );
    monthAndDaySubs->AppendL( day );

    HBufC* tmp = StringLoader::LoadLC( dateResource, *monthAndDaySubs );
    aLocInfo->iLunarMonthAndDay = *tmp;
    CleanupStack::PopAndDestroy( tmp );
    CleanupStack::PopAndDestroy( monthAndDaySubs );
    
    CleanupStack::PopAndDestroy( lunar_indicator_buf );
    CleanupStack::PopAndDestroy( lunar_leap_year );
    CleanupStack::PopAndDestroy( lunar_indicator_year ); 
    
    TRACE_EXIT_POINT;
    }

// End of file
