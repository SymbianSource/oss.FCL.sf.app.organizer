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
* Description:  Implementation of Notepad Document class.
*
*/


// INCLUDE FILES
#include "NpdDocument.h"
#include "NpdAppUi.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadDocument::CNotepadDocument
// C++ constructor.
// -----------------------------------------------------------------------------
//
CNotepadDocument::CNotepadDocument(CEikApplication& aApp)
    : CAknDocument(aApp)
    {
    }

// -----------------------------------------------------------------------------
// CNotepadDocument::CreateAppUiL
// Construct CNotepadAppUi and return it.
// -----------------------------------------------------------------------------
//
CEikAppUi* CNotepadDocument::CreateAppUiL()
    {
    return new(ELeave) CNotepadAppUi;
    }

// End of File  
