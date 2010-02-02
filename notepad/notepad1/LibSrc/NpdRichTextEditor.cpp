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


#include "NpdRichTextEditor.h"
#include "NpdEdwinBase.h"
#include "NpdDialogBase.h"
#include "NpdLibPanic.h"
#include "NpdViewerDialog.h"
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadRichTextEditor::DoEditorLayoutL
// Sets the layout of the Edwin and the scrollbar.
// -----------------------------------------------------------------------------
//
void CNotepadRichTextEditor::DoEditorLayoutL()
    {
    iEdwinBase->DoEditorLayoutL();
    }

// -----------------------------------------------------------------------------
// CNotepadRichTextEditor::InitNotepadEditorL
// -----------------------------------------------------------------------------
//
void CNotepadRichTextEditor::InitNotepadEditorL(
    CEikDialog* aParent,CNotepadEdwinLines* aEdwinLines)
    {
    iEdwinBase->InitNotepadEditorL(aParent,ETrue,aEdwinLines);
    }

// -----------------------------------------------------------------------------
// CNotepadRichTextEditor::InitNotepadEditorL
// -----------------------------------------------------------------------------
//
void CNotepadRichTextEditor::ConstructBaseL(CNotepadDialogBase* aBaseDialog)
    {
    iEdwinBase = new (ELeave) CNotepadEdwinBase();
    iEdwinBase->Share(this,this);	
    iBaseDialog = aBaseDialog;
    }

// -----------------------------------------------------------------------------
// CNotepadRichTextEditor::SetBackgroundColorL
// -----------------------------------------------------------------------------
//
void CNotepadRichTextEditor::SetBackgroundColorL()
    {
    iEdwinBase->SetBackgroundColorL();
    }

// -----------------------------------------------------------------------------
// CNotepadRichTextEditor::HandleResourceChange
// from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadRichTextEditor::HandleResourceChange(TInt aType)
    {
    iEdwinBase->HandleResourceChange(aType);
    }

// -----------------------------------------------------------------------------
// CNotepadRichTextEditor::Draw
// from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadRichTextEditor::Draw(const TRect& aRect) const   
    {
    iEdwinBase->Draw(aRect);
    }

// ---------------------------------------------------------
// CNotepadRichTextEditor::MopSupplyObject(TTypeUid aId)
// Gets an object whose type is encapsulated by 
// the specified TTypeUid object.
// ---------------------------------------------------------
//
TTypeUid::Ptr CNotepadRichTextEditor::MopSupplyObject(TTypeUid aId)
	{
	TTypeUid::Ptr ptr = TTypeUid::Null();
	if(iEdwinBase->SupplyMopObject(aId,ptr))
		return ptr;
	return CCoeControl::MopSupplyObject(aId);
	}

// ---------------------------------------------------------
// CNotepadRichTextEditor::~CNotepadRichTextEditor()
// Destructor
// ---------------------------------------------------------
//
CNotepadRichTextEditor::~CNotepadRichTextEditor()
	{
	delete iEdwinBase;
	}

// ---------------------------------------------------------
// CNotepadEdwin::GetLayout()
// returns reference to iLayout
// ---------------------------------------------------------
//

CTextLayout& CNotepadRichTextEditor::GetLayout()
	{
	return *iLayout;
	}

// ---------------------------------------------------------
// CNotepadEdwin::GetLayout()
// returns reference to iTextView
// ---------------------------------------------------------
//
CTextView& CNotepadRichTextEditor::GetTextView()
	{
	return *iTextView;	
	}

// ---------------------------------------------------------
// CNotepadEdwin::GetLayout()
// Calls SetCanDrawOusideRect()
// ---------------------------------------------------------
//
void CNotepadRichTextEditor::CallSetCanDrawOutsideRect()
	{
	SetCanDrawOutsideRect();
	}

// ---------------------------------------------------------
// CNotepadEdwin::GetLayout()
// returns reference to iCoeEnv
// ---------------------------------------------------------
//

CCoeEnv& CNotepadRichTextEditor::GetCoeEnv()
	{
	return *iCoeEnv;
	}

void CNotepadRichTextEditor::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	__ASSERT_DEBUG( iBaseDialog, Panic(ENotepadLibraryPanicNoEdwin) );
static_cast<CNotepadViewerDialog*>	(iBaseDialog)->HandleDialogPointerEventL(aPointerEvent);
 
	CEikRichTextEditor::HandlePointerEventL(aPointerEvent);
	}

// End of File  
