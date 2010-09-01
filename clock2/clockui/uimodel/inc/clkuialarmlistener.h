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
* Description:  The header file for CClkUiMdlAlarmListener class.
*
*/

#ifndef __CLKUI_ALARM_LISTENER_H__
#define __CLKUI_ALARM_LISTENER_H__

// System includes
#include <asclisession.h>

// User includes
#include "clkuilistenerbase.h"

// Forward declarations
class CClkUiAlarmModel;

// Class declaration
/**
* @class CClkUiMdlAlarmListener
* @brief The listener class for alarm server interface.
* @dll clkuimodel.dll
*/
class CClkUiMdlAlarmListener : public CClkUiMdlListenerBase
    {
    public:         // Constructor.
        
        /**
        * @brief C++ default constructor.
        * @param aAlarmSrvSes The alarm server session.
        * @param aAlarmModel Pointer to the alarm model.
        * @param aListenerPriority The active object priority.
        */
        CClkUiMdlAlarmListener( RASCliSession& aAlarmSrvSes, CClkUiAlarmModel& aAlarmModel, TInt aListenerPriority );
        
        /**
        * @brief Destructor.
        */
        ~CClkUiMdlAlarmListener();
        
    public:         // Functions from base classes.
        
        /**
        * @brief from CClkUiMdlListenerBase. Starts the listener activity.
        */
        void DoStart();
        
        /**
        * @brief from CClkUiMdlListenerBase. Cancels and stops the listener activity.
        */
        void DoCancel();
        
    private:        // Data
        
        /**
        * @var iAlarmSrvSes
        * @brief Reference to the alarm server session.
        */
        RASCliSession               iAlarmSrvSes;
        
        /**
        * @var iAlarmId
        * @brief Dummy alarm id.
        */
        TInt                        iAlarmId;
    };

#endif          // __CLKUI_ALARM_LISTENER_H__

// End of file
