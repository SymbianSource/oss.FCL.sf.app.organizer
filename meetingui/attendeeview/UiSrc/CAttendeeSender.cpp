/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides methods for wrapper class CSendAppUi.
*
*/



// INCLUDE FILES
#include "CAttendeeSender.h"
#include "AttendeeView.hrh"
#include "AttendeeViewConsts.h"
#include <sendui.h>
#include <eikmenup.h>
#include <SenduiMtmUids.h>
#include <CMessageData.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAttendeeSender::CAttendeeSender
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAttendeeSender::CAttendeeSender()
    {
    }

// ----------------------------------------------------
// CAttendeeSender::ConstructL
// ----------------------------------------------------
//
void CAttendeeSender::ConstructL()
    {
    iSendUi = CSendUi::NewL();
    }

// -----------------------------------------------------------------------------
// CAttendeeSender::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAttendeeSender* CAttendeeSender::NewL()
    {
    CAttendeeSender* self = new(ELeave)CAttendeeSender;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------
// CAttendeeSender::~CAttendeeSender
// ----------------------------------------------------
//
CAttendeeSender::~CAttendeeSender()
    {
    delete iSendUi;
    }

// ---------------------------------------------------------
// CAttendeeSender::DisplaySendMenuL
// ---------------------------------------------------------
//
void CAttendeeSender::DisplaySendMenuL(
                            CEikMenuPane& aMenuPane,
                            TInt aVisibleCount ) const
    {
    TInt pos( 0 );
    aMenuPane.ItemAndPos( EAttSendDummyCmd, pos );
    // Set dummy command dimmed anyway. It was only needed because
    // SendUI needs to know the right position:
    aMenuPane.SetItemDimmed( EAttSendDummyCmd, ETrue );

    // Now if there are items visible then let SendUi check whether
    // option can be shown:
    if ( aVisibleCount > 0 )
        {
        TSendingCapabilities capabilities( 0, 0,
                                TSendingCapabilities::EAllServices );                                                                                          
        iSendUi->AddSendMenuItemL( aMenuPane, pos, EAttCmdSend, capabilities );         
        }
    }

// ---------------------------------------------------------
// CAttendeeSender::ShowSendQueryL
// ---------------------------------------------------------
//
TUid CAttendeeSender::ShowSendQueryL( const TBool aSmsVisible ) const
    {
    
    TSendingCapabilities capabilities = TSendingCapabilities();
    capabilities.iFlags = TSendingCapabilities::ESupportsEditor;
    
    // Construct the MTM filter
	CArrayFixFlat<TUid>* mtmFilter =
		new(ELeave) CArrayFixFlat<TUid>(1);
	CleanupStack::PushL( mtmFilter );

    if ( !aSmsVisible )
        {
        mtmFilter->AppendL( KSenduiMtmSmsUid ); //Sms        
        }
    
    
    TUid uid = iSendUi->ShowSendQueryL( NULL, 
                                        capabilities, 
                                        mtmFilter );
    CleanupStack::PopAndDestroy( mtmFilter );                                         
    return uid;
    }

// ----------------------------------------------------
// CAttendeeSender::CreateAndSendL
// ----------------------------------------------------
//
void CAttendeeSender::CreateAndSendL( TUid aMtmUid, 
                                      CMessageData* aMessageData )
    {                                    
    iSendUi->CreateAndSendMessageL( aMtmUid, aMessageData );
    }
//  End of File
