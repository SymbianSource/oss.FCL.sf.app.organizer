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
* Description:  Expiration timer for timed calendar events.
 *                Always set to expire by the next timed calendar event (for today).
 *
*/


#ifndef EVENTEXPIRATIONTIMER_H
#define EVENTEXPIRATIONTIMER_H

// INCLUDES
#include <e32base.h>


// CLASS DECLARATION
NONSHARABLE_CLASS( CEventExpirationTimer ) : public CTimer
{
 public:
    /**
     * Two-phased constructor.
     * @since Series 60 2.6
     * @param ?arg1 ?description
     **/
    static CEventExpirationTimer* NewL(TCallBack& aCallBack);

    /**
     * ?member_description.
     * @since Series 60 2.6
     **/
    void ConstructL(void);

    /**
     * Destructor.
     * @since Series 60 2.6
     **/
    virtual ~CEventExpirationTimer(void);

 private:
    /**
     * Hide default constructor.
     * @since Series 60 2.6
     **/
    CEventExpirationTimer(TCallBack& aCallBack);

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

#endif  // EVENTEXPIRATIONTIMER_H


// End of File
