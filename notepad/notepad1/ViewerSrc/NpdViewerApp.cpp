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
* Description:  Implementation of Notepad Viewer Application class.
*
*/


// INCLUDE FILES
#include "NpdViewerApp.h"
#include "NpdViewerDocument.h"



#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CNotepadViewerApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

#if defined(__WINS__) && !defined(EKA2)
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }

EXPORT_C TInt WinsMain(TDesC* aCmdLine)
    {
    return EikStart::RunApplication(NewApplication, aCmdLine);
    }
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadViewerApp::CreateDocumentL()
// Creates CNotepadViewerDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CNotepadViewerApp::CreateDocumentL()
    {
    return new(ELeave) CNotepadViewerDocument(*this);
    }

// -----------------------------------------------------------------------------
// CNotepadViewerApp::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CNotepadViewerApp::AppDllUid() const
    {
    return KUidNpdViewer;
    }



// End of File  

