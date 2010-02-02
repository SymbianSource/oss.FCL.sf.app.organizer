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
* Description:   Base dialog for meeting request views
*
*/




// INCLUDE FILES
#include "CMRDialogBase.h"
#include "ICalUILog.h"
#include "MMRModelInterface.h" //MR model interface
#include <MAgnEntryUi.h> //entry ui callback interface
#include <MRCommands.hrh>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?classname::?classname
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMRDialogBase::CMRDialogBase(
    MAgnEntryUiCallback& aHandlerCallback )
    : iHandlerCallback( aHandlerCallback )
    {
    }

// Destructor
CMRDialogBase::~CMRDialogBase()
    {
    }
    
TInt CMRDialogBase::ExecuteLD()
	{
	return KErrNone;
	}
    
void CMRDialogBase::SetModel( MMRModelInterface& aMRModel )
    {
    iMRModel = &aMRModel;
    TRAP_IGNORE( HandleSetModelL() );
    }
    
void CMRDialogBase::HandleSetModelL()
    {
    // Nothing done by default, subclasses may override
    }

//-----------------------------------------------------------------------------
//  CMRDialogBase::IsDlgExitingCmd()
// In case of EMRCommandForward mail framework closes icalui after cmd handling
//-----------------------------------------------------------------------------
TBool CMRDialogBase::IsDlgExitingCmd( TInt aCommandId )
    {
    TBool retVal( EFalse );
    switch ( aCommandId )
        {
        //intentional fallthrough
		case EMRCommandShowDescriptionView:
		case EMRCommandShowAttendeesView:
		case EMRCommandRemoveFromCalendar:
		case EMRCommandSend:
		case EMRCommandSendUpdate:
		case EMRCommandCancelMR:
		case EMRCommandDeleteMR:
		case EMRCommandRespondAccept:
		case EMRCommandRespondTentative:
		case EMRCommandRespondDecline:
		case EMRCommandReplyToSender:
		case EMRCommandReplyToOrganiser:
		case EMRCommandReplyToAll:
	    case EAknCmdExit:
	    case EEikCmdExit:        
            {
            retVal = ETrue;
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
    
//-----------------------------------------------------------------------------
//  CMRDialogBase::ProcessCommandL()
//-----------------------------------------------------------------------------
void CMRDialogBase::ProcessCommandL( TInt aCommandId )
	{
	LOG("CMRDialogBase::ProcessCommandL");
	HideMenu();
	
	TInt ret = iHandlerCallback.ProcessCommandWithResultL( aCommandId );
	if ( IsDlgExitingCmd( aCommandId ) && ret == KErrNone )
	    {
        LOG("CMRDialogBase::ProcessCommandL, calling TryExitL");
	    TryExitL( aCommandId );
	    LOG("CMRDialogBase::ProcessCommandL, TryExitL called");
	    }
    LOG("CMRDialogBase::ProcessCommandL -> End");	
	}

//-----------------------------------------------------------------------------
//  CMRDialogBase::OkToExitL()
//-----------------------------------------------------------------------------
TBool CMRDialogBase::OkToExitL( TInt aButtonId )
	{
	LOG("CMRDialogBase::OkToExitL");
	TInt retVal( ETrue );
	if ( aButtonId == EAknSoftkeyOptions )
		{
		// we want to dynamically handle the opening of options menu
		DisplayMenuL();
	 	retVal = EFalse;
		}
    else
        {
	    LOG("CMRDialogBase::OkToExitL, calling HandleDlgExitingL");
        TInt cmdToPerform( HandleDlgExitingL( aButtonId ) );
        if ( cmdToPerform != EEikCmdCanceled )
            {            
            LOG("CMRDialogBase::OkToExitL, calling ProcessCommandL");
		    iHandlerCallback.ProcessCommandL( cmdToPerform );
            }
		retVal = ETrue;
	    }
    LOG("CMRDialogBase::OkToExitL -> End");
    return retVal;
	}
    
//  End of File
