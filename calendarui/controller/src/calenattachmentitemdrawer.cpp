/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

// ========== INCLUDE FILES ================================
#include <msvapi.h>
#include <eikspane.h>
#include <eikmenup.h>
#include <AknDef.h>
#include <aknappui.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <aknlists.h>
#include <gulicon.h>
#include <coeutils.h>               // for ConeUtils::FileExists
#include <bautils.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>          // CAknWaitDialog
#include <DocumentHandler.h>
#include <StringLoader.h>           // StringLoader
#include <AiwCommon.h>
#include <NpdApi.h>



#include <avkon.hrh>

#include <aknlayout.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include <AknUtils.h>
#include <AknsUtils.h>          // skinned icons
#include <AknsConstants.h>      // skinned icon ids

#include "calenattachmentinfo.h"
#include "calenattachmentmodel.h"
#include "calenviewattachmentsdialog.h"
#include "calenattachmentutils.h"
#include "calenattalistitemarray.h"
#include "calenattachmentitemdrawer.h"
#include "calendarui_debug.h"
#include <calendar.mbg>
#include "calendar.hrh"
#include <Calendar.rsg>       // resouce identifiers
#include <calencommonui.rsg>

#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>


// ---------------------------------------------------------
// CCalenAttachmentItemDrawer::CCalenAttachmentItemDrawer
//
//
// ---------------------------------------------------------
//
CCalenAttachmentItemDrawer::CCalenAttachmentItemDrawer(
    MTextListBoxModel* aTextListBoxModel,
    const CFont* aFont,
    CFormattedCellListBoxData* aFormattedCellData,
    CCalenAttachmentModel& aAttachmentModel,
    CCalenAttachmentsListBox* aAttaListBox )
    : CDoubleLargeStyleItemDrawer( aTextListBoxModel, aFont, aFormattedCellData, aAttaListBox ),
      iAttachmentModel( aAttachmentModel )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CCalenAttachmentItemDrawer::DrawItem
//
//
// ---------------------------------------------------------
//
void CCalenAttachmentItemDrawer::DrawItem(
    TInt aItemIndex,
    TPoint aItemRectPos,
    TBool aItemIsSelected,
    TBool aItemIsCurrent,
    TBool aViewIsEmphasized,
    TBool aViewIsDimmed ) const
    {   
    TRACE_ENTRY_POINT;
    


    CCalenAttachmentInfo& currAttInfo =
            iAttachmentModel.AttachmentInfoAt( aItemIndex );
    
    iGc->SetPenColor(iTextColor);
    iGc->SetBrushColor(iBackColor);
    
    SetupGc(aItemIndex);
    
   // if ( currAttInfo.IsSeparator() )
        {
        // draw separator line before item.
        
        // Resolve separator layout
        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
        TAknLayoutRect listPaneLayout;
        listPaneLayout.LayoutRect(
            mainPane,
            AknLayoutScalable_Avkon::list_gen_pane( 0 ).LayoutLine() );
        TAknLayoutRect doubleGraphicLayout;
        doubleGraphicLayout.LayoutRect(
            listPaneLayout.Rect(),
            AknLayoutScalable_Avkon::list_double_large_graphic_pane( 0 ).LayoutLine() );
        TAknLayoutRect separator;
        separator.LayoutRect(             
            doubleGraphicLayout.Rect(),
            AknLayoutScalable_Avkon::list_double_large_graphic_pane_g5( 1 ).LayoutLine() );

        MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal( iGc );
        if ( transApi )
            {
            TRect lineRect( TPoint( separator.Rect().iTl.iX, aItemRectPos.iY ),
                            TPoint( separator.Rect().iBr.iX, aItemRectPos.iY + separator.Rect().Height() ) );
            transApi->BeginRedraw( MAknListBoxTfxInternal::EListItem, lineRect, aItemIndex );
            transApi->StartDrawing( MAknListBoxTfxInternal::EListItem );
            }
        /*iGc->DrawLine( TPoint( separator.Rect().iTl.iX, aItemRectPos.iY ),
                       TPoint( separator.Rect().iBr.iX, aItemRectPos.iY ) );*/
                       
        // Reset gc because empty area is drawn after this.
        // Otherwise lines in empty area is drawn with line color!
        //iGc->Reset();
        CDoubleLargeStyleItemDrawer::DrawItem(
            aItemIndex,
            aItemRectPos,
            aItemIsSelected,
            aItemIsCurrent,
            aViewIsEmphasized,
            aViewIsDimmed );
        
        
        if ( transApi )
            {
            transApi->StopDrawing();
            transApi->EndRedraw( MAknListBoxTfxInternal::EListItem, aItemIndex );
            }
        }
    TRACE_EXIT_POINT;
    }


// End of File
