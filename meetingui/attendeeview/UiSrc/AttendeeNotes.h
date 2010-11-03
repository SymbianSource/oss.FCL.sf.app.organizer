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
* Description:   Attendee view Ui note class definition
*
*/



#ifndef __ATTENDEE_NOTES_H__
#define __ATTENDEE_NOTES_H__

#include <e32std.h>

// CLASS DECLARATION
/**
*  AttendeeNotes class.
*/
class AttendeeNotes
    {
    public: // new functions
        /**
        * Display AttendeeView deletes confirmation query
        * @param aAmount
        * @return positive value if yes, else zero or negative
        */	
        static TInt AttendeeViewConfirmationQueryL( const TInt aAmount = 1 );

        /**
        * Display AttendeeView information note.
        * @param resource id
        */	
        static void InformationNoteL( const TInt aResourceId );

        /**
        * Display AttendeeView information note
        * @param aResourceId
        * @param aText, which included to resource
        */	
        static void InformationNoteL( const TInt aResourceId, 
                                      const TDesC& aText );

        /**
        * Display Email address query
        * @param aDataText
        * @return positive value if pressed ok, else zero or negative
        */	
        static TInt EmailAddressQueryL( TDes& aDataText );

    };

#endif // __ATTENDEE_NOTES_H__

// End of File
