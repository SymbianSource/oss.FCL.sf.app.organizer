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
* Description:  TCalenWeekHour has data by one hour.
 *
*/


#ifndef     CALENWEEKHOUR_H
#define     CALENWEEKHOUR_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS
const TInt KEventsInHour(6); // Event indicators in a hour

//FIXME, inside class namespace ?
enum TEventColor
    {
    ENoEvent,
    EPublicEvent,
    EPrivateEvent,
    ENoSyncEvent,
    EManyEvents,
    EBlackColoredEvent
    };

//  CLASS DEFINITIONS

/**
 *  TCalenWeekHour has data by one hour
 */
NONSHARABLE_CLASS( TCalenWeekHour )
    {
public :
    enum TDataType
        {
        ENoData,
        EHasIcon,
        EHasBar
        };
public:  // Constructors and destructor

    /**
     * C++ default constructor.
     */
    TCalenWeekHour();

public:     // New funcitons
    /**
     * Flag of No data or Timed or Non-timed note
     * @return ENoData:  No data
     *         EHasIcon: Non-timed note data
     *         EHasBar:  Timed note data
     */
    TDataType DataType() const;

    /**
     * Return event type
     * @param ?arg1 aMinutes : 0-9minutes 1: 10-19minutes...,
     * @return ENoEvent          : No event
     *         EPublicEvent      : public event
     *         EPrivateEvent     : Private event
     *         EPublic2Event     : Overlapped public event
     *         EPrivate2Event    : Overlapped private event
     *         EPublicPrivateEvent: Overlapped public and private event
     */
    TUint32 Event(TInt aMinutes) const;
    /**
     * Set a event type
     * @param aMinutes 0 : 0-9minutes 1: 10-19minutes...,
     * @param aType 0 : ENoEvent | EPublicEvent | etc...
     */
    void SetEvent(TInt aMinutes, TUint32 aType);

    /**
     * Return a Icon index
     * @return icon index
     */
    TInt IconIndex() const;
    /**
     * Set a Icon index
     * @param aIconIndex Icon index
     */
    void SetIconIndex(TInt aIconIndex);

private:    // New data
    TDataType iDataType;
    union
        {
        TUint32 iEvent[KEventsInHour];
        TInt iIconIndex;
        };
    };

#endif      //  CALENWEEKHOUR_H


// End of File
