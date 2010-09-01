/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Inline methods for ECom plug-in use.
*
*/



#include <calentryview.h>

// simple wrapper class with array deletion on close
class RImplInfoPtrArrayUtils : public RImplInfoPtrArray
    {
    public:
        void Close()
            {
            ResetAndDestroy();
            RImplInfoPtrArray::Close();
            }
    };

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CMRUtils::~CMRUtils
//
// Destructor.
// ----------------------------------------------------------------------------
//
inline CMRUtils::~CMRUtils()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// ----------------------------------------------------------------------------
// CMRUtils::NewL
//
// ECom factory method.
// ----------------------------------------------------------------------------
//
inline CMRUtils* CMRUtils::NewL(
    const TDesC8& aMtmUid,
    MMRUtilsObserver& aObserver,
    CCalSession& aCalSession,
    CMsvSession* aMsvSession )
    {
    TAny* obj = NULL;

    TInt32 keyOffset = _FOFF( CMRUtils, iDtor_ID_Key );
    TEComResolverParams resolverParams;
    resolverParams.SetDataType( aMtmUid  );

    RImplInfoPtrArrayUtils implArray;
    CleanupClosePushL( implArray );
    REComSession::ListImplementationsL( TUid::Uid( KMRUtilsInterfaceUID ),
                                        resolverParams,
                                        implArray );
    if ( implArray.Count() > 0 )
    	{ // Construct MTM-specific implementation, pick first from the array
    	TUid implUid = implArray[0]->ImplementationUid();
    	obj = REComSession::CreateImplementationL( implUid, keyOffset );
    	}
    else
    	{ // Fallback to the default implementation
    	resolverParams.SetDataType( KMRUtilsDefaultImpl );
    	
    	obj = REComSession::CreateImplementationL( TUid::Uid( KMRUtilsInterfaceUID ),
                                                   keyOffset,
                                                   resolverParams );        
    	}
    CleanupStack::PopAndDestroy(); // implArray
    
    CMRUtils* self = reinterpret_cast<CMRUtils*> ( obj );

    self->iObserver = &aObserver;
    
    CleanupStack::PushL( self );
    self->SessionSetupL( aCalSession, aMsvSession );
    CleanupStack::Pop( self );
    return self;
    }

// End of file
