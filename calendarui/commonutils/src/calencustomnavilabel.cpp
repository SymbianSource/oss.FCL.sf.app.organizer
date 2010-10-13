/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Navi decorated control for eventview
 *
*/

#include "calencustomnavilabel.h"
#include "calendarui_debug.h"

#include <aknlayoutscalable_apps.cdl.h>
#include <eikapp.h>
#include <eikappui.h>
#include <AknUtils.h>
#include <calendar.mbg>
#include <AknIconUtils.h>
#include <debug.h>
#include <AknsUtils.h>


// ----------------------------------------------------------------------------
// CCustomNaviControl::NewL
// First stage construction
// ----------------------------------------------------------------------------
CCustomNaviControl* CCustomNaviControl::NewL(const TDesC& aName, const TRgb aColor)
    {
    TRACE_ENTRY_POINT;
    CCustomNaviControl* self = new(ELeave)CCustomNaviControl();
    CleanupStack::PushL(self);
    self->ConstructL( aName, aColor );
    CleanupStack::Pop(self);
    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCustomNaviControl::~CCustomNaviControl
// Destructor
// ----------------------------------------------------------------------------
CCustomNaviControl::~CCustomNaviControl()
    {
    TRACE_ENTRY_POINT;
    if(iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    if(iMask)
        {
        delete iMask;
        iMask = NULL;
        }

    if(iCalendarName)
        {
        delete iCalendarName;
        }
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCustomNaviControl::SetCalendarNameAndColor
// Sets the Calendar name and Color.
// ----------------------------------------------------------------------------    
void CCustomNaviControl::SetCalendarNameAndColor(const TDesC& aName, const TRgb aColor)
    {
    TRACE_ENTRY_POINT;
    delete iCalendarName;
    iCalendarColor = aColor;
    iCalendarName = aName.AllocL();
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCustomNaviControl::CCustomNaviControl
// Constructor
// ----------------------------------------------------------------------------    
CCustomNaviControl::CCustomNaviControl()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCustomNaviControl::ConstructL
// Second stage construction
// ----------------------------------------------------------------------------
void CCustomNaviControl::ConstructL(const TDesC& aName, const TRgb aColor)
    {
    TRACE_ENTRY_POINT;
    iCalendarColor = aColor;
    iCalendarName = aName.AllocL();
    CreateWindowL();
    // Create an icon
    TFileName IconFile;
    IconFile = ((CEikAppUi*)CCoeEnv::Static()->AppUi())->Application()->BitmapStoreName();
    AknIconUtils::CreateIconL(iBitmap, iMask,IconFile,
            EMbmCalendarQgn_indi_navi_color,
            EMbmCalendarQgn_indi_navi_color_mask);
    TRect naviPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ENaviPane, naviPane );
    SetRect(naviPane);
    ActivateL();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCustomNaviControl::Draw
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCustomNaviControl::Draw(const TRect& /*aRect*/) const
    {
    TRACE_ENTRY_POINT;
    
    TSize navi_pane_size;
    AknLayoutUtils::LayoutMetricsSize (AknLayoutUtils::ENaviPane, navi_pane_size);
    TRect navi_pane(navi_pane_size);
    TAknLayoutRect navi_cale_icon_pane;
    navi_cale_icon_pane.LayoutRect(navi_pane, 
                        AknLayoutScalable_Apps::navi_pane_bcale_mc_g1().LayoutLine() );
    TRect navi_cale_icon_ev2_pane = navi_cale_icon_pane.Rect();
    
    CWindowGc& gc = SystemGc();
    AknIconUtils::SetSize(iBitmap, navi_cale_icon_ev2_pane.Size());
    AknIconUtils::SetIconColor(iBitmap,iCalendarColor);
    gc.BitBlt(navi_cale_icon_ev2_pane.iTl,iBitmap);
    TAknTextLineLayout navi_cale_text_pane_t1 = 
                        AknLayoutScalable_Apps::navi_pane_bcale_mc_t1(0).LayoutLine();

    // Get heading layout properties
    // from field_cale_ev2_pane_t1 with list_cale_ev2_pane as parent
    TAknLayoutText headingTextLayout;
    headingTextLayout.LayoutText(navi_pane, navi_cale_text_pane_t1);
    
    const CFont* font = CCoeEnv::Static()->NormalFont();
    gc.UseFont(font);
    // If you see bright red text, the skins api is broken!
    TRgb textColourFromSkin(255,0,0);
    // Does not transfer ownership.
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::GetCachedColor(skin, 
                     textColourFromSkin, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG2);
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    // Set the text colour we found.
    charFormat.iFontPresentation.iTextColor = TLogicalRgb(textColourFromSkin);
    charFormatMask.SetAttrib(EAttColor);

	headingTextLayout.DrawText( gc, *iCalendarName, ETrue, charFormat.iFontPresentation.iTextColor );
    gc.DiscardFont();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCustomNaviControl::Sizechanged
// Child control was resized.
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCustomNaviControl::SizeChanged()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCustomNaviControl::CountComponentControls
// Return child control count.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCustomNaviControl::CountComponentControls() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return 0;
    }

// ----------------------------------------------------------------------------
// CCustomNaviControl::ComponentControl
// Return child control pointer.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCustomNaviControl::ComponentControl(TInt /*aIndex*/) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    
    return NULL;
    }

// end of file
