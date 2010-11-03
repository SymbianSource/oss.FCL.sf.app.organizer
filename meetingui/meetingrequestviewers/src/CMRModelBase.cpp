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
* Description: Implementation for meeting request model base 
*
*/




// INCLUDE FILES
#include "CMRModelBase.h"
#include "MRHelpers.h"
#include "ICalUILog.h"
#include "MREntryConsultant.h"
#include "CMRUtilsInternal.h"
#include "MRViewersPanic.h" 					//panic codes
#include "meetingrequestviewers.hrh" 	        //common constants
#include "ProcessingStructs.h" 					//scenario structures for meeting request processing
#include "CMRProcessor.h"
#include "CMRInfoPopup.h"
#include <e32std.h>
#include <calentry.h> 		//CCalEntry (Calendar entry API V2)
#include <caluser.h> 		//caluser and attendee 
#include <calalarm.h>
#include <MsgMailUIDs.h> 	//uid for mail application
#include <cemailaccounts.h>
#include <smtpset.h>
#include <imapset.h>
#include <eikenv.h>
#include <pop3set.h>
#include <MuiuMsvUiServiceUtilities.h> 
#include <SendUiConsts.h> 						//mailbox uids
#include <stringloader.h> 						//loading of string resources
#include <meetingrequestviewersuires.rsg> 			//resource definitions header
#include <cmrmailboxutils.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CMRModelBase" );

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
CMRModelBase::CMRModelBase(
    CMRMailboxUtils& aMailboxMRUtils,
    CMRUtilsInternal& aMRUtils,
    const MAgnEntryUi::TAgnEntryUiInParams& aInParams )
    : iMRUtils( aMRUtils ),
      iMRMailboxUtils(aMailboxMRUtils),
      iInParams( aInParams ),
      iIsEntryEdited( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// ?classname::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMRModelBase::ConstructL( RPointerArray<CCalEntry>& aEntries )
    {
    LOG("CMRHandler::ConstructL, creating processor");    
    iMRProcessor = CMRProcessor::NewL( iMRMailboxUtils,
                                       iMRUtils,
                                       iInParams,
                                       aEntries );

    LOG("CMRHandler::ConstructL, processing entries");
    iMRProcessor->ProcessEntriesL();    
    }

// Destructor
CMRModelBase::~CMRModelBase()
    {
    delete iMRProcessor;
    }
    
CCalEntry::TMethod CMRModelBase::MethodL()
	{
	return CombinedEntry()->MethodL();
	}

MMRInfoPopup* CMRModelBase::CreateInfoPopupL()
    {
    TBool mode( iInParams.iEditorMode ==
        MAgnEntryUi::EViewEntry ? ETrue : EFalse );
    return CMRInfoPopup::NewL( CEikonEnv::Static(),
                               iMRMailboxUtils,
                               *( CombinedEntry() ),
                               mode );
    }

TBool CMRModelBase::IsCmdAvailable( TInt aCommandId )
    {
    TInt retVal( EFalse );
    TRAP_IGNORE( retVal = IsCmdAvailableL( aCommandId ) );
    // command not available in case of error
    return retVal;
    }

TBool CMRModelBase::IsCmdAvailableL( TInt aCommandId )
    {
    TInt retVal( EFalse );
    switch ( aCommandId )
        {
        case EMRCommandDisableAlarm:
            {
            retVal = CanDisableAlarmL();
            break;
            }
        case EMRCommandAttachments:
            {
            retVal = CanShowAttachmentsL();
            break;
            }
        case EMRCommandMessageDetails:
            {
            retVal = CanShowInfoL();            
            break;
            }
        case EMRCommandRetrieve:
            {
            retVal = CanRetrieveL();            
            break;
            }
        case EMRCommandSaveFromFile:
            {
            retVal = CanSaveFromFileL();
            break;
            }
        default:
            {
            retVal = EFalse;
            break;
            }                                     
        }
    return retVal;
    }

HBufC* CMRModelBase::MailboxOpeningNoteL()
	{
	TInt resource( -1 );
	if ( MREntryConsultant::IsCancelledL( *CombinedEntry(), iMRMailboxUtils ) )
		{
		resource = R_QTN_CALE_MEETING_IS_CANCELLED;
		}
	else if ( iMRProcessor->ProcessingDbResult() ==
	            MMRUtilsTombsExt::EErrorObsolete )
		{
		resource = R_QTN_CALE_MEETING_IS_OUT_OF_DATE;
		}
    // other error cases:
	else if ( iMRProcessor->ProcessingDbResult() <= ENullOp )
		{
		resource = R_QTN_CALE_MEETING_IS_CORRUPTED;
		}
    else
        {
        return NULL;
        }
	return StringLoader::LoadL( resource, CCoeEnv::Static() );
	}

MMRModelInterface::TEditingMode CMRModelBase::EditingMode() const
    {
    return iMRProcessor->EditingMode();
    }
    	
void CMRModelBase::SetEditingModeL( MMRModelInterface::TEditingMode aEditMode )
    {
    iMRProcessor->SetEditingModeL( aEditMode );
    }

void CMRModelBase::SetEntryEdited()
    {
    iIsEntryEdited = ETrue;
    }
		
TBool CMRModelBase::IsEntryEdited()
    {
    return iIsEntryEdited;    
    }
    
TBool CMRModelBase::CanDisableAlarmL()
	{
	TBool retVal( EFalse );
	const CCalEntry& entry = *( CombinedEntry() );
	if ( IsEntryUsableL( entry ) )
		{
		CCalAlarm* alarm = entry.AlarmL(); // ownership transferred		
		if ( alarm )
		    {
		    delete alarm;
			retVal = ETrue;
		    }
		else
		    {		    
			retVal = EFalse;
		    }
		}
    return retVal;
	}	
	
TBool CMRModelBase::CanShowAttachmentsL()
	{	
	if ( iInParams.iCallingApp.iUid == KUidMailApplication )
		{
		return ETrue;
		}
    else
        {
        return EFalse;
        }
	}

TBool CMRModelBase::CanShowInfoL()
	{
	if ( iInParams.iCallingApp.iUid == KUidMailApplication )
		{
        return ETrue;
		}
    else
        {
        return EFalse;
        }
	}

TBool CMRModelBase::CanRetrieveL()
	{
	if ( iInParams.iCallingApp.iUid == KUidMailApplication )
		{
        return ETrue;
		}
    else
        {
        return EFalse;
        }
	}
	
TBool CMRModelBase::CanSaveFromFileL()
	{
	const CCalEntry& entry = *( CombinedEntry() );
	
	// allow storing cancelled request to db
	// -> IsEntryUsableL 2nd param == ETrue
	// check if exactly identical entry exists in db
	// -> ExistsInDbL 3rd param == ETrue
	if ( iInParams.iCallingApp.iUid == KUidBVAApplication &&
	     IsEntryUsableL( entry, ETrue ) &&
	     !MREntryConsultant::ExistsInDbL( entry, iMRUtils, ETrue ) )
		{
        return ETrue;
		}
    else
        {
        return EFalse;
        }
	}
	
CCalEntry* CMRModelBase::CombinedEntry()
	{
	return iMRProcessor->CombinedEntry();
	}
	
void CMRModelBase::RefreshViewableEntryL()
    {
    iMRProcessor->RefreshViewableEntryL();
    }
	
TBool CMRModelBase::IsSenderOrganizerL()
    {
    __ASSERT_DEBUG( iInParams.iCallingApp.iUid == KUidMailApplication,
                    Panic( EIllegalCallerApp ) );
    __ASSERT_DEBUG( iInParams.iMsgSession,
                    Panic( EMsvSessionNull ) );
    __ASSERT_DEBUG( iInParams.iMessageId != KMsvNullIndexEntryId,
                    Panic( ETMsvIDNull ) );
                    
    TBool retVal( EFalse );
    CCalUser* organizer = CombinedEntry()->OrganizerL();
    if ( organizer )
        {
        HBufC* senderAddr = MRHelpers::SenderAddressLC(
            *( iInParams.iMsgSession ),
            iInParams.iMessageId,
            ETrue );
            
        TPtrC organizerAddr =
            MRHelpers::AddressWithoutMailtoPrefix( organizer->Address() );
        
        if ( senderAddr->CompareF( organizerAddr ) == 0 )
            {
            retVal = ETrue;
            }
        CleanupStack::PopAndDestroy( senderAddr );
        }
    return retVal;
    }
    
TBool CMRModelBase::MailHeadersAvailable()
    {
    return ( iInParams.iMessageId != KMsvNullIndexEntryId );
    }
    
HBufC* CMRModelBase::MailHeaderFromFieldLC()
	{
	return iMRUtils.MailHeaderFromFieldLC( iInParams.iMessageId );
	}

HBufC* CMRModelBase::MailHeaderSubjectFieldLC()
	{
	return iMRUtils.MailHeaderSubjectFieldLC( iInParams.iMessageId );
	}

TBool CMRModelBase::IsEntryUsableL(
    const CCalEntry& aEntry,
    TBool aAllowCancelled )
    {
    TBool retVal( EFalse );
    if ( iMRProcessor->ProcessingResultOp() > ENullOp &&
         !MREntryConsultant::IsEntryOutOfDateL( aEntry ) )
        {
        if ( aAllowCancelled || 
             !MREntryConsultant::IsCancelledL( aEntry, iMRMailboxUtils ) )
            {            
            retVal = ETrue;
            }
        }
    return retVal;
    }
    
TBool CMRModelBase::IsSendableL( const CCalEntry& aEntry )
    {
    TBool retVal( EFalse );
    if ( IsEntryUsableL( aEntry ) )
        {        
        RPointerArray<CCalAttendee>& attendees = aEntry.AttendeesL();
        TInt count( attendees.Count() );    
        for ( TInt i( 0 ); i < count; ++i )
            {
            if ( attendees[i]->StatusL() != CCalAttendee::EDeclined )
                { // there exists at least one attendee to send to
                retVal = ETrue;
                break;
                }
            }
        }
    return retVal;
    }
    
//  End of File
