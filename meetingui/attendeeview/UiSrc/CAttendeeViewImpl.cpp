/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements interface for CAttendeeViewImpl
*
*/



// INCLUDE FILES
#include "cattendeeviewimpl.h"
#include "attendeeviewconsts.h"
#include "cattendeeeng.h"
#include "cattendeeviewerdlg.h"
#include "cattendeeeditordlg.h"
#include "cattendeelbmodel.h"
#include "cattendeeuiutils.h"
#include <attendeeview_res.rsg>
#include <calentry.h>
#include <caluser.h>

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CAttendeeViewImpl::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAttendeeViewImpl* CAttendeeViewImpl::NewL( /*TAny* aPhoneOwner*/ )
    {
    CAttendeeViewImpl* self = 
        new (ELeave) CAttendeeViewImpl( /* const_cast< CCalUser* >( aPhoneOwner )*/ );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); // self
    return self;
    }

// ---------------------------------------------------------
// CAttendeeViewImpl::ConstructL
// ---------------------------------------------------------
//
void CAttendeeViewImpl::ConstructL()
    {    
    // Create utils and read resource files
    iUiUtils = CAttendeeUiUtils::NewL();    
    iUiUtils->OpenResourceFilesL();  

    }

// ----------------------------------------------------------------------------
// CAttendeeViewImpl::CAttendeeViewImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CAttendeeViewImpl::CAttendeeViewImpl( /*CCalUser* aPhoneOwner*/ )
    //  : iPhoneOwner( aPhoneOwner )
    {
    }

// ---------------------------------------------------------
// CAttendeeViewImpl::~CAttendeeViewImpl
// ---------------------------------------------------------
//
CAttendeeViewImpl::~CAttendeeViewImpl()
    {
    delete iDialog;
    if ( iUiUtils )
        {
        iUiUtils->CloseResourceFile();
        }
    delete iUiUtils;
    delete iEngine;
    }

// ---------------------------------------------------------
// CAttendeeViewImpl::ExecuteViewL
// ---------------------------------------------------------
//
TInt CAttendeeViewImpl::ExecuteViewL( RPointerArray<CCalEntry>& aEntries,
                           const TAgnEntryUiInParams& aInParams,
                           TAgnEntryUiOutParams& aOutParams,
                           MAgnEntryUiCallback& aCallback )
    {
    // Check that aEntries only has single entry
    if ( aEntries.Count() != 1 )
        {
        User::Leave( KErrArgument );
        }
        
    CCalEntry* entry = aEntries[ 0 ];

    // Check that organizer is set.
    CCalUser* organizer = entry->OrganizerL();
    if ( !organizer )
        {
        User::Leave( KErrArgument );
        }
    delete organizer;
    
    // Check that phone owner is set.
    CCalUser* phoneOwner = entry->PhoneOwnerL();
    if ( !phoneOwner )
        {
        User::Leave( KErrArgument );
        }
    delete phoneOwner;
    
    // Create the engine and parse the entry.
    iEngine = CAttendeeEng::NewL( *entry );
    iEngine->ParseAgnEntryL();



    // Create the model
    MDesCArray* model = CAttendeeLBModel::NewLC( *iEngine );


    // Create the dialog
    if ( aInParams.iEditorMode == EEditExistingEntry )
        {
        // take model's ownership
        iDialog = CAttendeeEditorDlg::NewL( 
                                    model, 
                                    R_ATTENDEEVIEW_EDITOR_MENUBAR, 
                                    R_ATTENDEEVIEW_EDITOR_CONTEXT_MENUBAR,
                                    *iEngine,
                                    iHelpContext,
                                    aCallback );
        }
    else if ( aInParams.iEditorMode == EViewEntry )
        {
        // take model's ownership
        iDialog = CAttendeeViewerDlg::NewL( 
                                       model, 
                                       R_ATTENDEEVIEW_VIEW_MENUBAR, 
                                       R_ATTENDEEVIEW_VIEWER_CONTEXT_MENUBAR, 
                                       *iEngine,
                                       iHelpContext,
                                       aCallback );
        }

    else // ECreateNewEntry not supported.
        {
        User::Leave( KErrArgument );
        }

    // Pop the model since the ownership was taken.
    CleanupStack::Pop( static_cast<CAttendeeLBModel*>( model ) );
 
    // Run the dialog
    iDialog->PrepareLC( R_ATTENDEE_VIEW_DIALOG ); //no need to pop    
    TInt ret( 0 );
    ret = iDialog->RunLD();
    iDialog = NULL;

    // Fill aOutParams
    if ( iEngine->IsEditedL() )
        {
        aOutParams.iAction = EMeetingSaved;
        }
    else
        {
        aOutParams.iAction = ENoAction;
        }

    return ret;
    }
    
// ---------------------------------------------------------
// CAttendeeViewImpl::SetHelpContext
// ---------------------------------------------------------
//
void CAttendeeViewImpl::SetHelpContext( const TCoeHelpContext& aContext )
    {
    iHelpContext.iContext = aContext.iContext;
    iHelpContext.iMajor = aContext.iMajor;
    }


// End of File
