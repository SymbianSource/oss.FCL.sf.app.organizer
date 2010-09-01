/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class handles signals related to asynchrnous functionalities for calendar server
*
*/



#ifndef CALSVRSIGNALLER_H
#define CALSVRSIGNALLER_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>


// CLASS DECLARATION
NONSHARABLE_CLASS( TCalServerStart )
    {
public:
    inline TCalServerStart(TRequestStatus& aStatus);
    inline TPtrC AsCommand() const;

private:
    inline TCalServerStart(){};

private:
    TThreadId iId;
    TRequestStatus* iStatus;

    };

inline TCalServerStart::TCalServerStart(TRequestStatus& aStatus)
    : iId( RThread().Id() ),
      iStatus( &aStatus )
    {
        aStatus = KRequestPending;
    }

inline TPtrC TCalServerStart::AsCommand() const
    {
    return TPtrC( reinterpret_cast<const TText*>( this ), sizeof( TCalServerStart )/sizeof( TText ) );
    }

#endif // CALSVRSIGNALLER_H


// End of File
