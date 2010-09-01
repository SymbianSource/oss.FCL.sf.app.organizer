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
* Description:    Active class to reteive the coordiantes for current location
 *
*/


#ifndef __LOCATIONACQUISITION_H__
#define __LOCATIONACQUISITION_H__

// INCLUDES
#include "calencurrentlocationobserver.h"
#include <lbs.h>

// FORWARD DECLARATIONS
class RPositionServer;
class RPositioner;
class TPosition;
class TPositionInfo;

// CLASS DECLARATION
/**
 * The asynchronous operation handler class to retrieve the current location
 */
class CCalenCurrentLocationAcquisition : public CActive
    {
    public:
    	
    	/**
         * Symbian 1st phase construction
         * @param aObserver Reference of the observer that requests for current location
         */
    	static CCalenCurrentLocationAcquisition* NewL( MCalenCurrentLocationObserver& aObserver );
    	
    	/**
         * C++ constructor
         * @param aObserver Observer to callback once user selects the location
         */
        CCalenCurrentLocationAcquisition( MCalenCurrentLocationObserver& aObserver );
       
        /**
         * Destructor
         */
        ~CCalenCurrentLocationAcquisition();

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
        * Requests for current location asynchronously
        */
        void RequestL();
        
        /**
        * Cancels the ongoing request
        */
        void CancelRequest();
        
        /**
        * Function to return current position details
        */
        TPosition& CurrentPosition();
        
	private:
		
		 /**
         * Symbian 2nd phase constructor
         */
		 void ConstructL();
		 
    private:
        MCalenCurrentLocationObserver&   iObserver;
  
        RPositionServer iServer;
        
        RPositioner iPositioner;

        TPositionInfo iPositionInfo; 
         
        TPosition iPosition;
    };
#endif /* __LOCATIONACQUISITION_H__ */
