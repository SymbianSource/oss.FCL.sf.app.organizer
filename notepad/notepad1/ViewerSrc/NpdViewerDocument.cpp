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
* Description:  Implementation of Notepad Viewer Document class.
*
*/


// INCLUDE FILES
#include "NpdViewerDocument.h"
#include "NpdViewerAppUi.h"
#include <eikdoc.h>
#include <s32file.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNotepadViewerDocument()
// Constructor.
// -----------------------------------------------------------------------------
//
CNotepadViewerDocument::CNotepadViewerDocument(
    CEikApplication& aApp )
    : CEikDocument(aApp)
    {
    }

// -----------------------------------------------------------------------------
// ---------------------------------------------------------
// CNotepadViewerDocument::CreateAppUiL()
// constructs CNotepadViewerAppUi
// -----------------------------------------------------------------------------
//
CEikAppUi* CNotepadViewerDocument::CreateAppUiL()
    {
    return new(ELeave) CNotepadViewerAppUi;
    }

// -----------------------------------------------------------------------------
// CNotepadViewerDocument::OpenFileL
// -----------------------------------------------------------------------------
//
CFileStore* CNotepadViewerDocument::OpenFileL(
    TBool /*aDoOpen*/, 
    const TDesC& aFilename,
    RFs& /*aFs*/)
    {
    iAppUi->OpenFileL(aFilename);
    return NULL;
    }

// -----------------------------------------------------------------------------
// void CNotepadViewerDocument::OpenFileL(CFileStore*& aFileStore, RFile& aFile)
// -----------------------------------------------------------------------------
//
    
void CNotepadViewerDocument::OpenFileL(
	CFileStore*& /*aFileStore*/,
	RFile& aFile)
	{
	((CNotepadViewerAppUi*) iAppUi)->OpenFileL(aFile);
	}
// End of File  
