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
* Description:   Calendar Application Document
*
*/



#ifndef CALENDOC_H
#define CALENDOC_H

// Include files
#include <AknDoc.h>                 // CAknDocument

// Forward Declarations
class CCalenController;             // Calendar Controller

/**
* CCalenDocument Class Definition
* Document class for Calendar application.  The document takes ownership
* of the Calendar controller and is responsible for its destruction
*/
NONSHARABLE_CLASS( CCalenDocument ) : public CAknDocument
    {
    public:
        /**
        * First phase construction
        * Creates a new CCalenDocument instance
        * @param aApp Reference to a CCalenApplication
        */
        static CCalenDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor
        * Frees all resources used by this class
        */  
        virtual ~CCalenDocument();
    
    public:
        /**
        * Takes ownership of the controller.        
        * CCalenDocument owns the controller so that any customisation 
        * plugins that provide custom views can be successfully unloaded.
        * The CAknAppUi deletes the view array after ~CCalenAppUi has 
        * been called, therefore the plugins have to be deleted after the
        * CAknAppUI destructor has been called.        
        *  
        * The design of the Eikon framework is that the framework uses 
        * CreateAppUiL to create the appUi, and later on the AppUi's second
        * stage constructor is called. As the controller is dependent on the
        * appUi, it has to created in the AppUi's ConstructL() and ownership
        * passed to CCalenDocument.
        */ 
        void SetController( CCalenController* aController );
    

    private:
        /**
        * Private constructor
        * Creates a new CCalenDocument instance
        * @param aApp Reference to a CCalenApplication
        */  
        CCalenDocument( CEikApplication& aApp );

        /**
        * From CEikDocument 
        * Creates a new CCalenAppUi instance
        * @return CCalenAppUi pointer
        */
        CEikAppUi* CreateAppUiL(void);
        
    private:
        /**
        * The controller is created by the appUi and then passes
        * ownership to the document class
        */
        CCalenController* iController;
    };


#endif  //  CALENDOC_H

// End of File
