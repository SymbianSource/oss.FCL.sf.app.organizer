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
#include <NpdLib.rsg>
#include <barsread.h>
#include <applayout.cdl.h>
#include <eikrted.h>    // for CRichTextEditor
#include <aknlayoutscalable_apps.cdl.h>
#include <aknappui.h>
#include "NpdEditorLaf.h"
#include "NpdEdwin.h"
#include "NpdLibPanic.h"


const TInt KNotepadEditorLines(6);

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadEditorLaf::CNotepadEditorLaf
// public Constructor
// -----------------------------------------------------------------------------
//
CNotepadEditorLaf::CNotepadEditorLaf(CEikDialog* aParent)
    : CNotepadViewerLaf(aParent)
    {
    }

// -----------------------------------------------------------------------------
// CNotepadEditorLaf::~CNotepadEditorLaf
// Destructor
// -----------------------------------------------------------------------------
//
CNotepadEditorLaf::~CNotepadEditorLaf()
    {
    iRects.Close();
    }

// -----------------------------------------------------------------------------
// CNotepadEditorLaf::AddRectsAndLayout
// from CNotepadViewerLaf (called from SizeChanged)
// -----------------------------------------------------------------------------
//
void CNotepadEditorLaf::AddRectsAndLayout(const TRect& aRect, RRegion& aRegion)
    {
    CNotepadViewerLaf::AddRectsAndLayout(aRect, aRegion);
	TAknLayoutRect mainNotesPane;
	TAknLayoutRect lineLayout;
	TAknLayoutRect line;
	TAknLayoutScalableParameterLimits textLimits ;

	if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
		{
		TRect tempMainPane;
		TRect statusPane;
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,tempMainPane);
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,statusPane);
		TRect mainPane(TPoint(0, 0), tempMainPane.Size());
		mainNotesPane.LayoutRect(mainPane, AknLayoutScalable_Apps::main_notes_pane().LayoutLine());
		lineLayout.LayoutRect(mainNotesPane.Rect(),AknLayoutScalable_Apps::list_notes_pane().LayoutLine());
		textLimits = AknLayoutScalable_Apps::list_notes_text_pane_ParamLimits();
		}
	TInt lineCount = textLimits.LastRow() - textLimits.FirstRow() + 1;
	// The array iRects should be reset, when there is a change 
	// in the no of lines after a layout switch
	if ( iRects.Count() != lineCount )
		{
		iRects.Reset();
   		for (TInt j(textLimits.FirstRow()); j <= textLimits.LastRow(); j++)
			{
    		iRects.Append(TAknLayoutRect());
			}
		}
	if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
		{
		for ( TInt i(textLimits.FirstRow()); i <= textLimits.LastRow(); i++ )
			{
			line.LayoutRect(lineLayout.Rect(),AknLayoutScalable_Apps::list_notes_text_pane(i).LayoutLine());
			iRects[i].LayoutRect(line.Rect() ,AknLayoutScalable_Apps::list_notes_text_pane_g1().LayoutLine());
			aRegion.AddRect(iRects[i].Rect());
			}
		}
	else
		{
		for ( TInt i(0); i < KNotepadEditorLines; i++ )
			{
			iRects[i].LayoutRect(aRect,AppLayout::Note_writing_layout_elements_Line_5(i));
			aRegion.AddRect(iRects[i].Rect());
			}
		}
    }

// -----------------------------------------------------------------------------
// CNotepadEditorLaf::ConstructFromResourceL
// from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadEditorLaf::ConstructFromResourceL(TResourceReader& aReader)
    {
	TAknLayoutScalableParameterLimits textLimits ;  
    CNotepadViewerLaf::ConstructFromResourceL(aReader); 
    
    if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
		{
		textLimits = AknLayoutScalable_Apps::list_notes_text_pane_ParamLimits();
	    for (TInt i(textLimits.FirstRow()); i <= textLimits.LastRow(); i++)
		    {
	        User::LeaveIfError(iRects.Append(TAknLayoutRect()));
			}
		}
	else
		{
    	for (TInt i(0); i < KNotepadEditorLines; i++)
        	{
        	User::LeaveIfError(iRects.Append(TAknLayoutRect()));
        	}
		}
    }

// -----------------------------------------------------------------------------
// CNotepadEditorLaf::Draw
// from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadEditorLaf::Draw(const TRect& aRect) const
    {
    CNotepadViewerLaf::Draw(aRect);
    CWindowGc& gc = SystemGc();
    for ( TInt i(iRects.Count() - 1); i >= 0; i-- )
        {
        iRects[i].DrawRect(gc);
        }
    }

// End of File  
