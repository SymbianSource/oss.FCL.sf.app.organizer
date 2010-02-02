/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Provides methods for Meeting request mailbox utils ECom implementation.  
*
*/


// INCLUDES

#include "cmrmailboxutilsimpl.h"
#include "cmrutilsmsvhelper.h"
#include "cmrutilssettingsmgr.h"
#include "cmrmailboxinfo.h"
#include <cmrmbutilsmbinfohandler.h>
#include <bldvariant.hrh>
#include <featmgr.h>
#include <calentry.h>
#include <calsession.h>
#include <calcommon.h>
#include <caluser.h>
#include <utf.h>
#include <mmrmbutilssettings.h>

// CONSTANTS

namespace
{

const TInt KObserverGranularity = 1; // in most cases 0 or 1 observer
_LIT( KMailtoMatchPattern, "mailto:*" ); // this is never localized
const TInt KMailtoLength = 7; // "mailto:" length

}

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::NewL
// Symbian two-phased constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CMRMailboxUtilsImpl* CMRMailboxUtilsImpl::NewL( CMsvSession* aMsvSession )
	{	
	CMRMailboxUtilsImpl* self = new( ELeave ) CMRMailboxUtilsImpl();
	CleanupStack::PushL( self );
	self->ConstructL( aMsvSession );
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::CMRMailboxUtilsImpl
// Default constructor
// ----------------------------------------------------------------------------
//
CMRMailboxUtilsImpl::CMRMailboxUtilsImpl()
    : iObservers( KObserverGranularity )
    {    
    }
    
// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::~CMRMailboxUtilsImpl
// Destructor.
// ----------------------------------------------------------------------------
//        
CMRMailboxUtilsImpl::~CMRMailboxUtilsImpl()
    {
    FeatureManager::UnInitializeLib();
    
    iObservers.Close();    
    delete iMsvHelper;
    delete iSettingsMgr;
    delete iCmailHandler;
    }
    
// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::ConstructL
//
// 2nd phase constructor.
// ----------------------------------------------------------------------------
//        
void CMRMailboxUtilsImpl::ConstructL( CMsvSession* aMsvSession )    
    {
    FeatureManager::InitializeLibL();
    
    iSettingsMgr = CMRUtilsSettingsMgr::NewL();
    iMsvHelper = CMRUtilsMsvHelper::NewL( aMsvSession, *this );
    
	if ( FeatureManager::FeatureSupported( KFeatureIdFfEmailFramework ) )
		{
		// Create this only when CMail is present
		_LIT8( KCMail, "CMAIL" );
		TPtrC8 cmail( KCMail );
		iCmailHandler = 
                CMRMBUtilsMBInfoHandler::NewL( 
                        cmail,
                        *this );
		}
    }
    
    
// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::GetDefaultMRMailBoxL
// ----------------------------------------------------------------------------
//    
TInt CMRMailboxUtilsImpl::GetDefaultMRMailBoxL( TMailboxInfo& aBoxInfo )
    {
    aBoxInfo.iEntryId = KMsvNullIndexEntryId;
    TMsvId candidateId( KMsvNullIndexEntryId );
    TInt retVal = iSettingsMgr->GetDefaultMRMailBoxL( candidateId );
    
    if ( iCmailHandler )
        {
        TInt cmailId;
        MMRMBUtilsSettings* cmailSettings = 
                iCmailHandler->SettingsMgr();
                    
        if ( cmailSettings )
            {
            cmailSettings->GetDefaultMailboxSettingsL( cmailId );            
            if ( cmailId )
                {
                candidateId = cmailId;
                }
            }
        }    
    
    if ( retVal == KErrNone )
        {
        // try to read mailbox info
        const CMRMailBoxInfo* info = iMsvHelper->MRMailBoxInfoL( candidateId );
        
        if ( !info && iCmailHandler )
        	{
        	// Tru getting mailbox information using extension
        	info = iCmailHandler->MRMailBoxInfoL( candidateId );
        	}
        
        if ( !info )
            { 
            // default doesn't exist anymore, try to set another one
            RArray<CMRMailboxUtils::TMailboxInfo> mailBoxes;
            CleanupClosePushL( mailBoxes );
            ListMailBoxesL( mailBoxes );
            
            if ( mailBoxes.Count() > 0 )
                {
                candidateId = mailBoxes[0].iEntryId; 
                if ( SetDefaultMRMailBoxL( candidateId ) == KErrNone )
                    { 
                    // setting succeeded, read data of the new default mailbox                    
                    info = iMsvHelper->MRMailBoxInfoL( candidateId );
                    if ( !info && iCmailHandler )
                    	{
                    	// Try getting mailbox information using extension
                    	info = iCmailHandler->MRMailBoxInfoL( candidateId );
                    	}                    
                    }
                }
            
            // mailboxes
            CleanupStack::PopAndDestroy(); // codescanner::cleanup
            }
        
        if ( info )
            {
            // After all the steps we have found a valid MR mailbox,
            // give it to the caller then           
            aBoxInfo.iName.Set( info->Name() );
            aBoxInfo.iEmailAddress.Set( info->Address() );
            aBoxInfo.iMtmUid = info->MtmUid();
            aBoxInfo.iEntryId = info->EntryId();
            }
        else
            {            
            // We only return KErrNone or KErrNotFound based on the first
            // GetDefaultMRMailBoxL() check, later errors cause a leave
            User::Leave( KErrGeneral );
            }        
        }        
            
    return retVal;
    }


// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::SetDefaultMRMailBoxL
// ----------------------------------------------------------------------------
// 
TInt CMRMailboxUtilsImpl::SetDefaultMRMailBoxL( TMsvId aDefaultBox )
    {
    TInt retVal = iSettingsMgr->SetDefaultMRMailBoxL( aDefaultBox );
    
    if ( iCmailHandler )
        {        
        const CMRMailBoxInfo* info = 
                iCmailHandler->MRMailBoxInfoL( aDefaultBox);
        
        if ( info )
            {
            // This is CMAIL box
            MMRMBUtilsSettings* cmailSettings = 
                    iCmailHandler->SettingsMgr();
            
            if ( cmailSettings )
                {
                cmailSettings->SetDefaultMailboxSettingL( aDefaultBox );
                }
            }
        }
    
    NotifyObserversL( MMRMailboxUtilsObserver::EMRDefaultSet );
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::ListMailBoxesL
// ----------------------------------------------------------------------------
// 
void CMRMailboxUtilsImpl::ListMailBoxesL( RArray<TMailboxInfo>& aMailBoxes )
    {
    iMsvHelper->ListMRMailBoxesL( aMailBoxes );
    if ( iCmailHandler )
    	{
    	iCmailHandler->ListMRMailBoxesL( aMailBoxes );
    	}
    }

// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::GetSourceMtmUidL
// ----------------------------------------------------------------------------
//  
TInt CMRMailboxUtilsImpl::GetSourceMtmUidL(
    const CCalEntry& aCalEntry,
    TUid& aMtmUid )
    {
    TInt retVal( KErrNotFound );    
    
    RArray<CMRMailboxUtilsImpl::TMailboxInfo> mailboxes;
    CleanupClosePushL( mailboxes );
    ListMailBoxesL( mailboxes );
    
    TInt matchIndex( KErrNotFound );
    // Ownership of phone owner is not transferred:
    CCalUser* owner = PhoneOwnerL( aCalEntry, mailboxes, matchIndex );    
    if ( owner && KErrNotFound != matchIndex )
        {
        // Array contains itself out-of-bounds check
        aMtmUid = mailboxes[matchIndex].iMtmUid; // codescanner::accessArrayElementWithoutCheck2
        retVal = KErrNone;
        }
        
    CleanupStack::PopAndDestroy(); // mailboxes
    return retVal;  
    }

// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::ThisAttendeeL
// ----------------------------------------------------------------------------
// 
CCalAttendee* CMRMailboxUtilsImpl::ThisAttendeeL( const CCalEntry& aCalEntry )
    {    
    CCalAttendee* thisAtt = NULL;    
    CCalUser* phoneOwner = aCalEntry.PhoneOwnerL(); // ownership not transf.       

    if ( phoneOwner )
        {
        RPointerArray<CCalAttendee>& attendees = aCalEntry.AttendeesL();    
        TInt attCount( attendees.Count() );
        for ( TInt i( 0 );i < attCount; ++i )
            {            
            CCalAttendee* att = attendees[i];
            if ( att && phoneOwner == att )
                {
                // first match is enough:
                thisAtt = att;
                break;
                }
            }            
        }
    return thisAtt; // ownership not transferred, may be also NULL
    }

// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::IsOrganizerL
// ----------------------------------------------------------------------------
// 
TBool CMRMailboxUtilsImpl::IsOrganizerL( const CCalEntry& aCalEntry )
    {
    TBool retVal( EFalse );
    CCalUser* organizer = aCalEntry.OrganizerL(); // ownership not transf.
    CCalUser* phoneOwner = aCalEntry.PhoneOwnerL(); // ownership not transf.

    if ( organizer && phoneOwner && organizer == phoneOwner )
        {
        retVal = ETrue;
        }
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::SetPhoneOwnerL
// ----------------------------------------------------------------------------
//    
TInt CMRMailboxUtilsImpl::SetPhoneOwnerL(
    CCalEntry& aCalEntry,
    TMsvId /*aPrimaryBox*/ )
    {
    TInt retVal( KErrNone );
    CCalUser* phoneOwner = aCalEntry.PhoneOwnerL(); // ownership not transf.
    
    // TODO: we should optimize the lookup, by:
    // prioritising the mailbox where entry received + calendar mr mailbox
    // prioritising mailbox which causes last match (cache last match)    
    
    if ( !phoneOwner )
        {
        RArray<CMRMailboxUtilsImpl::TMailboxInfo> mailboxes;
        CleanupClosePushL( mailboxes );
        // TODO: handle aPrimaryBox!
        ListMailBoxesL( mailboxes );        
                
        // Match email addresses to organizer + attendee list addresses:
        TInt dummyIndex( -1 ); // don't care
        phoneOwner = PhoneOwnerL( aCalEntry, mailboxes, dummyIndex );
        
        if ( phoneOwner )
            {
            aCalEntry.SetPhoneOwnerL( phoneOwner );
            }
        else
            {
            retVal = KErrNotFound;
            }

        CleanupStack::PopAndDestroy(); // mailboxes
        }
    return retVal;        
    }
            
// ---------------------------------------------------------
// CMRMailboxUtilsImpl::AddressWithoutMailtoPrefix
// ---------------------------------------------------------
//
TPtrC CMRMailboxUtilsImpl::AddressWithoutMailtoPrefix( const TDesC& aAddress )
	{
	TPtrC addrWithoutPrefix;
    if ( aAddress.MatchF( KMailtoMatchPattern ) != KErrNotFound )
    	{
        addrWithoutPrefix.Set( aAddress.Mid( KMailtoLength ) );
        }
    else
    	{
        addrWithoutPrefix.Set( aAddress );
        }
    return addrWithoutPrefix;
	}
	
// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::PhoneOwnerL
// ----------------------------------------------------------------------------
//   
CCalUser* CMRMailboxUtilsImpl::PhoneOwnerL(
    const CCalEntry& aCalEntry,
    const RArray<CMRMailboxUtilsImpl::TMailboxInfo>& aMailBoxes,
    TInt& aMatchIndex )
    {
    aMatchIndex = -1; // no match yet

    // 1. test if phone owner is organizer:

    CCalUser* organizer = aCalEntry.OrganizerL(); // ownership not transf.
    if ( organizer )
        {
        if ( IsPhoneOwnerL( *organizer, aMailBoxes, aMatchIndex ) )
            {
            return organizer;
            }
        }

    // 2. test if phone owner is attendee:

    RPointerArray<CCalAttendee>& attendees = aCalEntry.AttendeesL();
    TInt attCount( attendees.Count() );
    for ( TInt i( 0 );i < attCount; ++i )
        {            
        CCalUser* att = attendees[i];
        if ( IsPhoneOwnerL( *att, aMailBoxes, aMatchIndex ) )
            {
            return att;
            }        
        }
        
    // Matching attendee was not found:
    return NULL;
    }

// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::IsPhoneOwnerL
// ----------------------------------------------------------------------------
//   
TBool CMRMailboxUtilsImpl::IsPhoneOwnerL(
    const CCalUser& aUser,
    const RArray<CMRMailboxUtilsImpl::TMailboxInfo>& aMailBoxes,
    TInt& aMatchIndex )
    {
    TPtrC addr = AddressWithoutMailtoPrefix( aUser.Address() );
    aMatchIndex = -1; // no match yet
    TInt boxCount( aMailBoxes.Count() );    
    for ( TInt i( 0 ); i < boxCount; ++i )
        {
        TPtrC boxAddr = aMailBoxes[i].iEmailAddress; 
        if ( addr.CompareF( boxAddr ) == 0 )
            {
            // first match is enough:
            aMatchIndex = i;
            return ETrue;
            }
        }
    return EFalse; // no match
    }

// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::AddObserverL
// ----------------------------------------------------------------------------
//  
void CMRMailboxUtilsImpl::AddObserverL( MMRMailboxUtilsObserver& aObserver )
    {
    iObservers.InsertInAddressOrderL( &aObserver );
    }
    
// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::RemoveObserver
// ----------------------------------------------------------------------------
//  
void CMRMailboxUtilsImpl::RemoveObserver( MMRMailboxUtilsObserver& aObserver )
    {
    TInt index = iObservers.FindInAddressOrder( &aObserver );
    if ( index != KErrNotFound )
        {        
        iObservers.Remove( index );
        }
    }
    
// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::MsvSessionL
// ----------------------------------------------------------------------------
//  
CMsvSession& CMRMailboxUtilsImpl::MsvSessionL()
    {
    return iMsvHelper->MsvSessionL();
    }
    
// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::HandleSessionEventL
// ----------------------------------------------------------------------------
//
void CMRMailboxUtilsImpl::HandleSessionEventL(
    TMsvSessionEvent aEvent,
    TAny* aArg1,
    TAny* /*aArg2*/,
    TAny* /*aArg3*/ )
    {
    CMsvEntrySelection* entries = static_cast<CMsvEntrySelection*>( aArg1 );
    switch ( aEvent )
        {        
        case EMsvEntriesCreated:
            {
            NotifyObserversL( MMRMailboxUtilsObserver::EMRMailboxCreated );
            break;
            }
        case EMsvEntriesChanged:
            {
            TMsvId defaultId;
            TInt err = iSettingsMgr->GetDefaultMRMailBoxL( defaultId );
            if ( err == KErrNone &&
                 entries->Find( defaultId ) != KErrNotFound )
                { // default changed
                NotifyObserversL( MMRMailboxUtilsObserver::EMRDefaultChanged );
                }
            else
                {                    
                NotifyObserversL( MMRMailboxUtilsObserver::EMRMailboxChanged );
                }
            break;
            }        
        case EMsvEntriesDeleted:
            {
            TMsvId defaultId;
            TInt err = iSettingsMgr->GetDefaultMRMailBoxL( defaultId );
            if ( err == KErrNone &&
                 entries->Find( defaultId ) != KErrNotFound )
                { // default deleted
                NotifyObserversL( MMRMailboxUtilsObserver::EMRDefaultDeleted );
                }
            else
                {
                NotifyObserversL( MMRMailboxUtilsObserver::EMRMailboxDeleted );
                }
            break;
            }
        default:
            { // not interested in other even types
            break;
            }
        }    
    }

// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::HandleMRMailboxEventL
// ----------------------------------------------------------------------------
//
void CMRMailboxUtilsImpl::HandleMRMailboxEventL( 
        MMRMailboxUtilsObserver::TEventType aType )
    {
    NotifyObserversL( aType );
    }

// ----------------------------------------------------------------------------
// CMRMailboxUtilsImpl::NotifyObserversL
// ----------------------------------------------------------------------------
//
void CMRMailboxUtilsImpl::NotifyObserversL(
    MMRMailboxUtilsObserver::TEventType aEvent )
    {
    TInt count( iObservers.Count() );   
    for ( TInt i( 0 ); i < count; ++i )
        {
        TRAP_IGNORE( iObservers[i]->HandleMRMailboxEventL( aEvent ) );
        }
    }    
    
// End of file
