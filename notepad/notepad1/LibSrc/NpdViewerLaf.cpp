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
* Description:  Implementation of Decoration graphics of Editor.
*
*/


// INCLUDE FILES
#include <AknUtils.h>
#include <AknIconArray.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <applayout.cdl.h>
#include <skinlayout.cdl.h>
#include <data_caging_path_literals.hrh>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknappui.h>
#include <NpdLib.rsg>
#include <npdlib.mbg>
#include <eikdialg.h>
#include <aknlayout.cdl.h>
#include "NpdLib.hrh"
#include "NpdViewerLaf.h"
#include "NpdEditorDialog.h"
#include "NpdEdwin.h"


_LIT(KNpdMbmDrive, "Z:");
_LIT(KNpdLibMbmFile, "npdlib.mbm");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadViewerLaf::CNotepadViewerLaf
// public Constructor
// -----------------------------------------------------------------------------
//
CNotepadViewerLaf::CNotepadViewerLaf(CEikDialog* aParent)
    : CCoeControl(), iParent(aParent)
    {
    // Note: These two lines are for enabling Window Server Autoflush. This
    // 		 allows you to execute drawing code step-by-step and see the 
    //		 the changes as they happen. 
    //
    //		 To enable autoflush, uncomment the code lines below, and press
    //		 CTRL+SHIFT+ALT+F in the emulator. 
  	//RWsSession &ws = iEikonEnv->WsSession();
  	//ws.SetAutoFlush( ETrue );

	iSkinUsed = ETrue;
	iBgContext = NULL;
    iStartPage = ETrue;
    }

// -----------------------------------------------------------------------------
// CNotepadViewerLaf::~CNotepadViewerLaf
// Destructor
// -----------------------------------------------------------------------------
//
CNotepadViewerLaf::~CNotepadViewerLaf()
    {
    delete iTopPageIcon;
    delete iTopPageMask;
    delete iMidPageIcon;
    delete iMidPageMask;
	delete iBgContext;
	if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
		{
		delete iTopLeftCornerIcon;
		delete iTopRightCornerIcon;
		delete iTopLeftCornerMask;
		delete iTopRightCornerMask;
		}
    }

// -----------------------------------------------------------------------------
// CNotepadViewerLaf::SetImage
// public new function
// -----------------------------------------------------------------------------
//
void CNotepadViewerLaf::SetImage( TBool aOnTopOfPage )
    {
    if ( aOnTopOfPage != iStartPage )
        {
        iStartPage = aOnTopOfPage;
        DrawNow();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadViewerLaf::AddRectsAndLayout
// -----------------------------------------------------------------------------
//
void CNotepadViewerLaf::AddRectsAndLayout(const TRect& aRect, RRegion& aRegion)
    {
    if(iSkinUsed)
    	{
	  	TAknLayoutRect upperEdgeSkinTl;
  		TAknLayoutRect upperEdgeSkinTr;
 
 		upperEdgeSkinTl.LayoutRect( aRect, SkinLayout::Notepad_skin_element_placing_Line_2() );  // <CDL LAYOUT>
  		upperEdgeSkinTr.LayoutRect( aRect, SkinLayout::Notepad_skin_element_placing_Line_3() );  // <CDL LAYOUT>
   
  		TRect pageTopRect( aRect.iTl/*upperEdgeSkinTl.Rect().iTl*/, upperEdgeSkinTr.Rect().iBr );
    	aRegion.AddRect(pageTopRect);
    	}
    else
    	{
//   	    aRegion.AddRect(aRect, AppLayout::Note_writing_layout_elements_Line_4());		
    	}
    }

// -----------------------------------------------------------------------------
// CNotepadViewerLaf::ConstructFromResourceL
// public from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadViewerLaf::ConstructFromResourceL(TResourceReader& /*aReader*/)
    {
	TInt ignor;
     TRAP(ignor,
        iBgContext = CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrNotepad, TRect(0,0,0,0), TRect(0,0,0,0), EFalse );
        );

	if (iBgContext)
		{
	    TFileName mbmfile(KNpdMbmDrive);
	    
    	//mbmfile.Append(KDC_RESOURCE_FILES_DIR);
    	
    	mbmfile.Append(KDC_APP_BITMAP_DIR);
    	mbmfile.Append(KNpdLibMbmFile);
    	
		MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		AknsUtils::CreateIconL(skin,KAknsIIDQsnFrNotepadSideT,iTopPageIcon,iTopPageMask,mbmfile,EMbmNpdlibQsn_fr_notepad_side_t,EMbmNpdlibQsn_fr_notepad_side_t_mask  );
		AknsUtils::CreateIconL(skin,KAknsIIDQsnFrNotepadContSideT,iMidPageIcon,iMidPageMask,mbmfile,EMbmNpdlibQsn_fr_notepad_cont_side_t ,EMbmNpdlibQsn_fr_notepad_cont_side_t_mask ); 
		if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
			{
			AknsUtils::CreateIconL(skin,KAknsIIDQsnFrNotepadCornerTl,iTopLeftCornerIcon,iTopLeftCornerMask,mbmfile,EMbmNpdlibQsn_fr_notepad_corner_tl ,EMbmNpdlibQsn_fr_notepad_corner_tl_mask ); 
			AknsUtils::CreateIconL(skin,KAknsIIDQsnFrNotepadCornerTr,iTopRightCornerIcon,iTopRightCornerMask,mbmfile,EMbmNpdlibQsn_fr_notepad_corner_tr ,EMbmNpdlibQsn_fr_notepad_corner_tr_mask ); 
			}
		}
    }

// -----------------------------------------------------------------------------
// CNotepadViewerLaf::SetContainerWindowL
// cf. CEikEdwin::SetContainerWindowL.
// -----------------------------------------------------------------------------
//
void CNotepadViewerLaf::SetContainerWindowL(const CCoeControl& aContainer)
    {
    if ( !OwnsWindow() )
        {
        CreateWindowL(&aContainer); // include SetMopParent
        }
    else
        {
        CCoeControl::SetContainerWindowL(aContainer);
        }
    }

// -----------------------------------------------------------------------------
// CNotepadViewerLaf::SizeChanged
// protected from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadViewerLaf::SizeChanged()
    {
    TRect rect( iParent->Rect() );

    RRegion region;
    AddRectsAndLayout(rect, region);
    Window().SetShape(region);
    region.Close();	
	if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
		{
		TAknLayoutRect bgLayout;
		TAknLayoutRect mainNotesPane;
		TRect tempMainPane;
		TRect statusPane;
    	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,tempMainPane);
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,statusPane);
		TRect mainPane(TPoint(0, 0), tempMainPane.Size());
		mainNotesPane.LayoutRect(mainPane, AknLayoutScalable_Apps::main_notes_pane().LayoutLine());
		bgLayout.LayoutRect(mainNotesPane.Rect(), AknLayoutScalable_Apps::bg_notes_pane().LayoutLine());
		iTlCornerLayout.LayoutRect(bgLayout.Rect(), AknLayoutScalable_Apps::bg_notes_pane_g2().LayoutLine());
		iTrCornerLayout.LayoutRect(bgLayout.Rect(), AknLayoutScalable_Apps::bg_notes_pane_g3().LayoutLine());
		iBitmapLayout.LayoutRect(bgLayout.Rect(), AknLayoutScalable_Apps::notes_bg_pane_g6().LayoutLine());
		}
	else
		{
		iBitmapLayout.LayoutRect(rect, AppLayout::Note_writing_layout_elements_Line_4());
		}
    AknIconUtils::SetSize(iTopPageIcon, iBitmapLayout.Rect().Size());
    AknIconUtils::SetSize(iMidPageIcon, iBitmapLayout.Rect().Size());
    AknIconUtils::SetSize(iTopPageMask, iBitmapLayout.Rect().Size());
    AknIconUtils::SetSize(iMidPageMask, iBitmapLayout.Rect().Size());
	if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
		{
		AknIconUtils::SetSize(iTopLeftCornerIcon, iTlCornerLayout.Rect().Size());
		AknIconUtils::SetSize(iTopRightCornerIcon, iTrCornerLayout.Rect().Size());
		AknIconUtils::SetSize(iTopLeftCornerMask, iTlCornerLayout.Rect().Size());
		AknIconUtils::SetSize(iTopRightCornerMask, iTrCornerLayout.Rect().Size());
		}

   	TAknLayoutRect mainpaneSkinCenter;
   	mainpaneSkinCenter.LayoutRect( rect, SkinLayout::Notepad_skin_element_placing_Line_1() );  // <CDL LAYOUT>
    iBgContext->SetFrameRects(rect,mainpaneSkinCenter.Rect());// = CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrNotepad, iBackgroundRect, TRect(0,0,0,0), EFalse );
    SetPosition(rect.iTl);
    }

// -----------------------------------------------------------------------------
// CNotepadViewerLaf::Draw
// private from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadViewerLaf::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
	MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRect main_pane = iParent->Rect();
	TBool skinUsed = ETrue;
	gc.Clear(aRect);
	if(iBgContext)
		{
    	skinUsed = AknsDrawUtils::Background( skin, iBgContext, gc, iBitmapLayout.Rect());
		if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
			{
			iTlCornerLayout.DrawImage(SystemGc(), iTopLeftCornerIcon, iTopLeftCornerMask);
			iTrCornerLayout.DrawImage(SystemGc(), iTopRightCornerIcon, iTopRightCornerMask);
			}

		if (skinUsed)
			{	
 		    if(!iStartPage)
    		    {
				TAknLayoutRect line2Rect;
				TAknLayoutRect line6Rect;
				TAknLayoutRect line3Rect;
				if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
					{
					TRect tempMainPane;
					TRect statusPane;
    				AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,tempMainPane);
					AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,statusPane);
					TRect mainPane(TPoint(0, 0), tempMainPane.Size());
					TAknLayoutRect mainNotesPane;
					mainNotesPane.LayoutRect(mainPane, 
						AknLayoutScalable_Apps::main_notes_pane().LayoutLine());

					line2Rect.LayoutRect( mainNotesPane.Rect(), 
						SkinLayout::Notepad_skin_element_placing_Line_2() );
					line6Rect.LayoutRect( mainNotesPane.Rect(), 
						SkinLayout::Notepad_skin_element_placing_Line_6() );
					line3Rect.LayoutRect( mainNotesPane.Rect(), 
						SkinLayout::Notepad_skin_element_placing_Line_3() );
					}
				else
					{
					TRect application = iAvkonAppUi->ApplicationRect();
					TAknLayoutRect scrLayout;
					scrLayout.LayoutRect(application,AknLayout::screen());
					TAknLayoutRect statusPaneLayout;
					TAknLayoutRect controlPaneLayout;
					statusPaneLayout.LayoutRect( scrLayout.Rect(), AknLayout::status_pane(scrLayout.Rect(), 0) );
					controlPaneLayout.LayoutRect( scrLayout.Rect(), AknLayout::control_pane(scrLayout.Rect()) );
					TRect clientRect( TPoint(0,0), TSize(scrLayout.Rect().Width() , scrLayout.Rect().Height()-statusPaneLayout.Rect().Height()-controlPaneLayout.Rect().Height() ) );
					line2Rect.LayoutRect(clientRect,SkinLayout::Notepad_skin_element_placing_Line_2());
					line6Rect.LayoutRect(clientRect,SkinLayout::Notepad_skin_element_placing_Line_6());
					line3Rect.LayoutRect(clientRect,SkinLayout::Notepad_skin_element_placing_Line_3());
					}
				AknsDrawUtils::DrawCachedImage(skin, gc, 
   					line2Rect.Rect(), KAknsIIDQsnFrNotepadCornerTl );
   				AknsDrawUtils::DrawCachedImage(skin, gc, 
   					line6Rect.Rect(), KAknsIIDQsnFrNotepadSideT );
   				AknsDrawUtils::DrawCachedImage(skin, gc, 
   					line3Rect.Rect(), KAknsIIDQsnFrNotepadCornerTr );
        		}
			}
		}
	if(!skinUsed || !iBgContext)
		{

   		AknsDrawUtils::DrawCachedImage(AknsUtils::SkinInstance(), gc, main_pane, KAknsIIDQsnBgAreaMain );

	    if(iStartPage)
    	    {
        	iBitmapLayout.DrawImage(SystemGc(), iTopPageIcon, iTopPageMask);
        	}
    	else
        	{
        	iBitmapLayout.DrawImage(SystemGc(), iMidPageIcon, iMidPageMask);
    	   	}
		}
    }

// ---------------------------------------------------------
// CNotepadEdwin::MopSupplyObject(TTypeUid aId)
// Handles a change to the control's resources.
// ---------------------------------------------------------
//
TTypeUid::Ptr CNotepadViewerLaf::MopSupplyObject(TTypeUid aId)
	{
	if(aId.iUid == MAknsControlContext::ETypeId)
		{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext);
		}
	return CCoeControl::MopSupplyObject(aId);
	}

void CNotepadViewerLaf::HandleResourceChange(TInt aType)
    {
		if(aType == KAknsMessageSkinChange)
			{
				if(iBgContext)
				{
					delete iBgContext;
					iBgContext = NULL;
				}
				TInt ignor;
				
				TRAP(ignor,
				 iBgContext = CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrNotepad, TRect(0,0,0,0), TRect(0,0,0,0), EFalse );
				 );

				if (iBgContext)
				{
				    TFileName mbmfile(KNpdMbmDrive);
				   	mbmfile.Append(KDC_APP_BITMAP_DIR);
    				mbmfile.Append(KNpdLibMbmFile);
    	
					MAknsSkinInstance* skin = AknsUtils::SkinInstance();
					TRAP_IGNORE( AknsUtils::CreateIconL(skin,KAknsIIDQsnFrNotepadSideT,iTopPageIcon,iTopPageMask,mbmfile,EMbmNpdlibQsn_fr_notepad_side_t,EMbmNpdlibQsn_fr_notepad_side_t_mask ) );
					TRAP_IGNORE( AknsUtils::CreateIconL(skin,KAknsIIDQsnFrNotepadContSideT,iMidPageIcon,iMidPageMask,mbmfile,EMbmNpdlibQsn_fr_notepad_cont_side_t ,EMbmNpdlibQsn_fr_notepad_cont_side_t_mask ) ); 
				}
				
				SizeChanged();
				DrawDeferred();
			
			}
		
    }
// End of File  
