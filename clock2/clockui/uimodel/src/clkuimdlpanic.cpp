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
* Description:  This is the source file for the Clock UI Model Panic and Fault definitions
*
*/

// System includes
#include <e32base.h>

// User includes
#include "clkuimdlpanic.h"

// Constants

// ---------------------------------------------------------
// Panic
// rest of the details are commented in the header
// ---------------------------------------------------------
//
GLDEF_C void Panic( TClkUiMdlPanic aPanic )
    {
    _LIT( KModuleName, "CLKUI-MDL-PANIC" );
    User::Panic( KModuleName, aPanic );
    }

// ---------------------------------------------------------
// Fault
// rest of the details are commented in the header
// ---------------------------------------------------------
//
GLDEF_C void Fault( TClkUiMdlFault aFault )
    {
    _LIT( KModuleName, "CLKUI-MDL-FAULT" );
    User::Panic( KModuleName, aFault );
    }

// End of file
