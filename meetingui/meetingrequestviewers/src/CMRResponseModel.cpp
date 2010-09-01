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
* Description:   Model for method RESPONSE
*
*/




// INCLUDE FILES
#include "CMRResponseModel.h"
#include "CMRModelBase.h"
#include "MMRDataFillerInterface.h"
#include "CMRDataFillerReply.h"
#include <eikenv.h> //CEikonEnv
#include <CRichBio.h> //rich bio
#include "CMRUtilsInternal.h"
#include <MsgMailUIDs.h> //uid for mail application
#include <stringloader.h>
#include <avkon.rsg> // resouce identifiers
#include <calcommon.h> //TCalTimeRange
#include <senduimtmuids.h> //mailbox id's
#include "MRViewersPanic.h"
#include <MRCommands.hrh>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CMRResponseModel" );

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
CMRResponseModel::CMRResponseModel(
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
void CMRResponseModel::ConstructL( RPointerArray<CCalEntry>& aEntries )
    {
    CMRModelBase::ConstructL( aEntries );
    }

// -----------------------------------------------------------------------------
// ?classname::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMRResponseModel* CMRResponseModel::NewL(
    RPointerArray<CCalEntry>& aEntries,
    CMRMailboxUtils& aMRMailboxUtils,
    CMRUtilsInternal& aMRUtils,
    const MAgnEntryUi::TAgnEntryUiInParams& aInParams )
    {
    CMRResponseModel* self = new( ELeave ) CMRResponseModel( aMRMailboxUtils,
                                                             aMRUtils,
                                                             aInParams );

    CleanupStack::PushL( self );
    self->ConstructL( aEntries );
    CleanupStack::Pop();

    return self;
    }

// Destructor
CMRResponseModel::~CMRResponseModel()
    {
    }

MMRModelInterface::TClosingStatus CMRResponseModel::ClosingStatusL()
	{
	TClosingStatus retVal( ENone );
	return retVal;
	}

MMRDataFillerInterface* CMRResponseModel::DataFillerL( CRichBio* aRichBio )
	{
	__ASSERT_DEBUG(aRichBio,Panic(ECRichBioNull));
	MMRDataFillerInterface* dataFillerInterface(NULL);
	dataFillerInterface = CMRDataFillerReply::NewL( *aRichBio,
	                                                *this,
	                                                *CEikonEnv::Static(),
	                                                iMRMailboxUtils,
	                                                iInParams );
	return dataFillerInterface;
	}

TBool CMRResponseModel::IsCmdAvailableL( TInt aCommandId )
    {
    TInt retVal( EFalse );
    if ( aCommandId == EMRCommandReplyToSender )
        {
        retVal = ETrue;
        }
    else
        {        
        retVal = CMRModelBase::IsCmdAvailableL( aCommandId );
        }
    return retVal;
    }

//  End of File
