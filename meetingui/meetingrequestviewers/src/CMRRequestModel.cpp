/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Model for method REQUEST
*
*/



// INCLUDE FILES
#include "CMRRequestModel.h"
#include "CMRModelBase.h"
#include "MMRDataFillerInterface.h"
#include "MREntryConsultant.h"
#include "CMRDataFillerMeetingRequest.h"
#include "meetingrequestviewers.hrh"
#include "MRViewersPanic.h"
#include "ICalUILog.h"
#include "CMRProcessor.h"
#include "CMRUtilsInternal.h"
#include "ProcessingStructs.h" //meeting request scenarios
#include <eikenv.h> //CEikonEnv
#include <CRichBio.h> //rich bio
#include <MsgMailUIDs.h> //uid for mail application
#include <MRCommands.hrh> // meeting request common commands
#include <senduimtmuids.h> //mailbox id's
#include <stringloader.h> //loading of string resources
#include <meetingrequestviewersuires.rsg> //resource definitions
#include <cmrmailboxutils.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CMRRequestModel" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?classname::?classname
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMRRequestModel::CMRRequestModel(
    CMRMailboxUtils& aMRMailboxUtils,
    CMRUtilsInternal& aMRUtils,
    const MAgnEntryUi::TAgnEntryUiInParams& aInParams )
    : CMRModelBase( aMRMailboxUtils, aMRUtils, aInParams )
    {
    
    }

// -----------------------------------------------------------------------------
// ?classname::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMRRequestModel::ConstructL( RPointerArray<CCalEntry>& aEntries )
    {
    CMRModelBase::ConstructL( aEntries );
    }

// -----------------------------------------------------------------------------
// ?classname::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMRRequestModel* CMRRequestModel::NewL(
    RPointerArray<CCalEntry>& aEntries,
    CMRMailboxUtils& aMRMailboxUtils,
    CMRUtilsInternal& aMRUtils,
    const MAgnEntryUi::TAgnEntryUiInParams& aInParams )
    {
    CMRRequestModel* self = new( ELeave ) CMRRequestModel( aMRMailboxUtils,
                                                           aMRUtils,
                                                           aInParams );

    CleanupStack::PushL( self );
    self->ConstructL( aEntries );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CMRRequestModel::~CMRRequestModel()
    {
    }

MMRModelInterface::TClosingStatus CMRRequestModel::ClosingStatusL()
	{
	TClosingStatus retVal( ENone );
	const CCalEntry& request = *( CombinedEntry() );
	
    if ( iMRProcessor->OwnerRole() == CMRProcessor::EOrganiser )
        { // We are an organizer -> check savind & sending statuses
        if ( IsEntryEdited() )
            { // user has edited the request somehow
            if ( IsSendableL( request ) )
                {
                if ( MREntryConsultant::IsSentL( request ) )
                    { // edited, i.e. an update request
                    retVal = ENotSentUpdate;
                    }
                else
                    { // completely new request
                    retVal = ENotSentNew;
                    }
                }
            else
                { // not sendable, just save the request
                retVal = ENotSaved;
                }
            }
        }        
	else
	    { // Not organizer -> check if response should be proposed
    	CCalAttendee* attendee = iMRMailboxUtils.ThisAttendeeL( request );
    	if ( IsEntryUsableL( request ) && attendee )
    		{
    		CCalAttendee::TCalStatus status( attendee->StatusL() );
    		if ( status == CCalAttendee::ENeedsAction )
    		    {
    		    retVal = ENotResponded;
    		    }
    		}
	    }		
	return retVal;
	}

MMRDataFillerInterface* CMRRequestModel::DataFillerL( CRichBio* aRichBio )
	{
	__ASSERT_DEBUG( aRichBio, Panic( ECRichBioNull ) );
	return CMRDataFillerMeetingRequest::NewL( *aRichBio,
	                                          *this,
	                                          *CEikonEnv::Static(),
	                                          iMRMailboxUtils,
	                                          iInParams );
	}

TBool CMRRequestModel::IsCmdAvailableL( TInt aCommandId )
    {
    TInt retVal( EFalse );
    switch ( aCommandId )
        {
        case EMRCommandSend:
            {
            retVal = CanSendL();
            break;
            }
        case EMRCommandSendUpdate:
            {
            retVal = CanSendUpdateL();
            break;
            }
        case EMRCommandCancelMR:
            {
            retVal = CanCancelMRL();            
            break;
            }
        case EMRCommandDeleteMR:
            {
            retVal = CanDeleteMRL();            
            break;
            }
        case EMRCommandRespondAccept:
        case EMRCommandRespondTentative:
        case EMRCommandRespondDecline:                
            {
            retVal = CanRespondL( aCommandId );            
            break;
            }
        case EMRCommandReplyToOrganiser:
            {
            retVal = CanReplyToOrganiserL();            
            break;
            }
        case EMRCommandReplyToSender:
            {
            retVal = CanReplyToSenderL();            
            break;
            }
        case EMRCommandReplyToAll:
            {
            retVal = CanReplyToAllL();            
            break;
            }
        case EMRCommandForward:
            {
            retVal = CanForwardL();            
            break;
            }       
        default:
            {
            retVal = CMRModelBase::IsCmdAvailableL( aCommandId );
            break;
            }                                     
        }
    return retVal;
    }

TBool CMRRequestModel::CanSendL()
    { 
    TBool retVal( EFalse );   
	const CCalEntry& request = *( CombinedEntry() );     
    if ( iInParams.iCallingApp.iUid == KUidCalendarApplication )
		{
        // Return ETrue if request is sendable, but hasn't been sent at all
        retVal = ( IsSendableL( request ) &&
                   !MREntryConsultant::IsSentL( request ) );
		}
    return retVal;
    }

TBool CMRRequestModel::CanSendUpdateL()
    {
    // Return ETrue if request is sendable but has been sent already
    TBool retVal( EFalse );  
	const CCalEntry& request = *( CombinedEntry() );      
    if ( iInParams.iCallingApp.iUid == KUidCalendarApplication )
		{
        // Return ETrue if request is sendable, is an update in nature,
        // but updated content hasn't been sent yet
        TBool unsentEdits = IsEntryEdited() ||
                            !MREntryConsultant::IsLatestSavedSentL( request );                                    
        retVal = ( IsSendableL( request ) &&
                   MREntryConsultant::IsSentL( request ) &&
                   unsentEdits );
		}
    return retVal;
    }
    
TBool CMRRequestModel::CanCancelMRL()
    {
    TBool retVal( EFalse );
	const CCalEntry& request = *( CombinedEntry() );     
    if ( iInParams.iCallingApp.iUid == KUidCalendarApplication &&
         iMRProcessor->OwnerRole() == CMRProcessor::EOrganiser )
		{
        // can cancel if at least one version of the request has been sent
        // and request is ok
        retVal = ( IsEntryUsableL( request ) &&
                   MREntryConsultant::IsSentL( request ) );
        }
    return retVal;    
    }
    
TBool CMRRequestModel::CanDeleteMRL()
    {
    TBool retVal( EFalse );
	const CCalEntry& request = *( CombinedEntry() );
    if ( iInParams.iCallingApp.iUid == KUidCalendarApplication &&
         MREntryConsultant::ExistsInDbL( request, iMRUtils ) )
		{
		if ( iMRProcessor->OwnerRole() == CMRProcessor::EOrganiser )
		    {		    
            // deletion is shown when cancellation not and vice versa
            retVal = !CanCancelMRL();    
		    }
        else
            { // if not organizer then can delete
            retVal = ETrue;
            }
        }  
    return retVal;        
    }

TBool CMRRequestModel::CanRespondL( TInt aCommandId )
	{
	TBool retVal( EFalse );
	const CCalEntry& request = *( CombinedEntry() );
    CCalAttendee* attendee = iMRMailboxUtils.ThisAttendeeL( request );
    if ( IsEntryUsableL( request ) && attendee )
        {
        CCalAttendee::TCalStatus status( attendee->StatusL() );
        if ( aCommandId == EMRCommandRespondAccept &&
             ( status != CCalAttendee::EAccepted ||
               status != CCalAttendee::EConfirmed ) )
            {
            retVal = ETrue;
            }
        else if ( aCommandId == EMRCommandRespondTentative &&
                  status != CCalAttendee::ETentative )
            {
            retVal = ETrue;
            }
        else if ( aCommandId == EMRCommandRespondDecline &&
                  status != CCalAttendee::EDeclined )
            {
            retVal = ETrue;
            }            
        }
	return retVal;
	}	

TBool CMRRequestModel::CanReplyToOrganiserL()
    {
    TBool retVal( EFalse );
    if ( iMRProcessor->OwnerRole() != CMRProcessor::EOrganiser )
        {        
        retVal = ETrue;
        }
	return retVal;
    }

TBool CMRRequestModel::CanReplyToSenderL()
    {    
    TBool retVal( EFalse );    
	if ( iInParams.iCallingApp.iUid == KUidMailApplication )
		{
        if ( iMRProcessor->OwnerRole() != CMRProcessor::EOrganiser )
            { // can reply to sender if sender is not also organizer
            retVal =!IsSenderOrganizerL();
            }
		}
	return retVal;
    }

TBool CMRRequestModel::CanReplyToAllL()
    {    
	return ETrue;
    }

TBool CMRRequestModel::CanForwardL()
	{
	LOG("CMRRequestModel::CanForwardL");
	TBool retVal( EFalse );
	const CCalEntry& request = *( CombinedEntry() );	
	if ( iInParams.iCallingApp.iUid == KUidMailApplication &&
	     IsEntryUsableL( request ) )
		{
		LOG("CMRRequestModel::CanForwardL returning ETrue");
        return ETrue;
		}
    LOG("CMRRequestModel::CanForwardL -> End");
	return retVal;
	}

//  End of File
