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
* Description:  Provides methods for Attendee View's engine class.
*
*/



// INCLUDE FILES
#include "CAttendeeEng.h"
#include "AttendeeParser.h"
#include "AttendeeEngUtils.h"
#include "AttendeeViewConsts.h"
#include "CAttendeeItemList.h"
#include "MAttendeeEngObserver.h"
#include <cpbkidlefinder.h> 
#include <CPbkContactEngine.h>
#include <cpbkcontactitem.h> 
#include <calentry.h>
#include <caluser.h>
#include <CPbkContactChangeNotifier.h>

//CONSTANTS
const TInt KInitialMatchEmail( -1 );
const TInt KOrganizer( 1 );

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CAttendeeEng::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAttendeeEng* CAttendeeEng::NewL( CCalEntry& aCalEntry )
    {
    CAttendeeEng* self = new(ELeave)CAttendeeEng( aCalEntry );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self); // self
    return self;
    }

// ---------------------------------------------------------
// CAttendeeEng::ConstructL
// ---------------------------------------------------------
//
void CAttendeeEng::ConstructL()
    {
    iAttendees = CAttendeeItemList::NewL();
    iDeletedAttendees = CAttendeeItemList::NewL();
    
    iContactEngine = CPbkContactEngine::NewL();
    
    iFind = new(ELeave)CPbkFieldIdArray;    
    iFind->AppendL( EPbkFieldIdEmailAddress );

    iNotifier = CPbkContactChangeNotifier::NewL( *iContactEngine, this );        
    }

// ----------------------------------------------------------------------------
// CAttendeeEng::CAttendeeEng
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CAttendeeEng::CAttendeeEng( CCalEntry& aCalEntry ) : iCalEntry( &aCalEntry )
    {
    }

// ---------------------------------------------------------
// CAttendeeEng::~CAttendeeEng
// ---------------------------------------------------------
//
CAttendeeEng::~CAttendeeEng()
    {
    delete iNotifier;
    delete iIdleFinder;                    
    delete iFind;
    delete iContactEngine;    
    delete iDeletedAttendees;
    delete iAttendees;    
    
    }

// ---------------------------------------------------------
// CAttendeeEng::ParseAgnEntryL
// ---------------------------------------------------------
//
void CAttendeeEng::ParseAgnEntryL()
	{    
    AttendeeParser::ParseAgnEntryL( *iCalEntry, *iAttendees );
    SortAttendeesL();
    MatchEmailsL();
	}

// ---------------------------------------------------------
// CAttendeeEng::AddItemsL
// ---------------------------------------------------------
//
void CAttendeeEng::AddItemL( TDesC& aEmailAddress )
    {        
    const TInt index( 
        AttendeeEngUtils::FindLastRequiredItemL( *iAttendees ) );    

    //Create CCalAttendee
    CCalAttendee* attendee = CCalAttendee::NewL( aEmailAddress );
    CleanupStack::PushL( attendee );
    attendee->SetRoleL( CCalAttendee::EReqParticipant );
   
    //Create CAttendeeItem
    CAttendeeItem* item = CAttendeeItem::NewL( attendee ); 
    CleanupStack::Pop( attendee );
    
    item->SetOwnership( ETrue );
        
    CleanupStack::PushL( item );
    iAttendees->InsertL( item, index );
       
    CleanupStack::Pop( item );
    }

// ---------------------------------------------------------
// CAttendeeEng::AddItemL
// ---------------------------------------------------------
//
void CAttendeeEng::AddItemL( CPbkContactItem& aPbkItem, 
                             const TPbkContactItemField* aAddress )
    {      
    const TInt index( 
        AttendeeEngUtils::FindLastRequiredItemL( *iAttendees ) );
                                                                           
    CCalAttendee* attendee = CCalAttendee::NewL( aAddress->Text() );  
    CleanupStack::PushL( attendee );
    attendee->SetRoleL( CCalAttendee::EReqParticipant );
    
    CAttendeeItem* item = CAttendeeItem::NewL( attendee );     
    CleanupStack::Pop( attendee );
    
    item->SetContactId( aPbkItem.Id() );
    item->SetOwnership( ETrue );
    
    CleanupStack::PushL( item );
    
    item->SetContactTitle( aPbkItem.GetContactTitleOrNullL() );       

    //check if item is already deleted    
    AttendeeEngUtils::CheckIfDeleted( *item, *iDeletedAttendees );
 
    iAttendees->InsertL( item, index );
    
    CleanupStack::Pop( item );
       
    }

// ---------------------------------------------------------
// CAttendeeEng::DeleteItem
// ---------------------------------------------------------
//
void CAttendeeEng::DeleteItemL( const TInt aIndex )
	{    
    if ( aIndex != 0 )
        {
        //delete from attendee view                        
        CAttendeeItem* item = iAttendees->Delete( aIndex );        
        if ( item )
            {
            CleanupStack::PushL( item );
            iDeletedAttendees->AppendL( item );
            CleanupStack::Pop( item );
            }

        NotifyL();
        }    
    else
        {
        NotifyError( KErrDelete );            
        }
	}

// ---------------------------------------------------------
// CAttendeeEng::DeleteItems
// ---------------------------------------------------------
//
void CAttendeeEng::DeleteItemsL( const CArrayFix<TInt>& aIndexes )
	{    
    const TInt count( aIndexes.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        DeleteItemL( aIndexes.At( i ) );
        }
	}

// ---------------------------------------------------------
// CAttendeeEng::ChangeAttendance
// ---------------------------------------------------------
//
void CAttendeeEng::ChangeAttendanceL( const TInt aIndex, 
           const CAttendeeItem::TAttendance aAttendance )
    {    
    CAttendeeItem& item = At( aIndex );

    if ( !item.IsOrganizer() )
        {
        item.SetAttendanceL( aAttendance );                
        }    
    else
        {
        NotifyError( KErrSetAs );            
        }

    }

// ---------------------------------------------------------
// CAttendeeEng::ChangeAttendances
// ---------------------------------------------------------
//
void CAttendeeEng::ChangeAttendancesL( const CArrayFix<TInt>& aIndexes, 
                         const CAttendeeItem::TAttendance aAttendance )
    {
    const TInt count( aIndexes.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        ChangeAttendanceL( aIndexes.At( i ), aAttendance );
        }    
    }

// ---------------------------------------------------------
// CAttendeeEng::NumberOfItems
// ---------------------------------------------------------
//
TInt CAttendeeEng::NumberOfItems() const
	{    
	return iAttendees->Count();
	}

// ---------------------------------------------------------
// CAttendeeEng::At
// ---------------------------------------------------------
//
CAttendeeItem& CAttendeeEng::At( const TInt aIndex ) const
	{
    return *iAttendees->At( aIndex );
	}

// ---------------------------------------------------------
// CAttendeeEng::IsEdited
// ---------------------------------------------------------
//
TBool CAttendeeEng::IsEditedL() const
    {
    TBool result( EFalse );
    
    if ( iDeletedAttendees->Count() != 0 || 
        ((iAttendees->Count() - KOrganizer) > iCalEntry->AttendeesL().Count()) )
        {        
        AttendeeParser::UpdateAgnEntryL( *iCalEntry,
                                         *iAttendees, 
                                         *iDeletedAttendees );        
        result = ETrue;
        
        }
    
    return result;
    }

// ---------------------------------------------------------
// CAttendeeEng::SortAttendeesL
// ---------------------------------------------------------
//
void CAttendeeEng::SortAttendeesL()
    {    
    iAttendees->SortL();
    NotifyL();
    }

// ---------------------------------------------------------
// CAttendeeEng::SetObserver
// ---------------------------------------------------------
//
void CAttendeeEng::SetObserver( MAttendeeEngObserver* aObserver )
	{
    iObserver = aObserver;	
	}

// ---------------------------------------------------------
// CAttendeeEng::RemoveObserver
// ---------------------------------------------------------
//
void CAttendeeEng::RemoveObserver()
	{
    iObserver = NULL;
	}

// ---------------------------------------------------------
// CAttendeeEng::ContactEngine
// ---------------------------------------------------------
//
CPbkContactEngine& CAttendeeEng::ContactEngine() const
    {    
    return *iContactEngine;
    }

// ---------------------------------------------------------
// CAttendeeEng::PbkContactItemL
// ---------------------------------------------------------
//
CPbkContactItem* CAttendeeEng::PbkContactItemL( const TInt aIndex ) const
    {    
    CAttendeeItem& attItem = At( aIndex );    
    CPbkContactItem* ret = NULL;
    if ( attItem.ContactId() != KNullContactId )
        {
        ret = iContactEngine->ReadContactLC( 
                                            attItem.ContactId() );
        CleanupStack::Pop( ret );
        }
    return ret;
    }

// ---------------------------------------------------------
// CAttendeeEng::IdleFindCallback
// ---------------------------------------------------------
//
void CAttendeeEng::IdleFindCallback()
    {
    if ( iIdleFinder->IsComplete() )
        {
        TRAPD( error, 
        if ( iIdleFinder->Error() != KErrNone )
            {             
            NotifyError( iIdleFinder->Error() );                
            }
        else
            {                                   
            UpdateAttendeesL( iIdleFinder->TakeContactIds() );                        
            NotifyL();
            }
        delete iIdleFinder;
        iIdleFinder = NULL;
        FindNextEmailL();        
        ); //End of TRAPD

        if ( error != KErrNone )
            {
            NotifyError( error );                
            }
        }    
    }

// ---------------------------------------------------------
// CAttendeeEng::Attendees
// ---------------------------------------------------------
//
MAttendeeItemList& CAttendeeEng::Attendees() const
    {    
    return *iAttendees;
    }

// ---------------------------------------------------------
// CAttendeeEng::DeletedAttendees
// ---------------------------------------------------------
//
MAttendeeItemList& CAttendeeEng::DeletedAttendees() const
    {    
    return *iDeletedAttendees;
    }

// ---------------------------------------------------------
// CAttendeeEng::HandleDatabaseEventL
// ---------------------------------------------------------
//
void CAttendeeEng::HandleDatabaseEventL( TContactDbObserverEvent aEvent )
    {
    switch( aEvent.iType )
        {
        case EContactDbObserverEventContactAdded: //Fall through
        case EContactDbObserverEventContactChanged:
        case EContactDbObserverEventUnknownChanges:
            {
            CPbkContactItem* contactItem = 
                iContactEngine->ReadContactLC( aEvent.iContactId );
            const TInt count( iAttendees->Count() );
            for ( TInt i( 0 ); i < count; ++i )
                {
                CAttendeeItem& item = At( i );
                if ( item.ContactId() == KNullContactId )
                    {
                    if ( AttendeeEngUtils::Compare( *contactItem,
                                                    item ) )
                        {
                        item.SetContactId( contactItem->Id() );
                        item.SetContactTitle( 
                            contactItem->GetContactTitleOrNullL() );
                        }
                    }
                }
            CleanupStack::PopAndDestroy( contactItem );
            NotifyL();
            break;
            }
        case EContactDbObserverEventContactDeleted:        
            {            
            const TInt count( iAttendees->Count() );
            for ( TInt i( 0 ); i < count; ++i )
                {
                CAttendeeItem& item = At( i );
                if ( item.ContactId() == aEvent.iContactId )
                    {                    
                    item.SetContactId( KNullContactId );
                    item.SetContactTitle( NULL );                           
                    }
                }            
            NotifyL();
            break;
            }
        default:
            {
            break;
            }
        }
    }
   
// ---------------------------------------------------------
// CAttendeeEng::MatchEmailsL
// ---------------------------------------------------------
//
void CAttendeeEng::MatchEmailsL()
    {
    iFindedIndex = KInitialMatchEmail;    
    FindNextEmailL();
    }

// ---------------------------------------------------------
// CAttendeeEng::FindNextEmailL
// ---------------------------------------------------------
//
void CAttendeeEng::FindNextEmailL()
    {    
    ++iFindedIndex;
    if ( iFindedIndex >= 0 &&
         iFindedIndex < iAttendees->Count() )
        {
        //email address must be without alias name.
        //e.g. name.name@domain.com
        iIdleFinder = iContactEngine->FindAsyncL(                 
                            At( iFindedIndex ).AgnAttendee()->Address(),
                            iFind, this );
        }          
    }

// ---------------------------------------------------------
// CAttendeeEng::UpdateAttendeesL
// ---------------------------------------------------------
//
void CAttendeeEng::UpdateAttendeesL( CContactIdArray* aFoundContacts )
    {  
    if ( aFoundContacts && aFoundContacts->Count() == 1 )
        {
        //Read contact
        CPbkContactItem* pbkItem = iContactEngine->ReadContactLC( 
                                    aFoundContacts->operator[]( 0 ) );

        //Update attendee
        CAttendeeItem& item = At( iFindedIndex );
        item.SetContactId( pbkItem->Id() );
        item.SetContactTitle( pbkItem->GetContactTitleOrNullL() );
       
        CleanupStack::PopAndDestroy(pbkItem); //pbkItem
        }
    delete aFoundContacts;
    }

// ---------------------------------------------------------
// CAttendeeEng::NotifyL
// ---------------------------------------------------------
//
void CAttendeeEng::NotifyL() const
    {
    if ( iObserver )
        {
        iObserver->NotifyL();
        }
    }

// ---------------------------------------------------------
// CAttendeeEng::NotifyError
// ---------------------------------------------------------
//
void CAttendeeEng::NotifyError( const TInt aErrorCode ) const
    {
    if ( iObserver )
        {
        iObserver->NotifyError( aErrorCode );
        }
    }
    

// End of File
