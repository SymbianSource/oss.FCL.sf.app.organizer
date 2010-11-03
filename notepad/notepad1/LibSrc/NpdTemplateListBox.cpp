/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of List of Templates.
*
*/


// INCLUDE FILES
#include <barsread.h>
#include <NpdLib.rsg>
#include "NpdTemplateListBox.h"
#include "NpdListDialog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadTemplateListBox::HandlePointerEventL
// 
// Handle the pointer events.
// 
// -----------------------------------------------------------------------------
//
void CNotepadTemplateListBox::HandlePointerEventL( const TPointerEvent& aPointerEvent )
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
	 CAknSingleGraphicStyleListBox::HandlePointerEventL( aPointerEvent );

    }

// -----------------------------------------------------------------------------
// CNotepadTemplateListBox::CNotepadTemplateListBox
// 
// The constructor of CNotepadTemplateListBox.
// 
// -----------------------------------------------------------------------------
//
CNotepadTemplateListBox::CNotepadTemplateListBox( CNotepadDialogBase& aParent )
                         :CAknSingleGraphicStyleListBox(), iParent( aParent ), iPreviousIndex( 0 )
    {
    }
// End of File  
