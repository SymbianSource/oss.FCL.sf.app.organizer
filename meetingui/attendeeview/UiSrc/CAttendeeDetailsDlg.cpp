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
* Description:  Provides interface methods of CAttendeeDetailsDlg.
*
*/



// INCLUDE FILES
#include "CAttendeeDetailsDlg.h"
#include "CAttendeeUiUtils.h"
#include "CAttendeeItem.h"
#include <AttendeeView_res.rsg>
#include <CPbkContactDetailsBaseControl.h>


// ============================ MEMBER FUNCTIONS ==============================
// ---------------------------------------------------------
// CAttendeeDetailsDlg::NewL
// ---------------------------------------------------------
//
CAttendeeDetailsDlg* CAttendeeDetailsDlg::NewL( 
                                             CPbkContactItem* aContactItem, 
                                             CPbkContactEngine* aEngine,
                                             CCalAttendee::TCalStatus aStatus,
                                             TBool aShowNaviLabel )
                                                
    {
    CAttendeeDetailsDlg* self = NewLC( aContactItem, aEngine, 
                                       aStatus, aShowNaviLabel );
    CleanupStack::Pop( self ); //self
    return self;
    }

// ---------------------------------------------------------
// CAttendeeDetailsDlg::NewLC
// ---------------------------------------------------------
//
CAttendeeDetailsDlg* CAttendeeDetailsDlg::NewLC( 
                                           CPbkContactItem* aContactItem, 
                                           CPbkContactEngine* aEngine,
                                           CCalAttendee::TCalStatus	aStatus,
                                           TBool aShowNaviLabel )
                                                
    {
    CAttendeeDetailsDlg* self = new(ELeave)CAttendeeDetailsDlg;
    CleanupStack::PushL( self );
    self->ConstructL( aContactItem, aEngine, aStatus, aShowNaviLabel );
    return self;
    }

// ---------------------------------------------------------
// CAttendeeDetailsDlg::ConstructL
// ---------------------------------------------------------
//
void CAttendeeDetailsDlg::ConstructL( CPbkContactItem* aContactItem, 
                                      CPbkContactEngine* aEngine,
                                      CCalAttendee::TCalStatus	aStatus,
                                      TBool aShowNaviLabel)
                                      
	{
    //Initial dialog
    CAknDialog::PrepareLC( R_ATTENDEE_DETAILS_DIALOG );

    //Create control
    iControl = CPbkContactDetailsBaseControl::NewL(
                                    R_ATTENDEE_CONTACTINFO_CONTROL,
                                    *this,
                                    aContactItem,        
                                    aEngine );
    
    //change navi label
    iUiUtils = CAttendeeUiUtils::NewL();
    iUiUtils->StoreNaviPane();

    //change title pane
    iUiUtils->StoreTitlePaneL();
    iUiUtils->ChangeTitlePaneL( R_ATTENDEE_DETAILS_TITLE_PANE );
    
    CreateNaviLabelL( aStatus, aShowNaviLabel );
	}

// ----------------------------------------------------------------------------
// CAttendeeDetailsDlg::CAttendeeDetailsDlg
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CAttendeeDetailsDlg::CAttendeeDetailsDlg()
    {
    }

// ---------------------------------------------------------
// CAttendeeDetailsDlg::~CAttendeeDetailsDlg
// ---------------------------------------------------------
//
CAttendeeDetailsDlg::~CAttendeeDetailsDlg()
    {
    //restore title pane
    if ( iUiUtils )
        {        
        iUiUtils->RestoreOldTitlePane();
        }

    //Delete restore also old navilabel
    delete iUiUtils;
    delete iControl;
    }

// ---------------------------------------------------------
// CAttendeeDetailsDlg::Draw
// ---------------------------------------------------------
//
void CAttendeeDetailsDlg::Draw( const TRect& /*aRect*/ ) const
    {
    // this removes flicker from CEikDialog's Draw().
    }

// ---------------------------------------------------------
// CAttendeeDetailsDlg::SizeChanged
// ---------------------------------------------------------
//
void CAttendeeDetailsDlg::SizeChanged()
    {
    if ( iControl )
        {
        iControl->SetRect( Rect() );
        iControl->DrawNow();
        }
    }

// ---------------------------------------------------------
// CAttendeeDetailsDlg::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CAttendeeDetailsDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                  TEventCode aType )
    {
    iControl->OfferKeyEventL( aKeyEvent, aType );  
    return CAknDialog::OfferKeyEventL( aKeyEvent, aType );     
    }

// ---------------------------------------------------------
// CAttendeeDetailsDlg::CountComponentControls
// ---------------------------------------------------------
//
TInt CAttendeeDetailsDlg::CountComponentControls() const
    {    
    return 1;
    }

// ---------------------------------------------------------
// CAttendeeDetailsDlg::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CAttendeeDetailsDlg::ComponentControl( TInt /*aIndex*/ ) const
    {    
    return iControl;
    }

// ---------------------------------------------------------
// CAttendeeDetailsDlg::CreateNaviLabelL
// ---------------------------------------------------------
//
void CAttendeeDetailsDlg::CreateNaviLabelL( CCalAttendee::TCalStatus aStatus, 
                                            TBool aShowNaviLabel )
    {
    
    if ( aShowNaviLabel )
        {
    	switch( aStatus )
            {
            case CCalAttendee::EAccepted:
                {
                iUiUtils->CreateNaviLabelL( R_ATTENDEE_NAVI_LABEL_ACCEPTED );
                break;
                }
            case CCalAttendee::ETentative:
                {
                iUiUtils->CreateNaviLabelL( R_ATTENDEE_NAVI_LABEL_TENTATIVE );
                break;
                }
            case CCalAttendee::EDeclined:
                {
                iUiUtils->CreateNaviLabelL( R_ATTENDEE_NAVI_LABEL_REJECTED );
                break;
                }        
            default:
                {
                iUiUtils->CreateNaviLabelL( 
                                        R_ATTENDEE_NAVI_LABEL_NOT_ANSWERED );
                break;
                }
            }        	
        }
    else
        {
    	//Set empty text to navi pane
    	iUiUtils->CreateNaviLabelL( KNullDesC );
        }
    
    }
// End of File
