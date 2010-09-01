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


#ifndef T_AICALENDARPLUGIN2FOCUSDATA_H
#define T_AICALENDARPLUGIN2FOCUSDATA_H

#include <e32std.h>
#include <calentry.h>
#include "aicalendarplugin2constants.hrh"

enum TAI2CalOpenType
{
    EAI2CalOpenEvent = 0,
    EAI2CalOpenToday,
    EAI2CalOpenOnEventDay
};

/**
 *  Utility class to store entries focus data.
 *
 *  @lib aicalendarplugin2
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( TAiCalendarFocusData )
    {

    public: // methods
    
        void LaunchCalendarApplicationL();

    public: // data
    
        TCalLocalUid iLocalUid;
        
        TTime iEntryTime;
        
        TAI2CalOpenType iType;

        TBool iOpenInViewer;
        
    private: // methods
    
        void CommandTailL( HBufC16*& aTailBuffer );
        
    };

#endif  // C_AICALENDARPLUGIN2FOCUSDATA_H


// End of File
