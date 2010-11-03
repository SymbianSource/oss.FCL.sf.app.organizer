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
* Description:  
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CAgnEntryUiImpl.h"
#include <CMRViewers.h>
#include <calentry.h>
#include <CalenEditorsPlugin.h>
#include <CalenInterimUtils2.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

}  // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CAgnEntryUiImpl::NewL
// ----------------------------------------------------------------------------
//
CAgnEntryUiImpl* CAgnEntryUiImpl::NewL( TAny* aMtmUid )
	{
	CAgnEntryUiImpl* self = new( ELeave ) CAgnEntryUiImpl( aMtmUid );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CAgnEntryUiImpl::CAgnEntryUiImpl
//
// Constructor.
// ----------------------------------------------------------------------------
//
CAgnEntryUiImpl::CAgnEntryUiImpl( TAny* aMtmUid )
    {
    iMtmUid = reinterpret_cast<HBufC8*>( aMtmUid ); // ownership transferred
    }

// ----------------------------------------------------------------------------
// CAgnEntryUiImpl::~CAgnEntryUiImpl
//
// Destructor.
// ----------------------------------------------------------------------------
//
CAgnEntryUiImpl::~CAgnEntryUiImpl()
    {
    delete iMtmUid;
    delete iCalEditor;
    delete iMRViewer;
    }

// ----------------------------------------------------------------------------
// CAgnEntryUiImpl::ConstructL
// ----------------------------------------------------------------------------
//
void CAgnEntryUiImpl::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CAgnEntryUiImpl::ExecuteViewL
// Client (at least Calendar) may call this method repeatedly, therefore we
// maintain iMRViewer and iCalEditor
// ----------------------------------------------------------------------------
//
TInt CAgnEntryUiImpl::ExecuteViewL(
    RPointerArray<CCalEntry>& aEntries,
    const TAgnEntryUiInParams& aInParams,
    TAgnEntryUiOutParams& aOutParams,
    MAgnEntryUiCallback& aCallback)
    {
    MAgnEntryUi* viewer = NULL;
    TInt retVal( KErrNone );

	CCalEntry& entry = *( aEntries[0] );
    if ( CCalenInterimUtils2::IsMeetingRequestL( entry ) )
        {
        if ( !iMRViewer )
            {            
            TRAPD( viewerErr, iMRViewer = CMRViewers::NewL( *iMtmUid ) );
            
            // If CMRViewers plugin is not found try to create
            // CCalenEditorsPlugin default implemetation
            if ( viewerErr != KErrNone )
                {
                iMRViewer = CCalenEditorsPlugin::NewL();
                }
            }
        viewer = iMRViewer;
        }
    else // vTodo etc.
        {
        if ( !iCalEditor )
            {            
            iCalEditor = CCalenEditorsPlugin::NewL();
            }
        viewer = iCalEditor;
        }

    if ( !iHelpContext.IsNull() )
        {        
        viewer->SetHelpContext( iHelpContext );
        }
        
    retVal = viewer->ExecuteViewL( aEntries, aInParams, aOutParams, aCallback );
    return retVal;
    }

// ----------------------------------------------------------------------------
// CAgnEntryUiImpl::SetHelpContext
// ----------------------------------------------------------------------------
//
void CAgnEntryUiImpl::SetHelpContext(
    const TCoeHelpContext& aContext )
    {
    iHelpContext.iContext = aContext.iContext;
    iHelpContext.iMajor = aContext.iMajor;
    }

// End of file
