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
* Description:  Declarations for Notepad Viewer Document class.
*
*/


#ifndef NOTEPADVIEWERDOCUMENT_H
#define NOTEPADVIEWERDOCUMENT_H

// INCLUDES
#include <eikdoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION
/**
*  Notepad Viewer Document class.
*  Notepad Viewer is not file-based.
*
*/
class CNotepadViewerDocument : public CEikDocument
    {
    public: // Constructor

        /**
        * Constructor. (no need for two-phased construction)
        *
        * @param aApp reference to an application object.
        */
        CNotepadViewerDocument(CEikApplication& aApp);

    private: // Functions from base classes

        /**
        * From CEikDocument.
        * Create CNotepadViewerAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();

        /**
        * From CEikDocument.
        * Always return NULL, so as not to create a INI file.
        *
        * @param aDoOpen ETrue if file exists. (ignored)
        * @param aFilename file to view. 
        * @param aFs FileServerSession. (ignored)
        * @return file store. (always NULL)
        */
        CFileStore* OpenFileL(TBool aDoOpen, const TDesC& aFilename,RFs& aFs);

        /**
        * From CEikDocument.
        *
        * @param aFileStore. (ignored)
        * @param aFile. 
        */
        void OpenFileL(CFileStore*& aFileStore, RFile& aFile);

    };

#endif // NOTEPADVIEWERDOCUMENT_H

// End of File

