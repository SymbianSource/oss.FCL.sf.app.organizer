/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This is the header file for the CAdtUpdaterApplication class.
*
*/

#ifndef __ADTUPDATER_APPLICATION_H__
#define __ADTUPDATER_APPLICATION_H__

// System includes
#include <aknapp.h>

// User includes

// Forward declarations

// Class declaration
/**
* @class CAdtUpdaterApplication
* @brief The CAknApplication inheriting class.
* @exe adtupdater.exe
*/
class CAdtUpdaterApplication : public CAknApplication
	{
	public:			// Functions from base classes
	
		/**
		* @brief From CAknApplication. Creates the document class.
		* @return CApaDocument* Pointer to document object.
		*/
		CApaDocument* CreateDocumentL();
		
		/**
	    * @brief From CApaApplication. Returns UID3.
	    * @return TUid Uid of the application.
	    */
	    TUid AppDllUid() const;
	};

#endif 		// __ADTUPDATER_APPLICATION_H__

// End of file
