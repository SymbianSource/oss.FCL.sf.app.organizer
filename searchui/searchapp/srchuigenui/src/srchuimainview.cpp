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
* Description:     This class is responsible for displaying all the available services on the device. It also contains a search field through which the search 
*
*/





//System Includes
#include <aknViewAppUi.h>
#include <avkon.rsg>
#include <aknenv.h>
#include <srchui.rsg>
#include <aknappui.h>

#include <aknnotewrappers.h>

//User Includes
#include "srchuimainview.h"
#include "srchuimaincontainer.h"
#include "srchuiappuids.h"
#include "srchui.hrh"

// ====================================== MEMBER FUNCTIONS =========================================

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::NewL
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
CSrchUiMainView* CSrchUiMainView::NewL(
                                     RPointerArray<TServiceInfo>& aServiceInfo,
                                     CSrchUiGeneral* aGenSrchUi)
    {
    CSrchUiMainView* self = CSrchUiMainView::NewLC(aServiceInfo,aGenSrchUi);
    CleanupStack::Pop( self );
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::NewLC
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
CSrchUiMainView* CSrchUiMainView::NewLC(RPointerArray<TServiceInfo>& aServiceInfo,
										CSrchUiGeneral* aGenSrchUi)
    {
    CSrchUiMainView* self = new ( ELeave ) CSrchUiMainView();
    CleanupStack::PushL( self );
    self->ConstructL(aServiceInfo,aGenSrchUi);
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::~CSrchUiServiceContainer
// Destructor.
// -------------------------------------------------------------------------------------------------
CSrchUiMainView::~CSrchUiMainView()
    {
    DoDeactivate();
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::SetCbaButtonsL
// Changes the CBA buttons.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainView::SetCbaButtonsL( TInt aResourceId )
    {
    if ( Cba() )
        {
        Cba()->SetCommandSetL( aResourceId );
        Cba()->DrawDeferred();
        }
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::SetContainerListBoxIndex
// Setter for iContainerListBoxIndex.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainView::SetContainerListBoxIndex( TInt aIndex )
    {
    iContainerListBoxIndex = aIndex;
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::ContainerListBoxIndex
// Getter for iContainerListBoxIndex.
// -------------------------------------------------------------------------------------------------
TInt CSrchUiMainView::ContainerListBoxIndex()    
    {
    return iContainerListBoxIndex;
    }


// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::Container
// returns a pointer to the container
// -------------------------------------------------------------------------------------------------
CCoeControl* CSrchUiMainView::Container()
    {
    return iContainer;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiMainView::GetData
// Getter for UI plug-ins info
// -------------------------------------------------------------------------------------------------
void CSrchUiMainView::GetData(RPointerArray<TServiceInfo>& aInfo)
    {
     	aInfo = iServiceInfo;
    
    }
 
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::Id
// Returns the unique id of the view.
// -------------------------------------------------------------------------------------------------
TUid CSrchUiMainView::Id() const
    {
    return KSearchViewUid;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::HandleCommandL
// Handles the commands that are passed from softkey generated commands and pop up's.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainView::HandleCommandL( TInt aCommand )
    {   
    TInt index = iContainer->CurrentItemIndex();

    switch ( aCommand )
        {
        case EEikCmdExit:
             iAvkonAppUi->ProcessCommandL(EAknCmdExit);
             break; 
        case EAknSoftkeyExit:
             iAvkonAppUi->RunAppShutter();
             break;
             
        case ESearchCmdSearch:
            {
            	iGenSrchUi->SetLastLaunchViewId( Id() );
                iContainer->DoSearchL();
            }
            break;

        case EAknSoftkeyBack:
             iAvkonAppUi->RunAppShutter();
             break;
        
        case ESearchCmdPreviousResults:
            {
            
            }
            break;

        case ESearchCmdOpen:
            {
	        if( index == 0)
	            {
	            TUid serviceid;
	            serviceid.iUid = 0;
							if(index  < iServiceInfo.Count())
							{
								serviceid = iServiceInfo[index]->iServiceId;
							}
	            iGenSrchUi->GetSupportedContentClassesL(serviceid);
	            iGenSrchUi->ConstructServiceSpecificMainViewL(serviceid);
	            }
            }
            break;
            
        case ESearchCmdSettings:
			{
			}
			break;
	    case ESearchCmdHelp:
            {
            }
            break;
        default:
			break;
        }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::DoActivateL
// Called by the framework when the view is activated.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
                                                                 const TDesC8& /*aCustomMessage*/ )
    {

    if ( !iContainer )
        {
        iContainer = new( ELeave )CSrchUiMainContainer( *this,iGenSrchUi );
		    iContainer->SetMopParent( this );
		    iContainer->ConstructL( ClientRect() );
		    AppUi()->AddToStackL( *this, iContainer );
		    iContainer->MakeVisible( ETrue );
        }
    else
        {
    	  iContainer->SetRect(ClientRect());
        }
    iContainer->UpdateNaviPaneL();
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::DoDeactivate
// Called by the framework when the view is de-activated.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainView::DoDeactivate()
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
void CSrchUiMainView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* /*aMenuPane*/ )
    {
	
		if (aResourceId == R_SEARCH_SERVICE_VIEW_MENU_PANE )                                    
        {
        }
    
    }   
// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::CSrchUiSearchView
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
CSrchUiMainView::CSrchUiMainView()
:   iContainerListBoxIndex( KErrNotFound )
    {
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiSearchView::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainView::ConstructL(RPointerArray<TServiceInfo>& aServiceInfo, 
								 CSrchUiGeneral* aGenSrchUi)
    {
    iServiceInfo = aServiceInfo;
    iGenSrchUi = aGenSrchUi;
    BaseConstructL( R_SEARCH_SERVICE_VIEW );
    }

// End of File
