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
* Description:   Factory methods for model AND dialog construction
*
*/




// INCLUDE FILES
#include "CMRObjectFactory.h"
#include "CMRRequestModel.h"
#include "CMRResponseModel.h"
#include "CMRCancelModel.h"
#include "CMRSummaryDlgBase.h"
#include "CMRDescriptionDlgBase.h"
#include "MRViewersPanic.h"
#include "meetingrequestviewers.hrh"
#include <cmrmailboxutils.h>

/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CMRObjectFactory" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

}  // namespace

MMRModelInterface* CMRObjectFactory::CreateMRModelL(
    RPointerArray<CCalEntry>& aEntries,
    const MAgnEntryUi::TAgnEntryUiInParams& aParams,
    CMRMailboxUtils& aMRMailboxUtils,
    CMRUtilsInternal& aMRUtils )
	{
	MMRModelInterface* model( NULL );	

    // all entries in the array have the same method
	switch ( aEntries[0]->MethodL() )
		{
		case CCalEntry::EMethodRequest:
				{
				model = CMRRequestModel::NewL( aEntries,
				                               aMRMailboxUtils,
				                               aMRUtils,
				                               aParams );
				break;
				}
		case CCalEntry::EMethodReply:
				{
				model = CMRResponseModel::NewL( aEntries,
                                                aMRMailboxUtils,
                                                aMRUtils,
                                                aParams );
				break;
				}
		case CCalEntry::EMethodCancel:
				{
				model = CMRCancelModel::NewL( aEntries,
				                              aMRMailboxUtils,
                                              aMRUtils,
                                              aParams );
				break;
				}
		default:
			{
			User::Leave( KErrNotSupported );
			}
		}
	return model;
	}
	
CMRDialogBase* CMRObjectFactory::CreateMRDialogL(
    CCalEntry::TMethod aMethod,
    const MAgnEntryUi::TAgnEntryUiInParams& aParams,
    MAgnEntryUiCallback& aHandlerCallback,
    CMRHandler::TMRViewStatus aRequestedView )
	{
	CMRHandler::TMRViewStatus createdView( aRequestedView );
	
	// if requested dialog is default for the view then we need to
	// define that here based on request method
	if ( aRequestedView == CMRHandler::EInitialView )
		{
		if ( aMethod == CCalEntry::EMethodRequest ||
		     aMethod == CCalEntry::EMethodRefresh ||
		     aMethod == CCalEntry::EMethodCancel )
		    {
		    createdView = CMRHandler::ELaunchMeetingRequestView;
		    }
        else if ( aMethod == CCalEntry::EMethodReply )
            {
            createdView = CMRHandler::ELaunchMeetingDescriptionView;
            }
        else
            {
            User::Leave( KErrNotSupported );
            }
		}
		
	if ( createdView == CMRHandler::ELaunchMeetingRequestView )
		{
		return CMRSummaryDlgBase::NewL( aHandlerCallback,
		                                aParams.iEditorMode );
		}
	else
		{
		__ASSERT_DEBUG( createdView ==
		                CMRHandler::ELaunchMeetingDescriptionView,
		                Panic( EIllegalParamValue ) );
		return CMRDescriptionDlgBase::NewL( aHandlerCallback );
		}		
	}
	
// End of file
