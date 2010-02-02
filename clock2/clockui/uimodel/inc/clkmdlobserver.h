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
* Description:   The MClkMdlObserver for clockserver/alarmserver notifications.
*
*/

#ifndef __CLK_MDL_OBSERVER_H__
#define __CLK_MDL_OBSERVER_H__

// System includes

// User includes

// Class declaration
/**
* @class MClkModelObserver
* @brief The interface observer class. Need to be implemented by the entity which requires
*        notifications upon a certain event.
* @dll clkuimodel.dll
*/
class MClkModelObserver
    {
    public:         // Pure virtual functions
        
	/**
	* @brief This has to be implemented by derived class, so that the notifications are 
	*	      funneled to the derived class object.
	* @param aReason The cause for notification.
	*/
        virtual void HandleUpdateL( TInt aReason ) = 0;
    };

#endif          // __CLK_MDL_OBSERVER_H__

// End of file
