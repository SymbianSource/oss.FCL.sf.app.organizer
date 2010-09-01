/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     A container class for CSrchUiResFolderView.
*
*/






#include <eikclbd.h>
#include <akntabgrp.h> 
#include <aknappui.h>
#include <srchui.rsg>
#include <avkon.hrh>
#include <aknnavi.h> 
#include <aknlists.h> 
#include <aknnavide.h> 
#include <AknIconArray.h> 
#include <searchiconinfo.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include <aknnavilabel.h> 
#include <akntitle.h> 
#include <utf.h>
#include <gnrlsearchuisvg.mbg> 
#include <data_caging_path_literals.hrh>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK
#include "srchuiresfolder.h"
#include "srchuigeneral.h"
#include "srchuiresfoldercontainer.h"
#include "srchuiresfolderview.h"
#include "srchuidefines.h"
#include "srchui.hrh"
#include "srchuiresfoldermodel.h"

const TInt KTitleLength = 50;

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::CSrchUiResFolderContainer
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
CSrchUiResFolderContainer::CSrchUiResFolderContainer( CSrchUiResFolderView& 
		aView, CSrchUiGeneral* aGeneralsrchUi ) :	iView( aView ) , iTouchFlag(EFalse)
		{
	  iGeneralsrchUi = aGeneralsrchUi;
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::ConstructL( const TRect& aRect )
    {
    iSrchUiResTitle = HBufC::NewL( KMaxCharsForNaviPaneText );
    TPtr des = iSrchUiResTitle->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8( des, 
    		iGeneralsrchUi->GetQueryString() );
    SetRoot( iGeneralsrchUi->SrchUiResRoot() );
		iModel = CSrchUiResFolderModel::NewL( iGeneralsrchUi );
		iModel->SetOpenedFolder( iOpenFolder );
    CreateWindowL();
		CEikStatusPane* statusPane = STATIC_CAST( CAknAppUi*, 
				iEikonEnv->EikAppUi() )->StatusPane();
		CCoeControl* control = ( statusPane->ControlL( 
				TUid::Uid( EEikStatusPaneUidNavi ) ) );
		iNaviPane = STATIC_CAST( CAknNavigationControlContainer*, control );
		SetupListBoxL();
		SetRect( aRect );
    // Restore previous listbox item index
    TInt prevIdx ( iView.ContainerListBoxIndex() );
        
    if ( prevIdx >= 0 && prevIdx < iModel->MdcaCount() )
        {
        if ( iSingleLineListBox )
            {
            iSingleLineListBox->SetCurrentItemIndex( prevIdx );
            }
        }
    ActivateL();
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::~CSrchUiResFolderContainer
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CSrchUiResFolderContainer::~CSrchUiResFolderContainer()
    {
    // Store listboxes current index so that it can be restored to the same place if needed
    TInt idx = 0;
    if ( iSingleLineListBox )
        {
        idx = iSingleLineListBox->CurrentItemIndex();
        }
    iView.SetContainerListBoxIndex( idx );
    delete iSingleLineListBox;
    delete iModel;
    delete iSrchUiResTitle;
    
    if ( iNaviDecorator ) 
    		{
    		delete iNaviDecorator;
    		iNaviDecorator = NULL;
    		}
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::SetRoot
// Sets the root folder for this directory structure.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::SetRoot( MSrchUiFolder* aFolder )
		{
		iOpenFolder = STATIC_CAST ( CSrchUiResFolder* , aFolder );
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::SetRoot
// Called by the framework when the view size is changed.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::SizeChanged()
    {
    if ( iSingleLineListBox )
        {
        iSingleLineListBox->SetRect( Rect() );
        }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::CountComponentControls
// Gets the number of controls contained in a compound control.
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiResFolderContainer::CountComponentControls() const
    {
    if ( iSingleLineListBox )
        {
        return 1;
        }
    else
        {
        return 0;
        }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::ComponentControl
// Gets the specified component of a compound control.
// -------------------------------------------------------------------------------------------------
//
CCoeControl* CSrchUiResFolderContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            if ( iSingleLineListBox )
                {
                return iSingleLineListBox;
                }
            break;

        default:
            return NULL;
        }
    return NULL;    // to avoid warning
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::OfferKeyEventL
// Called by the framework when a key event needs to be processed.
// -------------------------------------------------------------------------------------------------
//
TKeyResponse CSrchUiResFolderContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    if ( aType != EEventKey )
    	{ 
    	return EKeyWasNotConsumed;
    	}

	TKeyResponse response( EKeyWasNotConsumed );
	switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
        case EKeyRightArrow:
								{
        				MSrchUiFolder* parent = iOpenFolder->GetParentFolder();
            		if ( parent )
                		{
    								response = iTabGroup->OfferKeyEventL( aKeyEvent, aType );
            				if ( response == EKeyWasConsumed )
    	        					{
    	        					MSrchUiFolder* parent = iOpenFolder->GetParentFolder();
    	        					if ( parent )
    	        							{
       											iOpenFolder = STATIC_CAST ( CSrchUiResFolder* , 
       					            parent->GetSubFolder( iTabGroup->ActiveTabIndex() ) );
       											iModel->SetOpenedFolder( iOpenFolder );
                        		HandleItemAdditionL();
                        		SetCurrentItemIndexAndDrawL( 0 );
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
            		HandleItemSelectionL();
            		// dont use the MEikListBoxObserver and HandleListBoxEventL 
            		// because when switching between single line and double 
            		// line listboxes, things get messed up
            		// ex. Rocker pushed
            		//
            		// CCoeControl::OfferKeyEventL Called
            		//
            		// iSingleLineListBox->OfferKeyEventL( aKeyEvent, aType ) 
            		// called from here causes 
            		//  MEikListBoxObserver::HandleListBoxEventL to be called
            		//
            		// HandleItemSelectionL called from within 
            		//	MEikListBoxObserver::HandleListBoxEventL
            		// this causes the entering of the directory structure 
            		// and this uses double
            		//  line listbox, thus single line list box is deleted and
            		//  MEikListBoxObserver::HandleListBoxEventL is called via 
            		// this single line listbox
            		//  object ->Panics
            		}
            		break;
         		default:
                {
                response =  iSingleLineListBox->OfferKeyEventL( aKeyEvent, 
                		aType );
                }
           			break;
        	}

    if ( response != EKeyWasNotConsumed )
        {
        UpdateTitlePaneL();
        }
		return response;

    }
    

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::HandlePointerEventL
// Handles pointer events from framework.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::HandlePointerEventL(const TPointerEvent &aPointerEvent)
	{
	// Check if touch is enabled or not
    if( !AknLayoutUtils::PenEnabled() )
        {
        return;
        }
    
    TInt index = iSingleLineListBox->CurrentItemIndex();
    if(index < 0)
    	{
    		return;
    	}
    	
    TInt indexPos = KErrNotFound; 
    iSingleLineListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition,indexPos);
    if( index != KErrNotFound && indexPos != KErrNotFound )
        {
        switch( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            {
                iSingleLineListBox->HandlePointerEventL(aPointerEvent);   
                
                if( iSingleLineListBox->CurrentItemIndex() == index)
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
               if( iTouchFlag )
                {
                #ifdef RD_TACTILE_FEEDBACK 
                MTouchFeedback* feedback = MTouchFeedback::Instance();
                if ( feedback )
                    {
                    feedback->InstantFeedback( this, ETouchFeedbackList,ETouchFeedbackVibra,aPointerEvent );
                    }
                #endif //RD_TACTILE_FEEDBACK
                
                HandleItemSelectionL(); 
                }
            }
            break;
        case TPointerEvent::EDrag:
            {
                iSingleLineListBox->HandlePointerEventL(aPointerEvent);
                if( iTouchFlag  &&  iSingleLineListBox->CurrentItemIndex() != index )
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
           iSingleLineListBox->HandlePointerEventL(aPointerEvent);
            if( !iSingleLineListBox->Model()->ItemTextArray()->MdcaCount() )
            {
               HandleItemSelectionL(); 
            }  
        }  
     }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::HandleListBoxEventL
// Called by the framework when a list box event needs to be processed.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::HandleListBoxEventL( CEikListBox* aListBox,
		TListBoxEvent aEventType )
    {
    
    if ( aEventType == MEikListBoxObserver::EEventEnterKeyPressed )
        {
        
        
        }
     
     }
    
      
     
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::HandleItemSelectionL
// Handles the selection of the currently active item.
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiResFolderContainer::HandleItemSelectionL()
    {
    TInt currentItemIdx = CurrentItemIndex();
    TInt listBoxType = 0;
    if ( currentItemIdx >= 0 )
    		{
    		listBoxType = GetListBoxType( currentItemIdx );
    		if ( IsEmpty() )
        		{
        		return listBoxType;
        		}

				switch ( ( TFolderListBoxItem ) listBoxType )
						{
						case EListBoxFolder:
			    			{
			    			TInt selectedfolderidx = GetItemIndex( currentItemIdx );
								iGeneralsrchUi->LoadResultViewerL( selectedfolderidx );
			    			}
								break;
						}
				HandleItemAdditionL();
				return listBoxType;
	    	}
		return 0;
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::FolderSelectedL
// Handles the selection of the currently selected Folder.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::FolderSelectedL( MSrchUiFolder* aFolder )
		{
 		iOpenFolder = STATIC_CAST( CSrchUiResFolder*, aFolder );
    SetupListBoxL();
		iModel->SetOpenedFolder( iOpenFolder );

		if ( iModel->MdcaCount() )
	  	  {
        SetCurrentItemIndex( 0 );
	    	}
    UpdateTitlePaneL();
		}
		
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::GetListBoxType
// Returns EListBoxFolder when the selected item is a folder item.
// Returns EListBoxFile when the selected item is a file item.
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiResFolderContainer::GetListBoxType( TInt aIndex )
		{
		if ( aIndex+1 <= iOpenFolder->GetSubFoldersCount() )
				{
				return EListBoxFolder;
				}
		else
				{
				return EListBoxFile;
				}
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::GetItemIndex
// Folders and Files are stored into 1 array for use with the listbox.  First the 
//  folders are listed and then files.
//  The function returns a folder or file index.
// To know if the selected item was a folder or file the GetListBoxType method can be used.
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiResFolderContainer::GetItemIndex( TInt aIndex )
		{
		switch( ( TFolderListBoxItem )GetListBoxType( aIndex ) )
				{
				case EListBoxFolder:
						return aIndex;

				case EListBoxFile:
						return aIndex - iOpenFolder->GetSubFoldersCount();
				}
		return KErrNotFound;
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::GetItemIndex
// Handles changes to the title pane text.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::UpdateTitlePaneL()
		{
	
	  TBuf<KTitleLength> titleText;
    _LIT( KtitleFormat, "\"%S\"" );
    titleText.Format( KtitleFormat, iSrchUiResTitle );
	
	  const CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
	  CAknTitlePane* titlePane = STATIC_CAST(CAknTitlePane*, 
									sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

     // different
     titlePane->SetTextL( titleText );
     _LIT( KResMifFileName, "gnrlsearchuisvg.mif" );  
	    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
		TFileName mifFileName( driveLetter );
		mifFileName.Append( KDC_APP_BITMAP_DIR );
		mifFileName.Append( KResMifFileName );        
		
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		CFbsBitmap* bitmap = NULL;
		CFbsBitmap* mask	= NULL;
		
        AknsUtils::CreateIconL( skin, KAknsIIDQgnPropSearchIndeviceTitle,
                bitmap, mask, mifFileName,    
                EMbmGnrlsearchuisvgQgn_prop_search_indevice_title,
                EMbmGnrlsearchuisvgQgn_prop_search_indevice_title_mask );

        titlePane->SetSmallPicture( bitmap, mask, ETrue );
	
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::UpdateNaviPaneL
// Handles the changes needed to the Navigation Pane.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::UpdateNaviPaneL( const TDesC& aBuf )
		{
        
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* np= 
			(CAknNavigationControlContainer *)sp->ControlL( TUid::Uid( 
					EEikStatusPaneUidNavi ) ); 
    //in buf is the text you want display
    if ( iNaviDecorator ) 
        {
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }
        iNaviDecorator = np->CreateNavigationLabelL( aBuf );
        np->PushL( *iNaviDecorator );
        iNaviDecorator->SetControlType( CAknNavigationDecorator::ENaviLabel );
        CAknNaviLabel* naviLabel = ( CAknNaviLabel* )
        		iNaviDecorator->DecoratedControl();
        naviLabel->SetTextL( aBuf );
     
        iNaviPane->DrawNow();
        naviLabel->DrawNow();
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::GoUpOneLevelL
// Will go 1 step higher in the directory structure.
// -------------------------------------------------------------------------------------------------
//
TBool CSrchUiResFolderContainer::GoUpOneLevelL() 
		{
		MSrchUiFolder* parent = iOpenFolder->GetParentFolder();
    if ( parent )
    		{
    		iOpenFolder = STATIC_CAST( CSrchUiResFolder*, parent );
    		iModel->SetOpenedFolder( iOpenFolder );
    	
    		TInt groupIndex = iTabGroup->ActiveTabIndex(); 

        SetupListBoxL();

    		UpdateTitlePaneL();
				SetCurrentItemIndexAndDrawL( groupIndex );

				return ETrue;
    		}
		else
				{
				return EFalse;	// Was at Root level
				}
		}
	
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::SetupListBoxL
// Constructs and prepares the list box control.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::SetupListBoxL()
		{
    RPointerArray<CSrchUiResFolder> folders;
		iOpenFolder->SubFolders( folders );
	
    iSingleLineListBox = new ( ELeave ) CAknSingleGraphicStyleListBox;
    iSingleLineListBox->SetContainerWindowL( *this );
    iSingleLineListBox->ConstructL( this, EAknListBoxSelectionList );
    iSingleLineListBox->SetListBoxObserver( this );
	   
    iSingleLineListBox->SetRect( Rect() );
    iSingleLineListBox->ActivateL();
    iSingleLineListBox->CreateScrollBarFrameL( ETrue );
    iSingleLineListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
    	CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto );
    
		iSingleLineListBox->Model()->SetItemTextArray( iModel );
		iSingleLineListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // passing ownership of icons
		iSingleLineListBox->ItemDrawer()->ColumnData()->SetIconArray( 
				ListBoxIconsArrayL(folders) );
				
		folders.Reset();
 		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::IsEmpty
// ETrue when the directory structure is empty (no folders and no files), 
// EFalse otherwise.
// -------------------------------------------------------------------------------------------------
//
TBool CSrchUiResFolderContainer::IsEmpty()
		{
		return ( iModel->MdcaCount() == 0 );
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::ResultsClearedL
// Informs the control(listbox) that the previous results were deleted.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::ResultsClearedL()
		{
    SetRoot( iGeneralsrchUi->SrchUiResRoot() );
		iModel->SetOpenedFolder( iOpenFolder );
	
    SetCurrentItemIndex( 0 );

    HandleItemAdditionL();

		UpdateTitlePaneL();
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::FileSelected
// Handles the selection of the currently selected File.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::FileSelected( MFile* /*aFile*/)
		{
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::HandleItemAdditionL
// Calls the HandleItemAdditionL method on the correct listbox (single or double line).
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::HandleItemAdditionL()
    {
    if ( iSingleLineListBox )
        {
        iSingleLineListBox->HandleItemAdditionL();
      	}
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::CurrentItemIndex
// Asks the item index from the correct listbox
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiResFolderContainer::CurrentItemIndex()
    {
    return iSingleLineListBox->CurrentItemIndex();
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::SetCurrentItemIndex
// Calls the SetCurrentItemIndex method on the correct listbox (single or double line).
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::SetCurrentItemIndex( TInt aIndex )
    {
    if ( iSingleLineListBox )
        {
        iSingleLineListBox->SetCurrentItemIndex( aIndex );
      	}
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::SetCurrentItemIndexAndDraw
// Calls the SetCurrentItemIndexAndDraw method on the correct listbox (single or double line).
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::SetCurrentItemIndexAndDrawL( TInt aIndex )
    {
    if ( iSingleLineListBox )
        {
        iSingleLineListBox->HandleItemAdditionL();
        iSingleLineListBox->SetCurrentItemIndexAndDraw( aIndex );
        }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::GetOpenFolder
// Getter for iOpenFolder.
// -------------------------------------------------------------------------------------------------
//
MSrchUiFolder* CSrchUiResFolderContainer::GetOpenFolder() const
    {
    return iOpenFolder;
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::SearchFinishedL
// Informs the control(listbox) that one of the searches has finished.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::SearchFinishedL()
    {
    if ( iSingleLineListBox )
        {
        iSingleLineListBox->HandleItemAdditionL();
        }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::HandleResourceChange
// Handles a change to the application's resources which are shared across the environment.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::HandleResourceChange( TInt aType )
	{
	   TRAP_IGNORE(HandleResourceChangeL( aType ));
	}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::HandleResourceChange
// Handles a change to the application's resources which are shared across the environment.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::HandleResourceChangeL( TInt aType )
		{
    CCoeControl::HandleResourceChange( aType );

		// Event generated by the FW when the screen size changed/flipped.
		if ( aType == KEikDynamicLayoutVariantSwitch )
	  	  {
        TRect rect;
				AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        SetRect( rect );
	    	} 
		}

// -------------------------------------------------------------------------------------------------
// CSearchAppUi::ListBoxIconsArrayL
// Returns an array of icons for use in the listboxes.
// -------------------------------------------------------------------------------------------------
//
CArrayPtr<CGulIcon>* CSrchUiResFolderContainer::ListBoxIconsArrayL(
												RPointerArray<CSrchUiResFolder>& folders )
		{
    CArrayPtr<CGulIcon>* icons = new( ELeave ) CAknIconArray(
                                     KSearchListBoxIconsArrayGranularity );
    CleanupStack::PushL( icons );
 
    for ( TInt i=0; i<folders.Count(); i++ )
        {
        
        const CSearchIconInfo& icon = *( folders[i]->IconInfo() );
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
		    CFbsBitmap* bitmap = NULL;
		    CFbsBitmap* mask	= NULL;

				//Ext Icon Skinning implementation for Display
		
	      AknsUtils::CreateIconL( skin,KAknsIIDQgnIndiCalcButtonPlus,
										bitmap,mask,icon.GetIconFile(),
		            icon.GetIconIndex(), icon.GetIconMaskIndex() );
		    CGulIcon* picon = CGulIcon::NewL( bitmap, mask );
				CleanupStack::PushL( picon );
	      icons->AppendL( picon );
				  CleanupStack::Pop( picon );
        }
		CleanupStack::Pop( icons ); // icons
    return icons;
		}

// -------------------------------------------------------------------------------------------------
// CSearchAppUi::SetErrorNoteL
// For displaying Error Note
// -------------------------------------------------------------------------------------------------
//	
void CSrchUiResFolderContainer::SetErrorNoteL()
		{	
		
		if ( iSingleLineListBox )
				{
				delete iSingleLineListBox;
				iSingleLineListBox = NULL;
				}
    iSingleLineListBox = new ( ELeave ) CAknSingleGraphicStyleListBox;
    iSingleLineListBox->SetContainerWindowL( *this );
    iSingleLineListBox->ConstructL( this, EAknListBoxSelectionList );
    iSingleLineListBox->SetRect( Rect() );
    iSingleLineListBox->ActivateL();
    iSingleLineListBox->CreateScrollBarFrameL( ETrue );
    iSingleLineListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
    		CEikScrollBarFrame::EOn, CEikScrollBarFrame::EAuto );
    HBufC* text = iCoeEnv->AllocReadResourceLC( R_QTN_SEARCH_NO_RESULTS );
    iSingleLineListBox->View()->SetListEmptyTextL( *text ); //Set Error Note
    CleanupStack::PopAndDestroy( text ); // text
		}

TBool CSrchUiResFolderContainer::IsElementValid()
		{
		TInt index  = CurrentItemIndex();
		RPointerArray<CSrchUiResFolder> folders;
		iOpenFolder->SubFolders( folders );
	
		const CSrchUiResFolder* subFolder =
                    ( folders )[index];
   
		return( subFolder->GetAllResultsCount() );
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::ResultsAddedL
// Informs the control(listbox) that search results were added
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::ResultsAddedL()
		{
		MSrchUiFolder* parent = iOpenFolder->GetParentFolder();
    if ( parent )  // inside category-> counter needs update
        {
       	UpdateTitlePaneL();
        }
    HandleItemAdditionL();
		}

// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderContainer::SearchStatusChangedL
// Called from the application ui class after some search status change
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResFolderContainer::SearchStatusChangedL()
    {
		MSrchUiFolder* parent = iOpenFolder->GetParentFolder();
    if ( !parent )  // root level
        {
        }
    }
// End of File

