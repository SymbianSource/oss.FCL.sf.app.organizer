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
* Description:      inline method defination for  class CSrchUiServicePlugin 
*
*/






//System Includes
#include <ecom.h>
#include <e32cmn.h>

// ============================ MEMBER FUNCTIONS ===============================

	
// -----------------------------------------------------------------------------
// srchuiserviceplugin::NewL
// Ecom interface static factory method implementation.
// -----------------------------------------------------------------------------
//
inline CSrchUiServicePlugin* CSrchUiServicePlugin::NewL( TUid aImpUid )
    {
    CSrchUiServicePlugin* self = CSrchUiServicePlugin::NewLC( aImpUid );	
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CSearchUi::NewLC
// Ecom interface static factory method implementation.
// -----------------------------------------------------------------------------
//
inline CSrchUiServicePlugin* CSrchUiServicePlugin::NewLC( TUid aImpUid )
    {
    TAny* implementation = REComSession::CreateImplementationL ( aImpUid, _FOFF ( CSrchUiServicePlugin, iDtor_ID_Key ) );	
    CSrchUiServicePlugin* self = REINTERPRET_CAST( CSrchUiServicePlugin*, implementation );
    self->iPluginId = aImpUid;
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSrchUiServicePlugin::~CSrchUiServicePlugin
// Destructor
// -----------------------------------------------------------------------------
//
/*
CSrchUiServicePlugin::~CSrchUiServicePlugin()
    {
      REComSession::DestroyedImplementation( iDtor_ID_Key );
    // REComSession::Close();
    }
*/

// -----------------------------------------------------------------------------
// CSrchUiServicePlugin::ListAllImplementationsL
// Other Items are mentioned in the Header
// -----------------------------------------------------------------------------
//
inline void CSrchUiServicePlugin::ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray )
    {
  	TUid KSrchUiServicEcomInterfaceUidAsTUid;
    KSrchUiServicEcomInterfaceUidAsTUid.iUid = 0x10282407;
    REComSession::ListImplementationsL( KSrchUiServicEcomInterfaceUidAsTUid, aImplInfoArray );
    }

// -----------------------------------------------------------------------------
// CSrchUiServicePlugin::PluginId
// Other Items are mentioned in the Header 
// -----------------------------------------------------------------------------
//
inline TUid CSrchUiServicePlugin::PluginId() const
    {
    return iPluginId;
    }
 
