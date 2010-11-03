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
* Description:   Form to edit meeting request description field
*
*/




// INCLUDE FILES
#include "CMREditBeforeSendingViewForm.h"
#include "meetingrequestutils.hrh" 	//common constants
#include "MRHelpers.h"
#include <MeetingRequestUids.hrh>
#include "icalui.hlp.hrh"			// help resources
#include <meetingrequestutilsuires.rsg>
#include <avkon.hrh>
#include <aknquerydialog.h>
#include <stringloader.h>
#include <calentry.h> 		//CCalEntry (Calendar entry API V2)
#include <caluser.h> 		//caluser and attendee
#include <featmgr.h>
#include <hlplch.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

enum TPanicCode
    {
    };

_LIT( KPanicMsg, "CEditBeforeSendingViewForm" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
    
}  // namespace

// ============================= LOCAL FUNCTIONS ===============================


CEditBeforeSendingViewForm* CEditBeforeSendingViewForm::NewL(
    TMode aMode,
    const CCalEntry& aEntry,
    TDes& aText )
	{
	CEditBeforeSendingViewForm* self =
	    new( ELeave ) CEditBeforeSendingViewForm( aMode, aEntry, aText );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

CEditBeforeSendingViewForm::CEditBeforeSendingViewForm(
    TMode aMode,
    const CCalEntry& aEntry,
    TDes& aText )
    : iMode( aMode ),
      iEntry( aEntry ),
      iText( aText )
    {
    }

void CEditBeforeSendingViewForm::ConstructL()
    {
    CAknDialog::ConstructL( R_EDIT_BEFORE_SENDING_VIEW_MENUBAR );
    }

CEditBeforeSendingViewForm::~CEditBeforeSendingViewForm()
    {
    delete iInvitedAttendees;
    }

TBool CEditBeforeSendingViewForm::OkToExitL( TInt aButtonId )
    {

    TBool retVal( EFalse );
    switch ( aButtonId )
    	{
    	case EAknSoftkeyOptions:
			{
			// we want to dynamically handle the opening of options menu
			CAknDialog::DisplayMenuL();
		 	retVal = EFalse;
		 	break;
			}
    	case EAknSoftkeyExit:
    		{    		
    		retVal = ETrue;
    		break;
    		}
		case EAknSoftkeyCancel:
			{
			CAknQueryDialog* queryDlg = CAknQueryDialog::NewL();
    		if(queryDlg->ExecuteLD( R_EDIT_BEFORE_SENDING_CANCEL_RESPONSE_QUERY ) )
    		    {    		    
    			retVal = ETrue;
    		    }
    		else
    		    {    		    
    			retVal = EFalse;
    		    }
			break;
			}
    	case EEditBeforeSendingCmdSend:
    		{
    		retVal = ETrue;
    		break;
    		}
    	}
    return retVal;
    }
    
void CEditBeforeSendingViewForm::ProcessCommandL( TInt aCommandId )
	{
	HideMenu();
	switch ( aCommandId )
		{
		case EEditBeforeSendingCmdSend:
			{
			SaveFormDataL();
			TryExitL( aCommandId );
			break;
			}
		case EAknCmdHelp:
			{
			if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) ) 
        		{
        		HlpLauncher::LaunchHelpApplicationL(
            	iCoeEnv->WsSession(), iAvkonAppUi->AppHelpContextL() );
                }
			break;
			}
		case EAknCmdExit:
			{
			TryExitL( aCommandId );
			break;
			}
				
		}
	}

void CEditBeforeSendingViewForm::PreLayoutDynInitL()
    {
    SetEditableL( ETrue );
    LoadFormDataL();
    }

void CEditBeforeSendingViewForm::PostLayoutDynInitL()
	{
	}

TBool CEditBeforeSendingViewForm::SaveFormDataL()
    {
    CEikEdwin* ctrl = static_cast<CEikEdwin*> ( Control( EDescriptionEditor ) );
    ctrl->GetText( iText );
    return ETrue;
    }

void CEditBeforeSendingViewForm::LoadFormDataL()
    {
    TPtrC toAddr;
	if (iMode == EEditResponse )
		{
		toAddr.Set( MRHelpers::AddressWithoutMailtoPrefix( 
                    iEntry.OrganizerL()->Address() ) );
		}
	else
		{
		delete iInvitedAttendees;
		iInvitedAttendees = NULL;
		iInvitedAttendees =
		    StringLoader::LoadL( R_QTN_CALE_TO_INVITED_ATTENDEES );
        toAddr.Set( *iInvitedAttendees );
		}
    CEikEdwin* toEditor = static_cast<CEikEdwin*> ( Control( EToEditor ) );
    toEditor->SetTextL( &toAddr );

    CEikEdwin* subjectEditor =
        static_cast<CEikEdwin*> ( Control( ESubjectEditor ) );
    subjectEditor->SetTextL( ( &( iEntry.SummaryL() ) ) );
    }

void CEditBeforeSendingViewForm::DynInitMenuPaneL( TInt aResourceId,
												   CEikMenuPane* aMenuPane )
	{
	switch( aResourceId )
		{
		case R_EDIT_BEFORE_SENDING_VIEW_OPTIONS:
		    {		    
			DynInitMainMenuL( aMenuPane );
			break;
		    }
		default:
		    {		    
			break;
		    }
		}
	}

void CEditBeforeSendingViewForm::DynInitMainMenuL( CEikMenuPane* /*aMenuPane*/ )
	{
	}
	
// ---------------------------------------------------------
// CEditBeforeSendingViewForm::GetHelpContext
// Gets help context
// ---------------------------------------------------------
//
void CEditBeforeSendingViewForm::GetHelpContext(
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = TUid::Uid( KMeetingRequestViewersDllUID3 );

    // Help resources specified in HRH file.
    if ( iMode == EEditResponse )
    	{
    	aContext.iContext = KCALE_HLP_EDIT_RESPONSE;
    	}
    else if ( iMode == EEditCancellation )
    	{
    	aContext.iContext = KCALE_HLP_EDIT_CANCEL;
    	}
    }


//  End of File
