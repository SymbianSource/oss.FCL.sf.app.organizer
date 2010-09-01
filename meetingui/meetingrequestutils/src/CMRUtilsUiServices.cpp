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
* Description: Implementation for meeting request utils ui services  
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CMRUtilsUiServices.h"
#include "CMRUtilsInternal.h"
#include "CMREditBeforeSendingViewForm.h"
#include "CMRUtilsCalDbBase.h"
#include "MREntryConsultant.h"
#include "meetingrequestutils.hrh"
#include "MRUtilsFactory.h"
#include "CMRUtilsEmailSender.h"
#include <CalSession.h>
#include <CalEntry.h>
#include <CalInstance.h>
#include <CalInstanceView.h>
#include <CalCommon.h>
#include <CalUser.h>
#include <avkon.hrh>
#include <aknlistquerydialog.h>
#include <meetingrequestutilsuires.rsg>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <data_caging_path_literals.hrh>
#include <cmrmailboxutils.h>
#include <msvstd.h>
#include <SendUiConsts.h>
#include "ICalUILog.h"

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

const TInt KDescriptionLength = 700;

_LIT( KResourceFile,"meetingrequestutilsuires.rsc" );

enum TPanicCode
    {
    EPanicEmptyArray = 1,
    };

_LIT( KPanicMsg, "CMRUtilsUiServices" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
    
    const TInt KResourcePathMaxLen = 30;
    
}  // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::NewL
// ----------------------------------------------------------------------------
//
CMRUtilsUiServices* CMRUtilsUiServices::NewL(
    CMRUtilsInternal& aMRUtils,
    CCalSession& aCalSession,
    CMsvSession* aMsvSession,
    CMRMailboxUtils& aMRMailboxUtils )
	{
	
	LOG("CMRUtilsUiServices NewL");
	CMRUtilsUiServices* self =
	    new( ELeave ) CMRUtilsUiServices( aMRUtils,
	                                      aMsvSession, 
	                                      aMRMailboxUtils );
	CleanupStack::PushL( self );
	self->ConstructL( aCalSession );
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::CMRUtilsUiServices
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRUtilsUiServices::CMRUtilsUiServices( 
    CMRUtilsInternal& aMRUtils,
    CMsvSession* aMsvSession,
    CMRMailboxUtils& aMRMailboxUtils )
    : iOwnMsvSession( aMsvSession ? EFalse : ETrue ),
      iMRUtils( aMRUtils ),
      iMRMailboxUtils( aMRMailboxUtils ),
      iMsvSession( aMsvSession )
    {    
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsUiServices::~CMRUtilsUiServices
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CMRUtilsUiServices::~CMRUtilsUiServices()
    {   
    delete iEmailSender;
    
    if ( iOwnMsvSession )
        {
        delete iMsvSession;
        }    
    
    if ( iResourceFileOffset )
    	{
        CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
    	}    
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsUiServices::ConstructL
// ----------------------------------------------------------------------------
//    
void CMRUtilsUiServices::ConstructL( CCalSession& aCalSession )
    {
    LOG("CMRUtilsUiServices ConstructL: loading resopurces");
    iResourceFileOffset = MRHelpers::LoadResourceL( KResourceFile,
                                                    KDC_RESOURCE_FILES_DIR );
    LOG("CMRUtilsUiServices ConstructL: creating emailsender");
    iEmailSender = MRUtilsFactory::CreateEmailSenderL( aCalSession );
    
    if ( iMsvSession )
        { // if existing session was given as construction parameter,
          // then add observer manually
        iMsvSession->AddObserverL( *this );
        }    
        
    LOG("CMRUtilsUiServices ConstructL finished");        
    }

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::ReplyToL
// ----------------------------------------------------------------------------
//          
void CMRUtilsUiServices::ReplyToL(
    CMRUtilsInternal::TMailRecipients aRecipients,
    const CCalEntry& aCalEntry,
    const TDesC& aSenderAddr,
    TMsvId aMailbox )
    {
    iEmailSender->ReplyToL( aRecipients, aCalEntry, aMailbox, aSenderAddr );
    }        
 
// ----------------------------------------------------------------------------
// CMRUtilsUiServices::SendL
// ----------------------------------------------------------------------------
//          
void CMRUtilsUiServices::SendL(
    const CCalEntry& aCalEntry,
    TMsvId aMailbox )
    {  
    iEmailSender->SendL( aCalEntry, CorrespondingSmtpServiceL( aMailbox) );
    }
        
// ----------------------------------------------------------------------------
// CMRUtilsUiServices::DeleteWithUiL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsUiServices::DeleteWithUiL(
    const CCalEntry& aEntry,
    TMsvId aMailbox )
    {
    TInt retVal( KErrNone );    
    // try to cancel/respond if necessary, don't care if fails since deletion
    // is the important part of this method
    TRAP_IGNORE( PerformDeleteSubOpL( aEntry, NULL, aMailbox ) );
    // if entry is originating, then also modifying entries will get deleted:    
    iMRUtils.DeleteL( aEntry );        
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::DeleteWithUiL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsUiServices::DeleteWithUiL(
    CCalInstance* aInstance,
    TMsvId aMailbox )
    {
    TInt retVal( KErrNone );    
    
    if ( !aInstance )
        {
        User::Leave( KErrArgument );
        }    
    // try to cancel/respond if necessary, don't care if fails since deletion
    // is the important part of this method
    TRAP_IGNORE( PerformDeleteSubOpL( aInstance->Entry(),
                                      aInstance,
                                      aMailbox ) );
    // delete instance
    iMRUtils.InstanceView()->DeleteL( aInstance, CalCommon::EThisOnly );
    return retVal;        
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsUiServices::PerformDeleteSubOpL
// ----------------------------------------------------------------------------
//          
void CMRUtilsUiServices::PerformDeleteSubOpL(
    const CCalEntry& aEntry,
    const CCalInstance* aInstance,
    TMsvId aMailbox )
    {
    // create arrays since QueryAndSendL needs them
    RPointerArray<CCalEntry> entryInArray( 1 );
    CleanupClosePushL( entryInArray ); // does not own
    entryInArray.AppendL( &aEntry );    
    RArray<TInt> subOpIndexes( 1 );
	CleanupClosePushL( subOpIndexes );

    if ( IsValidForCancelL( aEntry ) )
        { // cancellation can be sent           
        subOpIndexes.AppendL( 0 );
        QueryAndSendL( entryInArray,
                       subOpIndexes,
                       aInstance,
                       aMailbox,
                       ETrue );
        }
    else if ( IsValidForResponseL( aEntry ) )
        { // response with declined status can be sent
        subOpIndexes.AppendL( 0 );
        QueryAndSendL( entryInArray,
                       subOpIndexes,
                       aInstance,
                       aMailbox,
                       EFalse,
                       CCalAttendee::EDeclined );
        }
    CleanupStack::PopAndDestroy( 2 ); // subOpIndexes, entryInArray
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsUiServices::CancelWithUiL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsUiServices::CancelWithUiL(
    const RPointerArray<CCalEntry>& aEntries,
    TMsvId aMailbox )
    {
    TInt retVal( KErrNone );
	TInt count( aEntries.Count() );
	RArray<TInt> cancelIndexes( count+1 );	// +1 to tolerate count == 0 case
	CleanupClosePushL( cancelIndexes );

    // evaluate which entries can be cancelled, update status in database
    EvaluateAndUpdateL( aEntries, cancelIndexes, ETrue );
    
    TInt cancelCount( cancelIndexes.Count() );
    if ( cancelCount == 0 )
        { // If there are no entries to cancel to we can return
        retVal = KErrArgument;
        }
    else
        { // query user for sending options and send if user wants
        retVal = QueryAndSendL( aEntries,
                                cancelIndexes,
                                NULL,
                                aMailbox,
                                ETrue );
        }
    CleanupStack::PopAndDestroy(); // cancelIndexes
    return retVal;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsUiServices::RespondWithUiL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsUiServices::RespondWithUiL(
    const RPointerArray<CCalEntry>& aEntries,
    CCalAttendee::TCalStatus aStatus,
    TMsvId aMailbox )
    {
    TInt retVal( KErrNone );
	TInt count( aEntries.Count() );
	RArray<TInt> respondIndexes( count+1 );	// +1 to tolerate count == 0 case
	CleanupClosePushL( respondIndexes );
	
    // evaluate which entries can be responded to, update status in database
    EvaluateAndUpdateL( aEntries, respondIndexes, EFalse, aStatus );
    
    TInt respondCount( respondIndexes.Count() );
    if ( respondCount == 0 )
        { // If there are no entries to respond to to we can return
        retVal = KErrArgument;
        }        
    else
        { // query user for sending options and send if user wants
        retVal = QueryAndSendL( aEntries,
                                respondIndexes,
                                NULL,
                                aMailbox,
                                EFalse,
                                aStatus );
        }
    CleanupStack::PopAndDestroy(); // respondIndexes
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::EvaluateAndUpdateL
// ----------------------------------------------------------------------------
//          
void CMRUtilsUiServices::EvaluateAndUpdateL(
    const RPointerArray<CCalEntry>& aEntries,
    RArray<TInt>& aValidIndexes,
    TBool aIsCancellation,
    CCalAttendee::TCalStatus aStatus )
    {
	TInt count( aEntries.Count() );
	// Go through all entries in the array, and update status of those requests
	// that we should cancel/respond. Store indexes of those requests.
    for ( TInt i( 0 ); i < count; ++i )
        {
        CCalEntry& entry = *( aEntries[i] );
        TBool isValid = aIsCancellation ? IsValidForCancelL( entry ) :
                                          IsValidForResponseL( entry );
        if ( isValid )
            {
            UpdateDbStatusL( entry, aIsCancellation, aStatus );
            aValidIndexes.AppendL( i );
            }
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::IsValidForCancelL
// ----------------------------------------------------------------------------
// 
TBool CMRUtilsUiServices::IsValidForCancelL( const CCalEntry& aEntry )
    {
        // entry must have been sent, otherwise cancellation won't be sent
    return ( IsValidRequestL( aEntry ) &&
             iMRMailboxUtils.IsOrganizerL( aEntry ) &&
             MREntryConsultant::IsSentL( aEntry ) );

    }
       
// ----------------------------------------------------------------------------
// CMRUtilsUiServices::IsValidForResponseL
// ----------------------------------------------------------------------------
//   
TBool CMRUtilsUiServices::IsValidForResponseL( const CCalEntry& aEntry )
    {
    CCalAttendee* thisAttendee = iMRMailboxUtils.ThisAttendeeL( aEntry );
    return ( IsValidRequestL( aEntry ) && thisAttendee );
    }
        
// ----------------------------------------------------------------------------
// CMRUtilsUiServices::IsValidRequestL
// ----------------------------------------------------------------------------
//  
TBool CMRUtilsUiServices::IsValidRequestL(
    const CCalEntry& aEntry ) const
    {
    TBool retVal( EFalse );
    if ( !MREntryConsultant::IsCancelledL( aEntry, iMRMailboxUtils ) &&
         !MREntryConsultant::IsEntryOutOfDateL( aEntry ) )
        {
        retVal= ETrue;
        }
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::UpdateDbStatusL
// ----------------------------------------------------------------------------
//   
void CMRUtilsUiServices::UpdateDbStatusL(
    const CCalEntry& aEntry,
    TBool aIsCancellation,
    CCalAttendee::TCalStatus aStatus ) const
    {
	CCalEntry* dbEntry = iMRUtils.FetchEntryL( aEntry.UidL(),
	                                           aEntry.RecurrenceIdL() );
	if ( dbEntry )
		{
	    CleanupStack::PushL( dbEntry );	    
	    if ( aIsCancellation )
	        { // we have already in evaluation phase ensured that phone
	          // owner is organizer
	        dbEntry->SetStatusL( CCalEntry::ECancelled );
	        iMRUtils.UpdateEntryL( *dbEntry );
	        }	    
	    else
	        {	        
    		CCalAttendee* thisAttendee =
    		    iMRMailboxUtils.ThisAttendeeL( *dbEntry );
    		if ( thisAttendee )
    		    { // this should be found since we found it in evaluation phase
        		thisAttendee->SetStatusL( aStatus );
        		iMRUtils.UpdateEntryL( *dbEntry );
        		}
            else
                { // bad entry given
                User::Leave( KErrArgument );
                }
	        }
        CleanupStack::PopAndDestroy( dbEntry );
		}
	else
		{ // database entry not found
		User::Leave( KErrNotFound );
		}
    }

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::QueryAndSendL
// ----------------------------------------------------------------------------
//          
TInt CMRUtilsUiServices::QueryAndSendL(
    const RPointerArray<CCalEntry>& aEntries,
    const RArray<TInt>& aValidIndexes,
    const CCalInstance* aInstance,
    TMsvId aMailbox,
    TBool aIsCancellation,
    CCalAttendee::TCalStatus aStatus )
    {
    // caller should ensure that there is at least one entry in array
    __ASSERT_DEBUG( aValidIndexes.Count() > 0, Panic( EPanicEmptyArray ) );
    
    TInt retVal( KErrNone );
    
    // we will use the first entry in the array of entries to be cancelled
    // or responded  as a base entry (it doesn't really matter which one).
    const CCalEntry& base = *( aEntries[aValidIndexes[0]] );
    
    TInt sendChoice( QuerySendChoiceL( base, aIsCancellation, aStatus ) );
    HBufC* description = HBufC::NewLC( KDescriptionLength );   

	if ( sendChoice == EEditAndSendChoice )
		{
		CEditBeforeSendingViewForm::TMode mode = aIsCancellation ?
		    CEditBeforeSendingViewForm::EEditCancellation :
		    CEditBeforeSendingViewForm::EEditResponse;
		TPtr descriptionPtr( description->Des() );
    	CEditBeforeSendingViewForm* editorForm =
    	    CEditBeforeSendingViewForm::NewL( mode, base, descriptionPtr );
    	TInt buttonID = editorForm->ExecuteLD(
    	    R_EDIT_BEFORE_SENDING_VIEW_FORM_DIALOG );
    	if ( buttonID == EEikCmdCanceled )
    		{
    		retVal = KErrCancel;
    		}
		}

    if ( retVal == KErrNone && sendChoice != EDoNotSendChoice )
        {
        TInt count( aValidIndexes.Count() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            const CCalEntry& entry = *( aEntries[aValidIndexes[i]] );            
            CCalEntry* respOrCancel = CreateToBeSentLC( entry,
                                                        aInstance,
                                                        *description,
                                                        aIsCancellation );
    		SendL( *respOrCancel, aMailbox );
            CleanupStack::PopAndDestroy( respOrCancel );
    	    }	
        }
    CleanupStack::PopAndDestroy( description );
    return retVal;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsUiServices::QuerySendChoiceL
// ----------------------------------------------------------------------------
//   
TInt CMRUtilsUiServices::QuerySendChoiceL(
    const CCalEntry& aBase,
    TBool aIsCancellation,
    CCalAttendee::TCalStatus aStatus ) const
    {
    TInt retVal( KErrNone );
	HBufC* headerText;
	TInt resource( 0 );
	if ( aIsCancellation )
	    {
	    resource = R_QTN_CALE_MEETIN_REQUEST_CANCELLED;
	    }
    else
        {        
    	switch( aStatus )
    		{
    		case CCalAttendee::EAccepted:
    			{
                resource = R_QTN_MAIL_MTG_SUB_ACCEPTED;
    			break;
    			}
    		case CCalAttendee::ETentative:
    			{
                resource = R_QTN_MAIL_MTG_SUB_TENTATIVE;
                break;
    			}
    		case CCalAttendee::EDeclined:
    			{
                resource = R_QTN_MAIL_MTG_SUB_DECLINED;
    			break;
    			}
            default:
                {
                User::Leave( KErrNotSupported );
                }
    		}
        }
	headerText = StringLoader::LoadLC( resource,
                                       aBase.SummaryL(),
                                       CEikonEnv::Static() );

	// show choice list for different alternatives
	CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &retVal );

	if ( aIsCancellation )
	    {
	    dlg->PrepareLC( R_CANCEL_LIST_QUERY );
	    }
    else
        {        
	    dlg->PrepareLC( R_SEND_RESPONSE_LIST_QUERY );
        }
        
	CAknPopupHeadingPane* headingPane = dlg->QueryHeading();
	headingPane->SetTextL( headerText->Des() );

	if ( dlg->RunLD() == EEikCmdCanceled )
		{
		retVal = KErrCancel;
		}
	CleanupStack::PopAndDestroy( headerText );
	return retVal;
    }

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::CreateToBeSentLC
// ----------------------------------------------------------------------------
//          
CCalEntry* CMRUtilsUiServices::CreateToBeSentLC(
    const CCalEntry& aBase,
    const CCalInstance* aInstance,
    const TDesC& aDescription,
    TBool aIsCancellation ) const
    {
    if ( aIsCancellation )
        {
        return CreateCancelLC( aBase, aInstance, aDescription );
        }
    else
        {
        return CreateResponseLC( aBase, aInstance, aDescription );
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::CreateResponseL
// ----------------------------------------------------------------------------
//          
CCalEntry* CMRUtilsUiServices::CreateResponseLC(
    const CCalEntry& aBase,
    const CCalInstance* aInstance,
    const TDesC& aDescription ) const
    {
    // use MRHelpers::ECopyOrganizer (attendee list is different than in base)
    CCalEntry* response = CreateFromLC( aBase,
                                       aInstance,
                                       CCalEntry::EMethodReply,
                                       aBase.SequenceNumberL(),
                                       MRHelpers::ECopyOrganizer );
	
    response->SetDescriptionL( aDescription );

	CCalAttendee* thisAttendee = iMRMailboxUtils.ThisAttendeeL( aBase );
    if ( !thisAttendee )
        {
        User::Leave( KErrNotFound );
        }
        
	CCalAttendee* thisCopy = MRHelpers::CopyAttendeeLC( *thisAttendee );
	response->AddAttendeeL( thisCopy );
	CleanupStack::Pop(); // thisCopy, ownership was transferred
	return response;    
    }
        
// ----------------------------------------------------------------------------
// CMRUtilsUiServices::CreateCancelL
// ----------------------------------------------------------------------------
//          
CCalEntry* CMRUtilsUiServices::CreateCancelLC(
    const CCalEntry& aBase,
    const CCalInstance* aInstance,
    const TDesC& aDescription ) const
    {
    // create a full copy of a base entry
    CCalEntry* cancellation = CreateFromLC( aBase,
                                           aInstance,
                                           CCalEntry::EMethodCancel,
                                           aBase.SequenceNumberL() + 1,
                                           MRHelpers::ECopyFull );

    cancellation->SetDescriptionL( aDescription );

	return cancellation;    
    }    

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::CreateFromLC
// ----------------------------------------------------------------------------
//       
CCalEntry* CMRUtilsUiServices::CreateFromLC(
    const CCalEntry& aBase,
    const CCalInstance* aInstance,
    CCalEntry::TMethod aMethod,
    TInt aSequenceNumber,
    MRHelpers::TCopyFields aCopyType ) const
    {	
    CCalEntry* entry = NULL;	
    
    if ( aInstance )
        { // create entry representing instance, instance also has a base
          // entry, but it may be common for entire series or a range
        HBufC8* calUid = aBase.UidL().AllocLC();
	    entry = CCalEntry::NewL( CCalEntry::EAppt,
                                 calUid,
                                 aMethod,
                                 aSequenceNumber,
                                 aInstance->StartTimeL(),
                                 CalCommon::EThisOnly );
        CleanupStack::Pop( calUid ); // ownership transferred
        CleanupStack::PushL( entry );
        
        MRHelpers::CopyFieldsL( aBase, *entry, aCopyType );
        }    
	else
	    { // usual case - create a copy of base entry
	    entry = MRHelpers::CopyEntryLC( aBase, aMethod, aCopyType );
        }
	
    entry->SetSequenceNumberL( aSequenceNumber );
	return entry;
    }

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::CorrespondingSmtpServiceL
// ----------------------------------------------------------------------------
//
TMsvId CMRUtilsUiServices::CorrespondingSmtpServiceL( TMsvId aRelatedService )
    {
    TMsvEntry entry;
    TMsvId dummyService;
    EnsureMsvSessionExistsL();
    User::LeaveIfError( iMsvSession->GetEntry( aRelatedService,
                                               dummyService,
                                               entry ) );
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
            smtpId = aRelatedService;
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
// CMRUtilsUiServices::EnsureMsvSessionExistsL
// ----------------------------------------------------------------------------
//
void CMRUtilsUiServices::EnsureMsvSessionExistsL()
    {
    if ( !iMsvSession )
        { // Lazy instantiation of iMsvSession
        iMsvSession = CMsvSession::OpenSyncL( *this );
        iOwnMsvSession = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// CMRUtilsUiServices::HandleSessionEventL
// ----------------------------------------------------------------------------
//
void CMRUtilsUiServices::HandleSessionEventL(
    TMsvSessionEvent aEvent,
    TAny* /*aArg1*/,
    TAny* /*aArg2*/,
    TAny* /*aArg3*/ )
    {
    switch ( aEvent )
        {
        case EMsvCloseSession:
        case EMsvServerTerminated:
            {
            if ( iOwnMsvSession )
                {
                delete iMsvSession;
                }
            iMsvSession = NULL; // New session constructed lazily only if needed
            break;
            }
        default:
            {
            // ignore other events
            break;
            }
        }
    }  

// End of file
