/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     An entry point of the application.  
*
*/







//System Includes
#include <eikstart.h>

//User Includes
#include "srchuiappapplication.h"

#ifdef __UI_FRAMEWORKS_V2__

//-----------------------------------------------------------
// Create an application, and return a pointer to it
//-----------------------------------------------------------
CApaApplication* NewApplication()
    {
    return new CSrchUiAppApplication;
    }

//-----------------------------------------------------------
// Main function of the application executable.
//-----------------------------------------------------------
TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

//-----------------------------------------------------------
// The following is required for wins on EKA1 
// (using the exedll target)
//-----------------------------------------------------------
#if defined(__WINS__) && !defined(EKA2)

//-----------------------------------------------------------
// Required for wins
//-----------------------------------------------------------
EXPORT_C TInt WinsMain(TDesC* aCmdLine)
    {
    return EikStart::RunApplication(NewApplication, aCmdLine);
    }

//-----------------------------------------------------------
// Dll entry point
//-----------------------------------------------------------
TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif

#else // __UI_FRAMEWORKS_V2__

//-----------------------------------------------------------
// Create an application, and return a pointer to it
//-----------------------------------------------------------
EXPORT_C CApaApplication* NewApplication()
    {
    return new CSrchUiAppApplication;
    }

//-----------------------------------------------------------
// DLL entry point, return that everything is ok
//-----------------------------------------------------------
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif // __UI_FRAMEWORKS_V2__

