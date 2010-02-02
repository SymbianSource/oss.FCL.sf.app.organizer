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
* Description:  Customized drawing of CCalenDayListBoxData.
 *                DrawItemText() passes extended parameters to CCalenDayListBoxData.
 *
*/


//debug
#include "calendarui_debug.h"

//  INCLUDE FILES
#include "calendaylistboxitemdrawer.h"
#include "calendaylistboxview.h"
#include "calendaylistbox.h"
#include "calendaycontainer.h"


#include <AknUtils.h>
#include <eikfrlbd.h>
#include <aknlayoutscalable_apps.cdl.h>
#define CHECK_IF_FIRSTLINE(a)  static_cast<CCalenDayListBoxView*>(iListBox->View())->IsFirstLineOfCell(a)

// ================= MEMBER FUNCTIONS =======================

// C++ constructor can NOT contain any code, that
// might leave.
//
CCalenDayListBoxItemDrawer::CCalenDayListBoxItemDrawer( MTextListBoxModel* aTextListBoxModel,
                                                        const CFont* aFont,
                                                        CFormattedCellListBoxData* aFormattedCellData,
                                                        CEikListBox* aListBox )
    : CFormattedCellListBoxItemDrawer(aTextListBoxModel, aFont, aFormattedCellData),
      iListBox( aListBox )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// Destructor
CCalenDayListBoxItemDrawer::~CCalenDayListBoxItemDrawer()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenDayListBoxItemDrawer::DrawItemText
// passes a parameter to CFormattedCellListBoxData.
// (other items were commented in a header).
// ---------------------------------------------------------
//
_LIT(EEmptyString, "");
void CCalenDayListBoxItemDrawer::DrawItemText
(TInt aItemIndex,               // Item index
 const TRect& aItemTextRect,    // Item rect
 TBool aItemIsCurrent,          // Current Item flag
 TBool aViewIsEmphasized,   // ignored.
 TBool /*aItemIsSelected*/) const // Item hilighted flag
    {
    TRACE_ENTRY_POINT;
    
    // Based on AVKON CFormattedCellListBoxItemDrawer
    // Removed code is commented out 
    iGc->SetPenColor(iTextColor);
    iGc->SetBrushColor(iBackColor);

    TPtrC temp=iModel->ItemText(aItemIndex);

    SetupGc(aItemIndex);

    TBufC<KMaxTotalDataLength> target(EEmptyString);
//     TBool removeicon = (!aItemIsSelected && !ItemMarkReverse()) || 
//                         (aItemIsSelected && ItemMarkReverse());
//     if( Flags() & EDrawMarkSelection && ItemMarkPosition() != -1 
//         && removeicon)
//     {
//         TPtrC repl = ItemMarkReplacement();
//         AknLAFUtils::ReplaceColumn(target.Des(), &temp, &repl, '\t', 
//                                    ItemMarkPosition());
//     }
//     else
//     {
    TPtrC empty(KNullDesC);
    AknLAFUtils::ReplaceColumn(target.Des(), &temp, &empty, '\t', -1);
//     }

    CFormattedCellListBoxData::TColors colors;
    colors.iText=iTextColor;
    colors.iBack=iBackColor;
    colors.iHighlightedText=iHighlightedTextColor;
    colors.iHighlightedBack=iHighlightedBackColor;

//    MAknsSkinInstance *skin = AknsUtils::SkinInstance();
//     if(FormattedCellData()->Control())
//     {
//         MAknsControlContext *cc = AknsDrawUtils::ControlContext( 
//                                   FormattedCellData()->Control() );
//         if(!cc)
//             cc = FormattedCellData()->SkinBackgroundContext();
//         AknsDrawUtils::Background( skin, cc, FormattedCellData()->Control(),
//                                    *iGc, aItemTextRect );
//     }
//     else
//     {
//         iGc->Clear(aItemTextRect);
//     }

    TBool highlightShown = ETrue;

    if(FormattedCellData()->RespectFocus() && !aViewIsEmphasized)
        highlightShown = EFalse;

    FormattedCellData()->Draw(Properties(aItemIndex), *iGc, &target, 
                              aItemTextRect, aItemIsCurrent&&highlightShown, 
                              colors);
    
    
    
    if(CHECK_IF_FIRSTLINE(aItemIndex))
        {
        CCalenDayContainer::SItemInfo& itemInfo = 
                           static_cast<CCalenDayListBox*>(iListBox)->iDayContainer->FindItem(aItemIndex);
        iColor = itemInfo.iColor;
        }
    

    TAknWindowComponentLayout tempLayout =
                    AknLayoutScalable_Apps::list_cale_time_pane_g6(
                            aItemIndex - static_cast<CCalenDayListBox*> (iListBox)->TopItemIndex());

    TAknLayoutRect colourstrips;
    colourstrips.LayoutRect( aItemTextRect, tempLayout.LayoutLine() );
    TRect finalRect(colourstrips.Rect());
    finalRect.SetHeight(aItemTextRect.Height());
    iGc->DrawRect( colourstrips.Rect() );
    iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
    iGc->SetBrushColor( TRgb(iColor) );
    iGc->DrawRect( finalRect );
    
    iGc->Reset();
    
    TRACE_EXIT_POINT;
    }

// End of File
