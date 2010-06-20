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
* Description:  The header file for CClkUiClkSrvListener class.
*
*/

#ifndef __CLKUI_CLKSRV_LISTENER_H__
#define __CLKUI_CLKSRV_LISTENER_H__

// System includes

// User includes
#include "clkuilistenerbase.h"
#include "clkuiclksrvmodel.h"
#include "clockserverclt.h"

// Forward declarations

// Class declaration
/**
* @class CClkUiClkSrvListener
* @brief The listener class for clockserver interface.
* @dll clkuimodel.dll
*/
class CClkUiClkSrvListener : public CClkUiMdlListenerBase
    {
    public:         // Constructor.
        
        /**
        * @brief C++ default constructor.
        * @param aClkSrvSes The clockserver session.
        * @param aClkSrvModel Pointer to the clksrv model.
        * @param aWhatChanged
        * @param aWhoChanged
        * @param aListenerPriority The active object priority.
        */
        CClkUiClkSrvListener( RClkSrvInterface& aClkSrvSes, CClkUiClkSrvModel& aClkSrvModel, TInt& aWhatChanged, TInt& aWhoChanged, TInt aListenerPriority );
        
        /**
         * Destructor
         */
        ~CClkUiClkSrvListener();
        
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
        * @var iClkSrvSes
        * @brief Reference to the clockserver session.
        */
        RClkSrvInterface                iClkSrvSes;
        
        /**
        * @var iWhatChanged
        * @brief Will be filled by clockserver when providing a notification.
        */
        TInt32                          iWhatChanged;
        
        /**
        * @var iWhoChanged
        * @brief Will be filled by clockserver when providing a notification.
        */
        TInt32                          iWhoChanged;
    };

#endif          // __CLKUI_CLKSRV_LISTENER_H__

// End of file
