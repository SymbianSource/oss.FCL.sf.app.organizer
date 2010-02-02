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
* Description:   Drawing Cell of MonthView of Calendar application.
 *
*/



//debug
#include "calendarui_debug.h"


// INCLUDE FILES
#include "calenmonthcelllistboxdata.h"

#include "calendar.hrh"

#include <AknsEffectAnim.h>
#include <AknUtils.h>
#include <gulicon.h>

#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>


// CONSTANTS
const TInt KMonthCells(2);


// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CCalenMonthCellListBoxData* CCalenMonthCellListBoxData::NewL()
    {
    TRACE_ENTRY_POINT;
    
    CCalenMonthCellListBoxData* self = new (ELeave) CCalenMonthCellListBoxData();
    // Construct grid animation by default
    self->ConstructLD( KAknsIIDQsnAnimGrid );
    
    TRACE_EXIT_POINT;
    return self;
    }

// Destructor
CCalenMonthCellListBoxData::~CCalenMonthCellListBoxData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CCalenMonthCellListBoxData::CCalenMonthCellListBoxData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenMonthCellListBoxData::DrawFormatted
// Draw date and bitmap of Month Grid
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenMonthCellListBoxData::DrawFormatted( TListItemProperties aProperties, 
                                                CWindowGc& aGc,
                                                const TDesC* aText,
                                                const TRect& aItemRect,
                                                TBool /*aHighlight*/,
                                                const TColors& aColors) const
    {
    TRACE_ENTRY_POINT;
    
    // This is minimized version of draw formatted needed to draw month view subcells.
    // In principle, there shouldn't be any month view specific functionality in here,
    // and we could use Avkon's DrawFormatted. However, Avkon DrawFormatted contains
    // lot of extra stuff, which seems unnecessary for month view.

    aGc.SetPenStyle(CGraphicsContext::ESolidPen);
    aGc.SetBrushStyle(CGraphicsContext::ENullBrush);
    aGc.SetPenColor(aColors.iText);
    aGc.SetBrushColor(aColors.iBack);

    TInt lastSubCell(LastSubCell());

    TPtrC text;

    // Draw subcells (there are only two of them
    for (TInt subcell(0); subcell < lastSubCell; ++subcell)
        {
        TextUtils::ColumnText( text, subcell, aText );
        TMargins m( SubCellMargins( subcell ) );
        TRect rect( aItemRect.iTl + SubCellPosition( subcell ) + TSize( m.iLeft, m.iTop ),
                    SubCellSize( subcell ) - TSize( m.iRight + m.iLeft, m.iBottom + m.iTop ) );

        // SKIP subcell if either there's NO DATA, or area is 0
        if (text.Length() || rect.iTl.iX != rect.iBr.iX || rect.iTl.iY != rect.iBr.iY)
            {
            CGraphicsContext::TTextAlign align( CGraphicsContext::ECenter );

            if( !SubCellIsGraphics( subcell ) )
                { // draw text
                const CFont* usedFont = Font( aProperties, subcell );
                if( usedFont == NULL )
                    {
                    usedFont = CEikonEnv::Static()->NormalFont();
                    }
                aGc.UseFont( usedFont );

                // baseline calc needed for each cell.
                TBuf<ECalenCharsInLine> clipbuf( text );
                AknTextUtils::DisplayTextLanguageSpecificNumberConversion( clipbuf );
                TInt baseLineOffset( SubCellBaselinePos(subcell) );
                baseLineOffset -= m.iTop;
                aGc.DrawText( clipbuf, rect, baseLineOffset, align );
                }
            else
                {  // drawing bitmap
                TLex lex(text);
                TInt index(0);
                lex.Val(index);
                if (index >= 0)
                    { 
                    CGulIcon* icon = (*IconArray())[index];
                    TPoint bmpPos = rect.iTl;

                    CFbsBitmap* colorBitmap = icon->Bitmap();
                    const TRect bmpRect( colorBitmap->SizeInPixels() );
                    aGc.BitBltMasked( bmpPos, colorBitmap, bmpRect, icon->Mask(), ETrue );
                    }
                }
            }
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenMonthCellListBoxData::LastSubCell
// an ordinal number of last subcell of cell
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CCalenMonthCellListBoxData::LastSubCell() const
    {
    TRACE_ENTRY_POINT;
    
    TRACE_EXIT_POINT;
    return KMonthCells;
    }

// ---------------------------------------------------------
// CCalenMonthCellListBoxData::Draw
// Draw a cell of Month Grid
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenMonthCellListBoxData::Draw(TListItemProperties aProperties, CWindowGc& aGc, const TDesC* aText, const TRect& aRect, TBool aHighlight, const TColors& aColors) const
    {
    TRACE_ENTRY_POINT;

    TRect aItemRect(aRect);

    // draw skinned highlight
    TBool highlightDrawn = EFalse;

    if(aHighlight)
        {
        MAknsSkinInstance *skin = AknsUtils::SkinInstance();
        MAknsControlContext *cc = AknsDrawUtils::ControlContext( Control() );

        if(!cc) cc = SkinBackgroundContext();

        if(cc)
            {
            aGc.SetPenStyle( CGraphicsContext::ENullPen );

            if( HighlightAnim() )
                {
                AboutToDrawHighlightAnim();
                if( HighlightAnim() )
                    highlightDrawn = HighlightAnim()->Render( aGc, aItemRect );
                }

            // Either animation does not exist or drawing it failed
            if( !highlightDrawn )
                {
                TAknLayoutRect grid_highlight_pane_cp08;
                grid_highlight_pane_cp08.LayoutRect( aItemRect /*P:cell_cale_month_pane*/, AknLayoutScalable_Apps::grid_highlight_pane_cp08().LayoutLine() );

                TAknLayoutRect cell_highlight_pane_g1;
                cell_highlight_pane_g1.LayoutRect(
                    grid_highlight_pane_cp08.Rect(),
                    AknLayoutScalable_Avkon::cell_highlight_pane_g1().LayoutLine() );

                highlightDrawn = AknsDrawUtils::DrawFrame( skin, aGc,
                                                           grid_highlight_pane_cp08.Rect(), 
                                                           cell_highlight_pane_g1.Rect(), 
                                                           KAknsIIDQsnFrGrid, KAknsIIDQsnFrGridCenter );
                }
            }
        }

    // draw the actual items.
    DrawFormatted(aProperties, aGc, aText, aItemRect, EFalse, aColors);

    aGc.Reset();
    
    TRACE_EXIT_POINT;
    }

// End of File
