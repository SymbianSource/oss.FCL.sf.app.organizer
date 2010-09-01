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
* Description:  An underline is drawn for DayView to TitlePane.
 *
*/

//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calentitlepane.h"

#include <eikspane.h>
#include <eiklabel.h>
#include <avkon.hrh>

//  LOCAL CONSTANTS AND MACROS

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenTitlePane& CCalenTitlePane::StaticL()
    {
    TRACE_ENTRY_POINT;
    
    CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
    CCalenTitlePane* tp = static_cast<CCalenTitlePane*>(sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
    
    TRACE_EXIT_POINT;
    return *tp;
    }

// Two-phased constructor.;
// Creates CCalenTitlePane, and swaps previous CAknTitlePane
EXPORT_C CCalenTitlePane* CCalenTitlePane::NewAndSwapL(CEikStatusPane* aPane) 
    {
    TRACE_ENTRY_POINT;
    
    CCalenTitlePane* self = new(ELeave)CCalenTitlePane;
    CleanupStack::PushL(self);
    CCoeControl* previous = aPane->SwapControlL(
        TUid::Uid(EEikStatusPaneUidTitle), self);

    TRect previousRect(0, 0, 0, 0);
    if (previous)
        {
        previousRect = previous->Rect();
        delete previous;
        previous = NULL;
        }

    CCoeControl* ctrl = aPane->ContainerControlL(
        TUid::Uid(EEikStatusPaneUidTitle));
    self->SetContainerWindowL(*ctrl);
    self->ConstructL();
    self->SetRect(previousRect);
    self->ActivateL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }


// Destructor
EXPORT_C CCalenTitlePane::~CCalenTitlePane()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenTitlePane::SetUnderLine
// Set underline flag to CEikLabel
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenTitlePane::SetUnderLine(TBool aUnderline)
    {
    TRACE_ENTRY_POINT;
    
    CEikLabel* label = static_cast<CEikLabel*>( ComponentControl(0) );

    if (iOrgContext)
        {
        iOrgContext = ControlContext();
        }
    SetControlContext(this);

    label->SetUnderlining(aUnderline);
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenTitlePane::UnderLine
// Returns underline flag
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CCalenTitlePane::UnderLine()
    {
    TRACE_ENTRY_POINT;
    
    CEikLabel* label = static_cast<CEikLabel*>( ComponentControl(0) );
    
    TRACE_EXIT_POINT;
    return label->IsUnderlined();
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenTitlePane::CCalenTitlePane()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// CCalenTitlePane::PrepareContext
// Relpaces from a nallow font to a bold font
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenTitlePane::PrepareContext(CWindowGc& aGc) const
    {
    TRACE_ENTRY_POINT;

    if (iOrgContext)
        {
        iOrgContext->PrepareContext(aGc);
        }

    TRACE_EXIT_POINT;
    }

// End of File
