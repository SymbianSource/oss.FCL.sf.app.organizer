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
* Description:  Implementation of Notepad application class.
*
*/


// INCLUDE FILES
#include "NpdApp.h"
#include "NpdDocument.h"




#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CNotepadApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// -----------------------------------------------------------------------------
// E32Dll(TDllReason) 
// Entry point function for SymbianOS Apps
// Returns: KErrNone: No error
// -----------------------------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
 {
    return KErrNone;
 }
#endif 



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadApp::AppDllUid
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CNotepadApp::AppDllUid() const
    {
    return KUidNotepad;
    }

// -----------------------------------------------------------------------------
// CNotepadApp::CreateDocumentL
// Creates CNotepadDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CNotepadApp::CreateDocumentL()
    {
    return new(ELeave) CNotepadDocument(*this);
    }

// End of File  

