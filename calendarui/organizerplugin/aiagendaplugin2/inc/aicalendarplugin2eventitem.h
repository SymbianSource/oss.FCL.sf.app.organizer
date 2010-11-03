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


#ifndef C_AICALENDARPLUGIN2EVENTITEM_H
#define C_AICALENDARPLUGIN2EVENTITEM_H

#include <e32base.h>
#include <e32std.h>
#include <calentry.h>
#include "aicalendarplugin2constants.hrh"
#include "aicalendarplugin2focusdata.h"

class CCalInstance;
class CAICalendarPlugin2ConstantData;
class CAiCalendarPlugin2EventStrings;

/**
*  Calendar plugin 2 local presentation of a calendar event item.
*
*  @lib aicalendarplugin2
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAiCalendarPlugin2EventItem ) : public CBase
    {
    public:

        /**
         * Static factory method for 2 stage construction.
         * @since Series 60 3.2
         * @param aCalInstance calendar instance object
         * @param aConstantData constant data container
         * @return CAiCalendarPlugin2EventItem pointer to contructed object
         */
        static CAiCalendarPlugin2EventItem* NewL(
                                const CCalInstance& aCalInstance,
                                CAICalendarPlugin2ConstantData& aConstantData );

        ~CAiCalendarPlugin2EventItem();

    // New methods

        /**
         * Getter for focus data
         * @since Series 60 3.2
         * @return TAiCalendarFocusData
         */
        TAiCalendarFocusData FocusData();

        /**
         * Getter for first line text
         * @since Series 60 3.2
         * @return TDesC
         */
        const TDesC& FirstLineText();

        /**
         * Getter for second line text
         * @since Series 60 3.2
         * @return TDesC
         */
        const TDesC& SecondLineText();

        /**
         * Getter for start time string
         * @since Series 60 3.2
         * @return TDesC
         */
        const TDesC& StartTimeText();

        /**
         * Getter for end time string
         * @since Series 60 3.2
         * @return TDesC
         */
        const TDesC& EndTimeText();

        /**
         * Getter for subject text
         * @since Series 60 3.2
         * @return TDesC
         */
        const TDesC& SubjectText();

        /**
         * Getter for location text
         * @since Series 60 3.2
         * @return TDesC
         */
        const TDesC& LocationText();

        /**
         * Returns a oneline string with a possible starttime and subject
         * separated with space. 
         * 
         * @since Series 60 5.0
         * @param aAddTime 
         * @return TDesC
         */
        const TDesC& SubjectTimeString( TBool aAddTime = ETrue );

        /**
         * Getter for item type
         * @since Series 60 3.2
         * @return TAICal2PluginItemType
         */
        TAICal2PluginItemType Type() const;
        
        /**
         * Getter for icon id
         * @since Series 60 3.2
         * @return TInt
         */
        TInt IconId();

        /**
         * Is event today.
         * @since Series 60 3.2
         * @return TBool
         */
        TBool IsToday();

        /**
         * Is event tomorrow.
         * @since Series 60 3.2
         * @return TBool
         */
        TBool IsTomorrow();

        /**
         * Is event end time today.
         * @since Series 60 3.2
         * @return TBool
         */
        TBool IsEndTimeToday();

        /**
         * Is event a not today event. Within specified day range EPluginDaysToHandleAsNotToday.
         * @since Series 60 3.2
         * @return TBool
         */
        TBool IsNotTodayEvent();

        /**
         * Is event a future event. The rest of not categorized not today items -> future items.
         * @since Series 60 3.2
         * @return TBool
         */
        TBool IsFutureEvent();

        /**
         * Is date needed.
         * @since Series 60 3.2
         * @return TBool
         */
        TBool DateNeeded();

        /**
         * Is time needed.
         * @since Series 60 3.2
         * @return TBool
         */
        TBool TimeNeeded();

        /**
         * Determine if two time/date's are on the same day
         * @since Series 60 3.2
         * @param aX time one
         * @param aY time two
         * @return TBool are they on same day
         **/
        static TBool OnSameDay(const TTime& aX, const TTime& aY);

        /**
         * Determine if TTime is tomorrow.
         * @since Series 60 3.2
         * @param aToday current time
         * @param aY tested time
         * @return TBool are they on same day
         **/
        static TBool OnNextDay(const TTime& aToday, const TTime& aEventTime);
        
        /**
         * Determine if TTime is tomorrow.
         * @since Series 60 3.2
         * @param aToday current time
         * @param aEventTime tested time
         * @param aRange the range within to stay
         * @return TBool are they on same day
         **/
        static TBool OnDayRange(const TTime& aToday, const TTime& aEventTime, TInt aRange);
        
        /**
         * Return the TTime of the event.
         */
        const TTime Time() const;

        /**
         * Return the last modified TTime of the event.
         */
        const TTime LastModifiedTime() const;
        
        TInt Checksum()
            {
            return iChecksum;
            };
        
        /**
         * Calculate checksum for event data.
         */
        TInt CreateChecksumFromString( const TDesC& aString );

    private:

    // Construction

        CAiCalendarPlugin2EventItem();
        
        void ConstructL( const CCalInstance& aCalInstance,
                         CAICalendarPlugin2ConstantData& aConstantData );
        
    // New methods
    
        /**
         * Extract generic event identifiers from calendar instance
         *     CAiCalendarPlugin2EventStrings object instance
         * @since Series 60 3.2
         * @param aCalInstance calendar instance object
         **/
        void ExtractEventIdentifiersL( const CCalInstance& aCalInstance );
        
        /**
         * Extract and translate item type from calendar instance object
         * @since Series 60 3.2
         * @param aCalInstance calendar instance object
         * @return TAICal2PluginItemType translated item type
         **/
        TAICal2PluginItemType ExtractTypeL( const CCalInstance& aCalInstance );

        /**
         * Determine local icon type for item type and status
         * @since Series 60 3.2
         * @param aType item type
         * @param aStatus item status
         * @return TInt local icon id
         **/
        TInt IconIdToEvent( TAICal2PluginItemType aType, CCalEntry::TStatus aStatus );
        
    public:  // Data members
        
        /**
         * Item uid.
         */
        HBufC8* iUid;
        
    private:  // Data members

        /**
         * Time strings creator and container.
         * Own.
         */
        CAiCalendarPlugin2EventStrings* iEventStrings;

        /**
         * Item type.
         */
        TAICal2PluginItemType iType;
        
        /**
         * Is event today.
         */
        TBool iIsToday;

        /**
         * Is event tomorrow.
         */
        TBool iIsTomorrow;
        
        /**
         * Is event not today event. Within specified day range EPluginDaysToHandleAsNotToday.
         */
        TBool iIsNotTodayEvent;

        /**
         * Is event future event.
         */
        TBool iIsFutureEvent;

        /**
         * Item start time (optional).
         */
        TTime iTime;

        /**
         * Item end time (optional).
         */
        TTime iEndTime;

        /**
         * Item last modified (optional).
         */
        TTime iLastModifiedTime;

        /**
         * Item local UID.
         */
        TCalLocalUid iLocalUid;

        /**
         * Item cal entry status (accepted/tentative etc).
         */
        CCalEntry::TStatus iStatus;

        /**
         * Icon id.
         */
        TInt iIconId;

        /**
         * Flag to store info on date string need
         */
        TBool iDateNeeded;
        
        /**
         * Flag to store info on time string need
         */
        TBool iTimeNeeded;
        
        /**
         * Open in viewer.
         */
        TBool iOpenInViewer;
        
        /**
         * Open first valid entry on index zero.
         */
        TBool iOpenFirstValid;
        
        /**
         * Handle tomorrow item as specially.
         */
        TBool iHandleTomorrowItems;

        /**
         * Day range for "not today" category.
         */
        TInt iNotTodayDayRange;

        /**
         * String checksums
         */
        TInt iChecksum;

        /**
         * String for oneline start time subject
         */
        HBufC *iStartTimeSubject;
        
        /**
         * String for unnamed
         */
        HBufC *iUnnamed;
    };

#endif  // C_AICALENDARPLUGIN2EVENTITEM_H


// End of File
