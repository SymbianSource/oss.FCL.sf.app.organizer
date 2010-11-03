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
* Description:  Implements Attendee View's engine utilities class. 
*
*/



#ifndef __ATTENDEE_ENG_UTILS_H__
#define __ATTENDEE_ENG_UTILS_H__

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATION
class CAttendeeItem;
class CPbkContactItem;
class CAttendeeItemList;

// CLASS DECLARATION
/**
*  Implements AttendeeView's engine utils
*/
class AttendeeEngUtils
    {
    public: //New functions        
        /**
        * Find last required item index
        * @param aAttendees
        * @return position which is the first after organizers or requires
        */
        static TInt FindLastRequiredItemL( CAttendeeItemList& aAttendees );

        /**
        * Check if just created item is already deleted.
        * If item is already deleted, delete it from deleted list.
        * @param aItem
        * @param aDeleted
        */	
        static void CheckIfDeleted( CAttendeeItem& aItem, 
                                    CAttendeeItemList& aDeleted );

        /**
        * Compare contact item's email addresses to attendee item's
        * email address.
        * @param aContactItem
        * @param aAttendeeItem
        * @return ETrue if CAttendeeItem email address is same, else EFalse
        */	
        static TBool Compare( CPbkContactItem& aContactItem,
                              CAttendeeItem& aAttendeeItem );

    };

#endif // __ATTENDEE_ENG_UTILS_H__

// End of File
