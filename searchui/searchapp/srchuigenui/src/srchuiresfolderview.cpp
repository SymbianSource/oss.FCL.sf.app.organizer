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
* Description:     The grouped result view for all the searched contents. 
*
*/






// System Includes
#include <aknViewAppUi.h>
#include <eikbtgpc.h>
#include <eikmenup.h>
#include <e32std.h>
#include <AknGlobalNote.h>
#include <StringLoader.h>
#include <avkon.rsg>
#include <srchui.rsg>
#include "srchuifolderfileif.h"
#include <hlplch.h> // help launcher
#include <csxhelp/search.hlp.hrh>

// User Includes
#include "srchuiresfolder.h"
#include "srchuigeneral.h"
#include "srchuiresfolderview.h"
#include "srchuiresfoldercontainer.h"
#include "srchuiappuids.h"
#include "srchui.hrh"
#include "srchuidefines.h"

const TInt KNaviStringLength = 20;

// ====================================== MEMBER FUNCTIONS =========================================

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::NewL
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSrchUiResFolderView* CSrchUiResFolderView::NewL(CSrchUiGeneral* aGeneralsrchUi, 
												 RPointerArray<CSrchUiResFolder>& aFolders)
	{
    CSrchUiResFolderView* self = CSrchUiResFolderView::NewLC(aGeneralsrchUi,aFolders );
    CleanupStack::Pop(self);
    return self;
	}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::NewLC
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSrchUiResFolderView* CSrchUiResFolderView::NewLC(CSrchUiGeneral* aGeneralsrchUi, 
												  RPointerArray<CSrchUiResFolder>& aFolders)
	{
    CSrchUiResFolderView* self = new (ELeave) CSrchUiResFolderView();
    CleanupStack::PushL(self);
    self->ConstructL(aGeneralsrchUi, aFolders);
    return self;
	}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::~CSrchUiResFolderView
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CSrchUiResFolderView::~CSrchUiResFolderView()
    {
    DoDeactivate();
	delete iContainer;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::ResultsAddedL
// Informs the control(listbox) that search results were added
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderView::ResultsAddedL()
	{
	if ( iContainer )
	    {
        iContainer->ResultsAddedL();
	    }
	}
	
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::ResultsClearedL
// Called from the application ui class when the previous results were deleted.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderView::ResultsClearedL()
    {
    SetContainerListBoxIndex( KErrNotFound );
	if ( iContainer )
	    {
        iContainer->ResultsClearedL();
	    }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::SearchStatusChangedL
// Called from the application ui class after some search status change.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderView::SearchStatusChangedL()
    {
   	if ( iContainer )
	    {
        iContainer->SearchStatusChangedL();
	    }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::SearchFinishedL
// Called from the application ui class after the search has finished.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderView::SearchFinishedL(TInt aNumberOfFoundDocuments)
    {
	if ( Cba() )
		{
		Cba()->SetCommandSetL( R_SOFTKEYS_OPTIONS_OPEN_BACK );
		Cba()->DrawDeferred();
		}
   	iNumberOfFoundDocuments = aNumberOfFoundDocuments;
    if ( iContainer )
    	{
	    // Stop the timer if it is active
    	if ( iPeriodicTimer->IsActive() )
		    {
		    iPeriodicTimer->Cancel();
		    }
		    
    	HBufC* naviStrFormat;  	
    	TInt count = 0;
    	RArray<TBool>& seltypes = iGeneralsrchUi->SelectedContentTypes();
    	for(TInt i =0; i<seltypes.Count();i++)
    		{
    	    if(seltypes[i])
    	    	{
    	        count++;
    	    	}
    		}
    	if((aNumberOfFoundDocuments == 0)&& (count == 1))	
    		{
			 	naviStrFormat = KNullDesC().Alloc();
	    		iContainer->SearchFinishedL();
	    		iContainer->SetErrorNoteL();
	    		iContainer->UpdateNaviPaneL(*naviStrFormat);
     		}
    	else
    		{
    		if(aNumberOfFoundDocuments == 1)
	    		{
					//load (%d result) from resource
					naviStrFormat = StringLoader::LoadL(R_QTN_SEARCH_ONE_RESULT);
	    		}
    		else
		    	{
			    	//load (%d results) from resource
			    	naviStrFormat = StringLoader::LoadL( R_QTN_SEARCH_MULTIPLE_RESULTS, 
			    	aNumberOfFoundDocuments );
		    	}
    	  

        iContainer->UpdateNaviPaneL(*naviStrFormat );
	    	//write code to display number of results in the Navi pane
        iContainer->SearchFinishedL();
			  delete naviStrFormat;
	  	  }
	    }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::GetOpenFolder
// Returns the pointer to opened folder
// -------------------------------------------------------------------------------------------------
//
MSrchUiFolder* CSrchUiResFolderView::GetOpenFolder()
    {
    return iContainer->GetOpenFolder();
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::SetContainerListBoxIndex
// Setter for iContainerListBoxIndex.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderView::SetContainerListBoxIndex( TInt aIndex )
    {
    iContainerListBoxIndex = aIndex;
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::ContainerListBoxIndex
// Getter for iContainerListBoxIndex.
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiResFolderView::ContainerListBoxIndex()    
    {
    return iContainerListBoxIndex;
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolderView::HandleItemAdditionL
// Handles the addition of an item to the model.
// -----------------------------------------------------------------------------
//
void CSrchUiResFolderView::HandleItemAdditionL()
{
    if(iContainer)
    {
        iContainer->HandleItemAdditionL();
    }
}
   
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::UpdateNaviPaneL
// Updates the navigation Pane.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderView::UpdateNaviPaneL()
    {
        if(iGeneralsrchUi->IsSearchOngoing())
        {
        	SetNaviTextL();
        }
        else
        {
            HBufC* naviStrFormat;  
        	if(iNumberOfFoundDocuments == 1)
    		{
			 //load (%d result) from resource
			 naviStrFormat = StringLoader::LoadL(R_QTN_SEARCH_ONE_RESULT);
    		}
    		else
	    	{
	    	 //load (%d results) from resource
	    	 naviStrFormat = StringLoader::LoadL( R_QTN_SEARCH_MULTIPLE_RESULTS, 
	    	 	iNumberOfFoundDocuments );
	    	}

        	iContainer->UpdateNaviPaneL( *naviStrFormat );
        	delete naviStrFormat;
        }
    }
 // -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::Id
// Returns the unique id of the view.
// -------------------------------------------------------------------------------------------------
//
TUid CSrchUiResFolderView::Id() const
    {
    return KSearchResultFolderViewUid;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::HandleCommandL
// Handles the commands that are passed from softkey generated commands and
// pop up's.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case ESearchCmdOpen:
			{
				iContainer->HandleItemSelectionL();
			}
			break;	

        case ESearchCmdSearch:
        	{
        	_LIT(KEmptyString,"");
	        TBufC<3> buf( KEmptyString );
	        HBufC* string = buf.AllocL();
            CleanupStack::PushL(string);
            iGeneralsrchUi->SetSearchStringL( *string );
            CleanupStack::PopAndDestroy(string);
    		
        	iGeneralsrchUi->StartNewSearchL();
        	}
        	break;
        
        case ESearchCmdHelp:
        	{
        	TCoeHelpContext helpContext;
            helpContext.iMajor = KSearchAppUid ;
            helpContext.iContext = KSEARCH_HLP_GROUPED; // add correct help context when available
            
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
             iAvkonAppUi->ProcessCommandL(EAknCmdExit);
             break;
             
        case EAknSoftkeyBack:
            {
	        iGeneralsrchUi->ActivatePreviousViewL();
            }
            break;
        	case ESearchCmdCancel:
        	   	// Stop the timer if it is active
    	    if ( iPeriodicTimer->IsActive() )
			    {
			    iPeriodicTimer->Cancel();
			    }
        	iGeneralsrchUi->CancelSearch();
        	if ( Cba() )
        	{
        		Cba()->SetCommandSetL( R_SOFTKEYS_OPTIONS_OPEN_BACK );
        		Cba()->DrawDeferred();
        	}
        	break;
		default:
			break;
        }
    
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::DoActivateL
// Called by the framework when the view is activated.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    if ( !iContainer )
        {
		iContainer = new( ELeave ) CSrchUiResFolderContainer( *this, iGeneralsrchUi );
        iContainer->SetMopParent( this );
		iContainer->ConstructL( ClientRect() );
    	iContainer->SetRoot( iGeneralsrchUi->SrchUiResRoot() );
        
        AppUi()->AddToStackL( *this, iContainer );
        iContainer->MakeVisible( ETrue );
        
        if ( Cba() )
        	{
        	if(iGeneralsrchUi->IsSearchOngoing())
        		{
        		Cba()->SetCommandSetL( R_SOFTKEYS_OPTIONS_OPEN_CANCEL );	
        		}
        	else
        		{
        		Cba()->SetCommandSetL( R_SOFTKEYS_OPTIONS_OPEN_BACK );
        		}
        	Cba()->DrawDeferred();
        	}
        }
    iContainer->UpdateTitlePaneL();
    iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    if(iGeneralsrchUi->IsSearchOngoing())
        {
        StartTimer();    
        }
    UpdateNaviPaneL();
    }
 
// -------------------------------------------------------------------------------------------------
// CSrchUiResView::DoDeactivate
// Called by the framework when the view is de-activated.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderView::DoDeactivate()
    {
    if ( iContainer )
		{
        AppUi()->RemoveFromViewStack( *this, iContainer );
        iContainer->MakeVisible( EFalse );
		delete iContainer;
		iContainer = NULL;
		}
	if( iPeriodicTimer ) 
		{
		delete iPeriodicTimer;
		iPeriodicTimer = NULL;
		}
	}
	
// -------------------------------------------------------------------------------------------------
// CSrchUiResView::DynInitMenuPaneL
// Called by the framework for dynamically initializing the menu pane.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    if(!iContainer->IsElementValid())
        {
        if ( aResourceId == R_SEARCH_RESULT_FOLDER_VIEW_MENU_PANE )
            {
        	aMenuPane->DeleteMenuItem( ESearchCmdOpen );
            }
        }
    }
           
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::CSrchUiResFolderView
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
CSrchUiResFolderView::CSrchUiResFolderView()
	{
	}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderView::ConstructL(CSrchUiGeneral* aGeneralsrchUi, 
									  RPointerArray<CSrchUiResFolder>& aFolders)
    {
    iGeneralsrchUi = aGeneralsrchUi;
    iFolders = aFolders;
    iCount_val = 1;
    BaseConstructL( R_SEARCH_RESULT_FOLDER_VIEW );
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolderView::PeriodL
// This function is called by the periodic timer.
// -----------------------------------------------------------------------------
//
TInt CSrchUiResFolderView::PeriodL( TAny* aPtr )
	{
  ( static_cast<CSrchUiResFolderView*>( aPtr ) )->SetNaviTextL();
    // Returning a value of TRUE indicates the callback should be done again
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolderView::SetNaviTextL
// Updates the navigation with the text about the staus.
// -----------------------------------------------------------------------------
//
void CSrchUiResFolderView::SetNaviTextL()
{
	HBufC16 *pbuf;	
	
	pbuf = StringLoader::LoadL(R_QTN_SEARCH_INPROGRESS);
	
	HBufC16* tmpbuf = HBufC::NewL( pbuf->Size() + 3 );
	TPtr bufPtr = tmpbuf->Des();
	bufPtr.Copy( *pbuf );
	
	for ( TInt i = 0; i <= iCount_val % 3; i++ ) 
			{
			_LIT ( KDot, "." );
			bufPtr.Append( KDot ); 		
			}
	
    iCount_val++;
    iContainer->UpdateNaviPaneL(*tmpbuf );
    delete pbuf;
    delete tmpbuf;
}
    
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::StartTimer
// Starts the timer.
// -------------------------------------------------------------------------------------------------
//    
void CSrchUiResFolderView::StartTimer()
	{
	// If the timer is not already running, start it
	if ( !iPeriodicTimer->IsActive() )
		{
		iPeriodicTimer->Start( 10, 1000000, 
                               TCallBack( CSrchUiResFolderView::PeriodL, this ) );
		}

	}
    
/// End of File
