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
* Description:  This file contains the panic codes for clockui model.
*
*/

#ifndef __CLKUIMDL_PANIC_H__
#define __CLKUIMDL_PANIC_H__

// System includes
#include <e32base.h>

// User includes

/**
* @enum TClkUiMdlPanic
* @brief The panic codes.
*/
enum TClkUiMdlPanic
    {
    EClkUiMdlWorld,
    EClkUiMdlAlarm,
    EClkUiMdlObserver,
    EClkUiMdlOutOfRange
    };

/**
* @enum TClkUiMdlFault
* @brief The fault codes.
*/
enum TClkUiMdlFault
    {
    EClkUiMdlfIdleObj,
    EClkUiMdlfPriorityInvalid,
    EClkUiMdlfIdleArgument,
    EClkUiMdlfNoNotifier,
    EClkUiMdlfOutOfRange,
    EClkUiMdlfTime
    };

// Function declarations
/**
* @brief Panic's the clkuimdl.
* @param aPanic The panic reason.
*/
GLREF_C void Panic( TClkUiMdlPanic aPanic );

/**
* @brief Flags a fault in clkuimdl.
* @param aFault The fault reason.
*/
GLREF_C void Fault( TClkUiMdlFault aFault );

#endif          // __CLKUIMDL_PANIC_H__

// End of file
