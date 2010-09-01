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
* Description:  Provides methods for .
*
*/



// INCLUDE FILES
#include "cattendeeviewerdlg.h"
#include "cattendeedialogbase.h"
#include "mattendeeeng.h"
#include "cattendeedetailsdlg.h"
#include "attendeeviewuid.h"
#include "attendeeview.hrh"

#include <attendeeview_res.rsg>
#include <stringloader.h>
#include <caluser.h>
#include <csxhelp/cale.hlp.hrh>
#include <mrcommands.hrh>

// ============================ MEMBER FUNCTIONS ==============================
CAttendeeViewerDlg* CAttendeeViewerDlg::NewL( 
                                 MDesCArray *aArray,
                                 TInt aMenuBarResourceId,                                          
                                 TInt aOkMenuBarResourceId,
                                 MAttendeeEng& aEngine,
                                 TCoeHelpContext& aHelpContext,
                                 MAgnEntryUiCallback& aEntryUiCallback )
    {
    CAttendeeViewerDlg* self = NewLC( aArray,
                                      aMenuBarResourceId,
                                      aOkMenuBarResourceId,
                                      aEngine,
                                      aHelpContext,
                                      aEntryUiCallback );
    CleanupStack::Pop(self); //self
  
    return self;
    }

CAttendeeViewerDlg* CAttendeeViewerDlg::NewLC( 
                                   MDesCArray *aArray,
                                   TInt aMenuBarResourceId,
                                   TInt aOkMenuBarResourceId,
                                   MAttendeeEng& aEngine,
                                   TCoeHelpContext& aHelpContext,
                                   MAgnEntryUiCallback& aEntryUiCallback )
    {
    CAttendeeViewerDlg* self = 
        new(ELeave)CAttendeeViewerDlg( aArray, 
                                       aEngine,
                                       aMenuBarResourceId,
                                       aOkMenuBarResourceId,
                                       aHelpContext,
                                       aEntryUiCallback );
    CleanupStack::PushL( self );
    self->ConstructL( aMenuBarResourceId );
    
    return self;
    }
// ----------------------------------------------------------------------------
// CAttendeeViewerDlg::CAttendeeViewerDlg
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CAttendeeViewerDlg::CAttendeeViewerDlg( 
                                 MDesCArray *aArray, 
                                 MAttendeeEng& aEngine,
                                 TInt aMenuBarResourceId,
                                 TInt aOkMenuBarResourceId,
                                 TCoeHelpContext& aHelpContext,
                                 MAgnEntryUiCallback& aEntryUiCallback ):
    CAttendeeDialogBase( aArray, 
                         aEngine, 
                         aMenuBarResourceId, 
                         aOkMenuBarResourceId,
                         aEntryUiCallback ),
    iHelpContext( aHelpContext )
    {

    }

// ---------------------------------------------------------
// CAttendeeViewerDlg::~CAttendeeViewerDlg
// ---------------------------------------------------------
//
CAttendeeViewerDlg::~CAttendeeViewerDlg()
    {
    }

// ---------------------------------------------------------
// CAttendeeViewerDlg::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CAttendeeViewerDlg::DynInitMenuPaneL( TInt aResourceId,
                                            CEikMenuPane* aMenuPane )
    {

    const TInt visible( ListBox()->Model()->NumberOfItems() );
    const TInt marked( ListBox()->SelectionIndexes()->Count() );
    switch( aResourceId )
        {      
        case R_ATTENDEEVIEW_VIEWER_MENU:
            {
            if ( iEntryUiCallback.IsCommandAvailable( EMRCommandRespondAccept )
                    && marked <= 0 )
                {
                aMenuPane->SetItemDimmed( EAttCmdRespond, EFalse );
                }
            else
                {
                aMenuPane->SetItemDimmed( EAttCmdRespond, ETrue );
                }
                
            if ( marked > 0 )
                {
                aMenuPane->SetItemDimmed( EAttDetails, ETrue );
                }
            if ( visible <= 0 ) 
                {
                aMenuPane->SetItemDimmed( EAttDetails, ETrue );
                }
            break;
            }
        case R_ATTENDEEVIEW_VIEWER_CONTEXT_MENU:
            {
            if ( marked > 0 )
                {                
                aMenuPane->SetItemDimmed( EAttDetails, ETrue );                
                aMenuPane->SetItemDimmed( EAttCmdRespond, ETrue );
                aMenuPane->SetItemDimmed( EAttCmdSend, EFalse );                               
                }
            else
                {                
                if ( iEntryUiCallback.IsCommandAvailable( EMRCommandRespondAccept ) )
                    {
                    aMenuPane->SetItemDimmed( EAttCmdRespond, EFalse );
                    }
                else
                    {
                    aMenuPane->SetItemDimmed( EAttCmdRespond, ETrue );
                    }
                    
                aMenuPane->SetItemDimmed( EAttCmdSend, ETrue );
                }
            break;
            }
        default:
            {
            CAttendeeDialogBase::DynInitMenuPaneL( aResourceId, aMenuPane );
            break;
            }
        }
    }

// ----------------------------------------------------
// CAttendeeViewerDlg::MakeEmptyTextListBoxL
// ----------------------------------------------------
//
void CAttendeeViewerDlg::MakeEmptyTextListBoxL()
    {
    HBufC* textFromResourceFile;
    textFromResourceFile = 
        StringLoader::LoadLC( R_ATTENDEE_VIEWER_EMPTY_TEXT, 
                              CEikonEnv::Static() );    
    ListBox()->View()->SetListEmptyTextL( *textFromResourceFile );
    CleanupStack::PopAndDestroy( textFromResourceFile ); 
    }

// ----------------------------------------------------
// CAttendeeDialogBase::DetailsCmdL
// ---------------------------------------------------------
//
void CAttendeeViewerDlg::DetailsCmdL()
    {
    TInt index( ListBox()->CurrentItemIndex() );
    if ( index >= 0 && index < ListBox()->Model()->NumberOfItems() 
         && index < iEngine.NumberOfItems())
        {
        CPbkContactItem* item = iEngine.PbkContactItemL( index );
        if ( !item )
            {
            item = CreateContactL(
                    iEngine.At( index ).AgnAttendee()->Address() );
            }
        CleanupStack::PushL( item );

        CAttendeeDetailsDlg* dlg = CAttendeeDetailsDlg::NewL(
                                item,
                                &iEngine.ContactEngine(),
                                iEngine.At( index ).AgnAttendee()->StatusL(),
                                EFalse ); 
        dlg->RunLD();

        CleanupStack::PopAndDestroy( item );
        }
    }

// -----------------------------------------------------------------------------
// CAttendeeDialogBase::GetHelpContext
// -----------------------------------------------------------------------------
//
void CAttendeeViewerDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    
    //If help text was given in construction use it 
    if(!iHelpContext.IsNull())
        {
        aContext = iHelpContext;
        }
    else
        {
        //Remove comments after new cale.hlp.hrh is ready
        //aContext.iContext = KCALE_HLP_ATTENDEE_VIEW;
        //aContext.iMajor = TUid::Uid( KEcomDllUID );
        }    
    }
   
// End of File
