/*
 * Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Global Data for Calendar application
 *
 */
#include "calendarui_debug.h"
#include "calenfilemapping.h"

#include <calsession.h>
#include <calentryview.h>           // Calendar Entry view

// -----------------------------------------------------------------------------
// CCalenFileMapping::CCalenFileMapping
// -----------------------------------------------------------------------------
CCalenFileMapping::CCalenFileMapping( ) 
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT        
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::NewL
// -----------------------------------------------------------------------------
CCalenFileMapping* CCalenFileMapping::NewL()
    {
    TRACE_ENTRY_POINT    
    CCalenFileMapping* selfPtr = new(ELeave) CCalenFileMapping();
    CleanupStack::PushL(selfPtr);
    selfPtr->ConstructL();
    CleanupStack::Pop(selfPtr);
    TRACE_EXIT_POINT       
    return selfPtr;
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::ConstructL
// -----------------------------------------------------------------------------
void CCalenFileMapping::ConstructL() // second-phase constructor
    {
    TRACE_ENTRY_POINT
    iStatus = EFalse;
    iSessionPtr = NULL;
    iEntryView = NULL;

    if(iCalendarFileName)
        {
        delete iCalendarFileName;
        iCalendarFileName = NULL;
        }
    TRACE_EXIT_POINT           
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::~CCalenFileMapping
// -----------------------------------------------------------------------------
CCalenFileMapping::~CCalenFileMapping()
    {
    TRACE_ENTRY_POINT
    
    if(iEntryView)
        {
        delete iEntryView;
        iEntryView = NULL;
        }

    if(iSessionPtr)
        {
        delete iSessionPtr;
        iSessionPtr = NULL;
        }

    if(iCalendarFileName)
        {
        delete iCalendarFileName;
        iCalendarFileName = NULL;
        }

    TRACE_EXIT_POINT 
    }


// -----------------------------------------------------------------------------
// CCalenFileMapping::GetCalendarFileName
// -----------------------------------------------------------------------------      
const TDesC& CCalenFileMapping::GetCalendarFileName() const
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return *iCalendarFileName;
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::SetCalendarFileNameL
// ----------------------------------------------------------------------------- 
void CCalenFileMapping::SetCalendarFileNameL(const TDesC& aCalendarFilename)
    {
    TRACE_ENTRY_POINT
    if(iCalendarFileName)
        {
        delete iCalendarFileName;
        iCalendarFileName = NULL;
        }
    iCalendarFileName = aCalendarFilename.AllocL();
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::SetCollectionId
// ----------------------------------------------------------------------------- 
void CCalenFileMapping::SetCollectionId(TCalCollectionId aColId)
    {
    TRACE_ENTRY_POINT    
    iColId = aColId;    
    TRACE_EXIT_POINT    
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::GetCollectionId
// -----------------------------------------------------------------------------   
TCalCollectionId CCalenFileMapping::GetCollectionId() const
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT    
    return iColId;
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::GetStatus
// ----------------------------------------------------------------------------- 
TBool CCalenFileMapping::GetStatus()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return iStatus;    
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::SetStatus
// ----------------------------------------------------------------------------- 
void CCalenFileMapping::SetStatus(TBool aStatus)
    {
    TRACE_ENTRY_POINT
    iStatus = aStatus;
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::SetEntryView
// ----------------------------------------------------------------------------- 
void CCalenFileMapping::SetEntryView(CCalEntryView* aEntryView)
    {
    TRACE_ENTRY_POINT
    iEntryView = aEntryView;
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::GetEntryView
// ----------------------------------------------------------------------------- 
CCalEntryView* CCalenFileMapping::GetEntryView()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return iEntryView;
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::SetSessionPtr
// ----------------------------------------------------------------------------- 
void CCalenFileMapping::SetSessionPtr(CCalSession* aSessionPtr)
    {
    TRACE_ENTRY_POINT
    iSessionPtr = aSessionPtr;
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenFileMapping::GetSessionPtr
// ----------------------------------------------------------------------------- 
CCalSession* CCalenFileMapping::GetSessionPtr()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return iSessionPtr;
    }

//End
