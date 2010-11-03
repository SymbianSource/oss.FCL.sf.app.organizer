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
* Description:  This is the header file for the CClockAlarmEditor class.
*
*/

#ifndef __CLOCK_ALARMEDITOR_H__
#define __CLOCK_ALARMEDITOR_H__

// System includes
#include <asshddefs.h>

// Forward Declarations
class CClockAlarmEditorImpl;

// Class declaration
/**
* @class CClockAlarmEditor
* @brief The object of this class is used to accept the alarm information from the user.
* @dll clockalarmeditor.dll
* 
* Usage for setting new alarm: 
*       CClockAlarmEditor* alarmEditor = CClockAlarmEditor::NewL();
*       TInt alarmId( alarmEditor->ExecuteLD() );
*       delete alarmEditor;
* 
* Usage for editing an existing alarm: 
*       CClockAlarmEditor* alarmEditor = CClockAlarmEditor::NewL( existingAlarmID );
*       TInt newlySetAlarmId( alarmEditor->ExecuteLD() );
*       delete alarmEditor;
*/
class CClockAlarmEditor : public CBase
	{
    public:			// Constructor and destructor
        
		/**
		* @brief 2-phased constructor. If the alarm id passed is invalid, the behviour would
		* 		 be same as setting a new alarm. 
		* @param aExistingAlarmId The id of the alarm the client wishes to edit.
		* @return CClockAlarmEditor* Pointer to the constructed CClockAlarmEditor object.
		*/
		IMPORT_C static CClockAlarmEditor* NewL( TAlarmId aExistingAlarmId = 0 );
		
		/**
		* @brief Destructor
		*/
		IMPORT_C ~CClockAlarmEditor();
		
	public:			// New functions
		
		/**
		* @brief Calls the form's ExecuteLD to launch the editor.
		* @return TInt 0 if user pressed cancel OR
		*         The newly set alarm ID if alarm was created OR
		*         One of the system wide error codes if alarm creation failed. 
		*/
        IMPORT_C TInt LaunchL();
  
    private:		// New functions
        
		/**
		* @brief Performs the 2nd phase of the 2-phase construction.
		* @param aExistingAlarmId The id of the alarm the client wishes to edit.
		*/
        void ConstructL( TAlarmId aExistingAlarmId );
        
		/**
		* @brief C++ default constructor.
		*/
		CClockAlarmEditor();
	
    private:		// Data
        
		/**
		* @var iClockAlarmEditorImpl
		* @brief Pointer to the alarm editor implementation class. 
		*/
		CClockAlarmEditorImpl*		iClockAlarmEditorImpl;
		
		/**
		* @var iAlarmId
		* @brief Retains the alarmid which can be used by the client.   
		*/
		TAlarmId					iAlarmId;
		
	};

#endif 			// __CLOCK_ALARMEDITOR_H__

// End of file
