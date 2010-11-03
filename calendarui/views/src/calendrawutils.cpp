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
* Description:   Drawing utilities for Arabic and Hebrew layout
 *
*/



//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calendrawutils.h"

#include <AknsDrawUtils.h>
#include <AknIconUtils.h>
#include <AknUtils.h>
#include <gulicon.h>
#include <w32std.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CalenDrawUtils::TextPos
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CalenDrawUtils::TextPos(TPoint* aResultArray, const TAknTextLineLayout& aLayout, TSize aItemSize)
    {
    TRACE_ENTRY_POINT;
    
    TRect itemRect( TPoint(0,0), aItemSize );
    TAknLayoutText layout;
    layout.LayoutText( itemRect, aLayout );
    aResultArray[0] = layout.TextRect().iTl;
    aResultArray[1] = layout.TextRect().iBr;
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CalenDrawUtils::GfxPos
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CalenDrawUtils::GfxPos(TPoint* aResultArray, const TAknWindowLineLayout& aLayout, TSize aItemSize)
    {
    TRACE_ENTRY_POINT;
    
    TRect itemRect( TPoint(0,0), aItemSize );
    TAknLayoutRect layout;
    layout.LayoutRect( itemRect, aLayout );
    aResultArray[0] = layout.Rect().iTl;
    aResultArray[1] = layout.Rect().iBr;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CalenDrawUtils::SetIconSizes(const TAknLayoutRect& aIconLayout, 
                                  CArrayPtr<CGulIcon>* aIconArray)
    {
    TRACE_ENTRY_POINT;
    
    if (aIconArray) // this can be done only if iconarray is set
        {
        TInt iconCount( aIconArray->Count() );
        for( TInt i(0); i < iconCount; ++i )
            {
            // note  AknIconUtils::SetSize sets both mask and bitmap size.
            // regardless which is given.
            AknIconUtils::SetSize( (*aIconArray)[i]->Bitmap(), aIconLayout.Rect().Size() );
            }
        }
    
    TRACE_EXIT_POINT;
    }


// End of file
