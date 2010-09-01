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
* Description:  The header file for CClkUiModelBase class.
*
*/

#ifndef __CLKUIMDL_BASE_H__
#define __CLKUIMDL_BASE_H__

// System includes
#include <e32base.h>
#include <e32def.h>

// Forward declarations
class CClkUiMdlListenerBase;
class MClkModelObserver;

// Class declaration
/**
* @class CClkUiModelBase
* @brief The Base class for all the model classes (alarm and clockserver).
* @dll clkuimodel.dll
*/
class CClkUiModelBase : public CBase
    {
    public:         // Constructor and destructor
        
        /**
        * @brief C++ default constructor
        */
        IMPORT_C CClkUiModelBase();
        
        /**
        * @brief Destructor
        */
        IMPORT_C virtual ~CClkUiModelBase();
        
    public:         // New functions
        
        /**
        * @brief Starts the listener.
        */
        IMPORT_C void Start();
        
        /**
        * @brief Stops the listener.
        */
        IMPORT_C void Stop();
        
        /**
        * @brief Registers the observer for notification.
        * @param aObserver of type MClkUiModelObserver.
        */
        IMPORT_C void SetModelObserver( MClkModelObserver* aObserver );
        
        /**
        * @brief Interpret listener status.
        * @param aStatus of type TInt.
        */
        virtual void ProcessListenerL( TInt aStatus ) = 0;
        
    protected:      // New functions
        
        /**
        * @brief Funnel for all calls to the notifier.
        * @param aReason The cause for the notification.
        */
        IMPORT_C void NotifyL( TInt aReason );

        /**
        * @brief Set the engine's listener as active.
        * @param aListern That has to be made active.
        */
        IMPORT_C void SetListenerActive( CClkUiMdlListenerBase* aListener );

    private:        // New functions

        /**
        * @brief A single function that actually calls the view's notify.
        * @param aReason The cause for the notification.
        */
        void DoObserverNotifyL( TInt aReason );
        
    private:        // Data
        
        /**
        * @var iListener
        * @brief Pointer to the listener object.
        */
        CClkUiMdlListenerBase*              iListener;
        
        /**
        * @var iObserver
        * @brief The model observer.
        */
        MClkModelObserver*                  iObserver;
        
        /**
        * @var iNotifying
        * @brief Will be ETrue if the Observer is being notified.
        */
        TBool                               iNotifying;
        
    };

#endif      // __CLKUIMDL_BASE_H__

// End of file
