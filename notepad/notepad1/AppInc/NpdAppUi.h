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
* Description:  Declaration of Notepad Application UI class.
*
*/


#ifndef NPDAPPUI_H
#define NPDAPPUI_H

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CNotepadListDialog;
class CNotepadApi;

// CLASS DECLARATION
/**
* Notepad application UI class.
* Notepad Application UI class using CNotepadApi.
*/
class CNotepadAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
        * second phase constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        virtual ~CNotepadAppUi();

    private: // Functions from base classes

        /**
        * From CEikAppUi.
        *
        * @param aCommand command to be handled.
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi.
        * Command line processing.
        *
        * When Notepad is launched by Shell, aTail is not set.
        * And Notepad normally starts in list mode.
        *
        * When Pinboard.app opens a short-cut of a memo
        * while Notepad is not running, Pinboard tries to launch Notepad 
        * with the memo's KEY number on the command line (aTail).
        * When Notepad is launched by Pinboard, Notepad must start 
        * in edit mode. 
        *
        * So this function looks at aTail parameter and set initial mode
        * properly.
        *
        * @param aCommand command.
        * @param aDocumentName Filename.
        * @param aTail optional command line parampeter.
        * @return ETrue if aDocumentName file exists.
        */
        TBool ProcessCommandParametersL(
            TApaCommand aCommand,
            TFileName& aDocumentName,
            const TDesC8& aTail );

        /**
        * From MCoeMessageObserver.
        * Message processing.
        *
        * While Notepad is running, Notepad may receive a message 
        * from Pinboard, whose parameter contains a memo Key number.
        * If Notepad receive a such message, it should be forced to 
        * switch edit mode with the specified memo.
        *
        * @param aClientHandleOfTargetWindowGroup client handle.
        * @param aMessageUid Uid.
        * @param aMessageParameters message containing a memo Key number.
        * @return ETrue if aDocumentName file exists.
        */
        MCoeMessageObserver::TMessageResponse HandleMessageL(
            TUint32 aClientHandleOfTargetWindowGroup, 
            TUid aMessageUid, 
            const TDesC8& aMessageParameters);

        /**
        * From CCoeAppUiBase.
        * Prepares the application to exit. Called after the exiting from 
        * the application's event loop but before the control environment 
        * active scheduler is stopped and the control environment destroyed.
        */
        void PrepareToExit();

    private: // Data

        CNotepadListDialog* iDialog; // never own
        CNotepadApi* iApi; // own
    };

#endif // NPDAPPUI_H

// End of File
