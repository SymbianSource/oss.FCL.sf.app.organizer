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


#ifndef C_AICALENDARPLUGIN2CONSTANTDATA_H
#define C_AICALENDARPLUGIN2CONSTANTDATA_H

// INCLUDES
#include <e32base.h>
#include <hscontentpublisher.h>

// FORWARD DECLARATIONS
class MAiContentItemIterator;


/**
 *  Constant data container.
 *  Active Idle Calendar plug-in 2 constant data container.
 *
 *  @lib aicalendarplugin2
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAICalendarPlugin2ConstantData ) : public CBase
{
    public:

    // Construction

        static CAICalendarPlugin2ConstantData* NewL();

        virtual ~CAICalendarPlugin2ConstantData();

    // New methods
    
        const TDesC& TimeFormatUsual();

        const TDesC& DateFormatWithoutYearWithZero();

        const TDesC& MakeEventOnL( const TTime& aTime );
        
        TAny* GetPropertyL( CHsContentPublisher::TProperty aProperty );

        /**
         * Tell if we need to append end time to "not today timed" events.
         */
        TBool                               iAppendEndTimeToOngoingItems;

        /**
         * Tell if we need to use alternate publish for upcoming and ongoing items.
         */
        TBool                               iUseAlternatePublish;

        /**
         * Open event in viewer.
         */
        TBool                               iOpenInViewer;
        
        /**
         * Amount of days that we handle as not today categorized.
         * 1 means tomorrow only etc. 1 - 7.
         */
        TInt                                iDaysToHandleAsNotToday;
        
        /**
         * Open first valid entry on index zero.
         */
        TBool                               iOpenFirstValid;
        
        /**
         * Publish status text and icon with alternate id. 
         */
        TBool                               iUseAlternateInfoPublish;
    private:

    // Construction

        CAICalendarPlugin2ConstantData();

        void ConstructL();

    private:  // Data members

        /**
         * Usual time format string (examples: "15:45" or "10:20 AM").
         * Own.
         */
        HBufC*                              iTimeFormatUsual;

        /**
         * Short date format string ("15/2/06").
         * Own.
         */
        HBufC*                              iDateFormatShortWithZero;

        /**
         * Short date format string ("05/02").
         * Own.
         */
        HBufC*                              iDateFormatWithoutYearWithZero;

        /**
         * Plugin content iterator.
         * Own.
         */
        MAiContentItemIterator*             iContent;

        /**
         * Resource content iterator. 
         * Own.
         */
        MAiContentItemIterator*             iResources;

        /**
         * Event content iterator.
         * Own.
         */
        MAiContentItemIterator*             iEvents;
        
        /**
         * String for "Next event on [day] [date]"
         * Own.
         */
        HBufC*                              iNextEventOnCombined;

};

#endif  // C_AICALENDARPLUGIN2CONSTANTDATA_H


// End of File
