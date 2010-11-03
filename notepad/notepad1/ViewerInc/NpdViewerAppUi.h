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
* Description:  Declarations for Notepad Viewer Application UI class.
*
*/


#ifndef NOTEPADVIEWERAPPUI_H
#define NOTEPADVIEWERAPPUI_H

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CNotepadViewerDialog;
class CNotepadApi;

// CLASS DECLARATION
/**
* Notepad Viewer application UI class.
* 
*/
class CNotepadViewerAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
        * Second phase onstructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        virtual ~CNotepadViewerAppUi();

    private: // Functions from base classes

        /**
        * From CEikAppUi.
        * Takes care of command handling.
        *
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi.
        * Command line processing.
        *
        * When DocumentHandler wants to launch NpdViewer as a standalone
        * application, it use RApaLsSession::StartDocument(filename, ...).
        *
        * CEikonEnv::ConstructAppFromCommandLineL(...), at first, pass 
        * the filename information to ProcessCommandParametersL 
        * correctly. But default CEikAppUi::ProcessCommandParametersL 
        * overwrite it's reference parameter (TFileName& aDocumentName) to 
        * application's defualt document name such as "NpdViewer.ini".
        * (In EikAppUi.cpp, 
        * Application()->GetDefaultDocumentFileName(aDocumentName);
        * do this). 
        * So,  when CEikonEnv::ConstructAppFromCommandLineL(...) calls
        * CApaDocument::OpenFileL(...), the filename information from 
        * Document Handler had been lost.
        * 
        * On the other hand, when DocumentHandler wants to launch NpdViewer 
        * as a embeded application, it use CApaDocument::OpenFileL() directly 
        * and never call ProcessCommandParametersL.
        *
        * So, in order to pass a correct filename information to OpenFileL 
        * in both case, we decide to override this function.
        *
        * @param aCommand command. (ignored)
        * @param aDocumentName Filename.
        * @param aTail optional command line parampeter. (ignored)
        * @return ETrue if aDocumentName file exists.
        */
        TBool ProcessCommandParametersL(
            TApaCommand aCommand,
            TFileName& aDocumentName,
            const TDesC8& aTail);

        /**
        * From CEikAppUi.
        * Start fileviewer with specified filename.
        * Both standalone and embeded case, this function is really called 
        *
        * @param aFilename Filename to view.
        */
        void OpenFileL(const TDesC& aFilename);

	public:
	
        /**
        * Start fileviewer with specified file handle.
        * Both standalone and embeded case, this function is really called 
        *
        * @param aFile File handle.
        */
        void OpenFileL(RFile& aFile);

    private: // Data

        CNotepadApi* iApi; // own
        TBool iCurrentFileOpened; // To avoid reopening of same file.

    };

#endif // NOTEPADVIEWERAPPUI_H

// End of File
