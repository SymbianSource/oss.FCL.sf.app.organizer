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
* Description:  Calendar timer class.
 *
*/



#ifndef __CALENTIMER_H__
#define __CALENTIMER_H__

// INCLUDES
#include <E32Base.h>

// CLASS DECLARATION

NONSHARABLE_CLASS( CCalenTimer ) : public CTimer
{
 public:
    /**
     * Two-phased constructor.
     * @since S60 3.0
     * @param ?arg1 ?description
     **/
    static CCalenTimer* NewL(TCallBack& aCallBack);

    /**
     * ?member_description.
     * @since S60 3.0
     **/
    void ConstructL(void);

    /**
     * Destructor.
     * @since S60 3.0
     **/
    virtual ~CCalenTimer(void);

 private:
    /**
     * Hide default constructor.
     * @since S60 3.0
     **/
    inline CCalenTimer(TCallBack& aCallBack);

 protected:  // From CActive
    /**
     * ?member_description.
     * @since S60 3.0
     **/
    inline void RunL(void);

 private:  // Data members
    // ?one_line_short_description_of_data
    TCallBack iCallBack;

};

#include "CalenTimer.inl"

#endif  // __CALENTIMER_H__


// End of File
