/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This is the source file for the CClockWorldContainer class.
*
*/

// System includes
#include <AknsBasicBackgroundControlContext.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <csxhelp/clk.hlp.hrh>
#include <AknDef.h>
#include <aknlists.h>
#include <AknIconArray.h>
#include <avkon.mbg>
#include <gulicon.h>
#include <StringLoader.h>
#include <AknsConstants.h>
#include <clock.rsg>
#include <clockapp.mbg>

// User includes
#include "clockworldcontainer.h"
#include "clockworldview.h"
#include "clockworldarray.h"
#include "clock.hrh"
#include "clockappui.h"
#include "clock_debug.h"
#include "clockserverclt.h"
#include "clock.h"

// Constants
const TInt KNumOfIcons( 19 );
const TInt KControlListBox( 1 );
const TInt KHomeCityImageIndex( 0 );
const TInt KDefaultImageIndex( 1 );
const TInt KDstIconIndex( 2 );
const TInt KBlankIcon( 3 );
const TInt KImageStartIndex( 4 );

// Literals
_LIT( KHomeCityIconFile, "\\resource\\apps\\clockapp.mbm" );

// ---------------------------------------------------------
// CClockWorldContainer::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldContainer* CClockWorldContainer::NewL( CClockWorldView* aView,
												  const TRect& aRect,
												  CClockWorldArray* aWorldArray )
	{
	__PRINTS( "CClockWorldContainer::NewL - Entry" );
	
	CClockWorldContainer* self = new( ELeave ) CClockWorldContainer;
	CleanupStack::PushL( self );
	
	self->ConstructL( aView, aRect, aWorldArray );
	
	CleanupStack::Pop( self );
	
	__PRINTS( "CClockWorldContainer::NewL - Exit" );
	
	return self;
	}

// ---------------------------------------------------------
// CClockWorldContainer::~CClockWorldContainer
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockWorldContainer::~CClockWorldContainer()
    {
    __PRINTS( "CClockWorldContainer::~CClockWorldContainer - Entry" );
    
	if( iBgContext )
	    {
	    delete iBgContext;
	    iBgContext = NULL;
	    }
	if( iListBox )
	    {
	    delete iListBox;
	    iListBox = NULL;
	    }
	    
	__PRINTS( "CClockWorldContainer::~CClockWorldContainer - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldContainer::OfferKeyEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
TKeyResponse CClockWorldContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                   TEventCode aType )
    {
    __PRINTS( "CClockWorldContainer::OfferKeyEventL - Entry" );
    
    TKeyResponse returnVal( EKeyWasNotConsumed );

    if( EEventKey == aType )
        {
        if( EStdKeyNo == aKeyEvent.iScanCode )
            {
            // This indicates that Call End key was pressed.
            // The application is hidden in the background.
            iView->ClockApplicationUi()->HandleCommandL( EClockWorldExit );

            returnVal = EKeyWasConsumed;

            __PRINTS( "CClockWorldContainer::OfferKeyEventL - Exit" );

            return returnVal;
            }
			
		// Check if automatic-time update is On.
		RClkSrvInterface clockServerClt;
		// Connect to clockserver
		TInt errorVal( clockServerClt.Connect() );
		// Check if timeupdate is On.
		TBool timeUpdateOn( EFalse );
		if( KErrNone == errorVal )
			{
			errorVal = clockServerClt.IsAutoTimeUpdateOn( timeUpdateOn );
			}
		// Close the session with the server.
		clockServerClt.Close();
        
        if( EKeyUpArrow == aKeyEvent.iCode ||
            EKeyDownArrow == aKeyEvent.iCode )
            {
            if( iListBox )
                {
                returnVal = iListBox->OfferKeyEventL( aKeyEvent, aType );
		        }
            }

        if( EKeyBackspace == aKeyEvent.iCode ||
            EKeyDelete == aKeyEvent.iCode )
            {
            if( KZerothIndex != iListBox->CurrentItemIndex() || timeUpdateOn )
            	{
				//Single click integration
                if ( iView->MenuBar()->ItemSpecificCommandsEnabled() )
                    {
                    // if highlighted
                    iView->HandleCommandL( EClockWorldRemoveRegion );
                    }
                // originally key was consumed anyway

            	}
			returnVal = EKeyWasConsumed;
            }

        if( EKeyOK == aKeyEvent.iCode || EKeyEnter == aKeyEvent.iCode )
            {
			//Single click integration
            return iListBox->OfferKeyEventL( aKeyEvent, aType ); 

            }
        }
    
    __PRINTS( "CClockWorldContainer::OfferKeyEventL - Exit" );
    
    return returnVal;	
    }

// ---------------------------------------------------------
// CClockWorldContainer::Draw
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldContainer::Draw( const TRect& aRect ) const
    {
    __PRINTS( "CClockWorldContainer::Draw - Entry" );
    
    CWindowGc&  gc = SystemGc();
    gc.Clear( aRect );
    
    if( iBgContext )
        {
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        AknsDrawUtils::Background( skinInstance, iBgContext, this, gc, aRect );
        }
    
    __PRINTS( "CClockWorldContainer::Draw - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldContainer::FocusChanged
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldContainer::FocusChanged( TDrawNow /*aDrawNow*/ )
    {
    __PRINTS( "CClockWorldContainer::FocusChanged - Entry" );
    
	if( iListBox )
		{
		iListBox->SetFocus( IsFocused() );
		}
	
	__PRINTS( "CClockWorldContainer::FocusChanged - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldContainer::HandlePointerEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    __PRINTS( "CClockWorldContainer::HandlePointerEventL - Entry" );
    
    // Check if touch is enabled or not.
    if( !AknLayoutUtils::PenEnabled() )
        {
        __PRINTS( "CClockWorldContainer::HandlePointerEventL - Exit" );
        
        return;
        }
    
   

    // Let the lisbox handle the events.
    iListBox->HandlePointerEventL( aPointerEvent );
    
    __PRINTS( "CClockWorldContainer::HandlePointerEventL - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldContainer::HandleListBoxEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldContainer::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent/* aEventType*/ )
    {
    __PRINTS( "CClockWorldContainer::HandleListBoxEventL - Entry" );

/*    if( aListBox == iListBox &&
		//Single click integration
        aEventType == EEventItemSingleClicked )

        {
		//Single click integration
        iView->HandleCommandL( EClockWorldMyRegion ); 
        }
	//Single click integration
    else if ( aListBox == iListBox && aEventType == EEventEnterKeyPressed )
        {
        if( !iView->IsSelectionListOpen() || !iView->IsGalleryOpen() &&
            iListBox->CurrentItemIndex() > 0 ) 
            {
            iView->HandleCommandL( EClockWorldMyRegion ); 
            }
        }*/

        
    __PRINTS( "CClockWorldContainer::HandleListBoxEventL - Entry" );
    }

// ---------------------------------------------------------
// CClockWorldContainer::ListBox
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CAknDoubleLargeStyleListBox* CClockWorldContainer::ListBox()
	{
	__PRINTS( "CClockWorldContainer::ListBox - Entry" );
	
	__PRINTS( "CClockWorldContainer::ListBox - Exit" );
	
	return iListBox;
	}

// ---------------------------------------------------------
// CClockWorldContainer::SizeChanged
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldContainer::SizeChanged()
    {
    __PRINTS( "CClockWorldContainer::SizeChanged - Entry" );
    
    TRect paneRect( this->Rect() );
    // Update the background context.
    iBgContext->SetRect( paneRect );
	// Update the listbox.
    // Set the top item index so that the list is updated correctly.
    iListBox->SetTopItemIndex( KZerothIndex );
	iListBox->SetExtent( this->Rect().iTl, this->Rect().Size() );
    
	__PRINTS( "CClockWorldContainer::SizeChanged - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldContainer::CountComponentControls
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockWorldContainer::CountComponentControls() const
    {
    __PRINTS( "CClockWorldContainer::CountComponentControls - Entry" );
    
    __PRINTS( "CClockWorldContainer::CountComponentControls - Exit" );
    
    return KControlListBox;
    }

// ---------------------------------------------------------
// CClockWorldContainer::ComponentControl
// rest of the details are commented in the header
// ---------------------------------------------------------
//  
CCoeControl* CClockWorldContainer::ComponentControl( TInt aIndex ) const
    {
    __PRINTS( "CClockWorldContainer::ComponentControl - Entry" );
    
	CCoeControl* coeControl( NULL );
    switch( aIndex )
        {
		case KErrNone:
			{
			coeControl = iListBox;
			}
			break;

        default:
            {
            // No implementation yet.
            }
            break;
        }
    
    __PRINTS( "CClockWorldContainer::ComponentControl - Exit" );
    
	return coeControl;
    }

// ---------------------------------------------------------
// CClockWorldContainer::MopSupplyObject
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TTypeUid::Ptr CClockWorldContainer::MopSupplyObject( TTypeUid aId )
    {
    __PRINTS( "CClockWorldContainer::MopSupplyObject - Entry" );
    
    if( MAknsControlContext::ETypeId == aId.iUid )
        {
        __PRINTS( "CClockWorldContainer::MopSupplyObject - Exit" );
        
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    
    __PRINTS( "CClockWorldContainer::MopSupplyObject - Exit" );
    
    return CCoeControl::MopSupplyObject( aId );
    }

// ---------------------------------------------------------
// CClockWorldContainer::HandleResourceChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockWorldContainer::HandleResourceChange( TInt aType )
    {
    __PRINTS( "CClockWorldContainer::HandleResourceChange - Entry" );
    
    CCoeControl::HandleResourceChange( aType );
    // Update the listbox.
    iListBox->HandleResourceChange( aType );
    
    switch( aType )
        {
        case KEikDynamicLayoutVariantSwitch:
            {
            SetRect( iView->ClientRect() );
            }
            break;
            
        case KAknsMessageSkinChange:
            {
            // First get the icon array from listbox item drawer.
            CArrayPtr< CGulIcon >* iconArray = iListBox->ItemDrawer()->FormattedCellData()->IconArray();
            CleanupStack::PushL( iconArray );                     
            
            // Delete the first three the icons from the array.
            iconArray->Delete( KHomeCityImageIndex, KImageStartIndex );	
          
            CFbsBitmap* iconBitmap( NULL );
            CFbsBitmap* iconMask( NULL );
            CGulIcon* finalIcon( NULL );
            
            // Construct the home city image.
            AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                                    KAknsIIDQgnPropClockHomecity,
                                    iconBitmap,
                                    iconMask,
                                    KHomeCityIconFile,
                                    EMbmClockappQgn_prop_clock_homecity,
                                    EMbmClockappQgn_prop_clock_homecity_mask );
            
            CleanupStack::PushL( iconBitmap );
            CleanupStack::PushL( iconMask );
            // Construct the icon.
            finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
            CleanupStack::PushL( finalIcon );
            // Append the above icon to the array.
            iconArray->InsertL( KHomeCityImageIndex, finalIcon );
                
            CleanupStack::Pop( finalIcon );
            CleanupStack::Pop( iconMask );
            CleanupStack::Pop( iconBitmap );

            // Construct the default image.
            AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                                    KAknsIIDQgnPropClockDefault,
                                    iconBitmap,
                                    iconMask,
                                    KHomeCityIconFile,
                                    EMbmClockappQgn_prop_clock_default,
                                    EMbmClockappQgn_prop_clock_default_mask );
            
            CleanupStack::PushL( iconBitmap );
            CleanupStack::PushL( iconMask );
            // Construct the icon.
            finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
            CleanupStack::PushL( finalIcon );
            // Append the above icon to the array.
            iconArray->InsertL( KDefaultImageIndex, finalIcon );
                
            CleanupStack::Pop( finalIcon );
            CleanupStack::Pop( iconMask );
            CleanupStack::Pop( iconBitmap );

            // Reset.
            iconBitmap = NULL ;
            iconMask = NULL ;
            finalIcon = NULL;

            // Construct the DST icon.
            AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
					                     KAknsIIDQgnIndiDstAdd,
					                     KAknsIIDQsnIconColors,
					                     EAknsCIQsnIconColorsCG13,
					                     iconBitmap,
					                     iconMask,
					                     AknIconUtils::AvkonIconFileName(),
					                     EMbmAvkonQgn_indi_dst_add,
					                     EMbmAvkonQgn_indi_dst_add_mask,
					                     TRgb( NULL, NULL, 255 ) );
            CleanupStack::PushL( iconBitmap );
            CleanupStack::PushL( iconMask );
            // Construct the icon.
            finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
            CleanupStack::PushL( finalIcon );
            // Append the above icon to the array.
            iconArray->InsertL( KDstIconIndex, finalIcon );

            CleanupStack::Pop( finalIcon );
            CleanupStack::Pop( iconMask );
            CleanupStack::Pop( iconBitmap );

            // Reset.
            iconBitmap = NULL ;
            iconMask = NULL ;
            finalIcon = NULL;

            // Now load a blank icon.
            iconBitmap = new( ELeave ) CFbsBitmap();
            CleanupStack::PushL( iconBitmap );
            iconMask = new( ELeave ) CFbsBitmap();
            CleanupStack::PushL( iconMask );

            iconBitmap->Create( TSize( NULL, NULL ), ENone );
            iconMask->Create( TSize( NULL, NULL ), ENone );

            // Construct the icon.
            finalIcon = NULL;
            finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
            CleanupStack::PushL( finalIcon );
            // Append it to the icon array.
            iconArray->InsertL( KBlankIcon, finalIcon );
            // Cleanup.
            CleanupStack::Pop( finalIcon );
            CleanupStack::Pop( iconMask );
            CleanupStack::Pop( iconBitmap );

            // Set the above iconarray as the icon holder for the listbox.
            iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( iconArray );
            CleanupStack::Pop( iconArray );
            }
            break;
            
        default:
        	{
        	// No implementation yet.
        	}
        	break;
        }
    
    __PRINTS( "CClockWorldContainer::HandleResourceChange - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldContainer::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
void CClockWorldContainer::ConstructL( CClockWorldView* aView,
									   const TRect& aRect,
									   CClockWorldArray* aWorldArray )
    {
    __PRINTS( "CClockWorldContainer::ConstructL - Entry" );
    
	// The view owning the container.
    iView = aView;
    iWorldArray = aWorldArray;
        
	CreateWindowL();
    
	// Construct the basic skin context.
	iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,
	                                                       aRect,
	                                                       EFalse );

	// Let us construct the listbox and prepare it for displaying.
	iListBox = new ( ELeave ) CAknDoubleLargeStyleListBox;
    iListBox->ConstructL( this, NULL );
    // Set the container control.
    iListBox->SetContainerWindowL( *this );
    // Add scrollbars to listbox
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
														 CEikScrollBarFrame::EAuto );
   	// Set items to the model and the ownership.
	iListBox->Model()->SetItemTextArray( aWorldArray );
	iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
	// Enable Marquee for the list.
	iListBox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );
	// Set the listbox event observer.
	iListBox->SetListBoxObserver( this );
	
	// Construct icon array with granularity 3. 1 for the image, 1 for the DST icon and 1 for blank.
	CArrayPtr< CGulIcon >* iconArray = new ( ELeave ) CAknIconArray( KNumOfIcons );
	CleanupStack::PushL( iconArray );
	
	// These hold the icons and their corresponding masks.
	CFbsBitmap* iconBitmap( NULL );
	CFbsBitmap* iconMask( NULL );
	CGulIcon* finalIcon( NULL );
	
	// Construct the home city image.
    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                            KAknsIIDQgnPropClockHomecity,
                            iconBitmap,
                            iconMask,
                            KHomeCityIconFile,
                            EMbmClockappQgn_prop_clock_homecity,
                            EMbmClockappQgn_prop_clock_homecity_mask );
    
    CleanupStack::PushL( iconBitmap );
    CleanupStack::PushL( iconMask );
    // Construct the icon.
    finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
    CleanupStack::PushL( finalIcon );
    // Append the above icon to the array.
    iconArray->InsertL( KHomeCityImageIndex, finalIcon );
        
    CleanupStack::Pop( finalIcon );
    CleanupStack::Pop( iconMask );
    CleanupStack::Pop( iconBitmap );

	// Construct the default image.
    AknsUtils::CreateIconL( AknsUtils::SkinInstance(),
                            KAknsIIDQgnPropClockDefault,
                            iconBitmap,
                            iconMask,
                            KHomeCityIconFile,
                            EMbmClockappQgn_prop_clock_default,
                            EMbmClockappQgn_prop_clock_default_mask );
    
    CleanupStack::PushL( iconBitmap );
	CleanupStack::PushL( iconMask );
	// Construct the icon.
	finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
	CleanupStack::PushL( finalIcon );
	// Append the above icon to the array.
	iconArray->AppendL( finalIcon );
	
	CleanupStack::Pop( finalIcon );
	CleanupStack::Pop( iconMask );
	CleanupStack::Pop( iconBitmap );
	
	// Reset.
	iconBitmap = NULL ;
	iconMask = NULL ;
	finalIcon = NULL;
	
	// Construct the DST icon.
	AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                                 KAknsIIDQgnIndiDstAdd,
                                 KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG13,
                                 iconBitmap,
                                 iconMask,
                                 AknIconUtils::AvkonIconFileName(),
                                 EMbmAvkonQgn_indi_dst_add,
                                 EMbmAvkonQgn_indi_dst_add_mask,
                                 TRgb( NULL, NULL, 255 ) );
	CleanupStack::PushL( iconBitmap );
	CleanupStack::PushL( iconMask );
	// Construct the icon.
	finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
	CleanupStack::PushL( finalIcon );
	// Append the above icon to the array.
	iconArray->AppendL( finalIcon );

	CleanupStack::Pop( finalIcon );
	CleanupStack::Pop( iconMask );
	CleanupStack::Pop( iconBitmap );

	// Reset.
	iconBitmap = NULL ;
	iconMask = NULL ;
	finalIcon = NULL;
	
	// Now load a blank icon.
	iconBitmap = new( ELeave ) CFbsBitmap();
	CleanupStack::PushL( iconBitmap );
	iconMask = new( ELeave ) CFbsBitmap();
	CleanupStack::PushL( iconMask );
	
	iconBitmap->Create( TSize( NULL, NULL ), ENone );
	iconMask->Create( TSize( NULL, NULL ), ENone );
	
	// Construct the icon.
	finalIcon = NULL;
	finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
	CleanupStack::PushL( finalIcon );
	// Append it to the icon array.
	iconArray->AppendL( finalIcon );
	// Cleanup.
	CleanupStack::Pop( finalIcon );
	CleanupStack::Pop( iconMask );
	CleanupStack::Pop( iconBitmap );

	// Set the above iconarray as the icon holder for the listbox.
	iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( iconArray );
    CleanupStack::Pop( iconArray );
    
    RClkSrvInterface clkSrvInterface;
    User::LeaveIfError( clkSrvInterface.Connect() );
    TBool timeUpdateOn( EFalse );
    // Get the state of the plugin.
    clkSrvInterface.IsAutoTimeUpdateOn( timeUpdateOn );
    clkSrvInterface.Close();

    
	
	// Display no locations only when Automatic time update is on and there are no cities added.
	if( timeUpdateOn && ( KErrNone == iWorldArray->MdcaCount() ) )
	    {
	    HBufC* noLocationBuf = StringLoader::LoadLC( R_WRLD_CLK_EMPTY_VIEW, iCoeEnv );
	    iListBox->View()->SetListEmptyTextL( noLocationBuf->Des() );
	    CleanupStack::PopAndDestroy( noLocationBuf );
	    }
	        
    SetRect( aRect );
    iListBox->ActivateL();
    
    __PRINTS( "CClockWorldContainer::ConstructL - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldContainer::CClockMainContainer
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
CClockWorldContainer::CClockWorldContainer()
    {
    __PRINTS( "CClockWorldContainer::CClockWorldContainer - Entry" );
    
    // No implementation yet.
    
    __PRINTS( "CClockWorldContainer::CClockWorldContainer - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldContainer::UpdateImageL
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
void CClockWorldContainer::UpdateImageL( TPtrC aFileName, TInt aIndex )
    {
	__PRINTS( "CClockWorldContainer::UpdateImage - Entry" );
    
    TInt imageIndex ( KErrNotFound );
             
    // Get the stored city information for the selected list item.
    TCityInfo cityInfo( iWorldArray->GetCity( aIndex ) );
    TInt oldImageIndex( iWorldArray->GetImageIndex( aIndex ) );
    
    if( aFileName.Compare( cityInfo.iImagePath ) ) 
        {
        // Check whether the selected image is loaded by any of the list items.
        imageIndex = iWorldArray->CheckIfImageLoaded( aFileName, aIndex );    
        }
    else
        {
        // User has chosen the same image as loaded for the listitem.
        imageIndex = oldImageIndex;
        }
    
    // In case the selected image is not found,get a new thumbnail. 	
    if( KErrNotFound == imageIndex )
        {
        iWorldArray->GetThumbnailL( aFileName, aIndex );
        imageIndex = oldImageIndex;
        if( KDefaultImageIndex != oldImageIndex )
            {
            iOldImagePath = HBufC::NewL( KImagePathLength );
            iOldImagePath->Des().Copy(cityInfo.iImagePath );
                                  
            }
        }
    
    // Update the new image index and path for the selected list item.
    iWorldArray->UpdateImageInListItem( imageIndex, aIndex, aFileName );
	
    
    __PRINTS( "CClockWorldContainer::UpdateImage - Exit" );
    }

// ---------------------------------------------------------
// CClockWorldContainer::RedrawItem
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
TBool CClockWorldContainer::RedrawItem( TInt aIndex )
    {
    __PRINTS( "CClockWorldContainer::RedrawItem - Entry" );
    
    TBool retValue( EFalse );
 
    // Check whether the current list index is less than the total list count.
    if( aIndex < iWorldArray->MdcaCount() )
        {    	
        TCityInfo city ( iWorldArray->GetCity( aIndex ) );
	   
	    // Check whether the image loaded is not a default image
	    if( city.iImagePath.Compare( KDefaultImagePath ) )
	        {       
            // Add the thumbnail icon and redraw the item
	        LoadThumbnailL( city.iThumbnailBitmap ,aIndex  );              	        
	        }
	    retValue = ETrue;
	    }
    
    __PRINTS( "CClockWorldContainer::RedrawItem - Exit" ); 
	
	return retValue;
	}

// ---------------------------------------------------------
// CClockWorldContainer::RemoveImage
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
void CClockWorldContainer::RemoveImage( TPtrC aFileName, TInt aImageIndex, TInt aListIndex )
    {
    __PRINTS( "CClockWorldContainer::RemoveImage - Entry" );
    
    // Check whether  the selected image is not the default image.
    // In case of a match the default image is not be deleted     	
    if( aFileName.Compare( KDefaultImagePath ) )
		{  	
		TInt loadedImageIndex ( KErrNotFound );

		// Check whether the selected image is loaded by any of the listitems.
		loadedImageIndex = iWorldArray->CheckIfImageLoaded( aFileName, aListIndex );

		// If image is not loaded then delete the image from the iconArray.
		if( ( KErrNotFound == loadedImageIndex ) && ( KImageStartIndex <= aImageIndex ) )
			{
			CArrayPtr< CGulIcon >* iconArray = iListBox->ItemDrawer()->FormattedCellData()->IconArray();
			CGulIcon* tempIcon ( iconArray->At( aImageIndex ) );

			// Decrement the image index of the lisitems by one
			// which have image index greater than aImageIndex in the iconArray
			// The reason is that delete resizes the iconArray.
			for( TInt item( FALSE ); item < iWorldArray->MdcaCount(); item++ )
				{
				TInt itemImageIndex ( iWorldArray->GetImageIndex( item ) );

				if( itemImageIndex > aImageIndex )
					{
					TCityInfo cityInfo = iWorldArray->GetCity( item ); 
					iWorldArray->UpdateImageInListItem( itemImageIndex - 1, item, cityInfo.iImagePath );	
					}
				}
             
			// Delete thumbnail for the given item.
			iWorldArray->DeleteThumbnailL( aFileName );
			
			// Delete the icon from the iconArray.
			iconArray->Delete( aImageIndex );
			iconArray->Compress();
			delete tempIcon;
			tempIcon = NULL;
			iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( iconArray );	
			}
		}

    __PRINTS( "CClockWorldContainer::RemoveImage - Exit" );
    }

// ----------------------------------------------------------------------------
// CClockWorldContainer::LoadThumbnailL
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CClockWorldContainer::LoadThumbnailL( CFbsBitmap* aBitmap, TInt aIndex )
    {
    TInt imageIndex ( KErrNotFound );
    TCityInfo city ( iWorldArray->GetCity( aIndex ) );
        
    // Check if the request for fetching thumbnail is complete.
    if(( KErrNotFound == city.iThumbnailRequestId ))
         {
         // Create an icon for the set thumbnail bitmap.
         if( aBitmap )
             {
             // Check whether the selected image is loaded by any of the list items.
             imageIndex = iWorldArray->CheckIfImageLoaded( city.iImagePath, aIndex );

             if( KImageStartIndex > imageIndex )        
                 { 
                 CArrayPtr< CGulIcon >* iconArray = iListBox->ItemDrawer()->FormattedCellData()->IconArray();
                 CleanupStack::PushL( iconArray );
            
                 CGulIcon* newIcon = CGulIcon::NewL( aBitmap );
                 CleanupStack::PushL( newIcon );
                 iconArray->AppendL( newIcon );
              
                 // Set the above iconarray as the icon holder for the listbox.
                 iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( iconArray );
                 imageIndex = iconArray->Count() - 1;
                
                 // Cleanup.
                 CleanupStack::Pop( newIcon );
                 CleanupStack::Pop( iconArray );
                 }            
                           
             TInt oldImageIndex( iWorldArray->GetImageIndex( aIndex ) );
             iWorldArray->UpdateImageInListItem( imageIndex, aIndex, city.iImagePath );
            
             // Check if the old image index is not the default image index and then remove the old image.
             if(( KDefaultImageIndex != oldImageIndex ) && ( iOldImagePath ) )
                  {
                  // Delete the old image if it is not used by any of the list items.
                  RemoveImage( iOldImagePath->Des(), oldImageIndex, aIndex );  
                  delete iOldImagePath;
                  iOldImagePath = NULL;             
                  }                                
             }
         else
             {
             // In case bitmap does not exist then load the default image.
             TPtrC defaultFilePath( KDefaultImagePath );
               
             // Set to the default image in case thumbnail is not found or is corrupt.
             imageIndex = KDefaultImageIndex;       
             iWorldArray->UpdateImageInListItem( imageIndex, aIndex, defaultFilePath );            
             }
         iListBox->RedrawItem( aIndex );
         }        
    }

// ---------------------------------------------------------
// CClockWorldContainer::Refresh
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClockWorldContainer::Refresh()
    {
    __PRINTS( "CClockWorldContainer::Refresh - Entry" );
    
    if ( iWorldArray )
        {
        iWorldArray->UpdateTimeEntryList();
        }
    DrawNow();
    
    __PRINTS( "CClockWorldContainer::Refresh - Exit" );
    }
	
// ----------------------------------------------------------------------------
// CClockWorldContainer::GetHelpContext
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CClockWorldContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    __PRINTS( "CClockWorldContainer::GetHelpContext - Entry" );

    aContext.iMajor = KClockAppUid;

    // This is specified in HRH file.
    aContext.iContext = KCLK_HLP_WRLD_CLK;

    __PRINTS( "CClockWorldContainer::GetHelpContext - Exit" );
    }

// End of file
