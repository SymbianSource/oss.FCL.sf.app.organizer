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
* Description:      CSearchSettingsContainer implementation
*
*/






// INCLUDE FILES
#include <appmngrgspluginrsc.rsg>
#include <aknlists.h>
#include <eikfutil.h>
#include <stringloader.h>
#include <csxhelp/am.hlp.hrh>
#include <centralrepository.h>
#include <akniconarray.h>
#include <featmgr.h>
#include <swinstallerinternalcrkeys.h>
#include <data_caging_path_literals.hrh>
#include <aknsutils.h>
#include <appmngr.mbg>
#include "SearchSettingsContainer.h"
 
// LOCAL CONSTANTS AND MACROS


const TInt KGranularity = 1;
_LIT(KAppMngrIconFileName, "AppMngr.mbm");
_LIT(KAppMngrIconFileNameMif, "AppMngr.mif");
_LIT(KZDrive, "z:");

_LIT(KListTextFormat, "\t%S");
const TInt KMaxCharsForSettingsString =              30;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CAppMngrContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSearchSettingsContainer* CSearchSettingsContainer::NewL( const TRect& /*aRect*/ )
    {
    CSearchSettingsContainer* self = new ( ELeave ) CSearchSettingsContainer();
    CleanupStack::PushL( self );  
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
// 
CSearchSettingsContainer::~CSearchSettingsContainer()
    {
    delete iListbox;
    delete iItems;
    delete iOcspCheckValueArray;
    delete iAllowUntrustedValueArray;    
    }

// ---------------------------------------------------------------------------
// CSearchSettingsContainer::UpdateListBoxContentL
//  
// ---------------------------------------------------------------------------
//
void CSearchSettingsContainer::UpdateListBoxContentL()
    {
    
    if(iListbox)
    {
    	delete iListbox;
    	iListbox = NULL;
    }
    
	iListbox = new( ELeave ) CAknSettingStyleListBox;
	iListbox->SetContainerWindowL( *this );
    iListbox->SetListBoxObserver( this );
	iListbox->ConstructL( this, EAknListBoxLoopScrolling );

	_LIT(KSearchSettingString,"In-Device Search");
    HBufC* cap; // = " IDS" 
    cap = HBufC::NewL(KMaxCharsForSettingsString);
    cap->Des().Copy(KSearchSettingString);
    
    
	TBuf<50> first;	
	TBuf<50> listText;
    TInt i = 0;
    first.Format(*cap);
    listText.Format(KListTextFormat,&first); 
    iArray.InsertL(i,listText);
	const TRect rect( Rect() ); 
    
    // the list rectangle takes the top left of the screen as (0,0),
    //that would mean that we have to give the co-ordintes relative to top left corner of the screen      
    
    TRect listRect(rect.iTl.iX, rect.iTl.iY, 
    				rect.Width(), rect.Height());
    
    iListbox->SetRect(listRect);    
  
	iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()
            ->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    iListbox->Model()->SetItemTextArray( &iArray );// iModel );
	iListbox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );

    iListbox->ActivateL();
	
	iListbox->SetCurrentItemIndex( 0 );
	iListbox->DrawNow();
    }

// ---------------------------------------------------------------------------
//  CSearchSettingsContainer::OfferKeyEventL
//  This handles the scrolling the list by arrows and the clear key and OK key
// ---------------------------------------------------------------------------
//
TKeyResponse CSearchSettingsContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                               TEventCode aType )
	{
    return iListbox->OfferKeyEventL(aKeyEvent, aType);
    }

// ---------------------------------------------------------
// CSearchSettingsContainer::HandleListBoxEventL
// Called by framework when the ListBox event is fired.
// ---------------------------------------------------------
//
void CSearchSettingsContainer::HandleListBoxEventL( CEikListBox* , 
		                          TListBoxEvent aEventType )
{
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
            HandleListBoxSelectionL();
            break;
        default:
           break;
        }
	
}

// ---------------------------------------------------------
// CSearchSettingsContainer::HandleListBoxSelectionL
// Called by framework when the ListBox item is selected.
// ---------------------------------------------------------
//   
void CSearchSettingsContainer::HandleListBoxSelectionL() 
{
	
}
// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSearchSettingsContainer::CSearchSettingsContainer()
: iArray(5)
	{
	}

// ---------------------------------------------------------
// CSearchSettingsContainer::ConstructL
// Symbian OS two phased constructor
// Creates new window and listbox
// ---------------------------------------------------------
//
void CSearchSettingsContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    SetBlank();

    iListbox = new( ELeave ) CAknSettingStyleListBox;

	iListbox->SetContainerWindowL( *this );
	iListbox->SetListBoxObserver( this );
    iListbox->ConstructL( this, EAknListBoxSelectionList );

    // Read the setting items array (contains first lines of the setting items)
    iItems = iCoeEnv->ReadDesC16ArrayResourceL( R_MIDP_GENERAL_SETTING_ARRAY );
    // read the ocsp check value array
    iOcspCheckValueArray = iCoeEnv->ReadDesC16ArrayResourceL( R_MIDP_OCSP_CHECK_VALUE_ARRAY );
    // read the allow untrusted setting value array
    iAllowUntrustedValueArray = iCoeEnv->ReadDesC16ArrayResourceL(R_MIDP_ALLOW_UNTRUSTED_VALUE_ARRAY);

    // set up the listbox
    iListbox->CreateScrollBarFrameL( ETrue );
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOn, 
                                                         CEikScrollBarFrame::EAuto );
    
    iListbox->SetRect( aRect.Size() );
    iListbox->ActivateL();

    SetRect( aRect );
    ActivateL();

    iListbox->SetCurrentItemIndex( 0 );

    LoadIconsL();   
    }

// -----------------------------------------------------------------------------
// CSearchSettingsContainer::LoadIconsL
// -----------------------------------------------------------------------------
//
void CSearchSettingsContainer::LoadIconsL()
    {       
    HBufC* iconFilePath = HBufC::NewL(5/*this is enough for drive etc*/ + 
        KDC_APP_BITMAP_DIR().Length() + KAppMngrIconFileName().Length() );
    CleanupStack::PushL( iconFilePath );        
    
    TPtr ptr = iconFilePath->Des();
    ptr.Append(KZDrive);
    ptr.Append(KDC_APP_BITMAP_DIR);
        
    #ifdef __SCALABLE_ICONS
        ptr.Append(KAppMngrIconFileNameMif);
    #else // !__SCALABLE_ICONS
        ptr.Append(KAppMngrIconFileName);
    #endif //__SCALABLE_ICONS
        
    CArrayPtr<CGulIcon>* icons = new( ELeave ) CAknIconArray( KGranularity );
    CleanupStack::PushL( icons );    
        
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    CGulIcon* icon = AknsUtils::CreateGulIconL( skinInstance,
                                                KAknsIIDQgnIndiSettProtectedAdd,
                                                *iconFilePath,
                                                EMbmAppmngrQgn_indi_sett_protected_add,
	                                            EMbmAppmngrQgn_indi_sett_protected_add_mask );
    CleanupStack::PushL( icon );
    icons->AppendL( icon );

    CleanupStack::Pop( 2 ); // icon, icons
    CleanupStack::PopAndDestroy(); // iconFilePath
    }

// ---------------------------------------------------------
// CSearchSettingsContainer::SizeChanged
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CSearchSettingsContainer::SizeChanged()
    {
    iListbox->SetRect( Rect() );
    }

// ---------------------------------------------------------
// CSearchSettingsContainer::CountComponentControls
// ---------------------------------------------------------
//
TInt CSearchSettingsContainer::CountComponentControls() const
    {
    return iListbox ? 1 : 0;
    }

// ---------------------------------------------------------
// CSearchSettingsContainer::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CSearchSettingsContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iListbox;
        default:
            return NULL;
        }
    }
// ---------------------------------------------------------------------------
// CSearchSettingsContainer::GetHelpContext
// Gives the help context to be displayed
// ---------------------------------------------------------------------------
//
void CSearchSettingsContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    { 
    aContext.iMajor = KUidAppMngr;
    aContext.iContext = KAM_HLP_GENERAL_SETTINGS;    
    }


// ---------------------------------------------------------
// CSearchSettingsContainer::HandleResourceChange
// Handles a resource relative event 
// ---------------------------------------------------------
//
void CSearchSettingsContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );    

    if ( aType == KEikDynamicLayoutVariantSwitch ) //Handle change in layout orientation
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
        SetRect( mainPaneRect );
		DrawNow();
		}
    
    }

// -----------------------------------------------------------------------------
// CSearchSettingsContainer::FocusChanged
// -----------------------------------------------------------------------------
//
void CSearchSettingsContainer::FocusChanged( TDrawNow aDrawNow )
    {
    if ( iListbox )
        {
        iListbox->SetFocus( IsFocused(), aDrawNow );
        }
    }

        
// End of File  
