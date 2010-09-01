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
* Description: Implementation for meeeting request command handler
*
*/




// INCLUDE FILES
#include "CMRCmdHandler.h"
#include "MRHelpers.h"
#include "MMRModelInterface.h"
#include "MREntryConsultant.h"
#include <e32std.h>
#include <calentry.h> 		//CCalEntry (Calendar entry API V2)
#include <caluser.h> 		//caluser and attendee
#include "CMRUtilsInternal.h"
#include <MsgMailUIDs.h> 	//uid for mail application
#include "MRViewersPanic.h" //panic codes for meeting request viewers
#include <mrcommands.hrh> 	//common constants
#include <avkon.hrh>
#include <aknlistquerydialog.h>
#include <meetingrequestviewersuires.rsg>
#include <stringloader.h>           // StringLoader
#include <aknnotewrappers.h>
#include <cmrmailboxutils.h>
#include <AknGlobalNote.h>
#include "ICalUILog.h"

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CMRCmdHandler" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMRCmdHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMRCmdHandler* CMRCmdHandler::NewL(
    MMRModelInterface& aModel,
    const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
    CMRMailboxUtils& aMRMailboxUtils,
    CMRUtilsInternal& aMRUtils )
    {

    LOG("CMRCmdHandler::NewL()");

    CMRCmdHandler* self = new( ELeave ) CMRCmdHandler( aModel,
                                                       aInParams,
                                                       aMRMailboxUtils,
                                                       aMRUtils );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();



    return self;
    }

// -----------------------------------------------------------------------------
// CMRCmdHandler::CMRCmdHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMRCmdHandler::CMRCmdHandler(
    MMRModelInterface& aModel,
    const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
    CMRMailboxUtils& aMRMailboxUtils,
    CMRUtilsInternal& aMRUtils )
    : iModel( aModel ),
      iInParams( aInParams ),
      iMRUtils( aMRUtils ),
      iMRMailboxUtils( aMRMailboxUtils )
    {
    }

// -----------------------------------------------------------------------------
// CMRCmdHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMRCmdHandler::ConstructL()
    {
    // Memorize original attendee list:
    RPointerArray<CCalAttendee>& attendees =
        iModel.CombinedEntry()->AttendeesL();
    TInt count( 0 );
    for ( TInt i( 0 ); i < count; ++i )
        {
        CCalAttendee* att = MRHelpers::CopyAttendeeLC( *( attendees[i] ) );
        iOriginalAttendees.AppendL( att ); // ownership transferred
        CleanupStack::Pop( att );
        }
    }

// Destructor
CMRCmdHandler::~CMRCmdHandler()
    {
    iOriginalAttendees.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CMRCmdHandler::SaveL
// ----------------------------------------------------------------------------
//
TInt CMRCmdHandler::SaveL()
    {
    CCalEntry& combinedEntry = *( iModel.CombinedEntry() );
    // we are an organizer and must set protocol fields,
    // set entry as non-sent -> 2nd parameter == EFalse
    SetProtocolFieldsL( combinedEntry, EFalse );
    // save combined entry
    return iMRUtils.StoreL( combinedEntry, EFalse );
    }

// ----------------------------------------------------------------------------
// CMRCmdHandler::SaveFromFileL
// ----------------------------------------------------------------------------
//
TInt CMRCmdHandler::SaveFromFileL( const RPointerArray<CCalEntry>& aEntries )
    {
    TInt retVal( KErrNone );
    CCalEntry& firstEntry = *( aEntries[0] );    
    TInt count( aEntries.Count() );
            
    // all entries have the same method
    if ( iModel.MethodL() == CCalEntry::EMethodReply )
        {
        // update first entry and take the result code
        retVal = iMRUtils.UpdateEntryL( firstEntry );        
        // update also other entries in file, but don't care about the result
        for ( TInt i( 1 ); i < count; ++i )
            {
            iMRUtils.UpdateEntryL( *( aEntries[i] ) );
            }        
        }
    else
        {
        // save first entry and take the result code
        retVal = iMRUtils.StoreL( firstEntry, EFalse );
        
        // save also other entries in file, but don't care about the result
        for ( TInt i( 1 ); i < count; ++i )
            {
            iMRUtils.StoreL( *( aEntries[i] ), EFalse );
            }
        }
    return retVal;        
    }
    
// ----------------------------------------------------------------------------
// CMRCmdHandler::SendL
// Send a request edited by the organizer (in Calendar application)
// ----------------------------------------------------------------------------
//
TInt CMRCmdHandler::SendL( TInt aSendCmd )
    {
    TMsvId boxToUse = MREntryConsultant::SendingMailBoxL( iInParams,
                                                          iMRMailboxUtils );
    CCalEntry& combinedEntry = *( iModel.CombinedEntry() );

    TInt retVal( KErrNone );

    if ( combinedEntry.SummaryL().Length() == 0 )
        { // Confirm that sending is ok although subject is empty
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        if ( !dlg->ExecuteLD( R_SEND_CONFIRM_NO_SUBJECT ) )
            {
            retVal = KErrCancel;
            }
        }

    if ( retVal == KErrNone )
        {
        // When saving before sending we must update protocol fields and
        // set 2nd parameter == ETrue
        SetProtocolFieldsL( combinedEntry, ETrue );
        MMRUtilsTombsExt::TMRUtilsDbResult dbResult =
            iMRUtils.StoreL( combinedEntry, EFalse );
        // Saving may return positive values to report success
        if ( dbResult >= MMRUtilsTombsExt::EUndefined )
            {
            HBufC* note =
                StringLoader::LoadLC( R_TEXT_CALENDAR_MEETING_REQUEST_SAVED );
            CAknConfirmationNote* dlg =
                new( ELeave ) CAknConfirmationNote( ETrue );
            dlg->ExecuteLD( *note );
            CleanupStack::PopAndDestroy(); // note
            }
        else if ( dbResult != MMRUtilsTombsExt::EErrorIdenticalExists )
            { // identical exists is not an error in sending scenario
            retVal = KErrGeneral;
            }
        }

    if ( retVal == KErrNone )
        {
        iMRUtils.SendWithUiL( combinedEntry, boxToUse );

        if ( aSendCmd == EMRCommandSendUpdate )
            {
            // cancellation must be sent to removed attendees
            CCalEntry* cancel =
                CreateCancelForRemovedAttendeesL( combinedEntry );
            if ( cancel )
                {
                CleanupStack::PushL( cancel );
                iMRUtils.SendWithUiL( *cancel, boxToUse );
                CleanupStack::PopAndDestroy( cancel );
                
                // since we have now notified removed attendees we should
                // reset the array (although shouldn't make any difference
                // in practise)
                iOriginalAttendees.ResetAndDestroy();
                }
            }
        }

    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRCmdHandler::SendL
// User may edit attendee list multiple times in one ICalUI session,
// and may also save entry multiple times before sending it, therefore we
// should collect attendees of an existing entry in the startup phase, and
// then compare it to the edited list just before sending.
// ----------------------------------------------------------------------------
//
CCalEntry* CMRCmdHandler::CreateCancelForRemovedAttendeesL(
    const CCalEntry& aEntry ) const
    {
    CCalEntry* cancel = NULL;
    
    RPointerArray<CCalAttendee> removedAttendees; // temporary helper array
    CleanupClosePushL( removedAttendees ); // doesn't own its items
    TInt originalCount( iOriginalAttendees.Count() );
    for ( TInt i( 0 ); i < originalCount; ++i )
        {
        CCalAttendee* att = MREntryConsultant::EqualAttendeeL(
            *( iOriginalAttendees[i] ), aEntry );
        if ( !att )
            {
            removedAttendees.AppendL( iOriginalAttendees[i] );
            }
        }
        
    TInt removedCount( removedAttendees.Count() );
    if ( removedCount > 0 )
        {
        // Create a skeleton for the cancellation, use the same sequence number
        // as aEntry (the update that was just sent) has
        cancel = MRHelpers::CopyEntryLC( aEntry, 
                                         CCalEntry::EMethodCancel, 
                                         MRHelpers::ECopyOrganizer );
        for ( TInt j( 0 ); j < removedCount; ++j )
            {        
	        CCalAttendee* attCopy =
	            MRHelpers::CopyAttendeeLC( *( removedAttendees[j] ) );
	        cancel->AddAttendeeL( attCopy );
	        CleanupStack::Pop(); // attCopy, ownership was transferred
            }
            
        CleanupStack::Pop( cancel );
        }

    CleanupStack::PopAndDestroy(); // removedAttendees, just close the array
    
    return cancel; // may be NULL    
    }

// ----------------------------------------------------------------------------
// CMRCmdHandler::CancelMRL
// This method is only used from Calendar application, and in that case we
// always handle just one entry at a time (entire series or one instance).
// ----------------------------------------------------------------------------
//
TInt CMRCmdHandler::CancelMRL()
    {
    TMsvId boxToUse = MREntryConsultant::SendingMailBoxL( iInParams,
                                                          iMRMailboxUtils );
    TInt retVal( KErrNone );
    CCalEntry* combinedEntry = iModel.CombinedEntry();
    RPointerArray<CCalEntry> entryArray( 1 );
    entryArray.AppendL( combinedEntry );
    CleanupClosePushL( entryArray );

    retVal = iMRUtils.CancelWithUiL( entryArray, boxToUse );
    CleanupStack::PopAndDestroy(); // entryArray, only close arary
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRCmdHandler::DeleteMRL
// This method is only used from Calendar application, and in that case we
// always handle just one entry at a time (entire series or one instance).
// ----------------------------------------------------------------------------
//
TInt CMRCmdHandler::DeleteMRL()
    {
    TMsvId boxToUse = MREntryConsultant::SendingMailBoxL( iInParams,
                                                          iMRMailboxUtils );
    TInt retVal( KErrNone );
    CCalEntry& combinedEntry = *( iModel.CombinedEntry() );
    retVal = iMRUtils.DeleteWithUiL( combinedEntry, boxToUse );
    return retVal;
    }

// ----------------------------------------------------------------------------
// CMRCmdHandler::CreateReplyL
// ----------------------------------------------------------------------------
//
void CMRCmdHandler::CreateReplyL( TInt aReplyType )
	{
	__ASSERT_DEBUG( iInParams.iMessageId != KMsvNullIndexEntryId,
	                Panic( ETMsvIDNull ) );

    CCalEntry& combinedEntry = *( iModel.CombinedEntry() );

	CMRUtilsInternal::TMailRecipients aRecipients( CMRUtilsInternal::EAll );

	switch( aReplyType )
		{
		case EMRCommandReplyToSender:
			{
			aRecipients = CMRUtilsInternal::ESender;
			break;
			}
		case EMRCommandReplyToOrganiser:
			{
			aRecipients = CMRUtilsInternal::EOrganizer;
			break;
			}
		case EMRCommandReplyToAll:
			{
			aRecipients = CMRUtilsInternal::EAll;
			break;
			}
		}

    HBufC* senderAddr = MRHelpers::SenderAddressLC(
            *( iInParams.iMsgSession ),
            iInParams.iMessageId,
            EFalse );
    TMsvId boxToUse = MREntryConsultant::SendingMailBoxL( iInParams,
                                                          iMRMailboxUtils );
	iMRUtils.ReplyToL( aRecipients, combinedEntry, *senderAddr, boxToUse );
	CleanupStack::PopAndDestroy( senderAddr );
	}

// ----------------------------------------------------------------------------
// CMRCmdHandler::RemoveFromCalendarL
//
// ----------------------------------------------------------------------------
//
void CMRCmdHandler::RemoveFromCalendarL(
    const RPointerArray<CCalEntry>& aEntries )
	{
    CCalEntry& firstEntry = *( aEntries[0] );
	if ( MREntryConsultant::IsModifyingEntryL( firstEntry ) )
	    { // if entry at index 0 is modifying entry, then all entries are,
	      // that means that we remove each of them but originating remains,
	      // this branch is probably rarely, if ever, used in real life
	    TInt count( aEntries.Count() );
	    for ( TInt i( 0 ); i < count; ++i )
	        {
	        iMRUtils.DeleteL( *( aEntries[i] ) );
	        }
	    }
    else
        { // entry at index 0 is originating, all entries for this meeting
          // will get deleted, this is the usual case
        iMRUtils.DeleteL( firstEntry );
        }

	HBufC* removeNote = StringLoader::LoadL( R_QTN_CALE_NOTE_MEETING_REMOVED );
	CleanupStack::PushL( removeNote );
	CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
	globalNote->ShowNoteL( EAknGlobalInformationNote, *removeNote );
	CleanupStack::PopAndDestroy( globalNote );
	CleanupStack::PopAndDestroy( removeNote );
	}

// ----------------------------------------------------------------------------
// CMRCmdHandler::CreateResponseL
// This method may be called also from mail or BVA, and in that case we may
// have to deal with an array of entries.
// ----------------------------------------------------------------------------
//
TInt CMRCmdHandler::CreateResponseL(
    const RPointerArray<CCalEntry>& aEntries,
    TInt aResponseCmd )
	{
	CCalAttendee::TCalStatus status( CCalAttendee::ENeedsAction );
	switch ( aResponseCmd )
		{
		case EMRCommandRespondAccept:
			{
			status = CCalAttendee::EAccepted;
			break;
			}
		case EMRCommandRespondTentative:
			{
			status = CCalAttendee::ETentative;
			break;
			}
		case EMRCommandRespondDecline:
			{
			status = CCalAttendee::EDeclined;
			break;
			}
		default:
			{
			User::Leave( KErrNotSupported );
			break;
			}
		}
    // TODO: Currently we are sending MR response via the mailbox which is stored
    // in cenrep settings, but it has been discussed that actually we should send
    // via the mailbox that is associated with ThisAttendeeL() email address.
    TMsvId boxToUse = MREntryConsultant::SendingMailBoxL( iInParams,
                                                          iMRMailboxUtils );
    return iMRUtils.RespondWithUiL( aEntries, status, boxToUse );
	}

void CMRCmdHandler::DisableAlarmL(
    const RPointerArray<CCalEntry>& aEntries )
	{
    TInt count( aEntries.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        CCalEntry& entry = *( aEntries[i] );
	    CCalEntry* dBEntry = iMRUtils.FetchEntryL( entry.UidL(),
	                                               entry.RecurrenceIdL() );
    	if ( dBEntry )
    		{
    	    CleanupStack::PushL( dBEntry );
    		dBEntry->SetAlarmL( NULL );
    		iMRUtils.UpdateEntryL( *dBEntry );
            CleanupStack::PopAndDestroy( dBEntry );
    		}
        }
	}

// this method may be moved to some other class if other classes than
// just CmdHandler need to use it.
void CMRCmdHandler::SetProtocolFieldsL(
    CCalEntry& aEntry,
    TBool aWillBeSentNow ) const
    {
    __ASSERT_DEBUG( iMRMailboxUtils.IsOrganizerL( aEntry ),
                    Panic( ENonOrganizerSettingProtocolFields ) );    
    
    // TODO: set seq number, do we need to compare entry to db entry and
    // what fields have changed and what type of entry it is?

    // Store information whether this request / this particular entry
    // will be sent now or just saved.

    if ( aWillBeSentNow )
        { // TODO: this will be done differently when Symbian supports other fields
          // to indicate sending status
        TTime currentTime;
        currentTime.UniversalTime();
        TCalTime dtstamp;
        dtstamp.SetTimeUtcL( currentTime );
        aEntry.SetDTStampL( dtstamp );
        if ( aEntry.StatusL() == CCalEntry::ENullStatus )
            { // If entry has had ENullStatus, we use now EConfirmed
              // as a stamp to mark that entry has been sent at least once.
            aEntry.SetStatusL( CCalEntry::EConfirmed );
            }
        }
    else
        {
        TCalTime dtstamp; // initialized to null time
        aEntry.SetDTStampL( dtstamp );
        }    
    }

//  End of File

