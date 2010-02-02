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
* Description: Layout calculations for PreviewPane.
 *
*/


//debug
#include "calendarui_debug.h"

#include "calenpreviewlayoutmanager.h"

#include "calenpreviewlabel.h"

#include "calenpreview.h"

#include <AknsUtils.h>
#include <eiklabel.h>

#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>

const TInt KPopupMaxEntries = 4;

CCalenPreviewLayoutManager* CCalenPreviewLayoutManager::NewL(CCalenPreview* aPreview)
    {
    TRACE_ENTRY_POINT;
    CCalenPreviewLayoutManager* self = 
        new (ELeave) CCalenPreviewLayoutManager(aPreview);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

CCalenPreviewLayoutManager::CCalenPreviewLayoutManager(CCalenPreview* aPreview)
    : iContainer(NULL) , iPreview(aPreview)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

CCalenPreviewLayoutManager::~CCalenPreviewLayoutManager()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

void CCalenPreviewLayoutManager::UpdateLayoutData()
    {
    TRACE_ENTRY_POINT;
    if( UsePreview() )
	{
        TAknLayoutRect list_fp_cale_pane;
        TAknLayoutRect popup_fixed_preview_cale_window;
        TAknLayoutText list_single_fp_cale_pane;
        TAknTextLineLayout textLine;

        popup_fixed_preview_cale_window.LayoutRect( 
            ApplicationWindow(), 
            AknLayoutScalable_Apps::popup_fixed_preview_cale_window(Orientation()).LayoutLine() );
        
        TAknLayoutScalableParameterLimits list_single_fp_cale_pane_limits =
            AknLayoutScalable_Apps::list_single_fp_cale_pane_ParamLimits();
        
        iLinesUsed = LineCount();
        
        list_fp_cale_pane.LayoutRect( 
            popup_fixed_preview_cale_window.Rect(), 
            AknLayoutScalable_Apps::list_fp_cale_pane(0).LayoutLine() );
                
        list_single_fp_cale_pane.LayoutText( 
            list_fp_cale_pane.Rect(), 
            AknLayoutScalable_Apps::list_single_fp_cale_pane_t(0, 0).LayoutLine() );
        
        textLine = 
            AknLayoutScalable_Apps::list_single_fp_cale_pane_t2(1).LayoutLine();
        iFirstLine.LayoutText( list_single_fp_cale_pane.TextRect(), textLine );
        
        textLine = 
            AknLayoutScalable_Apps::list_single_fp_cale_pane_t2(2).LayoutLine();
        iMiddleLine.LayoutText( list_single_fp_cale_pane.TextRect(), textLine );
        
        textLine = 
            AknLayoutScalable_Apps::list_single_fp_cale_pane_t2(3).LayoutLine();
        iLastLine.LayoutText( list_single_fp_cale_pane.TextRect(), textLine );
        
        textLine = 
            AknLayoutScalable_Apps::popup_fixed_preview_cale_window_t1(0).LayoutLine();
        iTitleLine.LayoutText( popup_fixed_preview_cale_window.Rect(), textLine );
        
        textLine = 
            AknLayoutScalable_Apps::popup_fixed_preview_cale_window_t2(0).LayoutLine();
        iEmptyLine.LayoutText( popup_fixed_preview_cale_window.Rect(), textLine );
        
        textLine = 
            AknLayoutScalable_Apps::list_single_fp_cale_pane_t1(0).LayoutLine();
        iPluginLine.LayoutText( list_single_fp_cale_pane.TextRect(), textLine );
        }
    else
        {
        TAknLayoutRect popup_cale_events_window;
        TAknTextLineLayout textLine;
        
        popup_cale_events_window.LayoutRect( 
            StatusPane(),
            AknLayoutScalable_Apps::popup_cale_events_window(0).LayoutLine() );

        textLine = 
            AknLayoutScalable_Apps::popup_cale_events_window_t1(0).LayoutLine();
        iFirstLine.LayoutText( popup_cale_events_window.Rect(), textLine );
        
        iMiddleLine = iFirstLine;
        iLastLine   = iFirstLine;
        iEmptyLine  = iFirstLine;
        iPluginLine = iFirstLine;
        iTitleLine  = iFirstLine;
        }
    TRACE_EXIT_POINT;
    }

void CCalenPreviewLayoutManager::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

TSize CCalenPreviewLayoutManager::MinimumSize()
    {
    TRACE_ENTRY_POINT;
    TSize asize;
    
   if(iLinesUsed > KPopupMaxEntries)
   	{
    iLinesUsed = iContainer->CountComponentControls();
   	}   
       
    if( UsePreview() )
        {
        TAknLayoutRect popup_fixed_preview_cale_window;
        
        popup_fixed_preview_cale_window.LayoutRect( 
            ApplicationWindow(), 
            AknLayoutScalable_Apps::popup_fixed_preview_cale_window(Orientation()).LayoutLine() );

        asize = popup_fixed_preview_cale_window.Rect().Size();
        }
    else if( iLinesUsed )
        {
        TAknLayoutRect popup_cale_events_window;
        
        popup_cale_events_window.LayoutRect( 
            StatusPane(), 
            AknLayoutScalable_Apps::popup_cale_events_window(iLinesUsed-1).LayoutLine() );
        
        asize = popup_cale_events_window.Rect().Size();
        }
    else
        {
        asize.iWidth = 0;
        asize.iHeight = 0;
        }

    if( iLinesUsed )
        {
        TAknWindowComponentLayout temp = 
            AknLayoutScalable_Avkon::bg_popup_preview_window_pane_g1();
        
        asize.iWidth -= temp.LayoutLine().il + temp.LayoutLine().ir;
        asize.iHeight -= temp.LayoutLine().it + temp.LayoutLine().ib;
        }

    TRACE_EXIT_POINT;
    return asize;
    }

// Public member functions inherited from MCoeLayoutManager

void CCalenPreviewLayoutManager::AttachL(CCoeControl &aCompoundControl)
    {
    TRACE_ENTRY_POINT;
    iContainer = &aCompoundControl;
    UpdateLayoutData();
    TRACE_EXIT_POINT;
    }
TBool CCalenPreviewLayoutManager::CanAttach() const
    {
    TRACE_ENTRY_POINT;
    TBool ret(ETrue);
    if( iContainer )
        ret = EFalse;
    TRACE_EXIT_POINT;
    return ret;
    }
void CCalenPreviewLayoutManager::Detach(CCoeControl &aCompoundControl)
    {
    TRACE_ENTRY_POINT;
    if( iContainer == &aCompoundControl )
        {
        iContainer = NULL;
        delete this;
        }
    TRACE_EXIT_POINT;
    }

TSize CCalenPreviewLayoutManager::CalcMinimumSize(
    const CCoeControl &aCompoundControl) const
    {
    TRACE_ENTRY_POINT;
    TSize ret(0,0);
    if(iContainer != NULL && 
       iContainer == &aCompoundControl)
        {
        ret = iContainer->Rect().Size();
        }
    TRACE_EXIT_POINT;
    return ret;
    }

void CCalenPreviewLayoutManager::PerformLayout()
    {
    TRACE_ENTRY_POINT;
    if(UsePreview())
        PerformPreviewLayout();
    else
        PerformPopupLayout();
    TRACE_EXIT_POINT;
    }

TInt CCalenPreviewLayoutManager::CalcTextBaselineOffset(
    const CCoeControl& /*aCompoundControl*/, 
    const TSize& /*aSize*/) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return 0;
    }

void CCalenPreviewLayoutManager::SetTextBaselineSpacing(
    TInt /*aBaselineSpacing*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

TInt CCalenPreviewLayoutManager::TextBaselineSpacing() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return 0;
    }

void CCalenPreviewLayoutManager::HandleAddedControlL(
    const CCoeControl& /*aCompoundControl*/, 
    const CCoeControl& /*aAddedControl*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

void CCalenPreviewLayoutManager::HandleRemovedControl(
    const CCoeControl& /*aCompoundControl*/, 
    const CCoeControl& /*aRemovedControl*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

TInt CCalenPreviewLayoutManager::HandleControlReplaced(
    const CCoeControl& /*aOldControl*/, 
    const CCoeControl& /*aNewControl*/)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return 0;
    }

// New public member functions

void CCalenPreviewLayoutManager::SetLinesUsed(TInt aUsedLines)
    {
    TRACE_ENTRY_POINT;
    if(!UsePreview() && aUsedLines <= KPopupMaxEntries)
        {
        if( aUsedLines > iContainer->CountComponentControls())
            iLinesUsed = iContainer->CountComponentControls();
        else
            iLinesUsed = aUsedLines;
        }
    TRACE_EXIT_POINT;
    }

TInt CCalenPreviewLayoutManager::FirstLineWidth() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iFirstLine.TextRect().Width();
    }

TInt CCalenPreviewLayoutManager::TitleLineWidth() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iTitleLine.TextRect().Width();
    }

TInt CCalenPreviewLayoutManager::MiddleLineWidth() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iMiddleLine.TextRect().Width();
    }

TInt CCalenPreviewLayoutManager::LastLineWidth() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iLastLine.TextRect().Width();
    }

TInt CCalenPreviewLayoutManager::PluginLineWidth() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iPluginLine.TextRect().Width();
    }

TInt CCalenPreviewLayoutManager::EmptyLineWidth() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iEmptyLine.TextRect().Width();
    }

const CFont* CCalenPreviewLayoutManager::LineFont() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iFirstLine.Font();
    }

const CFont* CCalenPreviewLayoutManager::TitleFont() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iFirstLine.Font();
    }

const CFont* CCalenPreviewLayoutManager::PluginFont() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iPluginLine.Font();
    }

void CCalenPreviewLayoutManager::SetContainerLayout()
    {
    TRACE_ENTRY_POINT;
    if(iContainer)
        {
        UpdateLayoutData();
        if(UsePreview())
            {
            AknLayoutUtils::LayoutControl(
                iContainer, 
                ApplicationWindow(),
                AknLayoutScalable_Apps::popup_fixed_preview_cale_window(Orientation()).LayoutLine() );
            }
        else
            {
            AknLayoutUtils::LayoutControl(
                iContainer, 
                StatusPane(),
                AknLayoutScalable_Apps::popup_cale_events_window(iLinesUsed-1).LayoutLine());
            }
        }
    TRACE_EXIT_POINT;
    }

TRect CCalenPreviewLayoutManager::StatusPane()
    {
    TRACE_ENTRY_POINT;
    TRect statusPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane, 
                                       statusPaneRect );
    TSize screenSize;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, screenSize );
    TRect screenRect( screenSize );
    TAknLayoutRect statusPane;
    statusPane.LayoutRect( screenRect, AknLayoutScalable_Avkon::status_pane( 0 ).LayoutLine() );
    statusPaneRect.SetWidth(statusPane.Rect().Width());

    TRACE_EXIT_POINT;
    return statusPaneRect;
    }

TRect CCalenPreviewLayoutManager::ApplicationWindow()
    {
    TRACE_ENTRY_POINT;
    TRect screenRect;
    TAknLayoutRect application_window;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, 
                                       screenRect );
    application_window.LayoutRect( 
        screenRect, 
        AknLayoutScalable_Apps::application_window().LayoutLine() );
    TRACE_EXIT_POINT;
    return application_window.Rect();
    }  

void CCalenPreviewLayoutManager::PerformPopupLayout()
    {
    TRACE_ENTRY_POINT;
    // Perform layout for popup pane
    TAknLayoutRect popup_cale_events_window_g2;
    TInt limit( iContainer->CountComponentControls() );
    TRect rect( iContainer->Rect() );
  
    //>> Temporary fix 
    TAknWindowComponentLayout temp = 
        AknLayoutScalable_Avkon::bg_popup_preview_window_pane_g1();
    rect.iTl.iX -= temp.LayoutLine().il; rect.iTl.iY -= temp.LayoutLine().it;
    rect.iBr.iX += temp.LayoutLine().ir; rect.iBr.iY += temp.LayoutLine().ib;
    //<<
    
    for(TInt i = 0; i < limit; i++)
        {
        
        
        CCalenPreviewLabel* line = 
            static_cast<CCalenPreviewLabel*>(iContainer->ComponentControl(i));
        
        switch( i%KPopupMaxEntries )
            {
            case 0:
                AknLayoutUtils::LayoutLabel( 
                    line, rect, 
                    AknLayoutScalable_Apps::popup_cale_events_window_t1(KPopupMaxEntries).LayoutLine());
                break;
            case 1:
                    AknLayoutUtils::LayoutLabel(
                        line, rect, 
                        AknLayoutScalable_Apps::popup_cale_events_window_t2(KPopupMaxEntries).LayoutLine());
                    break;
            case 2:
                AknLayoutUtils::LayoutLabel(
                    line, rect, 
                    AknLayoutScalable_Apps::popup_cale_events_window_t3(KPopupMaxEntries).LayoutLine());
                break;
            case 3:
                AknLayoutUtils::LayoutLabel( 
                    line, rect, 
                    AknLayoutScalable_Apps::popup_cale_events_window_t4(KPopupMaxEntries).LayoutLine());
                // icon
                popup_cale_events_window_g2.LayoutRect( 
                    rect,
                    AknLayoutScalable_Apps::popup_cale_events_window_g2(KPopupMaxEntries).LayoutLine() );
                line->SetMoreIconLayout(popup_cale_events_window_g2);
                break;
            }
        
        // Text color skinning
        TRgb textColor(KRgbBlack);
        line->GetColor(EColorLabelText, textColor);
        AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), textColor, 
                                  KAknsIIDQsnTextColors, 
                                  EAknsCIQsnTextColorsCG20);

        PIM_TRAPD_HANDLE( 
            AknLayoutUtils::OverrideControlColorL(*line, EColorLabelText, 
                                                  textColor));
        }
    TRACE_EXIT_POINT;
    }


void CCalenPreviewLayoutManager::PerformPreviewLayout()
    {
    TRACE_ENTRY_POINT;
    
    // Perform layout for preview pane
    TRect rect( iContainer->Rect() );
    TAknLayoutScalableParameterLimits list_single_fp_cale_pane_limits =
        AknLayoutScalable_Apps::list_single_fp_cale_pane_ParamLimits();
    TInt limit( list_single_fp_cale_pane_limits.LastRow());
    TInt componentCount( iContainer->CountComponentControls());

    //>> Temporary fix
    TAknWindowComponentLayout temp = 
        AknLayoutScalable_Avkon::bg_popup_preview_window_pane_g1();
    rect.iTl.iX -= temp.LayoutLine().il; rect.iTl.iY -= temp.LayoutLine().it;
    rect.iBr.iX += temp.LayoutLine().ir; rect.iBr.iY += temp.LayoutLine().ib;
    //<<

    for(TInt i = 0; i <= limit + 1 && i < componentCount; i++)
        {
        CCalenPreviewLabel* line = 
            static_cast<CCalenPreviewLabel*>(iContainer->ComponentControl(i));

        if(line->LabelType() == CCalenPreviewLabel::ECalenPreviewTitleRow)
            {
            AknLayoutUtils::LayoutLabel( 
                line, rect, 
                AknLayoutScalable_Apps::popup_fixed_preview_cale_window_t1(0).LayoutLine());
            }
        else
            {
            TAknLayoutRect list_fp_cale_pane;
            list_fp_cale_pane.LayoutRect( 
                rect, 
                AknLayoutScalable_Apps::list_fp_cale_pane(0).LayoutLine() );
                
            TInt iDx = 0;
          	iDx = iPreview->PluginControl() ? i : i-1;
          
            TAknLayoutRect list_single_fp_cale_pane;
            list_single_fp_cale_pane.LayoutRect( 
                list_fp_cale_pane.Rect(), 
                AknLayoutScalable_Apps::list_single_fp_cale_pane(iDx).LayoutLine() );


            TAknLayoutRect list_single_fp_cale_pane_g1;
            TAknLayoutRect list_single_fp_cale_pane_g2;

            switch (line->LabelType())
                {
               case CCalenPreviewLabel::ECalenPreviewFirstRow:
                    // icon
                    list_single_fp_cale_pane_g1.LayoutRect( 
                        list_single_fp_cale_pane.Rect(), 
                        AknLayoutScalable_Apps::list_single_fp_cale_pane_g1(0).LayoutLine() );
                    line->SetIconLayout(list_single_fp_cale_pane_g1);
                    // text
                    AknLayoutUtils::LayoutLabel( 
                        line, list_single_fp_cale_pane.Rect(), 
                        AknLayoutScalable_Apps::list_single_fp_cale_pane_t2(1).LayoutLine());
                    break;
                case CCalenPreviewLabel::ECalenPreviewMiddleRow:
                    AknLayoutUtils::LayoutLabel( 
                        line, list_single_fp_cale_pane.Rect(), 
                        AknLayoutScalable_Apps::list_single_fp_cale_pane_t2(2).LayoutLine());
                    break;
                case CCalenPreviewLabel::ECalenPreviewLastRow:
                    // icon
                    list_single_fp_cale_pane_g1.LayoutRect( 
                        list_single_fp_cale_pane.Rect(), 
                        AknLayoutScalable_Apps::list_single_fp_cale_pane_g1(0).LayoutLine() );
                    line->SetIconLayout(list_single_fp_cale_pane_g1);
                    // text
                    AknLayoutUtils::LayoutLabel( 
                        line, list_single_fp_cale_pane.Rect(), 
                        AknLayoutScalable_Apps::list_single_fp_cale_pane_t2(3).LayoutLine());
                    // icon
                     list_single_fp_cale_pane_g2.LayoutRect( 
                        list_single_fp_cale_pane.Rect(), 
                        AknLayoutScalable_Apps::list_single_fp_cale_pane_g2(0).LayoutLine() );
                    line->SetMoreIconLayout(list_single_fp_cale_pane_g2);
                    break;
                case CCalenPreviewLabel::ECalenPreviewEmptyRow:
                default:
                    AknLayoutUtils::LayoutLabel( 
                        line, rect, 
                        AknLayoutScalable_Apps::popup_fixed_preview_cale_window_t2(0).LayoutLine());
                    break;
                }
            }
        
        // Text color skinning
        TRgb textColor(KRgbBlack);
        line->GetColor(EColorLabelText, textColor);
        AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), textColor, 
                                  KAknsIIDQsnTextColors, 
                                  EAknsCIQsnTextColorsCG20);
        PIM_TRAPD_HANDLE(
            AknLayoutUtils::OverrideControlColorL(*line, EColorLabelText, 
                                                  textColor));
        }

    	if(iPreview->PluginControl())
		  	{
	    	TRAPD(error,LayoutPluginControlL(*iPreview->PluginControl()) );	
	    	if(error!=KErrNone)
				{
				// Do nothing to avoid warning	
				}
			TRAP(error,iPreview->AddPluginControlL());	
	    	if(error!=KErrNone)
				{
				// Do nothing to avoid warning	
				}
		  	}

    TRACE_EXIT_POINT;
    }
    
void CCalenPreviewLayoutManager::LayoutPluginControlL(CCoeControl& aControl)    
	{
	TRACE_ENTRY_POINT;
	
    CCoeControl* line = &aControl;
	TRect rectMain( iContainer->Rect() );
	
	 //>> Temporary fix
    TAknWindowComponentLayout temp = 
        AknLayoutScalable_Avkon::bg_popup_preview_window_pane_g1();
    rectMain.iTl.iX -= temp.LayoutLine().il; rectMain.iTl.iY -= temp.LayoutLine().it;
    rectMain.iBr.iX += temp.LayoutLine().ir; rectMain.iBr.iY += temp.LayoutLine().ib;
    //<< 
	
	if(line)
		{
		TAknLayoutRect list_fp_cale_pane;
		list_fp_cale_pane.LayoutRect( 
		rectMain,
		AknLayoutScalable_Apps::list_fp_cale_pane(0).LayoutLine() );

		TAknLayoutRect list_single_fp_cale_pane;
		list_single_fp_cale_pane.LayoutRect( 
		list_fp_cale_pane.Rect(), 
		AknLayoutScalable_Apps::list_single_fp_cale_pane(0).LayoutLine() );
		
		if(!Layout_Meta_Data::IsLandscapeOrientation())
			{
			line->SetRect(list_single_fp_cale_pane.Rect());
			AknLayoutUtils::LayoutLabel( 
                        static_cast<CEikLabel*>(line), list_single_fp_cale_pane.Rect(), 
                        AknLayoutScalable_Apps::list_single_fp_cale_pane_t1(0).LayoutLine());
           			line ->SetRect(list_single_fp_cale_pane.Rect());
			      static_cast<CEikLabel*>(line)->SetLabelAlignment(ELayoutAlignCenter);
			}
		else
			{
			TRect rect;
			rect = list_single_fp_cale_pane.Rect();
			rect.iBr.iY = rect.iBr.iY*2;
			line->SetRect(rect);
		    rectMain = rect;
			}
	    TRgb textColor(KRgbBlack);
        line->GetColor(EColorLabelText, textColor);
        AknsUtils::GetCachedColor(AknsUtils::SkinInstance(), textColor, 
                                  KAknsIIDQsnTextColors, 
                                  EAknsCIQsnTextColorsCG20);
        PIM_TRAPD_HANDLE(
        AknLayoutUtils::OverrideControlColorL(*line, EColorLabelText, 
                                                  textColor));
                                                  
        
        
		}
		
	TRACE_EXIT_POINT;	
	}

TBool CCalenPreviewLayoutManager::UsePreview()
    {
    TRACE_ENTRY_POINT;
    TBool previewMode(EFalse);
    TAknLayoutRect popup_fixed_preview_cale_window;

    popup_fixed_preview_cale_window.LayoutRect( 
        ApplicationWindow(), 
        AknLayoutScalable_Apps::popup_fixed_preview_cale_window(Orientation()).LayoutLine() );

    if( popup_fixed_preview_cale_window.Rect().Height() &&
        popup_fixed_preview_cale_window.Rect().Width() )
        previewMode = ETrue;
    TRACE_EXIT_POINT; 
    return previewMode;
    }

TInt CCalenPreviewLayoutManager::LineCount()
    {
    TRACE_ENTRY_POINT;
    TInt lines(KPopupMaxEntries);
    if( UsePreview() )
        {
        TAknLayoutScalableParameterLimits limits =
            AknLayoutScalable_Apps::list_single_fp_cale_pane_ParamLimits();
        
        lines = limits.LastRow() + 1;
        }
    TRACE_EXIT_POINT; 
    return lines;
    }
TInt CCalenPreviewLayoutManager::Orientation()
    {
    TRACE_ENTRY_POINT;
    TInt variant(0);
    if(Layout_Meta_Data::IsLandscapeOrientation())
        variant = 1;
    TRACE_EXIT_POINT;
    return variant;
    }

TRect CCalenPreviewLayoutManager::ReducePreview(TRect aRect)
    {
    TRACE_ENTRY_POINT;
    if(!UsePreview())
        {
        TAknLayoutRect main_pane;
        
        main_pane.LayoutRect( 
            ApplicationWindow(), 
            AknLayoutScalable_Apps::main_pane(Orientation() + 10).LayoutLine() );

        aRect.SetWidth( main_pane.Rect().Width() );
        aRect.SetHeight( main_pane.Rect().Height() );
        }
    TRACE_EXIT_POINT; 
    return aRect;
    }
    
    
 

//End of file
