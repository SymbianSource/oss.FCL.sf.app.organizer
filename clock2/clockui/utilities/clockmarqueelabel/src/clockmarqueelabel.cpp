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
* Description:   This is the source file for the CClockMarqueeLabel class.
*
*/

// System includes
#include <AknsSkinInstance.h>
#include <AknsDrawUtils.h>
#include <AknsUtils.h>
#include <AknMarqueeControl.h>
#include <AknUtils.h>
#include <AknLayoutFont.h>
#include <AknBidiTextUtils.h>

// User includes
#include "clockmarqueelabel.h"
#include "clock_debug.h"

// Constants
const TInt KNumLoops( 1 );

// ---------------------------------------------------------
// CClockMarqueeLabel::CClockMarqueeLabel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockMarqueeLabel::CClockMarqueeLabel()
    {
    __PRINTS( "CClockMarqueeLabel::NewL - Entry" );
    
    // No implementation yet.
    
    __PRINTS( "CClockMarqueeLabel::NewL - Exit" );
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMarqueeLabel::ConstructL( TBool aEnableMarquee )
    {
    __PRINTS( "CClockMarqueeLabel::ConstructL - Entry" );
    
    // Check if marquee has to be enabled.
    if ( aEnableMarquee )
        {
        // Contruct the marquee control.
        iMarqueeControl = CAknMarqueeControl::NewL();
        // Set the redraw callback for the marquee control
        TCallBack callback( RedrawCallback, this );
        iMarqueeControl->SetRedrawCallBack( callback );
        // Set the number of loops.
        iMarqueeControl->SetLoops( KNumLoops );
        iMarqueeControl->SetContainerWindowL( *this );
        }
    
    __PRINTS( "CClockMarqueeLabel::ConstructL - Exit" );
    }
    
// ---------------------------------------------------------
// CClockMarqueeLabel::~CClockMarqueeLabel
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockMarqueeLabel::~CClockMarqueeLabel()
    {
    __PRINTS( "CClockMarqueeLabel::~CClockMarqueeLabel - Entry" );
    
    if( iLabelText )
        {
        delete iLabelText;
        iLabelText = NULL;
        }
    if( iMarqueeControl )
        {
        delete iMarqueeControl;
        iMarqueeControl = NULL;
        }
    iBgContext = NULL;
    
    __PRINTS( "CClockMarqueeLabel::~CClockMarqueeLabel - Exit" );
    }
    
// ---------------------------------------------------------
// CClockMarqueeLabel::SetBackground
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMarqueeLabel::SetBackground( MAknsControlContext* aBgContext )
    {
    __PRINTS( "CClockMarqueeLabel::SetBackground - Entry" );
    
    iBgContext = aBgContext;
    
    __PRINTS( "CClockMarqueeLabel::SetBackground - Exit" );
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::RedrawRect
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMarqueeLabel::RedrawRect( const TRect& aRect ) const
    {
    __PRINTS( "CClockMarqueeLabel::RedrawRect - Entry" );
    
    // Check if the rectangle to be redrawn is within the bounds of own rect.
    if ( aRect.Intersects( Rect() ) )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        
        CWindowGc& gc = SystemGc();
        if ( iBgContext )
            {
            // Set the background skin.
            AknsDrawUtils::Background( skin, iBgContext, gc, Rect() );
            }

        CGraphicsContext::TTextAlign align = iAlignment.TextAlign();

        // Get baseline for current font
        TInt baselineOffset = 0;
        if ( iMarqueeControl )
            {
            const CAknLayoutFont* layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull( Font() );
            if ( layoutFont )
                {
                baselineOffset= layoutFont->TextPaneTopToBaseline();
                }
            }

        // Set the font and the pen color.
        gc.UseFont( Font() );
        
        TRgb cachedColor;
        // Get the cached color.
        AknsUtils::GetCachedColor( skin, cachedColor, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );
        gc.SetPenColor( cachedColor );
        
        // Draw marquee, if supported and necessary.
        if ( !iMarqueeControl || !iMarqueeControl->IsMarqueeOn() ||
             iMarqueeControl->DrawText( gc,
                                        Rect(),
                                        FullText(),
                                        baselineOffset,
                                        align,
                                        *Font() ) )
            {
            // Marquee is not enabled or all the loops have been executed
            // The text needs to be drawn here.
            // iMarqueeControl->Stop();
            __PRINTS( "Marquee has finished executing all loops" );
            gc.DrawText( *Text(), aRect, baselineOffset );
            }
        
        }
    
    __PRINTS( "CClockMarqueeLabel::RedrawRect - Exit" );
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::StartMarquee
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMarqueeLabel::StartMarquee()
    {
    __PRINTS( "CClockMarqueeLabel::StartMarquee - Entry" );
    
    if ( iMarqueeControl )
        {
        // Check if scrolling is required.
        TBool needToScroll = NeedToScroll();
        if ( needToScroll && !iMarqueeControl->IsMarqueeOn() )
            {
            __PRINTS( "Scrolling required" );
            // Reset the marquee control.
            iMarqueeControl->Reset();
            }
        // Enable the marquee.
        iMarqueeControl->EnableMarquee( needToScroll );
        if ( needToScroll )
            {
            // Start marquee.
            iMarqueeControl->Start();
            }
        else
            {
            __PRINTS( "Scrolling not required" );
            RedrawRect( Rect() );
            }
        }
    
    __PRINTS( "CClockMarqueeLabel::StartMarquee - Exit" );
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::StopMarquee
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMarqueeLabel::StopMarquee()
    {
    __PRINTS( "CClockMarqueeLabel::StopMarquee - Entry" );
    
    if ( iMarqueeControl )
        {
        iMarqueeControl->EnableMarquee( EFalse );
        iMarqueeControl->Stop();
        }
    
    __PRINTS( "CClockMarqueeLabel::StopMarquee - Exit" );
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::FullText
// rest of the details are commented in the header
// ---------------------------------------------------------
//
const TDesC& CClockMarqueeLabel::FullText() const
    {
    __PRINTS( "CClockMarqueeLabel::FullText - Entry" );
    
    if ( iLabelText )
        {
        __PRINTS( "CClockMarqueeLabel::FullText - Exit" );
        
        return *iLabelText;
        }
    else
        {
        __PRINTS( "CClockMarqueeLabel::FullText - Exit" );
        
        return *Text();
        }
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::SetLabelTextL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMarqueeLabel::SetLabelTextL( const TDesC& aText )
    {
    __PRINTS( "CClockMarqueeLabel::SetLabelTextL - Entry" );
    
    // Clear the previuosly set text.
    if ( iMarqueeControl )
        {
        delete iLabelText;
        iLabelText = NULL;
        iLabelText = aText.AllocL();
        }

    // Allocate 3 chars more ( make sure "..." fits ).
    HBufC* buf = HBufC::NewLC( aText.Length() + 3 );
    *buf = aText;
    TPtr ptr( buf->Des() );
    AknBidiTextUtils::ConvertToVisualAndClipL( ptr, *Font(), Rect().Width(), Rect().Width() );
    SetTextL( *buf );
    CleanupStack::PopAndDestroy( buf );
    
    __PRINTS( "CClockMarqueeLabel::SetLabelTextL - Exit" );
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::ResetMarquee
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMarqueeLabel::ResetMarquee()
    {
    __PRINTS( "CClockMarqueeLabel::ResetMarquee - Entry" );
    
    iMarqueeControl->Reset();
    
    __PRINTS( "CClockMarqueeLabel::ResetMarquee - Exit" );
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::Draw
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMarqueeLabel::Draw( const TRect& aRect ) const
    {
    __PRINTS( "CClockMarqueeLabel::Draw - Entry" );
    
    RedrawRect( aRect );
    
    __PRINTS( "CClockMarqueeLabel::Draw - Exit" );
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::CountComponentControls
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockMarqueeLabel::CountComponentControls() const
    {
    __PRINTS( "CClockMarqueeLabel::CountComponentControls - Entry" );
    
    TInt componentCount = 0;
    if ( iMarqueeControl )
        {
        componentCount++;
        }
    
    __PRINTS( "CClockMarqueeLabel::CountComponentControls - Exit" );
    
    return componentCount;
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::ComponentControl
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CCoeControl* CClockMarqueeLabel::ComponentControl( TInt aIndex ) const
    {
    __PRINTS( "CClockMarqueeLabel::ComponentControl - Entry" );
    
    ASSERT( aIndex == 0 );
    
    __PRINTS( "CClockMarqueeLabel::ComponentControl - Exit" );
    
    return iMarqueeControl;
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::NeedToScroll
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockMarqueeLabel::NeedToScroll() const
    {
    __PRINTS( "CClockMarqueeLabel::NeedToScroll - Entry" );
    
    TBool needToScroll( EFalse );
	// Check if the text fits in the width of the label
    if ( iLabelText && iFont->TextWidthInPixels( *iLabelText ) > Size().iWidth )
        {
        needToScroll = ETrue;
        }
    
    __PRINTS( "CClockMarqueeLabel::NeedToScroll - Exit" );
    
    return needToScroll;
    }

// ---------------------------------------------------------
// CClockMarqueeLabel::RedrawCallback
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockMarqueeLabel::RedrawCallback( TAny* aPtr )
    {
    __PRINTS( "CClockMarqueeLabel::RedrawCallback - Entry" );
    
    CClockMarqueeLabel* self = static_cast< CClockMarqueeLabel* >( aPtr );
    CWindowGc& gc = self->SystemGc();
    gc.Activate( *self->DrawableWindow() );
    self->Draw( self->Rect() );
    gc.Deactivate();

    __PRINTS( "CClockMarqueeLabel::RedrawCallback - Exit" );
    
    // Redraw again.
    return TRUE;
    }

// End of file
