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
* Description:   This is the source file for the CClkDateTimeViewPluginInterface class.
*
*/

// System Includes
#include <e32std.h>
#include <implementationproxy.h>
#include <clock.rsg>
#include <StringLoader.h>
#include <bautils.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include <akntitle.h>
#include <clockapp.mbg>

// User includes
#include "clkdatetimeviewinher.h"
#include "clkdatetimeviewplugininterface.h"
#include "clkdatetimeview.h"
// Constants
const TImplementationProxy KDateTimeViewImplementationTable[] = 
	{
    IMPLEMENTATION_PROXY_ENTRY( 0x102818EA,	CClkDateTimeViewPluginInterface::NewL ) // Used by GS plugin.
	};

// Literals
_LIT( KClockIconFileName, "Z:\\resource\\apps\\clockapp.mbm" );

// ---------------------------------------------------------
// ImplementationGroupProxy
// Gate/factory function.
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( KDateTimeViewImplementationTable ) 
                / sizeof( TImplementationProxy );
	return KDateTimeViewImplementationTable;
	}

// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CGSPluginInterface* CClkDateTimeViewPluginInterface::NewL()
    {
	// UID of the plugin.
	TUid pluginId = { 0x102818E9 };
	
	CClkDateTimeViewPluginInterface* self = new( ELeave ) CClkDateTimeViewPluginInterface();
    CleanupStack::PushL( self );

    self->ConstructL( pluginId, ETrue );
	
	CleanupStack::Pop( self );
	return ( CGSPluginInterface* ) self;
	}

// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::Id
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TUid CClkDateTimeViewPluginInterface::Id() const
	{
	// Return the id of the view.
	return iView->Id();
	}

// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClkDateTimeViewPluginInterface::ConstructL( TUid aId, TBool aClockInUse )
	{
	// The resource file used to find the clkdatetimeview resources.
	TFileName fileName( KRESOURCE );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    iOffset = iCoeEnv->AddResourceFileL( fileName );
    
	// Construct the datetimeview.
	BaseConstructL( R_CLK_DATE_TIME_SETT_VIEW );
	
	// We access the datetimeview through this object.
	iView = CClkDateTimeViewInher::NewLC( aId, aClockInUse );	
	CleanupStack::Pop( iView );
	}

// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::HandleCommandL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewPluginInterface::HandleCommandL( TInt aCommandId )
    {
	// Allow the view to handle the command.
    iView->HandleCommandL( aCommandId );
    }

// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::DynInitMenuPaneL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewPluginInterface::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
	iView->DynInitMenuPaneL( aResourceId, aMenuPane );
	}

// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::CClkDateTimeViewPluginInterface
// rest of the details are commented in the header
// ---------------------------------------------------------
//   
CClkDateTimeViewPluginInterface::CClkDateTimeViewPluginInterface()
	{
	// No implementation yet.
	}

// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::~CClkDateTimeViewPluginInterface
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClkDateTimeViewPluginInterface::~CClkDateTimeViewPluginInterface()
	{
	// First delete the view.
	if( iView )
		{
		delete iView;
		iView = NULL;
		}
	// Delete the resource file from the control environment.
	iCoeEnv->DeleteResourceFile( iOffset );
	}

// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::GetCaptionL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewPluginInterface::GetCaptionL( TDes& aCaption ) const
    {
	// This is the string displayed in General Settings for datetimeview.
    StringLoader::Load( aCaption, R_QTN_SET_FOLDER_DATIM );
    }

// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::CreateIconL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CGulIcon* CClkDateTimeViewPluginInterface::CreateIconL( const TUid /*aIconType*/ )
	{
	CFbsBitmap* iconBitmap( NULL );
    CFbsBitmap* iconMask( NULL ); 
 
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();    
    AknsUtils::CreateIconL( skinInstance,
							KAknsIIDQgnPropSetDatimSub,
							iconBitmap,
							iconMask,
    						KClockIconFileName,
    						EMbmClockappQgn_prop_set_datim_sub,
    						EMbmClockappQgn_prop_set_datim_sub_mask );

    CGulIcon* finalIcon = CGulIcon::NewL( iconBitmap, iconMask );
    finalIcon->SetBitmapsOwnedExternally( EFalse );

	// Cleanup.
    iconMask = NULL;
    iconMask = NULL;      
    return finalIcon;    
	}
	
// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::DoActivateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewPluginInterface::DoActivateL( const TVwsViewId& aPrevViewId,
												   TUid aCustomMessageId,
												   const TDesC8& aCustomMessage )
	{
	iView->DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
	
	// Update the title of the settings view when launched from GS.
	// Text to be displayed on the title pane.
	HBufC* titleText = StringLoader::LoadLC( R_CLOCK_TITLE_SETTINGS );
    CEikStatusPane* statusPane = StatusPane();
    // Get the titlepane.
	CAknTitlePane* titlePane = ( CAknTitlePane* )statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) );
    // Set the text.
	titlePane->SetTextL( titleText->Des() );
    
	// Cleanup.
	CleanupStack::PopAndDestroy( titleText );
	}
	
// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::DoDeactivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClkDateTimeViewPluginInterface::DoDeactivate()
	{
	iView->DoDeactivate();
	}

// ---------------------------------------------------------
// CClkDateTimeViewPluginInterface::HandleForegroundEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//	
void CClkDateTimeViewPluginInterface::HandleForegroundEventL( TBool aForeground )
	{
	iView->HandleForegroundEventL( aForeground );
	}

// End of File
