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
* Description:    Active class to launch the map and listen for user selection
 *
*/


#ifndef __LOCATIONACTIVESELECTOR_H__
#define __LOCATIONACTIVESELECTOR_H__

// INCLUDES
#include "calenselectionobserver.h"
#include <mnprovider.h>
#include <AknServerApp.h>				// MAknServerAppExitObserver

// FORWARD DECLARATIONS
class CMnMapView;
class TPosition;
class CPosLandmark;
class CCalenGlobalData;

// CLASS DECLARATION
/**
 * The asynchronous operation handler class to launch the map
 *
 */
class CCalenLocationActiveSelector : public CActive
    {
    public:
    	/**
         * Symbian 1st phase construction
         * @param aObserver Observer to callback once user selects the location
         */
    	static CCalenLocationActiveSelector* NewL(MCalenSelectionObserver& aObserver,
    												MAknServerAppExitObserver& aExitObserver,
    												CCalenGlobalData& aGlobalData);
    	
        /**
         * Destructor
         */
        ~CCalenLocationActiveSelector();

        // CActive-related methods
        /**
        * RunL method to handle the user selection
        */
        void RunL();
        
        /**
        * Cancel method to handle the user selection
        */
        void DoCancel();
        
        /**
        * Function to handle any errors in async request
        * @param aError   Error Code
        */
        TInt RunError( TInt aError );
		
		/**
        * Initiates selection operation
        * @param aProvider  Map Provider
        * 		 aPosition  Current position
        */
        void SelectL( CMnProvider& aProvider, 
        			  TPosition& aPosition,
        			  TBool isLaunchedFromEditors);
		
		/**
        * Initiates selection operation
        * @param aProvider  Map Provider
        */
		void SelectL( CMnProvider& aProvider,
		                CPosLandmark* aLandmarkToShow);
		
		/**
        * Disconnects from provider, when operation is completed
        */
        void Reset();
       
        /**
        * Reads all the landmarks from all the available databases in the system
        */
        void RetrieveAndAddLandMarksL(CMnMapView* aMapView,
                                        TBool isLaunchedFromEditors);
        
	private:
		 /**
         * C++ constructor
         * @param aObserver Observer to callback once user selects the location
         */
         CCalenLocationActiveSelector( MCalenSelectionObserver& aObserver,
         								MAknServerAppExitObserver& aExitObserver,
         								CCalenGlobalData& aGlobalData );
         
         /**
         * Symbian 2nd phase constructor
         */
         void ConstructL();
        
    private:
        MCalenSelectionObserver&   iObserver;
        MAknServerAppExitObserver& iExitObserver;
        CMnMapView*           iMapView;
        CCalenGlobalData& iGlobalData;
    };

#endif /* __LOCATIONACTIVESELECTOR_H__ */
