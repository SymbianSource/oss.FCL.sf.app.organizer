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
* Description:   Customized drawing of CCalenWeekLBData.
 *                DrawItemText() passes extended parameters to CCalenWeekLBData.
 *
*/



//debug
#include "calendarui_debug.h"

//  INCLUDE FILES
#include "calenweeklistboxitemdrawer.h"

#include "calenweeklistboxdata.h"
#include "calenweekdata.h"

#include "calendar.hrh"

#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <gulicon.h>

#include <eikappui.h>


// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code, that
// might leave.
//
CCalenWeekListBoxItemDrawer::CCalenWeekListBoxItemDrawer(
    MTextListBoxModel* aTextListBoxModel,
    const CFont* aFont,
    CFormattedCellListBoxData* aFormattedCellData)
    : CFormattedCellListBoxItemDrawer(aTextListBoxModel, aFont, aFormattedCellData)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// Destructor
CCalenWeekListBoxItemDrawer::~CCalenWeekListBoxItemDrawer()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenWeekListBoxItemDrawer::DrawItemText
// Draws grid lines of one list row of Calendar week view.
// Calls CCalenWeekLBData to draw contents of cells 
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCalenWeekListBoxItemDrawer::DrawItemText(TInt aItemIndex,               // Item index
                                               const TRect& aItemTextRect,    // Item rect
                                               TBool aItemIsCurrent,          // Current Item flag
                                               TBool /*aViewIsEmphasized*/,   // ignored.
                                               TBool /*aItemIsSelected*/) const // Item hilighted flag
    {
    TRACE_ENTRY_POINT;
    
    // We use custom DrawItemText, because Avkon's 
    // CFormattedCellListBoxItemDrawer does a whole lotta things,
    // e.g. background drawing, separator lines etc. 
    // Though it seems that it uses Enabled/Disabled functions for most 
    // of them, and setting properties properly could make this override
    // unnecessary.
    // It seems safer to have override, so that Avkon changes don't break
    // Calendar. Avkon implementation of new features probably don't do
    // it for Calendar week view in any case.

    iGc->SetPenColor(iTextColor);
    iGc->SetBrushColor(iBackColor);

    SetupGc(aItemIndex);

    TBufC<256> target(iModel->ItemText(aItemIndex));

    CFormattedCellListBoxData::TColors colors;
    colors.iText = iTextColor;
    colors.iBack = iBackColor;
    colors.iHighlightedText = iHighlightedTextColor;
    colors.iHighlightedBack = iHighlightedBackColor;

    CCalenWeekLBData* data =
        static_cast<CCalenWeekLBData*>( FormattedCellData() );
    data->Draw(Properties(aItemIndex), *iGc,
               &target, aItemTextRect, aItemIsCurrent, colors);
    
    TRACE_EXIT_POINT;
    }

// End of File
