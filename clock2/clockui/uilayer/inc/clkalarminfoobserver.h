/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   The MClkAlarmInfoObserver class for changes in alarm information. 
*
*/

#ifndef __CLK_ALARMINFO_OBSERVER_H__
#define __CLK_ALARMINFO_OBSERVER_H__

// System includes

// User includes

// Class declaration
/**
* @class MClkAlarmInfoObserver
* @brief The interface observer class. Need to be implemented by the entity which requires
*        the alarm info entered by the user in the alarm editor.
* @dll clock.exe
*/
class MClkAlarmInfoObserver
    {
    public:         // Pure virtual functions
        
        /**
        * @brief This has to be implemented by derived class, so that the alarm information
        *        is provided by the alarm editor.
        * @param aAlarmInfo The cause for notification.
        */
        virtual void SetAlarmInformation( SClkAlarmInfo aAlarmInfo, TAlarmId aAlarmId ) = 0;
    };

#endif          // __CLK_ALARMINFO_OBSERVER_H__

// End of file
