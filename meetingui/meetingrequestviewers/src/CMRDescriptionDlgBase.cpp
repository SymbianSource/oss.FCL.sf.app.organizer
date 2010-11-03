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
* Description:   Implementation of base description dialog
*
*/




// INCLUDE FILES
#include "CMRDescriptionDlgBase.h"
#include "MMRModelInterface.h"
#include "CMRMsgBaseControlAdapter.h"
#include "CMRDataFillerMeetingRequest.h"
#include "meetingrequestviewers.hrh"
#include "MRViewersPanic.h"
#include <e32std.h>
#include <MAgnEntryUi.h> 				//MAgnEntryUi
#include <eikmenup.h> 					//menupane
#include <eikenv.h> 					//eikonenv
#include <EIKAPPUI.H> 					//eikappui
#include <crichbio.h>					//rich bio for displaying MR data
#include <AknsControlContext.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <msgeditorview.h>				// messaging editor base
#include <MsgEditorAppUi.rsg>           // resource identifiers
#include <MsgMailViewer.rsg>
#include <MsgBaseControl.h>
#include <coeaui.h>
#include <MsgAddressControl.h>
#include <miutpars.h>                   // TImMessageField
#include <txtrich.h>					// CRichText
#include <aiwservicehandler.h>          // CAiwServiceHandler
#include <AiwContactAssignDataTypes.h>  // aiw
#include <CommonUiInternalCRKeys.h>
#include <FindItem.hrh>
#include <finditemmenu.rsg>
#include <finditemmenu.h>
#include <centralrepository.h>

using namespace AiwContactAssign;

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
_LIT( KPanicMsg, "CMRDescriptionDlgBase" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMRDescriptionDlgBase::?classname
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMRDescriptionDlgBase::CMRDescriptionDlgBase(
    MAgnEntryUiCallback& aHandlerCallback )
    : CMRDialogBase( aHandlerCallback )
    {
    }

// -----------------------------------------------------------------------------
// CMRDescriptionDlgBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMRDescriptionDlgBase::ConstructL()
    {
    CAknDialog::ConstructL( R_MR_DESCRDLG_OPTIONS_MENUBAR );

	iCenRep = CRepository::NewL( KCRUidCommonUi );
    iServiceHandler = CAiwServiceHandler::NewL();
        
    TUint32 flags( 0 );
    flags |= CMsgEditorView::EMsgDoNotUseDefaultBodyControl;
    flags |= CMsgEditorView::EMsgReadOnly;
    iView = CMsgEditorView::NewL(*this, flags);
    iView->SetMopParent(this);
    iEikonEnv->EikAppUi()->AddToStackL(iView,ECoeStackPriorityCba);
    
	iRichBio = new( ELeave ) CRichBio( ERichBioModeStandard );

	CMsgBaseControl* adapter = CMRMsgBaseControlAdapter::NewL(iView,*iRichBio);
	CleanupStack::PushL(adapter);

	CMsgBaseControl* bodycontrol = iView->RemoveControlL(EMsgComponentIdBody);
    delete bodycontrol;
    bodycontrol = NULL;

	iView->AddControlL(adapter,EMsgComponentIdBody,
        EMsgAppendControl,
        EMsgBody);
	CleanupStack::Pop(); // adapter

	iView->SetControlsModified( EFalse );
	ActivateL();
    }

// -----------------------------------------------------------------------------
// CMRDescriptionDlgBase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMRDescriptionDlgBase* CMRDescriptionDlgBase::NewL(
    MAgnEntryUiCallback& aHandlerCallback )
    {
    CMRDescriptionDlgBase* self =
        new( ELeave ) CMRDescriptionDlgBase( aHandlerCallback );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// Destructor
CMRDescriptionDlgBase::~CMRDescriptionDlgBase()
    {
    if ( iCenRepNotifyHandler )
        {        
        iCenRepNotifyHandler->StopListening();
        delete iCenRepNotifyHandler;
        }
    delete iCenRep;
    delete iFindItemMenu;    
    delete iServiceHandler;
	delete iRichBio;
	delete iDataFillerInterface;
	delete iView;
    }

TInt CMRDescriptionDlgBase::ExecuteLD()
	{
	TInt retVal( 0 );
	retVal = CAknDialog::ExecuteLD( R_MR_DESCRIPTION_DLG );
	return retVal;
	}

//-----------------------------------------------------------------------------
//  CMsgMailSendOptionsDlg::GetHelpContext()
//-----------------------------------------------------------------------------
void CMRDescriptionDlgBase::GetHelpContext(
    TCoeHelpContext& /*aContext*/ ) const
    {
    }

void CMRDescriptionDlgBase::DynInitMenuPaneL(
    TInt aMenuId,
    CEikMenuPane* aMenuPane )
	{
#ifdef RD_MESSAGING_VIRTUAL_PHONEBOOK     
    // Offer the menu for AIW framework first
    TBool wasConsumed = iServiceHandler->HandleSubmenuL( *aMenuPane );
    if ( wasConsumed )
        {
        return;
        }	
	else if ( iMRModel )
#else // !RD_MESSAGING_VIRTUAL_PHONEBOOK     
	if ( iMRModel )
#endif // RD_MESSAGING_VIRTUAL_PHONEBOOK     
		{
		if ( aMenuId == R_MR_DESCRDLG_MENUPANE )
			{
            AddFindItemMenuL( aMenuPane, EFalse );
            
#ifdef RD_MESSAGING_VIRTUAL_PHONEBOOK    

        if ( FromFieldHighlighted() )
            {        
            TUint assignFlags = 0;
        	
            CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
            inParamList.AppendL(
                TAiwGenericParam(
                    EGenericParamContactAssignData,
                    TAiwVariant(TAiwSingleContactAssignDataV1Pckg(
                    TAiwSingleContactAssignDataV1().SetFlags(assignFlags)))));

            inParamList.AppendL(
                TAiwGenericParam(
                    EGenericParamEmailAddress,
                    TAiwVariant()
                    ));
        	
	         // Let provider add its menu items to the menu
	        iServiceHandler->InitializeMenuPaneL( *aMenuPane,
                                                  aMenuId,
                                 	              KAiwCmdAssign,
                                	              inParamList );
            }
        else
            {
            aMenuPane->SetItemDimmed( KAiwCmdAssign, ETrue );
            }
            
#endif // RD_MESSAGING_VIRTUAL_PHONEBOOK 			
			
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
			                          !iMRModel->IsCmdAvailable( EMRCommandForward ) );
			aMenuPane->SetItemDimmed( EMRCommandSaveFromFile, 
			                          !iMRModel->IsCmdAvailable( EMRCommandSaveFromFile ) );
            TBool canRetrieve( iMRModel->IsCmdAvailable( EMRCommandRetrieve ) &&
                               iHandlerCallback.IsCommandAvailable( EMRCommandRetrieve ) );
			aMenuPane->SetItemDimmed( EMRCommandRetrieve, !canRetrieve );
            TBool canShowAtta( iMRModel->IsCmdAvailable( EMRCommandAttachments ) &&
                               iHandlerCallback.IsCommandAvailable( EMRCommandAttachments ) );
			aMenuPane->SetItemDimmed( EMRCommandAttachments, !canShowAtta );
			aMenuPane->SetItemDimmed( EMRCommandDisableAlarm,
			                          !iMRModel->IsCmdAvailable( EMRCommandDisableAlarm) );
			}
        else if ( aMenuId == R_DESCR_VIEW_OKOPTIONS_MENUPANE )
            {
            AddFindItemMenuL( aMenuPane, ETrue );
            
            TBool canRespond( iMRModel->IsCmdAvailable( EMRCommandRespondAccept ) ||
	                          iMRModel->IsCmdAvailable( EMRCommandRespondTentative ) ||
	                          iMRModel->IsCmdAvailable( EMRCommandRespondDecline ) );
			aMenuPane->SetItemDimmed( EMRCommandRespondSubmenu, !canRespond );
			aMenuPane->SetItemDimmed( EMRCommandSaveFromFile, 
			                          !iMRModel->IsCmdAvailable( EMRCommandSaveFromFile ) );			                          
			aMenuPane->SetItemDimmed( EMRCommandRemoveFromCalendar,
			                          !iMRModel->IsCmdAvailable( EMRCommandRemoveFromCalendar ) );
            TBool canReply( iMRModel->IsCmdAvailable( EMRCommandReplyToSender ) ||
	                        iMRModel->IsCmdAvailable( EMRCommandReplyToOrganiser ) ||
	                        iMRModel->IsCmdAvailable( EMRCommandReplyToAll ) );
			aMenuPane->SetItemDimmed( EMRCommandReplySubmenu, !canReply );
			aMenuPane->SetItemDimmed( EMRCommandForward,
			                          !iMRModel->IsCmdAvailable( EMRCommandForward ) );
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
		else if (aMenuId == R_MR_REPLY_TO_SUBMENU)
			{
			aMenuPane->SetItemDimmed( EMRCommandReplyToSender,
				                      !iMRModel->IsCmdAvailable( EMRCommandReplyToSender ) );
			aMenuPane->SetItemDimmed( EMRCommandReplyToOrganiser,
			                          !iMRModel->IsCmdAvailable( EMRCommandReplyToOrganiser ) );
			aMenuPane->SetItemDimmed( EMRCommandReplyToAll,
			                          !iMRModel->IsCmdAvailable( EMRCommandReplyToAll ) );
			}			
        else
            {			    
            if ( iFindItemMenu )
                {                
                iFindItemMenu->UpdateItemFinderMenuL( aMenuId, aMenuPane );
                }
            }
		}
	else
		{
		if ( aMenuId == R_MR_DESCRDLG_MENUPANE )
			{
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
	}

//-----------------------------------------------------------------------------
//  CMRDescriptionDlgBase::ProcessCommandL()
//-----------------------------------------------------------------------------
void CMRDescriptionDlgBase::ProcessCommandL( TInt aCommandId )
	{
    if ( iFindItemMenu && iFindItemMenu->CommandIsValidL( aCommandId ) )
        {
	    HideMenu();        
        iFindItemMenu->HandleItemFinderCommandL( aCommandId );
        }
#ifdef RD_MESSAGING_VIRTUAL_PHONEBOOK            
    else if( iServiceHandler->ServiceCmdByMenuCmd( aCommandId ) != KAiwCmdNone )
        {
        CreateContactL( aCommandId );
        }
#endif // RD_MESSAGING_VIRTUAL_PHONEBOOK                    
    else
        {        
        CMRDialogBase::ProcessCommandL( aCommandId );
        }
	}

// ----------------------------------------------------------------------------
// CMRDescriptionDlgBase::HandleDlgExitingL
// Desctiption dialog is always in view only mode, so nothing needs to be saved
// ----------------------------------------------------------------------------
//
TInt CMRDescriptionDlgBase::HandleDlgExitingL( TInt aCommandId )
    {
    TInt retCmd( EEikCmdCanceled );

	if ( aCommandId == EAknSoftkeyBack && iMRModel )
        {
		//In case of "Back" we are checking if this is a response message, and
		//if so let the user exit the meeting request views from description
		//view otherwise we change back to meeting request view
		CCalEntry::TMethod method = iMRModel->MethodL();
		if ( method == CCalEntry::EMethodReply )
		    {
            retCmd = EMRCommandExitDialogs;
			}
        else
            {
            retCmd = EMRCommandShowSummaryView;
            }
        }

    return retCmd;
    }

void CMRDescriptionDlgBase::PreLayoutDynInitL()
	{
	// TODO: if we can't be sure that iMRModel exists, then we should
	// handle a case when it doesn't.
	if ( iMRModel->MailHeadersAvailable() )
	    {	    
        iView->AddControlFromResourceL(
    	        R_MAIL_VIEWER_FROM, EMsgAddressControl, 0, EMsgHeader );

    	iView->AddControlFromResourceL(
    	        R_MAIL_VIEWER_SUBJECT, EMsgExpandableControl, 1, EMsgHeader );
    	        	    
    	HBufC* fromAddr = iMRModel->MailHeaderFromFieldLC();
    	SetAddressFieldTextL( *fromAddr );
    	CleanupStack::PopAndDestroy( fromAddr );
    	
    	HBufC* subject = iMRModel->MailHeaderSubjectFieldLC();
    	SetSubjectL( *subject );
    	CleanupStack::PopAndDestroy( subject );
    	
#ifdef RD_MESSAGING_VIRTUAL_PHONEBOOK
        iServiceHandler->AttachMenuL( R_MR_DESCRDLG_MENUPANE,
                                      R_MR_DATA_ASSIGN_INTEREST );
#endif // RD_MESSAGING_VIRTUAL_PHONEBOOK    
    
        iFindItemMenu = CFindItemMenu::NewL( EMRCommandFindItem );
        iFindItemMenu->AttachItemFinderMenuL( R_MR_DESCRDLG_MENUPANE );
	
	    iCenRepNotifyHandler = CCenRepNotifyHandler::NewL( 
		    *this, 
		    *iCenRep,
            CCenRepNotifyHandler::EIntKey, 
		    KCuiAutomaticHighlight );
	    iCenRepNotifyHandler->StartListeningL();
    	ReadCRSetAutomaticHighlightL();
	    }
	
	iView->ExecuteL( iEikonEnv->EikAppUi()->ClientRect(), EMsgComponentIdNull );
	}

TInt CMRDescriptionDlgBase::CountComponentControls() const
	{
	return 0;
	}

CCoeControl* CMRDescriptionDlgBase::ComponentControl(TInt /*aIndex*/) const
	{
	return NULL;
	}

void CMRDescriptionDlgBase::SizeChanged()
	{
	}

// TODO: we could prevent user from opening description dialog (or other dialogs)
// before data and model is ready. Then we could assure that description dialog
// always has a model. This method and the complexity caused by it would also be
// avoided.
void CMRDescriptionDlgBase::HandleSetModelL()
	{
	__ASSERT_DEBUG( !iDataFillerInterface, Panic( EDataFillerExists ) );

	iDataFillerInterface = iMRModel->DataFillerL( iRichBio );
	iDataFillerInterface->FillViewerWithDataL();
	
	// MsgEditorView should be drawn here again because model contents have changed
	//CMsgAddressControl* addressControl = AddressControl( EMsgComponentIdFrom );
	}

void CMRDescriptionDlgBase::RefreshData()
    {
	__ASSERT_DEBUG( iDataFillerInterface, Panic( EDataFillerNull ) );
    TRAP_IGNORE( iDataFillerInterface->FillViewerWithDataL() );
    iView->DrawDeferred();
    }

TKeyResponse CMRDescriptionDlgBase::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    if ( aKeyEvent.iCode == EKeyOK && aType == EEventKey )
        {
        // Show context sensitive options menu:
        iMenuBar->SetMenuTitleResourceId( R_DESCR_VIEW_OKOPTIONS_MENUBAR );
        iMenuBar->TryDisplayMenuBarL();
        iMenuBar->SetMenuTitleResourceId( R_MR_DESCRDLG_OPTIONS_MENUBAR );
        return EKeyWasConsumed;
        }
    else
        {        
        //the MsgEditorView should handle all the keyevents
        return iView->OfferKeyEventL( aKeyEvent,aType );
        }
    }

void CMRDescriptionDlgBase::Draw(const TRect& /*aRect*/ ) const
	{
	}

// -----------------------------------------------------------------------------
// CMRDescriptionDlgBase::HandleNotifyInt
// -----------------------------------------------------------------------------
//
void CMRDescriptionDlgBase::HandleNotifyInt(
    TUint32 aId,
    TInt aNewValue )
    {
    // Notifies changes on KCuiAutomaticHighlight
    if ( aId == KCuiAutomaticHighlight )
    	{
    	TRAP_IGNORE( SetAutomaticHighlightL( aNewValue ) );
    	}
    }

// -----------------------------------------------------------------------------
// CMRDescriptionDlgBase::HandleNotifyError
// -----------------------------------------------------------------------------
//
void CMRDescriptionDlgBase::HandleNotifyError(
    TUint32 /* aId */,
    TInt /* aError */,
    CCenRepNotifyHandler* /* aHandler */ )
    {
    // Do nothing
    }

void CMRDescriptionDlgBase::EditorObserver(
    TMsgEditorObserverFunc /*aFunc*/,
    TAny* /*aArg1*/,
    TAny* /*aArg2*/,
    TAny* /*aArg3*/ )
	{
	}

// ----------------------------------------------------------------------------
//  CMsgMailAppUi::AddressControl()
//  Returns the pointer to the address control (header field).
// ----------------------------------------------------------------------------
//
CMsgAddressControl* CMRDescriptionDlgBase::AddressControl() const
    {
    CMsgAddressControl* addressCtrl( NULL );
    CMsgBaseControl* ctrl = iView->ControlById( EMsgComponentIdFrom );
    if ( ctrl && ctrl->ControlType() == EMsgAddressControl )
    	{
    	addressCtrl = static_cast<CMsgAddressControl*>( ctrl );
    	}
    return addressCtrl;
    }

void CMRDescriptionDlgBase::SetAddressFieldTextL( const TDesC& aText )
	{
	CMsgAddressControl* addressControl = AddressControl();
	if ( addressControl )
		{		
	    TImMessageField msgField;
	    HBufC* address = msgField.GetValidInternetEmailAddressFromString(
	        aText ).AllocLC();
	    HBufC* alias = msgField.GetValidAlias( aText ).AllocLC();
	    addressControl->Reset();    
	    // restored recipients don't have contact id, so 0 used instead
	    addressControl->AddRecipientL( *alias, *address, ETrue, NULL );
	    CleanupStack::PopAndDestroy( 2 ); // alias, address
		}	
	}
	
CMsgExpandableControl* CMRDescriptionDlgBase::SubjectControl() const
    {
    CMsgExpandableControl* subjectCtrl( NULL );

    CMsgBaseControl* ctrl = iView->ControlById( EMsgComponentIdSubject );
    if ( ctrl && ctrl->ControlType() == EMsgExpandableControl )
    	{
    	subjectCtrl = static_cast<CMsgExpandableControl*>( ctrl );
    	}
    return subjectCtrl;
    }

void CMRDescriptionDlgBase::SetSubjectL( const TDesC& aText )
    {
    CMsgExpandableControl* subjectControl = SubjectControl();
    if ( subjectControl )
        {
        CRichText& subject = subjectControl->TextContent();
        subject.Reset();
        subject.InsertL( 0, aText );
        }
    }
    
void CMRDescriptionDlgBase::SetAutomaticHighlightL( TInt aValue )
    {    
    CMsgAddressControl* addressControl = AddressControl();
    if ( addressControl )
        {
        addressControl->SetAddressFieldAutoHighlight( aValue );
        }  
    }
    
// -----------------------------------------------------------------------------
// CMRDescriptionDlgBase::CreateContactL()
// aCommand is "create new" or "update existing"
// -----------------------------------------------------------------------------
//
void CMRDescriptionDlgBase::CreateContactL( TInt aCommand )
    {  
    // address control is the only highlightable control
    CMsgAddressControl* addressControl = AddressControl();
	if ( addressControl )
		{
		CMsgRecipientArray* addrArray = addressControl->GetRecipientsL();
		// must be exactly one address in "From" field
		__ASSERT_DEBUG( addrArray && addrArray->Count() == 1,
		                Panic( EIllegalMailHeader ) );

        CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();
        const TDesC& addr = *( addrArray->At( 0 )->Address() );
        inParamList.AppendL( TAiwGenericParam( EGenericParamEmailAddress,
                                               TAiwVariant( addr ) ) );
        iServiceHandler->ExecuteMenuCmdL( aCommand,
                                          inParamList,
                                          iServiceHandler->OutParamListL() );
		}
    }
    
// -----------------------------------------------------------------------------
// CMRDescriptionDlgBase::FromFieldHighlighted
// ----------------------------------------------------------------------------- 
//   
TBool CMRDescriptionDlgBase::FromFieldHighlighted()
    {
    TInt retVal( EFalse );
    CMsgBaseControl* focusedControl = iView->FocusedControl();
    CMsgAddressControl* addressControl = AddressControl();    
    if ( addressControl &&
         focusedControl == addressControl &&
         addressControl->AddressFieldAutoHighlight() )
        {
        retVal = ETrue;
        }
	return retVal;
    }

// ------------------------------------------------------------------------------
// CMRDescriptionDlgBase::AddFindItemMenuL()
// ------------------------------------------------------------------------------
//
void CMRDescriptionDlgBase::AddFindItemMenuL(
    CEikMenuPane* aMenuPane, 
    TBool aIsContextMenu )
    {
    TFindItemMenuCommands placeHolder = aIsContextMenu ?
                                        EFindItemContextMenuPlaceHolder :
                                        EFindItemMenuPlaceHolder;
    if ( iFindItemMenu )
        {
        // Add find item menu        
        iFindItemMenu->AddItemFindMenuL( NULL,
                                           aMenuPane,
                                           placeHolder,
                                           KNullDesC,
                                           ETrue,
                                           aIsContextMenu );
        }    
    }
    

// ------------------------------------------------------------------------------
// CMRDescriptionDlgBase::ReadCRSetAutomaticHighlightL()
// ------------------------------------------------------------------------------
//
void CMRDescriptionDlgBase::ReadCRSetAutomaticHighlightL()
    {
	TInt value( 0 );		
	TInt err = iCenRep->Get( KCuiAutomaticHighlight, value );
	if ( err == KErrNone )
	    {	    
	    SetAutomaticHighlightL( value );
	    }
    }
    
//  End of File
