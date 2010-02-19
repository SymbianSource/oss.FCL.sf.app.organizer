/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar Internal Cenrep Keys.
*
*/

#ifndef CALENDARINTERNALCRKEYS_H
#define CALENDARINTERNALCRKEYS_H


/** Calendar UI Settings API */
/** Provides access to the Calendar application's settings */

const TUid KCRUidCalendar = {0x101F874B};

/**
* Default alarm time in minutes before event starting time.
* Value is an integer between 0 and 60. Default: 15.
**/
const TUint32 KCalendarDefaultAlarmTime = 0x00000000;

/**
* Default starting view for Calendar. Value is an integer between 0 and 2.
* 0 = Month view (default)
* 1 = Week view
* 2 = Day view
**/
const TUint32 KCalendarDefaultStartView = 0x00000001;

/**
* Week view title in Calendar application.
* Value is eihter integer 0 or 1
* 0 = Week number (default)
* 1 = Week dates
**/
const TUint32 KCalendarWeekViewTitle = 0x00000002;

/**
* Is Chinese Lunar calendar information shown in Calendar
* (only available if feature is included in ROM image).
* Value is either integer 0 or 1
* 0 = Off
* 1 = On (default)
* This key value is deprecated and has been replaced by the KCRUidCalenLunarPlugin key.
**/
const TUint32 KCalendarLunarCalendar = 0x00000003;

/**
* Calendar alarm sound file, string.
* Default value : "z:\data\sounds\digital\alarm.aac"
**/
const TUint32 KCalendarSoundFile = 0x00000004;

/**
* Default Calendar alarm sound file.
* Default value : "z:\data\sounds\digital\alarm.aac"
**/
const TUint32 KCalendarDefaultSoundFile = 0x00000005;

/**
* A flag that is set when a Calendar alarm was lost due to time change.
* Default value: 0
*/
const TUint32 KCalendarLostAlarm = 0x00000006;

/**
* Calendar alarm snooze time interval in minutes.
* Default value: 5
*/
const TUint32 KCalendarSnoozeTime = 0x00000007;

//Added as an addition to the Symbian fix
/**
* Calendar Persistent Time to store the last time the System Time updated.
* Default Value : 0
*/
const TReal KCalendarPersistentTime  = 0x00000009;

/**
* Text string containing UIDs and enabled/disabled flag (separated
* by commas) for Calendar plugins.
* Default value: Empty
*/
const TUint32 KCalendarPluginAvailability = 0x0000000A;

/**
* Text string containing the name of last used calendar in unified editor.
*  Used for storing the last used calendar name
*  Default value : "c: calendar"
*/
const TUint32 KCalendarLastUsedCalendar = 0x0000000B;

/**
* Used for storing latest number used for appending to c:Calendar string. 
* Default value 0
*/
const TUint32 KCalendarFileNumber = 0x0000000C;

/**
* Boolean value to show missed alarm soft notification by calendar application.
* 1 = Supress the missed alarm soft notification
* 0= Show the missed alarm soft notification
**/
const TUint32 KCalendarSupressMissedAlarmSoftNotification = 0x0000000D;

/**
* Boolean value containing startup status 
* Used for calendar files creation on first boot up
* Value is either boolean 0 or 1
* 0 = Off (default) calendar files not created
* 1 = On   calendar files created
**/
const TUint32 KCalendarStartupStatus = 0x0000000E;



/**
* Text string containing default calendar colors separated by commas
* Used for creating default calendars at the startup of the device
* Default value: "16711680,65280,255"
*/
const TUint32 KCalendarDefaultColors = 0x0000000F;

/**
* String value containing unique calendar value 
* Used for identifying an entry by LUID + unique calendar value 
* Default value:"0"
*/
const TUint32 KCalendarAvailableValue = 0x00000010;


#endif  // CALENDARINTERNALCRKEYS_H
