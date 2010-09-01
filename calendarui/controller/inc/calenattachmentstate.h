/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#ifndef CALENATTACHMENTSTATE_H
#define CALENATTACHMENTSTATE_H

#include <e32base.h>
#include "calenstate.h"

/**
* Calendar attachment state
*/
class CCalenAttachmentState : public CCalenState
    {
    public:

        /**
         * Symbain constructor
         * 
         * @param aController reference to CCalenController
         * @param aOutstandingNotifications holds outstanding notifications
         */ 
        static CCalenAttachmentState* NewLC( CCalenController& aController,
                RHashSet<TCalenNotification>& aOutstandingNotifications );

        /**
         * Destructor
         */
        ~CCalenAttachmentState();
        
    public:
        
        /**
         * From CCalenState
         * Handles commands
         * 
         * @param aCommand command to be handled
         * @param aStateMachine access to the state machine
         */
        TBool HandleCommandL( const TCalenCommand& aCommand,
                              CCalenStateMachine& aStateMachine );
        /**
         * From CCalenState
         * Handles calendar notifications
         * 
         * @param aNotification any calendar notification
         * @param aStateMachine access to the state machine
         */
        void HandleNotificationL( const TCalenNotification& aNotification,
                                  CCalenStateMachine& aStateMachine ); 
        
        /**
         * Handles state activation
         * 
         * @param aStateMachine access to the state machine
         */
        void HandleStateActivationL(CCalenStateMachine& aStateMachine);
        
    private:
        
        /**
         * Default constructor
         * 
         * @param aController access to the CCalenController
         * @param aOutstandingNotifications holds outstanding notifications
         */
        CCalenAttachmentState( CCalenController& aController,
                    RHashSet<TCalenNotification>& aOutstandingNotifications );
        
        /**
         * Second phase construction
         */
        void ConstructL();
    };

#endif // CALENATTACHMENTSTATE_H

// end of file
