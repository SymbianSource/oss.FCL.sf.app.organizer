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
* Description:  Defines interface of attendee item list.
*
*/



#ifndef __MATTENDEE_ITEM_LIST_H__
#define __MATTENDEE_ITEM_LIST_H__

//  INCLUDES
#include "MAttendeeItem.h"

// CLASS DECLARATION
/**
* Define interface of attendee item list 
*/
class MAttendeeItemList
    {
	public:
        /**
        * Destructor.
        */
        virtual ~MAttendeeItemList(){};

    public: //New functions
        /**
        * Return number of items in MAttendeeList
        * @return number of items
        */
		virtual TInt ItemCount() const = 0;

        /**
        * Return reference to MAttendeeItem
        * @param aIndex, The index of the object reference within the array.
        * @return MAttendeeItem, this include also CCalUser
        */
		virtual MAttendeeItem& ItemAt( const TInt aIndex ) const = 0;

    };

#endif // __MATTENDEE_ITEM_LIST_H__

// End of File
