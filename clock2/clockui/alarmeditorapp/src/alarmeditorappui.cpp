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
* Description:   This is the source file for the CAlarmEditorAppUi class.
*
*/

// System includes
#include <e32base.h>

// User includes
#include "alarmeditorapp.h"
#include "alarmeditorappui.h"
#include "clockalarmeditor.h"

// Constants

// Literals

// ---------------------------------------------------------
// CAlarmEditorAppUi::CAlarmEditorAppUi
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAlarmEditorAppUi::CAlarmEditorAppUi()
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CAlarmEditorAppUi::~CAlarmEditorAppUi
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAlarmEditorAppUi::~CAlarmEditorAppUi()
    {
	// No implementation yet.
    }

// ---------------------------------------------------------
// CAlarmEditorAppUi::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAlarmEditorAppUi::ConstructL()
	{
	//Single Click integration
    BaseConstructL( EStandardApp | EAknEnableSkin | EAknEnableMSK |
            EAknSingleClickCompatible );

    	
	// Construct the editor and launch it.
	if( !iAlarmEditor )
        {
        // Construct the alarm editor.
        iAlarmEditor = CClockAlarmEditor::NewL();
        
        // Launch the editor.
        if( iAlarmEditor )
            {
            iAlarmEditor->LaunchL();
            delete iAlarmEditor;
            iAlarmEditor = NULL;
            }
        
        User::Exit( 0 );
        }
	}

// ---------------------------------------------------------
// CAlarmEditorAppUi::HandleCommandL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAlarmEditorAppUi::HandleCommandL( TInt aCommandId )
    {
    switch( aCommandId )
    	{
    	case EEikCmdExit:
	    case EAknCmdExit:
	    	{
	    	Exit(); 	
	    	}
	    	break;
	    	
    	default:
    		{
    		// No implementation yet.
    		}
    		break;    	
    	}
    }

// End of file
