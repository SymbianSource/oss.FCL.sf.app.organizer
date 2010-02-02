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


#include "calendarui_debug.h"
#include "calenextrarowformatter.h"
#include <calenregionalutil.rsg>
#include <AknBidiTextUtils.h>
#include <badesca.h> 
#include <eikenv.h>
#include <StringLoader.h>


// -----------------------------------------------------------------------------
// CollapseDuplicatesL
// -----------------------------------------------------------------------------
//
void CollapseDuplicatesL( TDes& aStr, TInt aPos, const TDesC& aSub )
    {
    TRACE_ENTRY_POINT;
    
    const TInt sublen = aSub.Length();
    if (aStr.Length() == 0 || sublen == 0)
        {
        return;
        }

    TPtrC remaining = aStr.Mid( aPos );
    TInt fstInRemaining = remaining.Find( aSub );
    
    if ( fstInRemaining >= 0 )
        {
        TInt restPos = fstInRemaining + sublen;
        TPtrC rest = remaining.Mid( restPos );
        TInt sndInRest = rest.Find( aSub );

        // 1) two substrings found in sequence 
        if (sndInRest == 0)
            { 
            // replace second substring with empty string
            TInt fst = aPos + fstInRemaining;
            TInt snd = aPos + restPos + sndInRest;
            aStr.Replace( snd, sublen, KNullDesC);
            // continue collapsing from first 
            CollapseDuplicatesL( aStr, fst, aSub );
            }
        // 2) substring found later in string 
        else if (sndInRest > 0)
            {         
            // continue collapsing from this second substring
            TInt snd = aPos + restPos + sndInRest;
            CollapseDuplicatesL( aStr, snd, aSub );
            }
        // 3) No second substring found -> nothing to collapse
        else             
            {
            TRACE_EXIT_POINT; 
            return;
            }
        }
    // No substring found 
    else
        {
        TRACE_EXIT_POINT;
        return;
        }
    }

// -----------------------------------------------------------------------------
// RemoveLeadingAndTrailingL
// -----------------------------------------------------------------------------
//
void RemoveLeadingAndTrailingL( TDes& aStr, const TDesC& aSub )
    {
    TRACE_ENTRY_POINT;
    
    // Trailing
    const TInt sublen = aSub.Length();
    if ( aStr.Right( sublen ).Find( aSub ) == 0 )
        {
        aStr.Replace( aStr.Length() - sublen, sublen, KNullDesC );
        }

    // Leading 
    if ( aStr.Left( sublen ).Find( aSub ) == 0 )
        {
        aStr.Replace( 0, sublen, KNullDesC );
        }
        
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenExtraRowFormatter::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenExtraRowFormatter* CCalenExtraRowFormatter::NewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenExtraRowFormatter* self = new (ELeave) CCalenExtraRowFormatter;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenExtraRowFormatter::~CCalenExtraRowFormatter
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenExtraRowFormatter::~CCalenExtraRowFormatter()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenExtraRowFormatter::CCalenExtraRowFormatter
// -----------------------------------------------------------------------------
//
CCalenExtraRowFormatter::CCalenExtraRowFormatter()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenExtraRowFormatter::ConstructL
// -----------------------------------------------------------------------------
//
void CCalenExtraRowFormatter::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


// -----------------------------------------------------------------------------
// CCalenExtraRowFormatter::FormatExtraRowInformationL
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CCalenExtraRowFormatter::FormatExtraRowInformationL( 
    CCalenLunarLocalizedInfo& aLocInfo, 
    RArray<CCalenLunarLocalizedInfo::TField>& aPrioritizedFields,
    TInt aMaxWidth,
    const CFont& aFont
    ,TBool aTwoLines
    )
    {
    TRACE_ENTRY_POINT;
    
    
    if ( aPrioritizedFields.Count() == 0)
        {
        iText = KNullDesC;
        
        TRACE_EXIT_POINT;
        return iText;
        }

    // Initialize substring labels
    RArray<CCalenLunarLocalizedInfo::TField> subLabels;
    CleanupClosePushL( subLabels );
    subLabels.AppendL( CCalenLunarLocalizedInfo::EAnimalYear );
    subLabels.AppendL( CCalenLunarLocalizedInfo::ELunarYear );
    subLabels.AppendL( CCalenLunarLocalizedInfo::ELunarMonthAndDay );
    subLabels.AppendL( CCalenLunarLocalizedInfo::EFestival );
    subLabels.AppendL( CCalenLunarLocalizedInfo::ESolarTerm );
    
    // ASSERT that all prioritized fields can be found from subLabels
    for ( TInt i=0; i < aPrioritizedFields.Count(); i++)
        {
        ASSERT( subLabels.Find( aPrioritizedFields[i] ) >= 0 ); 
        }

    TBool fits = EFalse;

    do 
        {
        // Initialize substring array 
        CPtrCArray* subs = new (ELeave) CPtrCArray(10);
        CleanupStack::PushL( subs );
        for ( TInt i = 0; i < subLabels.Count(); i++) 
            {
            subs->AppendL( TPtrC( KNullDesC ) );
            }
        // subs->InsertL( 0, TPtrC( KNullDesC ), 5 );
        
        // Set wanted fields to substring array
        for ( TInt i = 0; i < aPrioritizedFields.Count(); i++)
            {
            CCalenLunarLocalizedInfo::TField field = aPrioritizedFields[i];        
            TInt subIx = subLabels.Find( field );
            // Replace 
            subs->Delete(subIx);
            RDebug::Print( _L("A sub count  %d"), subs->Count() );                       
            subs->InsertL(subIx, TPtrC( aLocInfo.GetField( field ) ) );
            RDebug::Print( _L("B sub count %d"), subs->Count() );                       
            RDebug::Print( _L("B field %S"), &(subs->At(subIx)) );                       
            
            }
        
        // Format all fields to extra row     
        HBufC* extraRowFmt = StringLoader::LoadLC( R_CALE_EXTRA_ROW_LUNAR );
        
        RDebug::RawPrint( *extraRowFmt );
        
        TBuf<1000> fmt = *extraRowFmt;
        for (TInt i=0; i < subLabels.Count(); i++)
            {
            RDebug::Print( _L("Before Format") );
            RDebug::RawPrint( fmt );
            StringLoader::Format( iText, 
                                  fmt,
                                  i + 1, // %0U is a separator 
                                  subs->At( i ) );
            fmt = iText;
            RDebug::Print( _L("After Format") );
            RDebug::RawPrint( fmt );
            }
        
        // Now we have something like "Year of Dog%0U%0U6/11%0U%0U" 
        // First We need to remove multiple occurences of %0U
        _LIT(KSeparatorFmt, "%0U");
        
        CollapseDuplicatesL( iText, 0, KSeparatorFmt );
        RDebug::Print( _L("After collapse") );
        RDebug::RawPrint( iText );

        // Remove leading and trailing %0U
        // By now, we are sure that there is max 1 %0U in the beginning
        // and in the end of string.
        RemoveLeadingAndTrailingL( iText, KSeparatorFmt );
        RDebug::Print( _L("After leading and trailing removal") );
        RDebug::RawPrint( iText );
        

        // If there are now separators anymore, then do not fill them
        TBool hasSeparators = iText.Find( KSeparatorFmt ) >= 0;
        
        if ( hasSeparators ) 
            {
        
            // fill in separators
            HBufC* separator = StringLoader::LoadLC( R_CALE_LUNAR_SEPARATOR );
            fmt = iText;
            StringLoader::Format( iText, 
                                  fmt,
                                  0, // %0U is a separator 
                                  *separator );
        
            RDebug::Print( _L("After separator insert") );
            RDebug::RawPrint( iText );
            CleanupStack::PopAndDestroy( separator );
            }


        CleanupStack::PopAndDestroy( extraRowFmt );
        CleanupStack::PopAndDestroy( subs );
        
        fits = TryToFitL( iText, aMaxWidth, aFont

                          , aTwoLines

                          ); 
        if ( ! fits )
            {
            iText = KNullDesC;
            TInt last = aPrioritizedFields.Count() - 1;
            if ( last >= 0 )
                {
                aPrioritizedFields.Remove( last );
                }
            }

        
        } while ( ! fits && aPrioritizedFields.Count() );

    CleanupStack::PopAndDestroy( &subLabels );
    
    
    
    TRACE_EXIT_POINT;
    return iText;
    }

// -----------------------------------------------------------------------------
// CCalenExtraRowFormatter::TryToFitL
// -----------------------------------------------------------------------------
//
TBool CCalenExtraRowFormatter::TryToFitL( const TDesC& aStr, TInt aMaxWidth, const CFont& aFont
                                           , TBool aTwoLines )
    {
    TRACE_ENTRY_POINT;
    
    TBool result(EFalse);
    if(aTwoLines)
        {
        CArrayFixFlat<TPtrC>* textLines = new(ELeave)CArrayFixFlat<TPtrC>( 3 );
        CleanupStack::PushL( textLines );
        
        CArrayFixFlat<TInt>* lineWidths = new( ELeave )CArrayFixFlat<TInt>( 1 );
        CleanupStack::PushL( lineWidths );
        
        lineWidths->AppendL( aMaxWidth );
        
        HBufC* visualText = AknBidiTextUtils::ConvertToVisualAndWrapToArrayWholeTextL(
            aStr,
            *lineWidths,
            aFont,
            *textLines);

        result = (textLines->Count() <= 2);
            
        CleanupStack::PopAndDestroy( lineWidths );
        CleanupStack::PopAndDestroy( textLines );
        delete visualText;
        }
    else
        {
        CFont::TMeasureTextInput::TFlags logicalOrder = static_cast<CFont::TMeasureTextInput::TFlags>(0);
        TInt textW = AknBidiTextUtils::MeasureTextBoundsWidth( aFont, aStr, logicalOrder );
        result = (textW <= aMaxWidth);
        }   
    
    TRACE_EXIT_POINT;
    return result;
    }

//EOF

