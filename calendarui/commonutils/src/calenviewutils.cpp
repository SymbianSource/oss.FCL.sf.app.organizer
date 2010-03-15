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
* Description:  Implementation of View Utils
*
*/


#include <calenviewutils.h>
#include <calendateutils.h>
#include <Calendar.rsg>
#include <AknUtils.h>
#include <calentry.h>
#include <calalarm.h>
#include <calenagendautils.h>
#include <CalenInterimUtils2.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <Calendar.rsg>
#include <AknQueryDialog.h>
#include <calinstance.h>
#include <calsession.h>
#include <akntoolbar.h>
#include <aknappui.h>

#include "calendarui_debug.h"           // Debug.
#include "calendar.hrh"

_LIT( KWesternSummaryMarker, "\x200E" );
_LIT( KArabicSummaryMarker , "\x200F" );
/* KReplaceWhitespaceChars contains some characters that should be replaced by
 * space in Calendar popup, day view etc. Characters are:
 * \x0009 horizontal tab
 * \x000a new line
 * \x000b line tabulation (vertical
 * \x000c form feed
 * \x000d carriage return
 * \x2028 line separator
 * \x2029 paragraph separator
 */
_LIT( KReplaceWhitespaceChars, "\x0009\x000A\x000B\x000C\x000D\x2028\x2029" );
_LIT( KWesternSummaryLocationSeparator, ", " );
_LIT( KArabicSummaryLocationSeparator, " \x060c" );

// ----------------------------------------------------------------------------
// CalenViewUtils::ShowDateErrorNoteL
// Show Error note of out of range of date
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CalenViewUtils::ShowDateOutOfRangeErrorNoteL()
    {
    TRACE_ENTRY_POINT;

    HBufC* buf = StringLoader::LoadLC( R_QTN_CALE_INFO_YEAR_LIMIT );
    TPtr ptr = buf->Des();
    // Message contains year numbers, which have to be converted
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
    CAknInformationNote* dialog = new( ELeave )CAknInformationNote();
    dialog->ExecuteLD( *buf );
    CleanupStack::PopAndDestroy( buf );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CalenViewUtils::GetSummaryLocationTextL
// Fills a string with a more displayable text representation of the summary
// and location fields from an entry. The number of characters to fill in the
// target string must be specified. The function replaces problem characters
// with spaces and trims whitespace. See KReplaceWhitespaceChars for replaced
// characters.
// ----------------------------------------------------------------------------
//
EXPORT_C void CalenViewUtils::GetSummaryLocationTextL( 
        CCalEntry& aEntry, 
        TDes& aTarget, 
        TInt aMaxLength )
    {
    TRACE_ENTRY_POINT;

    // Every Copy and Append has to ensure that MaxLength is not exceed.
    const TInt max = aMaxLength;
    TInt freeSpace = max - 2; // freeSpace is recalculated after each update

    const TDesC& summary = aEntry.SummaryL();    
    //const TDesC& location = aEntry.LocationL();    
    const TDesC& westernSeparator = KWesternSummaryLocationSeparator;
    const TDesC& arabicSeparator =  KArabicSummaryLocationSeparator;
    
    //location string is not available for anniversary and todo
    HBufC16* text = HBufC16::NewLC( max );    
    TPtrC location = text->Des();
    if ( CCalEntry::ETodo != aEntry.EntryTypeL())
        {
            location.Set(aEntry.LocationL());
        }
    

    if(AknLayoutUtils::LayoutMirrored())
        {
        aTarget.Append(KArabicSummaryMarker);
        }
    else
        {
        aTarget.Append(KWesternSummaryMarker);
        }


    // "Summary"
    aTarget.Append( summary.Left( freeSpace ) );
    freeSpace = max - aTarget.Length();

    // If both summary and location exists, add separator ", " in between.
    if ( summary.Length() > 0 && location.Length() > 0 && freeSpace > 0 )
        {
        if ( User::Language() == ELangArabic )
            {
            aTarget.Append( arabicSeparator.Left( freeSpace ) );
            }
        else
            {
            aTarget.Append( westernSeparator.Left( freeSpace ) );
            }
        freeSpace = max - aTarget.Length();
        }

    // "Location". It's added even if summary is empty
    if( freeSpace > 0 )
		    {
		    aTarget.Append( location.Left( freeSpace ) );
		    }

    freeSpace = max - aTarget.Length();

    AknTextUtils::ReplaceCharacters(aTarget, KReplaceWhitespaceChars, TChar(' '));
    freeSpace = max - aTarget.Length();

    // Take away extra whitespace
    aTarget.TrimAll();
    freeSpace = max - aTarget.Length();

    // Set "<unnamed>" string for text, if there's nothing in buffer so far
    //if ( aTarget.Length() == 0 )
    if ( aEntry.SummaryL().Length()  == 0 && location.Length() == 0 )
        {
        HBufC* emptytext = StringLoader::LoadLC( R_CALEN_QTN_CALE_NO_SUBJECT );
        aTarget.Copy( emptytext->Left( freeSpace ) );
        freeSpace = max - aTarget.Length();
        CleanupStack::PopAndDestroy(emptytext);
        }
    CleanupStack::PopAndDestroy( text );
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CalenNoteDataUtil::DateQueryL
// Prompts the user for a time. Requires a title resource id, or pass 0
// for default title "Date:".
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CalenViewUtils::DateQueryL( TTime& aDate, TInt aPromptId )
    {
    TRACE_ENTRY_POINT;
    if ( aPromptId == 0 )
        {
        aPromptId = R_CALEN_DATE_PROMPT;
        }

    CAknTimeQueryDialog* dlg = CAknTimeQueryDialog::NewL( aDate,
                                                          CAknQueryDialog::ENoTone );
    CleanupStack::PushL(dlg);

    HBufC* prompt = StringLoader::LoadLC( aPromptId, CEikonEnv::Static() );
    dlg->SetPromptL( *prompt );
    CleanupStack::PopAndDestroy( prompt );

    CleanupStack::Pop(); // dlg

    TRACE_EXIT_POINT;
    return dlg->ExecuteLD( R_CALEN_DAYQUERY_NOTE );
    }

// ---------------------------------------------------------------------------
// CalenViewUtils::IsAllDayEvent
// Allday event is an event  with a duration of n*24h.
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CalenViewUtils::IsAlldayEventL( const CCalInstance& aInstance )
    {
    
    TRACE_ENTRY_POINT;

    TBool allDayEvent( EFalse );
    CCalEntry& entry = aInstance.Entry();
    if ( CCalEntry::EAppt == entry.EntryTypeL()
         ||CCalEntry::EEvent == entry.EntryTypeL()  )
        {    
        TCalTime startTime = entry.StartTimeL();
        TCalTime stopTime  = entry.EndTimeL();
    
        TTime startLocalTime = startTime.TimeLocalL();
        TTime stopLocalTime = stopTime.TimeLocalL();
        TTimeIntervalDays differenceInTime = stopLocalTime.DaysFrom(startLocalTime); // fix for AllDayEntry issue
        if( startLocalTime != stopLocalTime && 
                startLocalTime == CalenDateUtils::BeginningOfDay( startLocalTime ) && 
                stopLocalTime == CalenDateUtils::BeginningOfDay( stopLocalTime ) && 
                startLocalTime != stopLocalTime && differenceInTime.Int() == 1 ) // fix for AllDayEntry issue
            {
            allDayEvent = ETrue;
            }
        else
            {
            allDayEvent = EFalse;
            }
        }
    
    TRACE_EXIT_POINT;
    return allDayEvent;
    }

// End of file.
