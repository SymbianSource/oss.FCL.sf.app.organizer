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
* Description:     This class contains all the controls needed to display the heavy results. 
*
*/






// System Includes
#include <eikclbd.h>
#include <akntabgrp.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <srchuiresultview.rsg>
#include <gulicon.h>
#include <aknnavilabel.h> 
#include <searchresult.h>
#include <searchlightresult.h>
#include <AknIconArray.h> 
#include <searchiconinfo.h>
#include <searchdocumentid.h>
#include <data_caging_path_literals.hrh>
#include <akntitle.h>
#include <utf.h>
#include <srchuiresviewpluginicons.mbg>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK
// User Includes
#include "srchuiresultcontainer.h"
#include "srchuiresultview.h"
#include "srchuiresult.hrh"
#include "srchuiresultmodel.h"
#include "srchuiresfolder.h"
#include "srchuiresultviewdefines.h"

enum TSearchPanics
    {
    ESearchPanicUnknownContentType = 1,
    ESearchPanicProgramError
    };
    
const TInt KTitleLength = 50;

TInt const KBufferLength = 100;
// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::CSearchResultFolderContainer
// Performs the first phase of two phase construction.
// ----------------------------------------------------------------------------
//
CSrchUiResultContainer::CSrchUiResultContainer( CSrchUiResultView& aView )
:	iView(aView),iDoubleLineListBox(NULL), iSelectedFolderIdx(0) , iTouchFlag(EFalse)
	{
	}

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::ConstructL
// Performs the second phase construction.
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::ConstructL( const TRect& aRect,
              TInt aSelectedFolderIdx, CSrchUiResFolder* aSrchUiResRootFolder)
    {
    
    iIcons = NULL;
    
    iSearchResultsTitle = HBufC::NewL( KMaxCharsForNaviPaneText );
    TPtr des= iSearchResultsTitle->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8(des,iView.GetQueryString());

	iModel = CSrchUiResultModel::NewL();
	iModel->SetOpenedFolder( iOpenedFolder );

    CreateWindowL();
	CEikStatusPane* statusPane = STATIC_CAST( CAknAppUi*,
	                                    iEikonEnv->EikAppUi() )->StatusPane();
	
	CCoeControl* control = ( statusPane->ControlL(
	                                   TUid::Uid( EEikStatusPaneUidNavi ) ) );
	iNaviPane = STATIC_CAST( CAknNavigationControlContainer*, control );
	
	iSelectedFolderIdx = aSelectedFolderIdx;
	iSrchUiResRootFolder = aSrchUiResRootFolder;

        SetupListBoxL();    
	
	if ( iModel->MdcaCount() )
	    {
        SetCurrentItemIndex( 0 );
	    }
	    
    UpdateTitlePaneL();
	UpdateNaviPaneL();
	
	SetRect( aRect );
	
    // Restore previous listbox item index
    TInt prevIdx ( iView.ContainerListBoxIndex() );
        
    if ( prevIdx >= 0 && prevIdx < iModel->MdcaCount() )
        {
        iDoubleLineListBox->SetCurrentItemIndex( prevIdx );
        }
    
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::UpdateView
// 
// ----------------------------------------------------------------------------
void CSrchUiResultContainer::UpdateViewL(CSrchUiResFolder* aSrchUiResRootFolder, TBool aSearchFinished, TBool aResultRetrieve )
{
    	iModel->SetOpenedFolder( iOpenedFolder );

    
	iSrchUiResRootFolder = aSrchUiResRootFolder;
	
	iSearchFinished = aSearchFinished;

    if((iOpenedFolder->GetHeavyResultsCount() == 0) && (aSearchFinished) && aResultRetrieve )
    	{
    	//Search finished and No results are there. 
        SetErrorNoteLL( ETrue, EFalse );        
    	}
    else if ( iOpenedFolder->GetHeavyResultsCount() == 0 && !aSearchFinished ) 
    	{
    	//Still Searching and No results are there. 
    	SetErrorNoteLL( EFalse, ETrue );
    	}
    else if ( aSearchFinished && !aResultRetrieve ) 
    	{
    	//If results are not yet retrieved and search is finished
    	SetErrorNoteLL( ETrue, ETrue );
    	}
    else if ( iOpenedFolder->GetHeavyResultsCount() )
    	{
        SetupListBoxL();    
    	}
	
	
	if ( iModel->MdcaCount() )
	    {
        SetCurrentItemIndex( 0 );
	    }
	    
    UpdateTitlePaneL();
	
    // Restore previous listbox item index
    TInt prevIdx ( iView.ContainerListBoxIndex() );
        
    if ( prevIdx >= 0 && prevIdx < iModel->MdcaCount() )
        {
        iDoubleLineListBox->SetCurrentItemIndex( prevIdx );
        }
    ResultsAddedL();
    
}
// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::~CSearchResultFolderContainer
// Destructor.
// ----------------------------------------------------------------------------
//
CSrchUiResultContainer::~CSrchUiResultContainer()
    {
    // Store listboxes current index so that it can be
    // restored to the same place if needed
    TInt idx = 0;
    if ( iIcons ) 
    	{
	    iIcons->ResetAndDestroy();
	    iIcons = NULL;
    	}
    if(iDoubleLineListBox)
    {
        idx = iDoubleLineListBox->CurrentItemIndex();    
    }
    iView.SetContainerListBoxIndex( idx );
    
	if(iDoubleLineListBox)
	{
		delete iDoubleLineListBox;
	}
	if(iModel)
	{
		delete iModel;
	}
	if(iSearchResultsTitle)
	{
		delete iSearchResultsTitle;
	}
	if( iNaviDecorator1 ) 
    {
       	delete iNaviDecorator1;
       	iNaviDecorator1 = NULL;
    }
    if( iNaviDecorator2 ) 
    {
       	delete iNaviDecorator2;
       	iNaviDecorator2 = NULL;
    }
    
    if( iNaviDecorator3 ) 
    {
       	delete iNaviDecorator3;
       	iNaviDecorator3 = NULL;
    }
    
    iTabGroup = NULL;
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::HandleItemSelectionL
// Handles the selection of the currently active item.
// ----------------------------------------------------------------------------
//
const CSearchDocumentId& CSrchUiResultContainer::HandleItemSelectionL()
    {
    TInt currentItemIdx = CurrentItemIndex();
    CSearchResult* searchResult = NULL;
    if ( currentItemIdx >= 0 )
        {
        searchResult = iOpenedFolder->GetHeavyResult( currentItemIdx );			
        HandleItemAdditionL();
        }
    return searchResult->Result().DocumentId();
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::GetItemIndex
// Folders and Files are stored into 1 array for use with the listbox.
// First the folders are listed and then files.
//  The function returns a folder or file index.
// To know if the selected item was a folder or file the GetListBoxType method
// can be used.
// ----------------------------------------------------------------------------
//
TInt CSrchUiResultContainer::GetItemIndex( TInt aIndex )
	{
    return aIndex - iOpenedFolder->GetSubFoldersCount();
	}

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::GetOpenFolder
// Getter for iOpenFolder.
// ----------------------------------------------------------------------------
//
MSrchUiFolder* CSrchUiResultContainer::GetOpenFolder() 
    {
    return iOpenedFolder;
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::GetItemIndex
// Handles changes to the title pane text.
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::UpdateTitlePaneL()
	{
	CSrchUiResFolder* parent = iSrchUiResRootFolder;
    if ( parent )   // inside category results->use 
        {
        TBuf<KTitleLength> titleText;
        _LIT( KtitleFormat, "\"%S\"" );
        titleText.Format( KtitleFormat, iSearchResultsTitle );
	
	    const CEikStatusPane* sp = 
	                      CEikonEnv::Static()->AppUiFactory()->StatusPane();
	    CAknTitlePane* titlePane = STATIC_CAST(CAknTitlePane*, 
						sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

        // different
        titlePane->SetTextL( titleText );

		_LIT( KResMifFileName, "srchuiresviewpluginicons.mif" );  
	    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
		TFileName mifFileName( driveLetter );
		mifFileName.Append( KDC_APP_BITMAP_DIR );
		mifFileName.Append( KResMifFileName );        
		
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		CFbsBitmap* bitmap = NULL;
		CFbsBitmap* mask	= NULL;
		

        AknsUtils::CreateIconL( skin, KAknsIIDQgnPropSearchIndeviceTitle,
                bitmap, mask, mifFileName,
                EMbmSrchuiresviewpluginiconsQgn_prop_search_indevice_title,
                EMbmSrchuiresviewpluginiconsQgn_prop_search_indevice_title_mask );

        titlePane->SetSmallPicture( bitmap, mask, ETrue );
        }
	}

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::UpdateNaviPaneL
// Handles the changes needed to the Navigation Pane.
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::UpdateNaviPaneL()
	{	
    CSrchUiResFolder* parent = iSrchUiResRootFolder;
    if ( parent )	// All siblings and myself are placed in the TAB
        {
        if( iNaviDecorator1 ) 
        {
        	delete iNaviDecorator1;
        	iNaviDecorator1 = NULL;
        }
        iNaviDecorator1 = iNaviPane->CreateTabGroupL();

        iTabGroup = STATIC_CAST( CAknTabGroup*,
                                iNaviDecorator1->DecoratedControl() );
        iTabGroup->SetTabFixedWidthL( KTabWidthWithOneTab );
        
        //ADD TAB GROUPS
        TInt currentGroupIdx = iSelectedFolderIdx;

        //currentGroupIdx = parent->GetSubFolderIndex( iOpenedFolder );
		for( TInt i = 0; i < parent->GetSubFoldersCount(); i++)
			{
			iTabGroup->AddTabL( i, parent->GetSubFolder(i)->GetFolderName() );
			}

    	if ( currentGroupIdx >= 0 && currentGroupIdx < iTabGroup->TabCount() )
    		{
    		iTabGroup->SetActiveTabByIndex( currentGroupIdx );
    		}
    	else
    		{
    		iTabGroup->SetActiveTabByIndex( 0 );
    		}

    	
    	}

    else	// root folder, show status of search
       	{

       	}
	}



// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::UpdateNaviPaneL
// Handles the changes needed to the Navigation Pane.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResultContainer::UpdateNaviPaneL( const TDesC& aBuf )
		{
        
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* np= 
			(CAknNavigationControlContainer *)sp->ControlL( TUid::Uid( 
					EEikStatusPaneUidNavi ) ); 
	CSrchUiResFolder* parent = iSrchUiResRootFolder;
    //in buf is the text you want display
    if ( iNaviDecorator2 ) 
        {
        delete iNaviDecorator2;
        iNaviDecorator2 = NULL;
        }
        iNaviDecorator2 = np->CreateNavigationLabelL( aBuf );
        np->PushL( *iNaviDecorator2 );
        iNaviDecorator2->SetControlType( CAknNavigationDecorator::ENaviLabel );
 
        
        CAknNaviLabel* naviLabel = ( CAknNaviLabel* )
        		iNaviDecorator2->DecoratedControl();
        naviLabel->SetTextL( aBuf );
   
        iNaviPane->DrawNow();
        naviLabel->DrawNow();
      	}

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::GoUpOneLevelL
// Will go 1 step higher in the directory structure.
// ----------------------------------------------------------------------------
//
TBool CSrchUiResultContainer::GoUpOneLevelL() 
	{
	//MSrchUiFolder* parent = iOpenedFolder->GetParentFolder();
	CSrchUiResFolder* parent = iSrchUiResRootFolder;
    if ( parent )
    	{
    	iOpenedFolder = STATIC_CAST( CSrchUiResFolder*, parent );
    	iModel->SetOpenedFolder( iOpenedFolder );
    	
    	TInt groupIndex = iTabGroup->ActiveTabIndex(); 

        SetupListBoxL();


		SetCurrentItemIndexAndDrawL( groupIndex );

		return ETrue;
    	}
	else
		{
		return EFalse;	// Was at Root level
		}
	}

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::IsEmpty
// ETrue when the directory structure is
// empty (no folders and no files), EFalse otherwise.
// ----------------------------------------------------------------------------
//
TBool CSrchUiResultContainer::IsEmpty()
	{
	return ( iModel->MdcaCount() == 0 );
	}

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::SetRoot
// Sets the root folder for this directory structure.
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::SetRoot( MSrchUiFolder* aFolder )
	{
	iOpenedFolder = STATIC_CAST ( CSrchUiResFolder* , aFolder );
	}

// ----------------------------------------------------------------------------
// CSrchUiResultContainer::ResultsAddedL
// Informs the control(listbox) that search results were added
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::ResultsAddedL()
	{
	MSrchUiFolder* parent = iOpenedFolder->GetParentFolder();
    if ( parent )  // inside category-> counter needs update
        {
       	UpdateTitlePaneL();
        }
    HandleItemAdditionL();
	}

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::ResultsClearedL
// Informs the control(listbox) that the previous results were deleted.
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::ResultsClearedL()
	{
    SetRoot( iView.OpenFolder() );
	iModel->SetOpenedFolder( iOpenedFolder );
	
    SetCurrentItemIndex( 0 );

    HandleItemAdditionL();

	UpdateTitlePaneL();
	}

// ----------------------------------------------------------------------------
// CSrchUiResultContainer::SearchStatusChangedL
// Called from the application ui class after some search status change
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::SearchStatusChangedL()
    {
	MSrchUiFolder* parent = iOpenedFolder->GetParentFolder();
    if ( !parent )  // root level
        {
        UpdateNaviPaneL();
        }
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::SearchFinishedL
// Informs the control(listbox) that one of the searches has finished.
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::SearchFinishedL()
    {
    if( iDoubleLineListBox )
        {
        iDoubleLineListBox->HandleItemAdditionL();
        }
    else
        {
        User::Panic( KSearchResultFolderContainer, ESearchPanicProgramError );
        }
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::HandleItemAdditionL
// Calls the HandleItemAdditionL method on the correct
// listbox (single or double line).
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::HandleItemAdditionL()
    {
    if( iDoubleLineListBox )
        {
        iDoubleLineListBox->HandleItemAdditionL();
        }
    else
        {
        User::Panic( KSearchResultFolderContainer, ESearchPanicProgramError );
        }
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::CurrentItemIndex
// Asks the item index from the correct listbox (single or double line).
// ----------------------------------------------------------------------------
//
TInt CSrchUiResultContainer::CurrentItemIndex()
    {
    if( iDoubleLineListBox )
        {
        return iDoubleLineListBox->CurrentItemIndex();
        }
    
    // The program may never come here    
    User::Panic( KSearchResultFolderContainer, ESearchPanicProgramError );
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::SetCurrentItemIndex
// Calls the SetCurrentItemIndex method on the correct
// listbox (single or double line).
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::SetCurrentItemIndex( TInt aIndex )
    {
    if( iDoubleLineListBox )
        {
        iDoubleLineListBox->SetCurrentItemIndex( aIndex );
        }
    else
        {
        User::Panic( KSearchResultFolderContainer, ESearchPanicProgramError );
        }
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::SetCurrentItemIndexAndDraw
// Calls the SetCurrentItemIndexAndDraw method
//on the correct listbox (single or double line).
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::SetCurrentItemIndexAndDrawL( TInt aIndex )
    {
    if( iDoubleLineListBox )
        {
        iDoubleLineListBox->HandleItemAdditionL();
        iDoubleLineListBox->SetCurrentItemIndexAndDraw( aIndex );
        }
    else
        {
        User::Panic( KSearchResultFolderContainer, ESearchPanicProgramError );
        }
    }

// ----------------------------------------------------------------------------
// CSearchAppUi::ListBoxIconsArrayL
// Returns an array of icons for use in the listboxes.
// ----------------------------------------------------------------------------
//
CArrayPtr<CGulIcon>* CSrchUiResultContainer::ListBoxIconsArrayL(
                                                  CSrchUiResFolder* folders)
	{
    TInt count = folders->HeavyResults().Count();
    if( 0 < count ) 
    	{
    	RPointerArray<CSearchResult>& results = folders->HeavyResults();
    	CArrayPtr<CGulIcon>* icons = new( ELeave ) CAknIconArray( count );
    	CleanupStack::PushL( icons );
    	//Dont reset this(subfolders) 
     	RPointerArray<CSrchUiResFolder> subfolders;
     	folders->SubFolders( subfolders );
     	
    	for ( TInt i = 0; i < count; i++ )
        	{
	        //if( icon.GetIconFile().Find( _L(".mbm") ) != KErrNotFound )
	        
	        TUid uid1 ( results[i]->Result().ContentFolderId( ) );
	        CSrchUiResFolder* currentFolder = NULL;
	        
	        const CSearchIconInfo* icon = NULL;
	        
	        if ( 0 < subfolders.Count() ) 
	        	{
		        	for ( TInt i = 0; i < subfolders.Count(); i++ ) 
		        	{
		        	currentFolder = subfolders[i];
		        	if ( uid1.iUid == currentFolder->ContentFolderType().iUid ) 
		        		{
		        		icon = currentFolder->IconInfo();
		        		break;
		        		}
		        	}
	        	}
	        else 
	        	{
	        	icon = (folders->IconInfo());	
	        	}
	       
	        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
			CFbsBitmap* bitmap = NULL;
			CFbsBitmap* mask	= NULL;

			//Ext Icon Skinning implementation for Display
		    AknsUtils::CreateIconL(skin,KAknsIIDQgnIndiCalcButtonPlus,
			    bitmap,mask,icon->GetIconFile(),
			    icon->GetIconIndex(), icon->GetIconMaskIndex());

			CGulIcon* plusIcon = CGulIcon::NewL(bitmap,mask);
			CleanupStack::PushL(plusIcon);		
		    icons->AppendL(plusIcon);
			CleanupStack::Pop(plusIcon);
	        }
	        
	    subfolders.Reset();  
	    _LIT( KResMifFileName, "srchuiresviewpluginicons.mif" );  
	    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
		TFileName mifFileName( driveLetter );
		mifFileName.Append( KDC_APP_BITMAP_DIR );
		mifFileName.Append( KResMifFileName );
	    
	    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		CFbsBitmap* bitmap = NULL;
		CFbsBitmap* mask	= NULL;
		
		AknsUtils::CreateIconL( skin,KAknsIIDQgnIndiCalcButtonPlus,
			    bitmap, mask, mifFileName,
			    EMbmSrchuiresviewpluginiconsQgn_indi_mmc_add,
			    EMbmSrchuiresviewpluginiconsQgn_indi_mmc_add_mask );
			    
		CGulIcon* plusIcon = CGulIcon::NewL( bitmap,mask );
		CleanupStack::PushL( plusIcon );		
		icons->AppendL( plusIcon );
		CleanupStack::Pop( plusIcon );
	    
		CleanupStack::Pop( icons ); // icons
	    return icons;
	    }
	return NULL;
	}

// ----------------------------------------------------------------------------
// CSearchAppUi::IsElementValid
// checks whether there are any elements in the current folder selected
// ----------------------------------------------------------------------------
//
TBool CSrchUiResultContainer::IsElementValid()
{
	return iOpenedFolder->GetHeavyResultsCount();
}

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::OfferKeyEventL
// Called by the framework when a key event
// needs to be processed.
// ----------------------------------------------------------------------------
//
TKeyResponse CSrchUiResultContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    if (aType != EEventKey) return EKeyWasNotConsumed;

	TKeyResponse response(EKeyWasNotConsumed);

	switch (aKeyEvent.iCode)
        {
        case EKeyLeftArrow:
        case EKeyRightArrow:
            if(iTabGroup == NULL)
            {
                break;
            }
            else
			{
			if(aKeyEvent.iCode == EKeyRightArrow)
			{
			    if ( iSelectedFolderIdx < iSrchUiResRootFolder->GetSubFoldersCount() - 1 ) 
			    	{
			    	iSelectedFolderIdx += 1;
			    	iView.IncrementFolderIndex();	
			    	}
			    
				}
			else
				{
				if ( iSelectedFolderIdx > 0 ) 
					{
					iSelectedFolderIdx -=1;
			    	iView.DecrementFolderIndex();	
					}
			}
        	CSrchUiResFolder* parent = iSrchUiResRootFolder;
            if ( parent )
                {
    			response = iTabGroup->OfferKeyEventL(aKeyEvent, aType);
            	if ( response == EKeyWasConsumed )
    	        	{
    	        	CSrchUiResFolder* parent = iSrchUiResRootFolder;
    	        	if ( parent)
    	        		{
       					iOpenedFolder = STATIC_CAST ( CSrchUiResFolder* , 
       					 parent->GetSubFolder( iTabGroup->ActiveTabIndex() ) );
       					
       					
       					iModel->SetOpenedFolder( iOpenedFolder );
                        
                        
                        HandleItemAdditionL();
                        TInt resCount = iOpenedFolder->GetHeavyResultsCount();
                        //Upate the command button
                        iView.UpdateCommandButtonsL( resCount );
                        if( resCount > 0 )
                        	{
	                        SetupListBoxL();
	                        SetCurrentItemIndexAndDrawL( 0 );    
                        	}
                        else
	                        {
	                        SetErrorNoteLL( iSearchFinished, EFalse );
	                        }
    	        		}
    	        	}
                }
            else    // root level
                {
                // do nothing
                }
			}
			break;

        case EKeyDevice3:   // Rocker push
            {
            //HandleItemSelectionL();
            if(CurrentItemIndex()>=0)
	            {
	            iView.HandleSelectionL(HandleItemSelectionL());
	            // dont use the MEikListBoxObserver and HandleListBoxEventL because 
	            // when switching between single line and double line
	            // listboxes, things get messed up
	            // ex. Rocker pushed
	            //
	            // CCoeControl::OfferKeyEventL Called
	            //
	            // iSingleLineListBox->OfferKeyEventL( aKeyEvent, aType ) called
	            // from here causes
	            // MEikListBoxObserver::HandleListBoxEventL to be called
	            //
	            // HandleItemSelectionL called from within
	            // MEikListBoxObserver::HandleListBoxEventL
	            // this causes the entering of the directory
	            // structure and this uses double
	            // line listbox, thus single line list box is deleted and
	            // MEikListBoxObserver::HandleListBoxEventL is called via this
	            // single line listbox
	            //  object ->Panics
	            }
            }
            break;
         
         default:
            if ( iDoubleLineListBox )
            {
                response =  iDoubleLineListBox->OfferKeyEventL( aKeyEvent, aType );
            }
			break;
        }

    if ( response != EKeyWasNotConsumed)
        {
        UpdateTitlePaneL();
        
        }
    UpdateNaviResultPaneL();
	return response;
	}


// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::HandlePointerEventL
// Handles pointer events from framework.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResultContainer::HandlePointerEventL(const TPointerEvent &aPointerEvent)
	{
	// Check if touch is enabled or not
    if( !AknLayoutUtils::PenEnabled() )
        {
        return;
        }
    
    TInt index = iDoubleLineListBox->CurrentItemIndex();
    if(index < 0)
    	{
    		return;
    	}
    
    TInt indexPos = KErrNotFound; 
    iDoubleLineListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition,indexPos);
    if( index != KErrNotFound && indexPos != KErrNotFound )
        {
        switch( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            {
                iDoubleLineListBox->HandlePointerEventL(aPointerEvent);      
                if( iDoubleLineListBox->CurrentItemIndex() == index)
                {
                iTouchFlag = ETrue;
                
#ifdef RD_TACTILE_FEEDBACK 
                MTouchFeedback* feedback = MTouchFeedback::Instance();
                if ( feedback )
                    {
                    feedback->InstantFeedback( this, ETouchFeedbackBasic );
                    }
#endif //RD_TACTILE_FEEDBACK

                }
                else
                {
                iTouchFlag = EFalse;      
                }
            }
            break;
        case TPointerEvent::EButton1Up:
            {
            //PointUp event should be handled by listbox.
            iDoubleLineListBox->HandlePointerEventL(aPointerEvent);
            
               if( iTouchFlag )
                {
                iView.HandleSelectionL(HandleItemSelectionL());
                }
            }
            break;
        case TPointerEvent::EDrag:
            {
                iDoubleLineListBox->HandlePointerEventL(aPointerEvent);
                if( iTouchFlag  &&  iDoubleLineListBox->CurrentItemIndex() != index )
                {
                  iTouchFlag = EFalse;  
                }
            }
            break;
        default:
            break;
        }
        }
      else
        {
           iDoubleLineListBox->HandlePointerEventL(aPointerEvent);
            if( !iDoubleLineListBox->Model()->ItemTextArray()->MdcaCount() )
            {
               iView.HandleSelectionL(HandleItemSelectionL()); 
            }  
        }
    // The navi pane should be updated after changing the focus.  
    UpdateNaviResultPaneL();    
    }


// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::SizeChanged
// Called by the framework when the view size is changed.
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::SizeChanged()
    {
    iDoubleLineListBox->SetRect( Rect() );
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::CountComponentControls
// Gets the number of controls contained in a compound control.
// ----------------------------------------------------------------------------
//
TInt CSrchUiResultContainer::CountComponentControls() const
    {
    if ( iDoubleLineListBox )
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::ComponentControl
// Gets the specified component of a compound control.
// ----------------------------------------------------------------------------
//
CCoeControl* CSrchUiResultContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
	        {
	        if ( iDoubleLineListBox )
	            {
	            return iDoubleLineListBox;
	            }
	        }
        break;

        default:
        return NULL;
        }
    return NULL;    // to avoid warning
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::HandleResourceChange
// Handles a change to the application's resources
// which are shared across the environment.
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::HandleResourceChange( TInt aType )
    {
    TRAPD ( ignore, HandleResourceChangeL( aType ) );
    }

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::HandleResourceChangeL
// Handles a change to the application's resources which are
// shared across the environment.
// ----------------------------------------------------------------------------
//

void CSrchUiResultContainer::HandleResourceChangeL( TInt aType )
	{
	CCoeControl::HandleResourceChange( aType );
	// Event generated by the FW when the screen size changed/flipped.
	if ( aType == KEikDynamicLayoutVariantSwitch )
	    {
        TRect rect;
		AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        SetRect( rect );
	    }
	UpdateNaviResultPaneL(); 
	}

// ----------------------------------------------------------------------------
// CSearchResultFolderContainer::SetupListBoxL
// Constructs and prepares the list box control.
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::SetupListBoxL()
	{ 
    
    if ( iIcons ) 
    	{
    	iIcons->ResetAndDestroy();
    	iIcons = NULL;
    	}
    
    
    delete iDoubleLineListBox;
    iDoubleLineListBox = NULL;

	    
	iDoubleLineListBox = new (ELeave) CAknDoubleGraphicStyleListBox;
	iDoubleLineListBox->SetContainerWindowL(*this);
	iDoubleLineListBox->ConstructL( this, EAknListBoxSelectionList );

    iDoubleLineListBox->SetRect( Rect() );
    iDoubleLineListBox->ActivateL();
    
    //iDoubleLineListBox->CreateScrollBarFrameL(ETrue,EFalse,EFalse);
    iDoubleLineListBox->CreateScrollBarFrameL( ETrue );// change
    
    iDoubleLineListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
                        CEikScrollBarFrame::EOn, CEikScrollBarFrame::EOn);

	iDoubleLineListBox->Model()->SetItemTextArray( iModel );
	iDoubleLineListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // passing ownership of icons
    iIcons = ListBoxIconsArrayL( iOpenedFolder );
	iDoubleLineListBox->ItemDrawer()->ColumnData()->SetIconArray( 
	                            iIcons ); 
            	                                                    
	    
	}


// ----------------------------------------------------------------------------
// CSrchUiResFolderContainer::UpdateNaviResultPaneL
// Handles the changes needed to the Navigation Pane.
// ----------------------------------------------------------------------------
//
void CSrchUiResultContainer::UpdateNaviResultPaneL()
	{
        
        CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
        CAknNavigationControlContainer* np= 
			(CAknNavigationControlContainer *)sp->ControlL(
			                            TUid::Uid(EEikStatusPaneUidNavi));

	    HBufC* buffer = HBufC::NewLC( KBufferLength );
        //in buf is the text you want display
        if(iOpenedFolder->GetHeavyResultsCount() == 0)
        {
            _LIT(kNaviPaneFormat,"<%S>");   
            HBufC* folderName = iOpenedFolder->GetFolderName().AllocL(); 
            buffer->Des().Format(kNaviPaneFormat, folderName ); 
            delete folderName;
            folderName = NULL;       
        }
        else
        {
        	RPointerArray<CSearchLightResult> results;
        	results = iOpenedFolder->Results();
        	TUid folderId;
        	TInt currentItemIdx = CurrentItemIndex();
        	folderId.iUid = results[currentItemIdx]->ContentFolderId().iUid ;
        	
        	        	
        	TInt folderCount = 0;
        	RPointerArray<CSrchUiResFolder> folders;
        	iOpenedFolder->SubFolders( folders );
        	
        	folderCount = folders.Count();
        	
        	if( KNullUid.iUid == folderId.iUid || 0 == folderCount )
        	{
        	
		           	HBufC* folderName = iOpenedFolder->GetFolderName().AllocL();
		        	_LIT(kNaviPaneFormat,  "<%S %d/%d>");
		 	        TInt count = 0;
		 	        TInt index1 = 0;
		 	        
		 	        index1 = iOpenedFolder->GetHeavyResultsCount();
		 	        count = CurrentItemIndex() + 1;
		 	        
	             buffer->Des().Format(kNaviPaneFormat, folderName, count, index1 );
	             delete folderName;
            	folderName = NULL;
        	}
        	else 
        	{
        		
        		CSrchUiResFolder* tmpFolder = NULL;
        		for( TInt i = 0; i< folderCount; i++ )
        		{
        			if( folders[i]->ContentFolderType().iUid == folderId.iUid ) 
        			{
        				tmpFolder = folders[i];
        				break;
        			}
        				
        		}
        		HBufC* folderName = tmpFolder->GetFolderName().AllocL();
		        		
		 	    _LIT(kNaviPaneFormat1,  "<%S %d/%d>");
		 	    TInt count = 0;
		 	    TInt index1 = 0;
		 	        
		 	    index1 = tmpFolder->GetHeavyResultsCount();
		 	    count = CurrentItemIndex() + 1;
		 	        
			    buffer->Des().Format( kNaviPaneFormat1, folderName, count , results.Count() );
			    delete folderName;
            	folderName = NULL;
        		
       		
        	}
            folders.Reset();
        }
     
    if ( iNaviDecorator3 ) 
        {
       	delete iNaviDecorator3;
       	iNaviDecorator3 = NULL;
        }
        
    iNaviDecorator3 = np->CreateNavigationLabelL(buffer->Des());
    np->PushL(*iNaviDecorator3);
    CAknNaviLabel* naviLabel = (CAknNaviLabel*)iNaviDecorator3->DecoratedControl();
    naviLabel->SetTextL(buffer->Des());
    naviLabel->SetNaviLabelType( CAknNaviLabel::EHintText );

    iNaviPane->DrawNow();
    naviLabel->DrawNow();

    CleanupStack::PopAndDestroy(buffer);
	}
	
// ----------------------------------------------------------------------------
// CSearchAppUi::SetErrorNoteLL
// For displaying Error Note
// ----------------------------------------------------------------------------
//	
void CSrchUiResultContainer::SetErrorNoteLL( TBool searchFinished, TBool 
							aResultsRetrieving )
	{	
	  
	  if ( iIcons ) 
    	{	
    	iIcons->ResetAndDestroy();
    	iIcons = NULL;
    	}
	  
	  if(iDoubleLineListBox)
	  {
	    delete iDoubleLineListBox;
	    iDoubleLineListBox = NULL;
	  }

    iDoubleLineListBox = new (ELeave) CAknDoubleGraphicStyleListBox;
    iDoubleLineListBox->SetContainerWindowL( *this );
    iDoubleLineListBox->ConstructL( this, EAknListBoxSelectionList );
	   
    iDoubleLineListBox->SetRect( Rect() );
    iDoubleLineListBox->ActivateL();
    iDoubleLineListBox->CreateScrollBarFrameL( ETrue );
    iDoubleLineListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
                    CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto);
    HBufC* text = NULL;

    
    if ( searchFinished ) 
    {
    if ( 1 < iSrchUiResRootFolder->GetSubFoldersCount() )
	    {
        text = CCoeEnv::Static()->AllocReadResourceAsDes16L(
                                 R_QTN_SEARCH_SINGLE_NO_RESULTS );  
	    }
    else
	    {
	      text = CCoeEnv::Static()->AllocReadResourceAsDes16L(
                                 R_QTN_SEARCH_NO_RESULTS );
	    }
	if ( aResultsRetrieving )	
		{
		delete text;
		text = NULL;
		text = CCoeEnv::Static()->AllocReadResourceAsDes16L(
                                 R_QTN_SEARCH_FETCHING_RESULTS );
		}
    }
    
    if( text )
        {
    	iDoubleLineListBox->View()->SetListEmptyTextL(*text); //Set Error Note
        delete text;
        }
	}

// End of File
