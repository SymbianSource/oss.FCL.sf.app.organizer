/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Storage for single line of data in preview.
*
*/

// INCLUDE FILES
#include "bcpreviewpaneentry.h"

#include <calendateutils.h>

#include <StringLoader.h>
#include <AknBidiTextUtils.h>
#include <PUAcodes.hrh>
#include <finditemengine.h>
#include <calendar.rsg>

namespace  { // unnamed namespace for locals

const TInt KMaxTimePrefixLength = 22 + KAknBidiExtraSpacePerLine;

const TInt KMaxLine = 100;
/**
 * KReplaceWhitespaceChars contains some characters that should be replaced by
 *  space in Calendar popup, day view etc.
 * Following characters are replaced with space
 * \x0009 horizontal tab
 * \x000a new line
 * \x000b line tabulation (vertical
 * \x000c form feed
 * \x000d carriage return
 * \x2028 line separator
 * \x2029 paragraph separator
 */
_LIT( KReplaceWhitespaceChars, "\x0009\x000A\x000B\x000C\x000D\x2028\x2029" );
// not localized separator
_LIT( KHyphenSeparator, " - ");

// Ovi maps URL's
_LIT( KLocUrl1, "http://www.maps.ovi.com/?" );
_LIT( KLocUrl2, "http://maps.ovi.com/services/?" );
// Old type of ovi map URL
_LIT( KLocUrl3, "http://www.ovi.com/maps/l" );
} 

// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::CBCPreviewPaneEntry
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CBCPreviewPaneEntry::CBCPreviewPaneEntry( TRgb aOwnCalenColor ) :
    iEventType(CCalEntry::EAppt), iOwnCalenColor(aOwnCalenColor)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::~CBCPreviewPaneEntry
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CBCPreviewPaneEntry::~CBCPreviewPaneEntry()
    {
    TRACE_ENTRY_POINT;
    delete iStart;
    delete iEnd;
    delete iSummary;
    delete iLocation;
    delete iDescription;
    delete iItemText;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::EventType
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalEntry::TType CBCPreviewPaneEntry::EventType() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iEventType;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::NewL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CBCPreviewPaneEntry* CBCPreviewPaneEntry::NewL(
    CCalInstance* aInstance,
    TRgb aOwnCalenColor,
    const TTime& aDay)
    {
    TRACE_ENTRY_POINT;
    CBCPreviewPaneEntry* self = new (ELeave) CBCPreviewPaneEntry( aOwnCalenColor );

    CleanupStack::PushL( self );
    self->ConstructL(aInstance, aDay);
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::ConstructL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CBCPreviewPaneEntry::ConstructL(CCalInstance* aInstance,
                                    const TTime& aDay)
    {
    TRACE_ENTRY_POINT;
    iEventType = aInstance->Entry().EntryTypeL();

    TInt maxLen = KMaxLine;
    if(aInstance->Entry().EntryTypeL() == CCalEntry::EAppt)
        {
        FormatTimeL( aInstance, aDay, maxLen );
        }
    iItemText = HBufC ::NewLC( maxLen * 3 );
    FormatSummaryL( aInstance, maxLen );
    FormatLocationL( aInstance, maxLen );
    FormatDescriptionL( aInstance, maxLen );
    CleanupStack::Pop(iItemText);
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::FormatTimeL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CBCPreviewPaneEntry::FormatTimeL(CCalInstance* aInstance,
                                     const TTime& aDay,
                                     TInt& aMaxLen)
    {
    TRACE_ENTRY_POINT;

    CCalEntry& apptEntry = aInstance->Entry();
    TTimeIntervalMinutes duration;
    apptEntry.EndTimeL().TimeLocalL().MinutesFrom(
        apptEntry.StartTimeL().TimeLocalL(), duration );

    TTime startTime = aInstance->Time().TimeLocalL();
    TTime endTime = startTime + duration;

    // Format time
    HBufC* timeFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );
    TTime displayTime = CalenDateUtils::DisplayTimeOnDay(startTime, aDay);
    HBufC* startBuf = HBufC::NewLC( KMaxTimePrefixLength );
    TPtr startPtr = startBuf->Des();
    displayTime.FormatL( startPtr, *timeFormat );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( startPtr );

    iStart = startBuf;
    aMaxLen -= startBuf->Length();

    if( startTime != endTime )
        {
        
        aMaxLen -= 1;

        TTime end(TInt64(0));

        TTimeIntervalDays days(aDay.DaysFrom(TTime(TInt64(0))));
        TTimeIntervalDays endDays(endTime.DaysFrom(TTime(TInt64(0))));

        // Set end time to 23:59, if event continues to next day
        if (endDays != days)
            {
            TDateTime endDate = endTime.DateTime();
            endDate.SetHour(ECalenMaxHour);
            endDate.SetMinute(ECalenMaxMinute);
            endTime = endDate;
            }

        HBufC* endBuf = HBufC::NewLC( KMaxTimePrefixLength );
        TPtr endPtr = endBuf->Des();
        endTime.FormatL( endPtr, *timeFormat );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( endPtr );
        iEnd = endBuf;
        aMaxLen -= endBuf->Length();
        CleanupStack::Pop( endBuf );
        }

    CleanupStack::Pop( startBuf );
    CleanupStack::PopAndDestroy( timeFormat );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::FormatSummaryL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CBCPreviewPaneEntry::FormatSummaryL(CCalInstance* aInstance,
                                        TInt& aMaxLen)
    {
    TRACE_ENTRY_POINT;

    CCalEntry& apptEntry = aInstance->Entry();
    HBufC* textBuf = HBufC::NewL(aMaxLen);
    CleanupStack::PushL( textBuf );

    TPtr textPtr = textBuf->Des();

    const TDesC& summary = apptEntry.SummaryL();

    textPtr.Append( summary.Left( aMaxLen ) );
    AknTextUtils::ReplaceCharacters(textPtr, KReplaceWhitespaceChars, TChar(' '));
    textPtr.TrimAll();

    iSummary = textBuf;
    aMaxLen -= textBuf->Length();

    CleanupStack::Pop(textBuf);

    TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::FormatLocationL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CBCPreviewPaneEntry::FormatLocationL(CCalInstance* aInstance,
                                         TInt& aMaxLen)
    {
    TRACE_ENTRY_POINT;

    CCalEntry& apptEntry = aInstance->Entry();
    HBufC* textBuf = HBufC::NewL(aMaxLen);
    CleanupStack::PushL( textBuf );

    TPtr textPtr = textBuf->Des();

    const TDesC& location = apptEntry.LocationL();

    textPtr.Append( location.Left( aMaxLen ) );
    AknTextUtils::ReplaceCharacters(textPtr, KReplaceWhitespaceChars, TChar(' '));
    textPtr.TrimAll();

    iLocation = textBuf;
    aMaxLen -= textBuf->Length();

    CleanupStack::Pop(textBuf);

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::FormatDescriptionL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CBCPreviewPaneEntry::FormatDescriptionL(CCalInstance* aInstance,
                                            TInt& aMaxLen)
    {
    TRACE_ENTRY_POINT;

    CCalEntry& apptEntry = aInstance->Entry();
    HBufC* textBuf = HBufC::NewL(aMaxLen);
    CleanupStack::PushL( textBuf );

    TPtr textPtr = textBuf->Des();

    const TDesC& description = apptEntry.DescriptionL();

    textPtr.Append( description.Left( aMaxLen ) );
    
    TPtrC urlPtr;
    TInt pos = FindLocationUrlL ( textPtr, urlPtr );
    if ( pos != KErrNotFound )
    	 {
		 textPtr.Delete( pos, urlPtr.Length() );
		 textPtr.TrimLeft();
    	 }
    
    AknTextUtils::ReplaceCharacters(textPtr, KReplaceWhitespaceChars, TChar(' '));
    textPtr.TrimAll();

    iDescription = textBuf;
    aMaxLen -= textBuf->Length();

    CleanupStack::Pop(textBuf);

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::FindLocationL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CBCPreviewPaneEntry::FindLocationUrlL( const TDesC& aText, TPtrC& aUrl )
	{

	TInt result = KErrNotFound;
	// Search URL from given text
	CFindItemEngine* itemEngine = CFindItemEngine::NewL(aText,
			CFindItemEngine::EFindItemSearchURLBin);
	CleanupStack::PushL(itemEngine);

	// For each found item
	CFindItemEngine::SFoundItem item;
	itemEngine->Item(item);

	TInt itemCount = itemEngine->ItemCount();
	for (TInt i = 0; i < itemCount; ++i)
		{
		TPtrC url = aText.Mid(item.iStartPos, item.iLength);

		if ( url.Find(KLocUrl1) != KErrNotFound 
				|| url.Find(KLocUrl2) != KErrNotFound 
				|| url.Find(KLocUrl3) != KErrNotFound )
			{ // Location url found
			aUrl.Set( url );
			result = item.iStartPos;
			// Stop iteration
			break;
			}
		else
			{
			itemEngine->NextItem(item);
			}
		}

	CleanupStack::PopAndDestroy(itemEngine);

	return result;
	}
// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::GetStartTimeL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
const TDesC& CBCPreviewPaneEntry::GetTimeL() const
    {
    TRACE_ENTRY_POINT;
    if(this->EventType() == CCalEntry::EAppt)
        {
        iItemText->Des().Copy(*iStart);

        if( iEnd )
        	{
			iItemText->Des().Append( KHyphenSeparator );
        	iItemText->Des().Append(*iEnd);
        	}
        return *iItemText;
        }
    else
    	{
        return KNullDesC();
    	}
        
     TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::GetSummaryLocationAndDescriptionL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
const TDesC& CBCPreviewPaneEntry::GetSummaryLocationAndDescriptionL() const
    {
    TRACE_ENTRY_POINT;
    _LIT(KCOMMA, ", ");
    iItemText->Des().Copy(*iSummary);
    
    if( iLocation->Length() > 0 )
    	{
    	if( iSummary->Length() > 0 )
    		{ 		
    		iItemText->Des().Append( KCOMMA );
    		}
    	iItemText->Des().Append( *iLocation );
    	}
    
    if ( iDescription-> Length() > 0 )
    	{
		if( iSummary->Length() > 0  || iLocation->Length() > 0 )
			{ 		
			iItemText->Des().Append( KCOMMA );
			}
		iItemText->Des().Append( *iDescription );
    	}
    
    TRACE_EXIT_POINT;
    return *iItemText;
    }

// ---------------------------------------------------------------------------
// CBCPreviewPaneEntry::GetOwnCalendarColor
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TRgb CBCPreviewPaneEntry::GetOwnCalendarColor() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iOwnCalenColor;
    }

//  End of File
