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
* Description:  This API is only used by Attendee view. 
								Defines interface of the AttendeeView engine.
*
*/



#ifndef __MATTENDEE_ENG_H__
#define __MATTENDEE_ENG_H__

//  INCLUDES
#include "CAttendeeItem.h"

// FORWARD DECLARATION
class MAttendeeEngObserver;
class CCalEntry;
class CPbkContactItem;
class CDesC16Array;
class TPbkContactItemField;
class CPbkContactEngine;

// CLASS DECLARATION
/**
* This MAttendeeEng API is only used by Attendee view.
* Define interface of attendee engine
*/
class MAttendeeEng
    {
	public:
        /**
        * Destructor.
        */	
        virtual ~MAttendeeEng(){};

    public: //new functions        
        /**
        * Going through agenda entry and create 
        * CAttendeeItems in CAttemdeeItemList. 
        * Created list is sorted order.
        */	
		virtual void ParseAgnEntryL() = 0;

        /**
        * Create new CAttendeeItem and add it to
        * CAttendeeItemList. After adding list need
        * to sort.
        * @param aEmailAddress 
        */	
		virtual void AddItemL( TDesC& aEmailAddress ) = 0;

        /**
        * Create new CAttendeeItemsand add it to CAttendeeItemList
        * After adding list need to sort.
        * @param aPbkItem, reference to phonebook item
        * @param aAddress, pointer to used email address, because
        * phonebook item can have many Email addresses.
        */	
		virtual void AddItemL( CPbkContactItem& aPbkItem, 
                                const TPbkContactItemField* aAddress ) = 0;

        /**
        * Delete one CAttendeeItem from CAttendeeItemList
        * @param aIndex, index in array
        */	
		virtual void DeleteItemL( const TInt aIndex ) = 0;

        /**
        * Delete one or more CAttendeeItems from CAttendeeList
        * @param aIndexes, array of indexes
        */	
		virtual void DeleteItemsL( const CArrayFix<TInt>& aIndexes ) = 0;

        /**
        * Change one CAttendeeItem's attendance
        * @param aIndex,
        * @param aAttendance, see CAttendeeItem
        */	
        virtual void ChangeAttendanceL( const TInt aIndex, 
                         const CAttendeeItem::TAttendance aAttendance ) = 0;

        /**
        * Change one or more CAttendeeItem's attendance
        * @param aIndexes,
        * @param aAttendance, see CAttendeeItem
        */	
        virtual void ChangeAttendancesL( const CArrayFix<TInt>& aIndexes, 
                         const CAttendeeItem::TAttendance aAttendance ) = 0;


        /**
        * Return amount of items
        * @return TInt
        */	
		virtual TInt NumberOfItems() const = 0;

        /**
        * A reference to the CAttendeeItem at position 
        * aIndex the array.
        * @param aIndex
        * @return CAttndeeItem
        */
		virtual CAttendeeItem& At( const TInt aIndex ) const = 0;
		
        /**
        * Return 
        *   ETrue if CCalEntry is edited 
        *   EFalse if CCalEntry is not edited   
        * @return TBool
        */
        virtual TBool IsEditedL() const = 0;

        /**
        * Sort CAttendeeItems in CAttendeeItemList
        */
        virtual void SortAttendeesL() = 0;

        /**
        * Set CAttendeeEng's observer
        * Ownership not transferred
        * @param aObserver
        */
        virtual void SetObserver( MAttendeeEngObserver* aObserver ) = 0;

        /**
        * Remove observer        
        */
    	virtual void RemoveObserver() = 0;

        /**
        * Return reference to phonebook's contact engine
        * @return CPbkContactEngine
        */
        virtual CPbkContactEngine& ContactEngine() const = 0;

        /**
        * Return pointer to phonebook's contact item
        * Can be NULL pointer if contact not found
        * @param aIndex, The index of the object within the array
        * @return CPbkContactItem, ownership remove
        */
        virtual CPbkContactItem* PbkContactItemL( 
                                    const TInt aIndex ) const = 0;     
        
    };

#endif // __MATTENDEE_ENG_H__

// End of File
