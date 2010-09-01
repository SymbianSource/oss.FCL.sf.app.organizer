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
* Description:   This contains the E32Main, the entry point of the clock application.
*
*/

// System includes
#include <e32def.h>
#include <e32base.h>
#include <eikstart.h>

// User includes
#include "clockapplication.h"

// ---------------------------------------------------------
// NewApplication()
// Creates a new application
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CClockApplication;
    }

// ---------------------------------------------------------
// E32Main()
// Main entry point of the application
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of file
