/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides methods for Utils class.
                1.Stores and restores the Navi pane.
*               2.Reads resource files.
*
*/



// INCLUDE FILES
#include "CAttendeeUiUtils.h"
#include "AttendeeViewConsts.h"
#include <bautils.h>
#include <stringloader.h>
#include <AknAppUi.h>
#include <akntitle.h>
#include <data_caging_path_literals.hrh>

// ============================ MEMBER FUNCTIONS ===============================
// ----------------------------------------------------------------------------
// CAttendeeUiUtils::NewL
// ----------------------------------------------------------------------------
//
CAttendeeUiUtils* CAttendeeUiUtils::NewL()
    {
    CAttendeeUiUtils* self = new(ELeave)CAttendeeUiUtils;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self); //self
    return self;
    }

// -----------------------------------------------------------------------------
// CAttendeeUiUtils::CAttendeeUiUtils
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAttendeeUiUtils::CAttendeeUiUtils() : iPbkResourceFile( *CEikonEnv::Static() )
    {
    }

// ----------------------------------------------------------------------------
// CAttendeeUiUtils::ConstructL
// ----------------------------------------------------------------------------
//
void CAttendeeUiUtils::ConstructL()
    {
    iEnv = CEikonEnv::Static();
    iNaviPane = static_cast<CAknNavigationControlContainer*>(
            StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    
    }

// -----------------------------------------------------------------------------
// CAttendeeUiUtils::~CAttendeeUiUtils()
// -----------------------------------------------------------------------------
//
CAttendeeUiUtils::~CAttendeeUiUtils()
    {
    if ( iNaviPane && iNaviLabel )
        {
        iNaviPane->Pop( iNaviLabel );
        }
    delete iNaviLabel;
    delete iStoredTitlePaneText;  

    //If RestoreOldNaviPaneL is not called, it
    //must do atleast now
    TRAPD( error, RestoreOldNaviPaneL() );
    if ( error != KErrNone )
        {
        CEikonEnv::Static()->HandleError( error );
        }
                   
    delete iStoredDecorator;    
    }

// ----------------------------------------------------
// CAttendeeUiUtils::CreateNaviLabelL
// ----------------------------------------------------
//
void CAttendeeUiUtils::CreateNaviLabelL( const TDesC& aLabel )
    {
    if ( iNaviLabel )
        {
        iNaviPane->Pop( iNaviLabel );
        delete iNaviLabel;
        iNaviLabel = NULL;
        }

    iNaviLabel = iNaviPane->CreateNavigationLabelL( aLabel );
    iNaviPane->PushL( *iNaviLabel ); // activate navi label in navi pane
    }

// ----------------------------------------------------
// CAttendeeUiUtils::CreateNaviLabelL
// ----------------------------------------------------
//
void CAttendeeUiUtils::CreateNaviLabelL( const TInt aResourceId )
    {
    CEikonEnv* env = CEikonEnv::Static();
    HBufC* label = StringLoader::LoadLC( aResourceId, env);
    CreateNaviLabelL( *label );
    CleanupStack::PopAndDestroy(label); //label
    }

// ----------------------------------------------------
// CAttendeeUiUtils::StoreNaviPane
// ----------------------------------------------------
//
void CAttendeeUiUtils::StoreNaviPane()
    {
    if ( iNaviPane )
        {
        iStoredDecorator = iNaviPane->Top();
        }
    }

// ----------------------------------------------------
// CAttendeeUiUtils::RestoreOldNaviPaneL
// ----------------------------------------------------
//
void CAttendeeUiUtils::RestoreOldNaviPaneL()
    {
    if ( iStoredDecorator )
        {
        iNaviPane->PushL( *iStoredDecorator );
        iStoredDecorator = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CAttendeeUiUtils::OpenResourceFilesL
// ----------------------------------------------------------------------------
//
void CAttendeeUiUtils::OpenResourceFilesL()
    {
    FindResourceFileL();

    BaflUtils::NearestLanguageFile( iEnv->FsSession(), iResourceFileName );
    iResourceOffset = iEnv->AddResourceFileL( iResourceFileName );

    //Open phonebooks resource file
    iPbkResourceFile.OpenL();
    }

// ----------------------------------------------------------------------------
// CAttendeeUiUtils::CloseResourceFile
// ----------------------------------------------------------------------------
//
void CAttendeeUiUtils::CloseResourceFile()
    {
    iEnv->DeleteResourceFile( iResourceOffset );

    //Phonebooks resource file
    iPbkResourceFile.Close();
    }

// ----------------------------------------------------------------------------
// CAttendeeUiUtils::StoreTitlePaneL
// ----------------------------------------------------------------------------
//
void CAttendeeUiUtils::StoreTitlePaneL()
	{    
    CEikStatusPane* statusPane = StatusPane();
    if ( statusPane && statusPane->PaneCapabilities(
        TUid::Uid(EEikStatusPaneUidTitle) ).IsPresent() )
        {
        iTitlePane = static_cast<CAknTitlePane*>( statusPane->
                            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );  

        if ( iTitlePane->Text() ) 
            {
            iStoredTitlePaneText = iTitlePane->Text()->AllocL();
            }
        }
	}

// ----------------------------------------------------------------------------
// CAttendeeUiUtils::ChangeTitlePaneL
// ----------------------------------------------------------------------------
//
void CAttendeeUiUtils::ChangeTitlePaneL( TInt aResourceText )
    {
    if ( aResourceText != 0 )
        {
        HBufC* buf = StringLoader::LoadLC( aResourceText );
        ChangeTitlePaneL( buf );
        CleanupStack::Pop(buf); // buf, ownership transferred
        }
    }

// ----------------------------------------------------------------------------
// CAttendeeUiUtils::ChangeTitlePaneL
// ----------------------------------------------------------------------------
//
void CAttendeeUiUtils::ChangeTitlePaneL( HBufC* aTitle )
	{    
    CEikStatusPane* statusPane = StatusPane();
    if ( statusPane && statusPane->PaneCapabilities(
            TUid::Uid( EEikStatusPaneUidTitle ) ).IsPresent() )
        {
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>( statusPane->
                            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        titlePane->SetText( aTitle ); // Takes ownership of aTitle
        }
	}

// ----------------------------------------------------------------------------
// CAttendeeUiUtils::RestoreOldTitlePane
// ----------------------------------------------------------------------------
//
void CAttendeeUiUtils::RestoreOldTitlePane()
    {
    if ( iTitlePane && iStoredTitlePaneText )
        {
        // iTitlePane takes ownership of iStoredTitlePaneText
        iTitlePane->SetText( iStoredTitlePaneText );
        iStoredTitlePaneText = NULL;
        }
    }

// ----------------------------------------------------
// CAttendeeUiUtils::AttendeeViewIconFileL
// ----------------------------------------------------
//
TFileName CAttendeeUiUtils::AttendeeViewIconFileL()
    {
    TFileName file( KDC_APP_BITMAP_DIR );
    file.Append( KAttendeeViewIconFileName ); //always safe    
    GetFileWithCorrectDriveL( file );
    return file;
    }

// ----------------------------------------------------------------------------
// CAttendeeUiUtils::FindResourceFileL
// ----------------------------------------------------------------------------
//
void CAttendeeUiUtils::FindResourceFileL()
    {
    // Path and file name:
    TFileName* file = new(ELeave)TFileName;
    CleanupStack::PushL( file );
    file->Append( KDC_RESOURCE_FILES_DIR ); // always safe
    file->Append( KAttendeeViewRscFileName ); // always safe

    // Drive:
    TFileName* drive = new(ELeave)TFileName;
    CleanupStack::PushL( drive );
    Dll::FileName( *drive );

    // Add drive to path & file name:
    TParse* parse = new(ELeave)TParse;
    CleanupStack::PushL( parse );
    User::LeaveIfError( parse->Set( *file, drive, NULL ) );
    iResourceFileName = parse->FullName();

    CleanupStack::PopAndDestroy( 3 ); // parse, drive, file
    }

// ----------------------------------------------------------------------------
// CAttendeeUiUtils::StatusPane
// ----------------------------------------------------------------------------
//
CEikStatusPane* CAttendeeUiUtils::StatusPane()
    {
    return CEikonEnv::Static()->AppUiFactory()->StatusPane();
    }

// ----------------------------------------------------------------------------
// CAttendeeUiUtils::GetFileWithCorrectDriveL
// ----------------------------------------------------------------------------
//
void CAttendeeUiUtils::GetFileWithCorrectDriveL( TFileName& aFileName )
    {
    TFileName* dllDrive = new(ELeave)TFileName;
    CleanupStack::PushL( dllDrive );
    TParse* parse = new(ELeave)TParse;
    CleanupStack::PushL( parse );

    Dll::FileName( *dllDrive );
	User::LeaveIfError( parse->Set( *dllDrive, NULL, NULL ) );
    *dllDrive = parse->Drive(); // contains drive, e.g. "c:"
    User::LeaveIfError( parse->Set( *dllDrive, &aFileName, NULL ) );
	aFileName = parse->FullName();

    CleanupStack::PopAndDestroy( 2 ); // parse, dlldrive
    }

//  End of File
