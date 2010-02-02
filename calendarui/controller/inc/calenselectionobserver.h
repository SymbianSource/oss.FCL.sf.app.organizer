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
* Description:   Observer class to listen for user selection on the map
*
*/


#ifndef CALENSELECTIONOBSERVER_H
#define CALENSELECTIONOBSERVER_H

#include <e32base.h>
// FORWARD DECLARATIONS
class CMnMapView;

// The observer class
class MCalenSelectionObserver 
    {
    public:
				// clients must implement it
        virtual void HandleSelectionCompletedL( CMnMapView& aMapView, TInt aError ) = 0;
    };

#endif /* CALENSELECTIONOBSERVER_H */