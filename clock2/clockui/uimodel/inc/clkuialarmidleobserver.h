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
* Description:  the header file for CClkUiAlarmIdleObserver class.
*
*/

#ifndef __CLKUI_ALARM_IDLE_OBSERVER_H__
#define __CLKUI_ALARM_IDLE_OBSERVER_H__

// System includes

// User includes
#include "clkuiidleobserver.h"

// Forward declarations

// Class declaration
/**
* @class CClkUiAlarmIdleObserver
* @brief The alarm idle observer class.
* @dll clkuimodel
*/
class CClkUiAlarmIdleObserver : public CClkUiIdleObserver
    {
    public:             // Constructor
        
        /**
        * @brief 2-phased constructor.
        * @param aObserver Observer of type MClkUiMdlObserver.
        * @param aListenerPriority The listener priority.
        * @return CClkUiAlarmIdleObserver* Pointer to the constructed object.
        */
        IMPORT_C static CClkUiAlarmIdleObserver* NewL( MClkModelObserver* aObserver, TInt aListenerPriority );
    };

#endif          // __CLKUI_ALARM_IDLE_OBSERVER_H__

// End of file
