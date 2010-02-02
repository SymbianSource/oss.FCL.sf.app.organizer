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
* Description:  Implementation for CStopwatchIndicatorPlugin class.
*
*/

// System includes
#include <coeaui.h>
#include <badesca.h>
#include <StringLoader.h>
#include <avkon.rsg>
#include <ConeResLoader.h>
#include <stopwatchindicatorplugin.rsg>
#include <data_caging_path_literals.hrh>
#include <f32file.h>

// User includes
#include "stopwatchindicatorplugin.h"
#include "clock.h"

// Literals
_LIT( KPanePluginResFile, "stopwatchindicatorplugin.rsc" );

// ---------------------------------------------------------
// CStopwatchIndicatorPlugin::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CStopwatchIndicatorPlugin* CStopwatchIndicatorPlugin::NewL()
    {
    CStopwatchIndicatorPlugin* self = new( ELeave ) CStopwatchIndicatorPlugin;
    CleanupStack::PushL( self );

    self->ConstructL();
    
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CStopwatchIndicatorPlugin::CStopwatchIndicatorPlugin
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CStopwatchIndicatorPlugin::CStopwatchIndicatorPlugin()
    {
    // No implementation yet.
    }
    
// ---------------------------------------------------------
// CStopwatchIndicatorPlugin::~CStopwatchIndicatorPlugin
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CStopwatchIndicatorPlugin::~CStopwatchIndicatorPlugin()
    {
    // No implementation.
    }

// ---------------------------------------------------------
// CStopwatchIndicatorPlugin::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CStopwatchIndicatorPlugin::ConstructL()
    {
    // Get the environment.
    iEnv = CEikonEnv::Static();
    }

// ---------------------------------------------------------
// CStopwatchIndicatorPlugin::HandleIndicatorTapL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CStopwatchIndicatorPlugin::HandleIndicatorTapL( const TInt /*aUid*/ )
    {
    const TVwsViewId clkStopWatchView( KClockAppUid, KClockAppStopwatchViewId );
    
    // Launch the Clock application with the stopwatch view as default
    CCoeEnv::Static()->AppUi()->CreateActivateViewEventL( clkStopWatchView,
                                                          KClockAppStopwatchViewId,
                                                          KNullDesC8 );
    }

// ---------------------------------------------------------
// CStopwatchIndicatorPlugin::TextL
// rest of the details are commented in the header
// ---------------------------------------------------------
//  
HBufC* CStopwatchIndicatorPlugin::TextL( const TInt aUid, TInt& aTextType )
    {
    // Store the Uid.
    iUid = aUid;
    
    // The return text.
    HBufC* returnText = NULL;
    
    // Open the resource file to read the resources.
    RConeResourceLoader resourceLoader( *iEnv );

    // The file parser
    TParse fileParse;
    fileParse.Set( KPanePluginResFile, &KDC_APP_RESOURCE_DIR , NULL );

    // File name object
    TFileName fileName = fileParse.FullName();
    // Open the file name
    resourceLoader.OpenL( fileName ); 

    // Get the text to be displayed from resource file.
    returnText = StringLoader::LoadL( R_QTN_CLK_STYLUS_POPUP_STOPWATCH_RUNNING );
    
    // Set text type.
    aTextType = CAknIndicatorPlugin::EAknIndicatorPluginLinkText;
    
    // Cleanup.
    resourceLoader.Close();
    
    // Return the updated text.
    return returnText;
    }
    
// End of file
