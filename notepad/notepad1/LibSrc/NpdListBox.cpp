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
* Description:  Implementation of List of Memos/Templates.
*
*/


// INCLUDE FILES
#include <barsread.h>
#include <touchfeedback.h>
#include <touchlogicalfeedback.h>
#include <NpdLib.rsg>
#include "NpdListBox.h"
#include "NpdListDialog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadListBox::LoadMarginsL
// -----------------------------------------------------------------------------
//
void CNotepadListBox::LoadMarginsL()
    {
    TResourceReader rr;
    iCoeEnv->CreateResourceReaderLC(rr, R_NOTEPAD_MARGINS); // Push rr
    iMargins.iLeft = rr.ReadInt16();  // WORD left
    iMargins.iRight = rr.ReadInt16(); // WORD right
    CleanupStack::PopAndDestroy(); // rr
    }

// -----------------------------------------------------------------------------
// CNotepadListBox::SizeChanged
// 
// The margins are set by CAknSingleStyleListBox::SizeChanged() using 
// SetupColumnTextCellL. Thus CNotepadListBox overrides SizeChanged() to
// change the margins just set by CAknSingleStyleListBox::SizeChanged().
// -----------------------------------------------------------------------------
//
void CNotepadListBox::SizeChanged()
    {
    CAknSingleHeadingStyleListBox::SizeChanged();
    }

void CNotepadListBox::HandlePointerEventL( const TPointerEvent& aPointerEvent )
	{
	if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
	    {
	    if( aPointerEvent.iModifiers & EModifierShift )
	        {
	        STATIC_CAST( CNotepadListDialog*,&iParent )->MiddleSoftKeyL();
	        }
        else if ( Model()->NumberOfItems() == 0 )
            {
            STATIC_CAST( CNotepadListDialog*,&iParent )->HandleListBoxPointerEventL( CNotepadListDialog::EOpenListBoxContextMenu );
            }
	    }
   	if( ( aPointerEvent.iType == TPointerEvent::EButton1Down )&&( Model()->NumberOfItems() == 0 ) )
			{
			MTouchFeedback* feedback = MTouchFeedback::Instance();
			if(feedback)
				{
				 feedback->InstantFeedback( ETouchFeedbackBasic );
				}
			}
    CAknSingleHeadingStyleListBox::HandlePointerEventL( aPointerEvent );
	}

CNotepadListBox::CNotepadListBox(CNotepadDialogBase& aParent)
:CAknSingleHeadingStyleListBox(),iParent(aParent)
	{
		
	}
// End of File  
