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



// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// simple wrapper class with array deletion on close
class RImplInfoPtrArrayMRViewers : public RImplInfoPtrArray
    {
    public:
        void Close()
            {
            ResetAndDestroy();
            RImplInfoPtrArray::Close();
            }
    };

// ----------------------------------------------------------------------------
// CMRViewers::~CMRViewers
//
// Destructor.
// ----------------------------------------------------------------------------
//
inline CMRViewers::~CMRViewers()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// ----------------------------------------------------------------------------
// CMRViewers::NewL
//
// ECom factory method.
// ----------------------------------------------------------------------------
//
inline CMRViewers* CMRViewers::NewL( const TDesC8& aMtmId )
    {

	TAny* obj = NULL;

	    TInt32 keyOffset = _FOFF( CMRViewers, iDtor_ID_Key );
	    TEComResolverParams resolverParams;
	    resolverParams.SetDataType( aMtmId  );

	    // This param goes all the way to the implementation, unlike
	    // mtm id given to resolverParams:
	    HBufC8* constrParams = aMtmId.AllocLC();

	    RImplInfoPtrArrayMRViewers implArray;
	    CleanupClosePushL( implArray );
	    REComSession::ListImplementationsL( TUid::Uid( KMRViewersInterfaceUID ),
	                                        resolverParams,
	                                        implArray );
	    if ( implArray.Count() > 0 )
	        { // Construct MTM-specific implementation, pick first from the array
	        TUid implUid = implArray[0]->ImplementationUid();
		    obj = REComSession::CreateImplementationL( implUid,
	                                                   keyOffset,
	                                                   constrParams );
	        }
	    else
	        { // Fallback to the default implementation
	        resolverParams.SetDataType( KAgnEntryUiDefaultImpl );
		    obj = REComSession::CreateImplementationL( TUid::Uid( KMRViewersInterfaceUID ),
	                                                   keyOffset,
	                                                   constrParams,
	                                                   resolverParams );
	        }
	    CleanupStack::PopAndDestroy(); // implArray
	    CleanupStack::Pop( constrParams ); // ownership transferred

    return reinterpret_cast<CMRViewers*> ( obj );
    }

// End of file

