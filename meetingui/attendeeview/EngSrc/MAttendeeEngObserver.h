/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides an observer interface to attendee view engine.
*
*/




#ifndef __MATTENDEE_ENG_OBSERVER_H__
#define __MATTENDEE_ENG_OBSERVER_H__

// CLASS DECLARATIONS
/**
*  Engine provides this API only for attendee view
*/
class MAttendeeEngObserver
    {
    public: // New functions
        /**
        * Notifies the engine observer about occured event.
        * Engine observer has to update its data when this function is called.
        */
        virtual void NotifyL() = 0;

        /**
        * Called if occured event causes a leave in previous NotifyL method.
        * @param aErrorCode Occured general errorcode.
        */
        virtual void NotifyError( TInt aErrorCode ) = 0;
    };

#endif      // __MATTENDEE_ENG_OBSERVER_H__

// End of File
