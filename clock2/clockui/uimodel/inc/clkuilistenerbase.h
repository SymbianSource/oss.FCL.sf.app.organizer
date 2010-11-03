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
* Description:  The header file for CClkUiMdlListenerBase class.
*
*/

#ifndef __CLKUI_LISTENER_BASE_H__
#define __CLKUI_LISTENER_BASE_H__

// System includes
#include <e32base.h>

// User includes

// Forward declarations
class CClkUiModelBase;

// Class declaration
/**
* @class CClkUiMdlListenerBase
* @brief The base class for all the listeners.
* @dll clkuimodel.dll
*/
class CClkUiMdlListenerBase : public CActive
    {
    public:         // New functions
        
        /**
        * @brief Destructor
        */
        virtual ~CClkUiMdlListenerBase();
        
        /**
        * @brief Start the listener.
        */
        void Start();
        
        /**
        * @brief Needs to be implemented in the derived class. This subscribes for notification from
        *        the service provider.
        */
        virtual void DoStart() = 0;
        
    public:         // Functions from base classes
        
        /**
        * @brief From CActive. Called when the value of iStatus changes.
        */
        IMPORT_C void RunL();
        
        /**
        * @brief From CActive. Needs to be implemented by the derived class. This cancel any 
        *        subscriptions made with the service provider.
        */
        virtual void DoCancel() = 0;
        
    protected:      // New functions
        
        /**
        * @brief C++ default constructor.
        * @param aModel The model object, which is using the listener.
        * @param aPriority The AO priority, passed onto CActive while construction.
        */
        IMPORT_C CClkUiMdlListenerBase( CClkUiModelBase& aModel, TInt aPriority );
        
    private:        // New functions
        
        /**
        * @brief Funnels the notification to the model.
        * @param aReason The cause for the notification.
        */
        void NotifyModelL( TInt aReason );
        
    private:        // Data
        
        /**
        * @var iModel
        * @brief The model that's using the services of the listener.
        */
        CClkUiModelBase*                iModel;
    };

#endif          // __CLKUI_LISTENER_BASE_H__

// End of file
