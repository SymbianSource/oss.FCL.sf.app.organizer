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
* Description:     CSrchUiMainContainer class acts as a container for the CSrchUiMainView view. 
*
*/






//System Includes
#include <AknQueryDialog.h> 
#include <srchui.rsg>
#include <eikclbd.h>
#include <AknIconArray.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsControlContext.h> 
#include <AknsBasicBackgroundControlContext.h> 
#include <aknconsts.h>
#include <avkon.mbg>
#include <utf.h>
#include <aknnavilabel.h>
#include <akntitle.h>
#include <gnrlsearchuisvg.mbg> 
#include <data_caging_path_literals.hrh>

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK
//User Includes
#include "srchuimaincontainer.h"
#include "srchuiappuids.h"
#include "srchuidefines.h"
#include "srchuimainview.h"
#include "srchui.hrh"

const TInt KListItemLength = 100;
// ====================================== MEMBER FUNCTIONS =========================================

// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::CSrchUiServiceContainer
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
CSrchUiMainContainer::CSrchUiMainContainer( CSrchUiMainView& aView,CSrchUiGeneral* aGenSrchUi )
:	iView(aView),
    iSearchTextPtr( NULL,0 ),
    iArray(5)
    {
	iGenSrchUi = aGenSrchUi;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainContainer::ConstructL( const TRect& aRect  )
    {
    
    CreateWindowL();
	iSearchText = HBufC::NewL( KMaxSearchKeywordsLength );
	iSearchTextPtr.Set( iSearchText->Des() );

    RPointerArray<TServiceInfo> serviceInfo;
	iView.GetData(serviceInfo);
	
    SetupListBoxL();
	SetRect( aRect );

    ActivateL();
	UpdateNaviPaneL();	
	UpdateTitlePaneL();
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::~CSrchUiServiceContainer
// Destructor.
// -------------------------------------------------------------------------------------------------
CSrchUiMainContainer::~CSrchUiMainContainer()
    {
    	iArray.Reset();
    
    
    // Store listboxes current index so that it can be restored to the same place if needed
	if(iListBox)
		{
		iView.SetContainerListBoxIndex( iListBox->CurrentItemIndex() );
		}
	// Important to set the find box non-focusing before
	// deleting it, otherwise the focus changes triggered
	// by the removal of the control from stack will focus
	// the find box which is under deletion
    
    if(iListBox)
	    {
	    delete iListBox;
	    }

    if(iSearchText)
		{
		delete iSearchText;
		}
	
	if(iNaviDecorator)
		{
		delete iNaviDecorator;
		}
		
		if(iTitleText)
			{
				delete iTitleText;
			}
    }
	
// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::DoSearchL
// Starts off the search.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainContainer::DoSearchL()
    {
    if( iSearchText )
    	{
	    delete iSearchText;
	    iSearchText = NULL;
    	}
    iSearchText = KNullDesC().Alloc();
    
    HBufC8* des = HBufC8::NewLC( KMaxSearchKeywordsLength );
    TPtr8 ptr1 = des->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr1,*iSearchText);
    
    iGenSrchUi->SetSearchStringL( iSearchTextPtr );
    
    TUid serviceId(iView.Id());
    iGenSrchUi->GetSupportedContentClassesL(serviceId);
    
    RArray<TBool>& selectedContentTypes ( iGenSrchUi->SelectedContentTypes() );
    
    for(TInt i = 0; i<selectedContentTypes.Count(); i++)
    {
    	selectedContentTypes[i] = ETrue;
    }
    iGenSrchUi->SetAllContentClassesSelected(ETrue);
    iGenSrchUi->HandleSelectionL(*des, serviceId);
    CleanupStack::PopAndDestroy( des );
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::CurrentItemIndex
// Getter for index of the highlighted item in list box
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiMainContainer::CurrentItemIndex()
    {
    return iListBox->CurrentItemIndex() ;
    }
    	
// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::UpdateNaviPaneL
// Handles changes to the navigation pane text.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainContainer::UpdateNaviPaneL()
    {
	CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* np= 
		(CAknNavigationControlContainer *)sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi));

    //in buf is the text you want display
    _LIT(kTemp, "ABC");
    TBuf<5> buffer;
    buffer.Copy(kTemp);
    if( iNaviDecorator ) 
        {
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }
    iNaviDecorator = np->CreateNavigationLabelL(buffer);
    np->PushL(*iNaviDecorator);

    CAknNaviLabel* naviLabel = (CAknNaviLabel*)iNaviDecorator->DecoratedControl();

    naviLabel->SetTextL( buffer );
    np->DrawNow();
    naviLabel->DrawNow();
    np->Pop(); //pop naviDecorator
 
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::OfferKeyEventL
// Called by the framework when a key event needs to be processed.
// -------------------------------------------------------------------------------------------------
TKeyResponse CSrchUiMainContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
	TKeyResponse response( EKeyWasNotConsumed );
	response = iListBox->OfferKeyEventL( aKeyEvent, aType );

    // Default one is defined in r_search_view (AVKON_VIEW)
    if( iGenSrchUi->AiwFlag() == TRUE )
	    {
	    iView.SetCbaButtonsL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
	    }
    else
	    {
		iView.SetCbaButtonsL( R_AVKON_SOFTKEYS_OPTIONS_EXIT );
	    }

    return response;
    }


// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::HandlePointerEventL
// Handles pointer events from framework.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiMainContainer::HandlePointerEventL(const TPointerEvent &aPointerEvent)
	{
	// Check if touch is enabled or not
    if( !AknLayoutUtils::PenEnabled() )
        {
        return;
        }
    
    TInt index = iListBox->CurrentItemIndex();
    if(index < 0)
    	{
    		return;
    	}
    	
    TInt indexPos = KErrNotFound; 
    iListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition,indexPos);
    if( index != KErrNotFound && indexPos != KErrNotFound )
        {
        switch( aPointerEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            {
                    
                if( iListBox->CurrentItemIndex() == index)
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
                HandleItemSelectionL(); 
                }
            }
            break;
        case TPointerEvent::EDrag:
            {
                iListBox->HandlePointerEventL(aPointerEvent);
                if( iTouchFlag  &&  iListBox->CurrentItemIndex() != index )
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
           iListBox->HandlePointerEventL(aPointerEvent);
            if( !iListBox->Model()->ItemTextArray()->MdcaCount() )
            {
               HandleItemSelectionL(); 
            }  
        }  
     }


// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::SizeChanged
// Called by the framework when the view size is changed.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainContainer::SizeChanged()
    {
    const TRect rect( Rect() );
    if ( iListBox )
	    {
	    // the list rectangle takes the top left of the screen as (0,0),
	    //that would mean that we have to give the co-ordintes relative to top left corner of the screen      
	    TRect listRect(rect.iTl.iX, rect.iTl.iY, 
	    		  rect.Width(), rect.Height() );    
	    
	    iListBox->SetRect(listRect);    
	    }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::CountComponentControls
// Gets the number of controls contained in a compound control.
// -------------------------------------------------------------------------------------------------
TInt CSrchUiMainContainer::CountComponentControls() const
    {
    return 1;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::ComponentControl
// Gets the specified component of a compound control.
// -------------------------------------------------------------------------------------------------
CCoeControl* CSrchUiMainContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListBox;

        default:
            return NULL;
        }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::HandleResourceChange
// Handles a change to the application's resources which are shared across the environment.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainContainer::HandleResourceChange( TInt aType )
    {
    TRAPD ( ignore, HandleResourceChangeL( aType ) );
    }
	
// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::HandleResourceChange
// Handles a change to the application's resources which are shared across the environment.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainContainer::HandleResourceChangeL( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );

    // Event generated by the FW when the screen size changed/flipped.
    if ( aType == KEikDynamicLayoutVariantSwitch )
	    {
        TRect rect;
		AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        SetRect( rect );
        }
    UpdateNaviPaneL(); 
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::HandleListBoxEventL
// Called by the framework when a list box event needs to be processed.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    if ( aEventType == MEikListBoxObserver::EEventEnterKeyPressed )
        {
        HandleItemSelectionL();
        }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::SetupListBoxL
// Constructs and prepares the list box.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainContainer::SetupListBoxL()
    {
    delete iListBox;
    iListBox = NULL;
	
	iListBox = new( ELeave )CAknDoubleGraphicStyleListBox;
	iListBox->SetContainerWindowL( *this );
    iListBox->SetListBoxObserver( this );				// HandleListBoxEventL gets more events
	iListBox->ConstructL( this, EAknListBoxLoopScrolling );

	ConstructListTextL();

	const TRect rect( Rect() );    
    // the list rectangle takes the top left of the screen as (0,0),
    //that would mean that we have to give the co-ordintes relative to top left corner of the screen      
    TRect listRect(rect.iTl.iX, rect.iTl.iY, 
    				rect.Width(), rect.Height() );    

    iListBox->SetRect(listRect);    
	iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()
              ->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
	iListBox->Model()->SetItemTextArray( &iArray );
	iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    // iListBox takes ownership of the icons array
	iListBox->ItemDrawer()->ColumnData()->SetIconArray( ListBoxIconsArrayL() );
    iListBox->ActivateL();
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::HandleItemSelectionL
// Handles the selection of the currently active item.
// Called from HandleListBoxEventL.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainContainer::HandleItemSelectionL()
    {
    iView.HandleCommandL( ESearchCmdOpen );
    iListBox->DrawDeferred();
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::ListBoxIconsArrayL
// Returns an array of icons for use in the listbox.
// -------------------------------------------------------------------------------------------------
CArrayPtr<CGulIcon>* CSrchUiMainContainer::ListBoxIconsArrayL()
    {
    
    RPointerArray<TServiceInfo> aInfo;
    iView.GetData(aInfo);
    
    CArrayPtr<CGulIcon>* icons = new( ELeave ) CAknIconArray( aInfo.Count() );
    CleanupStack::PushL( icons );
    
    CFbsBitmap* serviceIcon = NULL;
	CFbsBitmap* serviceIconMask = NULL;
	
	for(TInt i = 0; i<aInfo.Count(); i++)
        {
        serviceIcon = aInfo[i]->iServiceIcon;
        CleanupStack::PushL( serviceIcon );
        serviceIconMask = aInfo[i]->iServiceIconMask;
        CleanupStack::PushL( serviceIconMask );
        
        CGulIcon* gulIcon = CGulIcon::NewL(serviceIcon, serviceIconMask); // icon for the list box
        gulIcon->SetBitmapsOwnedExternally(ETrue);
        CleanupStack::PushL( gulIcon );
	    icons->AppendL( gulIcon );
	    CleanupStack::Pop( gulIcon);
	    
	    CleanupStack::Pop( serviceIconMask);
	    
	    CleanupStack::Pop( serviceIcon);
        	
        }
	CleanupStack::Pop( icons );
	return icons;
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiMainContainer::ConstructListText
// Constructs and prepares the List box.
// -------------------------------------------------------------------------------------------------
void CSrchUiMainContainer::ConstructListTextL()
    {
    RPointerArray<TServiceInfo> aInfo;
    iView.GetData(aInfo);
    for(TInt i = 0; i<aInfo.Count(); i++)
        {
        TBuf<KListItemLength> listText;
        HBufC* cap = aInfo[i]->iPrimaryCaption;
        HBufC* brandText = aInfo[i]->iSecondaryCaption;
        listText.Format(KListTextFormat,i,cap,brandText); 
        iArray.InsertL(i,listText);
        }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiServiceContainer::UpdateTitlePaneL
// Handles changes to the title pane text.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiMainContainer::UpdateTitlePaneL()
	{
	
	  iTitleText = CCoeEnv::Static()->AllocReadResourceAsDes16L(
                                R_QTN_TITLE_SEARCH);

	    const CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
	   CAknTitlePane* titlePane = STATIC_CAST(CAknTitlePane*, 
									sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

       // different
       titlePane->SetTextL( iTitleText->Des() );
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

// End of File
