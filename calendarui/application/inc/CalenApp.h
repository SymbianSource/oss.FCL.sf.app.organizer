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
* Description:   Calendar Application
*
*/



#ifndef CALENAPP_H
#define CALENAPP_H

// Include files
#include <aknapp.h>             // CAknApplication

/**
* CCalenApplication class definition
* Provides factory to create concrete document object.
*/

NONSHARABLE_CLASS( CCalenApplication ) : public CAknApplication
    {
    private:
        /**
        * From CApaApplication, creates CCalenDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication, returns the application UID
        * @return Calendar application UID
        */
        TUid AppDllUid() const;
    };

#endif //  CALENAPP_H

// End of File

