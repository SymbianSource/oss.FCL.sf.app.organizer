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
* Description:  Provides methods for Attendee View's engine utilities class.
*
*/



// INCLUDE FILES
#include "AttendeeEngUtils.h"
#include "CAttendeeItemList.h"
#include "CAttendeeItem.h"
#include <calentry.h>
#include <cpbkcontactitem.h>
#include <caluser.h>

// ============================ MEMBER FUNCTIONS ==============================
// ---------------------------------------------------------
// AttendeeEngUtils::FindLastRequiredItem
// ---------------------------------------------------------
//
TInt AttendeeEngUtils::FindLastRequiredItemL( CAttendeeItemList& aAttendees )
    {
    const TInt count( aAttendees.Count() );
    TInt i( 0 );
    for ( ; i < count; ++i )
        {
        CAttendeeItem* item = aAttendees.At( i );
        
        if(!item->IsOrganizer() &&
        item->AttendanceL() != CAttendeeItem::ERequire )
            {
            return ++i;
            }
        }

    return i;
    }

// ---------------------------------------------------------
// AttendeeEngUtils::CheckIfDeleted
// ---------------------------------------------------------
//
void AttendeeEngUtils::CheckIfDeleted( CAttendeeItem& aItem, 
                                       CAttendeeItemList& aDeleted )
    {
    const TInt count( aDeleted.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        CAttendeeItem* temp = aDeleted.At( i );
        if ( aItem.ContactId() == temp->ContactId() )
            {
            if ( aItem.AgnAttendee()->Address().Compare( 
                    temp->AgnAttendee()->Address() ) == 0 )
                {                
                aDeleted.Remove( i );
                delete temp;
                temp = NULL;
                }
            }
        }
    }

// ---------------------------------------------------------
// AttendeeEngUtils::Compare
// ---------------------------------------------------------
//
TBool AttendeeEngUtils::Compare( CPbkContactItem& aContactItem,
                                 CAttendeeItem& aAttendeeItem )
    {
    TInt index( 0 );
    const TInt count( aContactItem.PbkFieldCount() );
    while ( index < count )
        {
        TPbkContactItemField* itemField = 
                    aContactItem.FindField( EPbkFieldIdEmailAddress, index );
        if ( itemField )
            {
            if ( itemField->Text().Compare( 
                    aAttendeeItem.AgnAttendee()->Address() ) == 0 )
                {
                return ETrue;
                }
            }
        ++index;
        }

    return EFalse;
    }

// End of File
