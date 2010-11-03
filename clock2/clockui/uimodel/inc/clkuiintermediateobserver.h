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
* Description:  The header file for CClkUiMdlIntermediateObserver class.
*
*/

#ifndef __CLKUI_INTERMEDIATE_OBSERVER_H__
#define __CLKUI_INTERMEDIATE_OBSERVER_H__

// System includes
#include <e32base.h>

// User includes
#include "clkmdlobserver.h"
// Forward declarations

// Class declaration
/**
* @class CClkUiMdlIntermediateObserver
* @brief The intermediate observer class.
* @dll clkuimodel
*/
class CClkUiMdlIntermediateObserver : public CBase, public MClkModelObserver
    {
    public:         // Constructor and destructor.
        
        /**
        * @brief C++ default constructor.
        */
        CClkUiMdlIntermediateObserver();
        
        /**
        * @brief Destructor.
        */
        ~CClkUiMdlIntermediateObserver();
        
    public:         // From base classes.
        
        /**
        * @brief From MClkUiMdlObserver. Calls the NotifyL, to notify the observer.
        * @param aReason The reason for notification.
        */
        IMPORT_C void HandleUpdateL( TInt aReason );
        
    public:         // New functions.
        
        /**
        * @brief This stores the observer.
        * @param aObserver The observer object.
        */
        IMPORT_C void SetModelObserver( MClkModelObserver* aObserver );
        
    protected:      // New functions.
        
        /**
        * @brief Notifies the observer.
        * @param aReason The reason for notification.
        */
        IMPORT_C void NotifyL( TInt aReason );
        
    private:        // Data
        
        /**
        * @var iObserver
        * @brief The model observer.
        */
        MClkModelObserver*          iObserver;
    };


#endif          // __CLKUI_INTERMEDIATE_OBSERVER_H__

// End of file
