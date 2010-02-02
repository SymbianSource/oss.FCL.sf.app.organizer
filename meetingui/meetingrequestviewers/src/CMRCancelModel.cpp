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
* Description:   Model for method CANCEL
*
*/




// INCLUDE FILES
#include "CMRCancelModel.h"
#include "CMRModelBase.h"
#include "ProcessingStructs.h" //meeting request scenarios
#include "MMRDataFillerInterface.h"
#include "MREntryConsultant.h"
#include "CMRProcessor.h"
#include "CMRDataFillerCancel.h"
#include <eikenv.h> //CEikonEnv
#include <CRichBio.h> //rich bio
#include "CMRUtilsInternal.h" //MR utils
#include <stringloader.h>
#include <avkon.rsg> // resouce identifiers
#include "meetingrequestviewers.hrh"
#include <senduimtmuids.h> //mailbox id's
#include "MRViewersPanic.h" //panic enums
#include <meetingrequestviewersuires.rsg> // resource identifiers

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CMRCancelModel" );

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
CMRCancelModel::CMRCancelModel(
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
void CMRCancelModel::ConstructL( RPointerArray<CCalEntry>& aEntries )
    {
    CMRModelBase::ConstructL( aEntries );
    }

// -----------------------------------------------------------------------------
// ?classname::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMRCancelModel* CMRCancelModel::NewL(
    RPointerArray<CCalEntry>& aEntries,
    CMRMailboxUtils& aMRMailboxUtils,
    CMRUtilsInternal& aMRUtils,
    const MAgnEntryUi::TAgnEntryUiInParams& aInParams)
    {
    CMRCancelModel* self = new( ELeave ) CMRCancelModel( aMRMailboxUtils,
                                                         aMRUtils,
                                                         aInParams );

    CleanupStack::PushL( self );
    self->ConstructL( aEntries );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CMRCancelModel::~CMRCancelModel()
    {
    }
	
MMRModelInterface::TClosingStatus CMRCancelModel::ClosingStatusL()
	{
	TClosingStatus retVal( ENone );
	return retVal;
	}
	
MMRDataFillerInterface* CMRCancelModel::DataFillerL(CRichBio* aRichBio)
	{
	__ASSERT_DEBUG( aRichBio, Panic( ECRichBioNull ) );
	MMRDataFillerInterface* dataFillerInterface(NULL);
	dataFillerInterface = CMRDataFillerCancel::NewL( *aRichBio,
	                                                 *this,
	                                                 *CEikonEnv::Static(),
	                                                 iMRMailboxUtils,
	                                                 iInParams );
	return dataFillerInterface;
	}

TBool CMRCancelModel::IsCmdAvailableL( TInt aCommandId )
    {
    TInt retVal( EFalse );
    switch ( aCommandId )
        {
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
        case EMRCommandRemoveFromCalendar:
            {
            retVal = CanRemoveFromCalendarL();            
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

TBool CMRCancelModel::CanReplyToOrganiserL()
    {
    TBool retVal( EFalse );
    if ( iMRProcessor->OwnerRole() != CMRProcessor::EOrganiser )
        {        
        retVal = ETrue;
        }
	return retVal;
    }

TBool CMRCancelModel::CanReplyToSenderL()
    {    
    TBool retVal( EFalse );    
	if ( iInParams.iCallingApp.iUid == KUidMailApplication )
		{
        if ( iMRProcessor->OwnerRole() != CMRProcessor::EOrganiser )
            { // can reply to sender if sender is not also organizer
            retVal = !IsSenderOrganizerL();
            }
		}
	return retVal;
    }

TBool CMRCancelModel::CanReplyToAllL()
    {    
	return ETrue;
    }

TBool CMRCancelModel::CanForwardL()
	{
	TBool retVal( EFalse );
	const CCalEntry& entry = *( CombinedEntry() );
	if ( iInParams.iCallingApp.iUid == KUidMailApplication &&
	     IsEntryUsableL( entry ) )
		{
        return ETrue;
		}
	return retVal;
	}
	
TBool CMRCancelModel::CanRemoveFromCalendarL()
	{
	const CCalEntry& entry = *( CombinedEntry() );
	return MREntryConsultant::ExistsInDbL( entry, iMRUtils );
	}

//  End of File
