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
* Description:  Defines interface of attendee item. 
*
*/



#ifndef __MATTENDEE_ITEM_H__
#define __MATTENDEE_ITEM_H__

//  INCLUDES
#include <cntdef.h>

// FORWARD DECLARATION
//class CCalUser;
class CCalAttendee;

// CLASS DECLARATION
/**
* Define interface of attendee item
*/
class MAttendeeItem
    {
    public:    
        /**
        * Destructor.
        */
        virtual ~MAttendeeItem(){};

    public: //New functions
        /**
        * Return the contact id of this attendee
        * @return contact id
        */
		virtual TContactItemId ContactId() const = 0;

        /**
        * Return the contact title or null
        * Ownership not transferred
        * @return contact title or null
        */
		virtual HBufC* ContactTitle() const = 0;        
        
        /**
        * Return the attendee of agenda model
        * NULL if CCalUser item is deleted from CCalEntry
        * Ownership not transferred
        * @return attendee
        */
        virtual CCalAttendee* AgnAttendee() const = 0;
        
    };

#endif // __MATTENDEE_ITEM_H__

// End of File
