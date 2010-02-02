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
* Description:  Implementation of Notepad/Template Editor.
*
*/


#include "NpdEdwinBase.h"

#include <AknUtils.h>
#include <txtglobl.h>
#include <barsread.h>

#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknsFrameBackgroundControlContext.h>
#include <applayout.cdl.h>
#include <eikdialg.h>
#include <skinlayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknappui.h>
#include <NpdLib.rsg>

#include "NpdEdwin.h"
#include "NpdRichTextEditor.h"

#include "NpdLibPanic.h"
#include "NpdEdwinLines.h"


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CNotepadEdwinBase::CNotepadEdwinBase()
// Constructor
// ---------------------------------------------------------
//
CNotepadEdwinBase::CNotepadEdwinBase()
	{
	iEdwinLines = NULL;
	iBgContext = NULL;
	iSkinContext = NULL;
    iIsFirstTimeDrawEditor = ETrue;
	}

// ---------------------------------------------------------
// CNotepadEdwinBase::~CNotepadEdwinBase()
// Destructor
// ---------------------------------------------------------
//
CNotepadEdwinBase::~CNotepadEdwinBase()
	{
	if(iBgContext)
		{
		delete iBgContext;
		iBgContext = NULL;	
		}
	if(iSkinContext)
		{
		delete iSkinContext;	
		}
	}

// ---------------------------------------------------------
// CNotepadEdwinBase::Share(CEikEdwin* aEdwin, MNotepadEdwinExposer* aEdwinExposer)
// Shares the edwin and its exposer
// ---------------------------------------------------------
//
void CNotepadEdwinBase::Share(CEikEdwin* aEdwin, MNotepadEdwinExposer* aEdwinExposer)
	{
	iEdwin = aEdwin;
	iEdwinExposer = aEdwinExposer;
	}
// -----------------------------------------------------------------------------
// CNotepadEdwinBase::DoEditorLayoutL
// Sets the layout of the Edwin and the scrollbar.
// -----------------------------------------------------------------------------
//
void CNotepadEdwinBase::DoEditorLayoutL()
	{
    	if (!iBgContext)
        	return;
    	UpdateScrollLayout();
    	iBackgroundRect = iParent->Rect();
    	if(iEdwinLines != NULL)
    		{
    		AknLayoutUtils::LayoutControl( iEdwinLines, 
        			iBackgroundRect, 0, 0, 0, 
        			AknLayoutUtilsNoValue, 
        			AknLayoutUtilsNoValue, 
        			iBackgroundRect.Width(), 
        			iBackgroundRect.Height() );
	 	}
	TAknLayoutRect mainNotesPane;
	TAknLayoutRect lineLayout;
	TRect tempMainPane;
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,tempMainPane);
	TRect mainPane(TPoint(0, 0), tempMainPane.Size());
	mainNotesPane.LayoutRect(mainPane, AknLayoutScalable_Apps::main_notes_pane().LayoutLine());
	lineLayout.LayoutRect(mainNotesPane.Rect(),AknLayoutScalable_Apps::list_notes_pane().LayoutLine());
	TAknLayoutScalableParameterLimits textLimits =AknLayoutScalable_Apps::list_notes_text_pane_ParamLimits();
	RArray<TAknTextComponentLayout> layouts;
	for (TInt i = 0; i < textLimits.LastRow()+1 ; i++)
    		{
    		TAknWindowComponentLayout paneLayout = AknLayoutScalable_Apps::list_notes_text_pane(i);
    		TAknTextComponentLayout textLayout = AknLayoutScalable_Apps::list_notes_text_pane_t1();
    		TAknTextComponentLayout composedTextLayout = TAknWindowComponentLayout::ComposeText(paneLayout, textLayout);
  		layouts.AppendL(composedTextLayout); 
    		}    
	TAknMultiLineTextLayout edwinLayout =
	TAknTextComponentLayout::Multiline( layouts );              
	AknLayoutUtils::LayoutEdwin(iEdwin, lineLayout.Rect(), edwinLayout, EAknsCIQsnTextColorsCG6);
	layouts.Reset();

    	// layout edwin overrides this value
    	iEdwin->SetTextSkinColorIdL(EAknsCIQsnTextColorsCG34);

    	// SetBackgroundColorL() contains modification to 
    	// Edwin's CParaFormat which was created by 
    	// AknLayoutUtils::LayoutEdwin(...), so iEditor->SetBackgroundColorL()
    	// must be called after AknLayoutUtils::LayoutEdwin(...).
    	//
    	SetBackgroundColorL();
	}

// -----------------------------------------------------------------------------
// CNotepadEdwinBase::InitNotepadEditorL
// -----------------------------------------------------------------------------
//
void CNotepadEdwinBase::InitNotepadEditorL(
    CEikDialog* aParent,
    TBool aIsRichTextEditor, 
    CNotepadEdwinLines* aEdwinLines)
    {
    iParent = aParent;
    iBackgroundRect = iParent->Rect();
    iEdwinLines = aEdwinLines;
    iIsRichTextEditor = aIsRichTextEditor;
    if (!iBgContext)
    	{
    	iBgContext = CAknsFrameBackgroundControlContext::NewL( KAknsIIDQsnFrNotepad, iBackgroundRect, iBackgroundRect, EFalse );
    	}
    if(!iSkinContext)
    	{
    	iSkinContext = CAknsBasicBackgroundControlContext::NewL(KAknsIIDQsnBgAreaMain,iBackgroundRect,EFalse);
    	}
    iBgContext->SetParentContext( iSkinContext );
    DoEditorLayoutL();    

    // EdwinEvent is used for dynamically changing Laf bitmap
    //
    iEdwin->AddEdwinObserverL(this);

    iEdwin->SetAknEditorFlags(EAknEditorFlagUseSCTNumericCharmap
                                | EAknEditorFlagEnablePictographInput
                                | EAknEditorFlagAllowEntersWithScrollDown );

    iEdwin->Text()->SetHasChanged(EFalse);
    SetAmountToFormat(iEdwin->Text()->DocumentLength()); // ensure switch to band formatting 
    iEdwinExposer->CallSetCanDrawOutsideRect(); // Redraw after Chinese FEP
    }

// ---------------------------------------------------------
// CNotepadEdwinBase::SupplyMopObject(TTypeUid aId,TTypeUid::Ptr& ptr)
// Gets an object whose type is encapsulated by 
// the specified TTypeUid object.
// ---------------------------------------------------------
//
TBool CNotepadEdwinBase::SupplyMopObject(TTypeUid aId,TTypeUid::Ptr& ptr)
	{
	if(aId.iUid == MAknsControlContext::ETypeId)
		{
		ptr =  MAknsControlContext::SupplyMopObject(aId, iSkinContext);
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNotepadEdwinBase::HandleEdwinEventL
// from MEikEdwinObserver
// -----------------------------------------------------------------------------
//
void CNotepadEdwinBase::HandleEdwinEventL(
    CEikEdwin* /*aEdwin*/,
    TEdwinEvent aEventType )
    {

    iTextLength = iEdwin->TextLength();

    if ( aEventType == EEventFormatChanged )
        {
        iEdwin->DrawNow();
        }
    if ( aEventType == EEventTextUpdate )
        { 
        iEdwin->TextView()->DocPosToXyPosL( iEdwin->CursorPos(), iPointAtCursor );
        iLineNumByYCoordinate = iPointAtCursor.iY;

        if ( !iIsFirstTimeDrawEditor )
            {

            if ( iTextLength < iPreTextLength &&  iLineNumByYCoordinate < iPreLineNumByYCoordinate )
            	{
                iEdwin->NotifyNewFormatL();
                iEdwin->DrawDeferred();
            	}
            }
        else
            {
            iIsFirstTimeDrawEditor = EFalse;
            }
        

        iPreLineNumByYCoordinate = iLineNumByYCoordinate;
        }

     iPreTextLength = iTextLength;
    }

// -----------------------------------------------------------------------------
// CNotepadEdwinBase::SetBackgroundColorL
// -----------------------------------------------------------------------------
//
void CNotepadEdwinBase::SetBackgroundColorL()
	{
    	TAknLayoutRect insiderect;
	TAknLayoutRect bgLayout;
	TAknLayoutRect mainNotesPane;
    TRect tempMainPane;
    TBool hasChanged;
    
    hasChanged = iEdwin->Text()->HasChanged();
    	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,tempMainPane);
	TRect mainPane(TPoint(0, 0), tempMainPane.Size());
	mainNotesPane.LayoutRect(mainPane, AknLayoutScalable_Apps::main_notes_pane().LayoutLine());
	bgLayout.LayoutRect(mainNotesPane.Rect(), AknLayoutScalable_Apps::bg_notes_pane().LayoutLine());
	insiderect.LayoutRect(bgLayout.Rect(), AknLayoutScalable_Apps::notes_bg_pane_g1().LayoutLine());
	TRgb bgcolor(insiderect.Color());
	iEdwin->CEikEdwin::SetBackgroundColorL(bgcolor);
    	iEdwinExposer->GetTextView().SetBackgroundColor(bgcolor);
	iEdwin->CEikEdwin::SetSkinBackgroundControlContextL(iBgContext);
	CGlobalText* text = STATIC_CAST(CGlobalText*, iEdwin->Text());
    	CParaFormat* paraFormat = CParaFormat::NewLC();
    	TParaFormatMask paraFormatMask;
    	text->GetParaFormatL(paraFormat, paraFormatMask, 
       		0, text->DocumentLength());
    	paraFormat->iFillColor = bgcolor;
    	paraFormatMask.SetAttrib(EAttFillColor);
    	text->ApplyParaFormatL(paraFormat,paraFormatMask, 
        	0, text->DocumentLength());
    if ( !hasChanged )
        {
        iEdwin->Text()->SetHasChanged( EFalse );
        }

	if(iIsRichTextEditor)
    		{
    		(STATIC_CAST(CEikRichTextEditor*,iEdwin))->ApplyParaFormatL(paraFormat,paraFormatMask);
    		}
     	CleanupStack::PopAndDestroy(); // paraFormat
	TResourceReader rr;
        iEdwinExposer->GetCoeEnv().CreateResourceReaderLC(rr, R_NOTEPAD_CURSOR_COLOR);
    	TRgb cursorColor(AKN_LAF_COLOR(rr.ReadInt16()));
    	CleanupStack::PopAndDestroy(); // rr
    	iEdwinExposer->GetTextView().SetCursorXorColor( TRgb( cursorColor.Value() ^ bgcolor.Value() ) );
    	iEdwin->NotifyNewFormatL();
	}

// -----------------------------------------------------------------------------
// CNotepadEdwinBase::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CNotepadEdwinBase::HandleResourceChange(TInt aType)
    {
    iEdwin->CEikEdwin::HandleResourceChange( aType );
    switch(aType)
    	{
    	case KEikColorResourceChange:
    	case KAknsMessageSkinChange:
    		TInt ignore;	
        	TRAP(ignore, SetBackgroundColorL());
        	break;
    	}
    if( aType == KAknsMessageSkinChange )
        {
        UpdateScrollLayout();
        }
    }

// -----------------------------------------------------------------------------
// CNotepadEdwinBase::Draw
// -----------------------------------------------------------------------------
//
void CNotepadEdwinBase::Draw(const TRect& aRect) const   
	{
    CWindowGc& gc = iEdwin->SystemGc();
    TRect tempMainPane;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,tempMainPane);
	TRect mainPane(TPoint(0, 0), tempMainPane.Size());
    	if( iBgContext )
		{
    		TAknLayoutRect mainpaneSkinTl;
    		TAknLayoutRect mainpaneSkinBr;
    		mainpaneSkinTl.LayoutRect( mainPane, SkinLayout::Notepad_skin_element_placing_Line_2() );  // <CDL LAYOUT>
    		mainpaneSkinBr.LayoutRect( mainPane, SkinLayout::Notepad_skin_element_placing_Line_5() );  // <CDL LAYOUT>
    		TAknWindowLineLayout mainPaneSkinCenter = SkinLayout::Notepad_skin_element_placing_Line_1();  // <CDL LAYOUT>
		TAknLayoutRect mainPaneLayout;
		mainPaneLayout.LayoutRect(mainPane,mainPaneSkinCenter);
    		TRect skinCenter( mainPaneLayout.Rect() );
    		TRect viewRect;
    		viewRect.iTl= mainpaneSkinTl.Rect().iTl;
    		viewRect.iBr = mainpaneSkinBr.Rect().iBr;
    	
    		if(iSkinContext)
    			{
    			iSkinContext->SetRect(iParent->Rect());
    			TRAP_IGNORE( iEdwin->CEikEdwin::SetSkinBackgroundControlContextL(iSkinContext) );
        		AknsDrawUtils::Background( AknsUtils::SkinInstance(), iSkinContext, iEdwin, gc, mainPane);	
    			}
        	TRAP_IGNORE( iEdwin->CEikEdwin::SetSkinBackgroundControlContextL(iBgContext) );
    		iBgContext->SetFrameRects( viewRect, mainPaneLayout.Rect() );
    		AknsDrawUtils::Background( AknsUtils::SkinInstance(), iBgContext, iEdwin, gc, mainPane,KAknsDrawParamNoClearUnderImage);
 		}
   	iEdwin->CEikEdwin::Draw(aRect);
	}

// ---------------------------------------------------------
// CNotepadEdwinBase::UpdateScrollLayout()
// Updates the scroll layout
// ---------------------------------------------------------
//
void CNotepadEdwinBase::UpdateScrollLayout()
	{
	CEikScrollBarFrame* sbFrame = iEdwin->ScrollBarFrame();
    	TInt type = sbFrame->TypeOfVScrollBar();
	if( type == CEikScrollBarFrame::EDoubleSpan )
        	{
		TAknLayoutRect mainNotesPane;
		TRect tempMainPane;
        	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,tempMainPane);
		TRect mainPane(TPoint(0, 0), tempMainPane.Size());
		TAknWindowLineLayout scrollPane = AknLayoutScalable_Apps::scroll_pane_cp06().LayoutLine();
		mainNotesPane.LayoutRect(mainPane, AknLayoutScalable_Apps::main_notes_pane().LayoutLine());
        	AknLayoutUtils::LayoutVerticalScrollBar(sbFrame, mainNotesPane.Rect(), scrollPane);
        	}
	}
// End of File  
