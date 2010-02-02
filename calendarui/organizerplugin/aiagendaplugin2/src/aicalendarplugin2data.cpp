/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
 *
*/

#include "aicalendarplugin2data.h"
#include "aicalendarplugin2contentmodel.h"
#include "aicalendarplugin2constants.hrh"
#include <aicontentmodel.h>
#include <aipropertyextension.h>
#include <e32cmn.h>

// Helper for sort method
TInt TimeDetermination( const CAiCalendarPlugin2EventItem& aOne,
                        const CAiCalendarPlugin2EventItem& aTwo )
    {
    if( aOne.Time() == aTwo.Time() )
        {
        TTime time_1 = aOne.LastModifiedTime();
        TTime time_2 = aTwo.LastModifiedTime();
        if( time_1 == time_2 )
            {
            return  0;
            }
        else if( time_1 > time_2 )
            {
            return  1; // oldest first
            }
        else
            {
            return -1;
            }
        }
    else if( aOne.Time() < aTwo.Time() )
        {
        return -1;
        }
    return 1;
    }

TInt TypeDetermination( const CAiCalendarPlugin2EventItem& aOne,
                        const CAiCalendarPlugin2EventItem& aTwo )
    {
    TBool oneIsMeeting = EFalse;
    TBool twoIsMeeting = EFalse;
    if( ( ( aOne.Type() == EPluginItemMeeting ) ||
          ( aOne.Type() == EPluginItemUpcomingMeeting ) ||
          ( aOne.Type() == EPluginItemOnGoingEvent ) ||
          ( aOne.Type() == EPluginItemOlderOnGoingEvent ) ) )
        {
        oneIsMeeting = ETrue;
        }
    if( ( aTwo.Type() == EPluginItemMeeting ) ||
        ( aTwo.Type() == EPluginItemUpcomingMeeting ) ||
        ( aTwo.Type() == EPluginItemOnGoingEvent ) ||
        ( aTwo.Type() == EPluginItemOlderOnGoingEvent ) )
        {
        twoIsMeeting = ETrue;
        }
        
    if( oneIsMeeting || twoIsMeeting )
        {
        // Meetings always overrule non timed
        return( ( oneIsMeeting == twoIsMeeting ) ? ( 0 ) : ( oneIsMeeting * -1 + twoIsMeeting ) );
        }
    else
        {
        // Both are non timed
        if( aOne.Type() == aTwo.Type() )
            {
            return 0;
            }
        else if( aOne.Type() == EPluginItemToDo )
            {
            return -1;
            }
        else if( aTwo.Type() == EPluginItemToDo )
            {
            return 1;
            }
        else if( aOne.Type() == EPluginItemDayNote )
            {
            return -1;
            }
        else if( aTwo.Type() == EPluginItemDayNote )
            {
            return 1;
            }
        }
    // Should never happed...
    return 0;
    }
    
// Sort method
TInt CompareEventItems( const CAiCalendarPlugin2EventItem& aOne,
                        const CAiCalendarPlugin2EventItem& aTwo )
    {
    if( aOne.Type() != aTwo.Type() )
        {
        if( aOne.OnSameDay( aOne.Time(), aTwo.Time() ) )
            {
            // Different types, same day
            TInt typeDetermination = TypeDetermination( aOne, aTwo );
            if( typeDetermination != 0 )
                {
                return typeDetermination;
                }
            }
        }
    // Same types or different types different day or both meetings(not same category)
    return TimeDetermination(  aOne, aTwo );
    }
    
// ============================ MEMBER FUNCTIONS ===============================

CAICalendarPlugin2Data* CAICalendarPlugin2Data::NewL()
    {
    CAICalendarPlugin2Data* self = new( ELeave )CAICalendarPlugin2Data;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAICalendarPlugin2Data::CAICalendarPlugin2Data()
  : iSpaceTakenByTimedEvent( 1 ),
    iSpaceTakenByNonTimedEvent( 1 ),
    iHadExpiredEvents( EFalse )
    {
    }

void CAICalendarPlugin2Data::ConstructL()
    {
    }

CAICalendarPlugin2Data::~CAICalendarPlugin2Data()
    {
    ClearDataArrays();

    iUpcoming2HrsItemData.Close();
    iUpcomingItemData.Close();
    iOngoing30MinItemData.Close();
    iOngoingItemData.Close();
    iNonTimedItemData.Close();
    iTomorrowItemData.Close();
    iNotTodayItemData.Close();
    iFutureItemData.Close();
    iLongOngoingItemData.Close();

    iEventFocusData.Reset();
    iEventFocusData.Close();

    iObservers.Reset();
    iObservers.Close();
    }
    
void CAICalendarPlugin2Data::ClearDataArrays()
    {
    // cleanup
    iUpcoming2HrsItemData.ResetAndDestroy();
    iUpcomingItemData.ResetAndDestroy();
    iOngoing30MinItemData.ResetAndDestroy();
    iOngoingItemData.ResetAndDestroy();
    iNonTimedItemData.ResetAndDestroy();
    iTomorrowItemData.ResetAndDestroy();
    iNotTodayItemData.ResetAndDestroy();
    iFutureItemData.ResetAndDestroy();
    iLongOngoingItemData.ResetAndDestroy();
    }
    
RPointerArray<CAiCalendarPlugin2EventItem>& CAICalendarPlugin2Data::Upcoming2HrsArray()
    {
    return iUpcoming2HrsItemData;
    }

RPointerArray<CAiCalendarPlugin2EventItem>& CAICalendarPlugin2Data::UpcomingArray()
    {
    return iUpcomingItemData;
    }

RPointerArray<CAiCalendarPlugin2EventItem>& CAICalendarPlugin2Data::Ongoing30MinArray()
    {
    return iOngoing30MinItemData;
    }

RPointerArray<CAiCalendarPlugin2EventItem>& CAICalendarPlugin2Data::OngoingArray()
    {
    return iOngoingItemData;
    }

RPointerArray<CAiCalendarPlugin2EventItem>& CAICalendarPlugin2Data::NonTimedEventArray()
    {
    return iNonTimedItemData;
    }

RPointerArray<CAiCalendarPlugin2EventItem>& CAICalendarPlugin2Data::TomorrowEventArray()
    {
    return iTomorrowItemData;
    }

RPointerArray<CAiCalendarPlugin2EventItem>& CAICalendarPlugin2Data::NotTodayItemArray()
    {
    return iNotTodayItemData;
    }

RPointerArray<CAiCalendarPlugin2EventItem>& CAICalendarPlugin2Data::FutureItemArray()
    {
    return iFutureItemData;
    }

RPointerArray<CAiCalendarPlugin2EventItem>& CAICalendarPlugin2Data::LongOngoingArray()
    {
    return iLongOngoingItemData;
    }

TInt CAICalendarPlugin2Data::AppendItemToCorrectArray( CAiCalendarPlugin2EventItem& aItem,
                                                        TInt& aDayNoteIndex )
    {
    TInt err;
    if( aItem.IsTomorrow() && aItem.IsFutureEvent() )
        {
        err = iTomorrowItemData.Append( &aItem );
        return err;
        }
    else if( !aItem.IsToday() && !aItem.IsFutureEvent() &&
             aItem.Type() == EPluginItemOlderOnGoingEvent )
        {
        err = iLongOngoingItemData.Append( &aItem );
        return err;
        }
    else if( !aItem.IsToday() && aItem.IsNotTodayEvent() &&
             !aItem.IsFutureEvent() &&
             ( aItem.Type() == EPluginItemDayNote || 
               aItem.Type() == EPluginItemToDo || 
               aItem.Type() == EPluginItemAnniv ) )
        {
        if( aItem.Type() == EPluginItemDayNote || 
               aItem.Type() == EPluginItemToDo )
            {
            // insert...just to make sure day notes are always first
            err = iNonTimedItemData.Insert( &aItem, aDayNoteIndex++ );
            }
        else
            {
            // append...just to make sure day notes are always first
            err = iNonTimedItemData.Append( &aItem );
            }
        return err;
        }
    else if( !aItem.IsToday() && aItem.IsNotTodayEvent() )
        {
        err = iNotTodayItemData.Append( &aItem );
        return err;
        }
    else if( !aItem.IsToday() && aItem.IsFutureEvent() )
        {
        err = iFutureItemData.Append( &aItem );
        return err;
        }
    switch( aItem.Type() )
        {
        case EPluginItemMeeting:
            {
            // Meetings starting within 2 hrs
            err = iUpcoming2HrsItemData.Append( &aItem );
            break;
            }
        case EPluginItemUpcomingMeeting:
            {
            // Meetings starting more than 2 hrs current time
            err = iUpcomingItemData.Append( &aItem );
            break;
            }
        case EPluginItemOnGoingEvent:
            {
            // Meetings that have started with 30 mon before current time
            // and are still on going.
            err = iOngoing30MinItemData.Append( &aItem );
            break;
            }
        case EPluginItemOlderOnGoingEvent:
            {
            // Meetings that have started more that 30 minutes ago and
            // are still ongoing
            err = iOngoingItemData.Append( &aItem );
            break;
            }
        case EPluginItemDayNote: // fallthrough
        case EPluginItemToDo:
            {
            // insert...just to make sure day notes are always first
            err = iNonTimedItemData.Insert( &aItem, aDayNoteIndex++ );
            break;
            }
        case EPluginItemAnniv:
            {
            // append...just to make sure day notes are always first
            err = iNonTimedItemData.Append( &aItem );
            break;
            }
        default:
            {
            err = KErrNotSupported;
            break;
            }
        }
    return err;
    }

void CAICalendarPlugin2Data::SortArrayByTime( RPointerArray<CAiCalendarPlugin2EventItem>& aArray )
    {
    // Sort
    aArray.Sort( TLinearOrder<CAiCalendarPlugin2EventItem>( CompareEventItems ) );
    }

TInt CAICalendarPlugin2Data::TotalTimedTodaySpaceReservation()
    {
    return
    ( iUpcoming2HrsItemData.Count() * iSpaceTakenByTimedEvent ) +
    ( iUpcomingItemData.Count() * iSpaceTakenByTimedEvent ) +
    ( iOngoing30MinItemData.Count() * iSpaceTakenByTimedEvent ) +
    ( iOngoingItemData.Count() * iSpaceTakenByTimedEvent );
    }

TInt CAICalendarPlugin2Data::TotalTodayItemCount()
    {
    return
    ( iUpcoming2HrsItemData.Count() ) +
    ( iUpcomingItemData.Count() ) +
    ( iOngoing30MinItemData.Count() ) +
    ( iOngoingItemData.Count() ) +
    iNonTimedItemData.Count();
    }

TInt CAICalendarPlugin2Data::TotalTimedTodayItemCount()
    {
    return
    ( iUpcoming2HrsItemData.Count() ) +
    ( iUpcomingItemData.Count() ) +
    ( iOngoing30MinItemData.Count() ) +
    ( iOngoingItemData.Count() );
    }

RPointerArray<MAiContentObserver>& CAICalendarPlugin2Data::ObserverArray()
    {
    return iObservers;
    }

void CAICalendarPlugin2Data::AppendObserverL( MAiContentObserver& aObserver )
    {
    // make sure that 1 obs only subscribed once
    for( TInt i( 0 ); i < iObservers.Count(); ++i )
        {
        if( iObservers[i] == &aObserver )
            {
            User::Leave( KErrAlreadyExists );
            }
        }
    iObservers.AppendL( &aObserver );
    }
    
RArray<TAiCalendarFocusData>& CAICalendarPlugin2Data::FocusDataArray()
    {
    return iEventFocusData;
    }

void CAICalendarPlugin2Data::LaunchCalendarApplication( TInt aIndex, TBool aLaunchFirstValid )
    {
    if( aIndex < 0 ||
        aIndex >= iEventFocusData.Count() )
        {
        // invalid index
        return;
        }

    TInt indexToLaunch = aIndex;
    if( aLaunchFirstValid )
        {
        for( TInt i = 0; i < iEventFocusData.Count(); ++i )
            {
            if( iEventFocusData[i].iType != EAI2CalOpenToday )
                {
                indexToLaunch = i;
                break;
                }
            }
        }

    TRAP_IGNORE(            
        iEventFocusData[indexToLaunch].LaunchCalendarApplicationL();
    );
    }
    
TBool CAICalendarPlugin2Data::HadExpiredEvents()
    {
    return iHadExpiredEvents;
    }

void CAICalendarPlugin2Data::SetHadExpiredEvents( TBool aHadExpired )
    {
    iHadExpiredEvents = aHadExpired;
    }
