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
* Description:    This class acts as the container for CSrchUiSearchView. It also handles the user inputs.  
*
*/






#include <AknQueryDialog.h> 
#include <srchui.rsg>
#include <eikclbd.h>
#include <AknIconArray.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <akntitle.h> 
#include <AknsControlContext.h> 
#include <AknsBasicBackgroundControlContext.h> 
#include <aknnavilabel.h> 
#include <aknconsts.h>
#include <avkon.mbg>
#include <utf.h>
#include <aknnavi.h> 
#include <aknnavide.h>
#include <gnrlsearchuisvg.mbg>
#include <data_caging_path_literals.hrh>
#include <srchuiindevice.rsg>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK
//User Includes
#include "srchuiservicecontainer.h"
#include "srchuiappuids.h"
#include "srchuimodel.h"
#include "srchuidefines.h"
#include "srchuiserviceview.h"

#include "srchui.hrh"
#include "srchuigeneral.h"

#include	<AknLayout.lag>       	 // AVKON LAF layouts
#include	<AknLayoutDef.h>
// ====================================== MEMBER FUNCTIONS =========================================

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::CSrchUiServiceContainer
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
CSrchUiServiceContainer::CSrchUiServiceContainer( CSrchUiSearchView& aView, 
												 CSrchUiGeneral* aGenSrchUi ):iView( aView ),
    iSearchTextPtr( NULL,0 ) ,iTouchFlag(EFalse)
    {
    iGenSrchUi = aGenSrchUi;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
	iModel = CSrchUiModel::NewL( iGenSrchUi );
       
	iSearchText = HBufC::NewL( KMaxSearchKeywordsLength );
	iSearchTextPtr.Set( iSearchText->Des() );

    SetupFindBoxL();
	SetupListBoxL();
	// Restore previous listbox item index
	TInt prevIdx ( iView.ContainerListBoxIndex() );
    if ( prevIdx >= 0 && prevIdx < iModel->MdcaCount() )
        {
        iListBox->SetCurrentItemIndex( prevIdx );
        }
    SetRect( aRect );
    ActivateL();

	iSearchFieldBox->SelectSearchTextL();
	UpdateNaviPaneL();
	UpdateTitlePaneL();
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::~CSrchUiServiceContainer
// Destructor.
// -------------------------------------------------------------------------------------------------
//
CSrchUiServiceContainer::~CSrchUiServiceContainer()
    {
    // Store listboxes current index so that it can be restored to the 
    //same place if needed
		if ( iListBox )
				{
				iView.SetContainerListBoxIndex( iListBox->CurrentItemIndex() );
				}


		if ( iListBox )
		{
		delete iListBox;
		iListBox = NULL;
		}

		// Important to set the find box non-focusing before
		// deleting it, otherwise the focus changes triggered
		// by the removal of the control from stack will focus
		// the find box which is under deletion
		if ( iSearchFieldBox )
		{
		iSearchFieldBox->SetNonFocusing();
		delete iSearchFieldBox;
		iSearchFieldBox = NULL;
		} 	

    if ( iModel ) 
    		{
        delete iModel;
        iModel = NULL;
      	}
   
    if ( iSearchText )
    		{
        delete iSearchText;
        iSearchText = NULL;
      	}
    if ( iNaviDecorator ) 
    		{
    		delete iNaviDecorator ;
    		iNaviDecorator = NULL;
    		}
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::DoSearchL
// Starts off the search.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::DoSearchL()
    {
    TBool atleastoneselected = EFalse;
    iSearchFieldBox->GetSearchText( iSearchTextPtr );
    
    HBufC8* des = HBufC8::NewLC( KMaxSearchKeywordsLength );
    TPtr8 ptr1 = des->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr1,iSearchTextPtr );
        
    iGenSrchUi->SetSearchStringL( iSearchTextPtr );
    
    TUid serviceId( iView.Id() );
    RArray<TBool>& selectedContentTypes ( iGenSrchUi->SelectedContentTypes() );
    
    for ( TInt i = 0; i<selectedContentTypes.Count() && !atleastoneselected; i++ )
   			{
    		if ( selectedContentTypes[i] )
    				{
    				atleastoneselected = ETrue;
    				}
    		}
    
    if ( !atleastoneselected )
    		{
    		HandleItemSelectionL(ETrue);
    		}
    iGenSrchUi->HandleSelectionL( *des, serviceId );
    CleanupStack::PopAndDestroy( des );
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::ClearTextL
// Clears the text of the search field
// -------------------------------------------------------------------------------------------------    
void CSrchUiServiceContainer::ClearTextL()
    {
    iSearchFieldBox->ResetL();
    HBufC* nullDesC = KNullDesC().AllocLC();
    iSearchFieldBox->SetSearchTextL( *nullDesC );
    CleanupStack::PopAndDestroy(nullDesC);
    iSearchFieldBox->DrawNow();
    }    

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::OfferKeyEventL
// Called by the framework when a key event needs to be processed.
// -------------------------------------------------------------------------------------------------
//
TKeyResponse CSrchUiServiceContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
													 TEventCode aType )
    {
		TKeyResponse response( EKeyWasNotConsumed );

    response = iSearchFieldBox->OfferKeyEventL( aKeyEvent, aType );

    if ( response == EKeyWasNotConsumed )
        {
	      response = iListBox->OfferKeyEventL( aKeyEvent, aType );
        }
        

    return response;
    }
       
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::SizeChanged
// Called by the framework when the view size is changed.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::SizeChanged()
    {
    const TRect rect( Rect() );
    if ( iListBox && iSearchFieldBox )
        {
        AknLayoutUtils::LayoutControl( iListBox,
	                                   rect,
	                                   AKN_LAYOUT_WINDOW_list_gen_pane( 1 ) );

        AknLayoutUtils::LayoutControl( iSearchFieldBox,
	                                    rect,
	                                    AKN_LAYOUT_WINDOW_find_pane );
        }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::CountComponentControls
// Gets the number of controls contained in a compound control.
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiServiceContainer::CountComponentControls() const
    {
    return 2;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::ComponentControl
// Gets the specified component of a compound control.
// -------------------------------------------------------------------------------------------------
//
CCoeControl* CSrchUiServiceContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListBox;

        case 1:
            return iSearchFieldBox;

        default:
            return NULL;
        }
    }
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::HandleResourceChange
// Handles a change to the application's resources which are shared across the environment.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::HandleResourceChange( TInt aType )
    {
    TRAPD ( ignore, HandleResourceChangeL( aType ) );
    }
	
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::HandleListBoxEventL
// Called by the framework when a list box event needs to be processed.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::HandleListBoxEventL( CEikListBox* aListBox,
		TListBoxEvent aEventType )
    {
    if ( aEventType == MEikListBoxObserver::EEventEnterKeyPressed 
    					|| aEventType == MEikListBoxObserver::EEventItemClicked )
        {
        if(!iTouchFlag)
        	{

        	HandleItemSelectionL();
        	UpDateCbaL();
        	
        	}
        
       	iTouchFlag = EFalse;
       	
        }
     else if(aEventType == MEikListBoxObserver::EEventItemDraggingActioned )
     	{
     	iTouchFlag = ETrue;
     	}
    
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::HandleItemMarkUnMarkL
// handles Items unmarked
// -------------------------------------------------------------------------------------------------
//    
void CSrchUiServiceContainer::HandleItemMarkUnMarkL()
    {
    HandleItemSelectionL();
    }
		
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::CurrentItemIndex
// returns current item index
// -------------------------------------------------------------------------------------------------
//     
TInt CSrchUiServiceContainer::CurrentItemIndex()
    {
    return iListBox->CurrentItemIndex();
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::UpdateNaviPaneL
// Handles changes to the navigation pane text.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::UpdateNaviPaneL()
		{
	  CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* np= 
				( CAknNavigationControlContainer * )sp->ControlL( 
				TUid::Uid( EEikStatusPaneUidNavi ) );

    //in buf is the text you want display
    TBuf<5> buffer;
    _LIT( kTemp, " " );
    buffer.Copy( kTemp );
    if ( iNaviDecorator ) 
        {
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }
    iNaviDecorator = np->CreateNavigationLabelL( buffer );
    np->PushL( *iNaviDecorator );
        
    CAknNaviLabel* naviLabel = ( CAknNaviLabel* )iNaviDecorator->DecoratedControl();

    naviLabel->SetTextL( buffer );
     
    np->DrawNow();
    naviLabel->DrawNow();
    np->Pop(); //pop naviDecorator	
		}
    
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::SetupListBoxL
// Constructs and prepares the list box.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::SetupListBoxL()
    {
    delete iListBox;
    iListBox = NULL;
	
    iListBox = new( ELeave )CAknSingleGraphicStyleListBox;
    iListBox->SetContainerWindowL( *this );
    iListBox->SetListBoxObserver( this );	// HandleListBoxEventL gets more events
    iListBox->ConstructL( this, EAknListBoxLoopScrolling );

    const TRect rect( Rect() );    
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()
            ->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    iListBox->Model()->SetItemTextArray( iModel );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // iListBox takes ownership of the icons array
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( ListBoxIconsArrayL() );

    iListBox->ActivateL();

    // Needed when listbox and findbox inside 1 view
    // Without this the offer key event will crash.
    CAknFilteredTextListBoxModel* model = 
        STATIC_CAST( CAknFilteredTextListBoxModel*, iListBox->Model() );
    
    model->CreateFilterL( iListBox, iSearchFieldBox );
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::SetupFindBoxL
// Constructs and prepares the find box.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::SetupFindBoxL()
    {
    delete iSearchFieldBox;
    iSearchFieldBox = NULL;
    
 	iSearchFieldBox = CAknSearchField::NewL( *this, CAknSearchField::ESearch,NULL,50 );
	  
	
    iSearchFieldBox->SetFocus( ETrue );
    
    CEikEdwin& editor = iSearchFieldBox->Editor();
    editor.SetEdwinObserver( this ); 
  
    if ( 0 < iGenSrchUi->SearchStringL().Length() )
        {
        HBufC8* text = iGenSrchUi->SearchStringL().AllocL();
        HBufC* des = HBufC::NewL(30);
        TPtr ptr1 = des->Des();
        CnvUtfConverter::ConvertToUnicodeFromUtf8( ptr1, *text );
        iSearchFieldBox->SetSearchTextL( ptr1 );
		//RMD
        delete(text);
        delete(des);
        }
    iSearchFieldBox->GetSearchText( iSearchTextPtr );
    iSearchFieldBox->SelectSearchTextL();
    // If more than 0 characters in find box, change the CBA buttons
    if ( iSearchTextPtr.Length() )
        {
        iView.SetCbaButtonsL( R_QTN_SOFTKEYS_SEARCH_MARK_CANCEL );
        }
    else
        { 
        	iView.SetCbaButtonsL( R_QTN_SOFTKEYS_OPTION_MARK_EXIT );        		
        }
		
    // The correct line position to use is 2, which corresponds
    // EABColumn in Avkon (not a public enumeration,
    // hence hard-coding used here)
    const TInt KSeparatorLinePos = 2;
    iSearchFieldBox->SetLinePos( KSeparatorLinePos );
    iSearchFieldBox->ActivateL();
    
    }
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::GetItemIndex
// Handles changes to the title pane text.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::UpdateTitlePaneL()
	{
	HBufC* titleText = CCoeEnv::Static()->AllocReadResourceAsDes16L(
	        R_QTN_TITLE_SEARCH_SERVICE);
	const CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
	CAknTitlePane* titlePane = STATIC_CAST( CAknTitlePane*, 
									sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    // different
    titlePane->SetText( titleText );
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
// CSrchUiServiceContainer::HandleItemSelectionL
// Handles the selection of the currently active item.
// Called from HandleListBoxEventL.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::HandleItemSelectionL()
    {
    TInt idx ( iListBox->CurrentItemIndex() );
    
    TInt realIdx ( idx-1 );   // Because of "All in device"
    
    RArray<TBool>& selectedContentTypes ( iGenSrchUi->SelectedContentTypes() );

    if ( idx == KAllInDeviceArrayIndex )
        {
        TBool newState ( !iGenSrchUi->AllContentClassesSelected() );
        iGenSrchUi->SetAllContentClassesSelected( newState );

        // Update all content types        
        for ( TInt i( 0 ); i<selectedContentTypes.Count(); i++ )
            {
            selectedContentTypes[i] = newState;
            }
        UpdateAllInDeviceSelection( selectedContentTypes );
        }
    else    // Some "real" content type selection has changed
        {
        selectedContentTypes[realIdx] = !selectedContentTypes[realIdx];   // toggle state

        UpdateAllInDeviceSelection( selectedContentTypes );
        }
    iListBox->DrawNow();
		}
		
		
void CSrchUiServiceContainer::HandleItemSelectionL(TBool aallSelected)
    {
    if(!aallSelected)
    	return;
    TInt idx ( iListBox->CurrentItemIndex() );
    
    TInt realIdx ( idx-1 );   // Because of "All in device"
    
    RArray<TBool>& selectedContentTypes ( iGenSrchUi->SelectedContentTypes() );


	TBool newState ( !iGenSrchUi->AllContentClassesSelected() );
	iGenSrchUi->SetAllContentClassesSelected( newState );

	// Update all content types        
	for ( TInt i( 0 ); i<selectedContentTypes.Count(); i++ )
		{
		selectedContentTypes[i] = newState;
		}
	UpdateAllInDeviceSelection( selectedContentTypes );
	
    iListBox->DrawNow();
	}
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::ListBoxIconsArrayL
// Returns an array of icons for use in the listbox.
// -------------------------------------------------------------------------------------------------
//
CArrayPtr<CGulIcon>* CSrchUiServiceContainer::ListBoxIconsArrayL()
    {
    CArrayPtr<CGulIcon>* icons = new( ELeave ) CAknIconArray( 2 );
    CleanupStack::PushL( icons );

		CFbsBitmap* iconBmp;
		CFbsBitmap* IconMaskBmp;

    // Check box off
  
    TAknsItemID KSearchCheckboxOff;
    KSearchCheckboxOff.Set( 0x0A8461F4, 1 );
		AknsUtils::CreateIconL( AknsUtils::SkinInstance(), KSearchCheckboxOff, 
							iconBmp, IconMaskBmp,
							KSvgIconFile, EMbmGnrlsearchuisvgQgn_prop_checkbox_off,
							EMbmGnrlsearchuisvgQgn_prop_checkbox_off_mask );
									   
    CleanupStack::PushL( iconBmp );		   
		CleanupStack::PushL( IconMaskBmp );
									   
		CGulIcon* gulIcon = CGulIcon::NewL( iconBmp, IconMaskBmp);
		CleanupStack::Pop( IconMaskBmp );		   
		CleanupStack::Pop( iconBmp );

		CleanupStack::PushL( gulIcon );	
	
		icons->AppendL( gulIcon );
    CleanupStack::Pop( gulIcon );	
    
		TAknsItemID KSearchCheckboxOn;
    KSearchCheckboxOn.Set( 0x0A8461F4, 2 );    
		AknsUtils::CreateIconL( AknsUtils::SkinInstance(), KSearchCheckboxOn, 
						iconBmp, IconMaskBmp,
						KSvgIconFile, EMbmGnrlsearchuisvgQgn_prop_checkbox_on,
						EMbmGnrlsearchuisvgQgn_prop_checkbox_on_mask );
		CleanupStack::PushL( iconBmp );		   
		CleanupStack::PushL( IconMaskBmp );
									   
		gulIcon = CGulIcon::NewL( iconBmp, IconMaskBmp);
		CleanupStack::Pop( IconMaskBmp );		   
    CleanupStack::Pop( iconBmp );

		CleanupStack::PushL( gulIcon );	
	
		icons->AppendL( gulIcon );
    CleanupStack::Pop( gulIcon );	

	  CleanupStack::Pop( icons );
    return icons;
    }
	
 

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::UpdateAllInDeviceSelection
// Updates the "All in device" selection.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::UpdateAllInDeviceSelection( 
		RArray<TBool>& aIncludedContentTypes )
    {
    TBool allSelected( ETrue );
    
    for ( TInt i( 0 ); i<aIncludedContentTypes.Count(); i++ )
        {
        if ( !aIncludedContentTypes[i] )
            {
            allSelected = EFalse;   // At least one is de-selected
            break;  // no need to continue
            }
        }

    if ( allSelected ) 
        {
        iGenSrchUi->SetAllContentClassesSelected( ETrue );
        }
    else
        {
        iGenSrchUi->SetAllContentClassesSelected( EFalse );
        }
    }


void CSrchUiServiceContainer::UpDateCbaL()
	{
    if( !iListBox )
        {
        return ;
        }
	TInt idx = iListBox->CurrentItemIndex();
	RArray<TBool>& selectedContentTypes ( iGenSrchUi->SelectedContentTypes() );  
	iSearchFieldBox->GetSearchText( iSearchTextPtr );
	if ( iSearchTextPtr.Length() )
        { 
        
	    if ( idx == KAllInDeviceArrayIndex )
	        {
				if( iGenSrchUi->AllContentClassesSelected() )
				{
					iView.SetCbaButtonsL( R_QTN_SOFTKEYS_SEARCH_UNMARK_CANCEL );
				}

	        	else
	        	{
	        		iView.SetCbaButtonsL( R_QTN_SOFTKEYS_SEARCH_MARK_CANCEL );        		
	        	}
	        }
	        else
	        {
				if( selectedContentTypes[idx-1])
				{
					iView.SetCbaButtonsL( R_QTN_SOFTKEYS_SEARCH_UNMARK_CANCEL );
				}

	        	else
	        	{
	        		iView.SetCbaButtonsL( R_QTN_SOFTKEYS_SEARCH_MARK_CANCEL );        		
	        	}
	        }
        }
    else
        {
	    if ( idx == KAllInDeviceArrayIndex )
	        {
				if( iGenSrchUi->AllContentClassesSelected() )
				{
					iView.SetCbaButtonsL( R_QTN_SOFTKEYS_OPTION_UNMARK_EXIT);
				}

	        	else
	        	{
	        		iView.SetCbaButtonsL( R_QTN_SOFTKEYS_OPTION_MARK_EXIT );        		
	        	}
	        }
	        else
	        {
				if( selectedContentTypes[idx-1])
				{
					iView.SetCbaButtonsL( R_QTN_SOFTKEYS_OPTION_UNMARK_EXIT );
				}

	        	else
	        	{
	        		iView.SetCbaButtonsL( R_QTN_SOFTKEYS_OPTION_MARK_EXIT);        		
	        	} 
	        }
        }
    	
	}

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::HandleResourceChange
// Handles a change to the application's resources which are shared across the environment.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::HandleResourceChangeL( TInt aType )
    {
    // Event generated by the FW when the screen size changed/flipped.
    if ( aType == KEikDynamicLayoutVariantSwitch )
    	{
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect(mainPaneRect);
	    }
    UpdateNaviPaneL();
    CCoeControl::HandleResourceChange( aType );
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::FocusChanged
// -------------------------------------------------------------------------------------------------
//
#ifdef RD_INTELLIGENT_TEXT_INPUT
void CSrchUiServiceContainer::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    iSearchFieldBox->SetFocus( IsFocused() );	
    }
#endif

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::HandleEdwinEventL
// when iSearchFieldBox content changed,this funtion  is called.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiServiceContainer::HandleEdwinEventL(CEikEdwin* aEdwin,TEdwinEvent aEventType)
    {
    UpDateCbaL();			
    UpdateNaviPaneL();
    
    if ( !iListBox )
        {
        SetupListBoxL();
        iSearchFieldBox->ResetL();
        }		
	}
