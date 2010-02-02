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
* Description: List Box Model class for Calendar Day view  
 *
*/

#include <calencommands.hrh>            // Calendar commands
#include <AknBidiTextUtils.h>
#include <AknUtils.h>
#include <eikenv.h>
#include <PUAcodes.hrh>
#include <StringLoader.h>
#include <txtrich.h>
#include <calentry.h>
#include <calinstance.h>
#include <calrrule.h>
#include <calalarm.h>
#include <centralrepository.h>
#include <Calendar.rsg>
#include <calsession.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenviewutils.h>
#include <calenagendautils.h>
#include "CalendarVariant.hrh"
#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION
#include <calendateutils.h>
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION
#include <calcalendarinfo.h>
#include <calenservices.h>

//debug
#include "calendarui_debug.h"
#include "calendaylistboxmodel.h"
#include "calencontroller.h"
#include "CalendarVariant.hrh"
#include "calendaylistbox.h"
#include "calentodoview.h" // for priorities
#include "calendar.hrh"
#include "CalendarPrivateCRKeys.h"
#include "calenglobaldata.h"

// LOCAL CONSTANTS AND MACROS
_LIT(KDesTimeSpace, " ");
_LIT(KDesSeparator, "\t");
_LIT(KIconSeperatorFormat, "%d\t"); // Day LB icon seperator format

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION
_LIT(KUnicodeRightwardsArrow, "\x2192"); // ->
_LIT(KUnicodeLeftwardsArrow, "\x2190");  // <-
_LIT(K24hFmt12am, " 00"); // already starts at hr 00
_LIT(K24hFmt12pm, "24 ");
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION


const TInt KTimedNoteMaxLines(4);
const TInt KHypenBufSize(4);
const TInt KNonTimedNoteMaxLines(1);

// KMaxColumnDataLength is Avkon defined minimal maximum length for column texts.
const TInt KMaxLengthOfDisplayedText(KTimedNoteMaxLines * KMaxColumnDataLength);

// =========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CCalenDayListBoxModel::CCalenDayListBoxModel
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CCalenDayListBoxModel::CCalenDayListBoxModel()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDayListBoxModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCalenDayListBoxModel::ConstructL( const CCalenDayContainer* aDayContainer )
    {
    TRACE_ENTRY_POINT;

    iDayContainer = aDayContainer;
    iItemTextArray = new(ELeave) CDesCArrayFlat(5);

    // Read time formats
    iTimeFormat = StringLoader::LoadL( R_QTN_TIME_USUAL_WITH_ZERO );

    TRACE_EXIT_POINT;
    }

void CCalenDayListBoxModel::UpdateLayoutValues( TInt aLayoutVariant,
TBool aMidnightVisualization)
    {
    TRACE_ENTRY_POINT;

    CCalenDayListBox::ReadLayout(
        aLayoutVariant,
        iLocale,
        iLaf.iGfxA,
        iLaf.iTextA,
        iLaf.iTextB,
        iLaf.iTextC,
        iLaf.iIconA,
        iLaf.iIconB,
        aMidnightVisualization );

    TRACE_EXIT_POINT;
    }

// Destructor
CCalenDayListBoxModel::~CCalenDayListBoxModel()
    {
    TRACE_ENTRY_POINT;

    delete iTimeFormat;
    delete iItemTextArray;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDayListBoxModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCalenDayListBoxModel* CCalenDayListBoxModel::NewL( const CCalenDayContainer* aDayContainer )
    {
    TRACE_ENTRY_POINT;

    CCalenDayListBoxModel* self =
        new( ELeave ) CCalenDayListBoxModel();

    CleanupStack::PushL( self );
    self->ConstructL( aDayContainer );
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenDayListBoxModel::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CDesCArrayFlat* CCalenDayListBoxModel::ItemTextArray()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iItemTextArray;
    }

// ----------------------------------------------------------------------------
// CCalenDayListBoxModel::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayListBoxModel::GetEntryTextDataL(CCalEntry& aEntry,
                                              TDes& aBuf) const
    {
    TRACE_ENTRY_POINT;

    const TInt maxLen( aBuf.MaxLength() - KTimedNoteMaxLines * KAknBidiExtraSpacePerLine );
    CalenViewUtils::GetSummaryLocationTextL( aEntry, aBuf, maxLen );

    TRACE_EXIT_POINT;
    }

#ifndef RD_CALEN_MIDNIGHT_VISUALIZATION
// ----------------------------------------------------------------------------
// CCalenDayListBoxModel::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayListBoxModel::LimitStartAndEndTime(TTime& aCurrentDay,
                                                 TTime& aStart,
                                                 TTime& aEnd)
    {
    TRACE_ENTRY_POINT;

    TTimeIntervalDays days(aCurrentDay.DaysFrom(TTime(TInt64(0))));
    TTimeIntervalDays startDays(aStart.DaysFrom(TTime(TInt64(0))));
    TTimeIntervalDays endDays(aEnd.DaysFrom(TTime(TInt64(0))));

    // set start time to 00:00, if event starts before current day
    if (startDays != days)
        {
        aStart = TTime(TInt64(0)) + startDays;
        }

    // Set end time to 23:59, if event continues to next day
    if (endDays != days)
        {
        TDateTime endDate = aEnd.DateTime();
        endDate.SetHour(ECalenMaxHour);
        endDate.SetMinute(ECalenMaxMinute);
        aEnd = endDate;
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDayListBoxModel::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayListBoxModel::InsertTimeL(TDes& aBuf,
                                        const TTime& aTime)
    {
    TRACE_ENTRY_POINT;

    TBuf<32> timeDes;
    aTime.FormatL( timeDes, *iTimeFormat );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( timeDes );
    AknBidiTextUtils::ConvertToVisualAndClipL( timeDes,
                                               *iLaf.iTextA.Font(),
                                               iLaf.iTextA.TextRect().Width(),
                                               iLaf.iTextA.TextRect().Width() );
    aBuf.Append(timeDes);

    TRACE_EXIT_POINT;
    }

#else // Midnight is ON

void CCalenDayListBoxModel::InsertTimeL(TDes& aBuf, const TTime& aTime, const TTimeStringFormat aInsert)
    {
    TRACE_ENTRY_POINT;

    TBuf<32> timeDes;
    switch(aInsert)
        {
            case EInsertTimeGiven:
                aTime.FormatL( timeDes, *iTimeFormat );
                break;

            case EInsert00:
                timeDes = K24hFmt12am();
                break;

            case EInsert24:
                timeDes = K24hFmt12pm();
                break;

            default:
                ASSERT(KErrArgument);
        }


    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( timeDes );
    AknBidiTextUtils::ConvertToVisualAndClipL( timeDes,
                                               *iLaf.iTextA.Font(),
                                               iLaf.iTextA.TextRect().Width(),
                                               iLaf.iTextA.TextRect().Width() );
    aBuf.Append(timeDes);

    TRACE_EXIT_POINT;
    }


#endif // RD_CALEN_MIDNIGHT_VISUALIZATION


// ----------------------------------------------------------------------------
// CCalenDayListBoxModel::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION
void CCalenDayListBoxModel::InsertHyphenL(TDes& aBuf,
                                               const TTime& aStart,
                                               const TTime& aEnd)

#else // keep the old definition
void CCalenDayListBoxModel::InsertHyphen(TDes& aBuf,
                                               const TTime& aStart,
                                               const TTime& aEnd)
#endif //RD_CALEN_MIDNIGHT_VISUALIZATION
    {
    TRACE_ENTRY_POINT;

    if (aStart != aEnd)
        {

        TBuf<KHypenBufSize> hyphen;
        hyphen.Append(TChar(KPuaCodeNarrowDash));

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION

        AknBidiTextUtils::ConvertToVisualAndClipL( hyphen,
                                              *iLaf.iTextB.Font(),
                                               iLaf.iTextB.TextRect().Width(),
                                               iLaf.iTextB.TextRect().Width() );
        aBuf.Append(hyphen);
#else
        aBuf.Append(hyphen);
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayListBoxModel::LastLineWidth
// Calculates a last line width
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCalenDayListBoxModel::LastLineWidthL(TInt aWidth, CCalEntry& aEntry) const
    {
    TRACE_ENTRY_POINT;

    CCalAlarm* alarmObj = aEntry.AlarmL();
    TBool alarm( alarmObj ? ETrue : EFalse );
    delete alarmObj;

    TCalRRule ignore;
    TBool repeat(CalenAgendaUtils::IsRepeatingL(aEntry));

    TBool priority( EFalse );

    if( aEntry.EntryTypeL() == CCalEntry::ETodo &&
        aEntry.PriorityL() != ETodoPriorityNormal )
        {
        priority = ETrue;
        }
    
    // Check if this entry has any geovalues for the location field
    CCalGeoValue* geovalue = aEntry.GeoValueL();  
    TBool mapIcon( geovalue ? ETrue : EFalse);
    delete geovalue;
    
    TInt width( aWidth );

    // FIXME, actually these should be read from LAF!

    if( alarm && repeat || alarm && priority || repeat && priority || mapIcon && alarm || mapIcon && repeat || mapIcon && priority)
        {
        width = aWidth -( iLaf.iIconB.Rect().Width() + iLaf.iIconA.Rect().Width() );
        }
    else if( alarm || repeat || priority || mapIcon)
        {
            width = aWidth - iLaf.iIconA.Rect().Width();
        }
    width = width - KAknBidiExtraSpacePerLine;

    TRACE_EXIT_POINT;
    return width;
    }

// ---------------------------------------------------------
// CCalenDayListBoxModel::WrapTextL
// (other items were commented in a header).
// ---------------------------------------------------------
//
HBufC* CCalenDayListBoxModel::WrapTextL(
    CCalEntry& aEntry,
    CArrayFixFlat<TPtrC>& aLineArray) const
    {
    TRACE_ENTRY_POINT;

    // This is rather complicated algorithm to calculate how many
    // lines given text takes.
    //
    // (Note: in below description of problem and solution we for simplicity
    // consider only meeting entries with maximum of 4 lines of text, but
    // code handles Todos, Day Events and Anniversaries with max 1 line
    // of text. It should be easy to adapt it to have different maximum number
    // of lines.)
    //
    // As UI spec says, textual representation of entry
    // (subject + location) takes as many lines as it needs.
    // If it doesn't fit to maximum number of lines (4), it is
    // truncated.
    //
    // Problem is that last line of entry might contain less space for
    // text than previous lines, as it can contain icons for alarm, repeat
    // and priority. Because Avkon APIs for text wrapping and truncating
    // cannot handle situation, in which line count is not defined AND
    // last line length varies, we have to programmatically try
    // to wrap and fit text first to 1 line, then to 2 lines, then to
    // 3 lines and finally truncate it to 4 lines.

    TBool isTimed =
        aEntry.EntryTypeL() == CCalEntry::EAppt ||
        aEntry.EntryTypeL() == CCalEntry::EReminder;
    const TInt KMaxLines =
        isTimed ? KTimedNoteMaxLines : KNonTimedNoteMaxLines;

    const TInt KLineWidth = iLaf.iTextC.TextRect().Width();

    CArrayFixFlat<TInt>* lineWidths = new( ELeave )CArrayFixFlat<TInt>( 5 );
    CleanupStack::PushL( lineWidths );

    // last line width is always dependent on icons and is thus calculated.
    // We insert last line width to line widths buffer here, and try to fit
    // text to this single line. After that we always add line with normal
    // width BEFORE this last line.
    // I.e.
    //  1st try: { LastLineWidth }
    //  2nd try: { NormalLineWidth, LastLineWidth }
    //  3rd try: { NormalLineWidth, NormalLineWidth, LastLineWidth }
    //  ...
    lineWidths->AppendL( LastLineWidthL( KLineWidth, aEntry ) );

    // Get entry text
    HBufC* text = HBufC::NewLC( KMaxLengthOfDisplayedText );
    TPtr textPtr = text->Des();
    GetEntryTextDataL( aEntry, textPtr );

    // First we try to fit text to [1, max_lines - 1] (i.e. 1,2 or 3 lines)
    TBool doesFit = EFalse;
    HBufC* visualText = NULL;
    while ( ! doesFit && lineWidths->Count() < KMaxLines  )
        {
        // Convert*WholeTextL wraps all text, so that if it doesn't
        // fit to given lines, it uses last line width to wrap remaining
        // texts. This is useful, because we then know did it fit or not.
        //
        // Note that it constructs new text buffer (visualText) that
        // has to be destroyed, if it's not used. If it's used, this
        // text has to be returned to caller and caller takes care of
        // destroying. This memory management is unfortunately complicated,
        // so be careful if you modify.
        visualText  =
            AknBidiTextUtils::ConvertToVisualAndWrapToArrayWholeTextL(
            *text,
            *lineWidths,
            *iLaf.iTextC.Font(),
            aLineArray);
        CleanupStack::PushL( visualText );

        // If amount of splitted lines is same (or less than) currently
        // tried lines, we can fit text to this amount of lines.
        TInt triedLines = lineWidths->Count();
        if ( aLineArray.Count() <= triedLines )
            {
            doesFit = ETrue;
            // In case Avkon method wraps text to less lines than we tried
            // it means that text fits to less lines, but icons has to
            // be put to empty line after text!
            // We have to manually add that empty line.
            if ( aLineArray.Count() < triedLines )
                {
                aLineArray.AppendL( TPtrC(KNullDesC) );
                }
            break; // exit from loop, we could fit text.
            }
        else
            {
            // Release visualText buffer and reset line array, because
            // we couldn't fit text on this try.
            CleanupStack::PopAndDestroy( visualText );
            visualText = NULL;
            aLineArray.Reset();

            // Prepend normal line width, and try again with more lines.
            // (Note: last line width stays last, because we prepend)
            lineWidths->InsertL( 0, KLineWidth );
            }
        }

    HBufC* result = NULL;
    if ( doesFit )
        {
        // If we could fit, we return visualText and caller has
        // to memory manage it. TPtrs in aLineArray points to this text
        // buffer.
        result = visualText;
        CleanupStack::Pop( visualText );
        CleanupStack::PopAndDestroy( text );
        }
    else
        {
        // If we couldn't fit before, we wrap text to max lines and truncate
        // it with ellipsis.
        TInt triedLines = lineWidths->Count();

        // truncate and add ellipsis
        TPtr truncatedText = text->Des();
        const TBool KUseEllipsis = ETrue;
        AknBidiTextUtils::ConvertToVisualAndWrapToArrayL(
            truncatedText,
            *lineWidths,
            *iLaf.iTextC.Font(),
            aLineArray,
            KUseEllipsis );

        // In case Avkon method wrapped text to less lines than we tried
        // it means that text fits to less lines, but icons has to
        // be put to empty line after text!
        // We have to manually add that empty line.
        if ( aLineArray.Count() < triedLines )
            {
            aLineArray.AppendL( TPtrC(KNullDesC) );
            }

        // We return original text, because
        // AknBidiTextUtils::ConvertToVisualAndWrapToArrayL
        // modifies that and TPtrs in aLineArray points to that buffer
        result = text;
        CleanupStack::Pop( text );
        }

    CleanupStack::PopAndDestroy( lineWidths );

    TRACE_EXIT_POINT;
    return result;
    };

// ----------------------------------------------------------------------------
// CCalenDayListBoxModel::?member_function
// ?implementation_description
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDayListBoxModel::CreateListBoxDataL(
    CArrayFixFlat<CCalenDayContainer::SItemInfo>& aLayoutTable,
    CArrayFixFlat<TInt>& aCellArray,
    MCalenServices& aServices,
    /*CCalInstanceView* aInstanceView*/
    TTime aCurrentDay)
    {
    TRACE_ENTRY_POINT;

    iItemTextArray->Reset();
    TBool isAllDayEvent;
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    aServices.GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);

    HBufC* listItemData = HBufC::NewLC(ECalenCharsInLine);

    // iterate layout
    for (TInt loop(0); loop < aLayoutTable.Count(); ++loop)
        {
        CCalenDayContainer::SItemInfo& itemInfo = aLayoutTable[loop];

        CCalEntry& entry = itemInfo.iTmpInstance->Entry();
        isAllDayEvent = EFalse;
        if (entry.EntryTypeL() == CCalEntry::EAppt || entry.EntryTypeL()
                == CCalEntry::EEvent)
            {
            isAllDayEvent = CalenViewUtils::IsAlldayEventL(
                    *itemInfo.iTmpInstance);
            }
        TCalCollectionId collectionId =
                itemInfo.iTmpInstance->InstanceIdL().iCollectionId;
        HBufC* calendarFileName = aServices.GetCalFileNameForCollectionId(collectionId).AllocLC();
        TInt index = calendarInfoList.Find( *calendarFileName,
                CCalenDayListBoxModel::CalendarInfoIdentifierL );
        
        CleanupStack::PopAndDestroy(calendarFileName);
        
        if (index != KErrNotFound && calendarInfoList[index]->Enabled())
            {
            itemInfo.iColor = calendarInfoList[index]->Color().Value();
            TPtr ptr = listItemData->Des();
            ptr.Zero();

            TTime start(TInt64(0));
            TTime end(TInt64(0));

            if ((entry.EntryTypeL() == CCalEntry::EAppt || entry.EntryTypeL()
                    == CCalEntry::EReminder) && (!isAllDayEvent))
                {
                // meeting notes
                // \t14:00\t-\t
                // 0 1      2
                //
                // no icon
                ptr.Append(KDesSeparator);

                TTimeIntervalMinutes duration;
                entry.EndTimeL().TimeLocalL().MinutesFrom(
                        entry.StartTimeL().TimeLocalL(), duration);
                start = itemInfo.iTmpInstance->Time().TimeLocalL();
                end = start + duration;
#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION
                InsertStartTimeL(ptr, start, aCurrentDay);
#else
                LimitStartAndEndTime( aCurrentDay, start, end );

                InsertTimeL(ptr, start);
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION
                if (!AknLayoutUtils::LayoutMirrored())
                    {
                    ptr.Append(KDesSeparator);
                    }

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION
                InsertHyphenL(ptr, start, end);
#else               
                InsertHyphen(ptr, start, end);
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION
                if (AknLayoutUtils::LayoutMirrored())
                    {
                    ptr.Append(KDesSeparator);
                    }

                ptr.Append(KDesSeparator);
                }
            else
            // Set icons and times for entry            
                {
                // non-timed notes
                TBuf<3> icon;
                icon.Num(
                        static_cast<TInt64> (iDayContainer->IconIndexFromEntryL(
                                entry, isAllDayEvent)));
                // icon\t\t\t
                // 0     1 2
                // gfxA  textA textB
                ptr.Append(icon);
                ptr.Append(KDesSeparator);
                ptr.Append(KDesSeparator);
                ptr.Append(KDesSeparator);
                }

            // textLines contain
            CArrayFixFlat<TPtrC>* textLines = new (ELeave) CArrayFixFlat<
                    TPtrC> (5);
            CleanupStack::PushL(textLines);

            // After WrapTextL call, visualText contains wrapped text in
            // memory buffer, textLines contain text pointers
            // (into visualText memory buffer) of beginning of each line of text.
            HBufC* visualText = WrapTextL(entry, *textLines);
            CleanupStack::PushL(visualText);

            // Put first line of text to current list line
            if (textLines->Count())
                {
                ptr.Append(textLines->At(0));
                }

            TBool isEndDisplayed = ETrue;
            // For subsequent text lines, we create separate list lines.
            if ((end != start) && (!isAllDayEvent))
                {
                isEndDisplayed = EFalse;
                }
            for (TInt i(1); i < textLines->Count() || !isEndDisplayed; i++)
                {
                if (itemInfo.iTopLine == KIndexError)
                    {
                    itemInfo.iTopLine = iItemTextArray->Count();
                    }
                iItemTextArray->AppendL(*listItemData);
                ptr.Zero();
                ptr.Append(KDesSeparator);
                // Sets an end time in second line
                if (!isEndDisplayed)
                    {
                    __ASSERT_ALWAYS( i == 1, User::Invariant() );

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION
                    InsertEndTimeL(ptr, end, aCurrentDay);
#else
                    InsertTimeL(ptr, end);
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION
                    isEndDisplayed = ETrue;
                    }
                else
                    {
                    ptr.Append(KDesTimeSpace);
                    }
                ptr.Append(KDesSeparator);
                ptr.Append(KDesSeparator);
                if (i < textLines->Count())
                    {
                    ptr.Append(textLines->At(i));
                    }
                }

            // end of last line (also for non-timed notes)
            ptr.Append(KDesSeparator);

            CleanupStack::PopAndDestroy(visualText);
            CleanupStack::PopAndDestroy(textLines);

            // Alarm, repeat and priority icons
            iDayContainer->InsertAdditionalIconsL(entry,
                    KIconSeperatorFormat(), ptr);

            // append created list item data to array of list items
            aCellArray.AppendL(iItemTextArray->Count());

            if (itemInfo.iTopLine == KIndexError)
                {
                itemInfo.iTopLine = iItemTextArray->Count();
                }
            iItemTextArray->AppendL(*listItemData);
            }
        }
    CleanupStack::PopAndDestroy(listItemData);
    CleanupStack::PopAndDestroy(&calendarInfoList);

    TRACE_EXIT_POINT;
    }

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION
void CCalenDayListBoxModel::InsertStartTimeL(TDes& aBuf,
                             const TTime& aStartTime,
                             const TTime& aCurrentDay)
    {
    TRACE_ENTRY_POINT;

    TBuf<6> arrow;
    TBuf<50> formattedStartTime;
    TBool mirrored( AknLayoutUtils::LayoutMirrored() );

    if(StartsBeforeFocusedDay(aStartTime, aCurrentDay))
        {
        arrow = GetArrowIndicator(mirrored);

        // need to format end time acc.to 12hr or 24hr format
        if(TimeFormat() == ETime24)
            {
            // 24 hour. show 00 only
            InsertTimeL(formattedStartTime, aStartTime, EInsert00);
            }
        else
            {
            // 12 hour. show hh:mm am/pm
            formattedStartTime = GetFormattedStartTimeL(aCurrentDay);
            }
        }
    else
        {
        // meeting starts after 12:00 am on current day
        // add start time as it is
        InsertTimeL( formattedStartTime, aStartTime );
        }

    if(mirrored)
        {
        // For mirrored UI layouts:
        aBuf.Append(formattedStartTime);
        aBuf.Append(arrow);
        }
    else
        {
        // For western UI layouts:
        aBuf.Append(arrow);
        aBuf.Append(formattedStartTime);
        }

    TRACE_EXIT_POINT;
    }

TBool CCalenDayListBoxModel::StartsBeforeFocusedDay(const TTime& aStartTime,
const TTime& aFocusedTime)
    {
    TRACE_ENTRY_POINT;

    const TTime focusedDay12am = CalenDateUtils::BeginningOfDay(aFocusedTime);
    if(aStartTime < focusedDay12am)
        {
        TRACE_EXIT_POINT;
        return ETrue;
        }

    TRACE_EXIT_POINT;
    return EFalse;
    }

TBool CCalenDayListBoxModel::EndsAfterFocusedDay(const TTime& aEndTime,
const TTime& aFocusedTime)
    {
    TRACE_ENTRY_POINT;

    const TTime nextDay12am = CalenDateUtils::BeginningOfDay(aFocusedTime)
    + TTimeIntervalDays(1);

    if(aEndTime >= nextDay12am)
        {
        TRACE_EXIT_POINT;
        return ETrue;
        }

    TRACE_EXIT_POINT;
    return EFalse;
    }

const TDesC& CCalenDayListBoxModel::GetArrowIndicator(const TBool aMirroredLayout)
    {
    TRACE_ENTRY_POINT;

    if(aMirroredLayout)
        {
        // unicode leftwards arrow for right to left layouts
        TRACE_EXIT_POINT;
        return KUnicodeLeftwardsArrow;
        }

    // unicode rightwards arrow for left to right layouts
    TRACE_EXIT_POINT;
    return KUnicodeRightwardsArrow;
    }

const TDesC& CCalenDayListBoxModel::GetFormattedStartTimeL(const TTime& aFocusedTime)
    {
    TRACE_ENTRY_POINT;

    iFormattedStartTime.Zero();
    const TTime focusedDay12am = CalenDateUtils::BeginningOfDay(aFocusedTime);
    InsertTimeL(iFormattedStartTime, focusedDay12am);

    TRACE_EXIT_POINT;
    return iFormattedStartTime;
    }

const TDesC& CCalenDayListBoxModel::GetFormattedEndTimeL(const TTime& aFocusedTime)
    {
    TRACE_ENTRY_POINT;

    iFormattedEndTime.Zero();
    const TTime focusedDay24hrs = CalenDateUtils::BeginningOfDay(aFocusedTime)
        + TTimeIntervalDays(1);

    InsertTimeL(iFormattedEndTime, focusedDay24hrs);

    TRACE_EXIT_POINT;
    return iFormattedEndTime;
    }

void CCalenDayListBoxModel::InsertEndTimeL(TDes& aBuf,
                                           const TTime& aEndTime,
                                           const TTime& aCurrentDay)
    {
    TRACE_ENTRY_POINT;

    TBuf<6> arrow;
    TBuf<50> formattedEndTime;
    TBool mirrored( AknLayoutUtils::LayoutMirrored() );

    if(EndsAfterFocusedDay(aEndTime, aCurrentDay))
        {
        arrow = GetArrowIndicator(mirrored);

        // need to format end time acc.to 12hr or 24hr format
        if(TimeFormat() == ETime24)
            {
            // 24 hour. show 24 only
            InsertTimeL(formattedEndTime, aEndTime, EInsert24);
            }
        else
            {
            // 12 hour. show hh:mm am/pm
            formattedEndTime = GetFormattedEndTimeL(aCurrentDay);
            }
        }
    else
        {
        // meeting does not continue after 24:00 on current day
        // add start time as it is
        InsertTimeL( formattedEndTime, aEndTime );
        }

    if(mirrored)
        {
        // For Mirrored Layouts:
        aBuf.Append(arrow);
        aBuf.Append(formattedEndTime);
        }
    else
        {
        // For Western Layouts:
        aBuf.Append(formattedEndTime);
        aBuf.Append(arrow);
        }

    TRACE_EXIT_POINT;
    }

TTimeFormat CCalenDayListBoxModel::TimeFormat() const
    {
    TLocale locale;
    return locale.TimeFormat();
    }
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION


// -----------------------------------------------------------------------------
// CCalenDayListBoxModel::CalendarInfoIdentifierL
// Searches for the index in calendar info list based on calendar file name
// -----------------------------------------------------------------------------
//
TBool CCalenDayListBoxModel::CalendarInfoIdentifierL(
        const HBufC* aName, const CCalCalendarInfo& aCalendarInfo)
    {
    TRACE_ENTRY_POINT;
    TBool retVal = EFalse;
    HBufC* calendarFileName = aCalendarInfo.FileNameL().AllocLC();
    retVal = calendarFileName->CompareF(*aName);
    CleanupStack::PopAndDestroy(calendarFileName);        
    TRACE_EXIT_POINT;
    return (!retVal);
    }

//  End of File

