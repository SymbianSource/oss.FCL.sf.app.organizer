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
* Description:   This is the header file for the CClockWorldThumbnailProvider class.
*
*/

#ifndef __CLOCK_WORLD_THUMBNAIL_PROVIDER_H__
#define __CLOCK_WORLD_THUMBNAIL_PROVIDER_H__

// System includes
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>

// Forward declarations
class CClockWorldArray;

// Class declaration
/**
* @class CClockWorldThumbnailProvider
* @brief This class manages thumbnails requests for worldclockview.
* @exe clock.exe
*/
class CClockWorldThumbnailProvider : public CBase,
                                     public MThumbnailManagerObserver
    {
    public:			// Constructor and destructor
    	
    	/**
		* @brief Creates a CClockWorldThumbnailProvider object. 
		* Performs the first phase of 2-phase construction.
		* @param aClockWorldArray A pointer to CClockWorldArray object.
		* @return CClockWorldThumbnailProvider* A pointer to the created instance of CClockWorldThumbnailProvider.
		*/
		static CClockWorldThumbnailProvider* NewL( CClockWorldArray * aClockWorldArray );
		
		/**
		* @brief Destructor.
		*/
		~CClockWorldThumbnailProvider();
			
    public:        // From Base Classes.
        
		/**
        * @brief Callback function from MThumbnailManagerObserver for getting thumbnails. 
        */		
		void ThumbnailReady( TInt aError, 
		                     MThumbnailData& aThumbnail, 
		                     TThumbnailRequestId aId );
	    
		/**
        * @brief Callback function from MThumbnailManagerObserver. 
        */      
		void ThumbnailPreviewReady( MThumbnailData& aThumbnail,
		                            TThumbnailRequestId aId );
		
    public:        // New functions
        
        /**
        * @brief Requests for thumbnail for a given image.
        * @param aFileName Filename with absolute filepath for the given image.
        */              
        TInt GetThumbnailL( const TDesC& aFileName );

        /**
        * @brief Requests for thumbnail for a given image.
        * @param aFileName Filename with absolute filepath for the given image.
        */              
        void DeleteThumbnailL( const TDesC& aFileName );

    private:		// New functions

        /**
		* @brief C++ default constructor.
		*/
        CClockWorldThumbnailProvider();
		
		/**
		* @brief By default Symbian OS constructor is private.
		* @param aClockWorldArray Pointer to the created CClockWorldThumbnailProvider instance.
		*/
        void ConstructL( CClockWorldArray * aClockWorldArray );       
        
    private:		// Data
        
        /**
		* @var iThumbnailManager
		* @brief Pointer to the created instance of CThumbnailManager.
		*/
        CThumbnailManager*			iThumbnailManager;
        
        /**
		* @var iWorldArray
		* @brief Pointer to the created instance of CClockWorldArray.
		*/
        CClockWorldArray*			iWorldArray;	    
    };

#endif //__CLOCK_WORLD_THUMBNAIL_PROVIDER_H__