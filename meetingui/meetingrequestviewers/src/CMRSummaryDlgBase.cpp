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
* Description:   Implementation of base summary dialog
*
*/



// INCLUDE FILES
#include "CMRSummaryDlgBase.h"
#include "ICalUILog.h"
#include "MMRModelInterface.h" //MRModelInterface
#include "MMRInfoPopup.h"
#include "meetingrequestviewers.hrh" //common constants and commands
#include <MAgnEntryUi.h> //MAgnEntryUi
#include "MRViewersPanic.h" //panic codes
#include <eikmenup.h> //menupane
#include <MRCommands.hrh>
#include <e32std.h> //standard epoc
#include <avkon.hrh> 	//enums
#include <aknlists.h>	//CAknDoubleLargeStyleListBox
#include <gulicon.h>	//icons
#include <AknIconArray.h>
#include <meetingrequestviewers.mbg>
#include <meetingrequestviewersuires.rsg>
#include <stringloader.h> //stringloader for resources
#include <AknListQueryDialog.h>
#include <AknQueryDialog.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CMRSummaryDlgBase" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMRSummaryDlgBase::CMRSummaryDlgBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMRSummaryDlgBase::CMRSummaryDlgBase(
    MAgnEntryUiCallback& aHandlerCallback,
    MAgnEntryUi::TAgnEntryUiEditorMode aEditorMode )
    : CMRDialogBase( aHandlerCallback ),
      iEditorMode( aEditorMode )
    {
    }

// -----------------------------------------------------------------------------
// CMRSummaryDlgBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMRSummaryDlgBase::ConstructL()
    {
    CAknDialog::ConstructL( R_MR_SUMMARYDLG_OPTIONS_MENUBAR );
	
    //idle timer for dialog exit from list box
    iIdleDialogExit = CIdle::NewL( CActive::EPriorityIdle );
    }

void CMRSummaryDlgBase::ConstructListBoxL()   
	{		
    LOG("CMRSummaryDlgBase::ConstructListBoxL, creating iLBModel");
    	
	iLBModel = CMRSummaryListBoxModel::NewL( iEikonEnv );  
	
    // if iMRModel not yet available it will be set later	
	if ( iMRModel )  
	    {	    
	    iLBModel->SetModel( *iMRModel );
	    }
	
    LOG("CMRSummaryDlgBase::ConstructListBoxL, creating iListBox");	
	iListBox = new (ELeave) CAknDoubleLargeStyleListBox;	
	iListBox->SetContainerWindowL( *this );
	TInt flags( EAknGenericListBoxFlags );
	iListBox->ConstructL( this, flags ); 	
	iListBox->SetListBoxObserver( this );
	
	// Create scroll bar
	iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
       
    CTextListBoxModel* lbModel = iListBox->Model();
    lbModel->SetItemTextArray( iLBModel );
    lbModel->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
    LOG("CMRSummaryDlgBase::ConstructListBoxL, creating icon list");	    
    CAknIconArray* iconList = new( ELeave ) CAknIconArray( 2 );
    CleanupStack::PushL( iconList );
    iconList->ConstructFromResourceL( R_LISTBOX_ICONS );
    iListBox->ItemDrawer()->FormattedCellData()->SetIconArrayL( iconList );
        
	CleanupStack::Pop( iconList );

    LOG("CMRSummaryDlgBase::ConstructListBoxL -> End");	    	
	}

// -----------------------------------------------------------------------------
// CMRSummaryDlgBase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMRSummaryDlgBase* CMRSummaryDlgBase::NewL(
    MAgnEntryUiCallback& aHandlerCallback,
    MAgnEntryUi::TAgnEntryUiEditorMode aEditorMode )
    {
    CMRSummaryDlgBase* self =
        new( ELeave ) CMRSummaryDlgBase( aHandlerCallback, aEditorMode );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CMRSummaryDlgBase::~CMRSummaryDlgBase()
    {
    LOG("CMRSummaryDlgBase::~CMRSummaryDlgBase");    
    delete iInfoPopup;
    delete iListBox;
	delete iLBModel;    
    delete iIdleDialogExit;
    LOG("CMRSummaryDlgBase::~CMRSummaryDlgBase -> End");       
    }

TInt CMRSummaryDlgBase::ExecuteLD()
	{
	TInt retVal( 0 );
    TInt resource( iEditorMode == MAgnEntryUi::EViewEntry ?
        R_MR_SUMMARY_VIEW_DLG : R_MR_SUMMARY_EDIT_DLG );
    LOG("CMRSummaryDlgBase::ExecuteViewL, executing dialog");
    retVal = CAknDialog::ExecuteLD( resource );
    LOG1( "CMRSummaryDlgBase::ExecuteViewL return code: %d", retVal );
	return retVal;
	}

//-----------------------------------------------------------------------------
//  CMRSummaryDlgBase::GetHelpContext()
//-----------------------------------------------------------------------------
void CMRSummaryDlgBase::GetHelpContext( TCoeHelpContext& /*aContext*/) const
    {
    //aContext.iMajor = TUid::Uid( KUidMsgMailEditor );
    //aContext.iContext = KMAIL_HLP_SENDING_OPTIONS;
    }
	
void CMRSummaryDlgBase::DynInitMenuPaneL(
    TInt aMenuId,
    CEikMenuPane* aMenuPane )
	{
    LOG("CMRSummaryDlgBase::DynInitMenuPaneL");	
	if( iMRModel )
		{
		if ( aMenuId == R_MR_SUMMARYDLG_MENUPANE )
			{
			//actual item is decided by what listbox item is selected
			if ( iListBox->CurrentItemIndex() == EMRDescriptionItem )
				{
				aMenuPane->SetItemDimmed( EMRCommandShowDescriptionView, EFalse );
				aMenuPane->SetItemDimmed( EMRCommandShowAttendeesView, ETrue );
				}
			else if ( iListBox->CurrentItemIndex() == EMRAttendeeItem )
				{
				aMenuPane->SetItemDimmed( EMRCommandShowAttendeesView, EFalse );
				aMenuPane->SetItemDimmed( EMRCommandShowDescriptionView, ETrue );
				}
            aMenuPane->SetItemDimmed( EMRCommandSend,
                                      !iMRModel->IsCmdAvailable( EMRCommandSend ) );
            aMenuPane->SetItemDimmed( EMRCommandSendUpdate,
                                      !iMRModel->IsCmdAvailable( EMRCommandSendUpdate ) );
            aMenuPane->SetItemDimmed( EMRCommandCancelMR,
                                      !iMRModel->IsCmdAvailable( EMRCommandCancelMR ) );
            aMenuPane->SetItemDimmed( EMRCommandDeleteMR,
                                      !iMRModel->IsCmdAvailable( EMRCommandDeleteMR ) );
            TBool canRespond( iMRModel->IsCmdAvailable( EMRCommandRespondAccept ) ||
	                          iMRModel->IsCmdAvailable( EMRCommandRespondTentative ) ||
	                          iMRModel->IsCmdAvailable( EMRCommandRespondDecline ) );
			aMenuPane->SetItemDimmed( EMRCommandRespondSubmenu, !canRespond );
			aMenuPane->SetItemDimmed( EMRCommandRemoveFromCalendar, 
			                          !iMRModel->IsCmdAvailable( EMRCommandRemoveFromCalendar ) );			
            TBool canReply( iMRModel->IsCmdAvailable( EMRCommandReplyToSender ) ||
	                        iMRModel->IsCmdAvailable( EMRCommandReplyToOrganiser ) ||
	                        iMRModel->IsCmdAvailable( EMRCommandReplyToAll ) );
			aMenuPane->SetItemDimmed( EMRCommandReplySubmenu, !canReply );
			aMenuPane->SetItemDimmed( EMRCommandForward,
			                          !iMRModel->IsCmdAvailable( EMRCommandForward) );
			aMenuPane->SetItemDimmed( EMRCommandSaveFromFile, 
			                          !iMRModel->IsCmdAvailable( EMRCommandSaveFromFile ) );
            TBool canRetrieve( iMRModel->IsCmdAvailable( EMRCommandRetrieve ) &&
                               iHandlerCallback.IsCommandAvailable( EMRCommandRetrieve ) );
			aMenuPane->SetItemDimmed( EMRCommandRetrieve, !canRetrieve );
            TBool canShowAtta( iMRModel->IsCmdAvailable( EMRCommandAttachments ) &&
                               iHandlerCallback.IsCommandAvailable( EMRCommandAttachments ) );                      
			aMenuPane->SetItemDimmed( EMRCommandAttachments, !canShowAtta );
			aMenuPane->SetItemDimmed( EMRCommandMessageDetails,
			                          !iMRModel->IsCmdAvailable( EMRCommandMessageDetails ) );
			aMenuPane->SetItemDimmed( EMRCommandDisableAlarm,
			                          !iMRModel->IsCmdAvailable( EMRCommandDisableAlarm ) );
			}
		else if ( aMenuId == R_MR_RESPOND_SUBMENU )
			{		
			aMenuPane->SetItemDimmed( EMRCommandRespondAccept,
			                          !iMRModel->IsCmdAvailable( EMRCommandRespondAccept ) );
			aMenuPane->SetItemDimmed( EMRCommandRespondTentative,
			                          !iMRModel->IsCmdAvailable( EMRCommandRespondTentative ) );
			aMenuPane->SetItemDimmed( EMRCommandRespondDecline,
			                          !iMRModel->IsCmdAvailable( EMRCommandRespondDecline ) );
			}
		else if ( aMenuId == R_MR_REPLY_TO_SUBMENU )
			{
			aMenuPane->SetItemDimmed( EMRCommandReplyToSender,
			                          !iMRModel->IsCmdAvailable( EMRCommandReplyToSender ) );
			aMenuPane->SetItemDimmed( EMRCommandReplyToOrganiser,
			                          !iMRModel->IsCmdAvailable( EMRCommandReplyToOrganiser ) );
			aMenuPane->SetItemDimmed( EMRCommandReplyToAll,
			                          !iMRModel->IsCmdAvailable( EMRCommandReplyToAll ) );
			}
		}
	else
		{
		if ( aMenuId == R_MR_SUMMARYDLG_MENUPANE )
			{		
			aMenuPane->SetItemDimmed( EMRCommandShowAttendeesView, ETrue );
			aMenuPane->SetItemDimmed( EMRCommandShowDescriptionView, ETrue );
            aMenuPane->SetItemDimmed( EMRCommandSend, ETrue );
            aMenuPane->SetItemDimmed( EMRCommandSendUpdate, ETrue );
            aMenuPane->SetItemDimmed( EMRCommandCancelMR, ETrue );
            aMenuPane->SetItemDimmed( EMRCommandDeleteMR, ETrue );				
			aMenuPane->SetItemDimmed( EMRCommandRespondSubmenu, ETrue );
			aMenuPane->SetItemDimmed( EMRCommandRemoveFromCalendar, ETrue );
			aMenuPane->SetItemDimmed( EMRCommandReplySubmenu, ETrue );
			aMenuPane->SetItemDimmed( EMRCommandForward, ETrue );
			aMenuPane->SetItemDimmed( EMRCommandRetrieve, ETrue );
			aMenuPane->SetItemDimmed( EMRCommandSaveFromFile, ETrue );			
			aMenuPane->SetItemDimmed( EMRCommandAttachments, ETrue );
			aMenuPane->SetItemDimmed( EMRCommandMessageDetails, ETrue );
			aMenuPane->SetItemDimmed( EMRCommandDisableAlarm, ETrue);
			}
		}
    LOG("CMRSummaryDlgBase::DynInitMenuPaneL -> End");
	}

void CMRSummaryDlgBase::HandleSetModelL()
	{
    LOG("CMRSummaryDlgBase::HandleSetModelL");				
	if ( iListBox )
	    { // if listbox not yet created then it's model is set later	    
        iLBModel->SetModel( *iMRModel );
        iListBox->HandleItemAdditionL();
	    }
	    
    __ASSERT_DEBUG( !iInfoPopup, Panic( EUnexpectedNonNull ) );
    iInfoPopup = iMRModel->CreateInfoPopupL();
    iInfoPopup->ShowInfoPopupL();
    LOG("CMRSummaryDlgBase::HandleSetModelL -> End");	    
	}

void CMRSummaryDlgBase::RefreshData()
    {
    // Listbox should definitely exist at this point
	__ASSERT_DEBUG( iListBox, Panic( EListboxNull ) );
    TRAP_IGNORE( iListBox->HandleItemAdditionL() );
    
    // Info popup should also exist:
    __ASSERT_DEBUG( iInfoPopup, Panic( EInfopopupNull ) );    
    TRAP_IGNORE( iInfoPopup->RefreshTextL( *( iMRModel->CombinedEntry() ),
                                           iListBox->CurrentItemIndex() ) );
    }

TInt CMRSummaryDlgBase::CountComponentControls() const	
	{
	return 1;
	}
	
CCoeControl* CMRSummaryDlgBase::ComponentControl( TInt aIndex ) const 
	{
	switch ( aIndex )
        {
        case 0:
            return iListBox;
        default:
            return NULL;
        }
	}
	
void CMRSummaryDlgBase::PreLayoutDynInitL()
	{
    LOG("CMRSummaryDlgBase::PreLayoutDynInitL");	
	ConstructListBoxL();
    LOG("CMRSummaryDlgBase::PreLayoutDynInitL -> End");	
	}

void CMRSummaryDlgBase::SizeChanged()
	{	
	if ( iListBox )
	    {	    
	    iListBox->SetRect( Rect() );
	    }
	}
	
TKeyResponse CMRSummaryDlgBase::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    TKeyResponse retVal( EKeyWasNotConsumed );
    TInt callBackResult( KErrNone );

	if ( aType == EEventKey && aKeyEvent.iCode == EKeyLeftArrow )
		{
		callBackResult = iHandlerCallback.ProcessCommandWithResultL(
		    EMRCommandNavigateBackward );
		if ( callBackResult != KErrNotSupported)
		    {		    
			retVal = EKeyWasConsumed;
		    }
		}
	else if ( aType == EEventKey && aKeyEvent.iCode == EKeyRightArrow )
		{
		callBackResult = iHandlerCallback.ProcessCommandWithResultL(
		    EMRCommandNavigateForward );
		if ( callBackResult != KErrNotSupported )
		    {		    
			retVal = EKeyWasConsumed;
		    }
		}
	else
		{
		// listbox handles the keyevent if it is not right or left arrow
		retVal = iListBox->OfferKeyEventL( aKeyEvent, aType );
		
		// if navigating up or down we must switch info popup data
		if ( aType == EEventKey && 
		     ( aKeyEvent.iCode == EKeyUpArrow ||
		       aKeyEvent.iCode == EKeyDownArrow ) )
            {		        
		    iInfoPopup->RefreshTextL( *( iMRModel->CombinedEntry() ),
		                              iListBox->CurrentItemIndex() );
            }
		}
	return retVal;
    }

void CMRSummaryDlgBase::HandleListBoxEventL(
    CEikListBox* aListBox,
    TListBoxEvent aEventType )
	{
	// we mustn't accept any further events if already exiting
	if ( aEventType == EEventEnterKeyPressed && !iIdleDialogExit->IsActive() )
		{
		TInt listboxCmd( EEikCmdCanceled ); // EEikCmdCanceled == 0
		if ( aListBox->CurrentItemIndex() == EMRDescriptionItem )
			{
			listboxCmd = EMRCommandShowDescriptionView;
			}
        else if ( aListBox->CurrentItemIndex() == EMRAttendeeItem )
            {
            listboxCmd = EMRCommandShowAttendeesView;
            }
            
        if ( listboxCmd != EEikCmdCanceled )
            { // either description or attendee view can be opened
			iHandlerCallback.ProcessCommandL( listboxCmd );
			
			// use idle timer to exit dialog, so that it is in stable state
			// this is required, otherwise dialog will panic
			iIdleDialogExit->Start( TCallBack( DelayedDialogExitCallBack, this ) );
            }
		}
	}

TInt CMRSummaryDlgBase::DelayedDialogExitCallBack( TAny* aSummaryDlg )
	{
    LOG("CMRSummaryDlgBase::DelayedDialogExitCallBack");	    
	CMRSummaryDlgBase* dlg = static_cast<CMRSummaryDlgBase*>( aSummaryDlg );
	TRAP_IGNORE( dlg->TryExitL( EKeyOK ) );
    LOG("CMRSummaryDlgBase::DelayedDialogExitCallBack -> End");	    	
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CMRSummaryDlgBase::HandleDlgExitingL
// There are only a few commands that require special exiting action.
// Exiting action is needed if there may be unsaved data, or if user has
// pressed "Back" and entry needs some action. With other commands that cause
// closing the viewer (e.g. "Respond") this kind of slow exit is not performed.
// ----------------------------------------------------------------------------
//
TInt CMRSummaryDlgBase::HandleDlgExitingL( TInt aCommandId )
    {
    TInt retCmd( EEikCmdCanceled );
    if ( aCommandId == EKeyOK || 
         aCommandId == EMRCommandShowDescriptionView || 
         aCommandId == EMRCommandShowAttendeesView )
        { // opening another view, not exiting viewer -> no action
        retCmd = EEikCmdCanceled; // == no special exit action
        }
    else if ( iMRModel )
        {                           
        MMRModelInterface::TClosingStatus status( iMRModel->ClosingStatusL() );
    	if ( aCommandId == EAknSoftkeyBack )
            {
        	if ( status == MMRModelInterface::ENotResponded )
        		{
        		retCmd = HandleDlgExitingResponseL();
        		}
            else
                {                
                retCmd = EMRCommandExitDialogs;
                }
            }
        else if ( aCommandId == EAknSoftkeyDone )
    	    {	    
    	    if ( status == MMRModelInterface::ENotSaved )
    	        {
    	        retCmd = EMRCommandSaveAndExit;
    	        }
    	    else if ( status == MMRModelInterface::ENotSentNew )
    	        {
    	        retCmd = EMRCommandSend;
    	        }
    	    else if ( status == MMRModelInterface::ENotSentUpdate )
    	        {
    	        retCmd = EMRCommandSendUpdate;
    	        }
            else
                {                
                retCmd = EMRCommandExitDialogs;
                }    	        	    
    	    }
        else if ( aCommandId == EAknCmdExit || aCommandId == EEikCmdExit )
            { // quiet mode, quick exit -> just save if needed
    	    if ( status == MMRModelInterface::ENotSaved )
    	        {
    	        retCmd = EMRCommandSaveAndExit;
    	        }        
            }
        }
    return retCmd;
    }
	
// ----------------------------------------------------------------------------
// CMRSummaryDlgBase::HandleDlgExitingResponseL
// 
// ----------------------------------------------------------------------------
//
TInt CMRSummaryDlgBase::HandleDlgExitingResponseL()
    {
    TInt retCmd( 0 );    
	TInt actionIndex( -1 );
	CAknListQueryDialog* dlg =
	    new( ELeave ) CAknListQueryDialog( &actionIndex );
    dlg->PrepareLC( R_NOT_RESPONDED_LIST_QUERY );
    if ( !iMRModel->IsCmdAvailable( EMRCommandDeleteMR ) )
        { // we set another text array if cannot allow delete
        MDesCArray* listArray = iCoeEnv->ReadDesCArrayResourceL(
            R_NOT_RESPONDED_ITEM_LIST_ARRAY_NO_DELETE );                    
        dlg->SetItemTextArray( listArray );
        dlg->SetOwnershipType( ELbmOwnsItemArray );
        }
        
    if ( dlg->RunLD() )
        {                
        if ( actionIndex == 0 )
            { // respond
	        TInt statusIndex( -1 );
	        CAknListQueryDialog* statusDlg =
	            new( ELeave ) CAknListQueryDialog( &statusIndex );
            if ( statusDlg->ExecuteLD( R_RESPOND_LIST_QUERY ) )
                {
                if ( statusIndex == 0 )
                    {
                    retCmd = EMRCommandRespondAccept;
                    }
                else if ( statusIndex == 1 )
                    {
                    retCmd = EMRCommandRespondTentative;
                    }
                else
                    {
                    retCmd = EMRCommandRespondDecline;
                    }
                }
            }
        else if ( actionIndex == 1 )
            { // do not respond
            retCmd = EMRCommandExitDialogs;
            }
        else
            { // delete request
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            if ( dlg->ExecuteLD( R_CONFIRM_DELETE_FROM_CALENDAR ) )
                {
                retCmd = EMRCommandRemoveFromCalendar;
                }
            }
        }
    return retCmd;
    }
	
// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CAttendeeLBModel::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMRSummaryListBoxModel* CMRSummaryListBoxModel::NewL( CEikonEnv* aEnv )
    {
    CMRSummaryListBoxModel* self = new( ELeave )CMRSummaryListBoxModel( aEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CAttendeeLBModel::ConstructL
// ---------------------------------------------------------
//
void CMRSummaryListBoxModel::ConstructL()
    {    
    iBuffer = HBufC::NewL( 256 ); // TODO: magic number
    
    iNoSubjectText =
        StringLoader::LoadL( R_QTN_CALE_SUMMARY_DESCRIPTION_NO_SUBJECT, iEnv );
    iAttendeesText =
        StringLoader::LoadL( R_QTN_CALE_SUMMARY_ATTENDEES, iEnv );
    iNoInvitedAttendeesText =
        StringLoader::LoadL( R_QTN_CALE_NO_INVITED_ATTENDEES, iEnv );
    }

// ----------------------------------------------------------------------------
// CAttendeeLBModel::CAttendeeLBModel
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CMRSummaryListBoxModel::CMRSummaryListBoxModel( CEikonEnv* aEnv )
    : iEnv( aEnv )
    {
    }

// ---------------------------------------------------------
// CAttendeeLBModel::~CAttendeeLBModel
// ---------------------------------------------------------
//
CMRSummaryListBoxModel::~CMRSummaryListBoxModel()
    {
    delete iBuffer;    
    delete iNoSubjectText;
    delete iAttendeesText;
    delete iNoInvitedAttendeesText;
    delete iInvitedAttendees;
    }



void CMRSummaryListBoxModel::SetModel( MMRModelInterface& aMRModel )
    {
    iMRModel = &aMRModel;    
    }

// ---------------------------------------------------------
// CAttendeeLBModel::MdcaCount
// ---------------------------------------------------------
//
TInt CMRSummaryListBoxModel::MdcaCount() const
    {    
    return 2;
    }

// ---------------------------------------------------------
// CAttendeeLBModel::MdcaPoint
// ---------------------------------------------------------
//
TPtrC16 CMRSummaryListBoxModel::MdcaPoint( TInt aIndex ) const
    {
    TPtr ptr( iBuffer->Des() );
    ptr.Zero();
    
    if ( iMRModel )
    	{
    	if ( aIndex == EMRDescriptionItem )
    		{
    		AppendDescriptionIcon( ptr );
    		// TODO: investigate if trapping is proper way of handling error here
    		TRAP_IGNORE( 
    		    {
    		    AppendDescriptionPrimaryL( ptr );
    		    AppendDescriptionSecondaryL( ptr );
    		    } );
    		}
    	else if ( aIndex == EMRAttendeeItem )
    		{
    		AppendAttendeeIcon( ptr );
    		AppendAttendeePrimary( ptr );
    		TRAP_IGNORE( AppendAttendeeSecondaryL( ptr ) );
    		}
    	}
    else
    	{
    	if ( aIndex == EMRDescriptionItem )
    		{
       		ptr.AppendNum( 0 );
    		ptr.Append( EKeyTab );
    		ptr.Append( *iNoSubjectText );
    		ptr.Append( EKeyTab );
       		}
    		
   		else if ( aIndex == EMRAttendeeItem )
   			{
   			ptr.AppendNum( 0 );
    		ptr.Append( EKeyTab );
    		ptr.Append( *iAttendeesText );
    		ptr.Append( EKeyTab );
    		ptr.Append( *iNoInvitedAttendeesText );
   			}
    	}	    
    return iBuffer->Des();
    }

void CMRSummaryListBoxModel::AppendDescriptionIcon( TPtr& aPtr ) const
	{
	aPtr.AppendNum( 0 );
	}
void CMRSummaryListBoxModel::AppendDescriptionPrimaryL( TPtr& aPtr ) const
	{
	__ASSERT_DEBUG( iMRModel, Panic( EUnexpectedNullMRModel ) );
	
	CCalEntry* entry = iMRModel->CombinedEntry();
	
	const TDesC& description = entry->SummaryL();
	aPtr.Append( EKeyTab );
	if ( description.Length() > 0 )
		{
		aPtr.Append( description );
		}
	else
		{
		aPtr.Append( *iNoSubjectText );
		}
		
	}
void CMRSummaryListBoxModel::AppendDescriptionSecondaryL( TPtr& aPtr ) const
	{
	__ASSERT_DEBUG( iMRModel, Panic( EUnexpectedNullMRModel ) );
		
	CCalEntry* entry = iMRModel->CombinedEntry();
	
	const TDesC& location = entry->LocationL();
	
	aPtr.Append( EKeyTab );
	aPtr.Append( location );
	}

void CMRSummaryListBoxModel::AppendAttendeeIcon( TPtr& aPtr ) const
	{
	aPtr.AppendNum( 1 );
	}
	
void CMRSummaryListBoxModel::AppendAttendeePrimary( TPtr& aPtr ) const
	{
	aPtr.Append( EKeyTab );
	aPtr.Append( *iAttendeesText );
	}
	
void CMRSummaryListBoxModel::AppendAttendeeSecondaryL( TPtr& aPtr ) const 
	{
	__ASSERT_DEBUG( iMRModel, Panic( EUnexpectedNullMRModel ) );
		
	CCalEntry* entry = iMRModel->CombinedEntry();
	TInt attendeeCount = entry->AttendeesL().Count();
	
	aPtr.Append( EKeyTab );
	if ( iInvitedAttendees )
	    {	    
		delete iInvitedAttendees;
		iInvitedAttendees = NULL;
	    }
	iInvitedAttendees = StringLoader::LoadL( R_QTN_CALE_INVITED_ATTENDEES,
	                                         attendeeCount, 
	                                         iEnv );
	
	if ( attendeeCount > 0 )
		{
		aPtr.Append( *iInvitedAttendees );
		}
	else
		{
		aPtr.Append( _L("No attendees") );
		}
	}


//  End of File

