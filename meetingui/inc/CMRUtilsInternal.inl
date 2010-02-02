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



#include <MeetingRequestUids.hrh>

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CMRUtilsInternal::~CMRUtilsInternal
//
// Destructor.
// ----------------------------------------------------------------------------
//
inline CMRUtilsInternal::~CMRUtilsInternal()
    {
    }

// ----------------------------------------------------------------------------
// CMRUtilsInternal::NewL
//
// ECom factory method.
// ----------------------------------------------------------------------------
//
inline CMRUtilsInternal* CMRUtilsInternal::NewL(
    MMRUtilsObserver& aObserver,
    CCalSession& aCalSession,
    CMsvSession* aMsvSession )
    {
    CMRUtilsInternal* self = NULL;
    CMRUtils* utils = CMRUtils::NewL( KMRUtilsDefaultImpl, 
                                      aObserver, 
                                      aCalSession, 
                                      aMsvSession );
    if ( utils )
        {
        self = reinterpret_cast<CMRUtilsInternal*> ( utils );
        }
    return self;
    }

// End of file
