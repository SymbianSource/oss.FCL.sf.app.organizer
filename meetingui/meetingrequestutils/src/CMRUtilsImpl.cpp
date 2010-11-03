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
* Description: Implementation for meeting request utils   
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CMRUtilsImpl.h"
#include "MRUtilsFactory.h"
#include "CMRUtilsEmailSender.h"
#include "CMRUtilsCalDbMgr.h"
#include "MRHelpers.h"
#include "ICalUILog.h"
#include <CalEntry.h>
#include <CalSession.h>
#include <CalCommon.h>
#include <CalUser.h>
#include <cmrmailboxutils.h>
#include "CMRUtilsUiServices.h"

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

enum TPanicCode
    {
    EPanicSessionSetupAlreadyDone = 1,
    EPanicMandatoryObserverMissing,
    EPanicMsvSessionExpected
    };

_LIT( KPanicMsg, "CMRUtilsImpl" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
    
}  // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CMRUtilsImpl::NewL
// ----------------------------------------------------------------------------
//
CMRUtilsImpl* CMRUtilsImpl::NewL()
	{
	LOG("CMRUtilsImpl::NewL()");
	CMRUtilsImpl* self = new( ELeave ) CMRUtilsImpl();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	LOG("CMRUtilsImpl::NewL() -> End");	
	return self;
	}

// ----------------------------------------------------------------------------
// CMRUtilsImpl::CMRUtilsImpl
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRUtilsImpl::CMRUtilsImpl()
    : iCalEngStatus( MMRUtilsObserver::ENotReady )
    {    
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsImpl::~CMRUtilsImpl
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CMRUtilsImpl::~CMRUtilsImpl()
    {    
    delete iCalDbMgr;
    delete iUiServices;
    delete iMRMailboxUtils;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsImpl::ConstructL
// ----------------------------------------------------------------------------
//    
void CMRUtilsImpl::ConstructL()
    {
    // See also SessionSetupL() for construction phase functionality    
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::EntryView
// ----------------------------------------------------------------------------
//
CCalEntryView* CMRUtilsImpl::EntryView()
    {
    if ( iCalEngStatus == EAvailable ||
         iCalEngStatus == EAvailableWithoutTombs )
        {
        return iCalDbMgr->NormalDbEntryView();
        }
    else
        {
        return NULL;
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::InstanceView
// ----------------------------------------------------------------------------
//
CCalInstanceView* CMRUtilsImpl::InstanceView()
    {
    if ( iCalEngStatus == EAvailable ||
         iCalEngStatus == EAvailableWithoutTombs )
        {
        return iCalDbMgr->NormalDbInstanceView();
        }
    else
        {
        return NULL;
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::FetchEntriesL
// ----------------------------------------------------------------------------
//  
void CMRUtilsImpl::FetchEntriesL(
    RPointerArray<CCalEntry>& aCalEntryArray,
    const CalCommon::TCalTimeRange& aTimeRange )
    {
    LeaveIfEngNotAvailableL();
    iCalDbMgr->FetchEntriesL( aCalEntryArray, aTimeRange );
    } 
                                 
// ----------------------------------------------------------------------------
// CMRUtilsImpl::FetchEntryL
// ----------------------------------------------------------------------------
//  
CCalEntry* CMRUtilsImpl::FetchEntryL(
    const TDesC8& aUid,
    const TCalTime& aRecurrenceId )
    {
    LeaveIfEngNotAvailableL();
    return iCalDbMgr->FetchEntryL( aUid, aRecurrenceId );
    }        

// ----------------------------------------------------------------------------
// CMRUtilsImpl::UpdateEntryL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsImpl::UpdateEntryL( const CCalEntry& aCalEntry )
    {
    LeaveIfEngNotAvailableL();
    return iCalDbMgr->UpdateEntryL( aCalEntry ); 
    }        

// ----------------------------------------------------------------------------
// CMRUtilsImpl::ResetTombsFileL
// ----------------------------------------------------------------------------
//          
void CMRUtilsImpl::ResetTombsFileL()
    {
    LeaveIfEngNotAvailableL();
    iCalDbMgr->ResetTombsFileL();
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsImpl::DeleteTombstonesL
// ----------------------------------------------------------------------------
//          
void CMRUtilsImpl::DeleteTombstonesL(
    const CalCommon::TCalTimeRange& aCalTimeRange )
    {
    LeaveIfEngNotAvailableL();
    iCalDbMgr->DeleteTombstonesL( aCalTimeRange );
    }
      
// ----------------------------------------------------------------------------
// CMRUtilsImpl::StoreL
// ----------------------------------------------------------------------------
//          
MMRUtilsTombsExt::TMRUtilsDbResult CMRUtilsImpl::StoreL(
    CCalEntry& aCalEntry,
    TBool aResurrect )
    {
    LeaveIfEngNotAvailableL();          
    return iCalDbMgr->StoreEntryCondL( aCalEntry, aResurrect );
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsImpl::CheckEntryCondL
// ----------------------------------------------------------------------------
//          
MMRUtilsTombsExt::TMRUtilsDbResult CMRUtilsImpl::CheckEntryCondL(
    const CCalEntry& aCalEntry )
    {
    LeaveIfEngNotAvailableL();
    // We want to keep const in CMRUtil API, but CalenInterimApi::StoreL()
    // takes non-const CCalEntry reference. With this method it is still
    // safe to cast away constness since in this case we are only performing
    // a check, not storing actually.
    CCalEntry& entry = const_cast<CCalEntry&>( aCalEntry );
    return iCalDbMgr->StoreEntryCondL( entry, EFalse, ETrue );
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::DeleteL
// ----------------------------------------------------------------------------
//          
void CMRUtilsImpl::DeleteL( const TDesC8& aUid )
    {
    LeaveIfEngNotAvailableL();
    iCalDbMgr->DeleteEntryCondL( aUid );
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::DeleteL
// ----------------------------------------------------------------------------
//          
void CMRUtilsImpl::DeleteL( const TCalLocalUid& aLocalUid )
    {
    LeaveIfEngNotAvailableL();
    iCalDbMgr->DeleteEntryCondL( aLocalUid );
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::DeleteL
// ----------------------------------------------------------------------------
//          
void CMRUtilsImpl::DeleteL( const CCalEntry& aCalEntry )
    {
    LeaveIfEngNotAvailableL();
    iCalDbMgr->DeleteEntryCondL( aCalEntry );
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::DeleteL
// ----------------------------------------------------------------------------
//          
void CMRUtilsImpl::DeleteL(
    const CalCommon::TCalTimeRange& aCalTimeRange )
    {
    LeaveIfEngNotAvailableL();
    iCalDbMgr->DeleteEntryCondL( aCalTimeRange );    
    }
   
// ----------------------------------------------------------------------------
// CMRUtilsImpl::DeleteWithUiL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsImpl::DeleteWithUiL( const CCalEntry& aEntry, TMsvId aMailbox )
    {
    LeaveIfEngNotAvailableL();
    LeaveIfUiExtNotAvailableL();
    return iUiServices->DeleteWithUiL( aEntry, aMailbox );
    }
   
// ----------------------------------------------------------------------------
// CMRUtilsImpl::DeleteWithUiL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsImpl::DeleteWithUiL( CCalInstance* aInstance, TMsvId aMailbox )
    {
    LeaveIfEngNotAvailableL();
    LeaveIfUiExtNotAvailableL();
    return iUiServices->DeleteWithUiL( aInstance, aMailbox );
    }
        
// ----------------------------------------------------------------------------
// CMRUtilsImpl::SendWithUiL
// ----------------------------------------------------------------------------
//  
void CMRUtilsImpl::SendWithUiL( const CCalEntry& aEntry, TMsvId aMailbox )
    {
    LeaveIfEngNotAvailableL();
    LeaveIfUiExtNotAvailableL();
    iUiServices->SendL( aEntry, aMailbox );
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::RespondWithUiL
// ----------------------------------------------------------------------------
//                     
TInt CMRUtilsImpl::RespondWithUiL(
    const RPointerArray<CCalEntry>& aEntries,
    CCalAttendee::TCalStatus aStatus,
    TMsvId aMailbox )
    {
    LeaveIfEngNotAvailableL();
    LeaveIfUiExtNotAvailableL();
    return iUiServices->RespondWithUiL( aEntries, aStatus, aMailbox );    
    }
                    
// ----------------------------------------------------------------------------
// CMRUtilsImpl::CancelWithUiL
// ----------------------------------------------------------------------------
// 
TInt CMRUtilsImpl::CancelWithUiL(
    const RPointerArray<CCalEntry>& aEntries,
    TMsvId aMailbox )
    {
    LeaveIfEngNotAvailableL();
    LeaveIfUiExtNotAvailableL();
    return iUiServices->CancelWithUiL( aEntries, aMailbox );    
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::ReplyToL
// ----------------------------------------------------------------------------
// 
void CMRUtilsImpl::ReplyToL(
    TMailRecipients aRecipients,
    const CCalEntry& aCalEntry,
    const TDesC& aSenderAddr,
    TMsvId aMailbox )
    {
    LeaveIfEngNotAvailableL();
    LeaveIfUiExtNotAvailableL();
    iUiServices->ReplyToL( aRecipients, aCalEntry, aSenderAddr, aMailbox );    
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::MailHeaderFromFieldLC
// ----------------------------------------------------------------------------
// 
HBufC* CMRUtilsImpl::MailHeaderFromFieldLC( TMsvId aMessage )
    {
    HBufC* fromField = NULL;
    if ( !iMsvSession )
        { // This method should only be called in case of opening from Mail,
          // -> valid MsvSession given by the client.
        __ASSERT_DEBUG( EFalse, Panic( EPanicMsvSessionExpected ) );
        User::Leave( KErrNotSupported );
        }
    else
        {
        TMsvId dummy( KMsvNullIndexEntryId );
        TMsvEntry entry;
        User::LeaveIfError( iMsvSession->GetEntry( aMessage, dummy, entry ) );        
        fromField = entry.iDetails.AllocLC();
        }
    return fromField;
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::MailHeaderSubjectFieldLC
// ----------------------------------------------------------------------------
// 
HBufC* CMRUtilsImpl::MailHeaderSubjectFieldLC( TMsvId aMessage )
    {
    HBufC* subjectField = NULL;
    if ( !iMsvSession )
        { // This method should only be called in case of opening from Mail,
          // -> valid MsvSession given by the client.
        __ASSERT_DEBUG( EFalse, Panic( EPanicMsvSessionExpected ) );
        User::Leave( KErrNotSupported );
        }
    else
        {
        TMsvId dummy( KMsvNullIndexEntryId );
        TMsvEntry entry;
        User::LeaveIfError( iMsvSession->GetEntry( aMessage, dummy, entry ) );        
        subjectField = entry.iDescription.AllocLC();
        }
    return subjectField;    
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsImpl::ExtensionL
// ----------------------------------------------------------------------------
//
TAny* CMRUtilsImpl::ExtensionL( TUid aExtensionUid )
    {
    if ( aExtensionUid == MMRUtilsTombsExt::ExtensionUid() )
        {
        return static_cast<MMRUtilsTombsExt*>( this );
        }
    else
        {        
        return NULL;
        }
    }
      
// ----------------------------------------------------------------------------
// CMRUtilsImpl::SessionSetupL
// This template method is called as a part of the ECom plugin construction.
// This replaces much of the ordinary ConstructL() functionality, a template
// method is easier for transmitting parameters.
// ----------------------------------------------------------------------------
//          
void CMRUtilsImpl::SessionSetupL( 
    CCalSession& aCalSession, 
    CMsvSession* aMsvSession )
    {                    
    LOG("CMRUtilsImpl::SessionSetupL()");
    iCalSession = &aCalSession;
    iMsvSession = aMsvSession;   
    LOG("CMRUtilsImpl::SessionSetupL(), creating mailbox utils");
    iMRMailboxUtils = CMRMailboxUtils::NewL( aMsvSession );    
    // This will remain null if no ui environment is available
    LOG("CMRUtilsImpl::SessionSetupL(), creating ui services");    
    iUiServices = MRUtilsFactory::CreateUiServicesL( *this,
                                                     *iCalSession,
                                                     iMsvSession,
                                                     *iMRMailboxUtils );    
    // Asynchronous initialization of CMRUtilsCalDbMgr:     
    LOG("CMRUtilsImpl::SessionSetupL(), creating db mgr");           
    iCalDbMgr = MRUtilsFactory::CreateCalDbMgrL( *iCalSession, *this );    
    LOG("CMRUtilsImpl::SessionSetupL() -> End");    
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::HandleCalEngStatus
// ----------------------------------------------------------------------------
//    
void CMRUtilsImpl::HandleCalEngStatus( TMRUtilsCalEngStatus aStatus )
    {
    iCalEngStatus = aStatus;
    __ASSERT_DEBUG( iObserver, EPanicMandatoryObserverMissing );
    iObserver->HandleCalEngStatus( iCalEngStatus );
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsImpl::HandleOperation
// ----------------------------------------------------------------------------
//    
void CMRUtilsImpl::HandleOperation(
    TInt aType,
    TInt aPercentageCompleted, 
    TInt aStatus )
    {
    iObserver->HandleOperation( aType, aPercentageCompleted, aStatus );
    }    

// ----------------------------------------------------------------------------
// CMRUtilsImpl::LeaveIfEngNotAvailableL
// ----------------------------------------------------------------------------
//
void CMRUtilsImpl::LeaveIfEngNotAvailableL() const
    {
    if ( iCalEngStatus == MMRUtilsObserver::ENotReady )
        {
        User::Leave( KErrNotReady );
        }
    else if ( iCalEngStatus == MMRUtilsObserver::ENotAvailable )
        {
        User::Leave( KErrCouldNotConnect );
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsImpl::LeaveIfEngNotAvailableL
// ----------------------------------------------------------------------------
//
void CMRUtilsImpl::LeaveIfUiExtNotAvailableL() const
    {
    if ( !iUiServices )
        {
        User::Leave( KErrNotSupported );
        }
    }
      
// End of file
