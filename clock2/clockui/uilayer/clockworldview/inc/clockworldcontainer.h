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
* Description:   This is the header file for the CClockWorldContainer class.
*
*/

#ifndef __CLOCK_WORLD_CONTAINER_H__
#define __CLOCK_WORLD_CONTAINER_H__

// System includes
#include <coecntrl.h>
#include <eiklbo.h>

// User includes
#include "clkcommon.h"

// Forward declarations
class CClockWorldView;
class CClockWorldArray;
class CAknsBasicBackgroundControlContext;
class CCoeEnv;
class CAknDoubleLargeStyleListBox;

// Constants

// Class declaration
/**
* @class CClockWorldContainer
* @brief The CCoeControl inheriting class.
* @exe clock.exe
*/
class CClockWorldContainer : public CCoeControl,
                             public MEikListBoxObserver
	{
	public:			// Constructor and destructor
			
		/**
		* @brief Creates a CClockWorldContainer object. Performs the first phase of 2-phase construction.
		* @param aView The CClockWorldView object.
		* @param aRect The main rect.
		* @param aWorldArray The CClockWorldArray object.
		* @return CClockWorldContainer* A pointer to the created instance of CClockWorldContainer
		*/
		static CClockWorldContainer* NewL( CClockWorldView* aView, const TRect& aRect, CClockWorldArray* aWorldArray );
		
		/**
		* @brief Destructor
		*/
		~CClockWorldContainer();
		
	public: 		// Functions from base classes

        /**
		* @brief From CCoeControl.
		* @see CCoeControl
		*/
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
        							 TEventCode aType );

		/**  
		* @brief From CCoeControl. Draw this CClockWorldContainer to the screen.
		* @param aRect the rectangle of this view that needs updating
		*/
    	void Draw( const TRect& aRect ) const;
    	
    	/**
    	* @brief From CCoeControl.
    	* @param aDrawNow.
    	* @see CCoeControl.
    	*/
    	void FocusChanged( TDrawNow aDrawNow );
    	
    	/**
    	* @brief From CCoeControl.
    	* @param aPointerEvent.
    	* @see CCoeControl.
    	*/
    	void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    	
    	/**
        * @brief Handles list box events.
        * This pure virtual function is invoked by CEikListBox to
        * notify the observer of list box events.
        * @param aListBox   The originating list box.
        * @param aEventType A code for the event. Further information 
        * may be obtained by accessing the list box itself.
        */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

	public:			// New functions

		/**
    	* @brief Returns the listbox.
		* @return CAknDoubleLargeStyleListBox* Pointer to the location listbox.
		*/
		CAknDoubleLargeStyleListBox* ListBox();
		
		/**
        * @brief Loads the thumbnail and updates the iconArray for the selected list item.
        * @param aFileName The absolute path of the image file.
        * @param aIndex Index of the currently selected listitem.
        */
        void UpdateImageL( TPtrC aFileName, TInt aIndex );
        
        /**
        * @brief Redraws  the input list item
        * @param aIndex The index of the list item.
        * @return ETrue when aIndex is less than the listbox count.
        * EFalse when aIndex is greater than the listbox count.
        */
        TBool RedrawItem( TInt aIndex );
        
        /**
        * @brief Removes an image when it is not loaded in any of the list items.
        * @param aFileName The absolute path of the image file.
        * @param aImageIndex Index of the image in the iconArray
        * @param aListIndex Index of the listitem.
        */
        void RemoveImage( TPtrC aFileName, TInt aImageIndex, TInt aListIndex );
        
        /**
        * @brief Loads thumbnail for a list item.
        * @param aBitmap The thumbnail bitmap to be loaded.
        * @param aIndex List item Index.
        */                
        void LoadThumbnailL( CFbsBitmap* aBitmap, TInt aIndex );
		
		/**
        * Refreshes container.
        */
        void Refresh();
        
   	private:    	// Functions from base classes

        /**
		* @brief From CCoeControl. Handles a chage of client area size.
		*/
        void SizeChanged();
        
        /**
		* @brief From CCoeControl. Returns the number of components.
		* @return TInt The number of component controls.
		*/
        TInt CountComponentControls() const;

        /**
		* @brief From CCoeControl. Returns a pointer of component.
		* @return CCoeControl* Component control.
		*/
        CCoeControl* ComponentControl( TInt aIndex ) const;
        
        /**  
		* @brief From CCoeControl. Returns the TTypeUid::Ptr, used to set the background context.
		* @param aId
		* @return TTypeUid::Ptr
		* @see CCoeControl
		*/
        TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
        
        /**
        * @brief From CCoeControl. Handles the chane in the control's resource.
        * @param aType The type of change.
        * @see CCoeControl.
        */
        void HandleResourceChange( TInt aType );
        
        /**
        * From CCoeControl Gets help context
        * @param aContext Help context
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
        
	private:		// New functions     
        
        /**
		* @brief By default Symbian OS constructor is private.
		* @param aView The CClockWorldView object.
		* @param aRect of this container.
		* @param aWorldArray The CClockWorldArray object.
		*/
        void ConstructL( CClockWorldView* aView, const TRect& aRect, CClockWorldArray* aWorldArray );
        
        /**
		* @brief C++ default constructor.
		*/
        CClockWorldContainer();
                
	private:       // Data
	    
	    /**
		* @var iView
		* @brief The CClockWorldView object.
		*/
	    CClockWorldView*                            iView;

		/**
	    * @var iListBox
		* @brief A listbox containing the cities selected by the user.
		*/
		CAknDoubleLargeStyleListBox*				iListBox;

	    /**
		* @var iBgContext
		* @brief The basic skin bg control context.
		*/
	    CAknsBasicBackgroundControlContext*			iBgContext;
		
		/**
		* @var iTouchFlag
		* @brief Will be ETrue if a control has been tapped.
		*/
		TBool										iTouchFlag;
				        
        /**
		* @var iWorldArray
		* @brief The CClockWorldArray object
		*/
        CClockWorldArray*                           iWorldArray;
        
        /**
        * @var iOldImagePath
        * @brief The image path.
        */
        HBufC*                                      iOldImagePath;

	};

#endif 		// __CLOCK_WORLD_CONTAINER_H__

// End of file
