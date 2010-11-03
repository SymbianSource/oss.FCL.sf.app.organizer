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
* Description:      SearchSettingsPlugin.cpp //GSAppsPlugin source code.
*
*/






// Includes
#include <AknViewAppUi.h>
#include <ConeResLoader.h>
#include <SearchSettingsPluginRsc.rsg>
#include <GSFWViewUIDs.h>
#include <GSMainView.h>
#include <GSPrivatePluginProviderIds.h>
#include <GSTabHelper.h>
#include <GSTabbedViewOwner.h>
#include <StringLoader.h>
#include <barsread.h>
#include <akntitle.h> 
#include <hlplch.h>
#include <set.hlp.hrh>

#include <ImplementationInformation.h>
//#include <srchuisettingsplugininterface.h>

#include "SearchSettingsPlugin.h"
#include "SearchSettingsContainer.h"
#include <srchuisettingsview.h>

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CSearchSettingsPlugin::NewL
// Static constructor
//
// ---------------------------------------------------------------------------
//
CSearchSettingsPlugin* CSearchSettingsPlugin::NewL( TAny* /*aInitParams*/ )
	{
	CSearchSettingsPlugin* self = new(ELeave) CSearchSettingsPlugin();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// CSearchSettingsPlugin::~CSearchSettingsPlugin
// Destructor
//
// ---------------------------------------------------------------------------
//
CSearchSettingsPlugin::~CSearchSettingsPlugin()
    {
    iResources.Close();
    }

// -----------------------------------------------------------------------------
// CSearchSettingsPlugin::Id
//
//
// -----------------------------------------------------------------------------
//
TUid CSearchSettingsPlugin::Id() const
    {
    return KGSSearchSettingsPluginUid ; //KGSAppsPluginUid;
    }


// -----------------------------------------------------------------------------
// CSearchSettingsPlugin::DoActivateL
//
//
// -----------------------------------------------------------------------------
// 

void CSearchSettingsPlugin::DoActivateL( const TVwsViewId& aPrevViewId,
                                 TUid aCustomMessageId,
                                 const TDesC8& aCustomMessage )
    {
    
    iPrevViewId = aPrevViewId;    
    CGSParentPlugin::DoActivateL( aPrevViewId,
                                  aCustomMessageId,
                                  aCustomMessage );
        
    if( iContainer )
    {
	    AppUi()->RemoveFromViewStack( *this, iContainer );
	    delete iContainer;
	    iContainer = NULL;
    }
 
    }


// -----------------------------------------------------------------------------
// CSearchSettingsPlugin::DoDeactivate
//
//
// -----------------------------------------------------------------------------
//
void CSearchSettingsPlugin::DoDeactivate()
    {
    
    CGSParentPlugin::DoDeactivate();
    }


// ========================= From CGSParentPlugin =====================


// -----------------------------------------------------------------------------
// CSearchSettingsPlugin::UpperLevelViewUid
//
//
// -----------------------------------------------------------------------------
//
TUid CSearchSettingsPlugin::UpperLevelViewUid()
    {
    return KGSAppsPluginUid ;
    }
 
// -----------------------------------------------------------------------------
// CSearchSettingsPlugin::GetHelpContext
//
//
// -----------------------------------------------------------------------------
//
void CSearchSettingsPlugin::GetHelpContext( TCoeHelpContext& aContext )
    {
    aContext.iMajor = KUidGS;
    aContext.iContext = KSET_HLP_APPLICATIONS_LIST;
    }   

// -----------------------------------------------------------------------------
// CSearchSettingsPlugin::HandleCommandL
// Handles user commands.
//
// -----------------------------------------------------------------------------
//
void CSearchSettingsPlugin::HandleCommandL( TInt aCommand )
{
    switch ( aCommand )
        {
        case EAknSoftkeyOpen:
        case EAknSoftkeyOk:
        case EAknSoftkeySelect:
        	case 504:
        	case 1001:
            {
			CGSParentPlugin::HandleCommandL(aCommand);
            break;
            } 
        case EAknSoftkeyBack:
			{
			AppUi()->ActivateLocalViewL( iPrevViewId.iViewUid );
            break;
			}
        case EAknSoftkeyExit:
            {
            static_cast<MEikCommandObserver*>( AppUi() )->ProcessCommandL( EAknCmdExit );
            break;
            }
        case EAknCmdHelp:
        case EEikCmdHelpContents:
            {
            }                        
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
	
}

// ---------------------------------------------------------------------------
// CSearchSettingsPlugin::HandleListBoxEventL
// Handles events of the ListBox.
//
// ---------------------------------------------------------------------------
//
void CSearchSettingsPlugin::HandleListBoxEventL( CEikListBox* /*aListBox*/ , 
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
// -----------------------------------------------------------------------------
// CSearchSettingsPlugin::GetCaptionL
//
//
// -----------------------------------------------------------------------------
//
void CSearchSettingsPlugin::GetCaptionL( TDes& aCaption ) const
    {
    // The resource file is already opened.
    HBufC* result = StringLoader::LoadL( R_GS_SEARCH_VIEW_CAPTION );
    aCaption.Copy( *result );
    delete result;
    }


// -----------------------------------------------------------------------------
// CSearchSettingsPlugin::CreateIconL
// creates the icon of the view.
//
// -----------------------------------------------------------------------------
//
CGulIcon* CSearchSettingsPlugin::CreateIconL( const TUid aIconType )
    {
    CGulIcon* icon;
    TParse* fp = new( ELeave ) TParse();
    CleanupStack::PushL( fp );
    fp->Set( KGSAppsPluginIconDirAndName, &KDC_BITMAP_DIR, NULL );
    icon = CGSPluginInterface::CreateIconL( aIconType );
    CleanupStack::PopAndDestroy( fp );

    return icon;
    }
    
// -----------------------------------------------------------------------------
// CSearchSettingsPlugin::PluginProviderCategory
//
//
// -----------------------------------------------------------------------------
//
TInt CSearchSettingsPlugin::PluginProviderCategory() const
    {
    return KGSPluginProviderInternal;
    }
    
// ---------------------------------------------------------------------------
// CSearchSettingsPlugin::HandleForegroundEventL
// Handles changes to the application when it switches to or from the foreground
//
// ---------------------------------------------------------------------------
//
void CSearchSettingsPlugin::HandleForegroundEventL( TBool /* Foreground */ )
    {
    }
    
// ---------------------------------------------------------------------------
// CSearchSettingsPlugin::CSearchSettingsPlugin
// Constructor
//
// ---------------------------------------------------------------------------
//
CSearchSettingsPlugin::CSearchSettingsPlugin()
    : CGSParentPlugin(), iResources( *iCoeEnv )
    {
    }

// ---------------------------------------------------------------------------
// CSearchSettingsPlugin::ConstructL
// Symbian OS two-phased constructor
//
// ---------------------------------------------------------------------------
//

void CSearchSettingsPlugin::ConstructL()
    {
    	TFileName resourceFileName;
        TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
        _LIT( KResoureFileName, "searchSettingsPluginRsc.rsc" );
       
        resourceFileName.Copy( driveLetter );  
        resourceFileName.Append( KDC_ECOM_RESOURCE_DIR );
        resourceFileName.Append( KResoureFileName );
        
    OpenLocalizedResourceFileL( resourceFileName, iResources );
    BaseConstructL( R_GS_APPLICATIONS_VIEW, R_GS_SEARCH_VIEW_TITLE );
    }

// ---------------------------------------------------------------------------
// CSearchSettingsPlugin::HandleListBoxSelectionL
// Handles selection of the ListBox items.
//
// ---------------------------------------------------------------------------
//    
void CSearchSettingsPlugin::HandleListBoxSelectionL() 
    {
    }
		                          
// End of File

