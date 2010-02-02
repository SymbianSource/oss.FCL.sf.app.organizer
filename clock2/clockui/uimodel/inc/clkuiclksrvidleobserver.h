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
* Description:  The header file for  CClkUiClkSrvIdleObserver class.
*
*/

#ifndef __CLKUI_CLKSRV_IDLE_OBSERVER_H__
#define __CLKUI_CLKSRV_IDLE_OBSERVER_H__

// System includes

// User includes
#include "clkuiidleobserver.h"

// Forward declarations

// Class declaration
/**
* @class CClkUiClkSrvIdleObserver
* @brief The clock server idle observer.
* @dll clkuimodel.dll
*/
class CClkUiClkSrvIdleObserver : public CClkUiIdleObserver
    {
    public:             // Constructor
        
        /**
        * @brief 2-phased constructor.
        * @param aObserver Observer of type MClkUiMdlObserver.
        * @param aListenerPriority The listener priority.
        * @return CClkUiClkSrvIdleObserver* Pointer to the constructed object.
        */
        static CClkUiClkSrvIdleObserver* NewL( MClkModelObserver* aObserver, TInt aListenerPriority );
    };

#endif          // __CLKUI_CLKSRV_IDLE_OBSERVER_H__

// End of file
