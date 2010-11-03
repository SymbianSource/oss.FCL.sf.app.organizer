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
* Description:   Inline methods for CMRMBUtilsMBInfoHandler
*
*/

class RImplInfoPtrArrayMRMBUtilsMBInfo : public RImplInfoPtrArray
    {
    public:
        void Close()
            {
            ResetAndDestroy();
            RImplInfoPtrArray::Close();
            }
    };

// ----------------------------------------------------------------------------
// CMRMBUtilsMBInfoHandler::~CMRMBUtilsMBInfoHandler
//
// Destructor.
// ----------------------------------------------------------------------------
//
inline CMRMBUtilsMBInfoHandler::~CMRMBUtilsMBInfoHandler()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// ----------------------------------------------------------------------------
// CMRMBUtilsMBInfoHandler::NewL
//
// ECom factory method.
// ----------------------------------------------------------------------------
//
inline CMRMBUtilsMBInfoHandler* CMRMBUtilsMBInfoHandler::NewL( 
		const TDesC8& aIdentifier,
		MMRMailboxUtilsObserver& aObserver )
    {
    TAny* obj = NULL;

    TInt32 keyOffset = _FOFF( CMRMBUtilsMBInfoHandler, iDtor_ID_Key );

    TEComResolverParams resolverParams;
    resolverParams.SetDataType( aIdentifier );

    RImplInfoPtrArrayMRMBUtilsMBInfo implArray;
    CleanupClosePushL( implArray );
    REComSession::ListImplementationsL( 
            TUid::Uid( KMRMailboxUtilsExtensionIFUID ),
            resolverParams,
            implArray );

	if ( !implArray.Count() )
		{				
		obj = NULL;		
		}
		
	else
	{
    TUid implUid = implArray[0]->ImplementationUid();
    obj = REComSession::CreateImplementationL( 
            implUid,
            keyOffset,
            static_cast<TAny*>( &aObserver ) );
	}
    CleanupStack::PopAndDestroy(); // implArray
    return reinterpret_cast<CMRMBUtilsMBInfoHandler*>( obj );
    }

// EOF
