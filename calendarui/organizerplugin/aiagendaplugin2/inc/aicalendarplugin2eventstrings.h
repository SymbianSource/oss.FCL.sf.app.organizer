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


#ifndef C_AICALENDARPLUGIN2EVENTSTRINGS_H
#define C_AICALENDARPLUGIN2EVENTSTRINGS_H

#include <e32base.h>
#include "aicalendarplugin2constants.hrh"

class CCalInstance;
class CAiCalendarPlugin2EventItem;

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib aicalendarplugin2
*  @since S60 3.1
*/
NONSHARABLE_CLASS( CAiCalendarPlugin2EventStrings ) : public CBase
    {
    public:

    // Construction

        static CAiCalendarPlugin2EventStrings* NewL( const CCalInstance& aCalInstance,
                                         const TDesC& aTimeFormat,
                                         const TDesC& aDateFormat,
                                         TBool aAppendEndTimeToOngoingEvents,
                                         TBool aUseAlternate,
                                         CAiCalendarPlugin2EventItem& aOwner );

        ~CAiCalendarPlugin2EventStrings();
        
        const TDesC& CombinedTimeString();
        
        const TDesC& StartTimeString();

        const TDesC& EndTimeString();

        const TDesC& CombinedSubjectLocationString();
        
        const TDesC& SubjectString();

        const TDesC& LocationString();

    private:

    // Construction

        CAiCalendarPlugin2EventStrings( TBool aAppendEndTimeToOngoingEvents,
                                        TBool aUseAlternate,
                                        CAiCalendarPlugin2EventItem& aOwner );

        void ConstructL( const CCalInstance& aCalInstance,
                         const TDesC& aTimeFormat,
                         const TDesC& aDateFormat );

    // New methods
    
        void ExtractDatesL(const CCalInstance& aCalInstance, const TDesC& aFormat);

        /**
         * ?member_description.
         * @since Series 60 3.2
         * @param ?arg1 ?description
         * @return ?description
         **/
        void ExtractTimeL( const CCalInstance& aCalInstance,
                           const TDesC& aFormat );

        /**
         * Extract subject and location strings from calendar instance
         * @since Series 60 3.2
         * @param aCalInstance calendar instance object
         **/
        void ExtractSubjectAndLocationL( const CCalInstance& aCalInstance );

        /**
         * Extract event subject string from cal instance
         * @since Series 60 3.2
         * @param aCalInstance calendar instance object
         * @return HBufC subject string buffer
         **/
        HBufC* ExtractSubjectL( const CCalInstance& aCalInstance );

        /**
         * Extract event location string from cal instance
         * @since Series 60 3.2
         * @param aCalInstance calendar instance object
         * @return HBufC location string buffer
         **/
        HBufC* ExtractLocationL( const CCalInstance& aCalInstance );
                                
        /**
         * Makes string alternates from ready extracted strings.
         */
        void MakeAlternateStringsL();
                                
    private:  // Data members

        HBufC* iDate;
        HBufC* iEndDate;
        HBufC* iTimeConstruct;
        HBufC* iStartTime;
        HBufC* iEndTime;
        HBufC* iSubjectLocationConstruct;
        HBufC* iSubject;
        HBufC* iLocation;
        TInt iBaseLength;
        TBool iAppendEndTimeToOngoingEvents;
        TBool iUseAlternatePublish;
        CAiCalendarPlugin2EventItem& iOwner;
                
    };

#endif  // C_AICALENDARPLUGIN2EVENTSTRINGS_H


// End of File
