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
* Description:   This is the header file for the CAlarmEditorAppUi class.
*
*/

#ifndef __ALARM_EDITOR_APPUI_H__
#define __ALARM_EDITOR_APPUI_H__

// System includes
#include <aknappui.h>

// User includes

// Forward declarations
class CClockAlarmEditor;

// Class declaration
/**
* @class CAlarmEditorAppUi
* @brief The CAknAppUi inheriting class.
* @exe alarmeditorapp.exe
*/
class CAlarmEditorAppUi : public CAknAppUi
	{
	public:			// Constructor and destructor

		/**
		* @brief C++ default constructor
		*/
	    CAlarmEditorAppUi();
		
		/**
		* @brief Destructor.
		*/
        ~CAlarmEditorAppUi();
        
	public:			// From base classes
	
		/**
		* @brief From CAknViewAppUi. Handles menu commands.
		* @param aCommandId The id of the command to be handled.
		*/
		void HandleCommandL( TInt aCommandId );
		
	private:		// New functions
	
		/**
		* @brief Symbian OS constructor. Performs the second phase of construction. May leave.
		*/
		void ConstructL();
		
	private:       // Data
	    
		/**
		* @var iAlarmEditor
		* @brief Object of alarm editor.
		*/
        CClockAlarmEditor*          iAlarmEditor;
	};

#endif 		// __ALARM_EDITOR_APPUI_H__

// End of file
