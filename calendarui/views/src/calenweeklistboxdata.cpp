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
* Description:   Drawing WeekView Listbox data.
 *                Event bar and Event icon, event indicator
 *
*/


//debug
#include "calendarui_debug.h"
#include "calenweeklistboxdata.h"
#include "calendrawutils.h"
#include "calenweekdata.h"
#include "calendar.hrh"

#include <Calendar.rsg>
#include <AknBidiTextUtils.h>
#include <AknsDrawUtils.h>
#include <AknsEffectAnim.h>
#include <AknUtils.h>
#include <barsread.h>
#include <eikenv.h>
#include <gulicon.h>
#include <gulutil.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <eikenv.h>

//  LOCAL CONSTANTS AND MACROS

const TInt KStartColorForPattern = 8716481;
const TInt KEndColorForPattern = 16777215;
const TInt KPatternBrushWidth = 7;
// ================= MEMBER FUNCTIONS =======================

// Destructor
CCalenWeekLBData::~CCalenWeekLBData()
    {
    TRACE_ENTRY_POINT;
    if(iGradientBitmap)
        {
        delete iGradientBitmap;
        iGradientBitmap = NULL;
        }
    TRACE_EXIT_POINT;
    }

// Two-phased constructor.
CCalenWeekLBData* CCalenWeekLBData::NewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenWeekLBData* self = new(ELeave) CCalenWeekLBData();
    // Construct list animation by default
    self->ConstructLD( KAknsIIDQsnAnimList );
    

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------
// CCalenWeekLBData::CurrentSubCellPos
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenWeekLBData::CurrentSubCellPos(TRect& aCurrentSubCellRect, const TPoint& aCurrentItemPos)
    {
    TRACE_ENTRY_POINT;
    
    aCurrentSubCellRect.SetRect( SubCellPosition(iHilightedSubCell + 1),
                                 SubCellSize(iHilightedSubCell + 1) );
    aCurrentSubCellRect.Move( aCurrentItemPos );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// CCalenWeekLBData::SetHilightedSubCell
// Set hilighted sub cell. But it does not redraw.
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenWeekLBData::SetHilightedSubCell(TInt aHilightedSubCell)
    {
    TRACE_ENTRY_POINT;
    
    iHilightedSubCell = aHilightedSubCell;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// CCalenWeekLBData::HilightedSubCell
// Return hilighted subcell index
// (other items were commented in a header).
// ----------------------------------------------------
//
TInt CCalenWeekLBData::HilightedSubCell()
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return iHilightedSubCell;
    }

// ----------------------------------------------------
// CCalenWeekLBData::SetCellCount
// Set cell count. because "LastSubCell" is private function.
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenWeekLBData::SetCelCount(TInt aCount)   // Setting cell count
    {
    TRACE_ENTRY_POINT;
    
    iCellCount = aCount;
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------
// CCalenWeekLBData::SetVariantIndex
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenWeekLBData::SetVariantIndex(TInt aLayout) 
    {
    TRACE_ENTRY_POINT;
    
    iLayoutVariant = aLayout;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// CCalenWeekLBData::SetCellCount
// Rreturn Cell count
// (other items were commented in a header).
// ----------------------------------------------------
//
TInt CCalenWeekLBData::CellCount() const
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return iCellCount;
    }

// ----------------------------------------------------
// CCalenWeekLBData::Draw
// Drawing item data and highlight
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenWeekLBData::Draw(TListItemProperties aProperties,// Item property
                            CWindowGc& aGc,                 // Graphics context
                            const TDesC* aText,             // Drawing item text
                            const TRect& aRect,             // Item rectangle
                            TBool aHighlight,               // ETrue: hilighted
                            const TColors& aColors) const   // Item color
    {
    TRACE_ENTRY_POINT;
    
    const TRect& aItemRect = aRect;

    // LAF code for highlight drawing. 
    // FIXME: dead code, can be removed?
    TRect subCellRect(SubCellPosition(iHilightedSubCell + 1), SubCellSize(iHilightedSubCell + 1));
    subCellRect.Move(aRect.iTl);

    // Draw the actual items.
    DrawFormatted(aProperties, aGc, aText, aItemRect, aColors, aHighlight);

    aGc.Reset();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// Constructor
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenWeekLBData::CCalenWeekLBData()
    {
    TRACE_ENTRY_POINT;
    
    TRAP_IGNORE(InitialisePatternL());

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// CCalenWeekLBData::DrawFormatted
// Drawing item
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenWeekLBData::DrawFormatted( TListItemProperties aProperties,   // Item property
                                      CWindowGc& aGc,                    // Graphics context
                                      const TDesC* aText,                // Drawing item text
                                      const TRect& aItemRect,            // Item rectangle
                                      const TColors& aColors,            // Item colors
                                      TBool /*aHighlight */) const
    {
    TRACE_ENTRY_POINT;
    
    const CFont* font = SubCellFont(0);
    const TText* ptr = aText->Ptr();
    const SDisplayData* dispData = (const SDisplayData*)ptr;

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = NULL;

    TInt lastSubCell(CellCount() - 1);
    // mark highlight as "not done" only if we need to draw the highlight
//    TBool highlightDone( aHighlight ? EFalse : ETrue );

    if(!font)
        {
        font = CEikonEnv::Static()->NormalFont();
        }

    if((cc = AknsDrawUtils::ControlContextOfParent( Control() )) == NULL)
        {
        cc = SkinBackgroundContext();
        }

    // Draw subcells into textRect

    for(TInt subcell(0); subcell <= lastSubCell; ++subcell)
        {
        aGc.SetPenStyle(CGraphicsContext::ESolidPen);
        aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
        // SetPosition, SetSize and margins support
        TRect bRect( SubCellPosition(subcell), SubCellSize(subcell) );
        TMargins m( SubCellMargins(subcell) );
        TRect cRect(
            bRect.iTl +TSize(m.iLeft, m.iTop),
            bRect.Size() -TSize(m.iRight + m.iLeft, m.iBottom + m.iTop) );
        TRect textRect( aItemRect.iTl +cRect.iTl, cRect.Size() );

        aGc.SetPenColor(aColors.iText);
        aGc.SetBrushColor(aColors.iBack);

        switch(subcell)
            {
            case ECalenWeekFirstDay:    // May be Monday
            case ECalenWeekSecondDay:   // May be Tuesday
            case ECalenWeekThirdDay:    // May be Wednesday
            case ECalenWeekForthDay:    // May be Thursday
            case ECalenWeekFifthDay:    // May be friday
            case ECalenWeekSixthDay:    // May be saturday
            case ECalenWeekSeventhDay:  // May be sunday
                {
                const TCalenWeekHour& hourData = dispData->iHourData[subcell -1];
                bRect.Move( aItemRect.iTl );

                if (! AknsDrawUtils::Background( skin, cc, NULL, aGc, bRect,
                                                 KAknsDrawParamNoClearUnderImage))
                    {
                    aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
                    aGc.Clear(bRect);
                    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
                    }

                //Focus is removed in Week View
                /*if(!highlightDone && aHighlight 
                   && subcell == iHilightedSubCell+1)
                    {
                    highlightDone = DrawHighlight( bRect, aGc, ETrue );
                    }*/

                switch(hourData.DataType())
                    {
                    case TCalenWeekHour::ENoData:
                    break;

                    case TCalenWeekHour::EHasIcon:
                        DrawIcon( aGc, hourData, bRect );
                    break;

                    case TCalenWeekHour::EHasBar:
                        DrawDurationBar( aGc, hourData, bRect );
                    break;
                    }

                //Focus is removed in Week View
                // draw non-skinned highlight
                /*if(!highlightDone && aHighlight && subcell == iHilightedSubCell +1)
                    {
                    highlightDone = DrawHighlight( bRect, aGc, EFalse );
                    }*/
                }
            break;

            case ECalenWeekHour:
                {
                // build the background of this column.
                TRect dRect( aItemRect.iTl +bRect.iTl, bRect.Size() );

                if (! AknsDrawUtils::Background( skin, cc, NULL, aGc, dRect, KAknsDrawParamNoClearUnderImage))
                    {
                    aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);
                    aGc.Clear( dRect );
                    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
                    }

                DrawCellText( aProperties, subcell, aGc, dispData->iHourDes, cRect, textRect, aColors );
                }
            break;

            default:
            break;
            }//switch
        }//for loop
     
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// CCalenWeekLBData::DrawHighlight
// (other items were commented in a header).
// ----------------------------------------------------
//
TBool CCalenWeekLBData::DrawHighlight(const TRect aCellRect, CWindowGc& aGc, const TBool aDrawSkinned) const
    {
    TRACE_ENTRY_POINT;
    
    // Note: aCellRect should match cell_cale_week_pane layout item

    // Highlight rect 
    TAknLayoutRect grid_highlight_pane_cp07;
    grid_highlight_pane_cp07.LayoutRect( 
        aCellRect /*P:cell_cale_week_pane*/,
        AknLayoutScalable_Apps::grid_highlight_pane_cp07().LayoutLine() );

    if( aDrawSkinned )
        {
        TBool done = EFalse;
        if( HighlightAnim() )
            {
            AboutToDrawHighlightAnim();
            if( HighlightAnim() )
                done = HighlightAnim()->Render( aGc, grid_highlight_pane_cp07.Rect() );
            }

        // Either animation does not exist or drawing it failed
        if( !done )
            {
            // Highlight skin center
            TAknLayoutRect cell_highlight_pane_g1;
            cell_highlight_pane_g1.LayoutRect(
                grid_highlight_pane_cp07.Rect(),
                AknLayoutScalable_Avkon::cell_highlight_pane_g1().LayoutLine() );

            done = AknsDrawUtils::DrawFrame( AknsUtils::SkinInstance(), aGc, 
                                             grid_highlight_pane_cp07.Rect(), 
                                             cell_highlight_pane_g1.Rect(),
                                             KAknsIIDQsnFrGrid, 
                                             KAknsIIDQsnFrGridCenter );
            }
        
        TRACE_EXIT_POINT;
        return done;
        }
    else
        {
        // non-skinned highlight. Is this possible anymore ?
        // And what color we should use? 
        grid_highlight_pane_cp07.DrawOutLineRect( aGc );
        }
    
    TRACE_EXIT_POINT;
    return ETrue;
    }

// ----------------------------------------------------
// CCalenWeekLBData::DrawCellText
// Drawing cell text
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenWeekLBData::DrawCellText(TListItemProperties aProperties, // Item property
                                    TInt aSubCell,                 // Sub cell index
                                    CWindowGc& aGc,                // Graphics context
                                    const TPtrC aText,             // Drawing desriptor
                                    const TRect& /*aCellRect*/,    // Cell rectangle
                                    const TRect& aTextRect,        // Text rectangle
                                    const TColors& aColors) const  // text colors
    {
    TRACE_ENTRY_POINT;
    
    CGraphicsContext::TTextAlign align(SubCellAlignment(aSubCell));
    const CFont* cellFont = Font(aProperties, aSubCell);
    aGc.UseFont(cellFont);
    aGc.SetPenColor(aColors.iText);
    TInt baseLineOffset = SubCellBaselinePos(aSubCell);

    TBuf<ECalenCharsInLine> numTmp = aText;
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(numTmp);
    TBuf<ECalenCharsInLine> visualTmp;
    AknBidiTextUtils::ConvertToVisualAndClip(
        numTmp, visualTmp, *cellFont, aTextRect.Width(), aTextRect.Width());

    aGc.DrawText(visualTmp, aTextRect, baseLineOffset, align);
    
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------
// CCalenWeekLBData::DrawDurationBar
// Drawing cell event indicator bar
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenWeekLBData::DrawDurationBar(
    CWindowGc& aGc,
    const TCalenWeekHour& aHourData,
    const TRect& aCellRect
    ) const
    {
    TRACE_ENTRY_POINT;
    
    // Note: Cell rectangle should match cell_cale_week_pane layout item

    // Indicator bar consists of 6 small parts, each representing 10 minutes of one hour
    // Each have color, which indicates current events synchronization type
    // or that there is multiple overlapping events in that 10 minutes slot
    const TInt KDurationBarCount = 6;
    ASSERT( KDurationBarCount == KEventsInHour );

    TAknLayoutRect durationBars[ KDurationBarCount ];
    for (TInt i=0; i < KDurationBarCount; ++i)
        {
        durationBars[i].LayoutRect(
            aCellRect /*P:cell_cale_week_pane*/,
            AknLayoutScalable_Apps::cell_cale_week_pane_g3(i).LayoutLine() );
        }

    // Set our pen properties
    aGc.SetPenStyle( CGraphicsContext::ENullPen );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );

    // Note: Below is 1) correct implementation and 2) currently used implementation

    // 1) Correct implementation is commented out. There are gaps between duration 
    // bars in layouts so we can't yet put correct implementation in.
//     for (TInt i=0; i < KEventsInHour; i++)
//         {
//         TEventColor barType = aHourData.Event(i);
//         if ( barType != ENoEvent )
//            {
//            aGc.SetBrushColor( BarColor( barType ) );
//         // FIXME: there is skin qgn_indi_cale_duration, that could be drawn here.
//         // It should be fetched and colored for different possible colors
//         // in sizechanged. Normal drawing has to be preserved, as this skin item
//         // is not usually available.
//            aGc.DrawRect( durationBars[i].Rect() );
//            }
//         }

    // 2) Implementation that circumvents layout data gaps between slots:
    // it adjust slot rect to be adjacent to next slot's rect

    for( TInt i=0; i < KEventsInHour; ++i)
        {
        TUint32 color= aHourData.Event(i);
        if ( color != ENoEvent || color == EBlackColoredEvent )
            {
            aGc.SetBrushColor( color );
            TRect slotRect = durationBars[i].Rect();
            
            // if not drawing last bar slot, extend rect to be adjacent to next slot
            // Note that TRect's bottom-right corner is outside of rect. 
            const TInt KLastSlotIndex = KEventsInHour - 1;
            if( i != KLastSlotIndex )
                {
                TInt nextSlot = i + 1;
                slotRect.iBr.iY = durationBars[ nextSlot ].Rect().iTl.iY;
                }
            
            if(color == EManyEvents)
                {
                // draw pattern for the conflicting meeting events from 
                // different calendar db
                aGc.SetPenStyle(CGraphicsContext::ENullPen);
                aGc.UseBrushPattern(iGradientBitmap);
                aGc.SetBrushStyle(CGraphicsContext::EPatternedBrush);
                aGc.DrawRect(slotRect);
                aGc.DiscardBrushPattern();
                }
            else
                {
                // Set pen properties
                aGc.SetPenStyle( CGraphicsContext::ENullPen );
                aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
                aGc.DrawRect( slotRect );
                }
            }
        }

    // Return normal pen properties (is this needed?)
    aGc.SetPenStyle( CGraphicsContext::ESolidPen );
    aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
// CCalenWeekLBData::DrawIcon
// Draw non-timed note icon icon
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenWeekLBData::DrawIcon( CWindowGc& aGc,
                                 const TCalenWeekHour& aHourData, 
                                 const TRect& aCellRect
                                 ) const
    {
    TRACE_ENTRY_POINT;
    
    // Note: aCellRect should match cell_cale_week_pane layout item
    // Also icon should have been layouted to correct size in SizeChanged of listbox

    TInt index(aHourData.IconIndex());

    if(index >= 0)
        {
        CGulIcon* icon = (*IconArray())[index];
        CFbsBitmap* bitmap = icon->Bitmap();

        TAknLayoutRect iconLayout;
        iconLayout.LayoutRect( aCellRect /*P:cell_cale_week_pane*/,
                               AknLayoutScalable_Apps::cell_cale_week_pane_g2(iLayoutVariant).LayoutLine() );

        if(icon->Mask())
            {
            TRect iconSizeRect( TPoint( 0, 0 ), bitmap->SizeInPixels() );
            aGc.BitBltMasked( iconLayout.Rect().iTl, bitmap, iconSizeRect, icon->Mask(), ETrue );
            }
        else
            {
            aGc.BitBlt( iconLayout.Rect().iTl, bitmap );
            }
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenWeekLBData::InitialisePatternL
// Initialise Pattern at the construction period
// ----------------------------------------------------------------------------
void CCalenWeekLBData::InitialisePatternL()
    {
    TRACE_ENTRY_POINT;
    // initialise pattern gradient    
    iGradientBitmap = new(ELeave)CFbsBitmap;
    CEikonEnv* env = CEikonEnv::Static();
    ColorUtils::TBitmapOrientation patternOrientation
            = ColorUtils::EBitmapOrientationVertical;
    ColorUtils::CreateGradientBitmapL( *iGradientBitmap,env->WsSession(),
                                        KPatternBrushWidth,
                                        patternOrientation,
                                        KStartColorForPattern,
                                        KEndColorForPattern);
    TRACE_EXIT_POINT;
    }
// End of File
