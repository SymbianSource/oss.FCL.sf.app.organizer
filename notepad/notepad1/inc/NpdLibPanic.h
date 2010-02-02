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
* Description:  Declaration of Panic Codes of Notepad Libarary.
*
*/


#ifndef NPDLIBPANIC_H
#define NPDLIBPANIC_H

// INCLUDES
#include <e32def.h>

enum TNotepadLibraryPanic
    {
    ENotepadLibraryPanicInvalidArgument,
    ENotepadLibraryPanicInvalidKey,
    ENotepadLibraryPanicNullDataFileName,
    ENotepadLibraryPanicDatabaseAlreadyOpened,
    ENotepadLibraryPanicInvalidRowInView,
    ENotepadLibraryPanicNoModel,
    ENotepadLibraryPanicNoEdwin,
    ENotepadLibraryPanicNoTextInEdwin,
    ENotepadLibraryPanicNoMenuResource,
    ENotepadLibraryPanicNullMenuPane,
    ENotepadLibraryPanicEditorInvalidMode,
    ENotepadLibraryPanicEditorEmpty,
    ENotepadLibraryPanicNullSequenceInEditor,
    ENotepadLibraryPanicNoPaperLines,
    ENotepadLibraryPanicNoControlForDecorationGraphics,
    ENotepadLibraryPanicOnlySupportedInNotepad,
    ENotepadLibraryPanicInvalidIndex,
    ENotepadLibraryPanicNoItem,
    ENotepadLibraryPanicInvalidFinalValueOfProgressNote,
    ENotepadLibraryPanicNullListControl,
    ENotepadLibraryPanicInvalidWidthOfListBox,
    ENotepadLibraryPanicNoFileSpecified,
    ENotepadLibraryPanicProgressDialogNotReady
    };

GLREF_C void Panic(TNotepadLibraryPanic aPanic);

#endif // NPDLIBPANIC_H

// End of File
