/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar Application UI. Delegates responsibilities to 
*                Calendar Controller
*
*/



#ifndef CALENAPPUI_H
#define CALENAPPUI_H

// Include files
#include <aknViewAppUi.h>           // CAknViewAppUi

// Forward Declarations
class CCalenController;             // Calendar Controller

/**
* CCalenAppUi Class Definition
* AppUi class for Calendar application.  Creates the Calendar controller
* and passes ownership to CCalenDocument.  Passes framework calls through
* to the Calendar Controller.
*/
NONSHARABLE_CLASS( CCalenAppUi ) : public CAknViewAppUi
    {
    public:
        /**
        * Constructor
        * C++ default constructor
        */
        CCalenAppUi();
        
        /**
        * ConstructL
        * Second phase construction
        */
        void ConstructL();
        
        /**
        * Destructor
        * Frees all resources used by this class
        */          
        virtual ~CCalenAppUi();

    public:
        /**
        * From CEikAppUi
        * Handle any commands not handled directly
        * by the views or controller
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);
        
        /**
        * From CAknViewAppUi
        * Handles layout changes in AppUi
        * @param aCommand command to be handled
        */
        void HandleResourceChangeL(TInt aType);
    
    protected:
        /**
         * From CEikApUi
         * Handle applicaiton switching from( to ) background to( from )
         * foreground event
         * @param aForeground ETrue to switch to the foreground or EFalse otherwise
         */
        void HandleForegroundEventL( TBool aForeground );

    private:
        /**
        * From CEikAppUi
        * Handles command line launch options
        * @param aCommand EApaCommandOpen | EApaCommandCreate | etc...
        * @param aDocumentName A document file name
        * @param aTail command line parameter
        */
        TBool ProcessCommandParametersL( TApaCommand aCommand,
                                         TFileName& aDocumentName, 
                                         const TDesC8& aTail);
        /**
        * From MCoeMessageObserver
        * Handle window group messages
        * @param aClientHandleOfTargetWindowGroup Windou group id
        * @param aMessageUid Message id
        * @param aMessageParameters Message parameter
        */
        MCoeMessageObserver::TMessageResponse HandleMessageL( 
                                    TUint32 aClientHandleOfTargetWindowGroup,
                                    TUid aMessageUid,
                                    const TDesC8& aMessageParameters );

    private:
        // Member data
        CCalenController* iController; // Owned by CCalenDocument
    };

#endif // CALENAPPUI_H

// End of File
