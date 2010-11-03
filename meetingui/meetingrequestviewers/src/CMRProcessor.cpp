/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation for meeting request processor 
*
*/




// INCLUDE FILES
#include "CMRProcessor.h"
#include "MRHelpers.h"
#include "mrdatalog.h"
#include "ICalUILogDef.h"
#include <e32std.h>
#include <calentry.h> 		//CCalEntry (Calendar entry API V2)
#include <caluser.h> 		//caluser and attendee
#include <MsgMailUIDs.h> 	//uid for mail application
#include <cemailaccounts.h>
#include <smtpset.h>
#include <imapset.h>
#include <pop3set.h>
#include <MuiuMsvUiServiceUtilities.h>
#include "MRViewersPanic.h" 			//panic codes
#include "meetingrequestviewers.hrh" 	//common constants
#include "ProcessingStructs.h" 			//scenario structures
#include <SendUiConsts.h> 				//mailbox uids
#include <stringloader.h> 				//loading of string resources
#include <meetingrequestviewersuires.rsg> 	//resource definitions header
#include <CalenInterimUtils2.h>
#include <cmrmailboxutils.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CMRProcessor" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMRProcessor::CMRProcessor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMRProcessor::CMRProcessor(
    CMRMailboxUtils& aMRMailboxUtils,
    CMRUtilsInternal& aMRUtils,
    const MAgnEntryUi::TAgnEntryUiInParams& aParams,
    RPointerArray<CCalEntry>& aEntries )
    : iMRMailboxUtils( aMRMailboxUtils ),
      iMRUtils( aMRUtils ),
      iEntryUiInParams( aParams ),
      iArrayOfEntries( aEntries ),
      iEditMode( MMRModelInterface::EModeNotSet ),
      iOwnerRole( ENotFound )
    {
	// set initial values to scenario data
	iCombinedScenData.iMethod = CCalEntry::EMethodNone;
	iCombinedScenData.iCallingApp = ENotSet;
	iCombinedScenData.iOpenedMailbox = ENotSet;
    iCombinedScenData.iUtilsDBResult = MMRUtilsTombsExt::EUndefined;		    	
    iCombinedScenData.iMethodLevelValidity = ENotSet;
	iCombinedScenData.iEditorMode = ENotSet;
    iCombinedScenData.iMROperation = ENotSet;    
	}

// -----------------------------------------------------------------------------
// CMRProcessor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMRProcessor::ConstructL()
    {    
    }

// -----------------------------------------------------------------------------
// ?classname::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMRProcessor* CMRProcessor::NewL(
    CMRMailboxUtils& aMRMailboxUtils,
    CMRUtilsInternal& aMRUtils,
    const MAgnEntryUi::TAgnEntryUiInParams& aParams,
    RPointerArray<CCalEntry>& aEntries )
    {
    CMRProcessor* self = new( ELeave ) CMRProcessor( aMRMailboxUtils,
                                                     aMRUtils,
                                                     aParams,
                                                     aEntries );
   CleanupStack::PushL( self );
   self->ConstructL();
   CleanupStack::Pop();

   return self;
   }

// Destructor
CMRProcessor::~CMRProcessor()
    {
    delete iCombinedEntry;
    }

CCalEntry* CMRProcessor::CombinedEntry()
	{
	// Should always be valid when this method is called
    __ASSERT_DEBUG( iCombinedEntry, Panic( ECombinedCalEntryNull ) );	
	return iCombinedEntry;
	}

void CMRProcessor::SetPhoneOwnerL()
    {
	__ASSERT_DEBUG( iArrayOfEntries.Count() > 0, Panic( EEmptyEntryArray ) );
	
    // set phone owner field of each entry:
    
    TInt count( iArrayOfEntries.Count() );
    for ( TInt i( 0 ); i < count; ++i )    
        {        
        CCalEntry& entry = *( iArrayOfEntries[i] );
        iMRMailboxUtils.SetPhoneOwnerL( entry, iEntryUiInParams.iMailBoxId );
        }
    
    // set iOwnerRole, use entry at index zero:
        
	CCalEntry& baseEntry = *( iArrayOfEntries[0] );	
	if ( iMRMailboxUtils.IsOrganizerL( baseEntry ) )
	    {
	    iOwnerRole = EOrganiser;
	    }	    
    else
        {        
	    CCalAttendee* thisAttendee = iMRMailboxUtils.ThisAttendeeL( baseEntry );
    	if ( thisAttendee )
    		{
    		switch ( thisAttendee->RoleL() )
    			{
    			case CCalAttendee::EOptParticipant:
    				{
    				iOwnerRole = EOptionalParticipant;
    				break;
    				}
    			case CCalAttendee::ENonParticipant:
    				{
    				iOwnerRole = ENonRequiredParticipant;
    				break;
    				}
                case CCalAttendee::EReqParticipant: // fall through
    			case CCalAttendee::EChair: // fall through
    			default:
    				{ // Note: chair MUST NOT be interpreted as organizer!
    				  // Req.participant is not correct either but has less
    				  // side effects.
    				iOwnerRole = ERequiredParticipant;
    				break;
    				}
    			}
    		}
        }
    }

CMRProcessor::TOwnerRole CMRProcessor::OwnerRole()
    {
    return iOwnerRole;
    }

MMRUtilsTombsExt::TMRUtilsDbResult CMRProcessor::ProcessingDbResult()
    {
    return iCombinedScenData.iUtilsDBResult;
    }

TInt CMRProcessor::ProcessingResultOp()
    {
    return iCombinedScenData.iMROperation;
    }

MMRModelInterface::TEditingMode CMRProcessor::EditingMode() const
    {
    return iEditMode;
    }
    
void CMRProcessor::SetEditingModeL( MMRModelInterface::TEditingMode aEditMode )
    {
    __ASSERT_DEBUG( iEditMode == MMRModelInterface::EModeNotSet,
                    Panic( EEditModeResetAttempt ) );    
    iEditMode = aEditMode;
    if ( aEditMode == MMRModelInterface::EEditMeeting )
        { // if we switch to edit meeting mode the combined entry must be re-set
        CCalEntry* entry = iArrayOfEntries[0];
        ResetCombinedEntryL( *entry, TCalTime() );                
		iCombinedEntry->CopyFromL( *entry );		
		iCombinedEntry->SetMethodL( entry->MethodL() );        
        }
    // no need to do anything if aEditMode == EEditInstance since that is the
    // default when launching the MR Viewer with incoming entry array.
    }

// TODO: CMRUtils::CheckEntryCondL doesn't, and shouldn't (because of e.g.
// calendar sync), notice problems with out-of-date entry (meeting occurred in past),
// but in processor we should identify that situation. Possibly add a new processing
// struct data field, and possibly a new TMROperation code also?

// TODO: somewhere we must set outparams: saved, meeting/instance deleted,
// but is processor a good place for it.     

// TODO: when handling processing leave, we must take care that iCombinedEntry
// is set to point to some entry, or to ensure that there doesn't become an access
// violation because someone is trying to access it.
void CMRProcessor::ProcessEntriesL()
	{
	if ( iArrayOfEntries.Count() < 1 )
	    {
	    User::Leave( KErrArgument );
	    }	    
    CCalEntry* entry = iArrayOfEntries[0];
    
    // Sets phone owner in entries and in iOwnerRole, this must be done at
    // early stage, before trying to read organizer or attendee fields
    SetPhoneOwnerL();
	    
	// set common values to scenario data
	iCombinedScenData.iMethod = entry->MethodL();
	iCombinedScenData.iCallingApp = CallingApp();
	iCombinedScenData.iOpenedMailbox = OpenedMailboxL( *entry );
	iCombinedScenData.iEditorMode = EditorMode();
    if ( iCombinedScenData.iEditorMode == MAgnEntryUi::EViewEntry )
        { // in other cases editing mode will be decided later, but 
          // in EViewEntry case we know it already now
        SetEditingModeL( MMRModelInterface::EViewOnly );
        }

    TRAPD( err,
        {        
        if ( entry->MethodL() == CCalEntry::EMethodReply )
            {
            ProcessResponseArrayL();
            }        
        else 
            {        
            if ( iEntryUiInParams.iCallingApp.iUid == KUidCalendarApplication )
                {
                ProcessRequestInCalendarL();
                }
            else
                {
                ProcessRequestOrCancelArrayL();
                }
            }        
        } );
    if ( err != KErrNone )
        {
        ProcessErrorL();
        }            
	}
		
/**
* In case of calendar we show in start-up:
* a) parent entry if meeting non-repeating
* b) existing child entry if received as input
* c) newly created child if repeating meeting but child doesn't exist
* User may later on choose "edit series" mode if meeting is repeating.
*/
void CMRProcessor::ProcessRequestInCalendarL()
    {
	if ( iArrayOfEntries.Count() != 1 )
	    {
	    // From calendar we support only receiving single entry at a time
	    User::Leave( KErrArgument );
	    }
	
    CCalEntry* entry = iArrayOfEntries[0];
    
    iCombinedScenData.iMROperation =
        MatchScenarioToDataL( iCombinedScenData, *entry );
    if ( iCombinedScenData.iMROperation <= ENullOp )
        {
        User::Leave( KErrArgument );
        }    

    if ( MREntryConsultant::IsRepeatingMeetingL( *entry ) )
        {
        // reset combined entry, it will be a child representing the instance
        ResetCombinedEntryL( *entry, iEntryUiInParams.iInstanceDate );
        
        if ( MREntryConsultant::IsModifyingEntryL( *entry ) )
            { // entry exists for the instance, use that
    		iCombinedEntry->CopyFromL( *entry );		
    		iCombinedEntry->SetMethodL( entry->MethodL() );
            }
        else
            { // create a fully populated child representing the instance
            SetInstanceStartAndEndL( *iCombinedEntry,
                                     *entry, 
                                     iEntryUiInParams.iInstanceDate );                      
            CCalenInterimUtils2::PopulateChildFromParentL( *iCombinedEntry,
                                                           *entry );
            }
        }
    else
        { // non-repeating meeting -> view/edit entire meeting
        // reset combined entry, it will be a parent
        ResetCombinedEntryL( *entry, TCalTime() );                
		iCombinedEntry->CopyFromL( *entry );		
		iCombinedEntry->SetMethodL( entry->MethodL() );
		
		// in case of non-repeating meeting the editing mode is always
		// EEditMeeting and user doesn't have a possibility to change it
		SetEditingModeL( MMRModelInterface::EEditMeeting );     
        }    
    }
	
void CMRProcessor::ProcessResponseArrayL()
	{

// TODO: we must check that entry isn't out of date or cancelled,
// should we do that in HandleResponseStatusL or where? It would be
// better to do it before calling TryCreateModForResponseL() to avoid
// unnecessary mod entry in	db.
	
	// 1. Handle entry at index 0 ,it may be response to a parent or a child
		
	CCalEntry& response = *( iArrayOfEntries[0] );
		
	// when checking against db entry, respond should look like a valid update:
	iCombinedScenData.iUtilsDBResult = iMRUtils.CheckEntryCondL( response );
	
	// ...but if attendee has responded to an instance of a meeting, then
	// corresponding modifying entry doesn't necessarily exist -> create it:
	if ( iCombinedScenData.iUtilsDBResult == MMRUtilsTombsExt::ECheckedValidNew )
	    {	    
	    CreateModForResponseL( response, iCombinedScenData );
	    }
	
	// check response validity on method level and set status to request, if ok:
	CCalEntry* request = HandleResponseStatusL( response, iCombinedScenData );
	if ( request )
	    {
	    CleanupStack::PushL( request );
    	// match to scenario, entry to be updated is request, not response
        MatchScenarioAndSaveIfNeededL( *request, iCombinedScenData );
	    }
    else
        {
        // if request not found saving won't occur, we still perform matching
        // to get scenario completed, we give response as a parameter but it
        // won't be used in this case
        MatchScenarioAndSaveIfNeededL( response, iCombinedScenData );
        }
		
    // 2. Go through additional entries, they don't affect combined entry
    //    and related functionality

    TScenarioData tmpScenario( iCombinedScenData );
	TInt count( iArrayOfEntries.Count() );
	for ( TInt i( 1 ); i < count; ++i )
		{
		CCalEntry& tmpResponse = *( iArrayOfEntries[i] );
		tmpScenario.iUtilsDBResult = iMRUtils.CheckEntryCondL( tmpResponse );
	    if ( tmpScenario.iUtilsDBResult == MMRUtilsTombsExt::ECheckedValidNew )
	        {	    
	        CreateModForResponseL( response, tmpScenario );
	        }		
		CCalEntry* tmpRequest = HandleResponseStatusL( response, tmpScenario );
		if ( tmpRequest )
		    { // for additional entries we only match scenario if request exists,
		      // otherwise it is unnecessary
		    CleanupStack::PushL( tmpRequest );
		    MatchScenarioAndSaveIfNeededL( *tmpRequest, tmpScenario );
		    CleanupStack::PopAndDestroy( tmpRequest );
		    }
		}
    
    // 3. Set combined entry == first response possibly mixed with request

    ResetCombinedEntryL( response, response.RecurrenceIdL() );
    iCombinedEntry->CopyFromL( response );
    iCombinedEntry->SetMethodL( CCalEntry::EMethodReply );

	if ( iCombinedScenData.iMROperation == EUpdateStatusToCalendar )
		{
    	// TODO: set response summary with status information    	
    	iCombinedEntry->SetLocationL( request->LocationL() );
    	iCombinedEntry->SetStartAndEndTimeL( request->StartTimeL(),
    	                                     request->EndTimeL() );
        // Response won't necessarily contain RRule/RDate information, but
        // that isn't shown to the user either -> not needed in iCombinedEntry.
		}
    else
        {
        User::Leave( KErrArgument );
        }
        
    if ( request )
        {
        CleanupStack::PopAndDestroy( request );
        }
	}
	
/**
* This method assumes that response has been verified to be valid.
*/
void CMRProcessor::CreateModForResponseL(
    const CCalEntry& aResponse,
    TScenarioData& aScenarioData ) const
    {
    // fetch parent:
    CCalEntry* request = iMRUtils.FetchEntryL( aResponse.UidL(),
                                               TCalTime() );
    CleanupStack::PushL( request );
    // create modifying entry:
    HBufC8* modUid = request->UidL().AllocLC();
	CCalEntry* mod = CCalEntry::NewL( CCalEntry::EAppt,
                                      modUid,
                                      CCalEntry::EMethodRequest,
                                      request->SequenceNumberL(),
                                      aResponse.RecurrenceIdL(),
                                      CalCommon::EThisOnly );
    CleanupStack::Pop( modUid ); // ownership transferred
    CleanupStack::PushL( mod );
    
    // In this case response shouldn't really have any RRules or RDates,
    // and the recurrence id should specify the instance which attendee
    // responded to. Therefore create modifying entry representing that
    // instance.
    SetInstanceStartAndEndL( *mod, *request, aResponse.RecurrenceIdL() );
    // Populate modifying entry fields, especially it is important that
    // also entire attendee list gets copied, since this modifying entry
    // will be exceptioned in the parent
    CCalenInterimUtils2::PopulateChildFromParentL( *mod, *request );
    // Modifying entry must have the same DTSTAMP as originating entry,
    // since in this case they were sent as one request
    mod->SetDTStampL( request->DTStampL() );        
    // Set modifying request as sent!
    // TODO: this will be done differently when Symbian supports other fields
    // to indicate sending status
    mod->SetStatusL( CCalEntry::EConfirmed );
        
    MMRUtilsTombsExt::TMRUtilsDbResult res = iMRUtils.StoreL( *mod, EFalse );
    if ( res != MMRUtilsTombsExt::EStoredUpdate )
        { // store should succeed if response was for a valid instance
        User::Leave( KErrGeneral );
        }
    
    CleanupStack::PopAndDestroy( 2 ); // mod, request
    
    // now there exists a modifying entry and response is
    // an update to it:
    aScenarioData.iUtilsDBResult = MMRUtilsTombsExt::ECheckedValidUpdate;
    }
 
CCalEntry* CMRProcessor::HandleResponseStatusL(
    const CCalEntry& aResponse,
    TScenarioData& aScenarioData ) const
    {
    aScenarioData.iMethodLevelValidity = EInvalid;
    if ( aResponse.AttendeesL().Count() != 1 )
        { // response MUST have exactly one attendee, the respondent
        User::Leave( KErrArgument );
        }
	CCalAttendee& respondent = *( aResponse.AttendeesL()[0] );	

    CCalEntry* request = iMRUtils.FetchEntryL( aResponse.UidL(),
                                               aResponse.RecurrenceIdL() );
    if ( request  )
        {
        CleanupStack::PushL( request );        	
    	TBool changed( EFalse );
    	CCalAttendee* dbAttendee =
    	    RespondentInRequestL( aResponse, *request, changed );
    	if ( dbAttendee && changed )
    		{
    		dbAttendee->SetStatusL( respondent.StatusL() );
    		aScenarioData.iMethodLevelValidity = EValidNeedsSave;
    		}
        else if ( dbAttendee && !changed )
            {
            aScenarioData.iMethodLevelValidity = EValidNoNeedSave;
            }
        CleanupStack::Pop( request );
        }
    return request; // may be NULL
    }

/**
* We always show the first entry, if it is a child user will see that
* child and responses affect that (and other children in the array)
* If the first entry is a parent responses affect also children in the array.
*
* Also in case of cancellation entry is stored to db instead of just updating
* the status, this means that e.g. description may be changed, but we
* consider that is the desired behavior.
*/
void CMRProcessor::ProcessRequestOrCancelArrayL()
	{
	// 1. Handle entry at index 0 ,it may be a parent or a child	
	
	CCalEntry& entry = *( iArrayOfEntries[0] );
	iCombinedScenData.iUtilsDBResult = iMRUtils.CheckEntryCondL( entry );
    MatchScenarioAndSaveIfNeededL( entry, iCombinedScenData );
		
    // 2. Go through additional entries, they don't affect combined entry
    //    and related functionality

    TScenarioData tmpScenario( iCombinedScenData );
	TInt count( iArrayOfEntries.Count() );
	for ( TInt i( 1 ); i < count; ++i )
		{
		CCalEntry& tmpEntry = *( iArrayOfEntries[i] );
		tmpScenario.iUtilsDBResult = iMRUtils.CheckEntryCondL( tmpEntry );
		MatchScenarioAndSaveIfNeededL( tmpEntry, tmpScenario );
		}
    
    // 3. Set combined entry == first entry in the input array

    ResetCombinedEntryL( entry, entry.RecurrenceIdL() );

	if ( iCombinedScenData.iMROperation == EViewExistingEntry )
		{
		iCombinedEntry->CopyFromL( entry );				
		iCombinedEntry->SetMethodL( entry.MethodL() );				
		}
    else if ( iCombinedScenData.iMROperation == ELoadIdenticalEntryFromDB || 
              iCombinedScenData.iMROperation ==EStoreEntryToCalendar )
		{
		// Identical entry exists in db -> fetch parent entry
		// from db and prepare for viewing
		ReadEntryFromDbL( entry, *iCombinedEntry );
		}
    else
        {
        User::Leave( KErrArgument );
        }
    }

TBool CMRProcessor::MatchScenarioAndSaveIfNeededL(
    CCalEntry& aCalEntry,
    TScenarioData& aScenarioData ) const
    {
    TBool retVal( EFalse );	
	aScenarioData.iMROperation =
	    MatchScenarioToDataL( aScenarioData, aCalEntry );
	
	if ( aScenarioData.iMROperation == EStoreEntryToCalendar )
		{
		MMRUtilsTombsExt::TMRUtilsDbResult dbResult =
		    iMRUtils.StoreL( aCalEntry, EFalse );
		if ( dbResult < MMRUtilsTombsExt::EUndefined )
			{ // store shouldn't fail if all checks were successful
			User::Leave( KErrGeneral );
			}
        else
			{ // we have saved something
			retVal = ETrue;
			}
		}
    else if ( aScenarioData.iMROperation == EUpdateStatusToCalendar )
        { // update shouldn't fail if all checks were successful
        User::LeaveIfError( iMRUtils.UpdateEntryL( aCalEntry ) );        
        retVal = ETrue;
        }
    return retVal;
    }

void CMRProcessor::ProcessErrorL()
    {
    switch ( iCombinedScenData.iMROperation )
        {
        case EErrorSituation:
            {
    		if ( iCombinedScenData.iUtilsDBResult ==
	    	     MMRUtilsTombsExt::EErrorHasBeenDeleted )
		        { // possibly valid entry but has been deleted from the phone
		        
        	    // request has been deleted, we show the received response then
        	    // according to UI spec. if organizer cancels request it gets deleted
        	    // TODO: show info note Original meeting request is not in phone’s calendar. §qtn.mail.mtg.req.note.no.mtg§ with
                // TODO: this note should be shown always as an opening note, some opening notes
                // are shown only when launching for the first time -> handle that!
    		    }
            else
                {// TODO: handle
                }
            break;
            }
        case EErrorUnexpectedViewOnly:
            { // possibly valid entry but unexpected in this context
            // TODO: handle
            break;
            }
        case EErrorObsoleteViewOnly:
            { // possibly valid entry but obsolete in this context
            // TODO: handle
            break;
            }
        default:
            {
            User::Leave( KErrGeneral );
            }       
        }
    // show errorneus entry, but only for viewing
	CCalEntry& entry = *( iArrayOfEntries[0] );
    ResetCombinedEntryL( entry, entry.RecurrenceIdL() );
	iCombinedEntry->CopyFromL( entry );				
	iCombinedEntry->SetMethodL( entry.MethodL() );	
    }

TInt CMRProcessor::MatchScenarioToDataL(
    TScenarioData currentScenario,
    const CCalEntry& aEntry ) const
	{
    MRDATA_LOG("# start scenario match #");
    MRDATA_LOG1("entry name: %S", &(aEntry.SummaryL()));
    	
	TInt retVal( EErrorSituation );
	
	CArrayFixFlat<TInt>* scenarioValues =
	    new( ELeave ) CArrayFixFlat<TInt>( dataFieldCount-1 );

	CleanupStack::PushL( scenarioValues );

    // Order of fields must match the order used in ProcessingStructs.h
	scenarioValues->AppendL( currentScenario.iMethod );
	scenarioValues->AppendL( currentScenario.iCallingApp );
	scenarioValues->AppendL( currentScenario.iOpenedMailbox );
	scenarioValues->AppendL( currentScenario.iUtilsDBResult );
	scenarioValues->AppendL( currentScenario.iMethodLevelValidity );
	scenarioValues->AppendL( currentScenario.iEditorMode );
	
	// Go through each predefined scenario and see if one of them matches
	for ( TInt scenarioCounter( 0 );
	      scenarioCounter < scenarioCount;
	      scenarioCounter++ )
		{
        TBool scenarioMismatch( EFalse );
		const TInt* arrayPtr = scenarioArray[scenarioCounter];
		for ( TInt i( 0 ); i < dataFieldCount-1; i++ )
			{
			TInt arrayVal = *( arrayPtr+i );
			TInt scenarioVal = scenarioValues->At( i );

			if( arrayVal != ENotSet )
				{
				if ( scenarioVal != arrayVal )
				    {				    
					scenarioMismatch = ETrue;
				    }
				}
			}

		if ( !scenarioMismatch )
			{ // Match was found, retVal is operation corresponding to the
			  // matching scenario, i.e. the last field in that scenario array
			retVal = *( arrayPtr + dataFieldCount - 1 );
            MRDATA_LOG1("scenario id: %d", scenarioCounter );
            MRDATA_LOG1("operation: %d", retVal );			
			break;
			}
		}

	CleanupStack::PopAndDestroy( scenarioValues );
	    
    MRDATA_LOG("# end scenario match #");	
	return retVal;
	}

TInt CMRProcessor::CallingApp() const
	{
	TInt callingApp = iEntryUiInParams.iCallingApp.iUid;
    TInt retVal( ENotSet );

	switch ( callingApp )
		{
		case KUidCalendarApplication:
			{
			retVal = ECallerCalendarApp;
			break;
			}
		case KUidMailApplication:
			{
			retVal = ECallerMailApp;
			break;
			}
		case KUidBVAApplication:
			{
			retVal = ECallerBVApp;
			break;
			}
		default:
			{
			//error situation, should never come here
			retVal = ENotSet;
			break;
			}
		}
    return retVal;
	}

TInt CMRProcessor::EditorMode() const
    {
    if ( iEntryUiInParams.iCallingApp.iUid == KUidCalendarApplication )
        { // Editor mode is relevant only when calling app is calendar
        return iEntryUiInParams.iEditorMode;
        }
    else
        {
        return MAgnEntryUi::EViewEntry;
        }
    }

// TODO: should we also support drafts mailbox type?
// That should be done by checking IsSent( entry ) in the outbox case.
TInt CMRProcessor::OpenedMailboxL( const CCalEntry& aEntry ) const
	{
	TInt retVal( ENotSet );	
	if ( iEntryUiInParams.iCallingApp.iUid == KUidMailApplication )	
	    {	    
        // Organizer sends other types than responses, and attendees
        // on the other hand send only responses
        TBool isOrganizer( iMRMailboxUtils.IsOrganizerL( aEntry ) );
        TBool isResponse( aEntry.MethodL() == CCalEntry::EMethodReply );
        if ( ( isOrganizer && !isResponse ) || ( !isOrganizer && isResponse  ) )
            {
            retVal = EOpenedFromOutbox;
            }
        else
            {
            retVal = EOpenedFromInbox;
            }
	    }
    return retVal;
	}

CCalAttendee* CMRProcessor::RespondentInRequestL(
    const CCalEntry& aResponse,
    const CCalEntry& aRequest,
    TBool& aStatusChange ) const
	{
	__ASSERT_DEBUG( aResponse.MethodL() == CCalEntry::EMethodReply,
	                Panic( EUnexpectedEntryMethodType ) );   

	aStatusChange = EFalse;
	CCalAttendee* respondent = aResponse.AttendeesL()[0];
	CCalAttendee* dbAttendee =
	    MREntryConsultant::EqualAttendeeL( *respondent, aRequest );
    if ( dbAttendee )
        {            
		if ( dbAttendee->StatusL() != respondent->StatusL() )
            {
            aStatusChange = ETrue;
            }
        }
    return dbAttendee;
	}
	
void CMRProcessor::ReadEntryFromDbL(
    const CCalEntry& aSourceEntry,
    CCalEntry& aTargetEntry ) const
	{
    CCalEntry* dbEntry = iMRUtils.FetchEntryL( aSourceEntry.UidL(),
                                               aSourceEntry.RecurrenceIdL() );
	CleanupStack::PushL( dbEntry );	
	aTargetEntry.CopyFromL( *dbEntry );
	aTargetEntry.SetMethodL( dbEntry->MethodL() );	

	CleanupStack::PopAndDestroy( dbEntry );
	}
 
// method to be called externally
void CMRProcessor::RefreshViewableEntryL()
    {
    __ASSERT_DEBUG( iCombinedEntry, Panic( ECombinedCalEntryNull ) );
    if ( MREntryConsultant::ExistsInDbL( *iCombinedEntry, iMRUtils ) )
        { // this method is only feasible if entry exists in the database,
          // otherwise we cannot refresh anything...
        ResetCombinedEntryL( *iCombinedEntry, iCombinedEntry->RecurrenceIdL() );
        ReadEntryFromDbL( *iCombinedEntry, *iCombinedEntry );
        }
    }

void CMRProcessor::ResetCombinedEntryL(
    const CCalEntry& aBase, 
    const TCalTime& aInstanceDate )
    {
	HBufC8* calUid = aBase.UidL().AllocLC();
    CCalEntry* newEntry;
    if ( aInstanceDate.TimeUtcL() != Time::NullTTime() )
        {
    	newEntry = CCalEntry::NewL( CCalEntry::EAppt,
	                                calUid,
	                                aBase.MethodL(),
	                                aBase.SequenceNumberL(),
	                                aInstanceDate,
	                                CalCommon::EThisOnly );
        }
    else
        {        
    	newEntry = CCalEntry::NewL( CCalEntry::EAppt,
	                                calUid,
	                                aBase.MethodL(),
	                                aBase.SequenceNumberL() );
        }
    delete iCombinedEntry;
    iCombinedEntry = NULL;    
    iCombinedEntry = newEntry;
	CleanupStack::Pop( calUid ); // ownership transferred to iCombinedEntry    
    }
    
void CMRProcessor::SetInstanceStartAndEndL(
    CCalEntry& aChild,
    const CCalEntry& aParent,
    const TCalTime& aInstanceStart ) const
    {
    TTime dtstart( aParent.StartTimeL().TimeUtcL() ); // first instance start
    TTime dtend( aParent.EndTimeL().TimeUtcL() ); // first instance end
    TTime instStart( aInstanceStart.TimeUtcL() );    
    TTime nullTime( Time::NullTTime() );
    if ( dtstart == nullTime || dtend == nullTime || instStart == nullTime )
        {
        User::Leave( KErrArgument );
        }        
    
    TTimeIntervalMicroSeconds duration( 0 );
    duration = dtend.MicroSecondsFrom( dtstart );
    TCalTime instEnd;    
	instEnd.SetTimeUtcL( instStart + duration );

    aChild.SetStartAndEndTimeL( aInstanceStart, instEnd );
    }
    
