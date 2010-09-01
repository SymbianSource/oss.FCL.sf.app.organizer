/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Creates the application document and returns the application UID. 
*
*/






// User Includes
#include "srchuiappdocument.h"
#include "srchuiappapplication.h"

//-----------------------------------------------------------
// UID for the application, this should correspond to the uid
// defined in the mmp file
//-----------------------------------------------------------
static const TUid KUidSearchApplicationApp = {0x10282411};



//-----------------------------------------------------------
// Return the UID for the SearchApplication application
//-----------------------------------------------------------
TUid CSrchUiAppApplication::AppDllUid() const
    {
    return KUidSearchApplicationApp;
    }


//-----------------------------------------------------------
// Create an SearchApplication document, and return 
// a pointer to it
//-----------------------------------------------------------
CApaDocument* CSrchUiAppApplication::CreateDocumentL()
    {  
    CApaDocument* document = CSrchUiAppDocument::NewL(*this);
    return document;
    }

// end of file
