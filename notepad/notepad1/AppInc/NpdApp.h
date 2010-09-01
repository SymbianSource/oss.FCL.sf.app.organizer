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
* Description:  Declaration of Notepad Application class.
*
*/


#ifndef NPDAPP_H
#define NPDAPP_H

// INCLUDES
#include <aknapp.h>
#include "Notepad.hrh"

// CONSTANTS
const TUid KUidNotepad = { NOTEPAD_UID };

// CLASS DECLARATION
/**
* Notepad application class.

*/
class CNotepadApp : public CAknApplication
    {
    private: // Functions from base classes

        /**
        * From CApaApplication.
        * Returns application's UID (KUidNotepad).
        *
        * @return The value of KUidNotepad.
        */
        TUid AppDllUid() const;

        /**
        * From CApaApplication.
        * Creates CNotepadDocument document object.
        *
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // NPDAPP_H

// End of File

