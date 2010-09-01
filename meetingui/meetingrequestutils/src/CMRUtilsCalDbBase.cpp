/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:Implementation for meeting request utils calendar 
* data base
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CMRUtilsCalDbBase.h"
#include "MREntryConsultant.h"
#include "MRHelpers.h"
#include <ct/RCPointerArray.h>
#include <CalEntryView.h>
#include <CalInstanceView.h>
#include <CalEntry.h>
#include <CalInstance.h>
#include <CalUser.h>
#include <CalRRule.h>
#include <CalTime.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

enum TPanicCode
    {
    EPanicNonEmptyParamArray = 1
    };

_LIT( KPanicMsg, "CMRUtilsCalDbBase" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
    
}  // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::CMRUtilsCalDbBase
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRUtilsCalDbBase::CMRUtilsCalDbBase(
    MMRUtilsCalDbObserver& aDbObserver,
    MCalProgressCallBack& aCmdObserver )
    : iDbObserver( aDbObserver ),
      iCmdObserver( aCmdObserver ),
      iDbStatus( MMRUtilsCalDbObserver::EUninitialized )
    {
    
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::~CMRUtilsCalDbBase
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CMRUtilsCalDbBase::~CMRUtilsCalDbBase()
    {   
    // subclasses take care of deleting entry & instance view if they
    // create them!
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::EntryView
// ----------------------------------------------------------------------------
//    
const CCalEntryView* CMRUtilsCalDbBase::EntryView() const
    {
    return iCalEntryView;
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::InstanceView
// ----------------------------------------------------------------------------
//        
const CCalInstanceView* CMRUtilsCalDbBase::InstanceView() const
    {
    return iCalInstanceView;
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::Session
// ----------------------------------------------------------------------------
//        
const CCalSession* CMRUtilsCalDbBase::Session() const
    {
    return iCalSession;
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::EntryView
// ----------------------------------------------------------------------------
//    
CCalEntryView* CMRUtilsCalDbBase::EntryView()
    {
    return iCalEntryView;
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::InstanceView
// ----------------------------------------------------------------------------
//        
CCalInstanceView* CMRUtilsCalDbBase::InstanceView()
    {
    return iCalInstanceView;
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::Session
// ----------------------------------------------------------------------------
//        
CCalSession* CMRUtilsCalDbBase::Session()
    {
    return iCalSession;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::DbStatus
// ----------------------------------------------------------------------------
//        
MMRUtilsCalDbObserver::TDbStatus CMRUtilsCalDbBase::DbStatus()
    {
    return iDbStatus;
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::ResetDbL
// ----------------------------------------------------------------------------
//        
void CMRUtilsCalDbBase::ResetDbL()
    {
    // By default database reseting is not supported
    User::Leave( KErrNotSupported );
    }

// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::FetchWithRangeL
// According to UI spec. we only remove a repeating entry if all occurences
// are within given timerange. This implementation however considers
// originating and modifying entries separately (i.e. modifying entry may
// be removed without originating entry being removed ). It is done like this
// because 1) it's faster and 2) they are semantically different requests.
// ----------------------------------------------------------------------------
//
void CMRUtilsCalDbBase::FetchWithRangeL(
    RPointerArray<CCalEntry>& aCalEntryArray,
    const CalCommon::TCalTimeRange& aCalTimeRange,
    TBool aFetchFullCopy )
    {
    __ASSERT_DEBUG( aCalEntryArray.Count() == 0,
                    Panic( EPanicNonEmptyParamArray ) );
    
    if ( !iCalInstanceView || !iCalEntryView )
        {
        User::Leave( KErrNotSupported );
        }
        
    RCPointerArray<CCalInstance> tmpFindArray;
    CleanupClosePushL( tmpFindArray );    

    iCalInstanceView->FindInstanceL( tmpFindArray, 
					   			     CalCommon::EIncludeAppts, 
					   			     aCalTimeRange );
					   			     
    TInt count( tmpFindArray.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {        
        const CCalEntry& entry( tmpFindArray[i]->Entry() );
        
        // This is used to ensure that if entry has already been added we don't
        // bother to try again:
        TInt alreadyExistsIndex( aCalEntryArray.FindInOrder( &entry,
                TLinearOrder<CCalEntry>( CMRUtilsCalDbBase::Compare ) ) );
        
        if ( alreadyExistsIndex < 0 &&
             IsCompletelyWithinRangeL( entry, aCalTimeRange ) )
            {
            // we create a full or a skeleton copy, based on given argument:
            MRHelpers::TCopyFields copyType( aFetchFullCopy ?
                MRHelpers::ECopyFull : MRHelpers::ECopySkeleton );
            CCalEntry* copy = MRHelpers::CopyEntryLC( entry,
                                                      entry.MethodL(),
                                                      copyType );
            aCalEntryArray.InsertInOrderL( copy, 
                TLinearOrder<CCalEntry>( CMRUtilsCalDbBase::Compare ) );
            CleanupStack::Pop( copy ); // ownership transferred to array
            }
        }

    CleanupStack::PopAndDestroy(); // tmpFindArray
    }
  
// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::IsCompletelyWithinRangeL
// To optimize performance we return from the method immediatelly when EFalse
// has been confirmed.
// ----------------------------------------------------------------------------
//          
TBool CMRUtilsCalDbBase::IsCompletelyWithinRangeL(
    const CCalEntry& aEntry,
    const CalCommon::TCalTimeRange& aCalTimeRange )
    {
    // 1. Check occurrences specified with a recurrence rule:

    TCalRRule rule;
    if ( aEntry.GetRRuleL( rule ) )
        {
        // In this implementation we don't check if end time of the last
        // occurrence exceeds the time range, we only check the start/until of
        // the rule (according to the standard the start time of the rule equals
        // the start time of the first occurrence).
        // Furthermore ex-dates are not considered at all.
                
        if ( rule.DtStart().TimeUtcL() < aCalTimeRange.StartTime().TimeUtcL() ||
             rule.Until().TimeUtcL() > aCalTimeRange.EndTime().TimeUtcL() )
            {
            return EFalse;
            }
        }
        
    // 2. Check occurrences specified with r-dates:
            
    RArray<TCalTime> rDateList;   
    aEntry.GetRDatesL( rDateList );
    TInt count( rDateList.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        // In this implementation we don't check if end time of r-date
        // occurence exceeds the time range.        
        
        if ( rDateList[i].TimeUtcL() < aCalTimeRange.StartTime().TimeUtcL() ||
             rDateList[i].TimeUtcL() > aCalTimeRange.EndTime().TimeUtcL() )
            {
            rDateList.Close();
            return EFalse;
            }
        }
    rDateList.Close();        
    return ETrue;        
    }
   
// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::Compare
// ----------------------------------------------------------------------------
//   
TInt CMRUtilsCalDbBase::Compare(
    const CCalEntry& aFirst,
    const CCalEntry& aSecond )
    {
    TInt retVal( 0 ); // default value is that items are equal
    TRAP_IGNORE( retVal = CompareL( aFirst, aSecond ) );
    return retVal;
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::CompareL
// ----------------------------------------------------------------------------
//   
TInt CMRUtilsCalDbBase::CompareL(
    const CCalEntry& aFirst,
    const CCalEntry& aSecond )
    {
    TInt result( aFirst.UidL().Compare( aSecond.UidL() ) );
    if ( result == 0 )
        { // UIDs were identical, how about recurrence id's then:
        TTime firstRec( aFirst.RecurrenceIdL().TimeUtcL() );
        TTime secondRec( aSecond.RecurrenceIdL().TimeUtcL() );
        if ( firstRec > secondRec )
            {
            result = 1;
            }
        else if ( firstRec < secondRec )
            {
            result = -1;
            }
        else
            {
            result = 0;
            }
        }
    return result;        
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::Progress
// ----------------------------------------------------------------------------
//    
void CMRUtilsCalDbBase::Progress( TInt /*aPercentageCompleted*/ )
    {
    // No interest currently
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsCalDbBase::NotifyProgress
// ----------------------------------------------------------------------------
//
TBool CMRUtilsCalDbBase::NotifyProgress()
    { 
    // No interest currently
    return EFalse;
    }

// End of file
