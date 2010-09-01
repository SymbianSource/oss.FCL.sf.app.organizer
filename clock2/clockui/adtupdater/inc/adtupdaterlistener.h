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
* Description:   This is the header file for the CAdtUpdaterListener class.
*
*/

#ifndef __ADTUPDATER_LISTENER_H__
#define __ADTUPDATER_LISTENER_H__

// System includes

// User includes
#include "clockserverclt.h"

// Forward declarations
class CAdtUpdaterContainer;

// Class declaration
/**
* @class CAdtUpdaterListener
* @brief The active object class which listens for Nitz changes
* @exe adtupdater.exe
*/
class CAdtUpdaterListener : public CActive
	{
	public:			// Constructor and destructor
	
		/**
		* @brief Create a CAdtUpdaterListener object. Performs the first phase of 2-phase construction.
		* @param aObserver Of type CAdtUpdaterContainer 
		* @return CAdtUpdaterListener* A pointer to the created instance of CAdtUpdaterListener
		*/
        static CAdtUpdaterListener* NewL( CAdtUpdaterContainer* aObserver );
        
		/**
		* @brief The destructor.
		*/
	    ~CAdtUpdaterListener();
	    
	public:			// New functions
		
		/**
        * @brief Subscribes for notfication when Nitz is available.
        */     
		void WaitForNitzInfoL();
		
		/**
		* @brief Checks with clockserver one final time for availability of time information.
		*/
		TBool ConfirmDataAvailabilityL();
		
		/**
		* @brief Notifies the observer with the time information.
		*/
		void NitzInfoAvailableL();
		
		/**
		* @brief Provides the current Mcc value.
		*/	
		TInt GetCurrentMcc();
		
	private:		// From base classes
		
		/**
		* @brief From CActive. Called when the value of iStatus changes.
		*/
		void RunL();
		
		/**
		* @discussion From CActive. Called when Cancel is called on the AO.
		*/
		void DoCancel();
	    
	private:		// New functions     
	        
        /**
        * @brief By default Symbian OS constructor is private. Performs the 2nd phase of construction.
        */
        void ConstructL();
        
        /**
        * @brief C++ default constructor.
        */
        CAdtUpdaterListener( CAdtUpdaterContainer* aObserver );
        
    private:		// Data
	 	
	 	/** 
		* @var iObserver
		* @brief CADTUpdaterContainer* observer.
		*/
	 	CAdtUpdaterContainer*				iObserver;
	 	
	 	/**
	 	* @var iClkSrvIf
	 	* @brief Handle to clockserver session.
	 	*/
	 	RClkSrvInterface					iClkSrvIf;
	 	
	 	/**
	 	* @var iInfoAvailable
	 	* @brief Indicates if Nitz info is available or not.
	 	*/
	 	TBool								iInfoAvailable;
	 	
	 	/**
	 	* @var iWhatChanged
	 	* @brief Filled by clockserver, indicates what component was changed.
	 	*/
	 	TInt32								iWhatChanged;
	 	
	 	/**
	 	* @var iWhoChanged
	 	* @brief Filled by clockserver, indicates who changed a component.
	 	*/
	 	TInt32								iWhoChanged;
	 	
	 	/**
	 	* @var iTimeAvailable
	 	* @brief Will be set to ETrue when NITZ information is available.
	 	*/
	 	TBool								iTimeAvailable;
	 	
	 	/**
	 	* @var iMccAvailable
	 	* @brief Will be set to ETrue when timezone information is available.
	 	*/
	 	TBool								iMccAvailable;
	 	
	 	/**
	 	* @var iTimeAttributes
	 	* @brief The time information received from the protocols.
	 	*/
	 	STimeAttributes						iTimeAttributes;
	 	
	 	/**
	 	* @var iMcc
	 	* @brief The mobile country code, received from clockserver.
	 	*/
	 	TInt								iCurrentMcc;
	 	
	 	/**
	 	* @var iTimeZoneId
	 	* @brief The timezone id, received from clockserver.
	 	*/
	 	TInt								iTimeZoneId;
	};

#endif 			// __ADTUPDATER_LISTENER_H__

// End of file
