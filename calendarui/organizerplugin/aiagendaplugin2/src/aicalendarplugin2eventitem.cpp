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


#include "aicalendarplugin2eventitem.h"
#include "aicalendarplugin2eventstrings.h"
#include "aicalendarplugin2constants.hrh"
#include "aicalendarplugin2constantdata.h"
#include "aicalendarplugin2contentmodel.h"
#include <calinstance.h>
#include <Calendar.rsg> // for R_CALEN_QTN_CALE_NO_SUBJECT
#include <StringLoader.h> // for loading R_CALEN_QTN_CALE_NO_SUBJECT

CAiCalendarPlugin2EventItem* CAiCalendarPlugin2EventItem::NewL(
                             const CCalInstance& aCalInstance,
                             CAICalendarPlugin2ConstantData& aConstantData )
    {
    CAiCalendarPlugin2EventItem* self = new( ELeave )CAiCalendarPlugin2EventItem;
    CleanupStack::PushL( self );
    self->ConstructL( aCalInstance, aConstantData );
    CleanupStack::Pop( self );
    return self;
    }

CAiCalendarPlugin2EventItem::CAiCalendarPlugin2EventItem()
    {
    }

void CAiCalendarPlugin2EventItem::ConstructL(
                             const CCalInstance& aCalInstance,
                             CAICalendarPlugin2ConstantData& aConstantData )
    {
    iHandleTomorrowItems = ( aConstantData.iDaysToHandleAsNotToday == KAIRangeTomorrowOnly );
    
    iNotTodayDayRange = aConstantData.iDaysToHandleAsNotToday;

    // This needs to be first as data from this is used on the other 2 extractors
    ExtractEventIdentifiersL( aCalInstance );

    TTime today;
    today.HomeTime();
    const TTime start( aCalInstance.StartTimeL().TimeLocalL() );
    const TDateTime x = today.DateTime();
    const TDateTime y = start.DateTime();

    iEventStrings =
            CAiCalendarPlugin2EventStrings::NewL( aCalInstance,
                                                  aConstantData.TimeFormatUsual(),
                                                  aConstantData.DateFormatWithoutYearWithZero(),
                                                  aConstantData.iAppendEndTimeToOngoingItems,
                                                  aConstantData.iUseAlternatePublish,
                                                  *this );
    iOpenInViewer = aConstantData.iOpenInViewer;
    iOpenFirstValid = aConstantData.iOpenFirstValid;
    iChecksum = CreateChecksumFromString( SecondLineText() );
    iChecksum += CreateChecksumFromString( FirstLineText() );
    
    iUnnamed = StringLoader::LoadL( R_CALEN_QTN_CALE_NO_SUBJECT );
    }

CAiCalendarPlugin2EventItem::~CAiCalendarPlugin2EventItem()
    {
    delete iUnnamed;
    delete iStartTimeSubject;
    delete iEventStrings;
    delete iUid;
    }

void CAiCalendarPlugin2EventItem::ExtractEventIdentifiersL( const CCalInstance& aCalInstance )
    {
    iType = ExtractTypeL( aCalInstance );
    iStatus =  aCalInstance.Entry().StatusL();
    iIconId = IconIdToEvent( iType, iStatus );
    iLocalUid = aCalInstance.Entry().LocalUidL();
    iTime = aCalInstance.Time().TimeLocalL();
    iEndTime = aCalInstance.EndTimeL().TimeLocalL();
    iLastModifiedTime = aCalInstance.Entry().LastModifiedDateL().TimeUtcL();
    
    delete iUid;
    iUid = NULL;
    iUid = aCalInstance.Entry().UidL().AllocL();
    
    // Multiple-day Todo requires date
    TTime today;
    today.HomeTime();
    if ( iType == EPluginItemToDo )
    	{
    	iIsToday = OnSameDay( today, iEndTime );
    	iIsTomorrow = OnNextDay( today, iEndTime );

    	if( OnDayRange( today, iEndTime, iNotTodayDayRange ) )
    		{
    	    iIsNotTodayEvent = ETrue;
    	    }

    	if( iEndTime > today )
    		{
    	    iIsFutureEvent = ETrue;
    		}
    	}
            
    if( !iIsToday )
        {
        // Event is not today, and future event. Date needed. Otherwise not.
        iDateNeeded = ETrue;
        if( iType != EPluginItemDayNote &&
            iType != EPluginItemAnniv &&
            iType != EPluginItemToDo )
            {
            iTimeNeeded = ETrue;
            }
        }
    if( iType != EPluginItemDayNote &&
        iType != EPluginItemAnniv &&
        iType != EPluginItemToDo &&
        iIsToday )
        {
        iTimeNeeded = ETrue;
        }
    }
    
TAICal2PluginItemType CAiCalendarPlugin2EventItem::ExtractTypeL(
                                const CCalInstance& aCalInstance )
    {
    TTime today;
    today.HomeTime();
    const TTime start( aCalInstance.StartTimeL().TimeLocalL() );
    iIsToday = OnSameDay( today, start );
    iIsTomorrow = OnNextDay( today, start );

    if( OnDayRange( today, start, iNotTodayDayRange ) )
        {
        iIsNotTodayEvent = ETrue;
        }

    if( start > today )
        {
        iIsFutureEvent = ETrue;
        }

    switch( aCalInstance.Entry().EntryTypeL() )
        {
        case CCalEntry::EAppt:
            {
            /*
            UI Spec this priorization is based on:

            1. Display events which begin within two (2) hours.

            2. Display events which have begun at the maximum 30 minutes ago.
            If each ongoing event doesn’t fit to plug-in then events which starting
            time is closer current time are displayed.

            3. Display upcoming events (starting time >= current time) which are left.

            4. Display ongoing events (ending time > current time) which are left.
            If each ongoing event doesn’t fit to plug-in then events which starting
            time is closer current time are displayed. This includes long meetings that
            have lasted for day(s).

            5. Past events (ending time < current time) are not displayed in the Organizer
            plug-in. This is a precaution bacause future engine releases dont
            contain expired events.
            */
            const TTimeIntervalMinutes oneMinute( 1 );
            const TTimeIntervalMinutes thirtyOneMinutes( 31 );
            const TTimeIntervalHours twoHours( 2 );
            const TTime end( aCalInstance.EndTimeL().TimeLocalL() );

            const TBool upcomingEvent2hrs = ( start > today ) &&
                                            ( start <= ( today + twoHours ) ); // 1.

            const TBool ongoingEvent = ( ( start >= ( today - thirtyOneMinutes ) ) &&
                                       ( start <= today ) &&
                                       ( ( end +  oneMinute ) >= today ) ); // 2.

            const TBool upcomingEvent = ( start > today + twoHours ); // 3.

            // This handles properly also the "long meeting" case, where
            // the meeting has begun yesterday or before that.
            const TBool olderOngoingEvent = ( ( start < ( today - thirtyOneMinutes ) ) &&
                                            ( ( end + oneMinute ) >= today ) ); // 4.

            const TBool expiredEvent = ( end + oneMinute ) < today; // 5.

            if( expiredEvent )
                {
                return EPluginItemExpired;
                }
            else if( upcomingEvent2hrs )
                {
                return EPluginItemMeeting;
                }
            else if( ongoingEvent )
                {
                return EPluginItemOnGoingEvent;
                }
            else if( upcomingEvent )
                {
                if( !iIsToday )
                    {
                    // Handle the "not today meeting" case
                    return EPluginItemMeeting;
                    }
                return EPluginItemUpcomingMeeting;
                }
            else if( olderOngoingEvent )
                {
                return EPluginItemOlderOnGoingEvent;
                }
            else
                {
                return EPluginItemMeeting;
                }
            }

        case CCalEntry::EEvent:
            {
            return EPluginItemDayNote;
            }

        case CCalEntry::EAnniv:
            {
            return EPluginItemAnniv;
            }

        case CCalEntry::ETodo:
            {
            return EPluginItemToDo;
            }

        default:
            return EPluginItemNoEvents;
        }
    }

TBool CAiCalendarPlugin2EventItem::OnSameDay( const TTime& aX, const TTime& aY )
    {
    const TDateTime x = aX.DateTime();
    const TDateTime y = aY.DateTime();

    return ( x.Year() == y.Year() )
        && ( x.Month() == y.Month() )
        && ( x.Day() == y.Day() );
    }

TBool CAiCalendarPlugin2EventItem::OnNextDay( const TTime& aToday, const TTime& aEventTime )
    {
    TTime todayPlusOne( aToday );
    todayPlusOne += TTimeIntervalDays( 1 );

    const TDateTime x = todayPlusOne.DateTime();
    const TDateTime y = aEventTime.DateTime();

    return ( x.Year() == y.Year() )
        && ( x.Month() == y.Month() )
        && ( x.Day() == y.Day() );
    }

TBool CAiCalendarPlugin2EventItem::OnDayRange( const TTime& aToday,
                                               const TTime& aEventTime,
                                               TInt aRange )
    {
    TTime todayPlusRange( aToday );
    todayPlusRange += TTimeIntervalDays( aRange + 1 );
    
    const TDateTime x( todayPlusRange.DateTime().Year(),
                       todayPlusRange.DateTime().Month(),
                       todayPlusRange.DateTime().Day(), 0, 0, 0, 0 );
    
    todayPlusRange = x;
    
    return ( aEventTime < todayPlusRange );
    }
    
const TTime CAiCalendarPlugin2EventItem::Time() const
    {
    return iTime;
    }

const TTime CAiCalendarPlugin2EventItem::LastModifiedTime() const
    {
    return iLastModifiedTime;
    }

TInt CAiCalendarPlugin2EventItem::IconIdToEvent(
                                TAICal2PluginItemType aType,
                                CCalEntry::TStatus aStatus )
    {
    switch( aType )
        {
        case EPluginItemMeeting: // fall through
        case EPluginItemUpcomingMeeting:
        case EPluginItemOnGoingEvent:
        case EPluginItemOlderOnGoingEvent:
            {
            switch( aStatus )
                {
                case CCalEntry::ETentative:
                    {
                    return EAICalendarPlugin2IconMeetingTentative;
                    }
                case CCalEntry::EConfirmed:
                    {
                    return EAICalendarPlugin2IconMeetingAccepted;
                    }
                case CCalEntry::ECancelled:
                    {
                    // Should not happen!
                    return KErrNotFound;
                    }
                default:
                    {
                    // fall through
                    }
                }
            return EAICalendarPlugin2IconAppt;
            }

        case EPluginItemDayNote:
            {
            return EAICalendarPlugin2IconMemo;
            }

        case EPluginItemAnniv:
            {
            return EAICalendarPlugin2IconAnniv;
            }
        case EPluginItemToDo:
            {
            return EAICalendarPlugin2IconTodo;
            }
        }
    return KErrNotFound;
    }

TAiCalendarFocusData CAiCalendarPlugin2EventItem::FocusData()
    {
    TAiCalendarFocusData focusData;
    focusData.iLocalUid = iLocalUid;
    focusData.iEntryTime = iTime;
    switch( iType )
        {
        case EPluginItemMeeting: // fall through
        case EPluginItemUpcomingMeeting:
        case EPluginItemOnGoingEvent:
        case EPluginItemOlderOnGoingEvent:
        case EPluginItemDayNote:
        case EPluginItemAnniv:
        case EPluginItemToDo:
            {
            if( iOpenFirstValid )
                {
                focusData.iType = EAI2CalOpenOnEventDay;
                }
            else
                {
                focusData.iType = EAI2CalOpenEvent;
                }
            break;
            }
        default:
            {
            focusData.iType = EAI2CalOpenToday;
            break;
            }
        }
    focusData.iOpenInViewer = iOpenInViewer;
    return focusData;
    }

const TDesC& CAiCalendarPlugin2EventItem::FirstLineText()
    {
    return iEventStrings->CombinedTimeString();
    }

const TDesC& CAiCalendarPlugin2EventItem::SecondLineText()
    {
    return iEventStrings->CombinedSubjectLocationString();
    }

const TDesC& CAiCalendarPlugin2EventItem::StartTimeText()
    {
    return iEventStrings->StartTimeString();
    }

const TDesC& CAiCalendarPlugin2EventItem::EndTimeText()
    {
    return iEventStrings->EndTimeString();
    }

const TDesC& CAiCalendarPlugin2EventItem::SubjectText()
    {
    return iEventStrings->SubjectString();
    }

const TDesC& CAiCalendarPlugin2EventItem::SubjectTimeString( TBool aAddTime )
    {
    delete iStartTimeSubject;
    iStartTimeSubject = NULL;
    TInt length = 0;
    if ( aAddTime )
        {
        // +1 for the space
        length += StartTimeText().Length() + 1;
        }

    if( SubjectText() == iUnnamed->Des() &&
             LocationText().Length() > 0 )
        {
        length += ( LocationText().Length() );
        }
    else if( SubjectText().Length() > 0 )
        {
        length += ( SubjectText().Length() );
        }

    iStartTimeSubject = HBufC::New( length );
    _LIT( KOrgTimeSubjectSeparator, " ");
    if ( iStartTimeSubject )
        {
        TPtr tempPtr = iStartTimeSubject->Des();
        if ( aAddTime )
            {
            tempPtr.Append( StartTimeText() );
            tempPtr.Append( KOrgTimeSubjectSeparator );            
            }
        if( SubjectText() == iUnnamed->Des() &&
            LocationText().Length() > 0 )
            {
            tempPtr.Append( LocationText() );
            }
        else if( SubjectText().Length() > 0 )
            {
            tempPtr.Append( SubjectText() );
            }
            
        return *iStartTimeSubject;
        }
    return KNullDesC;
    }

const TDesC& CAiCalendarPlugin2EventItem::LocationText()
    {
    return iEventStrings->LocationString();
    }

TAICal2PluginItemType CAiCalendarPlugin2EventItem::Type() const
    {
    return iType;
    }

TInt CAiCalendarPlugin2EventItem::IconId()
    {
    return iIconId;
    }

TBool CAiCalendarPlugin2EventItem::IsToday()
    {
    return iIsToday;
    }

TBool CAiCalendarPlugin2EventItem::IsNotTodayEvent()
    {
    return iIsNotTodayEvent;
    }

TBool CAiCalendarPlugin2EventItem::IsTomorrow()
    {
    return iIsTomorrow;
    }

TBool CAiCalendarPlugin2EventItem::IsEndTimeToday()
    {
    TTime today;
    today.HomeTime();
    return OnSameDay( today, iEndTime );
    }

TBool CAiCalendarPlugin2EventItem::IsFutureEvent()
    {
    return iIsFutureEvent;
    }

TBool CAiCalendarPlugin2EventItem::DateNeeded()
    {
    return iDateNeeded;
    }

TBool CAiCalendarPlugin2EventItem::TimeNeeded()
    {
    return iTimeNeeded;
    }
    
TInt CAiCalendarPlugin2EventItem::CreateChecksumFromString( const TDesC& aString )
    {
    TInt checksum = 0;
    
    for ( TInt i = 0; i < aString.Length(); i++ )
        {
        checksum += aString[i] * ( i + 1);
        }
    return checksum * iType;       
    }

// END OF FILE

