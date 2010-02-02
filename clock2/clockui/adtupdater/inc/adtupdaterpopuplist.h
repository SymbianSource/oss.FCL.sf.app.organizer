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
* Description:   This is the header file for the CAdtUpdaterPopupList class.
*
*/

#ifndef __ADTUPDATER_POPUPLIST_H__
#define __ADTUPDATER_POPUPLIST_H__

// System includes
#include <aknPopup.h>

// User includes

// Forward declarations
class CAknPopupList;
class MListBoxItemChangeObserver;
class CAknSinglePopupMenuStyleListBox;
class CEikListBox;

// Class declaration
/** 
* @class CAdtUpdaterPopupList
* @brief An instance of this is used to display popup lists.
* @exe adtupdater.exe
*/
class CAdtUpdaterPopupList : public CAknPopupList,
									MListBoxItemChangeObserver 
    {
	public:			// Constructor and destructor
	
		/**
        * @brief Create a CAdtUpdaterPopupList object. Performs the first phase of 2-phase construction.
        * @param aListBox Pre-existing listbox-derived class.
        * @param aCbaResource Softkey pane to display while pop-up is active.
        * @param aType Popup Layout type.
        * @return CAdtUpdaterPopupList*
        */
        static CAdtUpdaterPopupList* NewL( CAknSinglePopupMenuStyleListBox* aListBox,
        								   TInt aCbaResource,
        								   AknPopupLayouts::TAknPopupLayouts aType );
		/**
		* @brief Destructor.
		*/
	    ~CAdtUpdaterPopupList();
	    
	public:			// Functions from base classes
	
		/**
		* @brief From CCoeControl. Handle activation of control.
		* @see CCoeControl
        */
        void ActivateL();

        /**
        * @brief From MListBoxItemChangeObserver. Handle enable/disable LSK.
        * @see MListBoxItemChangeObserver
        */                            
        void ListBoxItemsChanged( CEikListBox* aListBox );
	    
	private:		// New functions
        
        /**
		* @brief C++ default constructor. 
		*/
	    CAdtUpdaterPopupList();

        /**
	    * @brief Perform the second phase construction of a CAdtUpdaterPopupList object.
	    * @param aListBox Pre-existing listbox-derived class
        * @param aCbaResource Softkey pane to display while pop-up is active
        * @param aType Popup Layout type
	    */
        void ConstructL( CAknSinglePopupMenuStyleListBox* aListBox,
        				 TInt aCbaResource,
        				 AknPopupLayouts::TAknPopupLayouts aType );
	};


#endif 		// __ADTUPDATER_POPUPLIST_H__

// End of file


