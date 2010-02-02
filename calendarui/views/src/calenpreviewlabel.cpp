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
* Description:   Storage for single line of data in preview.
 *
*/


//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calenpreviewlabel.h"

#include "calenpreviewlabelobserver.h"
#include <gulicon.h>

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewLabel::CCalenPreviewLabel() 
    : iIcon(NULL), iMoreIcon(NULL)
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
CCalenPreviewLabel::~CCalenPreviewLabel()
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
CCalenPreviewLabel* CCalenPreviewLabel::NewL() 
    {
    TRACE_ENTRY_POINT;
    
    CCalenPreviewLabel* self = new (ELeave) CCalenPreviewLabel();
    
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewLabel::SetLabelObserver(
    MCalenPreviewLabelObserver* aObserver)
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
void CCalenPreviewLabel::SetIcon(CGulIcon* aIcon)
    {
    TRACE_ENTRY_POINT;
    iIcon = aIcon;
    TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CGulIcon* CCalenPreviewLabel::Icon()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iIcon;
    }
// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CGulIcon* CCalenPreviewLabel::MoreIcon()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iMoreIcon;
    }
// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewLabel::SetMoreIcon(CGulIcon* aIcon)
    {
    TRACE_ENTRY_POINT;
    iMoreIcon = aIcon;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewLabel::SetLabelType(TPreviewLabelType aType)
    {
    TRACE_ENTRY_POINT;
    iType = aType;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewLabel::TPreviewLabelType CCalenPreviewLabel::LabelType() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iType;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewLabel::HandleVisualisationL(TPreviewVisualisation aCommand)
    {
    TRACE_ENTRY_POINT;
    if(aCommand == EAddVisualisation)
        {
        SetUnderlining(ETrue); // underline color change or what ever
        Window().Invalidate(Rect());
        }
    else
        {
        SetUnderlining(EFalse);
        Window().Invalidate(Rect());
        }
    TRACE_EXIT_POINT;
    }
   
// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewLabel::SetIconLayout(TAknLayoutRect aLayout)
    {
    TRACE_ENTRY_POINT;
    iIconLayout = aLayout;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TRect CCalenPreviewLabel::IconRect()const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iIconLayout.Rect();
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewLabel::SetMoreIconLayout(TAknLayoutRect aLayout)
    {
    TRACE_ENTRY_POINT;
    iMoreIconLayout = aLayout;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TRect CCalenPreviewLabel::MoreIconRect() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iMoreIconLayout.Rect();
    }

//  End of File
