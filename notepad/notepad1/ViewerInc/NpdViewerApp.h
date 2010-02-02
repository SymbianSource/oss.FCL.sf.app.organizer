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
* Description:  Declarations for Notepad Viewer Application class.
*
*/


#ifndef NOTEPADVIEWERAPP_H
#define NOTEPADVIEWERAPP_H

// INCLUDES
#include <aknapp.h>
#include <NpdApi.h>
#include "NpdViewer.hrh"

// CONSTANTS
const TUid KUidNpdViewer = { NPDVIEWER_UID };

// CLASS DECLARATION
/**
* Notepad application class.
* CNotepadViewerApp application class.
* Provides factory to create concrete document object.
* 
*/
class CNotepadViewerApp : public CAknApplication
    {
    private: // Functions from base classes

        /**
        * From CApaApplication.
        * Creates CNotepadViewerDocument document object.
        *
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication.
        * returns application's UID (KUidNpdViewer).
        *
        * @return The value of KUidNpdViewer.
        */
        TUid AppDllUid() const;
    };

#endif // NOTEPADVIEWERAPP_H

// End of File
