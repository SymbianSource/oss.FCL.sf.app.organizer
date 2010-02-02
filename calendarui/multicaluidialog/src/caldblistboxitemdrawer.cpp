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
 *  Description : Implemrntation class for calenlistdialog .
 */

//System include
#include <calsession.h>
#include <calentryview.h>
#include <calcalendarinfo.h>
#include <StringLoader.h>
#include <AknsBasicBackgroundControlContext.h>
#include <aknconsts.h>
#include <AknIconArray.h>
#include <aknlists.h>
#include <akntitle.h>
#include <eikclbd.h>
#include <aknnavi.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <data_caging_path_literals.hrh>
#include <pathinfo.h>
#include <bautils.h>
#include <aknenv.h>
#include <eikapp.h>
#include <eikenv.h>
#include <gulicon.h>
#include <multicaluidialog.rsg>
#include <multicaluidialog.mbg>

//User include
#include "multicaluidialogimpl.h"
#include "multicaluidialogmodel.h"
#include "calendarui_debug.h"
#include "caldblistboxitemdrawer.h"



// ----------------------------------------------------------------------------
// CCalenMultipleDbListboxItemDrawer::CCalenMultipleDbListboxItemDrawer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenDbListboxItemDrawer::CCalenDbListboxItemDrawer(MTextListBoxModel* aTextListBoxModel,
                                                     const CFont* aFont,
                                                     CFormattedCellListBoxData* aFormattedCellData,
                                                     CEikListBox* aListBox ,
                                                     const RArray<TInt32>& aColorUidArray)
    : CFormattedCellListBoxItemDrawer(aTextListBoxModel,aFont,aFormattedCellData),
      iListBox(aListBox),
      iColorUidArray(aColorUidArray)
    
    {
    TRACE_ENTRY_POINT
    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbListboxItemDrawer::CCalenMultipleDbListboxItemDrawer
// (other items were commented in a header). 
// ----------------------------------------------------------------------------
//
CCalenDbListboxItemDrawer::~CCalenDbListboxItemDrawer()
    {
    TRACE_ENTRY_POINT
    
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbListboxItemDrawer::DrawItem
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenDbListboxItemDrawer::DrawItem(TInt aItemIndex, 
                                         TPoint aItemRectPos, 
                                         TBool aItemIsSelected, 
                                         TBool aItemIsCurrent, 
                                         TBool aViewIsEmphasized,
                                         TBool aViewIsDimmed) const
     {
     TRACE_ENTRY_POINT
     TRect actualItemRect(aItemRectPos, iItemCellSize);
     SetupGc(aItemIndex);
         
     CFormattedCellListBoxItemDrawer::DrawItem(aItemIndex,
                                                    aItemRectPos,
                                                    aItemIsSelected,
                                                    aItemIsCurrent,
                                                    aViewIsEmphasized,
                                                    aViewIsDimmed);
     
     iGc->Reset();
     
	 // Ggetting the layout for clolourstrips for list dialog
	 TAknWindowComponentLayout tempLayout = 
             AknLayoutScalable_Apps::list_cale_time_pane_g6( aItemIndex ) ;     

     TAknLayoutRect colourstrips;
     colourstrips.LayoutRect( actualItemRect, tempLayout.LayoutLine() );
      
     iGc->SetPenColor( CGraphicsContext::ENullPen );
     iGc->DrawRect(  colourstrips.Rect() );
     iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
     iGc->SetBrushColor( TRgb(iColorUidArray[aItemIndex] ));
     iGc->DrawRect(  colourstrips.Rect() );   
 
     
     iGc->Reset();
     
     TRACE_EXIT_POINT
     }



// End of file. 
