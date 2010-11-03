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
* Description:  The model part of popup
*/

#include <calinstance.h>
#include <calenagendautils.h>
#include <caleninstanceid.h>            // TCalenInstanceId
#include <calenviewutils.h>
#include <calenservices.h>
#include <centralrepository.h>
#include <calcalendarinfo.h>

#include "calendarui_debug.h"
#include "calenpreviewdata.h"
#include "calenpreviewlayoutmanager.h"
#include "calenpreviewentry.h"
#include "calenglobaldata.h"
#include "calendateutils.h"
#include "CalendarPrivateCRKeys.h"
#include "calenglobaldata.h"
#include "calennativeview.h"

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewData::CCalenPreviewData(
    CCalenPreviewLayoutManager *aLayoutManager,
    MCalenServices& aServices,
    TInt aMaxInstances)
    : iLayoutManager(aLayoutManager), 
      iNotShownEntries(EFalse),
      iMaxInstances(aMaxInstances),
      iServices( aServices )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewData::ConstructL(const TTime& aDay)
    {
    TRACE_ENTRY_POINT;
    FindInstancesL(aDay);
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewData* CCalenPreviewData::NewL(
    CCalenPreviewLayoutManager *aLayoutManager,
    MCalenServices& aServices,
    TInt aMaxInstances,
    TTime aFocusedTime)
    {
    TRACE_ENTRY_POINT;

    CCalenPreviewData* self = new( ELeave ) CCalenPreviewData(aLayoutManager,
                                                              aServices,
                                                              aMaxInstances);
    CleanupStack::PushL( self );
    self->ConstructL(aFocusedTime);
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
    }
// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewData* CCalenPreviewData::NewL(
    CCalenPreviewLayoutManager *aLayoutManager,
    MCalenServices& aServices,
    TInt aMaxInstances,
    CCalInstance* aInst)
    {
    TRACE_ENTRY_POINT;

    CCalenPreviewData* self = new( ELeave ) CCalenPreviewData(aLayoutManager,
                                                              aServices, 
                                                              aMaxInstances);
    CleanupStack::PushL( self );
    self->iSingleInstance = ETrue;
    self->iInstances.Append(aInst);
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewData::~CCalenPreviewData()
    {
    TRACE_ENTRY_POINT;
    if(iLayoutManager)
        iLayoutManager->SetLinesUsed(0);
    /* Single instance is not owned by CalenPreviewData 
       so it is not deleted here*/
    iTodoPriorityOrder.Close();
    if(!iSingleInstance)
        iInstances.ResetAndDestroy();
    iInstances.Close();
    iEntries.ResetAndDestroy();
    iEntries.Close();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CCalenPreviewData::InstanceCount() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iInstances.Count();
    }

TInt CCalenPreviewData::EntryCount() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iEntries.Count();
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenPreviewData::NotShownEntries() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iNotShownEntries;
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewData::SetToday(TBool aToday) 
    {
    TRACE_ENTRY_POINT;
    iToday = aToday;
    TRACE_EXIT_POINT;
    }
// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TBool CCalenPreviewData::Today() 
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iToday;
    }

// ---------------------------------------------------------------------------
// CCalenPreviewData::RemoveInstancesL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewData::RemoveInstancesL(const CCalEntry::TType aType)
    {
    TRACE_ENTRY_POINT;
    
    TInt i( iInstances.Count() - 1 );

    if(aType == CCalEntry::ETodo)
        {
        for(TInt i(0); i < iTodoPriorityOrder.Count() && iInstances.Count() > iMaxInstances; i++)
            {
            for(TInt j(0); j < iInstances.Count() && iInstances.Count() > iMaxInstances; j++)
                {
                if( iTodoPriorityOrder[i] == iInstances[j] )
                    {
                    delete iInstances[j];
                    iInstances.Remove(j);
                    iTodoPriorityOrder.Remove(i);
                    i--;
                    iNotShownEntries = ETrue;
                    break;
                    }   
                }
            }
        }
    else
        {
        TInt i( iInstances.Count() - 1 );
        while( i >= 0 && 
               iInstances.Count() > iMaxInstances )
            {
            if( iInstances[i]->Entry().EntryTypeL() == aType )
                {
                delete iInstances[i];
                iInstances.Remove(i);
                iNotShownEntries = ETrue;
                }
            i--;
            }
        }
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPreviewData::RemoveInstances
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewData::RemoveInstances()
    {
    TRACE_ENTRY_POINT;
    
    TInt i( 0 );

    while( iInstances.Count() > iMaxInstances )
        {
        i = iInstances.Count() - 1;
        delete iInstances[i];
        iInstances.Remove( i );
        iNotShownEntries = ETrue;
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPreviewData::RemoveInstance
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewData::RemoveInstance(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    
    if( aIndex < iInstances.Count() && aIndex >= 0 )
        {
        delete iInstances[aIndex];
        iInstances.Remove(aIndex);
        }

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPreviewData::DeleteInstances
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewData::DeleteInstances()
    {
    TRACE_ENTRY_POINT;
    iInstances.ResetAndDestroy();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenPopupDataBuilder::FindInstancesL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewData::FindInstancesL(const TTime& aDay)
    {
    TRACE_ENTRY_POINT;

    iInstances.ResetAndDestroy();
    
    RArray<TInt> colIdArray;
    CCalenNativeView::GetActiveCollectionidsL( iServices, colIdArray );
                
    if(colIdArray.Count() > 0)
        {  
          CalenAgendaUtils::CreateEntryIdListForDayL( iInstances,
                                iServices.InstanceViewL(colIdArray),
                                aDay,
                              ETrue/*custom sort for popup*/ );
          
        }
    else
          {
          CalenAgendaUtils::CreateEntryIdListForDayL( iInstances,
                                            iServices.InstanceViewL(),
                                            aDay,
                                          ETrue/*custom sort for popup*/ );      
          }
    
    colIdArray.Reset();
    TRACE_EXIT_POINT;  
    }

// ---------------------------------------------------------------------------
// CCalenPreviewData::GetInstance
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalInstance* CCalenPreviewData::Instance(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iInstances[aIndex];
    }
// ---------------------------------------------------------------------------
// CCalenPreviewData::GetInstance
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
CCalenPreviewEntry* CCalenPreviewData::Entry(TInt aIndex)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iEntries[aIndex];
    }

// ---------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenPreviewData::CreateEntriesL(const TTime& aDay)
    {
    TRACE_ENTRY_POINT;
    TInt rowsUsed(0);
    CCalenPreviewEntry* temp = NULL;
    
    /* Calculate need for each row */    
    TInt maxRows = iLayoutManager->UsePreview() ? 
    (iMaxInstances - iInstances.Count() + 1) : 1;
    
    TInt count = iInstances.Count();
    if(iInstances.Count() > 0)
        {
        RPointerArray<CCalCalendarInfo> calendarInfoList;
        iServices.GetAllCalendarInfoL(calendarInfoList);
        CleanupClosePushL(calendarInfoList); 
        
        for( TInt i(0); i < iInstances.Count(); i++)
            {
            CCalEntry& aEntry = iInstances[i]->Entry();
            TCalCollectionId colId = iInstances[i]->InstanceIdL().iCollectionId;
            HBufC* calendarFileName = iServices.GetCalFileNameForCollectionId(colId).AllocLC();
            TInt calIndex = calendarInfoList.Find(*calendarFileName, 
                        CCalenPreviewData::CalendarInfoIdentifierL);
            CleanupStack::PopAndDestroy(calendarFileName);
            if(calIndex != KErrNotFound && calendarInfoList[calIndex]->Enabled())
                {
                temp = CCalenPreviewEntry::NewL(iInstances[i], iLayoutManager,
                aDay, maxRows,calendarInfoList[calIndex]->Color().Value());
                iEntries.AppendL(temp);
                rowsUsed += temp->LineCount();
                }
            }
        CleanupStack::PopAndDestroy(&calendarInfoList);
        }
    else if(iLayoutManager->UsePreview())
        {
        /* Create entry for empty text */
        temp = CCalenPreviewEntry::NewL(
        iLayoutManager,
        CCalenPreviewEntry::ECalenPreviewEntryEmpty,0);
        iEntries.AppendL(temp);
        rowsUsed += temp->LineCount();
        }

    /* Calculate row division */
    TInt rows = maxRows;
    while(rowsUsed > iMaxInstances)
        {
        RemoveLine( CCalEntry::ETodo, rowsUsed, rows );
        if(rowsUsed == iMaxInstances)
            break;
        RemoveLine( CCalEntry::EAnniv, rowsUsed, rows );
        if(rowsUsed == iMaxInstances)
            break;
        RemoveLine( CCalEntry::EEvent, rowsUsed, rows );
        if(rowsUsed == iMaxInstances)
            break;

        RemoveMeetingLineL( rowsUsed, rows );
        rows--;
        }

    for(TInt i(0); i < iEntries.Count(); i++)
        {
        TBool lastLine(EFalse);
        if( i == iEntries.Count() - 1 &&
            NotShownEntries())
            lastLine = ETrue;
        iEntries[i]->FormatTextL(lastLine);
        }

    iLayoutManager->SetLinesUsed(rowsUsed);
    TRACE_EXIT_POINT;
    }

void CCalenPreviewData::RemoveLine(const CCalEntry::TType aType,
                                   TInt& aRowsUsed, TInt aRows)
    {
    TRACE_ENTRY_POINT;

    if(aType == CCalEntry::ETodo)
        {
        // use todo priority list
        for(TInt i(0); i < iTodoPriorityOrder.Count() && aRowsUsed > iMaxInstances; i++)
            {
            for(TInt j(0); j < iInstances.Count() && aRowsUsed > iMaxInstances; j++)
                {
                if( iTodoPriorityOrder[i] == iInstances[j] &&
                    iEntries[j]->LineCount() == aRows && 
                    iEntries[j]->RemoveLine())
                    {
                    aRowsUsed--;
                    break;
                    }
                }   
            }
        }
    else
        {
        for(TInt i = iEntries.Count()-1; 
            i >= 0 && aRowsUsed > iMaxInstances; i--)
            {
            if( iEntries[i]->EventType() == aType &&
                iEntries[i]->LineCount() == aRows && 
                iEntries[i]->RemoveLine())
                {
                aRowsUsed--;
                }
            }
        }
    TRACE_EXIT_POINT;
    }

void CCalenPreviewData::RemoveMeetingLineL(TInt& aRowsUsed, TInt aRows)
    {
    TRACE_ENTRY_POINT;

    if(iToday)
        {
        // Past events
        for(TInt i(0); i < iEntries.Count() && aRowsUsed > iMaxInstances; i++)
            {
            if( ( iEntries[i]->EventType() == CCalEntry::EAppt ||
                  iEntries[i]->EventType() == CCalEntry::EReminder ) &&
                iInstances[i]->EndTimeL().TimeLocalL() < iCurrent)
                {
                if(iEntries[i]->LineCount() == aRows && 
                   iEntries[i]->RemoveLine())
                    {
                    aRowsUsed--;
                    }
                }
            }
        // Ongoing meetings, start time less than 30 from current time
        TTime range(iCurrent - TTimeIntervalMinutes(iRange));
        for(TInt i(0); i < iEntries.Count() && aRowsUsed > iMaxInstances; i++)
            {
            if( (iEntries[i]->EventType() == CCalEntry::EAppt ||
                 iEntries[i]->EventType() == CCalEntry::EReminder) &&
                iInstances[i]->EndTimeL().TimeLocalL() > iCurrent &&
                iInstances[i]->StartTimeL().TimeLocalL() < range)
                {
                if(iEntries[i]->LineCount() == aRows && 
                   iEntries[i]->RemoveLine())
                    {
                    aRowsUsed--;
                    }
                }
            }
        // Upcoming events
        for(TInt i(iEntries.Count()-1); i >= 0  && aRowsUsed > iMaxInstances; i--)
            {
            if( (iEntries[i]->EventType() == CCalEntry::EAppt ||
                 iEntries[i]->EventType() == CCalEntry::EReminder) &&
                iInstances[i]->StartTimeL().TimeLocalL() > iCurrent)
                {
                if(iEntries[i]->LineCount() == aRows && 
                   iEntries[i]->RemoveLine())
                    {
                    aRowsUsed--;
                    }
                }
            }
        // Ongoing in range
        for(TInt i(0); i < iEntries.Count() && aRowsUsed > iMaxInstances; i++)
            {
            if( (iEntries[i]->EventType() == CCalEntry::EAppt ||
                 iEntries[i]->EventType() == CCalEntry::EReminder) &&
                iInstances[i]->StartTimeL().TimeLocalL() > range && 
                iInstances[i]->StartTimeL().TimeLocalL() < iCurrent &&
                iInstances[i]->EndTimeL().TimeLocalL() > iCurrent)
                {
                if(iEntries[i]->LineCount() == aRows && 
                   iEntries[i]->RemoveLine())
                    {
                    aRowsUsed--;
                    }
                }
            }
        }
    else
        {
        for(TInt i = iEntries.Count()-1; i >= 0 && aRowsUsed > iMaxInstances; i--)
            {
            if( iEntries[i]->EventType() == CCalEntry::EAppt ||
                iEntries[i]->EventType() == CCalEntry::EReminder)
                {
                if(iEntries[i]->LineCount() == aRows && 
                   iEntries[i]->RemoveLine())
                    {
                    aRowsUsed--;
                    }
                }
            }
        }
    TRACE_EXIT_POINT;
    }

void CCalenPreviewData::PrioritizeTodosL()
    {
    TRACE_ENTRY_POINT;
    for(TInt i(0); i < iInstances.Count(); i++)
        {
        if( iInstances[i]->Entry().EntryTypeL() == CCalEntry::ETodo )
            {
            iTodoPriorityOrder.AppendL(iInstances[i]);
            }
        }

    TLinearOrder<CCalInstance> instanceListOrder( CCalenPreviewData::TodoPrioCompare );
    iTodoPriorityOrder.Sort( instanceListOrder );
    TRACE_EXIT_POINT;
    }

void CCalenPreviewData::SetTimedRangeL(const TTime aCurrent, const TInt aRange)
    {
    TRACE_ENTRY_POINT;
    iCurrent = aCurrent;
    iRange = aRange;
    TRACE_EXIT_POINT;
    }

TInt CCalenPreviewData::TodoPrioCompare(const CCalInstance& aInstance_1, 
                                        const CCalInstance& aInstance_2)
    {
    TRACE_ENTRY_POINT;

    const CCalEntry& entry_1 = aInstance_1.Entry();
    const CCalEntry& entry_2 = aInstance_2.Entry();

    TInt res = KErrArgument;
    PIM_TRAPD_HANDLE( (res = DoTodoPrioCompareL(entry_1, entry_2)) );
    
    TRACE_EXIT_POINT;    
    return res;
    }

TInt CCalenPreviewData::DoTodoPrioCompareL(const CCalEntry& aEntry_1, 
                                           const CCalEntry& aEntry_2)
    {
    TRACE_ENTRY_POINT;

    TInt ret( 0 );
    CCalEntry::TStatus status_1 = aEntry_1.StatusL();
    CCalEntry::TStatus status_2 = aEntry_2.StatusL();

    if( status_1 == CCalEntry::ENullStatus )  status_1 = CCalEntry::ETodoNeedsAction;
    if( status_2 == CCalEntry::ENullStatus )  status_2 = CCalEntry::ETodoNeedsAction;

    if( status_1 == status_2 )
        {
        TTime time_1 = aEntry_1.EndTimeL().TimeUtcL();
        TTime time_2 = aEntry_2.EndTimeL().TimeUtcL();
        
        if( time_1 == time_2 )
            {
            const TUint pri_1( aEntry_1.PriorityL() );
            const TUint pri_2( aEntry_2.PriorityL() );
            
            if( pri_1 == pri_2 )
                {
                time_1 = aEntry_1.LastModifiedDateL().TimeUtcL();
                time_2 = aEntry_2.LastModifiedDateL().TimeUtcL();
                
                if( time_1 == time_2 )     ret = 0;
                else if( time_1 > time_2 ) ret = -1;
                else                       ret = 1;
                }
            else
                {
                if( pri_1 > pri_2 ) ret = -1;
                else                ret = 1;
                }
            }
        else
            {
            if( time_1 > time_2 ) ret = 1;
            else                  ret = -1;
            }
        }
    else
        {
        if( status_1 == CCalEntry::ETodoCompleted ) ret = 1;
        else                                        ret = -1;
        }
    TRACE_EXIT_POINT;
    return ret;
    }

// --------------------------------------------------------------------------
// CCalenPreviewData::ProcessMonthInstancesL
// ?implementation_description
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void CCalenPreviewData::ProcessMonthInstancesL(const TTime aRangeStart, 
                                               const TBool aIsToday)
    {
    TRACE_ENTRY_POINT;
    /**
     * Priority order
     * by types:
     *  1. EAppt (and EReminder)
     *  2. ETodo
     *  3. EAnniv
     *  4. EEvent 
     * by properties:
     *  1. date/time (start time for EAppt and EReminder)
     *  2. by duration (shotest 1st) (only EAppt and EReminder) 
     *     FIXME: AL - EReminder has no duration...do we prefer EAppts?
     *  3. priority (only ETodo)
     *  4. by modified date (oldest 1st)
     **/

    // remove all excess entries

    PrioritizeTodosL();    

    if( iInstances.Count() > iMaxInstances )
        {
        RemoveInstancesL( CCalEntry::ETodo );

        RemoveInstancesL( CCalEntry::EAnniv );
        RemoveInstancesL( CCalEntry::EEvent );
 
        
        TInt i( 0 );
        // ...next, all 'expired' timed notes (applies only for 'today')...
        while( i < iInstances.Count() && 
               iInstances.Count() > iMaxInstances && 
               aIsToday )
            {
            CCalInstance* inst = iInstances[i];
            CCalEntry& entry = inst->Entry();
            CCalEntry::TType type = entry.EntryTypeL();
            TTime entryStart(0);

            TBool timedEntry( type == CCalEntry::EAppt || 
                              type == CCalEntry::EReminder );

            // StartTimeL for undated to-dos will leave w/ KErrArgument!
            if( timedEntry )
                {
                entryStart = inst->StartTimeL().TimeLocalL();
                }
            //  past or (timed entry AND out of time range)
            if( timedEntry && 
                ( inst->EndTimeL().TimeLocalL() < iCurrent || 
                  entryStart < aRangeStart ) )
                {
                RemoveInstance(i);
                iNotShownEntries = ETrue;
                }
            else
                {
                i++;
                }
            }
        // ...and finally all timed notes that doesn't fit
        RemoveInstances();
        }
    
    TRACE_EXIT_POINT;
    }

// --------------------------------------------------------------------------
// CCalenPreviewData::ProcessWeekInstancesL
// ?implementation_description
// (other items were commented in a header).
// --------------------------------------------------------------------------
//
void CCalenPreviewData::ProcessWeekInstancesL(const TTime aStart, 
                                              const TTime aEnd)
    {
    TRACE_ENTRY_POINT;

    TInt i( 0 );

    // remove entries that do not intersect with the time range
    while( i < iInstances.Count() )
        {
        CCalEntry& entry = iInstances[i]->Entry();
        CCalEntry::TType type = entry.EntryTypeL();
        TTime entryStart( TInt64(0) );
        TTime entryEnd( TInt64(0) );

        TBool timedEntry( type == CCalEntry::EAppt || type == CCalEntry::EReminder 
                          || CalenViewUtils::IsAlldayEventL( *( iInstances[i] ) ) );

        // calling StartTimeL for undated to-do will leave w/ KErrArgument!
        if( timedEntry )
            {
            entryStart = iInstances[i]->StartTimeL().TimeLocalL();
            entryEnd = iInstances[i]->EndTimeL().TimeLocalL();
            }

        // (timed entry AND doesn't intersect with the time range)
        //  OR
        // (non-timed entry)
        if( !timedEntry ||
            (timedEntry && !CalenDateUtils::TimeRangesIntersect(aStart, aEnd, entryStart, entryEnd)) )
            {
            RemoveInstance(i);
            }
        else
            {
            i++;
            }
        }

    // remove timed notes that doesn't fit (starting from bottom)
    RemoveInstances();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenPreviewData::CalendarInfoIdentifierL
// Returns the multiple DB information.
// -----------------------------------------------------------------------------
//
TBool CCalenPreviewData::CalendarInfoIdentifierL( const HBufC* aName,
                                        const CCalCalendarInfo& aCalendarInfo)
    {
    TRACE_ENTRY_POINT;
    TBool retVal = EFalse;
    HBufC* calendarFileName = aCalendarInfo.FileNameL().AllocLC();
    retVal = calendarFileName->CompareF(*aName);
    CleanupStack::PopAndDestroy(calendarFileName);
    TRACE_EXIT_POINT;
    return (!retVal);
    }

//  End of File
