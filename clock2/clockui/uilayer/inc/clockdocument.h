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
* Description:   This is the header file for the CClockDocument class.
*
*/

#ifndef __CLOCK_DOCUMENT_H__
#define __CLOCK_DOCUMENT_H__

// System includes
#include <AknDoc.h>
#include <eikapp.h>
#include <s32file.h>
#include <tz.h>

// User includes
#include "clockworldview.h"

// Forward declarations
class CClkUiAlarmModel;
class CTzLocalizer;

// Constants
const TInt KHomeCityLength( 64 );

// Class declaration
/**
* @class CClockDocument
* @brief The CAknDocument inheriting class.
* @exe clock.exe
*/
class CClockDocument : public CAknDocument
	{
	public:        // Constructor and destructor

        /**
        * @brief Performs the first phase of 2-phase construction.
        * @param aApp CEikApplication&
        * @return CClockDocument* Pointer to the object of the document
        */
        static CClockDocument* NewL( CEikApplication& aApp );

        /**
        * @brief Destructor.
        */
        ~CClockDocument();
        
	public:        // New functions
	    
	    /**
        * @brief Reads the data from the document file.
        */
        void ReadDataL();
        
        /**
        * @brief Stores the data to the document file.
        */
        void StoreDataL();
	    
	    /**
	    * @brief Returns the alarm model.
	    * @return CClkUiAlarmModel* Pointer to the alarm model.
	    */
	    CClkUiAlarmModel* AlarmModel();
	    
	    /**
        * @brief Returns the previous alarm time.
        * @return TTime The previous alarm time.
        */
	    //TTime GetPrevAlarmTime();
	    
	    /**
	    * @brief Sets the new alarm time value as the previous alarm time.
	    * @param aPrevAlarmTime The new alarm time.
	    */
	    //void SetPrevAlarmTime( TTime aPrevAlarmTime );
	    
	    /**
	    * @brief Retrieves list of cities shown in world clock view.
	    * @return CArrayPakFlat< TCityInfo >* Pointer to the list of items.
	    */
	    CArrayPakFlat< TCityInfo >* GetWorldClockList();
	    
	    /**
        * @brief Returns the current home city.
        * @return TBuf A buffer containing the current home city.
        */
        const TBuf< KHomeCityLength > GetHomeCity();

        /**
        * @brief Sets and updates the home city.
        * @param aCityInfo The new home city information.
        */
        void SetHomeCityL( TCityInfo& aCityInfo );
        
        /**
        * @brief Updates the home city.
        */
        void UpdateHomeCityL();
        
        /**
        * @brief Gets wether homecity item is added to the world clock view or not.
        * @return TBool ETrue if homecity item is added to the list else EFalse.
        */
        inline TBool IsHomeCityItemAdded();

        /**
        * @brief 
        * @param aAddHomeCityItem
        */
        inline void SetHomeCityItemAdded( TBool aAddHomeCityItem );
        
	private:       // New functions
	 
        /**
        * @brief C++ default Constructor.
        * @param aApp CEikApplication&
        */
	 	CClockDocument( CEikApplication& aApp );

        /**
        * @brief Performs the second phase of 2-phase construction. May leave.
        */
        void ConstructL();
        
        /**
        * @brief Reads city data from stream.
        * @param aStream The stream from which data has to be read.
        * @param aCityInfo The city info read from the stream.
        * @param aTzHandle Handle to the tzserver.
        */
        void ReadCityL( RFileReadStream& aStream, TCityInfo& aCityInfo, RTz& aTzHandle );
        
	 private:      // Functions from base classes.

		/**
		* @brief From CEikCocument. Creates application UI class.
		*/
		CEikAppUi* CreateAppUiL();
         
	 private:      // Data
	     
	    /**
	    * @var iAlarmModel
	    * @brief The alarm model object.
	    */
	    CClkUiAlarmModel*                  iAlarmModel;
	     
	    /**
	    * @var iPrevAlarmTime
	    * @brief Holds the previous value of alarm time or 09:00 if no alarms are set.
	    */
	    TInt64                             iPrevAlarmTime;
	     
	    /**
		* @var iWorldClockList
		* @brief Contains the list of cities added in world clock view.
		*/
	    CArrayPakFlat< TCityInfo >*        iWorldClockList;
	    
	    /**
		* @var iTzLocalizer
		* @brief Pointer to CTzLocalizer class.
		*/
	    CTzLocalizer*                      iTzLocalizer;
	    
	    /**
	    * @var iHomeCity
	    * @brief Holds the current home city.
	    */
	    TBuf< KHomeCityLength >            iHomeCity;
	    
        /**
        * @var iHomeCityItemAdded
        * @brief Is ETrue if the  homecity item is added to world clock view
        *           else is EFalse.
        */
        TBool                              iHomeCityItemAdded;
	};

#include "clockdocument.inl"

#endif 		// __CLOCK_DOCUMENT_H__

// End of file
