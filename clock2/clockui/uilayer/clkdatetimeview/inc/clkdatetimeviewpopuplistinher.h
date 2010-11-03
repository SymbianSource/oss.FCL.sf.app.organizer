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
* Description:   This is the header file for the CClkDateTimeViewPopupListInher class.
*
*/

#ifndef __CLK_DATETIMEVIEW_POPUPLIST_INHER_H__
#define __CLK_DATETIMEVIEW_POPUPLIST_INHER_H__

// System includes
#include <aknPopup.h>
#include <aknlists.h>
#include <eiklbx.h>

// User includes

// Forward declarations

// Class declaration
/**
* @class CClkDateTimeViewPopupListInher
* @brief The popup list class.
* @dll clkdatetimeview.dll
*/
NONSHARABLE_CLASS( CClkDateTimeViewPopupListInher ) : public CAknPopupList, 
													  public MListBoxItemChangeObserver 
	{
	public:				// Constructor and destructor
	
		/**
		* @brief Two-phased constructor.
		* @param aListBox Pre-existing listbox-derived class.
		* @param aCbaResource Softkey pane to display while pop-up is active.
		* @param aType The type of popup.
		* @return CClkDateTimeViewPopupListInher* Pointer to the constructe object.
		*/
		static CClkDateTimeViewPopupListInher* NewL( CAknSinglePopupMenuStyleListBox* aListBox,
													 TInt aCbaResource,
													 AknPopupLayouts::TAknPopupLayouts aType );
	
		/**
		* @brief Destructor
		*/	
		~CClkDateTimeViewPopupListInher();
		
	public:				// Functions from base classes	
									 
		/**
		* @brief From CCoeControl, handle activation of control.
		*/						
		void ActivateL();

		/**
		* @brief From MListBoxItemChangeObserver, handle enable/disable LSK.
		* @param aListBox The listbox in context.
		*/							
		void ListBoxItemsChanged( CEikListBox* aListBox ); 
										
	private:			// New functions
		
		/**
		* @brief C++ default constructor.
		*/
		CClkDateTimeViewPopupListInher();

		/**
		* @brief Performs the 2nd phase of two-phase construction.
		* @param aListBox Pre-existing listbox-derived class.
		* @param aCbaResource Softkey pane to display while pop-up is active.
		* @param aType The type of popup.
		*/
		void ConstructL( CAknSinglePopupMenuStyleListBox* aListBox, 
						 TInt aCbaResource,
						 AknPopupLayouts::TAknPopupLayouts aType );

	};

#endif 			// __CLK_DATETIMEVIEW_POPUPLIST_INHER_H__

// End of file
