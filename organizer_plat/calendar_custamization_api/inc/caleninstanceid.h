/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This class handles the TCalenInstanceId references to a possible instance of entry.
*
*/

#ifndef CALENINSTANCEID_H
#define CALENINSTANCEID_H

// System includes
#include <calentry.h>
#include <calinstance.h>
#include <caltime.h>

// Forward declarations
class CCalInstanceView;
class CCalEntryView;

// Class declaration
/**
 * @class TCalenInstanceId 
 * @brief References to a possible instance of (repeating) entry.
 * Reference is very weak:
 * There might not be such instance, and case by case has to be decided
 * what should be done, if such instance doesn't exists.
 *
 * This class is meant to be used, when we want to store what instance was
 * used/focused/modified/displayed, but we don't want to store actual instance.
 * Actual instance might become unusable, if underlying entry is modified
 * or removed.
 *
 * Thus it's safer to store weak reference to it and fetch instance when
 * needed.
 *
 * Entry type is cached to this class only for optimization.
 * Entry type is needed e.g. when opening Options-menus to decide
 * what items should be there and fetching real instances to get entry
 * type would be too heavy operation.
 */
class TCalenInstanceId
    {
	public:

		/**
		 * @brief Factory method. Creates a TCalenInstanceId object
		 * 
		 * @param aInstance Reference of type CCalInstance for which the instance id is required
		 * @return TCalenInstanceId The instance id created
		 */
		static TCalenInstanceId CreateL( const CCalInstance& aInstance );

		/**
		 * @brief Overloaded factory method, Creates a TCalenInstanceId object
		 * 
		 * @param aEntry Reference of type CCalEntry for which the instance id is required
		 * @param aInstanceTime Reference of type TCalTime
		 * @param aColId The collection id
		 * 
		 * @return TCalenInstanceId The instance id created
		 */
		static TCalenInstanceId CreateL( const CCalEntry& aEntry,
									 	 const TCalTime& aInstanceTime,
										 const TCalCollectionId aColId = -1 );

		/**
		 * @brief Overloaded factory method, Creates a TCalenInstanceId object
		 * 
		 * @param aLuid Reference of type TCalLocalUid for which the instance id is required
		 * @param aInstanceTime Reference of type TTime
		 * @param aInstanceViewed ETrue if the instance is viewed in the viewer, EFalse otherwise
		 * @param aColId The collection id.
		 * 
		 * @return TCalenInstanceId The instance id created
		 */
		static TCalenInstanceId CreateL( const TCalLocalUid& aLuid,
										 const TTime& aInstanceTime,
										 TBool aInstanceViewed,
										 const TCalCollectionId aColId = -1 );

		/**
		 * @brief Returns a null instance id
		 * 
		 * @return TCalenInstanceId The null instance id 
		 */
		static TCalenInstanceId NullInstanceId();
    
		/**
		 * @brief Default C++ constructor
		 */    
		inline TCalenInstanceId();

		/**
		 * @brief Default C++ Constructor
		 * @param aLuid The local uid of type TCalLocalUid
		 * @param aInstanceTime The instance time
		 * @param aType The entry type specified by CCalEntry::TType
		 * @param aColId The collection id.
		 */
		inline TCalenInstanceId( const TCalLocalUid& aLuid,
								 const TTime& aInstanceTime,
								 CCalEntry::TType aType,
								 const TCalCollectionId aColId = -1);
		
		/**
		 * @brief Overloaded C++ Constructor
		 * @param aLuid The local UID
		 * @param aInstanceTime The instance time
		 * @param aInstanceViewed ETrue if the instance is viewed in viewer, EFalse otherwise
		 * @param aColId The collection id.
		 */
		inline TCalenInstanceId( const TCalLocalUid& aLuid,
								 const TTime& aInstanceTime,
								 TBool aInstanceViewed,
								 const TCalCollectionId aColId = -1 );
		/**
		 * @brief Overloaded operator. Checks whether two objects of type TCalenInstanceId are same 
		 * 
		 * @return ETrue if the objects compared are same. EFalse otherwise
		 */    
		TBool operator==( const TCalenInstanceId& aX ) const;
    
	public:			// Data members
	
		/**
		 * @var iEntryLocalUid
		 * @brief The entry local UID
		 */
		TCalLocalUid		iEntryLocalUid;

		/**
		 * @var iInstanceTime
		 * @brief The instance time
		 */
		TTime				iInstanceTime;
		
		/**
		 * @var iColId
		 * @brief The collection id.
		 */
		TCalCollectionId	iColId;
		
		/**
		 * @var iType
		 * @brief The calendar entry type
		 */
		CCalEntry::TType	iType;
		
		/**
		 * @var iInstanceViewed
		 * @brief ETrue if the instance is viewed in the viewer, EFalse otherwise
		 */
		TBool				iInstanceViewed;
    };

#include "CalenInstanceId.inl"

#endif // CALENINSTANCEID_H

// End of file
