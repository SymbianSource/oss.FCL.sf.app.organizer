/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements RECAL customisation api
*
*/

// SYSTEM INCLUDES
#include <calendar.rsg>
#include <calenservices.h>
#include <calenlauncher.h>   //Recal's native views Uids
#include <calencommands.hrh>
#include <eikmenup.h>
#include <bautils.h>
#include <featmgr.h>

// PROJECT INCLUDES
#include "cbcviewplugin.h"
#include "cbcdayview.h"
#include "cbcresourcemanager.h"
#include "calenuid.h"
#include "calendarui_debug.h" //debug

// CONSTANTS
namespace
    {
    _LIT( KResourceFile, "z:\\resource\\bcviewplugingui.rsc" );
    }

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CBCViewPlugin::NewL
// Static constructor for this class
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CBCViewPlugin* CBCViewPlugin::NewL( MCalenServices* aServices )
    {
    TRACE_ENTRY_POINT;
    CBCViewPlugin* self = new ( ELeave ) CBCViewPlugin( aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CBCViewPlugin::CBCViewPlugin
// C++ constructor for this class
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CBCViewPlugin::CBCViewPlugin( MCalenServices* aServices )
:   iServices( aServices )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }
    
// -----------------------------------------------------------------------------
// CBCViewPlugin::ConstructL
// 2nd phase constructor for this class.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CBCViewPlugin::ConstructL()
    {    
    TRACE_ENTRY_POINT;
    FeatureManager::InitializeLibL();
    CCoeEnv* coeEnv( CCoeEnv::Static() );
    TFileName resourceFile(KResourceFile);
    BaflUtils::NearestLanguageFile( 
            coeEnv->FsSession(),  resourceFile );
    
    iResourceOffset = coeEnv->AddResourceFileL( resourceFile );
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCViewPlugin::~CBCViewPlugin
// Destructor for this class
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CBCViewPlugin::~CBCViewPlugin()
    {
    TRACE_ENTRY_POINT;
    CCoeEnv* coeEnv( CCoeEnv::Static() );
    coeEnv->DeleteResourceFile( iResourceOffset );
    if ( iServices )
        {
        iServices->Release();
        }

    FeatureManager::UnInitializeLib();
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCViewPlugin::GetCustomViewsL
// Array is filled with business calendar views
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CBCViewPlugin::GetCustomViewsL( RPointerArray<CCalenView>& aCustomViewArray )
    {
    TRACE_ENTRY_POINT;
    // Append Day view to aCustomViewArray
    CBCDayView* view = CBCDayView::NewLC( iServices );
    aCustomViewArray.AppendL( view );
    CleanupStack::Pop( view );
    TRACE_EXIT_POINT;
    }
    
// -----------------------------------------------------------------------------
// CBCViewPlugin::GetCustomSettingsL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CBCViewPlugin::GetCustomSettingsL( RPointerArray<CAknSettingItem>& /*aCustomSettingArray*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }
    
// -----------------------------------------------------------------------------
// CBCViewPlugin::InfobarL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCoeControl* CBCViewPlugin::InfobarL( const TRect& /*aRect*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }
    
// -----------------------------------------------------------------------------
// CBCViewPlugin::InfobarL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CBCViewPlugin::InfobarL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return KNullDesC();
    }

// -----------------------------------------------------------------------------
// CBCViewPlugin::CustomPreviewPaneL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCalenPreview* CBCViewPlugin::CustomPreviewPaneL( TRect& /*aRect*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// -----------------------------------------------------------------------------
// CBCViewPlugin::PreviewPaneL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCoeControl* CBCViewPlugin::PreviewPaneL( TRect& /*aRect*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }
    
// -----------------------------------------------------------------------------
// CBCViewPlugin::CommandHandlerL
// Decides what command handler is returned
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MCalenCommandHandler* CBCViewPlugin::CommandHandlerL( TInt /*aCommand*/ )
	{
    TRACE_ENTRY_POINT;
    //no command handler
    TRACE_EXIT_POINT;
	return NULL;
    }
    
// -----------------------------------------------------------------------------
// CBCViewPlugin::RemoveViewsFromCycle
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CBCViewPlugin::RemoveViewsFromCycle( RArray<TInt>& /*aViews*/ )
    {
    TRACE_ENTRY_POINT;
    //the plugin does not remove any views from cycle
    TRACE_EXIT_POINT;
    }
    
// -----------------------------------------------------------------------------
// CBCViewPlugin::CustomiseMenuPaneL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CBCViewPlugin::CustomiseMenuPaneL( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBCViewPlugin::CanBeEnabledDisabled
// Checks whether the plugin can be enabled or disabled from calendar settings
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool  CBCViewPlugin::CanBeEnabledDisabled()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CBCViewPlugin::CalenCustomisationExtensionL
// No implementation needed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TAny*  CBCViewPlugin::CalenCustomisationExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }
    
// End of file
