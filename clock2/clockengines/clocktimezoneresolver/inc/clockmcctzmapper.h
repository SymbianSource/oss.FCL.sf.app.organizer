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
* Description:   The header file of the CClockMCCTzIdMapper class.
*
*/

#ifndef __CLOCK_MCC_TZ_MAPPER_H__
#define __CLOCK_MCC_TZ_MAPPER_H__

// System includes
#include <f32file.h>
#include <e32base.h>
#include <barsc.h>
#include <barsread.h>
#include <etelmm.h>

// Forward declarations
class CTzId;

/**
* @class CClockMCCTzIdMapper
* @brief Provides a mapping between numeric Time Zone Ids generated by the
* 		 Time Zone Database compiler (tzcompiler.exe) as found in TzIdentities.ini,
* 		 and Mobile Country Code (MCC) as defined in ITU E.212
* 		 The international identification plan for mobile terminals and mobile users
* @dll clocktimezoneresolver.dll
*/
class CClockMCCTzIdMapper : public CBase
	{
	public:			//Construction and Destruction
	
		/**
		* @brief Creates a CClockMCCTzIdMapper object
		* @return CClockMCCTzIdMapper*
		*/
		static CClockMCCTzIdMapper* NewL();
		
		/**
		* @brief Creates a CClockMCCTzIdMapper object and leaves a copy on the cleanup stack
		* @return CClockMCCTzIdMapper*
		*/
		static CClockMCCTzIdMapper* NewLC();
		
		/**
		* @brief Destructor
		*/
		~CClockMCCTzIdMapper();

	public:			// New functions
		
		/**
		* @brief Find all TzIds for a given MCC with optional matching StdTimeOffset
		* @param aMCC Mobile Country Code to search for
		* @param aTzIdArray Array of CTzID objects to add matching zones to.
		* @param aStdTimeOffset Limit returned zones to zones with a matching offset
		*/
		void TzIdFromMccL( const RMobilePhone::TMobilePhoneNetworkCountryCode& aMCC,
						   RArray< CTzId >& aTzIdArray,
						   const TTimeIntervalMinutes& aStdTimeOffset = -1 );
		
		/**
		* @brief Find the MCC for a given TzId
		* @param aTzId CTzId to search for
		* @return the MCC of the DST zone, or KErrNotFound if not found.
		*/
		TInt MCCFromTzIdL( const CTzId& aTzId );
		
		/**
		* @brief Find the timezone ID for a given standard offset.
		* @param aStdTimeOffset Standard offset, of type TTimeIntervalMinutes.
		* @param aTzIdArray Array of timezone IDs.
		*/
		void TzIdFromOffsetL( const TTimeIntervalMinutes& aStdTimeOffset, RArray< CTzId >& aTzIdArray );

		/**
		* @brief Find the UTC offset for a particular zone ignoring DST changes
		* @param aTz Opened Timezone server session
		* @param aTzId Timezone to get offset for
		* @return Offset in minutes from UTC
		*/
		TInt UTCOffSetWithoutDSTChangesL( RTz& aTz, const CTzId& aTzId ) const;
		
		/**
		* @brief Find the UTC offset for a particular zone ignoring DST changes
		* @param aMCC Mobile Country Code to search for
		* @param aTzIdArray Array of CTzID objects to add matching zones to.
		*/
		void DoGetTzIdFromMCCL( TInt aMcc, RArray< CTzId >& aTzIdArray );
	
	private:
		
		/** 
		* @brief Symbian OS 2nd phase constructor is private.
		*/
		void ConstructL();

	private:		// Member data
		
		/**
		* @var iFs 
		* @brief An object of type RFs
		*/
		RFs							iFs;
		
		/**
		* @var iMCCResourceFile 
		* @brief An object of type RResourceFile
		*/
		RResourceFile 				iMCCResourceFile;
		
		/**
		* @var iResourceReader 
		* @brief An object of type TResourceReader
		*/
		TResourceReader 			iResourceReader;
		
		/**
		* @var iResourceBuffer 
		* @brief An object of type HBufC8*
		*/
		HBufC8* 					iResourceBuffer;

		/**
		* @var TMCCResourceOffset 
		* @brief Enum for resource
		*/
		enum TMCCResourceOffset
			{
			EMCCRssSignature = 1,		/** Offset of RSS signature. */
			EMCCFirstResource			/** The first resource we are interested in. */
			};
	};

#endif	// __CLOCK_MCC_TZ_MAPPER_H__

// End of file
