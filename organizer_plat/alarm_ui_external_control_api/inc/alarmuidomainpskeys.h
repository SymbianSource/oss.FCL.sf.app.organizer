/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Domain Publish&Subscribe definitions of the AlarmUI subsystem.
*
*/



#ifndef ALARMUIDOMAINPSKEYS_H
#define ALARMUIDOMAINPSKEYS_H

// INCLUDES
#include <e32property.h>
#include <e32err.h>

const TUid KPSUidAlarmExtCntl = { 0x102072D4 }; // reusing an AlarmUI dll UID

// =============================================================================
// Alarm UI External Control API
// =============================================================================
// Use TUid KPSUidAlarmExtCntl = { 0x102072D4 } 

/**
* Notifies Alarm UI to stop an active alarm.
*/
const TUint32 KAlarmStopKey = 0x00000001;
enum TAlarmUIStopAlarm
    {
    EAlarmUIStopAlarmUninitialized = 0,
    EAlarmUIStopAlarm
    };

/**
* Notifies Alarm UI to snooze an active alarm.
*/
const TUint32 KAlarmSnoozeKey = 0x00000002;
enum TAlarmUISnoozeAlarm
    {
    EAlarmUISnoozeAlarmUninitialized = 0,
    EAlarmUISnoozeAlarm
    };


#endif // ALARMUIDOMAINPSKEYS_H

// End of File
