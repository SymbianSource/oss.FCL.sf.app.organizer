/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Storage for single line of data in preview.
 *
*/

//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calenpreviewentry.h"


#include "calenpreviewlabelobserver.h"
#include "calenpreviewentryobserver.h"
#include "calenpreviewlabel.h"

#include <calendateutils.h>
#include "calenpreviewlayoutmanager.h"

#include <StringLoader.h>
#include <AknBidiTextUtils.h>
#include <PUAcodes.hrh>

#include <Calendar.rsg>
#include "calendar.hrh"

const TInt KMaxTimePrefixLength = 22 + KAknBidiExtraSpacePerLine;
const TUint KSpecialHyphen = KPuaCodeShortHyphen;
const TUint KSpecialSpace = KPuaCodeShortSpace;

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

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewEntry::CCalenPreviewEntry() : 
    iStart(NULL), iHyphen(NULL), iEnd(NULL), iSummary(NULL), 
    iLocation(NULL), iDescription(NULL),iText(NULL), iTextLines(NULL),
    iLines(0), iEventType(CCalEntry::EAppt)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewEntry::~CCalenPreviewEntry()
    {
    TRACE_ENTRY_POINT;
    iLabels.Reset();
    if(iTextLines)
        iTextLines->Reset();
    delete iTextLines;
    delete iText;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalEntry::TType CCalenPreviewEntry::EventType() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iEventType;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewEntry::TPreviewEntryType CCalenPreviewEntry::EntryType() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iEntryType;
    }
// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::SetHyphenL()
    {
    TRACE_ENTRY_POINT;
    iHyphen = HBufC::NewL( 1 );
    TPtr bufPtr = iHyphen->Des();
    bufPtr.Append(TChar(KSpecialHyphen));
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
HBufC* CCalenPreviewEntry::FullTextL()
    {
    TRACE_ENTRY_POINT;

    if(iText)
        {
        TRACE_EXIT_POINT;
        return NULL;
        }

    TBuf<1> space; 
    space.Append(TChar(KSpecialSpace));

    HBufC* text = HBufC::NewL(CalculateTextLengthL() + 
                              KAknBidiExtraSpacePerLine * iLines);

    CleanupStack::PushL( text );
    
    TPtr ptr = text->Des();
    if(iStart && iStart->Length())
        ptr.Append(iStart->Des());
    
    if(iHyphen)
        ptr.Append(iHyphen->Des());
    else
    	ptr.Append(space);
    
    if(iEnd && iEnd->Length() && iLines > 1)
        {
        ptr.Append(iEnd->Des());
        ptr.Append(space);
        }
    
    if(iSummary && iSummary->Length())
        ptr.Append(iSummary->Des());
    
    HBufC* separator = StringLoader::LoadLC( R_CALEN_PREVIEW_SEPARATOR );

    if(iLocation && iLocation->Length())
        {
        if(iSummary && iSummary->Length())
            ptr.Append(separator->Des());
        ptr.Append(iLocation->Des());
        }
    if(iDescription && iDescription->Length())
        {
        if( (iSummary && iSummary->Length()) || 
            (iLocation && iLocation->Length()) )
            ptr.Append(separator->Des());
        ptr.Append(iDescription->Des());
        }
    
    if((iSummary && !iSummary->Length())
    	&& (iLocation && !iLocation->Length())
        && (iDescription && !iDescription->Length()))
    	{
    	HBufC* emptytext = StringLoader::LoadLC( R_CALEN_QTN_CALE_NO_SUBJECT );
        	
    	ptr.Append(emptytext->Des());
    
        CleanupStack::PopAndDestroy(emptytext);
    	}
		
    CleanupStack::PopAndDestroy( separator );
    CleanupStack::Pop( text );
    TRACE_EXIT_POINT;
    return text;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::DoLineUsageL(TInt aMaxRows)
    {
    TRACE_ENTRY_POINT;
    // textLines contain 
    if(aMaxRows > 1)
        {
        CArrayFixFlat<TPtrC>* textLines = new(ELeave)CArrayFixFlat<TPtrC>(
            aMaxRows);
        CleanupStack::PushL( textLines );
        
        HBufC* visualText = NULL;
        
        CArrayFixFlat<TInt>* lineWidths = new( ELeave )CArrayFixFlat<TInt>( 2 );
        CleanupStack::PushL( lineWidths );

        switch (iEntryType)
            {
            case ECalenPreviewEntryNormal:
                lineWidths->AppendL( iLayoutManager->FirstLineWidth() );
                lineWidths->AppendL( iLayoutManager->MiddleLineWidth() );
                break;
            case ECalenPreviewEntryPlugin:
                lineWidths->AppendL( iLayoutManager->PluginLineWidth() );
                break;
            case ECalenPreviewEntryEmpty:
            default:
                lineWidths->AppendL( iLayoutManager->EmptyLineWidth() );
                break;
            }
        HBufC* text = FullTextL();
        
        CleanupStack::PushL( text );
        
        visualText = 
            AknBidiTextUtils::ConvertToVisualAndWrapToArrayWholeTextL(
                text->Des(),
                *lineWidths,
                *iLayoutManager->LineFont(),
                *textLines);

        iLines = Min(iLines, textLines->Count());
                
        CleanupStack::PopAndDestroy( text );
        CleanupStack::PopAndDestroy( lineWidths );
        CleanupStack::PopAndDestroy( textLines );
        delete visualText;
        }
    else
        {
        iLines = 1;
        }
    TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::FormatTextL(TBool aShortLine)
    {
    TRACE_ENTRY_POINT;
    
    if(!iLayoutManager)
        {
        TRACE_EXIT_POINT;
        return;
        }

    HBufC* text = FullTextL();
    CleanupStack::PushL( text );

    delete iStart;       iStart = NULL;
    delete iHyphen;      iHyphen = NULL;
    delete iEnd;         iEnd = NULL;
    delete iSummary;     iSummary = NULL;
    delete iLocation;    iLocation = NULL;
    delete iDescription; iDescription = NULL;

    CArrayFixFlat<TInt>* lineWidths = new( ELeave )CArrayFixFlat<TInt>( 
        iLines );
    CleanupStack::PushL( lineWidths );
    
    iTextLines = new(ELeave)CArrayFixFlat<TPtrC>(iLines);

    if( aShortLine )
        lineWidths->AppendL( iLayoutManager->LastLineWidth() );
    else
        {
        if(iEntryType == ECalenPreviewEntryNormal)
            {
            lineWidths->AppendL( iLayoutManager->FirstLineWidth() );
            for(TInt i = 1; i < iLines; i++)
                lineWidths->AppendL( iLayoutManager->MiddleLineWidth() );
            }
        else if(iEntryType == ECalenPreviewEntryEmpty)
            {
            lineWidths->AppendL( iLayoutManager->EmptyLineWidth() );
            for(TInt i = 1; i < iLines; i++)
                lineWidths->AppendL( iLayoutManager->EmptyLineWidth() );
            }
        else
            {
            lineWidths->AppendL( iLayoutManager->PluginLineWidth() );
            for(TInt i = 1; i < iLines; i++)
                lineWidths->AppendL( iLayoutManager->PluginLineWidth() );
            }
        }
    
    TPtr truncatedText = text->Des();
    const TBool KUseEllipsis = ETrue; 

    AknBidiTextUtils::ConvertToVisualAndWrapToArrayL(
        truncatedText,
        *lineWidths,
        *iLayoutManager->LineFont(),
        *iTextLines,
        KUseEllipsis);
    CleanupStack::PopAndDestroy( lineWidths );
    CleanupStack::Pop( text );
	
    iText = text;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCalenPreviewEntry::CalculateTextLengthL()
    {
    TRACE_ENTRY_POINT;

    TBuf<1> space; 
    space.Append(TChar(KSpecialSpace));

    HBufC* separator = StringLoader::LoadLC( R_CALEN_PREVIEW_SEPARATOR );

    TInt result(0);
    if(iStart)
        result += iStart->Length();
    if(iHyphen)
        result += iHyphen->Length();
    else
    	result += space.Length(); 
    
    if(iEnd)
        {
        result += iEnd->Length();
        result += space.Length(); 
        }
    if(iSummary)
        {
        result += iSummary->Length();
        }
    if(iLocation)
        {
    	result += iLocation->Length();
        result += separator->Length();
        }
    if(iDescription)
        {
        result += iDescription->Length();
        result += separator->Length();
        }
    
    if((iSummary && !iSummary->Length())
    	&& (iLocation && !iLocation->Length())
        && (iDescription && !iDescription->Length()))
    	{
    	HBufC* emptytext = StringLoader::LoadLC( R_CALEN_QTN_CALE_NO_SUBJECT );
        	
    	result += emptytext->Length();
    
        CleanupStack::PopAndDestroy(emptytext);
    	}
    
    CleanupStack::PopAndDestroy( separator );
    TRACE_EXIT_POINT;
    return result;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TPtrC CCalenPreviewEntry::GetLine(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iTextLines->At(aIndex);
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCalenPreviewEntry::LineCount()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iLines;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenPreviewEntry::RemoveLine()
    {
    TRACE_ENTRY_POINT;
    TBool ret(EFalse);
    if(iLines > 1)
        {
        iLines--;
        ret = ETrue;
        }
    TRACE_EXIT_POINT;
    return ret;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewEntry* CCalenPreviewEntry::NewL(
    CCalInstance* aInstance,
    CCalenPreviewLayoutManager* aLayoutManager, 
    const TTime& aDay,
    TInt aMaxRows,TUint32 aColor)
    {
    TRACE_ENTRY_POINT;

    CCalenPreviewEntry* self = new (ELeave) CCalenPreviewEntry;

    CleanupStack::PushL( self );
    self->ConstructL(aInstance, aLayoutManager, aDay, aMaxRows,aColor);
    CleanupStack::Pop( self );    

    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewEntry* CCalenPreviewEntry::NewL(
    CCalenPreviewLayoutManager* aLayoutManager,
    TPreviewEntryType aEntryType,TUint32 aColor)
    {
    TRACE_ENTRY_POINT;
    CCalenPreviewEntry* self = new (ELeave) CCalenPreviewEntry;
    CleanupStack::PushL( self );
    self->ConstructL(aLayoutManager, aEntryType,aColor);
    CleanupStack::Pop( self );    
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::ConstructL(CCalenPreviewLayoutManager* aLayoutManager,
                                    TPreviewEntryType aEntryType,TUint32 aColor)
    {
    TRACE_ENTRY_POINT;
    iColor= aColor;
    iLayoutManager = aLayoutManager;
    iLines = 2;
    iEntryType = aEntryType;
    FormatEmptyTextL();
    DoLineUsageL(iLines);
    TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::ConstructL(CCalInstance* aInstance,
                                    CCalenPreviewLayoutManager* aLayoutManager,
                                    const TTime& aDay, 
                                    TInt aMaxRows,
                                    TUint32 aColor)
    {
    TRACE_ENTRY_POINT;
    iColor= aColor;
    iEntryType =  ECalenPreviewEntryNormal;
    iEventType = aInstance->Entry().EntryTypeL();

    iInstanceId = TCalenInstanceId::CreateL( *aInstance );
    iLayoutManager = aLayoutManager;
    TInt maxLen(aMaxRows * KMaxColumnDataLength);

    iLines = aMaxRows;

    if(aInstance->Entry().EntryTypeL() == CCalEntry::EAppt)
        {
        FormatTimeL(aInstance, aDay, maxLen);
        }

    FormatSummaryL(aInstance, maxLen);
    FormatLocationL(aInstance, maxLen);
    FormatDescriptionL(aInstance, maxLen);
    DoLineUsageL(aMaxRows);
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::AddObservedLabelL(CCalenPreviewLabel *aLabel)
    {
    TRACE_ENTRY_POINT;
    iLabels.AppendL(aLabel);
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenPreviewEntry::Contains(TPoint aPoint)
    {
    TRACE_ENTRY_POINT;
    TBool result(EFalse);
    if(aPoint.iX > iLabels[0]->Rect().iTl.iX &&
       aPoint.iY > iLabels[0]->Rect().iTl.iY &&
       aPoint.iX < iLabels[iLabels.Count()-1]->Rect().iBr.iX &&
       aPoint.iY < iLabels[iLabels.Count()-1]->Rect().iBr.iY && 
       iEntryType != ECalenPreviewEntryEmpty)
        result = ETrue;
    TRACE_EXIT_POINT;
    return result;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::HandleLabelPointerEventL(const TPointerEvent& 
                                                  aPointerEvent)
    {
    TRACE_ENTRY_POINT;
    if(AknLayoutUtils::PenEnabled())
        {
        switch(aPointerEvent.iType)
            {
            case TPointerEvent::EButton1Down:
                for(TInt i = 0; i < iLabels.Count(); i++)
                    {
                    iLabels[i]->HandleVisualisationL(
                        CCalenPreviewLabel::EAddVisualisation);
                    }
                if(iObserver)
                    iObserver->HandlePreviewEntryEventL(iInstanceId, 
                                                        aPointerEvent.iType);
                break;
            case TPointerEvent::EButton1Up:
                for(TInt i = 0; i < iLabels.Count(); i++)
                    {
                    iLabels[i]->HandleVisualisationL(
                        CCalenPreviewLabel::ERemoveVisualisation);
                    }
                if(iObserver)
                    iObserver->HandlePreviewEntryEventL(iInstanceId, 
                                                        aPointerEvent.iType);
                break;
            case TPointerEvent::EDrag:
                if(Contains(aPointerEvent.iPosition))
                    {
                    if(iObserver)
                        iObserver->HandlePreviewEntryEventL(iInstanceId, 
                                                            aPointerEvent.iType);
                    for(TInt i = 0; i < iLabels.Count(); i++)
                        {
                        iLabels[i]->HandleVisualisationL(
                            CCalenPreviewLabel::EAddVisualisation);
                        }
                    }
                break;
                }
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::SetEntryObserver(
    MCalenPreviewEntryObserver* aObserver)
    {
    TRACE_ENTRY_POINT;
    iObserver = aObserver;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::FormatTimeL(CCalInstance* aInstance,
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

    TBuf<1> hyphen;
    hyphen.Append(TChar(KSpecialHyphen));

    if( startTime != endTime )
        {
        SetHyphenL();
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
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::FormatSummaryL(CCalInstance* aInstance,
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
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::FormatLocationL(CCalInstance* aInstance,
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
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::FormatDescriptionL(CCalInstance* aInstance,
                                            TInt& aMaxLen)
    {
    TRACE_ENTRY_POINT;
    
    CCalEntry& apptEntry = aInstance->Entry();
    HBufC* textBuf = HBufC::NewL(aMaxLen);
    CleanupStack::PushL( textBuf );
    
    TPtr textPtr = textBuf->Des();
    
    const TDesC& description = apptEntry.DescriptionL();
    
    textPtr.Append( description.Left( aMaxLen ) );
    AknTextUtils::ReplaceCharacters(textPtr, KReplaceWhitespaceChars, TChar(' '));
    textPtr.TrimAll();

    iDescription = textBuf;
    aMaxLen -= textBuf->Length();
    
    CleanupStack::Pop(textBuf);

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewEntry::FormatEmptyTextL()
    {
    TRACE_ENTRY_POINT;
    
    HBufC* emptyText = StringLoader::LoadLC(R_CALEN_QTN_CALE_NO_EVENTS);
    iDescription = emptyText;
    CleanupStack::Pop(emptyText);

    TRACE_EXIT_POINT;
    }


//  End of File
