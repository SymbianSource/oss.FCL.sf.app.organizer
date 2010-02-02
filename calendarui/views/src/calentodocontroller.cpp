/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  To-Do view's utility class.
*
*/

//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "calentodocontroller.h"

#include <calenagendautils.h>
#include <calendateutils.h>
#include "calentodocontainer.h"

#include <calenservices.h>
#include "calennativeview.h"

#include <sysutil.h>
#include <ErrorUI.h>

#include <calinstance.h>
#include <calinstanceview.h>
#include <calentry.h>
#include <calentryview.h>
#include <calrrule.h>
#include <caltime.h>
#include <calcommon.h>
#include <eikenv.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenviewutils.h>

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCalenTodoController::CCalenTodoController
// C++ default constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenTodoController::CCalenTodoController( 
        MCalenServices& aServices )
    : iServices( aServices )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoController::ConstructL
// Symbian OS default constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoController::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoController::~CCalenTodoController
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenTodoController::~CCalenTodoController()
    {
    TRACE_ENTRY_POINT;

    CleanupInstances();
    iActiveListIndices.Reset();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoController::RefreshL
// Refresh agenda name list.
// ----------------------------------------------------------------------------
//
void CCalenTodoController::RefreshL()
    {
    TRACE_ENTRY_POINT;

    iList.ResetAndDestroy();
    iActiveListIndices.Reset();
    
    CalCommon::TCalViewFilter filter = CalCommon::EIncludeCompletedTodos   |
                                       CalCommon::EIncludeIncompletedTodos |
                                       // only fetch the first instance for repeating to-dos!
                                       CalCommon::EIncludeRptsNextInstanceOnly;

    TCalTime start;  start.SetTimeLocalL( TCalTime::MinTime() );
    TCalTime end;  end.SetTimeLocalL( TCalTime::MaxTime() );

    CalCommon::TCalTimeRange timeRange( start, end );
    
    RArray<TInt> colIdArray;
    CCalenNativeView::GetActiveCollectionidsL( iServices, colIdArray );
    
    if(colIdArray.Count() > 0)
        { 
        iServices.InstanceViewL(colIdArray)->FindInstanceL( iList, filter, timeRange );
        }    
        
    colIdArray.Reset();
    
    CalenAgendaUtils::SortInstanceList( iList );
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoController::InstanceList
// Return ToDo instance list pointer.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
RPointerArray<CCalInstance>& CCalenTodoController::InstanceList()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iList;
    }

// ----------------------------------------------------------------------------
// CCalenTodoController::SetToStartL
// Index is setted to ZERO and fetch data.
// ----------------------------------------------------------------------------
//
void CCalenTodoController::SetToStartL()
    {
    TRACE_ENTRY_POINT;

    iIndex = 0;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoController::SetToNextL
// Index is incremented and fetch data.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenTodoController::SetToNextL()
    {
    TRACE_ENTRY_POINT;

    ++iIndex;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenTodoController::IsAtEnd
// Check terminus of index.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenTodoController::IsAtEnd() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return (iIndex == iList.Count());
    }

// ----------------------------------------------------------------------------
// CCalenTodoController::Instance
// Return instance id of current index.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalInstance* CCalenTodoController::Instance() const
    {
    TRACE_ENTRY_POINT;

    if( iIndex < 0 || iIndex >= Count() )
        {
        TRACE_EXIT_POINT;
        return NULL;
        }

    TRACE_EXIT_POINT;
    return iList[ iIndex ];
    }

// ----------------------------------------------------------------------------
// CCalenTodoController::InstanceId
// Return instance id of the index.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalInstance* CCalenTodoController::InstanceL(TInt aIndex) const     // Index of list
    {
    TRACE_ENTRY_POINT;
    //1.iActiveListIndices contains iList indices as values.
    //2.iActiveListIndices is populated in CreateEntryItertorL of container
    
    if( aIndex < 0 || aIndex >= Count() || iActiveListIndices.Count() <= 0 )
        {
        TRACE_EXIT_POINT;
        return NULL;
        }

    TRACE_EXIT_POINT;
    return iList[ iActiveListIndices[aIndex] ];
    }

// ----------------------------------------------------------------------------
// CCalenTodoController::IsCrossOutL
// Check status of cross out.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenTodoController::IsCrossOutL(TInt aIndex) // Index of list
    {
    TRACE_ENTRY_POINT;

    TBool crossout( EFalse );

    if( iList.Count() )
        {
        CCalInstance* instance = InstanceL( aIndex );
        CCalEntry& entry = instance->Entry();

        if( entry.StatusL() == CCalEntry::ETodoCompleted )
            {
            crossout = ETrue;
            }
        }

    TRACE_EXIT_POINT;
    return crossout;
    }

// ----------------------------------------------------------------------------
// CCalenTodoController::Count
// Return number of item.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenTodoController::Count() const
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    return iList.Count();
    }


TInt CCalenTodoController::FindInstanceIndexL(const TCalenInstanceId& aId) const
    {
    TRACE_ENTRY_POINT;

    for(TInt index(0); index < iList.Count(); ++index)
        {
        if( iList[index]->Entry().LocalUidL() == aId.iEntryLocalUid )
            {
            // FIXME, if several instances match, we should one
            // that closest matches aId.iInstanceTime
            TRACE_EXIT_POINT;
            return index;
            }
        }

    TRACE_EXIT_POINT;
    return KErrNotFound;
    }

void CCalenTodoController::CleanupInstances()
    {
    TRACE_ENTRY_POINT
    iList.ResetAndDestroy();
    TRACE_EXIT_POINT
    }
// End of File
