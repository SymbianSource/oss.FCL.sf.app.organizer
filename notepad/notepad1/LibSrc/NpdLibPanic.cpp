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
* Description:  Implementation of Panic function of Notepad Library.
*
*/


// INCLUDE FILES
#include <e32std.h>
#include "NpdLibPanic.h"

// CONSTANTS
_LIT(KNotepadLibraryPanic, "NPDLIB");

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// Panic
// -----------------------------------------------------------------------------
//
GLDEF_C void Panic(
    TNotepadLibraryPanic aPanic ) // Panic code of this module
    {
    User::Panic(KNotepadLibraryPanic, aPanic);
    }

// End of File  
