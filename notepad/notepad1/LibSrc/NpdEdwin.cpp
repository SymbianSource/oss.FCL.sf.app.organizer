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


#include "NpdEdwin.h"
#include "NpdEdwinBase.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadEdwin::DoEditorLayoutL
// Sets the layout of the Edwin and the scrollbar.
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::DoEditorLayoutL()
    {
    iEdwinBase->DoEditorLayoutL();
    }

// -----------------------------------------------------------------------------
// CNotepadEdwin::InitNotepadEditorL
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::InitNotepadEditorL(
    CEikDialog* aParent,
    CNotepadEdwinLines* aEdwinLines)
    {
    iEdwinBase->InitNotepadEditorL(aParent,EFalse,aEdwinLines);
    }

// -----------------------------------------------------------------------------
// CNotepadRichTextEditor::InitNotepadEditorL
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::ConstructBaseL()
    {
    iEdwinBase = new (ELeave) CNotepadEdwinBase();
    iEdwinBase->Share(this,this);	
    }
    
// -----------------------------------------------------------------------------
// CNotepadEdwin::SetBackgroundColorL
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::SetBackgroundColorL()
    {
    iEdwinBase->SetBackgroundColorL();
    }

// -----------------------------------------------------------------------------
// CNotepadEdwin::HandleResourceChange
// from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::HandleResourceChange( TInt aType )
    {
    if ( iEdwinBase )
    	{
        iEdwinBase->HandleResourceChange( aType );
    	}
    }

// -----------------------------------------------------------------------------
// CNotepadEdwin::Draw
// from CCoeControl
// -----------------------------------------------------------------------------
//
void CNotepadEdwin::Draw(const TRect& aRect) const   
    {
    iEdwinBase->Draw(aRect);
    }

// ---------------------------------------------------------
// CNotepadEdwin::MopSupplyObject(TTypeUid aId)
// Gets an object whose type is encapsulated by 
// the specified TTypeUid object.
// ---------------------------------------------------------
//
TTypeUid::Ptr CNotepadEdwin::MopSupplyObject(TTypeUid aId)
	{
	TTypeUid::Ptr ptr = TTypeUid::Null();
	if(iEdwinBase->SupplyMopObject(aId,ptr))
		{
			return ptr;
		}
		//return CCoeControl::MopSupplyObject(aId);
		return CEikEdwin::MopSupplyObject(aId);
	}

// ---------------------------------------------------------
// CNotepadEdwin::~CNotepadEdwin()
// Destructor
// ---------------------------------------------------------
//
CNotepadEdwin::~CNotepadEdwin()
	{
	delete iEdwinBase;
	}

// ---------------------------------------------------------
// CNotepadEdwin::GetLayout()
// returns reference to iLayout
// ---------------------------------------------------------
//

CTextLayout& CNotepadEdwin::GetLayout()
	{
	return *iLayout;
	}

// ---------------------------------------------------------
// CNotepadEdwin::GetLayout()
// returns reference to iTextView
// ---------------------------------------------------------
//
CTextView& CNotepadEdwin::GetTextView()
	{
	return *iTextView;	
	}

// ---------------------------------------------------------
// CNotepadEdwin::GetLayout()
// Calls SetCanDrawOusideRect()
// ---------------------------------------------------------
//
void CNotepadEdwin::CallSetCanDrawOutsideRect()
	{
	SetCanDrawOutsideRect();
	}

// ---------------------------------------------------------
// CNotepadEdwin::GetLayout()
// returns reference to iCoeEnv
// ---------------------------------------------------------
//

CCoeEnv& CNotepadEdwin::GetCoeEnv()
	{
	return *iCoeEnv;
	}
// End of File  
