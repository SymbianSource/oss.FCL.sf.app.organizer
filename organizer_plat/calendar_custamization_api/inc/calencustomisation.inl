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
* Description:   Inline functions for the Calendar customisation class
 *
*/


// -----------------------------------------------------------------------
// CCalenCustomisation::CreateImplementationL
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline CCalenCustomisation* CCalenCustomisation::CreateImplementationL( 
                                         TUid aUid, MCalenServices* aServices )
    {
    TAny* ptr = REComSession::CreateImplementationL( aUid,
                   _FOFF( CCalenCustomisation, iDtor_ID_Key ) , aServices);
    CCalenCustomisation* impl = static_cast<CCalenCustomisation*> (ptr);
    return impl;
    }

// -----------------------------------------------------------------------
// CCalenCustomisation::~CCalenCustomisation
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline CCalenCustomisation::~CCalenCustomisation()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
    
// End of file

    
