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
* Description:  Calendar multiple db
*
*/


#include "calendarui_debug.h"           // Debug macros
#include "calenmultipledbmanager.h"
#include "calencontroller.h"

// ----------------------------------------------------------------------------
// CCalenMultipleDbManager::CCalenMultipleDbManager
// Constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenMultipleDbManager::CCalenMultipleDbManager()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbManager::NewL
// Constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenMultipleDbManager* CCalenMultipleDbManager::NewL()
    {
    TRACE_ENTRY_POINT
    CCalenMultipleDbManager* self = new(ELeave) CCalenMultipleDbManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_EXIT_POINT
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbManager::~CCalenAlarmManager
// Destructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenMultipleDbManager::~CCalenMultipleDbManager()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbManager::ConstructL()
// 2nd phase
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbManager::ConstructL()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbManager::HandleCommandL()
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenMultipleDbManager::HandleCommandL(const TCalenCommand& /*aCommand*/ )
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbManager::CalenCommandHandlerExtensionL
// Dummy implementation.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TAny* CCalenMultipleDbManager::CalenCommandHandlerExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenMultipleDbManager::HandleNotificationL()
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMultipleDbManager::HandleNotification(TCalenNotification /*aNotification*/ )
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    }

