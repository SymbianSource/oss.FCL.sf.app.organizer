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
* Description:   This is the source file for the CAlarmEditorDocument class.
*
*/

// System includes

// User includes
#include "alarmeditordocument.h"
#include "alarmeditorappui.h"

// Constants

// Literals

// ---------------------------------------------------------
// CAlarmEditorDocument::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAlarmEditorDocument* CAlarmEditorDocument::NewL( CEikApplication& aApp )
    {
    CAlarmEditorDocument* selfObj = new ( ELeave ) CAlarmEditorDocument( aApp );
    CleanupStack::PushL( selfObj );
    
    selfObj->ConstructL();
    
    CleanupStack::Pop( selfObj );
    return selfObj;
    }

// ---------------------------------------------------------
// CAlarmEditorDocument::~CAlarmEditorDocument
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAlarmEditorDocument::~CAlarmEditorDocument()
    {
	// No implementation yet.
    }

// ---------------------------------------------------------
// CAlarmEditorDocument::CAlarmEditorDocument
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAlarmEditorDocument::CAlarmEditorDocument( CEikApplication& aApp ) : CAknDocument( aApp )
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CAlarmEditorDocument::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CAlarmEditorDocument::ConstructL()
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CAlarmEditorDocument::CreateAppUiL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CEikAppUi* CAlarmEditorDocument::CreateAppUiL()
    {
    return new( ELeave ) CAlarmEditorAppUi;
    }

// End of file
