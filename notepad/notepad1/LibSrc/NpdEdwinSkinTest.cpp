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
* Description:  Implementation of Notepad/Template Editor.
*
*/


#include <AknUtils.h>
#include <txtglobl.h>
#include <barsread.h>
#include <NpdLib.rsg>
#include "NpdEdwin.h"
#include "NpdLibPanic.h"
#include "NpdLib.laf"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadEdwin::LafSizeChanged
// Called from CNotepadEditorLaf::SizeChanged().
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::LafSizeChanged(const TRect& aRect)
    {
    iLayoutShadow.LayoutRect( aRect, 
        NOTEPAD_EDITOR_SHADOW_C, //TInt C
        NOTEPAD_EDITOR_SHADOW_L, //TInt l
        NOTEPAD_EDITOR_SHADOW_T, //TInt t
        AknLayoutUtilsNoValue, //TInt r
        AknLayoutUtilsNoValue, //TInt b
        NOTEPAD_EDITOR_SHADOW_W, //TInt W
        NOTEPAD_EDITOR_SHADOW_H ); //TInt H
    iLayoutOutline.LayoutRect( aRect, 
        NOTEPAD_EDITOR_OUTLINE_C, //TInt C
        NOTEPAD_EDITOR_OUTLINE_L, //TInt l
        NOTEPAD_EDITOR_OUTLINE_T, //TInt t
        AknLayoutUtilsNoValue, //TInt r
        AknLayoutUtilsNoValue, //TInt b
        NOTEPAD_EDITOR_OUTLINE_W, //TInt W
        NOTEPAD_EDITOR_OUTLINE_H ); //TInt H
    iLayoutInside.LayoutRect( aRect,
        NOTEPAD_EDITOR_INSIDE_C, //TInt C
        NOTEPAD_EDITOR_INSIDE_L, //TInt l
        NOTEPAD_EDITOR_INSIDE_T, //TInt t
        AknLayoutUtilsNoValue, //TInt r
        AknLayoutUtilsNoValue, //TInt b
        NOTEPAD_EDITOR_INSIDE_W, //TInt W
        NOTEPAD_EDITOR_INSIDE_H ); //TInt H
    }

// -----------------------------------------------------------------------------
// CNotepadEdwin::InitNotepadEditorL
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::InitNotepadEditorL(
    const TRect& aRect,
    CNotepadViewerLaf& aLafControl )
    {
    iLaf = &aLafControl;
    AknLayoutUtils::LayoutControl( iLaf, 
        aRect, 0, 0, 0, AknLayoutUtilsNoValue, AknLayoutUtilsNoValue, 
        aRect.Width(), aRect.Height() );
    AknLayoutUtils::LayoutEdwin(this, aRect,
        NOTEPAD_EDITOR_LAYOUT_FONT, //TInt font 
        NOTEPAD_EDITOR_LAYOUT_C, // TInt C
        NOTEPAD_EDITOR_LAYOUT_L, // TInt l 
        NOTEPAD_EDITOR_LAYOUT_R, // TInt r 
        NOTEPAD_EDITOR_LAYOUT_B, // TInt B
        NOTEPAD_EDITOR_LAYOUT_W, // TInt W
        NOTEPAD_EDITOR_LAYOUT_J, // TInt J
        NOTEPAD_EDITOR_LAYOUT_LINES, // TInt aNumberOfLinesShown
        NOTEPAD_EDITOR_LAYOUT_NEXTB); // TInt aNextLineBaseline

    // SetBackgroundColorL() contains modification to 
    // Edwin's CParaFormat which was created by 
    // AknLayoutUtils::LayoutEdwin(...), so iEditor->SetBackgroundColorL()
    // must be called after AknLayoutUtils::LayoutEdwin(...).
    //
    SetBackgroundColorL();

    // EdwinEvent is used for dynamically changing Laf bitmap
    //
    AddEdwinObserverL(this);

    Text()->SetHasChanged(EFalse);
    SetAmountToFormat(); // to show fast even if large text
    SetCanDrawOutsideRect(); // Redraw after Chinese FEP
    }

// -----------------------------------------------------------------------------
// CNotepadEdwin::ChangeDecorationGraphicsL
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::ChangeDecorationGraphicsL()
    {
    __ASSERT_DEBUG( iLaf, 
        Panic(ENotepadLibraryPanicNoControlForDecorationGraphics) );
    TPoint p;
    iLaf->SetImage(iLayout->PosInBand(0, p) ? 0 : 1);
    }

// -----------------------------------------------------------------------------
// CNotepadEdwin::HandleEdwinEventL
// from MEikEdwinObserver
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::HandleEdwinEventL(
    CEikEdwin* /*aEdwin*/,
    TEdwinEvent aEventType )
    {
    __ASSERT_DEBUG( iLaf, 
        Panic(ENotepadLibraryPanicNoControlForDecorationGraphics) );
    if ( aEventType == EEventNavigation )
        {
        ChangeDecorationGraphicsL();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadEdwin::SetBackgroundColorL
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::SetBackgroundColorL()
    {
    TRgb bgcolor(iLayoutInside.Color());
    CEikEdwin::SetBackgroundColorL(bgcolor);
    iTextView->SetBackgroundColor(bgcolor);
    CGlobalText* text = STATIC_CAST(CGlobalText*, Text());
    CParaFormat* paraFormat = CParaFormat::NewLC();
    TParaFormatMask paraFormatMask;
    text->GetParaFormatL(paraFormat, paraFormatMask, 
        0, text->DocumentLength());
    paraFormat->iFillColor = bgcolor;
    paraFormatMask.SetAttrib(EAttFillColor);
    text->ApplyParaFormatL(paraFormat,paraFormatMask, 
        0, text->DocumentLength());
    CleanupStack::PopAndDestroy(); // paraFormat
    TResourceReader rr;
    iCoeEnv->CreateResourceReaderLC(rr, R_NOTEPAD_CURSOR_COLOR);
    TRgb cursorColor(AKN_LAF_COLOR(rr.ReadInt16()));
    CleanupStack::PopAndDestroy(); // rr
    iTextView->SetCursorXorColor(
        TRgb( cursorColor.Value() ^ bgcolor.Value() ) );
    NotifyNewFormatL();
    }

// -----------------------------------------------------------------------------
// CNotepadEdwin::HandleResourceChange
// from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::HandleResourceChange(TInt aType)
    {
    CEikEdwin::HandleResourceChange( aType );
    if( aType == KEikColorResourceChange )
        {
        TRAPD(r, SetBackgroundColorL());
        }
    }

// -----------------------------------------------------------------------------
// CNotepadEdwin::Draw
// from CCoeControl
// -----------------------------------------------------------------------------
//
#include <aknsutils.h>
#include <aknsDrawUtils.h>
#include <aknappui.h>
void CNotepadEdwin::Draw(const TRect& aRect) const
    {
MAknsSkinInstance* skin = AknsUtils::SkinInstance();
CFbsBitmap* bitmap = AknsUtils::GetCachedBitmap( skin, KAknsIIDSkinBmpMainPaneUsual);
    CWindowGc& gc = SystemGc();
    if (bitmap) {
TSize size(bitmap->SizeInPixels());
RDebug::Print(_L("size=(%d, %d)"), size.iWidth, size.iHeight);
        TRect rect(iAvkonAppUi->ClientRect());
        TAknLayoutRect layout;
        layout.LayoutRect(TRect(0, 0, 176, 164), 221, 0, 0,
        AknLayoutUtilsNoValue,
        AknLayoutUtilsNoValue,
        176, 164);
        layout.DrawImage(gc, bitmap, 0);
//    iLayoutShadow.DrawImage(gc, bitmap, 0);
    }
    iLayoutShadow.DrawRect(gc);
    iLayoutOutline.DrawOutLineRect(gc);
    iLayoutInside.DrawRect(gc);
    CEikEdwin::Draw(aRect);
    }

// End of File  
