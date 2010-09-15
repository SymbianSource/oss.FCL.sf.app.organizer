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
* Description:   This is the source file for the CClockCitySelectionListImpl class.
*
*/

// System includes
#include <aknlists.h>
#include <akntitle.h>
#include <eikclbd.h>
#include <clock.rsg>
#include <aknsfld.h>
#include <eikcapc.h>
#include <bautils.h>
#include <hlplch.h>
#include <featmgr.h>
#include <AknUtils.h>
#include <eikmenup.h>
#include <StringLoader.h>
#include <data_caging_path_literals.hrh>
#include <tzlocalizationdatatypes.h>
#include <tzlocalizer.h>
#include <tz.h>
#include <csxhelp/clk.hlp.hrh>
#include <ConeResLoader.h>

// User includes
#include "clockcityselectionlistimpl.h"
#include "clockcityselectionlistpopup.h"
#include "clockcityselectionlist.hrh"
#include "clockworldview.h"
#include "clock_debug.h"
#include "clkcommon.h"

// Constants
const TInt KMaxDataLength( 120 );
const TInt KArrayGranularity( 24 );
const TInt KSingleCity( 1 );

// Literals
_LIT( KResourceFile, "\\resource\\apps\\clock.rsc" );
_LIT( KEmpty, " " );
_LIT( KFieldSeparator, "\t" );

// ---------------------------------------------------------
// CClockCitySelectionListImpl::CClockCitySelectionListImpl
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockCitySelectionListImpl::CClockCitySelectionListImpl() : CAknSelectionListDialog( iSelectionIndex, NULL, NULL )
    {
    __PRINTS( "CClockCitySelectionListImpl::CClockCitySelectionListImpl - Entry" );
    
    iCityDialogOffset = 0;
	iCitySelected = EFalse;
	
	__PRINTS( "CClockCitySelectionListImpl::CClockCitySelectionListImpl - Exit" );
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::~CClockCitySelectionListImpl
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockCitySelectionListImpl::~CClockCitySelectionListImpl()
    {
    __PRINTS( "CClockCitySelectionListImpl::~CClockCitySelectionListImpl - Entry" );
    
    iCoeEnv->DeleteResourceFile( iCityDialogOffset );
    
    __PRINTS( "CClockCitySelectionListImpl::~CClockCitySelectionListImpl - Exit" );
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockCitySelectionListImpl::ConstructL( TCityInfo* aCityInfo )
    {
    __PRINTS( "CClockCitySelectionListImpl::ConstructL - Entry" );
    
    TFileName fileName( KResourceFile );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    TRAP_IGNORE( iCityDialogOffset = iCoeEnv->AddResourceFileL( fileName ) );

    CAknSelectionListDialog::ConstructL( R_CLOCK_CITY_SELECTION_LIST_MENUBAR );
    
    iCityArray = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    
    CTzLocalizer* cTzLocalizer = CTzLocalizer::NewL();
    CleanupStack::PushL( cTzLocalizer );
    
    CTzLocalizedCityGroupArray* cityGroupArray( cTzLocalizer->GetAllCityGroupsL( CTzLocalizer::ETzAlphaNameAscending ) );   
    CleanupStack::PushL( cityGroupArray );
    
    for( TInt index( KErrNone ); index < cityGroupArray->Count(); index++ )
        {
        CTzLocalizedCityGroup& cityGroup( cityGroupArray->At( index ) );
        if( KErrNone != cityGroup.Name().Compare( KEmpty ) )
            {
            TBuf< KMaxDataLength > countryName;         
            countryName.Append( KFieldSeparator );
            countryName.Append( cityGroup.Name() );
            countryName.Append( KFieldSeparator );
            iCityArray->AppendL( countryName );
            }
        }
    iCityInfo = aCityInfo;
    
    // Cleanup
    CleanupStack::PopAndDestroy( cityGroupArray );
    CleanupStack::PopAndDestroy( cTzLocalizer );
    
    __PRINTS( "CClockCitySelectionListImpl::ConstructL - Exit" );
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::DynInitMenuPaneL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockCitySelectionListImpl::DynInitMenuPaneL( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    __PRINTS( "CClockCitySelectionListImpl::DynInitMenuPaneL - Entry" );
    
    // No implementation yet.
    
    __PRINTS( "CClockCitySelectionListImpl::DynInitMenuPaneL - Exit" );
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::HandleResourceChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockCitySelectionListImpl::HandleResourceChange( TInt aType )
    {
    __PRINTS( "CClockCitySelectionListImpl::HandleResourceChange - Entry" );
    
    if( KEikDynamicLayoutVariantSwitch == aType )
        {
        CAknSearchField* findPane( FindBox() );
        if( findPane )
            {
            TBool isVisible( findPane->IsVisible() );
            CAknSelectionListDialog::HandleResourceChange( aType );
            findPane->MakeVisible( isVisible );
            }
        }
    else
        {
        CAknSelectionListDialog::HandleResourceChange( aType );     
        }
    
    __PRINTS( "CClockCitySelectionListImpl::HandleResourceChange - Exit" );
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::ExecuteLD
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockCitySelectionListImpl::ExecuteLD()
    {
    __PRINTS( "CClockCitySelectionListImpl::ExecuteLD - Entry" );
    
    __PRINTS( "CClockCitySelectionListImpl::ExecuteLD - Exit" );
    
    return CAknSelectionListDialog::ExecuteLD( R_CLOCK_CITY_SELECTION_LIST_DIALOG );
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::OKToExit
// rest of the details are commented in the header
// ---------------------------------------------------------
//  
TBool CClockCitySelectionListImpl::OKToExit()
    {
    __PRINTS( "CClockCitySelectionListImpl::OKToExit - Entry" );
    
    __PRINTS( "CClockCitySelectionListImpl::OKToExit - Exit" );
    
    return ETrue;
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::PreLayoutDynInitL
// rest of the details are commented in the header
// ---------------------------------------------------------
// 
void CClockCitySelectionListImpl::PreLayoutDynInitL()
    {
    __PRINTS( "CClockCitySelectionListImpl::PreLayoutDynInitL - Entry" );
    
    CEikStatusPane* statusPane( iEikonEnv->AppUiFactory()->StatusPane() );
    CAknTitlePane* titlePane = STATIC_CAST( CAknTitlePane*, statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    HBufC* titleText( NULL );
    titleText = StringLoader::LoadLC( R_CLOCK_CITY_SELECTION_LIST_COUNTRY_TITLE, iCoeEnv );
    
    titlePane->SetTextL( *titleText );
    titlePane->DrawNow();
    CleanupStack::PopAndDestroy( titleText );

    CAknSingleStyleListBox* listBox = static_cast< CAknSingleStyleListBox* > ( Control( EClockCityListBoxID ) );
        
    // Enable Marquee for the list
    CColumnListBoxData* columnData( listBox->ItemDrawer()->ColumnData() );
    columnData->EnableMarqueeL( ETrue );
    
    User::LeaveIfNull( listBox );

    CTextListBoxModel* listBoxModel( listBox->Model() );
    listBoxModel->SetItemTextArray( iCityArray );

    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    listBox->HandleItemAdditionL();
    listBox->UpdateScrollBarsL();   

    CAknSelectionListDialog::PreLayoutDynInitL();
    
    __PRINTS( "CClockCitySelectionListImpl::PreLayoutDynInitL - Exit" );
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::ProcessCommandL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockCitySelectionListImpl::ProcessCommandL( TInt aCommandId )
    {
    __PRINTS( "CClockCitySelectionListImpl::ProcessCommandL - Entry" );
    
    CAknSelectionListDialog::ProcessCommandL( aCommandId );
    
    __PRINTS( "CClockCitySelectionListImpl::ProcessCommandL - Exit" );
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::OfferKeyEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TKeyResponse CClockCitySelectionListImpl::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    __PRINTS( "CClockCitySelectionListImpl::OfferKeyEventL - Entry" );
    
    if( EEventKey == aType )
        {
        
        if( EStdKeyNo == aKeyEvent.iScanCode )
            {
            // This indicates that Call End key was pressed.
            TryExitL( KClockHideInBackground );
            
            __PRINTS( "CClockCitySelectionListImpl::OfferKeyEventL - Exit" );
            
            return EKeyWasConsumed;
            }
        
        if( EKeyOK == aKeyEvent.iCode || EKeyEnter == aKeyEvent.iCode )
            {
            DoSelectCityL();
            
            __PRINTS( "CClockCitySelectionListImpl::OfferKeyEventL - Exit" );
            
            return EKeyWasConsumed;
            }
        }

    if( EStdKeyEnter != aKeyEvent.iCode && EEventKey == aType )
        {
        CAknSingleStyleListBox* listbox = static_cast< CAknSingleStyleListBox* > ( Control( EClockCityListBoxID ) );
        TInt itemIndex( listbox->CurrentItemIndex() );

        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        if( cba->IsCommandInGroup( EAknSoftkeyCancel ) )
          {
        if( KErrNone > itemIndex )
            {
        cba->SetCommandSetL( R_AVKON_SOFTKEYS_CANCEL );
        cba->DrawDeferred();
            }
        else
            {
        cba->SetCommandSetL( R_AVKON_SOFTKEYS_SELECT_CANCEL );
        cba->DrawDeferred();
            }
          }
        }

    if( ( EStdKeyBackspace == aKeyEvent.iCode || EStdKeyDelete == aKeyEvent.iCode ) ||
        ( EEventKeyUp == aType ) )
        {
        CAknSingleStyleListBox* listbox = static_cast< CAknSingleStyleListBox* > ( Control( EClockCityListBoxID ) );        
        TInt itemIndex( listbox->CurrentItemIndex() );

        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        if( cba->IsCommandInGroup( EAknSoftkeyCancel ) )
            {
        if( 0 > itemIndex )
            {
        cba->SetCommandSetL( R_AVKON_SOFTKEYS_CANCEL );
        cba->DrawDeferred();
            }
        else
            {
        cba->SetCommandSetL( R_AVKON_SOFTKEYS_SELECT_CANCEL );
        cba->DrawDeferred();
            }
            }
        }
    
    __PRINTS( "CClockCitySelectionListImpl::OfferKeyEventL - Exit" );

    return CAknSelectionListDialog::OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::OkToExitL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockCitySelectionListImpl::OkToExitL( TInt aButtonId )
    {
    __PRINTS( "CClockCitySelectionListImpl::OkToExitL - Entry" );
    
    if( EAknSoftkeySelect == aButtonId )
        {
        DoSelectCityL();
        
        __PRINTS( "CClockCitySelectionListImpl::OkToExitL - Exit" );
        
        return ETrue;
        }
    
    __PRINTS( "CClockCitySelectionListImpl::OkToExitL - Exit" );
    
    return CAknSelectionListDialog::OkToExitL( aButtonId );
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::HandleListBoxEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//    
void CClockCitySelectionListImpl::HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType )
    {
    __PRINTS( "CClockCitySelectionListImpl::HandleListBoxEventL - Entry" );
    
    if( !AknLayoutUtils::PenEnabled() || iCitySelected )
        {
        __PRINTS( "CClockCitySelectionListImpl::HandleListBoxEventL - Exit" );
        
        return;
        }
	//Single click integration
    if( EEventItemSingleClicked == aEventType )

        {
        DoSelectCityL( EFalse );
        if( iCitySelected )
            {
            CAknSelectionListDialog::HandleListBoxEventL( aListBox, aEventType );
            }
        }
    
    __PRINTS( "CClockCitySelectionListImpl::HandleListBoxEventL - Exit" );
    }

// ---------------------------------------------------------
// CClockCitySelectionListImpl::DoSelectCityL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockCitySelectionListImpl::DoSelectCityL( TBool aTryExit )
    {
    __PRINTS( "CClockCitySelectionListImpl::DoSelectCityL - Entry" );
    
    // The country listbox.
    CAknSingleStyleListBox* listBox = 
                    static_cast< CAknSingleStyleListBox* > ( Control( EClockCityListBoxID ) );
	
    // The item currently focused item in the list.
    TInt itemIndex( listBox->CurrentItemIndex() );

    // If the index is valid.
    if( KErrNone <= itemIndex ) 
        {
        // Find the item text.
        TPtrC itemText( listBox->Model()->ItemText( itemIndex ) );
        // The country name, in the item focused.
        TInt tabLocation( itemText.Locate( TChar( '\t' ) ) );
        TPtrC countryName( itemText.Mid( tabLocation + 1, itemText.Length() - 2 ) );

        // The tzlocalizer.
        CTzLocalizer* tzLocalizer = CTzLocalizer::NewL();   
        CleanupStack::PushL( tzLocalizer );
        // This group will be the country.
        CTzLocalizedCityGroup* localizedCityGroup( tzLocalizer->FindCityGroupByNameL( countryName ) );
        CleanupStack::PushL( localizedCityGroup );
        // The cities under the group.
        CTzLocalizedCityArray* localizedCityArray( 
                                        tzLocalizer->GetCitiesInGroupL( localizedCityGroup->Id(),
                                                                        CTzLocalizer::ETzAlphaNameAscending ) );
        CleanupStack::PushL( localizedCityArray );
        CTzLocalizedCity* tzLocalizedCity( NULL );
        
        // If there's only one city, then choose the city and return.
        if( KSingleCity == localizedCityArray->Count() )
            {
            iCitySelected = ETrue;
            tzLocalizedCity = &( localizedCityArray->At( KErrNone ) );
            }
        // We've more than one city in the list. Construct a list to display the same.
        else
            {
            CAknSinglePopupMenuStyleListBox* cityListBox = new ( ELeave ) CAknSinglePopupMenuStyleListBox;
            CleanupStack::PushL( cityListBox );
            // Create a new popupList to display the city list for a country.
            CClockCitySelectionListPopup* popupCityList = 
                                        CClockCitySelectionListPopup::NewL( cityListBox,
                                                                            R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                                                                            AknPopupLayouts::EMenuWindow );
            CleanupStack::PushL( popupCityList );
            // Set the properties of the list.
            cityListBox->ConstructL( popupCityList, EAknListBoxSelectionList | EAknListBoxScrollBarSizeExcluded );
            cityListBox->CreateScrollBarFrameL( ETrue );
            cityListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
                                                                    CEikScrollBarFrame::EAuto );
            // Enable Marquee for the list
            cityListBox->ItemDrawer()->FormattedCellData()->EnableMarqueeL( ETrue );	                                                        
                
            // The array for the strings representing the cities.
            CDesCArrayFlat* cityListArray = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
            CleanupStack::PushL( cityListArray );

            for( TInt index( FALSE ); index < localizedCityArray->Count(); index++ )			
                {
                TBuf< KMaxDataLength > cityName;
                cityName.Append( ( localizedCityArray->At( index ) ).Name() );
                cityListArray->AppendL( cityName );
                }
            // Cleanup.
            CleanupStack::Pop( cityListArray );

            CTextListBoxModel* cityModel( cityListBox->Model() );
            cityModel->SetItemTextArray( cityListArray );
            cityModel->SetOwnershipType( ELbmOwnsItemArray );
            
            // Set the title for the citylist popup.
            HBufC* cityTitle = StringLoader::LoadLC( R_CLOCK_CITY_SELECTION_LIST_SELECT_CITY );
            popupCityList->SetTitleL( cityTitle->Des() );
            CleanupStack::PopAndDestroy( cityTitle );	
            popupCityList->EnableFind();
            
            // Hide the menu.
            HideMenu();

            if( popupCityList->ExecuteLD() )
                {
				iCitySelected = ETrue;
                tzLocalizedCity = &( localizedCityArray->At( cityListBox->CurrentItemIndex() ) );
                }
            
            // Cleanup.
            CleanupStack::Pop( popupCityList );
            CleanupStack::PopAndDestroy( cityListBox );
            }

        if( NULL != tzLocalizedCity )
            {
            RTz rTzHandle;
            User::LeaveIfError( rTzHandle.Connect() );
            CleanupClosePushL( rTzHandle );

            iCityInfo->iCountry.Copy( countryName );
            iCityInfo->iCity.Copy( tzLocalizedCity->Name() ); 
            iCityInfo->iTimeZoneId = tzLocalizedCity->TimeZoneId();

            RArray< TInt > timeZoneId;
            RArray< TInt > zoneOffset;

            timeZoneId.Append( iCityInfo->iTimeZoneId );
            rTzHandle.GetOffsetsForTimeZoneIdsL( timeZoneId, zoneOffset );
            CleanupStack::PopAndDestroy( &rTzHandle );

            iCityInfo->iUtcOffset = zoneOffset[ 0 ];

            timeZoneId.Close();
            zoneOffset.Close();

            TInt cityGroupId( tzLocalizedCity->GroupId() );

            CTzLocalizedCityArray* cityArray( tzLocalizer->GetCitiesInGroupL( cityGroupId, CTzLocalizer::ETzUnsorted ) );
            CleanupStack::PushL( cityArray );

            TInt selectedIndex( FALSE );
            for( TInt index( FALSE ); index < cityArray->Count(); index++ )
                {
                if( KErrNone == ( cityArray->At( index ) ).Name().Compare( tzLocalizedCity->Name() ) )
                    {
                    selectedIndex = index;
                    }
                }

            iCityInfo->iCityOffsetInGroup = selectedIndex;
            iCityInfo->iCityGroupId = cityGroupId;       			
            
            // Cleanup.
            CleanupStack::PopAndDestroy( cityArray );


            }
        // Cleanup.
        CleanupStack::PopAndDestroy( localizedCityArray );
        CleanupStack::PopAndDestroy( localizedCityGroup );
        CleanupStack::PopAndDestroy( tzLocalizer );
        }
    
    __PRINTS( "CClockCitySelectionListImpl::DoSelectCityL - Exit" );
    }

// ---------------------------------------------------------
// NewDialog
// This is the function used to instantiate the dialog when calling DLL dynamically.
// ---------------------------------------------------------
//
EXPORT_C MClockCitySelectionList* NewDialog()
    {
    __PRINTS( "NewDialog - Entry" );
    
    MClockCitySelectionList* citySelectionList( NULL );
	TRAP_IGNORE( citySelectionList = new ( ELeave ) CClockCitySelectionListImpl() );
    
	__PRINTS( "NewDialog - Exit" );
	
	return citySelectionList;
    }

// End of file
