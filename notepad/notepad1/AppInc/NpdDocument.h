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
* Description:  Declaration of Notepad Document class.
*
*/


#ifndef NPDDOCUMENT_H
#define NPDDOCUMENT_H

// INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION
/**
* Notepad Document class.
* Notepad application is not file based.
*
*/
class CNotepadDocument : public CAknDocument
    {
    public: // Constructor

        /**
        * Constructor.
        *
        * @param aApp reference to an application object.
        */
        CNotepadDocument(CEikApplication& aApp);

    private: // Functions from base classes

        /**
        * From CEikDocument.
        * Create CNotepadAppUi "App UI" object.
        *
        * @return created CEikAppUi object.
        */
        CEikAppUi* CreateAppUiL();

    };

#endif // NPDDOCUMENT_H

// End of File

