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


#ifndef C_AICALENDARPLUGIN2DATA_H
#define C_AICALENDARPLUGIN2DATA_H

// INCLUDES
#include <e32base.h>
#include <aicontentobserver.h>
#include "aicalendarplugin2eventitem.h"


/**
 *  Calendar plugin 2 event data container.
 *
 *  @lib aicalendarplugin2
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAICalendarPlugin2Data ) : public CBase
{
    public:

    // Construction

        static CAICalendarPlugin2Data* NewL();

        virtual ~CAICalendarPlugin2Data();
        
    // New methods

        void ClearDataArrays();

        RPointerArray<CAiCalendarPlugin2EventItem>& Upcoming2HrsArray();

        RPointerArray<CAiCalendarPlugin2EventItem>& UpcomingArray();

        RPointerArray<CAiCalendarPlugin2EventItem>& Ongoing30MinArray();

        RPointerArray<CAiCalendarPlugin2EventItem>& OngoingArray();

        RPointerArray<CAiCalendarPlugin2EventItem>& LongOngoingArray();

        RPointerArray<CAiCalendarPlugin2EventItem>& NonTimedEventArray();
        
        RPointerArray<CAiCalendarPlugin2EventItem>& TomorrowEventArray();
        
        RPointerArray<CAiCalendarPlugin2EventItem>& NotTodayItemArray();

        RPointerArray<CAiCalendarPlugin2EventItem>& FutureItemArray();
        
        TInt AppendItemToCorrectArray( CAiCalendarPlugin2EventItem& aItem,
                                        TInt& aDayNoteIndex );
                                        
        TInt TotalTimedTodaySpaceReservation();

        TInt TotalTodayItemCount();

        TInt TotalTimedTodayItemCount();

        RPointerArray<MAiContentObserver>& ObserverArray();
        
        void AppendObserverL( MAiContentObserver& aObserver );

        RArray<TAiCalendarFocusData>& FocusDataArray();

        void SetSpaceTakenByTimedEvent( TInt aSpace ){iSpaceTakenByTimedEvent = aSpace;};
        
        void SetSpaceTakenByNonTimedEvent( TInt aSpace ){iSpaceTakenByNonTimedEvent = aSpace;};
        
        TInt SpaceTakenByTimedEvent(){return iSpaceTakenByTimedEvent;};
        
        TInt SpaceTakenByNonTimedEvent(){return iSpaceTakenByNonTimedEvent;};
        
        void LaunchCalendarApplication( TInt aIndex, TBool aLaunchFirstValid );
        
        TBool HadExpiredEvents();

        void SetHadExpiredEvents( TBool aHadExpired );
        
        void SortArrayByTime( RPointerArray<CAiCalendarPlugin2EventItem>& aArray );
        
    private:

    // Construction

        CAICalendarPlugin2Data();

        void ConstructL();
        
    private:  // Data members

        /**
         * Item data array for normal upcoming timed today items (meeting/appointment).
         * Starting within 2 hrs.
         * Own.
         */
        RPointerArray<CAiCalendarPlugin2EventItem> iUpcoming2HrsItemData;

        /**
         * Item data array for normal upcoming timed today items (meeting/appointment).
         * Own.
         */
        RPointerArray<CAiCalendarPlugin2EventItem> iUpcomingItemData;

        /**
         * Item data array for normal ongoing timed today items (meeting/appointment)
         * that have started within 30 mins.
         * Own.
         */
        RPointerArray<CAiCalendarPlugin2EventItem> iOngoing30MinItemData;

        /**
         * Item data array for normal ongoing timed today items (meeting/appointment).
         * Own.
         */
        RPointerArray<CAiCalendarPlugin2EventItem> iOngoingItemData;

        /**
         * Item data array for non timed today items (Anniversary/ToDo/Day note).
         * Own.
         */
        RPointerArray<CAiCalendarPlugin2EventItem> iNonTimedItemData;

        /**
         * Item data array for all tomorrow items (meeting/appointment/Anniversary/ToDo/Day Note, not today).
         * Own.
         */
        RPointerArray<CAiCalendarPlugin2EventItem> iTomorrowItemData;

        /**
         * Item data array for all not today items that are within the specified day range
         * EPluginDaysToHandleAsNotToday. (meeting/appointment/Anniversary/ToDo/Day Note, not today).
         * Own.
         */
        RPointerArray<CAiCalendarPlugin2EventItem> iNotTodayItemData;

        /**
         * Item data array for all not today items that have begun before to day, and are still ongoing.
         * EPluginDaysToHandleAsNotToday. (meeting/appointment/Anniversary/ToDo/Day Note, not today).
         * Own.
         */
        RPointerArray<CAiCalendarPlugin2EventItem> iLongOngoingItemData;

        /**
         * Item data array for all rest not today items (meeting/appointment/Anniversary/ToDo/Day Note, not today).
         * Own.
         */
        RPointerArray<CAiCalendarPlugin2EventItem> iFutureItemData;

        /**
         * Publishing observers.
         * Own.
         */
        RPointerArray<MAiContentObserver>   iObservers;

        /**
         * Data array for event focusing data in calendar app.
         */
        RArray<TAiCalendarFocusData>        iEventFocusData;
        
        /**
         * Number of space (lines or othervise) one timed event line reserves.
         * Depends on the layout of the theme.
         */
        TInt                                iSpaceTakenByTimedEvent;

        /**
         * Number of space (lines or othervise) one non timed event line reserves.
         * Depends on the layout of the theme.
         */
        TInt                                iSpaceTakenByNonTimedEvent;
        
        /**
         * Had expired events?
         */
        TBool                               iHadExpiredEvents;

};

#endif  // C_AICALENDARPLUGIN2DATA_H


// End of File
