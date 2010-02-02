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
* Description:   This class represents as calendar session for a client thread on the server-side. 
*
*/



#ifndef CALSVRSESSION_H
#define CALSVRSESSION_H

// INCLUDES
#include "CalenSvrDBManager.h"

#include <e32base.h>

// FORWARD DECLARATIONS
class CCalenServer;

// CLASS DECLARATION
NONSHARABLE_CLASS( CCalSvrSession ) : public CSession2, 
                                      public CCalenSvrDBManager::MCalenDBUser
    {
public:
    // construct/destruct
    CCalSvrSession();
    void ConstructL(CCalenServer& aServer);
    ~CCalSvrSession();

    // service dispatcher - from CSession
    void ServiceL(const RMessage2& aMessage);

public: // From MCalenEngineOpenCallback
    void DatabaseOpened();
    void DatabaseTemporarilyClosed();

private:
    CCalenServer* Server() const;

    void InitializeL(const RMessage2& aMessage);
    void CancelInitializeL();
    void UninitializeL();
    void HandleInitReady();
    void DoDestructL();

private:

    RMessage2 iInitMessage;
    TBool iInitActive;

    TBool iRegistered;

    };

#endif // CALSVRSESSION_H


// End of File
