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
* Description:  Provides methods for CAttendeeCaller class.
*
*/



// INCLUDE FILES
#include "cattendeecaller.h"
#include "attendeeview.hrh"
#include <attendeeview_res.rsg>
#include <aiwcommon.hrh>
#include <eikmenub.h>
#include <cpbkphonenumberselect.h>
#include <cpbkcontactitem.h>
#include <cpbkfieldinfo.h>
#include <aiwdialdatatypes.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAttendeeCaller::CAttendeeCaller
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAttendeeCaller::CAttendeeCaller()
    {
    }

// ----------------------------------------------------
// CAttendeeCaller::ConstructL
// ----------------------------------------------------
//
void CAttendeeCaller::ConstructL()
    {
	//Create service handler
	iServiceHandler = CAiwServiceHandler::NewL();

	//Atttach menu
	iServiceHandler->AttachMenuL( R_ATTENDEEVIEW_CALL_MENU,
							      R_ATTENDEEVIEW_CALL_AIW_INTEREST );

    // Attach also base interest
    iServiceHandler->AttachL( R_ATTENDEEVIEW_CALL_AIW_INTEREST );       
    }

// -----------------------------------------------------------------------------
// CAttendeeCaller::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAttendeeCaller* CAttendeeCaller::NewL()
    {
    CAttendeeCaller* self = new(ELeave)CAttendeeCaller;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CAttendeeCaller::~CAttendeeCaller
// ----------------------------------------------------
//
CAttendeeCaller::~CAttendeeCaller()
    {
    delete iServiceHandler;
    }

// ---------------------------------------------------------
// CAttendeeCaller::InitializeServiceHandlerL
// ---------------------------------------------------------
//
void CAttendeeCaller::InitializeServiceHandlerL( CEikMenuPane& aMenuPane,
                                                 TBool aVisible ) const
    {
    if ( aVisible )
        {

        iServiceHandler->InitializeMenuPaneL(
                    aMenuPane,
                    R_ATTENDEEVIEW_CALL_MENU,
                    KAiwCmdCall,
                    iServiceHandler->InParamListL() );
                    
        }
    else
        {
        aMenuPane.SetItemDimmed( KAiwCmdCall, ETrue );
        }
    }

// ---------------------------------------------------------
// CAttendeeCaller::CallCmdL
// ---------------------------------------------------------
//
void CAttendeeCaller::CallCmdL( TInt aCommandId, CPbkContactItem& aPbkItem )
    {
    
    if ( iServiceHandler->ServiceCmdByMenuCmd( aCommandId ) == KAiwCmdNone &&
           aCommandId != EAttCallCmd )
        {
        return;
        }
        
    TAiwDialDataV1 dialData;

    // Set basic dial data
    dialData.SetWindowGroup( CCoeEnv::Static()->RootWin().Identifier() );
    dialData.SetContactId( aPbkItem.Id() );

    const TPbkContactItemField* field = SelectPhoneNumberL( aPbkItem );
    if ( !field )
        {
        return;
        }

    dialData.SetNumberType( field->FieldInfo().FieldId() );
    TAiwTelephoneNumber phoneNumber( 
        field->Text().Left( KAiwTelephoneNumberLength ) );
    dialData.SetTelephoneNumber( phoneNumber );
    dialData.SetRemoveInvalidChars( ETrue );

    // If the command was not launched via menu but the send key
    // (which is the case when iCommandId is EAttCallCmd), we
    // need to deduct whether to launch voice or video call
    if ( aCommandId == EAttCallCmd )
        {
        // Default to voice call
        dialData.SetCallType( EAiwVoice );
        
        // CASE A:
        // If there is a no default phone number field, but there
        // is a default video number field, launch video call
        if ( !aPbkItem.DefaultPhoneNumberField() &&
              aPbkItem.DefaultVideoNumberField() )
            {
            dialData.SetCallType( EAiwForcedVideo );
            }

        // CASE B:
        // No default fields at all. This means that the user
        // was shown a number selection list, or there was just
        // one number
        else if ( !aPbkItem.DefaultPhoneNumberField() &&
                  !aPbkItem.DefaultVideoNumberField() )
            {
            // If the selected field was a video field, launch video call
            if ( field->FieldInfo().FieldId() == EPbkFieldIdPhoneNumberVideo )
                {
                dialData.SetCallType( EAiwForcedVideo );
                }
            }         
        }

    TAiwDialDataV1Pckg dialDataPckg( dialData );
    CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
    TPtrC8 ptr;
    ptr.Set( dialDataPckg );
    TAiwVariant variant( ptr );

    TAiwGenericParam param( EGenericParamCallDialDataV1,
                            variant );
    paramList.AppendL( param );

    if ( aCommandId == EAttCallCmd )
       {
       iServiceHandler->ExecuteServiceCmdL( KAiwCmdCall,
                                    paramList,
                                    iServiceHandler->OutParamListL(), 
                                    0,
                                    NULL );
       }
    else
        {
        iServiceHandler->ExecuteMenuCmdL( aCommandId,
                                      paramList,
                                      iServiceHandler->OutParamListL(),
                                      0,
                                      NULL );   
        }
    }

// ---------------------------------------------------------
// CAttendeeCaller::HandleSubmenuL
// ---------------------------------------------------------
//
TBool CAttendeeCaller::HandleSubmenuL( CEikMenuPane& aPane )
    {
    return iServiceHandler->HandleSubmenuL( aPane );
    }

// ---------------------------------------------------------
// CAttendeeCaller::SelectPhoneNumberL
// ---------------------------------------------------------
//
const TPbkContactItemField* CAttendeeCaller::SelectPhoneNumberL( 
                                  CPbkContactItem& aContactItem )
    {    
    CPbkPhoneNumberSelect::TParams params( aContactItem );    
	    
    CPbkPhoneNumberSelect* dlg = new(ELeave)CPbkPhoneNumberSelect;
    dlg->ExecuteLD( params );
    
    return params.SelectedField();       
    }
    
//  End of File
