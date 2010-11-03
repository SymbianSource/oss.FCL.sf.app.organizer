/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements Attendee View's item list class.
*
*/



#ifndef __CATTENDEE_ITEM_LIST_H__
#define __CATTENDEE_ITEM_LIST_H__

//  INCLUDES
#include "MAttendeeItemList.h"
#include <e32base.h>
#include <cntdef.h>

// FORWARD DECLARATION
class CAttendeeItem;

// CLASS DECLARATION
/**
*  Implements AttendeeView's item list
*/
class CAttendeeItemList: public CBase, public MAttendeeItemList
    {
	public:
		/**
		* Two-phased constructor.
		* @return CAttendeeItemList object.
		*/
		static CAttendeeItemList* NewL();

		/**
		* Destructor.
		*/
		virtual ~CAttendeeItemList();

	private:
		/**
		* By default Symbian 2nd phase constructor is private.
		*/
		void ConstructL();

		/**
		* C++ default constructor.
        * @return CAttendeeItemList object
		*/
		CAttendeeItemList();

    private: //From MAttendeeItemList
    	//These two functions are for 3rd parties application's.
		TInt ItemCount() const;
		MAttendeeItem& ItemAt( const TInt aIndex ) const;

	public: //New functions
        /**
        * Appends a CAttendeeItem pointer into the array.
        * Ownership is transfered
        * @param aItem         
        */
		void AppendL( const CAttendeeItem* aItem );

        /**
        * Inserts a CAttendeeItem pointer into the array at a 
        * specified position. Ownership is transfered
        * @param aItem
        * @param aPos, position within the array
        */
		void InsertL( const CAttendeeItem* aItem, const TInt aPos );

        /**
        * Remove the CAttendeeItem pointer at the specified position 
        * from the array. Ownership is transfered 
        * @param aIndex
        * @return removed object if it was in CCalEntry, otherwise return NULL
        */
		CAttendeeItem* Delete( const TInt aIndex );

        /**
        * Remove the CAttendeeItem pointer at the specified position 
        * from the array. Notice that you need to have ownership already,
        * otherwise comes alloc failure
        * @param aIndex
        */
        void Remove( const TInt aIndex );

        /**
        * Gets the number of CAttendeeItem pointers in the array.
        * @return TInt
        */
		TInt Count() const;

        /**
        * Returns pointer to the CAttendeeItem located at 
        * the specified position within the array.
        * Ownership not transfered
        * @param aIndex
        * @return CAttendeeItem
        */
		CAttendeeItem* At( const TInt aIndex ) const;

        /**
        * Sorts the CAttendeeItem pointers within the array. 
        * Sorts DESC order
        */
        void SortL();

	private: //data
        ///Own: List of CAttendeeItems
		RPointerArray<CAttendeeItem> iItems;

    };

#endif // __CATTENDEE_ITEM_LIST_H__

// End of File
