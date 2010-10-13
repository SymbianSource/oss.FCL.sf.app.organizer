/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Action class for handling location related commands
 *
*/



#ifndef __LOCATIONUI_H__
#define __LOCATIONUI_H__

// INCLUDES
#include <e32base.h>
#include <AknLaunchAppService.h>
#include <AknServerApp.h>				// MAknServerAppExitObserver
#include <mnprovider.h>
#include <calencommandhandler.h>
#include <calennotificationhandler.h>   // MCalenNotificationHandler

#include "calenselectionobserver.h"
#include "calencurrentlocationobserver.h"
#include "calencurrentlocationacquisition.h"

// FORWARD DECLARATIONS
class CCalenController;
class CMnMapView;
class CCalenLocationActiveSelector;
class CCalenGlobalData;
class CPosLandmark;
class TPosition;

// CLASS DECLARATION

/**
 * CCalenLocationUi encapsulates location/Map related behavior in Calendar application.
 *
 */
NONSHARABLE_CLASS( CCalenLocationUi ) : public CBase,
	                                    public MCalenNotificationHandler,
	                                    public MCalenCommandHandler,
	                                    public MAknServerAppExitObserver,
	                                    public MCalenSelectionObserver,
	                                    public MCalenCurrentLocationObserver
    {
    public: // public API
        /**
         * Symbian 1st phase construction
         * @param aController Reference of the controller that owns the editui
         */
        IMPORT_C static CCalenLocationUi* NewL( CCalenController& aController );
        
        /**
         * Destructor
         */
        virtual ~CCalenLocationUi();
        
        void MapClosed();

    public:  // MCalenCommandHandler

        /**
         * From MCalenCommandHandler.
         * Handles key presses.
         * 
         * @param aCommand The command Id of the key.
         */
        TBool HandleCommandL( const TCalenCommand& aCommand );

        /**
         * From MCalenCommandHandler.
         * Allows extending this API without breaking BC.
         * 
         * @param aExtensionUid specifies
         * @return extension of the requested type
         */
        TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid );
        
    public:  // from MCalenNotificationHandler
        /** 
         * Handle notifications
         * @param aNotification Nofication to be handled
         */
        void HandleNotification(const TCalenNotification aNotification );
        
    public:  // from MAknServerAppExitObserver
      	/**
      	* Handle the exit of a connected server app.
      	* @param aReason The reason that the server application exited.
	    * This will either be an error code, or the command id that caused
	    * the server app to exit.
	    */
        void HandleServerAppExit(TInt aReason);
    
    public:		// From MCalenSelectionObserver
    	/**
    	* Handles user selection for a location on the map.
    	* @param aMapView  Map View object through which map is launched
    	*		 aError    Error code if any generated from the maps
    	*/
    	void HandleSelectionCompletedL( CMnMapView& aMapView, TInt aError );
    
    public:		// From MCalenCurrentLocationObserver
    	/**
    	* Callback function that gets called when current locaiton
    	* async request to get current location is complete
    	*/
    	void NotifyCurrentLocationL( );
    	
    	/**
    	* Callback function that gets called if there is any error
    	* while fetching coordinates for current location
    	* @param errorCode	Error code 
    	*/
    	void NotifyErrorL(const TInt errorCode);
    		
    private: // own methods
        /**
         * C++ constructor
         * @param aController Reference to the controller that owns the caleneditui
         */
        CCalenLocationUi( CCalenController& aController );
        
        /**
         * Symbian 2nd phase constructor
         */
        void ConstructL();
        
        /**
        * Function to make request for current locaiton coordinates
        */
        void RequestCurrenLocationL();
        
        /**
        * Issues asynchronous request to launch the map with coords of current location 
        * for user selection. It uses LocationSelector active object to listen to the 
        * user selection.
        * @param aPosition	Current position
        */
        void LaunchMapL(TPosition& aPosition);
        
        /**
        * Issues asynchronous request to launch the map with default coords for user selection
        * It uses LocationSelector active object to listen to the user selection
        */
        void LaunchMapL();
        
        /**
        * Issues synchronous request to show a particular location on the map
        */
        void ShowLocationOnMapL();
        
        /**
        * Returns the currently available map providers in the system
        * @param aServicesNeeded  List of services needed from the Map provider
        * @return Map Provider object
        */
        CMnProvider* GetFirstProviderL( CMnProvider::TServices aServicesNeeded );
        
        /**
        * Function to query user whether to replace or keep existing location
        * @param aLocaitonName  Location name to be prompted to the user
        */
        TInt ShowLocationReplaceNoticeL(TPtrC aLocaitonName);
        
        /**
        * Stores the locaiton details of currently focusses entry in Day/Event Viewer 
        * into Agenda Server
        * @param landmark  LandMark object
        */
        void StoreLocationInfoL(CPosLandmark* landmark);
        
        /**
        * Function to query user whether to append or replace the exisitng location details
        * @return User response
        */
        TInt ShowLocationAppendOrReplaceL();
        
        /**
        * Displays "Address Updated" transient notice to the user
        * @return None
        **/
        void ShowAddressUpdatedNoticeL();
        
		/**
		* Function to query user to define a location as location field is empty when "Show locaiton"
		* hard key is pressed.
		* @return 	User response
		**/
        TInt ShowDefineLocationQuery();
        
        
    private: // data
    	CCalenController&  iController;
    	CCalenGlobalData* iGlobalData;
    	CMnProvider* iProvider;
    	CMnMapView*  iMapView;
    	CCalenLocationActiveSelector* iLocationSelector;
    	CCalenCurrentLocationAcquisition* iCurrLocAcquisition;
    	TBool isRequestPending;	
    	TBool isGetLocationAndSave; // When requested from Views
    	TBool isReplaceLocation; // When requested from Views
    	TPosition iCurrPosition;
    	TBool isCurrLocAvailable;
    	HBufC* iLocationText;
    };

#endif /* __LOCATIONUI_H__ */