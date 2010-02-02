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
* Description:   Implements UI utils
*
*/

#ifndef CALENACTIONUIUTILS_H
#define CALENACTIONUIUTILS_H

// System includes
#include <badesca.h>
#include <calcommon.h>

// Forward declarations
class CCalEntryView;
class CCalEntry;
class CCalInstance;
class CCalInstanceView;
class TCalenInstanceId;

// Class declaration
/**
 * @class CalenActionUiUtils
 * @brief Implements UI utils found in the services API. We have to do this here
 * because both the controller and the services need these functions, but
 * the controller doesn't know about the services
 */
NONSHARABLE_CLASS( CalenActionUiUtils )
    {
    public:
	
		/**
		 * TDeleteConfirmationType
		 * The type of confirmation to display when deleting an entry
		 */
        enum TDeleteConfirmationType
            {
            EDeleteEntry = 1,
            EDeleteToDo,
            EDeleteToDos,
            EDeleteAll
            };

		/**
		 * TRepeatQueryType
		 * The type repeat query
		 */
        enum TRepeatQueryType
            {
            ESave = 1,
            EDelete,
            EEdit
            };

    public:
        
		/**
		 * @brief Show delete confirmation query
		 * @param aType Query type
		 * @param aCount Number of entries that need to be deleted
		 * @return TInt KErrNone if OK was pressed, or one of the system wide error codes
		 */
		IMPORT_C static TInt ShowDeleteConfirmationQueryL( const TDeleteConfirmationType aType,
														   const TInt aCount = 0 );
        
		/**
		 * @brief Shows a query when user is trying to edit, delete or save a repeating entry
		 * @param aAnswer Contains the user selected answer of type CalCommon::TRecurrenceRange
		 * @param aType Query is being displayed for saving, deleting or editing an entry
		 * @return TBool ETrue if OK was pressed, EFalse otherwise
		 */ 
		IMPORT_C static TBool ShowRepeatTypeQueryL( CalCommon::TRecurrenceRange& aAnswer,
													const TRepeatQueryType aType );

		/**
		 * @brief  Shows the list for selecting the type of the new entry to be created by the user
		 * @param aListItems The different types of entries that the user can create
		 * @param aListItemCommands Commands associated for creating the entry of each of the types
		 * @return TInt KErrNone if OK was pressed, or one of the system wide error codes
		 */
		IMPORT_C static TInt ShowNewNoteQueryL( CDesCArrayFlat*& aListItems,
												CArrayFixFlat <TInt>*& aListItemCommands );

		/**
		 * @brief Prompts the user to input a date. Requires a title resource id, or pass 0 for default title "Date:"
		 * @param aDate Date to be displayed. Will be updated with the user's choice once OK is pressed
		 * @param aPromptId Resource id of dialog title
		 * @return TInt KErrNone if OK was pressed, or one of the system wide error codes
		 */
		IMPORT_C static TInt DateQueryL( TTime& aDate, TInt aPromptId = 0 );

		/**
		 * @brief Marks a to-do entry as completed or restores a completed to-do and saves it to the entry view
		 * @param aEntryView CCalEntryView to save this instance to
		 * @param aEntry CCalEntry reference. Entry type must be CCalEntry::ETodo, else the function will leave
		 * @param aStatus Must be ETrue to set as completed. EFalse to restore completed to-do
		 */
        IMPORT_C static void SetToDoCompleteStatusL( CCalEntryView& aEntryView,
													 CCalEntry* aEntry,
													 const TBool aStatus );

		/**
		 * @brief Finds an instance with the given instance id and instance view
		 * @param aId Target instance id
		 * @param aInstanceView Reference to the instance view
		 * @return CCalInstance* The instance which has aId
		 */
		IMPORT_C static CCalInstance* FindPossibleInstanceL( const TCalenInstanceId& aId,
															 CCalInstanceView& aInstanceView );

    private: // New functions
        
		/**
		 * @brief Prepares the item list for NewEntry Query
		 * @param aListItems ListBox items for NewEntry query
		 * @param aListItemCommands Corresponding command for each listbox item
		 */
		static void PrepareLbDataForNewNotesQueryL( CDesCArrayFlat*& aListItems,
													CArrayFixFlat<TInt>*& aListItemCommands );

		/**
		 * @brief Default C++ constructor
		 * Hidden since it is a static utility class
		 */
		CalenActionUiUtils();

		/**
		 * @brief Prevent assignment of the objects
		 */
		CalenActionUiUtils& operator=( const CalenActionUiUtils& );
    };

#endif // CALENACTIONUIUTILS_H

// End of file
