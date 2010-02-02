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
* Description:  ECom search interface definition
*
*/


// INCLUDE FILES
#include <ecom/ecom.h>
#include <searchcommon.h>

// ============================ MEMBER FUNCTIONS ===============================

	
// -----------------------------------------------------------------------------
// CSearchPlugin::NewL
// Ecom interface static factory method implementation.
// -----------------------------------------------------------------------------
//
inline CSearchPlugin* CSearchPlugin::NewL( TUid aImpUid )
	{
	CSearchPlugin* self = CSearchPlugin::NewLC( aImpUid );	
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CSearchUi::NewLC
// Ecom interface static factory method implementation.
// -----------------------------------------------------------------------------
//
inline CSearchPlugin* CSearchPlugin::NewLC( TUid aImpUid )
	{
	TAny* implementation = REComSession::CreateImplementationL ( aImpUid,
	    _FOFF ( CSearchPlugin, iDtor_ID_Key ) );
	CSearchPlugin* self = REINTERPRET_CAST( CSearchPlugin*, implementation );
	self->iPluginId = aImpUid;
	CleanupStack::PushL( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSearchPlugin::~CSearchPlugin
// Destructor
// -----------------------------------------------------------------------------
//
inline CSearchPlugin::~CSearchPlugin()
	{
	REComSession::DestroyedImplementation( iDtor_ID_Key );
	}

// -----------------------------------------------------------------------------
// CSearch::ListAllImplementationsL
// Lists all implementations which satisfy this ecom interface
//  (KSearchEcomInterfaceUid).
// -----------------------------------------------------------------------------
//
inline void CSearchPlugin::ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray )
	{
	REComSession::ListImplementationsL( KSearchPluginInterfaceUid, aImplInfoArray );
	}

// -----------------------------------------------------------------------------
// CSearch::ListAllImplementationsL
// Gets the plugin id.
// 
// -----------------------------------------------------------------------------
//
inline TUid CSearchPlugin::PluginId() const
  {
  return iPluginId;
  }
 
