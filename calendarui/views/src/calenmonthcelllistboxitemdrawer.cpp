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
* Description:   Preparing to draw a cell (setting color and underline).
 *                Moreover, drawing cell's bottom and right lines.
 *
*/


//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calenmonthcelllistboxitemdrawer.h"

#include <calendateutils.h>
#include "calenmonthgrid.h"

#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <eikfrlbd.h>

// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code, that
// might leave.
//
CCalenMonthCellListBoxItemDrawer::CCalenMonthCellListBoxItemDrawer( MTextListBoxModel* aTextListBoxModel,
                                                                    CCalenMonthGrid* aCalendarMonthGrid,
                                                                    const CFont* aFont,
                                                                    CFormattedCellListBoxData* aFormattedCellData)
    : CFormattedCellListBoxItemDrawer( aTextListBoxModel, aFont, aFormattedCellData ),
      iCalendarMonthGrid( aCalendarMonthGrid )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// Destructor
CCalenMonthCellListBoxItemDrawer::~CCalenMonthCellListBoxItemDrawer()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenMonthCellListBoxItemDrawer::DrawItemText
// Prepareing to draw a cell(setting color and underline).
// And drawing cell's bottom and right lines.
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CCalenMonthCellListBoxItemDrawer::DrawItemText(TInt aItemIndex, const TRect& aItemTextRect, TBool aItemIsCurrent, TBool /*aViewIsEmphasized*/, TBool aItemIsSelected) const
    {
    TRACE_ENTRY_POINT;
    
    iGc->SetPenColor(iTextColor);
    iGc->SetBrushColor(iBackColor);

    SetupGc(aItemIndex);

    // Skin support
    MAknsSkinInstance *skin = AknsUtils::SkinInstance();

    TBool skinUsed = EFalse;
    if(FormattedCellData()->Control())
        {
        MAknsControlContext *cc = AknsDrawUtils::ControlContext( FormattedCellData()->Control() );

        if(!cc)
            {
            cc = FormattedCellData()->SkinBackgroundContext();
            }
        skinUsed = AknsDrawUtils::Background( skin, cc, NULL, *iGc, aItemTextRect, KAknsDrawParamNoClearUnderImage);
        }

    if( !skinUsed )
        {
        iGc->Clear( aItemTextRect );
        }

    // Setup colors, mainly try to fetch them from skins. 
    // FIXME: investigate if this could be done somewhere else, so that we set them to 
    // properties of grid

    CFormattedCellListBoxData::TColors colors;
    colors.iBack = iBackColor;
    colors.iText = iTextColor;

    if(aItemIsSelected)
        { 
        // active month days
        if(aItemIsCurrent) 
            { 
            // highlighted day's number is drawn with grid highlight color
            AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), 
                                      colors.iText, 
                                      KAknsIIDQsnTextColors, 
                                      EAknsCIQsnTextColorsCG11);

            }
        else
            { 
            // normal active months days' numbers are drawn 
            // with month view text color
            AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), 
                                      colors.iText, 
                                      KAknsIIDQsnTextColors, 
                                      EAknsCIQsnTextColorsCG22);
            }

        }
    else
        { 
        // passive month days
        AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), 
                                  colors.iText, 
                                  KAknsIIDQsnTextColors, 
                                  EAknsCIQsnTextColorsCG21);
        }

    // set underline if current day is today.
    // (current day is day of item.)
    TTime today( CalenDateUtils::Today() );
    TTime currentDay( iCalendarMonthGrid->FirstDayOfGrid() +TTimeIntervalDays( aItemIndex ) );

    TBool underline( CalenDateUtils::OnSameDay( today, currentDay ) );
    iGc->SetUnderlineStyle( underline ? EUnderlineOn : EUnderlineOff );

    TPtrC cellData( iModel->ItemText(aItemIndex) );
    FormattedCellData()->Draw(Properties(aItemIndex), *iGc, &cellData, aItemTextRect, aItemIsCurrent, colors);
    
    TRACE_EXIT_POINT;
    }


// End Of File
