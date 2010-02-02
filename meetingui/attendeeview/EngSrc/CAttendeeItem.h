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
* Description:  Implements an CAttendeeItem class in Attendee View.
*
*/



#ifndef __CATTENDEE_ITEM_H__
#define __CATTENDEE_ITEM_H__

//  INCLUDES
#include "MAttendeeItem.h"
#include <e32base.h>
#include <cntdef.h>

// FORWARD DECLARATION
class CCalUser;

// CLASS DECLARATION
/**
*  Implements AttendeeView's item
*/
class CAttendeeItem : public CBase, public MAttendeeItem        
    {   
    public: //Types:
        enum TAttendance
            {
            //EOrganizer, //=> organizer
            ERequire,   //=>Attendee && Require
            EOptional//,   //=>(Attendee || Delegate ) && !Require
            //ENone
            };
    public:
        /**
        * Two-phased constructor.
        * @param aCalAttendee, don't give null pointer
        * @return CAttendeeItem object.
        */
        static CAttendeeItem* NewL( CCalAttendee* aCalAttendee );

        /**
        * Two-phased constructor.
        * @param aCalAttendee, don't give null pointer
        * @return CAttendeeItem object.
        */
        static CAttendeeItem* NewLC( CCalAttendee* aCalAttendee );

        /**
        * Destructor.
        */
        virtual ~CAttendeeItem();
        
    private:    
        /**
        * C++ default constructor.    
        * @param aCalAttendee, don't give null pointer
        * @return CAttendeeItem object
        */
        CAttendeeItem( CCalAttendee* aCalAttendee );

    public: //From MAttendeeItem
        TContactItemId ContactId() const;
        HBufC* ContactTitle() const;        
        CCalAttendee* AgnAttendee() const;
        
    public: //New functions
        /**
        * Set contact item
        * @param aId
        */
        void SetContactId( TContactItemId aId );		

        /**
        * Set contact title
        * Ownership transfered
        * @param aTitle
        */
        void SetContactTitle( HBufC* aTitle );	

        /**
        * Set ownership
        * @param aOwnership
        */
        void SetOwnership( TBool aOwnership );

        /**
        * Returns 
        *   ETrue if CAttendeeItem own CCalUser item
        *   EFalse if CAttendeeItem not own CCalUser item
        * @return TBool
        */
        TBool Ownership() const;		       		

        /**
        * Set CCalUser
        * Ownership transfered if after this called SetOwnertship( ETrue )
        * @param aCalAttendee
        */
        void SetAgnAttendee( CCalAttendee* aCalAttendee );        
        
        /**
        * Set attendance
        * @param aAttendance
        */
		void SetAttendanceL( TAttendance aAttendance );

        /**
        * Returns attendance of CAttendeeItem
        * @return TAttendance
        */
		TAttendance AttendanceL() const;
		
		
		 /**
        * Returns boolean value wether
        * this attendee is organizer or not
        * @return TBool
        */
		TBool IsOrganizer() const;
		
		/**
        * Set item to be organizer (ETrue)
        * or not (EFalse)
        * @param aOrganizerStatus
        */
		void SetOrganizerStatus(TBool aOrganizerStatus);


        /**
        * Compare the attendees and return DESC order
        * @param aLhs
        * @param aRhs
        * @return 0, if the two objects are equal
        *         a negative value, if the first object is less than the second
        *         a positive value, if the first object is greater than the 
        *         second.
        */
        static TInt CompareL( const CAttendeeItem& aLhs, 
                              const CAttendeeItem& aRhs );
               
    private: //New functions        
        /**
        * Compare the attendaces of the attendence request
        * return DESC order
        * @param aLhs
        * @param aRhs
        * @return 0, if the two objects are equal
        *         a negative value, if the first object is less than the second
        *         a positive value, if the first object is greater than the 
        *         second.
        */
        static TInt CompareAttendancesL( const CAttendeeItem& aLhs, 
                                         const CAttendeeItem& aRhs );

        /**
        * Compare the status of the attendence request
        * return DESC order
        * @param aLhs
        * @param aRhs
        * @return 0, if the two objects are equal
        *         a negative value, if the first object is less than the second
        *         a positive value, if the first object is greater than the 
        *         second.
        */
        static TInt CompareStatusL( const CAttendeeItem& aLhs, 
                                    const CAttendeeItem& aRhs );

    private:    // data
        ///TContactItemId
        TContactItemId iContactID;

        ///Own: Alias name
        HBufC* iContactTitle;

        ///Ref: or Own: depence to iOwnership
        CCalAttendee* iCalAttendee;

        ///Boolean variable to clarify if 
        ///item owns iCalAttendee
        TBool iOwnership;
        
        //Boolean to inform if this item
        //is meeting organizer or not
        TBool iOrganizerSatus;       
    };

#endif // __CATTENDEE_ITEM_H__

// End of File
