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
* Description:   This is the header file for the CAdtUpdaterAppUi class.
*
*/

#ifndef __ADTUPDATER_APPUI_H__
#define __ADTUPDATER_APPUI_H__

// System includes
#include <aknappui.h>

// User includes

// Forward declarations
class CAdtUpdaterContainer;

// Class declaration
/**
* @class CAdtUpdaterAppUi
* @brief The CAknAppUi inheriting class.
* @exe adtupdater.exe
*/
class CAdtUpdaterAppUi : public CAknAppUi
	{
	public: 		// Constructor and destructor

		/**
		* @brief C++ default constructor
		*/
		CAdtUpdaterAppUi();
		
        /**
        * @brief Destructor.
        */
        ~CAdtUpdaterAppUi();
        
	public:			// From base classes
	
		/**
		* @brief From CAknViewAppUi. Handles menu commands.
		* @param aCommandId The id of the command to be handled.
		*/
		void HandleCommandL( TInt aCommandId );
		
		/**
		* @brief From CAknAppUi. Handles when application gains or looses foreground.
		* @param aForeground EFalse when application is being sent to background.
		*/
		void HandleForegroundEventL( TBool aForeground );
		
	public:			// New functions
	
		/**
		* @brief Switches the application from background to foreground and vise versa.
		* @param aForeground ETrue if the application needs to be brought to the foreground.
		*/
		void ToggleAppViewL( TBool aForeground );
		
		/**
		* @brief Hides the status pane of the application.
		* @param aHide ETrue if status pane has to be hidden.
		*/
		void HideStatusPane( TBool aHide );
		
	private:		// New functions
	
		/**
		* @brief Symbian OS constructor. Performs the second phase of construction. May leave.
		*/
		void ConstructL();

	public:			// Data
	
		/**
		* @var iContainer
		* @brief The container object.
		*/
		CAdtUpdaterContainer*			iContainer;
	};

#endif 		// __ADTUPDATER_APPUI_H__

// End of file
