/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// alarmalert.h

#ifndef __ALARM_ALERT_H__
#define __ALARM_ALERT_H__

// The plugin name. Do not remove the "com.nokia.hb" part.
const QString ALARM_ALERT_PLUGIN("com.nokia.hb.alarmalertdialog");

// The string identifier for command
const QString alarmCommand("command");

// The string identifier for alarm subject
const QString alarmSubject("subject");

// The string identifier for alarm location
const QString alarmLocation("location");

// The string identifier for alarm date and time
const QString alarmDateTime("dateTime");

// The string identifier for alarm type
const QString alarmType("type");

// The string identifier for alarm ringing type
const QString alarmRingingType("ringingType");

// The string identifier for alarm snoozability
const QString alarmCanSnooze("canSnooze");


/**
 * @enum Commands
 * @brief Some common commands that can be sent
 * back by the alarm dialog
 */
enum Commands {
	Stop,
	Silence,
	Snooze,
	Shown,
	Other
};

#endif		// __ALARM_ALERT_H__
