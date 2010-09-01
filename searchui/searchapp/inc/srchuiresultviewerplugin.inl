/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      inline method defination for class CSrchUiResultViewerPlugin
*
*/






//System Includes
#include <ecom.h>
#include <e32cmn.h>

// ============================ MEMBER FUNCTIONS ===============================

	
// -----------------------------------------------------------------------------
// CSrchUiResultViewerPlugin::NewL
// Ecom interface static factory method implementation.
// -----------------------------------------------------------------------------
//
CSrchUiResultViewerPlugin* CSrchUiResultViewerPlugin::NewL( TUid aImpUid)
    {
    CSrchUiResultViewerPlugin* self = CSrchUiResultViewerPlugin::NewLC( aImpUid );	
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CSrchUiResultViewerPlugin::NewLC
// Ecom interface static factory method implementation.
// -----------------------------------------------------------------------------
//
CSrchUiResultViewerPlugin* CSrchUiResultViewerPlugin::NewLC( TUid aImpUid)
    {
    TAny* implementation = REComSession::CreateImplementationL ( aImpUid, 
                     _FOFF ( CSrchUiResultViewerPlugin, iDtor_ID_Key ) );	
    
    CSrchUiResultViewerPlugin* self = 
               REINTERPRET_CAST( CSrchUiResultViewerPlugin*, implementation );
    
    self->iPluginId = aImpUid;

    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSrchUiResultViewerPlugin::~CSrchUiResultViewerPlugin
// Destructor
// -----------------------------------------------------------------------------
//
/*
CSrchUiResultViewerPlugin::~CSrchUiResultViewerPlugin()
    {
      REComSession::DestroyedImplementation( iDtor_ID_Key );
    // REComSession::Close();
    }
*/
// -----------------------------------------------------------------------------
// CSrchUiResultViewerPlugin::ListAllImplementationsL
// Other Items are mentioned in the Header
// -----------------------------------------------------------------------------
//
inline void CSrchUiResultViewerPlugin::ListAllImplementationsL( 
			RImplInfoPtrArray& aImplInfoArray )
    {
  	TUid KSrchUiResultEcomInterfaceUidAsTUid;
    KSrchUiResultEcomInterfaceUidAsTUid.iUid = 0x1028240E;
    REComSession::ListImplementationsL( KSrchUiResultEcomInterfaceUidAsTUid,
    		 aImplInfoArray );
    }

// -----------------------------------------------------------------------------
// CSrchUiResultViewerPlugin::PluginId
// Other Items are mentioned in the Header 
// -----------------------------------------------------------------------------
//
TUid CSrchUiResultViewerPlugin::PluginId() const
    {
    return iPluginId;
    }


// -----------------------------------------------------------------------------
// CSrchUiResultViewerPlugin::PluginId
// Other Items are mentioned in the Header 
// -----------------------------------------------------------------------------
//
CAknViewAppUi* CSrchUiResultViewerPlugin::AppUi()
{
    return iAppUi;
}
 
