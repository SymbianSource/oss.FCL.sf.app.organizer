/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main settings view class
*
*/


//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calengsplugin.h"

#include "calengsplugincontainer.h"
#include "CalenUid.h"
#include "calendar.hrh"
#include <Calendar.rsg>
#include <calendarsettings.mbg>

#include <featmgr.h>
#include <StringLoader.h>   // for StringLoader
#include <hlplch.h>         // for HlpLauncher
#include <bautils.h>        // for BaflUtils
#include <pathinfo.h>

_LIT( KCalenIconDirAndFile,"calendarsettings.mif" );
_LIT( KCalenAppResourceFile, "calendar.rsc" );
_LIT( KResourceFile, "calencommonui.rsc" );

// ========================= MEMBER FUNCTIONS =================================

// ----------------------------------------------------------------------------
// CCalenGSPlugin::CCalenGSPlugin
// C++ constructor.
// ----------------------------------------------------------------------------
//
CCalenGSPlugin::CCalenGSPlugin()
    : iResourceLoader( *iCoeEnv )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPlugin::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CCalenGSPlugin* CCalenGSPlugin::NewL( TAny* /*aInitParams*/ )
    {
    TRACE_ENTRY_POINT;

    CCalenGSPlugin* self = new(ELeave) CCalenGSPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenGSPlugin::ConstructL
// Second phase of construction.
// ----------------------------------------------------------------------------
//
void CCalenGSPlugin::ConstructL()
    {
    TRACE_ENTRY_POINT;
    // Find the resource file
        TFileName fileName;
	// Get the complate path of the DLL from where it is currently loaded
	Dll::FileName( fileName );
	
	TFileName resourcePath;
	resourcePath.Append(fileName.Mid(0,2));
	resourcePath.Append(KDC_APP_RESOURCE_DIR);
    // Find the resource file
    TParse parse;
    parse.Set( KCalenAppResourceFile,
               &resourcePath,
               NULL );
    TFileName resourceFileName( parse.FullName() );

    // Get language of resource file
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), resourceFileName );

   // Add the the commonUI resource file to the environment
    TFileName resFile;
    resFile.Append(fileName.Mid(0,2));
    resFile.Append( KDC_RESOURCE_FILES_DIR );
    resFile.Append( KResourceFile );

    CEikonEnv* eikonEnv = CEikonEnv::Static();
    BaflUtils::NearestLanguageFile( eikonEnv->FsSession(), resFile );
    iResourceFileOffset = eikonEnv->AddResourceFileL( resFile );

    // Open the resource file
    iResourceLoader.OpenL( resourceFileName );
    FeatureManager::InitializeLibL();

    BaseConstructL( R_CALENDAR_GS_SETTING_LIST_VIEW );

    iCaption = StringLoader::LoadL( R_CALENDAR_GS_VIEW_TITLE );
    TRACE_EXIT_POINT;
    }


// ----------------------------------------------------------------------------
// CCalenGSPlugin::~CCalenGSPlugin
// Destructor.
// ----------------------------------------------------------------------------
//
CCalenGSPlugin::~CCalenGSPlugin()
    {
    TRACE_ENTRY_POINT;
    FeatureManager::UnInitializeLib();

    if( iContainer )
        {
        if( iAppUi )
            {
            iAppUi->RemoveFromViewStack( *this, iContainer );
            }
        delete iContainer;
        }

    if( iResourceFileOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
        }

    iResourceLoader.Close();
	
	delete iCaption;
	    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPlugin::Id
// Returns the UID of this view.
// ----------------------------------------------------------------------------
//
TUid CCalenGSPlugin::Id() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return KUidCalenGSPlugin;
    }

// ----------------------------------------------------------------------------
// CCalenGSPlugin::HandleCommandL
// Handles commands.
// ----------------------------------------------------------------------------
//
void CCalenGSPlugin::HandleCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;
    switch ( aCommand )
        {
        case ECalenCmdChange:      /* fall through... */
        case EAknSoftkeyChange:    /* fall through... */
        case EAknCmdOpen:
            // Delegate to container
            Container()->HandleListBoxSelectionL( EAknCmdOpen );
            break;

        case EAknSoftkeyBack:
            // Activate previous view
#ifdef _DEBUG
    RDebug::Print(_L("CalenGS: activating view 0x%x"), iPrevViewId.iViewUid.iUid);
#endif
            iAppUi->ActivateLocalViewL( iPrevViewId.iViewUid );
            break;

        case EAknCmdHelp:
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), iAppUi->AppHelpContextL() );
                }
            break;

        default:
            iAppUi->HandleCommandL( aCommand );
            break;
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPlugin::HandleClientRectChange
// Handles screen size change.
// ----------------------------------------------------------------------------
//
void CCalenGSPlugin::HandleClientRectChange()
    {
    TRACE_ENTRY_POINT;

    if ( iContainer )
        {
        Container()->SetRect( ClientRect() );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPlugin::Container
// Returns a handle to the container class.
// ----------------------------------------------------------------------------
//
CCalenGSPluginContainer* CCalenGSPlugin::Container()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return static_cast<CCalenGSPluginContainer*>( iContainer );
    }

// ----------------------------------------------------------------------------
// CCalenGSPlugin::HandleListBoxSelectionL
// Handles any user actions while in the list view.
// ----------------------------------------------------------------------------
//
void CCalenGSPlugin::HandleListBoxSelectionL()
    {
    TRACE_ENTRY_POINT;

    // Delegate to container
    Container()->HandleListBoxSelectionL( ECalenCmdChange );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPlugin::GetCaptionL
// Populates aCaption with the caption text to be displayed in the navi pane.
// ----------------------------------------------------------------------------
//
void CCalenGSPlugin::GetCaptionL( TDes& aCaption ) const
    {
    TRACE_ENTRY_POINT;

    aCaption.Copy( *iCaption );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPlugin::NewContainerL
// Creates new iContainer.
// ----------------------------------------------------------------------------
//
void CCalenGSPlugin::NewContainerL()
    {
    TRACE_ENTRY_POINT;

    iContainer = new( ELeave ) CCalenGSPluginContainer;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenGSPlugin::CreateIconL
// Returns a new icon.
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenGSPlugin::CreateIconL( const TUid /*aIconType*/ )
    {
    TRACE_ENTRY_POINT;
    TParse* fp = new( ELeave ) TParse();
    CleanupStack::PushL( fp );
    
  	TFileName dllName;
	// Get the complate path of the DLL from where it is currently loaded
	Dll::FileName( dllName );
	
	TFileName resourcePath;
	resourcePath.Append(dllName.Mid(0,2));
	resourcePath.Append(KDC_APP_BITMAP_DIR);
    User::LeaveIfError( fp->Set( KCalenIconDirAndFile, &resourcePath, NULL ) );

    TBuf< KMaxFileName > fileName = fp->FullName();
    CleanupStack::PopAndDestroy( fp );

    CGulIcon* icon = AknsUtils::CreateGulIconL( AknsUtils::SkinInstance(),
                                                KAknsIIDQgnPropSetAppsCale,
                                                fileName,
                                                EMbmCalendarsettingsQgn_prop_set_apps_cale,
                                                EMbmCalendarsettingsQgn_prop_set_apps_cale );
    TRACE_EXIT_POINT;
    return icon;
    }

// End of file
