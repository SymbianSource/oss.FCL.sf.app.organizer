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
* Description:   Notification interface for S60 Calendar customizations
*
*/


#ifndef CALENCUTTENTLOCATIONOBSERVER_H
#define CALENCUTTENTLOCATIONOBSERVER_H

#include <e32base.h>
// FORWARD DECLARATIONS
class CMnMapView;

// The observer class
class MCalenCurrentLocationObserver 
    {
    public:
		// clients must implement these functions
		/**
		* Callback function to inform observers that
		* request for current location is succesfully complete
		*/
        virtual void NotifyCurrentLocationL(  ) = 0;
        
        /**
        * Callback function to inform observers that
        * request for current location is failed due to an error
        * @param errorCode 	Error Code
        */
        virtual void NotifyErrorL(const TInt errorCode) = 0;
    };

#endif /* CALENCUTTENTLOCATIONOBSERVER_H */
