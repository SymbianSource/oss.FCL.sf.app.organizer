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
* Description:  The header file for CClkUiIdleObserver class.
*
*/

#ifndef __CLKUI_IDLE_OBSERVER_H__
#define __CLKUI_IDLE_OBSERVER_H__

// System includes

// User includes
#include "clkuiintermediateobserver.h"

// Forward declarations

// Class declaration
/**
* @class CClkUiIdleObserver
* @brief The idle observer class.
* @dll clkuimodel
*/
class CClkUiIdleObserver : public CClkUiMdlIntermediateObserver
    {
    public:         // Constructor and destructor.
    
        /**
        * @brief C++ default constructor.
        */
        IMPORT_C CClkUiIdleObserver();

        /**
        * @brief Destructor.
        */
        IMPORT_C ~CClkUiIdleObserver();

        /**
        * @brief The symbian OS 2nd phase constructor, can leave.
        * @param aObserver Observer of type MClkUiMdlObserver.
        * @param aPriority The priority.
        */
        IMPORT_C void ConstructL( MClkModelObserver* aObserver, TInt aPriority );

    public:         // Functions from base classes.

        /**
        * @brief From MClkUiMdlObserver. Called when the model is changed.
        * @param aReason The reason for notification.
        */
        IMPORT_C virtual void HandleUpdateL( TInt aReason );
        
    public:         // New functions

        /**
        * @brief Cancel any outstanding requests with the CIdle.
        */
        void CancelIdleRefresh();

    protected:      // New functions

        /**
        * @brief Cancel any outstanding requests with CIdle.
        */
        IMPORT_C virtual void DoIdleCancel();

        /**
        * @brief Notify the idle observer of the change.
        */
        IMPORT_C virtual void DoIdleNotifyL();

    private:        // New functions

        /**
        * @brief Starts the idle notification service.
        */
        void StartIdleRefresh();

        /**
        * @brief Callback from CIdle about any change.
        * @param aPtr Will be this object.
        * @return TInt
        */
        static TInt IdleCallBackL( TAny* aPtr );

    private:        // Data

        /**
        * @var iReason
        * @brief The reason for notification.
        */
        TInt            iReason;
        
        /**
        * @var iIdle
        * @brief The idle object, owned.
        */
        CIdle*          iIdle;
    };

#endif      // __CLKUI_IDLE_OBSERVER_H__

// End of file
