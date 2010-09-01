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
// CMRMailboxUtils::~CMRMailboxUtils
//
// Destructor.
// ----------------------------------------------------------------------------
//
inline CMRMailboxUtils::~CMRMailboxUtils()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// ----------------------------------------------------------------------------
// CMRMailboxUtils::NewL
//
// ECom factory method.
// ----------------------------------------------------------------------------
//
inline CMRMailboxUtils* CMRMailboxUtils::NewL( CMsvSession* aMsvSession )
    {    
    TAny* obj = NULL;
    TInt32 keyOffset = _FOFF( CMRMailboxUtils, iDtor_ID_Key );
    obj = REComSession::CreateImplementationL( TUid::Uid( KMRMailboxUtilsImplUID ),
                                               keyOffset,
                                               aMsvSession );
    return reinterpret_cast<CMRMailboxUtils*> ( obj );
    }

// End of file

