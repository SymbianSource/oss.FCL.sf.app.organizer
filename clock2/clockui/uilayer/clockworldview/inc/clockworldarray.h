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
* Description:   This is the header file for the CClockWorldArray class.
*
*/

#ifndef __CLOCK_WORLD_ARRAY_H__
#define __CLOCK_WORLD_ARRAY_H__

// System includes
#include <tzlocalizationdatatypes.h>

// User includes
#include "clockworldview.h"
#include "clkcommon.h"

// Constants
const TInt KMaxCitiesInList( 15 );

// Forward declarations
class CTzId;
class CClockWorldThumbnailProvider;

// Literals
_LIT( KDefaultImagePath, "" );

// Class declaration
/**
* @class CClockWorldArray
* @brief The array of country and city list for the worldclock list.
* @exe clock.exe
*/
class CClockWorldArray : public CBase,
						 public MDesCArray
	{
	public:			// Constructor and Destructor
		
		/**
		* @brief Two-phased constructor. 
		* @param aArray The list of cities.
		* @param aEnv The control environment.
		* @return CClockWorldArray* Pointer to the constructed object.
		*/
		static CClockWorldArray* NewL( CArrayPakFlat< TCityInfo >* aArray, CEikonEnv* aEnv );
		
		/**
		* @brief Destructor.
		*/
		~CClockWorldArray();
		
	public:			// From base classes
    
	    /**
	    * @brief From MDesCArray. Returns the number of items in the array.
	    * @return TInt The number of items in the array.
	    */
	    TInt MdcaCount() const;
	    
	    /**
	    * @brief Returns a descriptor to list item text of the given index.
	    * @param aIndex The index of the list item.
	    * @return TPtrC16 Descriptor pointing to the list item content.
	    */
	    TPtrC16 MdcaPoint( TInt aIndex ) const;
	    
	public:			// New functions
	
		/**
        * @brief Adds a city to array.
        * @param aCityInfo City data to be inserted.
        */
		void AddCityL( TCityInfo aCityInfo );
		
		/** 
		* @brief Removes citydata from array. Index out of bounds is simply ignored.
		* @param aIndex Index of the city to be removed.
		*/
		void RemoveCity( TInt aIndex );
		
		/**
		* @brief Returns information of the city at aIndex in the list.
		* 		 Index out of bounds raises an E32USER-CBase 21 panic.
		* @param aIndex Index of the item.
		* @return TCityInfo Information of the city.
		*/
		const TCityInfo& GetCity( TInt aIndex );
		
		/** 
		* @brief Searches the list for given aCityInfo.
		* @param aCityInfo TCityInfo class containing the city info to be searched for.
		* @return TInt Index of the city if there is a match or 
		* 			   KErrNotFound if there's no match.
		*/
		TInt GetListBoxIndex( TCityInfo aCityInfo );
	    
	    /**
	    * @brief Updates the image index and image path for a particular listitem.
	    * @param aImageIndex The index of the image in the iconArray.
	    * @param aListIndex The index of the list item.
	    * @param aFileName The path of image file.
	    */
	    void UpdateImageInListItem( TInt aImageIndex, TInt aListIndex, TPtrC aFileName );
	    
	    /**
	    * @brief Checks whether the image file is used by any of the listitems.
	    * @param aFileName The path of image file.
	    * @param aListIndex listitem index.
	    * @return TInt Index of the image in the listitem if there is a match or 
		* KErrNotFound if there's no match.
		*/
	    TInt CheckIfImageLoaded( TPtrC aFileName, TInt aListIndex  );
	    
	    /**
	    * @brief Returns the image index for a particular listitem.
	    * @param aListItem  Index of a list item.
	    * @return TInt Index of the image.
	    */
	    TInt GetImageIndex( TInt aListItem );
	    
	    /**
	    * @brief Gets the current home city and appends it to the city list.
	    */
	    void InsertHomeCityItem();
	    
	    /**
	    * @brief Inserts the previous home city at the position of the new home city in the list.
	    * @param aCityInfo The info of the previous home city.
	    * @param aIndex The position at which it has to be inserted.
	    */
	    void InsertOldHomeCityItem( TCityInfo aCityInfo, TInt aIndex );
	    
	    /**
        * @brief Sets the thumbnail bitmap returned by the Thumbnail Manager.
        * @param aBitmap  Bitmap returned by the Thumbnail Manager.
        * @param aThumbnailRequestId thumbnail request id given by Thumbnail Manager .
        */
	    void SetThumbnail( CFbsBitmap *aBitmap, TInt aThumbnailRequestId );
        
	    /**
        * @brief Request the thumbnail bitmap from the Thumbnail Manager.
        * @param aFileName Image filename for which thumbnail is requested.
        * @param aIndex city index.
        */
	    void GetThumbnailL( TPtrC aFileName, TInt aIndex );
        
	    /**
        * @brief Deletes the thumbnail for a given image.
        * @param aFileName Filename for a given image.
        */
        void DeleteThumbnailL( TPtrC aFileName );

	    /**
        * @brief Sets clockworlditemprocessor.
        * @param aItemProcessor Pointer to the CClockWorldItemProcessor instance.
        */
	    void SetItemProcessor( CClockWorldItemProcessor* aItemProcessor );
        
		 /** 
	    * @brief Updates single city time entry.
	    * @param aIndex City time entry index.
	    */
	    void UpdateTimeEntry( TInt aIndex );

        /** 
        * Updates city time list.
        */
        void UpdateTimeEntryList();

	private:		// New functions
	
		/**
		* @brief C++ default constructor.
		*/
		CClockWorldArray();
		
		/**
		* @brief Performs the 2nd phase of the two-phased construction.
		* @param aEnv The control environment.
		* @param aArray The list of cities.
		*/
		void ConstructL( CArrayPakFlat< TCityInfo >* aArray, CEikonEnv* aEnv  );
		
		/**
		* @brief Returns if DST is applicable or not for the timezone index at aIndex.
		* @param aIndex The index of the timezone in the list.
		* @return TBool ETrue if Dst is application for the given city at aIndex.
		*/
		TBool IsDstOnL( TInt aIndex ) const;
		
		/**
		* @brief Gets the offset with DST for the given timezone id.
		* @param aDstOffset This will contain the DST offset obtained from the rules.
		* @param aTzId The timezone id of zone for which we need the DST offset.
		*/
	    void GetOffSetWithDstL( TInt32& aDstOffset, const CTzId& aTzId ) const;
		
	private:		// Data
	
		/**
		* @var iCityData
		* @brief An array of TCityInfo.
		*/
		CArrayPakFlat< TCityInfo >*			iCityArray;
		
		/**
		* @var iListData
		* @brief An array strings displayed in the city list.
		*/
		CArrayPtrFlat< HBufC >*				iListData;
		
		/**
        * @var iEnv
        * @brief The control environment.
        */
        CEikonEnv*                          iEnv;
        
        /**
        * @var iThumbnailProivder
        * @brief CClockWorldThumbnailProvider instance.
        */
        CClockWorldThumbnailProvider*       iThumbnailProvider;
        
        /**
        * @var iItemProcessor
        * @brief CClockWorldItemProcessor instance.
        */
        CClockWorldItemProcessor*          iItemProcessor;       
	};

#endif 			// __CLOCK_WORLD_ARRAY_H__

// End of file
