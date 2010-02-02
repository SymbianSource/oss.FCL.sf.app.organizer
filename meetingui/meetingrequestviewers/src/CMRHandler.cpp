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
* Description: Implementation for meeting request handler   
*
*/




// INCLUDE FILES
#include "CMRHandler.h"
#include "MMRModelInterface.h"		
#include "MREntryConsultant.h"	
#include "CMRObjectFactory.h"			
#include "CMRCmdHandler.h"
#include "mrdatalog.h"
#include "ICalUILog.h"
#include "meetingrequestviewers.hrh" 	//constants for meeting request viewers
#include <CAttendeeView.h>				//attendees view
#include <MAgnEntryUi.h>				//TAgnEntryUiInParams & entry ui callback
#include "CMRUtilsInternal.h"			//Meeting request utilities library
#include <MRCommands.hrh> 				//common commands for attendee, editor and meeting request views
#include <AknGlobalNote.h>
#include <CalenEditorsPlugin.h> 		//calendar editors ecom plugin
#include <stringloader.h>				//resource string loading
#include <cmrmailboxutils.h>
#include <AknQueryDialog.h>
#include "ICalUILog.h"
#include "MRViewersPanic.h"

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CMRHandler" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMRHandler::CMRHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMRHandler::CMRHandler(
    RPointerArray<CCalEntry>& aEntries,
    const MAgnEntryUi::TAgnEntryUiInParams& aParams,
	MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
    MAgnEntryUiCallback& aCallback )
    : iEntries( aEntries ),
      iInParams( aParams ),
      iOutParams( aOutParams ),
      iCallback( aCallback )
    {
    LOG("CMRHandler::CMRHandler()");
    iStatus = EInitialView;
    }

// -----------------------------------------------------------------------------
// CMRHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//

void CMRHandler::ConstructL()
    {	                           
    LOG("CMRHandler::ConstructL");
    //calling app is calendar, create calendar editor ecom plugin
    if (iInParams.iCallingApp.iUid == KUidCalendarApplication )
    	{
    	iCalendarEditorsPlugin = CCalenEditorsPlugin::NewL();
    	}
    LOG("CMRHandler::ConstructL -> End");
    }

// -----------------------------------------------------------------------------
// CMRHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMRHandler* CMRHandler::NewL(
    RPointerArray<CCalEntry>& aEntries,
    const MAgnEntryUi::TAgnEntryUiInParams& aParams,
    MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
    MAgnEntryUiCallback& aCallback )
    {
    CMRHandler* self = new( ELeave ) CMRHandler( aEntries,
                                                 aParams,
                                                 aOutParams,
                                                 aCallback );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CMRHandler::~CMRHandler()
    {
    LOG("CMRHandler::~CMRHandler");
	delete iModel;
	delete iCalendarEditorsPlugin;
	delete iCmdHandler;
    // entry arrray only referenced so iEntries mustn't be closed
    LOG("CMRHandler::~CMRHandler -> End");
    }

// -----------------------------------------------------------------------------
// CMRHandler::IsCommandAvailable
// -----------------------------------------------------------------------------
//
TBool CMRHandler::IsCommandAvailable( TInt aCommandId )
    {
    if ( aCommandId == EMRCommandRetrieve ||
         aCommandId == EMRCommandAttachments ||
         aCommandId == EMRCommandUnreadOpeningNote )
        {
        return iCallback.IsCommandAvailable( aCommandId );
        }
    else
        {        
        // otherwise caller knows the availability
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CMRHandler::ProcessCommandWithResultL
// -----------------------------------------------------------------------------
//
TInt CMRHandler::ProcessCommandWithResultL( TInt aCommandId )
	{
	TInt retVal( KErrNone );
	
	switch( aCommandId )
		{				
		case EMRCommandExitDialogs:
	    case EAknCmdExit:
	    case EEikCmdExit:		
			{
			iStatus = ECloseMeetingRequestViews;
			break;
			}
		case EMRCommandReplyToSender:   // fall through
		case EMRCommandReplyToOrganiser:// fall through
		case EMRCommandReplyToAll:      
			{
			//do reply
			iCmdHandler->CreateReplyL( aCommandId );
			iStatus = ECloseMeetingRequestViews;
			break;
			}			
		case EMRCommandShowDescriptionView:
			{
			if ( iInParams.iCallingApp.iUid == KUidCalendarApplication &&
			     iInParams.iEditorMode != MAgnEntryUi::EViewEntry )
				{ // ECreateNewEntry, EEditExistingEntry
				iStatus = ELaunchMeetingEditorView;
                }
            else
				{
				iStatus = ELaunchMeetingDescriptionView;
				}
            break;
			}
		case EMRCommandShowAttendeesView:
			{
			//set status to launch attendee view
			iStatus = ELaunchAttendeeView;
			break;
			}
		case EMRCommandShowSummaryView:
			{
			//set status to launch summary view;
			iStatus = ELaunchMeetingRequestView;
			break;
			}			
        // forward actually closes MR Viewer (mail framework does that)			
		case EMRCommandForward:
		case EMRCommandAttachments:
		case EMRCommandMessageDetails:
        case EMRCommandRetrieve:
			{
			retVal = iCallback.ProcessCommandWithResultL( aCommandId );
			break;
			}
		case EMRCommandNavigateBackward:
		case EMRCommandNavigateForward:
			{
			// check if mailbox has other entries so that navigation is possible
			if ( iCallback.IsCommandAvailable( aCommandId ) )
				{
				TBool allowExit( ETrue );
				
				// TODO: check exit condition for MR, present user a dialog with
				//  saving & sending options... same as when closing the view

				
				// pass command to callback, framework takes care of the rest
				if ( allowExit )
					{
					iStatus = ECloseMeetingRequestViews;
					retVal = iCallback.ProcessCommandWithResultL( aCommandId );
					}
				else
					{
					// user cancelled navigation
					retVal = KErrCancel;
					}
				}
			else
				{
				retVal = KErrNotSupported;
				}
            break;
			}	
			
		// commands which modify or might possibly modify entry(s), and which
		// cause exit mr viewer if completed succesfully:
		case EMRCommandSend:            // fall through
		case EMRCommandSendUpdate:      // fall through
		case EMRCommandCancelMR:        // fall through
		case EMRCommandDeleteMR:        // fall through
		case EMRCommandRespondAccept:   // fall through
		case EMRCommandRespondTentative:// fall through
		case EMRCommandRespondDecline:  // fall through		
		case EMRCommandSaveAndExit:	
            {
            TRAPD( err, retVal = ProcessTrappedModifCmdL( aCommandId ) );
            if ( err != KErrNone || retVal != KErrNone )
                { // error result or leave occurred, don't exit viewer but 
                  // ensure data integrity
                // TODO: show error note possibly?
                RefreshViewL();
                User::LeaveIfError( err );
                }
            else
                {
                iStatus = ECloseMeetingRequestViews;
                }
            break;
            }
				
        // commands which modify or might possibly modify entry(s), and which
        // do not exit mr viewer:
		case EMRCommandSave:               // fall through
		case EMRCommandDisableAlarm:       // fall through
		case EMRCommandRemoveFromCalendar: // fall through
		case EMRCommandSaveFromFile:
		    {
		    // trap leaves to ensure data integrity
            TRAPD( err, retVal = ProcessTrappedModifCmdL( aCommandId ) );
            RefreshViewL();
            User::LeaveIfError( err );
            break;
		    }
		    			
			/*
		case EMRCommandHelpMeetingDescriptionView:
			{
			//do help showing of currently displayed view
			break;
			}
		case EMRCommandHelpMeetingRequestView:
			{
			//do showing of help for meeting request view
			break;
			}*/
		}
        
	return retVal;
	}

// -----------------------------------------------------------------------------
// CMRHandler::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CMRHandler::ProcessCommandL( TInt aCommandId )
	{
	ProcessCommandWithResultL( aCommandId );
	}

// -----------------------------------------------------------------------------
// CMRHandler::ProcessTrappedModifCmdL
// Commands which may modify entry(s) and should be placed inside a TRAP to
// ensure that data gets refreshed also in case a leave occurs when processing
// command.
// -----------------------------------------------------------------------------
//
TInt CMRHandler::ProcessTrappedModifCmdL( TInt aCommandId )
    {
    LOG("CMRHandler::ProcessTrappedModifCmdL");
    TInt retVal( KErrNone );
    
	switch( aCommandId )
		{    
		case EMRCommandSend:
		case EMRCommandSendUpdate:
		    { // do send request or update
		    retVal = iCmdHandler->SendL( aCommandId );
			break;
		    }
		case EMRCommandCancelMR:
		    { // do cancel request
		    retVal = iCmdHandler->CancelMRL();
			break;
		    }
		case EMRCommandDeleteMR:
		    { // do delete request
            retVal = iCmdHandler->DeleteMRL();
			break;
		    }		    
		case EMRCommandRespondAccept:
		case EMRCommandRespondTentative: // fall through
		case EMRCommandRespondDecline:   // fall through
			{
			//do respond to organiser
			iCmdHandler->CreateResponseL( iEntries, aCommandId );
			break;
			}
		case EMRCommandSaveAndExit:
		    {
		    iCmdHandler->SaveL();
		    break;
		    }		    
		case EMRCommandSave:
		    {
		    iCmdHandler->SaveL();
		    break;
		    }		    				    
		case EMRCommandDisableAlarm:
			{
			iCmdHandler->DisableAlarmL( iEntries );
			break;
			}
		case EMRCommandRemoveFromCalendar:
			{
			iCmdHandler->RemoveFromCalendarL( iEntries );
			break;
			}
		case EMRCommandSaveFromFile:
		    {
		    iCmdHandler->SaveFromFileL( iEntries );
		    break;
		    }
        default:
            {
            User::Leave( KErrNotSupported );
            }
		}
    LOG("CMRHandler::ProcessTrappedModifCmdL -> End");		
    return retVal;
    }

// -----------------------------------------------------------------------------
// CMRHandler::SetHelpContext
// -----------------------------------------------------------------------------
//
void CMRHandler::SetHelpContext( const TCoeHelpContext& /*aContext*/ )
	{
	}

// -----------------------------------------------------------------------------
// CMRHandler::ShowOpeningNoteL
// -----------------------------------------------------------------------------
//
void CMRHandler::ShowOpeningNoteL()
	{
	HBufC* openingNote = NULL;

	if ( !iModel )
	    { // error has occurred since model hasn't been created
	    // TODO: implement error note or other way of handling the error
	    }
    else
        {
    	openingNote = iModel->MailboxOpeningNoteL();
        }

	if ( openingNote )
		{
		CleanupStack::PushL( openingNote );
		CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
		globalNote->ShowNoteL( EAknGlobalInformationNote, *openingNote );
		CleanupStack::PopAndDestroy( globalNote );
		CleanupStack::PopAndDestroy( openingNote );
		}
	}

// -----------------------------------------------------------------------------
// CMRHandler::HandleEngReadyL
// -----------------------------------------------------------------------------
//
void CMRHandler::HandleEngReadyL(
    const TDesC8& /*aMtmUid*/,
    CMRUtilsInternal& aMRUtils,
    CMRMailboxUtils& aMRMailboxUtils )
    {
    // this method should only get called once
    __ASSERT_DEBUG( !iCmdHandler && !iModel, Panic( EUnexpectedNonNull ) );
                    
    LOG("CMRHandler::HandleEngReadyL, creating iModel");
    iModel = CMRObjectFactory::CreateMRModelL( iEntries, 
                                               iInParams, 
                                               aMRMailboxUtils,
                                               aMRUtils );

    #ifdef ICALUI_ENABLE_MRDATA_LOGGING
        LogAvailableFunctionsL();
    #endif

    LOG("CMRHandler::HandleEngReadyL, creating iCmdHandler");
    iCmdHandler = CMRCmdHandler::NewL( *iModel,
                                       iInParams,
                                       aMRMailboxUtils,
                                       aMRUtils );

    iCurrentDialog->SetModel( *iModel );

    if ( IsCommandAvailable( EMRCommandUnreadOpeningNote ) )
        {            
    	// TODO: if combinedEntry is NULL, then there was some error.
    	// ShowOpeningNoteL() should take care of notifying user in this case.
    	ShowOpeningNoteL();
        }
	}

// -----------------------------------------------------------------------------
// CMRHandler::ExecuteViewL
// -----------------------------------------------------------------------------
//
TInt CMRHandler::ExecuteViewL()
	{
    LOG("CMRHandler::ExecuteViewL");
	// Even if there are multiple entries, method is the same for all:
	CCalEntry::TMethod method = iEntries[0]->MethodL();

    // for returning from attendee view or calendar editor
    TMRViewStatus previousView( EInitialView ); 

	TInt returnCode( KErrNone );
	while ( iStatus != ECloseMeetingRequestViews )
		{
		if ( iStatus == EInitialView ||
		     iStatus == ELaunchMeetingRequestView ||
		     iStatus == ELaunchMeetingDescriptionView )
		    {
		    previousView = iStatus;
		    
            LOG("CMRHandler::ExecuteViewL, creating dialog");
			iCurrentDialog = CMRObjectFactory::CreateMRDialogL( method,
			                                                    iInParams,
			                                                    *this,
			                                                    iStatus );
		    if ( iModel )
		        { // Set model if it is already available
		            LOG("CMRHandler::ExecuteViewL, setting model");
		        iCurrentDialog->SetModel( *iModel );
		        }
		    LOG("CMRHandler::ExecuteViewL, executing dialog");
			returnCode = iCurrentDialog->ExecuteLD();
			iCurrentDialog = NULL;
		    }		
		else if ( iStatus == ELaunchAttendeeView )
		    {
			returnCode = ExecuteAttendeeViewL();
			//return to the view the attendee view was launched from
			iStatus = previousView;		    
		    }
        else if ( iStatus == ELaunchMeetingEditorView )
            {
			returnCode = ExecuteEditorViewL();
			//return to the view the editor was launched from
			iStatus = previousView;            
            }
		}
    LOG("CMRHandler::ExecuteViewL -> End");
	return returnCode;
	}

// -----------------------------------------------------------------------------
// CMRHandler::ExecuteEditorViewL
// -----------------------------------------------------------------------------
//
TInt CMRHandler::ExecuteEditorViewL()
    {
    QueryAndSetEditingModeL();    
    
    RPointerArray<CCalEntry> tmpArray( 1 );
    CleanupClosePushL( tmpArray ); // only close, doesn't own it's entry
    tmpArray.AppendL( iModel->CombinedEntry() );
    TInt retVal = iCalendarEditorsPlugin->ExecuteViewL( tmpArray,
                                                        iInParams, 
                                                        iOutParams, 
                                                        *this );
    if ( retVal == KErrNone &&
         iOutParams.iAction == MAgnEntryUi::EMeetingSaved )
        {
        iModel->SetEntryEdited();
        }
    CleanupStack::PopAndDestroy(); // tmpArray
    return retVal;
    }

// -----------------------------------------------------------------------------
// CMRHandler::ExecuteAttendeeViewL
// -----------------------------------------------------------------------------
//
TInt CMRHandler::ExecuteAttendeeViewL()
	{
	User::Leave( KErrNotSupported );
	return KErrNotSupported;
	
	/*
	QueryAndSetEditingModeL();	
	CAttendeeView::TAttendeeViewMode mode( CAttendeeView::EViewer );
	if ( iModel->EditingMode() != MMRModelInterface::EViewOnly )
		{
        mode = CAttendeeView::EEditor;
		}
	
	// TODO: we may get into trouble if combined entry changes while
	// attendee view is still running -> we might need to use another
	// copy with attendee view
	
    CCalEntry* entry = iModel->CombinedEntry();
    
    // TODO: currently we don't resolve phone owner addr, when
	// attendee view API has been changed we might instantiate attendee
	// view already e.g. in HandleEngReadyL(). Fix this then...
	const TDesC& address = KNullDesC;
	    //MREntryConsultant::PhoneOwnerAddrL( *entry, *iMRMailboxUtils );

	TInt filters( 0 );
	// TODO: Attendee view should support individual Accept/Tentative/Decline cmds
	if ( iModel->IsCmdAvailable( EMRCommandRespondAccept ) )
		{
		filters =+ CAttendeeView::ERespond;
		}
	if ( iInParams.iCallingApp.iUid == KUidCalendarApplication )
		{
		if ( iModel->IsCmdAvailable( EMRCommandSend ) )
		    {
		    filters =+ CAttendeeView::ESend;
		    }
		if ( iModel->IsCmdAvailable( EMRCommandSendUpdate ) )
		    {
		    filters =+ CAttendeeView::ESendUpdate;
		    }
		if ( iModel->IsCmdAvailable( EMRCommandCancelMR ) )
		    {
		    filters =+ CAttendeeView::ECancelation;
		    }
		}
	CAttendeeView::TMenuFilterFlags menuFilter(
	    static_cast<CAttendeeView::TMenuFilterFlags>( filters ) );
	    
    // Unfortunately we need to create initParams in heap since
    // attendee view wants to take it's ownership
	CAttendeeView::TAttendeeViewInit* initParams =
	    new( ELeave ) CAttendeeView::TAttendeeViewInit(
	        *entry, mode, CONST_CAST( TDesC&, address ), *this );
    CleanupStack::PushL( initParams );
	initParams->SetMenuFilter( menuFilter );
	
	CAttendeeView* attendeeView = CAttendeeView::NewL( initParams );
	CleanupStack::Pop(); // initParams, ownership transferred
    CleanupStack::PushL( attendeeView );
	
	TInt retVal = attendeeView->ExecuteL();
	
	// Note: without calling IsEntryEditedL() attendee view
	// wouldn't update the input entry!
    if ( attendeeView->IsEntryEditedL() )
        {
        iModel->SetEntryEdited();
        }
    CleanupStack::PopAndDestroy( attendeeView );
    
    return retVal;
    */
	}

// -----------------------------------------------------------------------------
// CMRHandler::RefreshViewL
// -----------------------------------------------------------------------------
//
void CMRHandler::RefreshViewL()
    {
    if ( iCurrentDialog )
        {
        iModel->RefreshViewableEntryL();
        iCurrentDialog->RefreshData();
        }
    }

// -----------------------------------------------------------------------------
// CMRHandler::QueryAndSetEditingModeL
// -----------------------------------------------------------------------------
//
void CMRHandler::QueryAndSetEditingModeL()
    {
    if ( iModel->EditingMode() == MMRModelInterface::EModeNotSet )
        {
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        // TODO: replace this with Calendar Utils query when available
        _LIT(KQueryText,"Edit series instead of instance?");
        CleanupStack::PushL( dlg );
        dlg->SetPromptL( KQueryText );
        CleanupStack::Pop( dlg );
        if ( dlg->ExecuteLD( R_GENERAL_CONFIRMATION_QUERY ) )
            {
            iModel->SetEditingModeL( MMRModelInterface::EEditMeeting );
            }
        else
            {
            iModel->SetEditingModeL( MMRModelInterface::EEditInstance );
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CMRHandler::LogAvailableFunctionsL
// This method is for testing purposes only, and should not be called in real
// builds to avoid caused overhead.
// Not all commands listed in MRCommands.hrh are checked here, only the ones
// that are feasible. Furthermore, this may not correspond 100% to the items
// visible in the options menu, these results reflect only model's opinion.
// -----------------------------------------------------------------------------
//
void CMRHandler::LogAvailableFunctionsL() const
    {
    TBool res( EFalse );    
    MRDATA_LOG("# start function availability check #");    
    res = iModel->IsCmdAvailable( EMRCommandRespondAccept );
    MRDATA_LOG1("can EMRCommandRespondAccept: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandRespondTentative );
    MRDATA_LOG1("can EMRCommandRespondTentative: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandRespondDecline );
    MRDATA_LOG1("can EMRCommandRespondDecline: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandReplyToSender );
    MRDATA_LOG1("can EMRCommandReplyToSender: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandReplyToOrganiser );
    MRDATA_LOG1("can EMRCommandReplyToOrganiser: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandReplyToAll );
    MRDATA_LOG1("can EMRCommandReplyToAll: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandForward );
    MRDATA_LOG1("can EMRCommandForward: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandRetrieve );
    MRDATA_LOG1("can EMRCommandRetrieve: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandRemoveFromCalendar );
    MRDATA_LOG1("can EMRCommandRemoveFromCalendar: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandShowCalendar );
    MRDATA_LOG1("can EMRCommandShowCalendar: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandAttachments );
    MRDATA_LOG1("can EMRCommandAttachments: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandMessageDetails );
    MRDATA_LOG1("can EMRCommandMessageDetails: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandDisableAlarm );
    MRDATA_LOG1("can EMRCommandDisableAlarm: %d", res );    
    res = iModel->IsCmdAvailable( EMRCommandSaveFromFile );
    MRDATA_LOG1("can EMRCommandSaveFromFile: %d", res );    
    res = iModel->IsCmdAvailable( EMRCommandSend );
    MRDATA_LOG1("can EMRCommandSend: %d", res );    
    res = iModel->IsCmdAvailable( EMRCommandSendUpdate );
    MRDATA_LOG1("can EMRCommandSendUpdate: %d", res );
    res = iModel->IsCmdAvailable( EMRCommandDeleteMR );
    MRDATA_LOG1("can EMRCommandDeleteMR: %d", res );       
    MRDATA_LOG("# end function availability check #");        
    }
    
//  End of File
