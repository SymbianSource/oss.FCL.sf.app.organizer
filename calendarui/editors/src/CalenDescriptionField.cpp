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
* Description:  ?Description
*
*/


// system includes
#include <AknUtils.h>
#include <AknDef.h>

// user includes
#include "CalenDescriptionField.h"
#include "CalenDescription.h"

// debug
#include "calendarui_debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CCalenDescriptionField::CCalenDescriptionField
// C++ constructor can NOT contain any code, that might leave.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenDescriptionField::CCalenDescriptionField( CCalenDescription& aDescription )
    : iDescription( aDescription )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDescriptionField::~CCalenDescriptionField
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCalenDescriptionField::~CCalenDescriptionField()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenDescriptionField::OfferKeyEventL
// Handle key events. Could show/edit/remove description.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CCalenDescriptionField::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                     TEventCode aType )
    {
    TRACE_ENTRY_POINT;
    
    TKeyResponse exitCode = EKeyWasNotConsumed;
    if (aType == EEventKey)
        {
        switch( aKeyEvent.iCode )
            {
            case EKeyOK:
                {
                iDescription.ShowL();
                exitCode = EKeyWasConsumed;
                }
                break;
           	case EKeyEnter:
           	    {
                iDescription.ShowL(); // Open notepad to edit description
                exitCode = EKeyWasConsumed;
           	    }
                break;            
            case EKeyBackspace:
                {
                iDescription.RemoveL();
                exitCode = EKeyWasConsumed;
                }
                break;
            default:
                break;
            }
        }
    else if( aType == EEventKeyDown )
        {
        TInt code = aKeyEvent.iScanCode;
        if ( ('0' <= code && code <= '9') || // digit keys 
             ('A' <= code && code <= 'Z') ) // character keys
            {
            // calls modal dialog, for which key is posted
            iDescription.EditL(); 
            exitCode = EKeyWasConsumed;
            }
        }
    else
        {
        // do nothing
        }

    if( exitCode != EKeyWasConsumed )
        {
        exitCode = CEikEdwin::OfferKeyEventL( aKeyEvent, aType );
        }

    TRACE_EXIT_POINT;
    return exitCode;
    }

// -----------------------------------------------------------------------------
// CCalenDescriptionField::SetTextL
// Sets the given text to the description field.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenDescriptionField::SetTextL( const TDesC* aData ) 
    {
    TRACE_ENTRY_POINT;

    // Shorten data  and take max length of head of text, 
    // otherwise edwin takes tail of text. 
    CTextLayout* layout = const_cast<CTextLayout *>( iTextView->Layout() );

    if ( !layout->Truncating() )
        {
        layout->SetTruncating( TRUE );
        }

    TInt maxLen = iTextLimit;
    HBufC* stripBuf = HBufC::NewLC( maxLen );
    TPtr stripPtr = stripBuf->Des();
    stripPtr.Append( aData->Left( maxLen ) );
    // Replace some common unicode control characters with spaces
    // as Edwin can't show them correctly. 
    // - normal ASCII line break (\n) 
    // - normal ASCII tab (\t)
    // - paragraph separator (0x2029)
    // - line break (0x2028)
    _LIT( KParagraphCharacter, "\n\t\x2029\x2028" );
    TChar space(' ');
    AknTextUtils::ReplaceCharacters( stripPtr, KParagraphCharacter, space );
    CEikEdwin::SetTextL( stripBuf );
    CleanupStack::PopAndDestroy( stripBuf );

    TRACE_EXIT_POINT;
    }

//  End of File
