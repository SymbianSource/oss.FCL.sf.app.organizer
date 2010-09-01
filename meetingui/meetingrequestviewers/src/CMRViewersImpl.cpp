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
* Description: Implementation for meeting request viewers  
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CMRViewersImpl.h"
#include "ICalUILog.h"
#include "MRHelpers.h"
#include "CMRHandler.h"
#include "MRViewersPanic.h" //panic enums
#include <coemain.h> //coe env
#include <bautils.h> //bafl utils
#include <data_caging_path_literals.hrh> //data caging constants
#include <f32file.h> //file server session
#include <CMRMailboxUtils.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CMRViewersImpl" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

_LIT( KResourceFileName,"meetingrequestviewersuires.rsc" );

}  // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CMRViewersImpl::NewL
// ----------------------------------------------------------------------------
//
CMRViewersImpl* CMRViewersImpl::NewL( TAny* aMtmUid )
	{
	CMRViewersImpl* self = new( ELeave ) CMRViewersImpl( aMtmUid );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CMRViewersImpl::CMRViewersImpl
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRViewersImpl::CMRViewersImpl( TAny* aMtmUid )
    : iEngStatus( ENotReady )
    {
    iMtmUid = reinterpret_cast<HBufC8*>( aMtmUid ); // ownership transferred
    }

// ----------------------------------------------------------------------------
// CMRViewersImpl::~CMRViewersImpl
//
// Destructor.
// ----------------------------------------------------------------------------
//
CMRViewersImpl::~CMRViewersImpl()
    {
    LOG("CMRViewersImpl::~CMRViewersImpl");
    delete iIdleTimer;
    delete iMtmUid;
    delete iMRHandler;
    delete iMRUtils;
    delete iMRMailboxUtils;    

    if ( iResourceFileOffset )
    	{
        CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
    	}

    LOG("CMRViewersImpl::~CMRViewersImpl -> End");
    }

// ----------------------------------------------------------------------------
// CMRViewersImpl::ConstructL
// ----------------------------------------------------------------------------
//
void CMRViewersImpl::ConstructL()
    {
    LOG("CMRViewersImpl::ConstructL, loading resource");
    iResourceFileOffset = MRHelpers::LoadResourceL( KResourceFileName,
                                                    KDC_RESOURCE_FILES_DIR );	
    LOG("CMRViewersImpl::ConstructL -> End");
    }

// ----------------------------------------------------------------------------
// CMRViewersImpl::ExecuteViewL
// ----------------------------------------------------------------------------
//
TInt CMRViewersImpl::ExecuteViewL(
    RPointerArray<CCalEntry>& aEntries,
    const TAgnEntryUiInParams& aInParams,
    TAgnEntryUiOutParams& aOutParams,
    MAgnEntryUiCallback& aCallback)
    {
    LOG("CMRViewersImpl::ExecuteViewL");
    TInt returnCode( KErrNone );
    
    // check that utils objects (configured to use given sessions) exist:
    EnsureUtilsExistL( &( aInParams.iCalSession ), aInParams.iMsgSession );
    
    // CMRHandler is not reusable, must be re-created each time
    LOG("CMRViewersImpl::ExecuteViewL, creating iMRHandler");
    delete iMRHandler;
    iMRHandler = NULL;
    iMRHandler = CMRHandler::NewL( aEntries,
                                   aInParams,
                                   aOutParams,
                                   aCallback );
    iMRHandler->SetHelpContext( *iContext );

    LOG("CMRViewersImpl::ExecuteViewL, executing view");
    returnCode = iMRHandler->ExecuteViewL();        

    LOG("CMRViewersImpl::ExecuteViewL -> End");
    return returnCode;
    }

// ----------------------------------------------------------------------------
// CMRViewersImpl::SetHelpContext
// ----------------------------------------------------------------------------
//
void CMRViewersImpl::SetHelpContext(
    const TCoeHelpContext& aContext )
    {
	iContext = &aContext;
    }

// ----------------------------------------------------------------------------
// CMRViewersImpl::EnsureUtilsExistL
// Will cause HandleCalEngStatus() callback regardless MR Utils is created
// or not. It is done that way to ensure that program flow is similar in
// both cases.
// ----------------------------------------------------------------------------
//
void CMRViewersImpl::EnsureUtilsExistL(
    CCalSession* aCalSession,
    CMsvSession* aMsgSession )
    {
    // no iMRUtils exist or session has changed so that must re-instantiate
    if ( !iMRUtils || 
         aCalSession != iCalSession || 
         aMsgSession != iMsgSession )
        {
        delete iMRUtils;
        iMRUtils = NULL;
        LOG("CMRViewersImpl::EnsureUtilsExistL, creating iMRUtils");
	    iMRUtils = CMRUtilsInternal::NewL( *this,
	                                       *aCalSession,
	                                       aMsgSession );
        }
    else
        {
        delete iIdleTimer;
        iIdleTimer = NULL;
        iIdleTimer = CIdle::NewL( CActive::EPriorityIdle );
        iIdleTimer->Start( TCallBack( HandleEngStatusCheck, this ) );
        }
    
    // no iMRMailboxUtils exists or session has changed
    if ( !iMRMailboxUtils || aMsgSession != iMsgSession )
        {        
        delete iMRMailboxUtils;
        iMRMailboxUtils = NULL;
        LOG("CMRViewersImpl::EnsureUtilsExistL, creating iMRMailboxUtils");
	    iMRMailboxUtils = CMRMailboxUtils::NewL( aMsgSession );
        }
        
    // now set session pointer members to contain current values:
    iCalSession = aCalSession;
    iMsgSession = aMsgSession;        
	}

// ----------------------------------------------------------------------------
// CMRViewersImpl::HandleEngStatusCheck
// ----------------------------------------------------------------------------
//
TInt CMRViewersImpl::HandleEngStatusCheck( TAny* aThis )
    {
    CMRViewersImpl* self = static_cast<CMRViewersImpl*>( aThis );
    self->HandleCalEngStatus( self->iEngStatus );
    delete self->iIdleTimer;
    self->iIdleTimer = NULL;
    return 0;
    }

// ----------------------------------------------------------------------------
// CMRViewersImpl::HandleCalEngStatus
// ----------------------------------------------------------------------------
//
void CMRViewersImpl::HandleCalEngStatus( TMRUtilsCalEngStatus aStatus )
	{
	iEngStatus = aStatus;
    TRAPD( err, DoHandleCalEngStatusL( iEngStatus) );
    if ( err )
        {
        // TODO: handle severe error, no meeting request can be shown,
        // either show error note and empty viewer or exit viewer.
        // Notice that in theory status events can be received at any time,
        // viewer might be already happily running.
        }
	}
		
// ----------------------------------------------------------------------------
// CMRViewersImpl::DoHandleCalEngStatusL
// ----------------------------------------------------------------------------
//
void CMRViewersImpl::DoHandleCalEngStatusL( TMRUtilsCalEngStatus aStatus )
	{
    LOG("CMRViewersImpl::DoHandleCalEngStatusL");
	if ( aStatus == MMRUtilsObserver::EAvailable )
		{
        iMRHandler->HandleEngReadyL( *iMtmUid, *iMRUtils, *iMRMailboxUtils );
		}
    else
        { // Problem in the cal eng status, handle error
        LOG("CMRViewersImpl::DoHandleCalEngStatusL, bad cal eng status");
        User::Leave( KErrGeneral );
        }
    LOG("CMRViewersImpl::DoHandleCalEngStatusL -> End");
	}

// End of file
