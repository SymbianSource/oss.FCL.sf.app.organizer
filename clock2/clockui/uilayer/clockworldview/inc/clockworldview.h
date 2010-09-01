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
* Description:   This is the header file for the CClockWorldView class.
*
*/

#ifndef __CLOCK_WORLD_VIEW_H__
#define __CLOCK_WORLD_VIEW_H__

// System includes
#include <aknview.h>
#include <MMGFetchCanceler.h>           // MMGFetchCanceler

// User includes
#include "clkcommon.h"

// Constants
const TInt KCityCountryLength( 64 );
const TInt KImagePathLength( 256 );
const TInt KOnlyHomeCity( 1 );

// Forward declarations
class CClockWorldContainer;
class CClockWorldArray;
class RLibrary;
class CClockWorldItemProcessor;
class CClockAppUi;

// Class declaration
/**
* @class TCityInfo
* @brief Encapsulates information about the timezone.
* @exe clock.exe
*/
class TCityInfo
	{
	public:			// Data
	
		/**
		* @var iCity
		* @brief 
		*/
		TBuf< KCityCountryLength >		iCity;
	
		/**
		* @var iCountry
		* @brief 
		*/
		TBuf< KCityCountryLength >		iCountry;
	
		/**
		* @var iUtcOffset
		* @brief 
		*/
		TInt							iUtcOffset;
	
		/**
		* @var iTimeZoneId
		* @brief 
		*/
		TInt							iTimeZoneId;
	
		/**
		* @var iCityOffsetInGroup
		* @brief 
		*/
		TInt							iCityOffsetInGroup;
	
		/**
		* @var iCityGroupId
		* @brief 
		*/
		TInt							iCityGroupId;

		/**
		* @var iImagePath
		* @brief 
		*/
	    TBuf< KImagePathLength >        iImagePath;
        
	    /**
        * @var iThumbnailBitmap
        * @brief 
        */
	    CFbsBitmap*                     iThumbnailBitmap;
	    
	    /**
        * @var iThumbnailRequestId
        * @brief 
        */
	    TInt                            iThumbnailRequestId;
	};

/**
* @class CClockWorldView
* @brief The CAknView inheriting class.
* @exe clock.exe
*/
class CClockWorldView : public CAknView                        
	{
	public:        // Constructor and destructor
			
		/**
		* @brief Create a CClockWorldView object. Performs the first phase of 2-phase construction.
		* @return CClockWorldView* A pointer to the created instance of CClockWorldView
		*/
		static CClockWorldView* NewL();
		
		/**
		* @brief Destructor
		*/
		~CClockWorldView();
		
	public:        // From base classes
	
		/**
		* @brief From CAknView. Returns this view Id.
		* @return TUid The id of the view.
		*/
	    TUid Id() const;

		/**
		* @param aCommand The id of the command to be handled.
		* @brief From MEikCommandObserver. Handles a user command relating to this view.
		*/
	    void HandleCommandL( TInt aCommand );
    
		/**
		* @brief From MEikMenuObserver. Dynamically initializes the menu items.
		* @param aResourceId The resource which triggered the action.
		* @param aMenuPane Pointer to the menupane of type CEikMenuPane.
		*/
	    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
	    
		/**
		* @brief From MEikMenuObserver. Dynamically initializes the menubar.
		* @param aResourceId The resource which triggered the action.
		* @param aMenuBar Pointer to the menupane of type CEikMenuBar.
		*/
	    void DynInitMenuBarL( TInt aResourceId, CEikMenuBar* aMenuBar );
	    
	    /**        
	    * @brief From CEikAppUi.
	    *        Called when the application switches from foreground to background and vice versa.
	    * @param aForeground ETrue if foreground, EFalse if sent to background.
	    */
	    void HandleForegroundEventL( TBool aForeground );
	    
	public:        // New functions
	    
		/**
		* @brief Returns the application rect to the container.
		* @return TRect The application rect.
		*/
	    TRect GetApplicationRect();
	   
	    
		/**
		* @brief Handles the environment changes. Called from CClockAppUi.
		* @param aEnvChanged Of type TClockEnvChanged.
		*/
        void HandleEnvChangeL( TClockEnvChanged aEnvChanged );
        
        /**
  	    * @brief Returns ETrue if the selection is currently open.
  	    * @return TBool 
  	    */
        TBool IsSelectionListOpen();
        
        /**
  	    * @brief Returns ETrue if the gallery is currently open.
  	    * @return TBool 
  	    */
        TBool IsGalleryOpen();    
        
        /**
		* @brief Callback every interval time.
		* @param aThis This class.
		* @return TInt ETRUE or EFALSE
		*/
        static TInt TimerCallback( TAny* aThis );
        
        /**
        * @brief Returns the application's appui.
        * @return CClockAppUi The pointer to the clock application ui.
        */
        CClockAppUi* ClockApplicationUi();

	protected:     // From base classes
	
		/**
		* @discussion From CAknView. Activates this view.
		* @see CAknView.
		*/
		void DoActivateL( const TVwsViewId& aPrevViewId,
	    				  TUid aCustomMessageId,
	    				  const TDesC8& aCustomMessage );

		/**
		* @brief From CAknView. Deactivates this view.
		*/
	    void DoDeactivate();
	
	private:       // New functions
	
		/**
		* @brief Performs the second phase of 2-phase construction. May leave.
		*/
        void ConstructL();
        
        /**
		* @brief C++ default constructor
		*/
        CClockWorldView();

		/**
		* @brief Adds a new location to the list.
		*/
		void AddLocationL();

		/**
		* @brief Removes a location from the list.
		*/
		void RemoveLocationL();

		/**
		* @brief Sets the currently selected item as the home zone.
		*/
		void SetHomeLocationL();
		
		/**
		* @brief Launches the Gallery and 
		* Sets the currently selected image for the focussed list item.
		*/
		void AddImageL();
            
        /**
        * Stops the periodic timer.
        */
        void StopTimerL();
    
        /**
        * Restartst the periodic timer.
        */
        void RestartTimerL();
 
	private:		// Data
	
		/**
		* @var iContainer
		* @brief The container object.
		*/
		CClockWorldContainer*				iContainer;
		
		/**
		* @var iWorldArray
		* @brief The MDesCArray implementing object, acting as the data provider for the world country/city list.
		*/
		CClockWorldArray*					iWorldArray;
		
		/**
		* @var iMaxCitiesAdded
		* @brief
		*/
		HBufC*								iMaxCitiesAdded;

	

		/**
		* @var iClockCitySelector
		* @brief The library handle for the MClockCitySelectionList.
		*/
		RLibrary							iClockCitySelector;

		/**
		* @var iSelectionListAvailable
		* @brief ETrue if the selection list is available.
		*/
		TBool								iSelectionListAvailable;

		/**
		* @var iSelectionListOpen
		* @brief ETrue if the selection is in use currently.
		*/
		TBool								iSelectionListOpen;

		/**
		* @var iTimer
		* @brief The periodic timer to keep the ui time updated.
		*/
        CPeriodic*                          iTimer;
        
		/**
		* @var iItemProcessor
		* @brief CClockWorldItemProcessor object
		*/
        CClockWorldItemProcessor*           iItemProcessor;
        
        /**
		* @var iGalleryOpen
		* @brief ETrue if the gallery is in use currently.
		*/
		TBool								iGalleryOpen;
		
		/**
	    * @var iItemToFocus
	    * @brief The item to be focused when the view is activated.
	    */
		TInt                                iItemToFocus;
		
		/**
		* @var iCanceler
		* @brief MMGFetchCanceler object to close the gallary.
		*/
		MMGFetchCanceler*                   iCanceler;
	};

#endif 		// __CLOCK_WORLD_VIEW_H__

// End of file
