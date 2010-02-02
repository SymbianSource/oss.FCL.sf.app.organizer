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
* Description:  Provides Class methods for handling message entries and using CMsvSession
*
*/


// INCLUDES
#include "cmrutilsmsvhelper.h"
#include "cmrmailboxinfo.h"
#include <SenduiMtmUids.h>
#include <cemailaccounts.h>
#include <smtpset.h>
#include <imapset.h>
#include <pop3set.h>
#include <MuiuMsvUiServiceUtilities.h>
#include <MeetingRequestIds.hrh>
#include <mtclbase.h>
#include <mtclreg.h>
#include <utf.h>

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::NewL
// Symbian two-phased constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CMRUtilsMsvHelper* CMRUtilsMsvHelper::NewL(
    CMsvSession* aSession,
    MMsvSessionObserver& aParentObserver )
	{
	CMRUtilsMsvHelper* self =
	    new( ELeave ) CMRUtilsMsvHelper( aSession, aParentObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::CMRUtilsMsvHelper
// Default constructor
// ----------------------------------------------------------------------------
//
CMRUtilsMsvHelper::CMRUtilsMsvHelper(
    CMsvSession* aSession,
    MMsvSessionObserver& aParentObserver )
    : iParentObserver( aParentObserver ),
      iSession( aSession ),
      iOwnSession( aSession ? EFalse : ETrue ),
      iEmailAddr(NULL)
    {
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::~CMRUtilsMsvHelper
// Destructor.
// ----------------------------------------------------------------------------
//
CMRUtilsMsvHelper::~CMRUtilsMsvHelper()
    {
    iMRMailBoxes.ResetAndDestroy();
    delete iMtmReg;
    if ( iOwnSession )
        {
        if(iSession)
        {
           delete iSession; 
        }
        }
    else if ( iSession )
        { // just remove observer
        iSession->RemoveObserver( *this );
        }
    delete iEmailSettings;
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::ConstructL
// Symbian OS default constructor
// ----------------------------------------------------------------------------
//
void CMRUtilsMsvHelper::ConstructL()
    {
    if ( iSession )
        { // if existing session was given as construction parameter,
          // then add observer manually
        iSession->AddObserverL( *this );
        }   
    // Create session if doesn't exist yet: 
    EnsureSessionExistsL();
    
    FindMRMailBoxesL();
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::MRMailBoxes
// returns array of mailbox.
// ----------------------------------------------------------------------------
//
const RPointerArray<CMRMailBoxInfo>& CMRUtilsMsvHelper::MRMailBoxes() const
    {
    return iMRMailBoxes;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::MRMailBoxInfoL
// returns mailbox info
// ----------------------------------------------------------------------------
//
const CMRMailBoxInfo* CMRUtilsMsvHelper::MRMailBoxInfoL(
    TInt aEntryId ) const
    {
    CMRMailBoxInfo* info( NULL );
    TInt count( iMRMailBoxes.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( iMRMailBoxes[i]->EntryId() == aEntryId )
            {
            info = iMRMailBoxes[i];
            break;
            }
        }
    return info;
    }
    
// TODO: siirrä tämä utilsimpliin
// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::ListMRMailBoxesL
// lists all the mailboxes available.
// ----------------------------------------------------------------------------
//
void CMRUtilsMsvHelper::ListMRMailBoxesL(
    RArray<CMRMailboxUtils::TMailboxInfo>& aMailBoxes )
    {
    EnsureSessionExistsL();
    aMailBoxes.Reset();

    TInt count( iMRMailBoxes.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        CMRMailboxUtils::TMailboxInfo info;
        info.iName.Set( iMRMailBoxes[i]->Name() );
        info.iEmailAddress.Set( iMRMailBoxes[i]->Address() );
        info.iMtmUid = iMRMailBoxes[i]->MtmUid();
        info.iEntryId = iMRMailBoxes[i]->EntryId();
        aMailBoxes.AppendL( info );
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::ExtensionL
// extension API.
// ----------------------------------------------------------------------------
//
TAny* CMRUtilsMsvHelper::ExtensionL( TUid /*aExtensionUid*/ )
    {
	return NULL;
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::MsvSessionL
// returns session.
// ----------------------------------------------------------------------------
//
CMsvSession& CMRUtilsMsvHelper::MsvSessionL()
    {
    EnsureSessionExistsL();
    return *iSession;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::EMailAddressL 
// ----------------------------------------------------------------------------
//
//TBuf<KMREmailAddrLength> CMRUtilsMsvHelper::EMailAddressL( TMsvEntry& entry )
void  CMRUtilsMsvHelper::EMailAddressL( TMsvEntry& entry )
    {
    EnsureSessionExistsL();
   // TBuf<KMREmailAddrLength> emailAddr;
    
    if ( entry.iMtm.iUid == KSenduiMtmImap4UidValue ||
         entry.iMtm.iUid == KSenduiMtmPop3UidValue ||
         entry.iMtm.iUid == KSenduiMtmSyncMLEmailUidValue )
        {        
        // Read e-mail address from SMTP settings:
        TMsvId smtpId = CorrespondingSmtpServiceL( entry );
        CImSmtpSettings* smtpSettings = SmtpSettingsLC( smtpId );
       iEmailAddr = smtpSettings->EmailAddress();
        CleanupStack::PopAndDestroy( smtpSettings );
        }
    else if ( IsMRCapableL( entry.iMtm ) )
        { // 3rd party mtm types -> query from client mtm
        CBaseMtm* mtm = iMtmReg->NewMtmL( entry.iMtm );
        CleanupStack::PushL( mtm );
        CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection();
        CleanupStack::PushL( selection );
        selection->AppendL( entry.Id() );
        RBuf8 addrData;
        CleanupClosePushL( addrData );
        addrData.CreateL( KMREmailAddrLength );        
        mtm->InvokeSyncFunctionL( KMTMMailAddressRequestFunctionId,
                                  *selection,
                                  addrData );
        if ( addrData.Length() > 0 )
            {
            User::LeaveIfError( CnvUtfConverter::ConvertToUnicodeFromUtf8(
                iEmailAddr, addrData ) );
            }        
        CleanupStack::PopAndDestroy( 3 ); // addrData, selection, mtm
        }
    else
        { // we can't resolve email address for other types, and that's fine
        User::Leave( KErrNotSupported );
        }

   // return emailAddr;
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::CorrespondingSmtpServiceL
// returns the smtp id.
// ----------------------------------------------------------------------------
//
TMsvId CMRUtilsMsvHelper::CorrespondingSmtpServiceL( TMsvEntry& entry ) const
    {
    TMsvId smtpId( KMsvNullIndexEntryId );

    switch ( entry.iMtm.iUid )
        {
        case KSenduiMtmImap4UidValue: // flow through
        case KSenduiMtmPop3UidValue:
            {
            // In these cases smtp entry is available in iRelatedId:
            smtpId = entry.iRelatedId;
            break;
            }
        case KSenduiMtmSmtpUidValue:
        case KSenduiMtmSyncMLEmailUidValue:
            {
            // In these cases we already know the msvid for the smtp settings
            // (for syncml there is also smtp settings!):
            smtpId = entry.Id();
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    return smtpId;
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::SmtpSettingsLC
// returns smtp settings.
// ----------------------------------------------------------------------------
//
CImSmtpSettings* CMRUtilsMsvHelper::SmtpSettingsLC( TMsvId aSmtpService )
    {
    
// TODO: onko tässä lazyssa järkeä?    
    if ( !iEmailSettings )
        { // Lazy instantiation of iEmailSettings
        iEmailSettings = CEmailAccounts::NewL();
        }
        
    TSmtpAccount smtpAccount;
    iEmailSettings->GetSmtpAccountL( aSmtpService, smtpAccount );
    CImSmtpSettings* smtpSettings = new( ELeave ) CImSmtpSettings();
    CleanupStack::PushL( smtpSettings );
    iEmailSettings->LoadSmtpSettingsL( smtpAccount, *smtpSettings );
    return smtpSettings;
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::IsMRCapableL
// returns ETrue if MRCapable, EFalse otherwise.
// ----------------------------------------------------------------------------
//

TBool CMRUtilsMsvHelper::IsMRCapableL( TUid aMtmUid )
    {
    TBool retVal( EFalse );
    if ( aMtmUid.iUid == KSenduiMtmImap4UidValue ||
         aMtmUid.iUid == KSenduiMtmPop3UidValue ||
         aMtmUid.iUid == KSenduiMtmSyncMLEmailUidValue )
        {
        retVal = ETrue;
        }
    else if ( aMtmUid.iUid == KSenduiMtmSmtpUidValue )
        { // don't consider smtp's here, corresponding imap/pop boxes only
        retVal = EFalse;
        }
    else
        {        
        // we can create and destroy mtm without performance loss since registry
        // unloads them only if unused for a period of time
        CBaseMtm* mtm = iMtmReg->NewMtmL( aMtmUid );
        CleanupStack::PushL( mtm );
        TInt dummyResponse( 0 );
        TInt result( mtm->QueryCapability(
            TUid::Uid( KMtmUiSupportMeetingRequest ), dummyResponse ) );
        CleanupStack::PopAndDestroy( mtm );        
        retVal = ( ( result == KErrNone ) ? ETrue : EFalse );
        }
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::EnsureSessionExistsL
// ensures the session existense, creates if not exists.
// ----------------------------------------------------------------------------
//
void CMRUtilsMsvHelper::EnsureSessionExistsL()
    {
    if ( !iSession )
        {
        iSession = CMsvSession::OpenSyncL( *this );
        iOwnSession = ETrue;
        }
    if ( !iMtmReg )
        {
        iMtmReg = CClientMtmRegistry::NewL( *iSession );        
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::HandleSessionEventL
// ----------------------------------------------------------------------------
//
void CMRUtilsMsvHelper::HandleSessionEventL(
    TMsvSessionEvent aEvent,
    TAny* aArg1,
    TAny* aArg2,
    TAny* aArg3 )
    {
    switch ( aEvent )
        {
        case EMsvCloseSession:
        case EMsvServerTerminated:
            {
            delete iMtmReg;
            iMtmReg = NULL;
            
            if ( iOwnSession )
                {
                delete iSession;
                }
            iSession = NULL; // New session constructed only if needed
            break;
            }
        case EMsvEntriesCreated:
            {
            CMsvEntrySelection* entries =
                static_cast<CMsvEntrySelection*>( aArg1 );            
            if ( AddToListIfMRMailBoxesL( *entries ) )
                { // at least MR mailbox created
                iParentObserver.HandleSessionEventL( aEvent,
                                                     aArg1,
                                                     aArg2,
                                                     aArg3 );                
                }
            break;
            }            
        case EMsvEntriesChanged:        
        case EMsvEntriesDeleted:        
            {
            CMsvEntrySelection* entries =
                static_cast<CMsvEntrySelection*>( aArg1 );        
            if ( ContainsMRMailBoxEntryL( *entries ) )
                {
                // Note: in case of entry deletion it is important that
                // iMRMailBoxes array is refreshed only after evaluating
                // ContainsMRMailBoxEntryL()
                FindMRMailBoxesL();                
                iParentObserver.HandleSessionEventL( aEvent,
                                                     aArg1,
                                                     aArg2,
                                                     aArg3 );
                }
            break;
            }            
        default:
            {
            // ignore other events
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::ContainsMRMailBoxEntryL
// ----------------------------------------------------------------------------
//
TBool CMRUtilsMsvHelper::ContainsMRMailBoxEntryL(
    const CMsvEntrySelection& aEntries ) const
    {
    TBool retVal( EFalse );
    
    TInt count( iMRMailBoxes.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( aEntries.Find( iMRMailBoxes[i]->EntryId() ) != KErrNotFound )
            {
            retVal = ETrue;
            break;
            }
        }
    return retVal;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::FindMRMailBoxesL
// ----------------------------------------------------------------------------
//
void CMRUtilsMsvHelper::FindMRMailBoxesL()
    {    
    EnsureSessionExistsL();    
    iMRMailBoxes.ResetAndDestroy();
    // list also hidden (3rd party) accounts -> 2nd parameter == ETrue
    CMsvEntrySelection* allMailBoxes =
        MsvUiServiceUtilities::GetListOfAccountsL( *iSession, ETrue );
    CleanupStack::PushL( allMailBoxes );
    AddToListIfMRMailBoxesL( *allMailBoxes );
    CleanupStack::PopAndDestroy( allMailBoxes );
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::AddToListIfMRMailBoxesL
// ----------------------------------------------------------------------------
//
TBool CMRUtilsMsvHelper::AddToListIfMRMailBoxesL(
    const CMsvEntrySelection& aEntries )
    {
    TBool retVal( EFalse );
    TInt count( aEntries.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        TBool result( EFalse );
        // don't care if individual mailboxes cause some problems
        TRAP_IGNORE( result = AddToListIfMRMailBoxL( aEntries.At( i ) ) );
        if ( result )
            { // retVal can only go from EFalse to ETrue, not other way
            retVal = ETrue;
            }
        }
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRUtilsMsvHelper::AddToListIfMRMailBoxL
// ----------------------------------------------------------------------------
//
TBool CMRUtilsMsvHelper::AddToListIfMRMailBoxL( TMsvId aEntryId )
    {    
    TBool retVal( EFalse );
    EnsureSessionExistsL();
    TMsvEntry entry;
    TMsvId dummyService;
    
    CMRMailBoxInfo* info( NULL );
    TInt result = iSession->GetEntry( aEntryId, dummyService, entry );    

    if ( result == KErrNone &&
         entry.iType == KUidMsvServiceEntry &&        
         IsMRCapableL( entry.iMtm ) )
        {        
         EMailAddressL( entry ) ;
        if ( MsvUiServiceUtilities::IsValidEmailAddressL( iEmailAddr ) )
            {            
            info = CMRMailBoxInfo::NewL( entry.iDetails,
                                         iEmailAddr,
                                         entry.iMtm,
                                         aEntryId );    
            CleanupStack::PushL( info );
            iMRMailBoxes.AppendL( info ); // ownership transferred
            retVal = ETrue;
            CleanupStack::Pop( info );
            }
        }
        
    return retVal;
    }

    
// End of file
