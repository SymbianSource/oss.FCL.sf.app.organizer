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
* Description:  Implementation of paper lines of Notepad/Template Editor.
*
*/


// INCLUDE FILES
#include <AknUtils.h>
#include <barsread.h>
#include <applayout.cdl.h>
#include <eikrted.h>    // for CRichTextEditor
#include <aknlayoutscalable_apps.cdl.h>
#include <aknappui.h>
#include <eikdialg.h>
#include <AknsFrameBackgroundControlContext.h>
#include <skinlayout.cdl.h>

#include "NpdEdwinLines.h"
#include "NpdEdwin.h"
#include "NpdLibPanic.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadEdwinLines::CNotepadEdwinLines
// public Constructor
// -----------------------------------------------------------------------------
//
CNotepadEdwinLines::CNotepadEdwinLines(CEikDialog* aParent)
    : CCoeControl(),iParent(aParent),iBgContext(NULL)
    	{
    	
    	}

// -----------------------------------------------------------------------------
// CNotepadEdwinLines::~CNotepadEdwinLines
// Destructor
// -----------------------------------------------------------------------------
//
CNotepadEdwinLines::~CNotepadEdwinLines()
	{
    	iRects.Close();
    	if(iBgContext != NULL)
    		{
    		delete iBgContext;
    		iBgContext = NULL;	
		}
	}

// -----------------------------------------------------------------------------
// CNotepadEdwinLines::AddRectsAndLayout
// from CNotepadEdwinLines (called from SizeChanged)
// -----------------------------------------------------------------------------
//
void CNotepadEdwinLines::AddRectsAndLayout(RRegion& aRegion)
	{
	TAknLayoutRect mainNotesPane;
	TAknLayoutRect lineLayout;
	TAknLayoutRect line;
	TAknLayoutScalableParameterLimits textLimits ;
	
	TRect tempMainPane;
	TRect statusPane;
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,tempMainPane);
	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,statusPane);
	TRect mainPane(TPoint(0, 0), tempMainPane.Size());
	mainNotesPane.LayoutRect(mainPane, AknLayoutScalable_Apps::main_notes_pane().LayoutLine());
	lineLayout.LayoutRect(mainNotesPane.Rect(),AknLayoutScalable_Apps::list_notes_pane().LayoutLine());
	textLimits = AknLayoutScalable_Apps::list_notes_text_pane_ParamLimits();

	TInt lineCount = textLimits.LastRow() - textLimits.FirstRow() + 1;
	
	// The array iRects should be reset, when there is a change 
	// in the no of lines after a layout switch
	
	if ( iRects.Count() != lineCount )
		{
		iRects.Reset();
   		for (TInt j(textLimits.FirstRow()); j <= textLimits.LastRow(); j++)
			{
    		iRects.Append( TAknLayoutRect() );
			}
		}
	
	for ( TInt i(textLimits.FirstRow()); i <= textLimits.LastRow(); i++ )
		{
		line.LayoutRect(lineLayout.Rect(),AknLayoutScalable_Apps::list_notes_text_pane(i).LayoutLine());
		iRects[i].LayoutRect(line.Rect() ,AknLayoutScalable_Apps::list_notes_text_pane_g1().LayoutLine());
		aRegion.AddRect(iRects[i].Rect());
		}
	}

// -----------------------------------------------------------------------------
// CNotepadEdwinLines::ConstructFromResourceL
// from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadEdwinLines::ConstructFromResourceL(TResourceReader& /*aReader*/)
	{
	TAknLayoutScalableParameterLimits textLimits ;  
    	
    	TInt ignor;
	TRAP(ignor,
    		iBgContext = CAknsFrameBackgroundControlContext::NewL( 
    			KAknsIIDQsnFrNotepad, 
    			TRect(0,0,0,0), 
    			TRect(0,0,0,0), 
    			EFalse );
        		);
        ignor = KErrNone;		
    	textLimits = AknLayoutScalable_Apps::list_notes_text_pane_ParamLimits();
	for (TInt i(textLimits.FirstRow()); i <= textLimits.LastRow(); i++)
		{
	 	User::LeaveIfError(iRects.Append(TAknLayoutRect()));
		}
	}

// -----------------------------------------------------------------------------
// CNotepadEdwinLines::Draw
// from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadEdwinLines::Draw(const TRect& /*aRect*/) const
	{
    	CWindowGc& gc = SystemGc();
    	for(TInt i(iRects.Count() - 1); i >= 0; i--)
        		{
        		iRects[i].DrawRect(gc);
        		}
    	}

// ---------------------------------------------------------
// CNotepadEdwinLines::MopSupplyObject(TTypeUid aId)
// Handles a change to the control's resources.
// ---------------------------------------------------------
//
TTypeUid::Ptr CNotepadEdwinLines::MopSupplyObject(TTypeUid aId)
	{
	if((aId.iUid == MAknsControlContext::ETypeId) && (iBgContext != NULL))
		{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext);
		}
	return CCoeControl::MopSupplyObject(aId);
	}

// ---------------------------------------------------------
// CNotepadEdwinLines::HandleResourceChange(TInt aType)
// Handles a change to the control's resources.
// ---------------------------------------------------------
//
void CNotepadEdwinLines::HandleResourceChange(TInt aType)
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
			iBgContext = CAknsFrameBackgroundControlContext::NewL( 
				KAknsIIDQsnFrNotepad, 
				TRect(0,0,0,0), 
				TRect(0,0,0,0), 
				EFalse );
			);
		ignor = KErrNone;
		SizeChanged();
		DrawDeferred();
		}
	}

void CNotepadEdwinLines::SizeChanged()
	{
    	TRect rect( iParent->Rect() );
    	RRegion region;
    	AddRectsAndLayout(region);
    	Window().SetShape(region);
    	region.Close();
	}

// -----------------------------------------------------------------------------
// CNotepadEdwinLines::SetContainerWindowL
// cf. CEikEdwin::SetContainerWindowL.
// -----------------------------------------------------------------------------
//
void CNotepadEdwinLines::SetContainerWindowL(const CCoeControl& aContainer)
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
// End of File  
