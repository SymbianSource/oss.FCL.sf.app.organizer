/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "calendarui_debug.h"

#include "CalenLunarLocalizer.h"

#include "CalenLunarInfo.h"
#include "CalenLunarLocalizedInfo.h"
#include "CalenExtraRowFormatter.h"
#include <calenregionalutil.rsg>

#include <aknbiditextutils.h>
#include <avkon.rsg>
#include <badesca.h> 
#include <eikenv.h>
#include <StringLoader.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

//CONSTANTS
const TInt KCharMonth = 0x6708;
const TInt KCharFirstMonth = 0x6b63;
const TInt KCharChu = 0x521d;
const TInt KCharTen = 0x5341;
const TInt KCharTwenty= 0x5eff;
_LIT(KChineseDigits, 
     "\x0030\x4e00\x4e8c\x4e09\x56db\x4e94\x516d\x4e03\x516b\x4e5d");

_LIT( KResourceChinesePluginFile, "calenregionalutil.rsc" );
#define KResourcePath KDC_RESOURCE_FILES_DIR


// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizer* CCalenLunarLocalizer::NewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenLunarLocalizer* self = new (ELeave) CCalenLunarLocalizer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::~CCalenLunarLocalizer
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizer::~CCalenLunarLocalizer()
    {
    TRACE_ENTRY_POINT;
    
    delete iFestivalNames;
    delete iSolarTermNames;
    delete iHeavenlyStemNames;
    delete iTerrestialBranchNames;
    delete iAnimalYearNames;
    delete iGregorianDateFormat;
    delete iRowFormatter;
    
    if( iResourceFileOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::CCalenLunarLocalizer
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizer::CCalenLunarLocalizer() : iEikEnv( CEikonEnv::Static() )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::ConstructL
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenLunarLocalizer::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    iRowFormatter = CCalenExtraRowFormatter::NewL();
    
    TFileName dllName;
    // Get the complate path of the DLL from where it is currently loaded
    Dll::FileName( dllName );
    
    TFileName resourceFilename;
    resourceFilename.Append(dllName.Mid(0,2));
    resourceFilename.Append(KResourcePath);
    resourceFilename.Append(KResourceChinesePluginFile);
    BaflUtils::NearestLanguageFile( CEikonEnv::Static()->FsSession(), resourceFilename );
    // Add the resource file.
    iResourceFileOffset = CEikonEnv::Static()->AddResourceFileL( resourceFilename );

    iFestivalNames = iEikEnv->ReadDesCArrayResourceL(R_CALEN_CHI_FESTIVALS);
    iSolarTermNames = iEikEnv->ReadDesCArrayResourceL(R_CALEN_CHI_SOLAR_ITEMS);
    iHeavenlyStemNames = iEikEnv->ReadDesCArrayResourceL(R_CALEN_CHI_HEAV_STEMS);
    iTerrestialBranchNames = iEikEnv->ReadDesCArrayResourceL(R_CALEN_CHI_TERR_BRANCHES);
    iAnimalYearNames = iEikEnv->ReadDesCArrayResourceL(R_CALEN_CHI_ANIMAL_YEARS);

    iGregorianDateFormat = StringLoader::LoadL(R_QTN_DATE_USUAL_WITH_ZERO);


#ifdef _DEBUG
    // Validate assumptions
    TInt maxStem = -1;
    for (TInt i = 0; i < iHeavenlyStemNames->Count(); i++)
        {
        maxStem = Max( maxStem, iHeavenlyStemNames->MdcaPoint(i).Length() );
        }
    
    TInt maxBranch = -1;
    for (TInt i = 0; i < iTerrestialBranchNames->MdcaPoint(i).Length() ; i++)
        {
        maxBranch = Max( maxBranch, iTerrestialBranchNames->MdcaPoint(i).Length() );        
        }
    ASSERT( maxStem + maxBranch <= 100);
#endif
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::LocalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizedInfo* CCalenLunarLocalizer::LocalizeL( TCalenLunarInfo& aInfo )
    {
    TRACE_ENTRY_POINT;
   
    
    CCalenLunarLocalizedInfo* localized = CCalenLunarLocalizedInfo::NewL();
    CleanupStack::PushL( localized );

    if ( aInfo.HasFestival() ) 
        {
        localized->iFestival.Set( iFestivalNames->MdcaPoint( aInfo.iFestival ) );
        }
    
    if ( aInfo.HasSolarTerm() ) 
        {
        localized->iSolarTerm.Set( iSolarTermNames->MdcaPoint( aInfo.iSolarTerm ) );
        }

    // Animal year
    localized->iAnimalYear.Set( iAnimalYearNames->MdcaPoint( aInfo.AnimalYear() ) );
    

    // Lunar year    
    CPtrCArray* yearSubs = new (ELeave) CPtrCArray(2);
    CleanupStack::PushL( yearSubs );
    yearSubs->AppendL( iHeavenlyStemNames->MdcaPoint( aInfo.iHeavenlyStem ) );
    yearSubs->AppendL( iTerrestialBranchNames->MdcaPoint( aInfo.iTerrestialBranch ) );

    HBufC* tmp = StringLoader::LoadLC( R_CALE_LUNAR_YEAR, *yearSubs, iEikEnv );
    RDebug::Print( *tmp );
    localized->iLunarYear = *tmp;
    CleanupStack::PopAndDestroy( tmp );
    CleanupStack::PopAndDestroy( yearSubs );

    RDebug::Print(     localized->iLunarYear  );

    LocalizeMonthAndDayL(localized, aInfo);
    
    // Full lunar date
    CPtrCArray* subs = new (ELeave) CPtrCArray(2);
    CleanupStack::PushL( subs );
    subs->AppendL( localized->LunarMonthAndDay() );
    subs->AppendL( localized->LunarYear() );
    
    tmp = StringLoader::LoadLC( R_CALE_LUNAR_FULL_DATE, *subs );
    localized->iFullLunarDate = *tmp;
    CleanupStack::PopAndDestroy( tmp );
    CleanupStack::PopAndDestroy( subs );
    
    
    aInfo.iGregorianDate.FormatL( localized->iGregorianDate, 
                                 *iGregorianDateFormat );
    CleanupStack::Pop( localized );
    
    
    
    TRACE_EXIT_POINT;
    return localized;
    }


// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::GetExtraRowTextL
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CCalenLunarLocalizer::GetExtraRowTextL( CCalenLunarLocalizedInfo& aLocInfo, TInt aMaxWidth, 
                                              const CFont& aFont, TBool aTwoLines )
    {
    TRACE_ENTRY_POINT;
    
    RArray<CCalenLunarLocalizedInfo::TField> fields;
    CleanupClosePushL( fields );
    fields.AppendL( CCalenLunarLocalizedInfo::EFestival );
    fields.AppendL( CCalenLunarLocalizedInfo::ESolarTerm );
    fields.AppendL( CCalenLunarLocalizedInfo::ELunarMonthAndDay );
    fields.AppendL( CCalenLunarLocalizedInfo::EAnimalYear );
    fields.AppendL( CCalenLunarLocalizedInfo::ELunarYear );
    
    TPtrC text = iRowFormatter->FormatExtraRowInformationL( aLocInfo, fields, aMaxWidth, aFont

                                                            , aTwoLines 

                                                            );
    CleanupStack::PopAndDestroy( &fields );
    
    
    TRACE_EXIT_POINT;
    return text;
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::LocalizeMonthAndDayL
// -----------------------------------------------------------------------------
//
void CCalenLunarLocalizer::LocalizeMonthAndDayL(CCalenLunarLocalizedInfo* aLocInfo,
                                                TCalenLunarInfo& aInfo)
    {
    // Lunar Month and Day
    TInt dateResource = aInfo.iLunarDate.iLeapMonth ? R_CALE_LUNAR_LEAP_DATE : R_CALE_LUNAR_DATE;
    TBuf<10> month;
    TBuf<10> day;
    GetChineseMonth(aInfo.iLunarDate, month);
    GetChineseDay(aInfo.iLunarDate, day);

    CPtrCArray* monthAndDaySubs = new (ELeave) CPtrCArray(2);
    CleanupStack::PushL( monthAndDaySubs );
    monthAndDaySubs->AppendL(month);
    monthAndDaySubs->AppendL(day);
    
    HBufC* tmp = StringLoader::LoadLC( dateResource, *monthAndDaySubs );
    aLocInfo->iLunarMonthAndDay = *tmp;
    CleanupStack::PopAndDestroy( tmp );
    CleanupStack::PopAndDestroy( monthAndDaySubs );
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::GetChineseMonth
// -----------------------------------------------------------------------------
//
void CCalenLunarLocalizer::GetChineseMonth(const TChineseDate& aChineseDate,
                                           TDes& aDes)
    {
    TRACE_ENTRY_POINT;

    if (aChineseDate.iMonth == 1)
        {
        aDes.Append(KCharFirstMonth); // Special character for first month
        }
    else
        {
        GetChineseDigits(aChineseDate.iMonth, aDes);
        }
    
    aDes.Append(KCharMonth);
    
    TRACE_EXIT_POINT;
    }
    
// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::GetChineseDay
// -----------------------------------------------------------------------------
//
void CCalenLunarLocalizer::GetChineseDay(const TChineseDate& aChineseDate,
                                         TDes& aDes)
    {
    TRACE_ENTRY_POINT;
    
    if (aChineseDate.iDay <= 10) // day
        {
        aDes.Append(KCharChu); 
        }
    
    GetChineseDigits(aChineseDate.iDay, aDes);
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::GetChineseDigits
// -----------------------------------------------------------------------------
//
void CCalenLunarLocalizer::GetChineseDigits(TInt aNum, TDes& aDes)
    {
    TRACE_ENTRY_POINT;
    
    ASSERT( 0 < aNum && aNum <= 30 );
    TInt tens = aNum / 10;
    TInt ones = aNum % 10;
    if (aNum < 10) 
        {
        aDes.Append((KChineseDigits())[ones]);
        }
    else if (10 == aNum)
        {
        aDes.Append(KCharTen);
        }
    else if (10 < aNum && aNum < 20)
        {
        aDes.Append(KCharTen);
        aDes.Append((KChineseDigits())[ones]);
        }
    else if (20 == aNum)
        {
        aDes.Append((KChineseDigits())[tens]);
        aDes.Append(KCharTen);
        }
    else if (20 < aNum && aNum < 30)
        {
        aDes.Append(KCharTwenty);
        aDes.Append((KChineseDigits())[ones]);
        }
    else if (30 == aNum)
        {
        aDes.Append((KChineseDigits())[tens]);
        aDes.Append(KCharTen);
        }
    else 
        {
        __ASSERT_DEBUG(EFalse, User::Invariant());
        }
    
    TRACE_EXIT_POINT;
    }

//EOF


