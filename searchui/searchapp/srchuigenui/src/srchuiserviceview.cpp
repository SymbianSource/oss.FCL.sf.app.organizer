/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     This class is responsible for the display of all the supported contents available for the search * operation. It also contains a search field through which the search can be triggered. 
*
*/






//System Includes
#include <aknViewAppUi.h>
#include <avkon.rsg>
#include <aknenv.h>
#include <avkon.hrh>
#include <srchui.rsg>
#include <hlplch.h> // help launcher
#include <StringLoader.h> 
#include <csxhelp/search.hlp.hrh>

//User Includes
#include "srchuiserviceview.h"
#include "srchuidefines.h"
#include "srchuiservicecontainer.h"
#include "srchuiappuids.h"
#include "srchui.hrh"
#include "srchuigeneral.h"
_LIT(KSpellButton, "Spell");

// ====================================== MEMBER FUNCTIONS =========================================

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::NewL
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSrchUiSearchView* CSrchUiSearchView::NewL(CSrchUiGeneral* aGenSrchUi)
    {
    CSrchUiSearchView* self = CSrchUiSearchView::NewLC(aGenSrchUi);
    CleanupStack::Pop( self );
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::NewLC
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSrchUiSearchView* CSrchUiSearchView::NewLC(CSrchUiGeneral* aGenSrchUi)
    {
    CSrchUiSearchView* self = new ( ELeave ) CSrchUiSearchView();
    CleanupStack::PushL( self );
    self->ConstructL(aGenSrchUi);
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::~CSrchUiServiceContainer
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CSrchUiSearchView::~CSrchUiSearchView()
    {
    DoDeactivate();

    delete iContainer;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::DynInitMenuPaneL
// Changes the CBA buttons.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiSearchView::SetCbaButtonsL( TInt aResourceId )
    {
    if ( Cba() )
        {
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();    
        const TDesC* cbaText = NULL;
            if( cba )
                {
                MEikButtonGroup* buttonGroup = cba->ButtonGroup();
                for( TInt buttonPos = 0; buttonPos < 3; buttonPos++ )
                    {
                    TInt cmdId = buttonGroup->CommandId( buttonPos );
                    CCoeControl* button = buttonGroup->GroupControlById( cmdId );         
                    if( button && buttonGroup->IsCommandVisible( cmdId ))
                        {
                        CEikLabel* label = static_cast<CEikLabel*>( button->ComponentControl(0) );
                        cbaText = label->Text();    
                        if( ( cbaText->Match( KSpellButton )!=KErrNotFound ) && !iCancel )
                        	{
                        	iUpdataCba = EFalse;
                        	break;
                        	}
                        else
                        	{
                        	if(iCancel && ( cbaText->Match( KSpellButton )!=KErrNotFound ))
                        		{
                        		HBufC* optionText = StringLoader::LoadLC( R_TEXT_SOFTKEY_OPTION  );
                        		Cba()->SetCommandL( 0,EAknSoftkeyOptions,*optionText );
                        		Cba()->DrawDeferred();
                        		CleanupStack::PopAndDestroy(); 
                        		}
                        	iUpdataCba = ETrue;
                        	}
                        }
                    }
                }
        if( iUpdataCba )
        	{
            Cba()->SetCommandSetL( aResourceId );
            Cba()->DrawDeferred();
        	}
        }
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::SetContainerListBoxIndex
// Setter for iContainerListBoxIndex.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiSearchView::SetContainerListBoxIndex( TInt aIndex )
    {
    iContainerListBoxIndex = aIndex;
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::ContainerListBoxIndex
// Getter for iContainerListBoxIndex.
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiSearchView::ContainerListBoxIndex()    
    {
    return iContainerListBoxIndex;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::Id
// Returns the unique id of the view.
// -------------------------------------------------------------------------------------------------
//
TUid CSrchUiSearchView::Id() const
    {
    return KSearchServiceViewUid;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::HandleCommandL
// Handles the commands that are passed from softkey generated commands and
// pop up's.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiSearchView::HandleCommandL( TInt aCommand )
    {   
    switch ( aCommand )
        {
        case ESearchCmdSearch:
            {
            iGenSrchUi->SetLastLaunchViewId( Id() );
            iContainer->DoSearchL();
            }
            break;
            
        case EAknSoftkeyBack:
            if( iGenSrchUi->AiwFlag() == FALSE )
                {
                iGenSrchUi->ActivatePreviousViewL();
                }
            else
                {
                iAvkonAppUi->RunAppShutter();
                }
            break;
        
        case EEikCmdExit:
               iAvkonAppUi->ProcessCommandL(EAknCmdExit);     
                break;
            
        case ESearchCmdMark:
            {
            iContainer->HandleItemMarkUnMarkL();
            }
            break;
	        
        case ESearchCmdUnMark:
            {
            iContainer->HandleItemMarkUnMarkL();
            }
            break;

        case ESearchCmdGoToMainView:
            {
            iGenSrchUi->GoToMainViewL();
            }
            break;
            
        case ESearchCmdCancel:
        		{
        		iContainer->ClearTextL();     		
        		_LIT(KEmptyString,"");
            TBufC<3> buf( KEmptyString );
            HBufC* string = buf.AllocL();
            CleanupStack::PushL(string);
    	      iGenSrchUi->SetSearchStringL( *string );
            CleanupStack::PopAndDestroy(string);		
            iCancel = ETrue;
          	iContainer->UpDateCbaL();
          	iCancel = EFalse;
          	}
        		break;
        case ESearchCmdSettings:
            {	    
    	    	iGenSrchUi->LaunchUiSettingsViewL();
            }
            break;
        case ESearchCmdHelp:
            {
            TCoeHelpContext helpContext;
            helpContext.iMajor = KSearchAppUid ;
            helpContext.iContext = KSEARCH_HLP_INDEVICE_MAIN; //add correct help context when available
            
            CArrayFix<TCoeHelpContext>* hlpContextArray = 
                    new ( ELeave ) CArrayFixFlat<TCoeHelpContext> ( 1 );
            
            CleanupStack::PushL( hlpContextArray );
            hlpContextArray->AppendL( helpContext );
            CleanupStack::Pop( hlpContextArray );
            
            //Launch help - takes ownership of hlpContextArray
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                                 hlpContextArray );
                
                
            }
            break;
    	case EAknSoftkeyExit:
            iAvkonAppUi->RunAppShutter();
            break;
    
    default:
        break;
        }
        iContainer->UpdateNaviPaneL();
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::DoActivateL
// Called by the framework when the view is activated.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiSearchView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
                                                                 const TDesC8& /*aCustomMessage*/ )
    {
    if ( !iContainer )
        {
        iContainer = new( ELeave )CSrchUiServiceContainer( *this, iGenSrchUi );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        iContainer->MakeVisible( ETrue );
        iContainer->UpdateNaviPaneL();
        
        // Draw again for can not refresh fully when back to home screen
		    iContainer->DrawDeferred();
        }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::DoDeactivate
// Called by the framework when the view is de-activated.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiSearchView::DoDeactivate()
    {
    if ( iContainer )
		    {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        iContainer->MakeVisible( EFalse );
		    delete iContainer;
		    iContainer = NULL;
		    }
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::DynInitMenuPaneL
// Called by the framework for dynamically initializing the menu pane.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiSearchView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {

    if (aResourceId == R_SEARCH_VIEW_MENU_PANE )                                    
        {
        TBool newState = ETrue;
        TInt idx = iContainer->CurrentItemIndex();

        RArray<TBool>& selectedContentTypes ( iGenSrchUi->SelectedContentTypes() );

        if ( idx == KAllInDeviceArrayIndex )
            {
            newState = !iGenSrchUi->AllContentClassesSelected();
            }
        else
            {
            TInt realIdx ( idx-1 );   // Because of "All in device"
            newState = !selectedContentTypes[realIdx];
            }
            
       if( newState == TRUE)
            {
        	aMenuPane->DeleteMenuItem( ESearchCmdUnMark );
            }
        else
            {
            aMenuPane->DeleteMenuItem( ESearchCmdMark );
            }
            
        if( iGenSrchUi->AiwFlag() == FALSE )
            {
            aMenuPane->DeleteMenuItem( ESearchCmdGoToMainView );
            }
        }
    }
        
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::CSrchUiSearchView
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
CSrchUiSearchView::CSrchUiSearchView()
:   iContainerListBoxIndex( KErrNotFound )
    {
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiSearchView::ConstructL(CSrchUiGeneral* aGenSrchUi)
    {
    iUpdataCba = ETrue;
    iCancel = EFalse;
    iGenSrchUi = aGenSrchUi;
    BaseConstructL( R_SEARCH_VIEW );
    }

// End of File
