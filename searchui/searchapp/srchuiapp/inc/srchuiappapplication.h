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






#ifndef C_CSRCHUIAPPAPPLICATION_H
#define C_CSRCHUIAPPAPPLICATION_H

//System Include
#include <aknapp.h>

/**
 *  This classs is the  application part of the AVKON application
 *  framework for the SearchApplication application.
 */

class CSrchUiAppApplication : public CAknApplication
    {
  public:  // from CAknApplication

    /** 
	* Returns the application DLL UID value
	* @param
	* @return the UID of this Application/Dll
	*/
    TUid AppDllUid() const;

  protected: // from CAknApplication
    /** 
	* Create a CApaDocument object and return a pointer to it
	*
	* @return pointer to the created document
	*/
    CApaDocument* CreateDocumentL();
    
    };

#endif // C_CSRCHUIAPP_APPLICATION_H_
