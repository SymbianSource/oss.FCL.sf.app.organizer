/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Expiration timer for timed calendar events.
 *                Always set to expire by the next timed calendar event (for today).
 *
*/



#ifndef CALENPREVIEWTIMER_H
#define CALENPREVIEWTIMER_H

#include "CalendarVariant.hrh"

// INCLUDES
#include <e32base.h>


// CLASS DECLARATION
NONSHARABLE_CLASS( CCalenPreviewTimer ) : public CTimer
    {
    public:
    /**
     * Two-phased constructor.
     * @param ?arg1 ?description
     **/
    static CCalenPreviewTimer* NewL(TCallBack& aCallBack);

    /**
     * ?member_description.
     **/
    void ConstructL(void);

    /**
     * Destructor.
     **/
    virtual ~CCalenPreviewTimer(void);

    private:
    /**
     * Hide default constructor.
     **/
    CCalenPreviewTimer(TCallBack& aCallBack);

    protected:  // From CActive
    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void RunL(void);

    private:  // Data members
    // ?one_line_short_description_of_data
    TCallBack iCallBack;
    };

#endif  // CALENPREVIEWTIMER_H


// End of File
