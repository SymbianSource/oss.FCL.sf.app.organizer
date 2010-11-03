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
* Description:  Provides methods for Attendee View's parser class.
*
*/



// INCLUDE FILES
#include "AttendeeParser.h"
#include "CAttendeeItemList.h"
#include "CAttendeeItem.h"
#include <calentry.h>
#include <caluser.h>

// ============================ MEMBER FUNCTIONS ==============================
// ---------------------------------------------------------
// AttendeeParser::ParseAgnEntryL
// ---------------------------------------------------------
//
void AttendeeParser::ParseAgnEntryL( CCalEntry& aCalEntry,
									 CAttendeeItemList& aAttendees )
	{
           
    //Create internal attendee for organizer
    CCalAttendee* organizer = CCalAttendee::NewL(
                                    aCalEntry.OrganizerL()->Address() );       
    CleanupStack::PushL( organizer );                  
    organizer->SetCommonNameL( aCalEntry.OrganizerL()->CommonName() );    
        
    //Take ownership
    CAttendeeItem* organizerItem = CAttendeeItem::NewL( organizer );
    organizerItem->SetOwnership( ETrue );
    CleanupStack::Pop( organizer );                   
    CleanupStack::PushL( organizerItem );                        
    aAttendees.InsertL( organizerItem, 0);        
    organizerItem->SetOrganizerStatus( ETrue );    
    CleanupStack::Pop( organizerItem );
         
    //This count doesn't include organizer
    const TInt count( aCalEntry.AttendeesL().Count() );    
    TInt requiresIndex( 1 );//starts after organizer     
    for( TInt j( 0 ); j < count; ++j )
        {
        CAttendeeItem* item = CAttendeeItem::NewLC(
                                (aCalEntry.AttendeesL())[j] );
     
        
         if ( item->AttendanceL() == CAttendeeItem::ERequire )
            {
            aAttendees.InsertL( item, requiresIndex );            
            ++requiresIndex;            
            }
         else //Optional
            {
            aAttendees.AppendL( item );
            }
       
        CleanupStack::Pop( item ); //item
        }
	}

// ---------------------------------------------------------
// AttendeeParser::UpdateAgnEntryL
// ---------------------------------------------------------
//
void AttendeeParser::UpdateAgnEntryL( CCalEntry& aCalEntry,
                              		  CAttendeeItemList& aAttendees,
                              		  CAttendeeItemList& aDeleted )
    {
    //Remove deleted attendees from CCalEntry
    RemoveDeletedL( aCalEntry, aDeleted );

    //Insert new attendees to CCalEntry. Organizer that is added to internal
    //attendee list should not be added because is already in the cal entry.
    const TInt count( aAttendees.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        CAttendeeItem* item = aAttendees.At( i );
        if ( item && item->Ownership() && !item->IsOrganizer())
            {            
            //change ownership
            item->SetOwnership( EFalse );
            //take ownership in the first line                             
            aCalEntry.AddAttendeeL( item->AgnAttendee() );                            
            }
        }                
    }

// ---------------------------------------------------------
// AttendeeParser::RemoveDeletedL
// ---------------------------------------------------------
//
void AttendeeParser::RemoveDeletedL( CCalEntry& aCalEntry,
                            		 CAttendeeItemList& aDeleted )
    {
    const TInt count( aDeleted.Count() );
    TInt attCount( 0 );
    for ( TInt i( 0 ); i < count; ++i )
        {
        CAttendeeItem* item = aDeleted.At( i );
        if ( item )
            {
            CCalAttendee* attendee = item->AgnAttendee();
            attCount = aCalEntry.AttendeesL().Count();
            for ( TInt j( 0 ); j < attCount; ++j )
                {
                if ( attendee && attendee == (aCalEntry.AttendeesL())[j])
                    {               
                    //copy CCalUser to CAttendeeItem and then delete it
                    CCalAttendee* copy = CCalAttendee::NewL( attendee->Address(),
                                                             attendee->SentBy());
                    
                    CleanupStack::PushL( copy );
              
                    copy->SetCommonNameL( attendee->CommonName() );
                    copy->SetRoleL( attendee->RoleL() );
                    copy->SetStatusL( attendee->StatusL() );
                    copy->SetResponseRequested( attendee->ResponseRequested() );
                                   
                    item->SetAgnAttendee( copy );
                    item->SetOwnership( ETrue );
                    CleanupStack::Pop( copy );
                    aCalEntry.DeleteAttendeeL( j );
                    break;
                    }
                }
            }
        }
    }

// End of File
