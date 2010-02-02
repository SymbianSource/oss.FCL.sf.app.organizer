/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar alarm state machine
*
*/

#ifndef CALENALARMSTATE_H
#define CALENALARMSTATE_H

// System includes.
#include <e32base.h>

// User includes.
#include "calenstate.h"

// CLASS DEFINITIONS
/**
* Calendar viewing state
*/
class CCalenAlarmState : public CCalenState
    {
    public:
        static CCalenAlarmState* NewLC( CCalenController& aController,
                                                         RHashSet<TCalenNotification>&  aOutstandingNotifications );
        ~CCalenAlarmState();

    public:
        // From CCalenState
        TBool HandleCommandL( const TCalenCommand& aCommand,
                              CCalenStateMachine& aStateMachine );
        void HandleNotificationL(const TCalenNotification& aNotification,
                                   CCalenStateMachine& aStateMachine );
        void HandleStateActivationL(CCalenStateMachine& aStateMachine);
    private:
        CCalenAlarmState( CCalenController& aController,
                                    RHashSet<TCalenNotification>&  aOutstandingNotifications );
        void ConstructL();
    };

#endif // CALENALARMSTATE_H

// end of file
