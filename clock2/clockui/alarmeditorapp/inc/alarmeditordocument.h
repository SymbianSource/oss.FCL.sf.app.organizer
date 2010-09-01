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
* Description:   This is the header file for the CAlarmEditorDocument class.
*
*/

#ifndef __ALARM_EDITOR_DOCUMENT_H__
#define __ALARM_EDITOR_DOCUMENT_H__

// System includes
#include <AknDoc.h>
#include <eikapp.h>

// Class declaration
/**
* @class CAlarmEditorDocument
* @brief The CAknDocument inheriting class.
* @exe alarmeditorapp.exe
*/
class CAlarmEditorDocument : public CAknDocument
	{
	public:        // Constructor and destructor

        /**
        * @brief Performs the first phase of 2-phase construction.
        * @param aApp CEikApplication&
        * @return CAlarmEditorDocument* Pointer to the object of the document
        */
        static CAlarmEditorDocument* NewL( CEikApplication& aApp );

        /**
        * @brief Destructor.
        */
        ~CAlarmEditorDocument();
              
	private:       // New functions
	 
        /**
        * @brief C++ default Constructor.
        * @param aApp CEikApplication&
        */
	 	CAlarmEditorDocument( CEikApplication& aApp );

        /**
        * @brief Performs the second phase of 2-phase construction. May leave.
        */
        void ConstructL();
                       
	private:      // Functions from base classes.

		/**
		* @brief From CEikCocument. Creates application UI class.
		*/
		CEikAppUi* CreateAppUiL();
	};

#endif 		// __ALARM_EDITOR_DOCUMENT_H__

// End of file
