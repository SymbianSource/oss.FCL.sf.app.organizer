/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides Solar Term dates for Lunar Calendar. Reads and caches
*                binary data file that lists solar term dates. 
*
*/



//debug
#include "calendarui_debug.h"

#include "calensolarterms.h"

#include "calenlunarpaths.h"

#include <f32file.h>
#include <s32file.h>


// CONSTANTS
_LIT(KSolarTermsFile, "SolarItems");
// search path for solar item file
//Uncomment for emulator
_LIT( KSolarTermsPath, "//private//10005901//" );  

//_LIT( KSolarTermsPath, "//Data//calenlunarchinese//" );
const TInt KFirstSolarTermYear(1900);
const TInt KLastSolarTermYear(2100);


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCalenSolarTerms* CCalenSolarTerms::NewL(RFs& aFs)
    {
    TRACE_ENTRY_POINT;
    
    CCalenSolarTerms* self = new (ELeave) CCalenSolarTerms(aFs);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCalenSolarTerms::~CCalenSolarTerms()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CCalenSolarTerms::CheckSolarTermDateL( const TDateTime& aDate )
    {
    TRACE_ENTRY_POINT;
    
    ReadSolarTermsL( aDate );
    if ( HasSolarTermDataAvailable( aDate ) )
        {
        // Solar festival data is available for this date 
        for (TInt i(0); i < KSolarTermCount; i++)
            {
            TDateTime date = iSolarTermDates[i];
            if (aDate.Month() == date.Month() && aDate.Day() == date.Day())
                {
                // First item in iSolarTermNames is LiChun ("Spring begins")
                // occuring around 4.2 in western year. 
                // It is first solar term in Chinese Lunar year, but it's 
                // third term in western year.
                // 
                // iSolarTermDates list terms from beginning of western year,
                // that's why we subtract 2 and take modulo 24 here, to convert
                // order from 
                // Xiao Han (~ 6.1.), Da Han (~22.1), Li Chun (~4.2)...
                // to 
                // Li Chun (~4.2.), ..., ..., Da Han (~22.1)
                i += KSolarTermCount - 2;
                TInt foundIndex = i % KSolarTermCount;
                
                TRACE_EXIT_POINT;
                return foundIndex;
                }
            }
        TRACE_EXIT_POINT;
        return KErrNotFound;
        }
    else
        {
        // Solar festival data is NOT available for this date 
        TRACE_EXIT_POINT;
        return KErrNotSupported;
        }
    }



// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CCalenSolarTerms::CCalenSolarTerms(RFs& aFs) : iFs( aFs )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CCalenSolarTerms::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
TBool CCalenSolarTerms::HasSolarTermDataAvailable(const TDateTime& aDate) const
    {
    TRACE_ENTRY_POINT;
    
    // Note: day parameter for TDateTime starts from 0, not from 1
    const TDateTime KMinAvailable( KFirstSolarTermYear, EJanuary, 0, 0, 0, 0, 0 );
    const TDateTime KMaxAvailable( KLastSolarTermYear, EDecember, 31 - 1, 23, 59, 59, 0 );
    
    TRACE_EXIT_POINT;
    return TTime(KMinAvailable) <= TTime(aDate) &&
           TTime(aDate) <= TTime(KMaxAvailable);
    }


// ---------------------------------------------------------
// Reads and caches 24 solar term dates for one year
// from "SolarItems" file
// ---------------------------------------------------------
//
void CCalenSolarTerms::ReadSolarTermsL(TDateTime aDate)
    {
    TRACE_ENTRY_POINT;
    
    // Caches one year of solar items
    if ( ! HasSolarTermDataAvailable( aDate ) )
        {
        return;
        }

    TInt year = aDate.Year();

    if (iCachedYear != year)
        {
        RFile file;
        RFs& fs = iFs;
        TFindFile ffile(fs);
        User::LeaveIfError(ffile.FindByDir(KSolarTermsFile, KSolarTermsPath));
        User::LeaveIfError(file.Open(fs,
                                     ffile.File(), EFileRead));
        CleanupClosePushL(file);

        TInt seekPos = 
            (year - KFirstSolarTermYear) * sizeof(TUint8) * KSolarTermCount;

        RFileReadStream readStream( file, seekPos );
        readStream.PushL();

         for (TInt i(0); i < KSolarTermCount; i++)
             {
             TMonth month = static_cast<TMonth>( EJanuary + (i / 2) );
             TInt day = readStream.ReadUint8L();
             TDateTime dt(year, month, day-1, 0, 0, 0, 0);
             iSolarTermDates[i] = dt;
             }

        CleanupStack::PopAndDestroy(2); // readStream, file
        iCachedYear = year;
        }
    
    TRACE_EXIT_POINT;
    }
