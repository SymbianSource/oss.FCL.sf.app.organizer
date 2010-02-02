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
* Description:  Implements Attendee View's parser class.
*
*/



#ifndef __ATTENDEE_PARSER_H__
#define __ATTENDEE_PARSER_H__

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATION
class CCalEntry;
class CCalUser;
class CAttendeeItemList;

// CLASS DECLARATION
/**
*  Implements AttendeeView's parser
*/
class AttendeeParser
    {
    public: //New functions
        /**         
        * Going through agenda entry and create 
        * CAttendeeItems in CAttemdeeItemList. 
        * List need to sort after parsing
        * @param aCalEntry
        * @param aAttendees
        */	
    	static void ParseAgnEntryL( CCalEntry& aCalEntry,
    							    CAttendeeItemList& aAttendees );

        /**
        * Update agenda entry
        * @param aCalEntry
        * @param aAttendees
        * @param aDeleted
        */	
        static void UpdateAgnEntryL( CCalEntry& aCalEntry,
                                     CAttendeeItemList& aAttendees,
                                     CAttendeeItemList& aDeleted );
    private:
        /**
        * Remove deleted items from CCalEntry
        * @param aCalEntry        
        * @param aDeleted
        */	
        static void RemoveDeletedL( CCalEntry& aCalEntry,
                                    CAttendeeItemList& aDeleted );
    };

#endif // __ATTENDEE_PARSER_H__

// End of File
