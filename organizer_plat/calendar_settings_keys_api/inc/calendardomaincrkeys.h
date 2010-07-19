/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   The file contains the uid of domain keys.
*
*/

#ifndef CALENDARDOMAINCRKEYS_H
#define CALENDARDOMAINCRKEYS_H

/** Calendar UI Settings keys*/
/** Provides access to the Calendar application's settings keys */
const long int KCRUidCalendar =  { 0x101F874B };

/**
* Defines the time between alarm snoozes, in minutes.
* Default value: 5
**/
const long int KCalendarSnoozeTime = 0x00000001;

/**
* Default Calendar alarm sound file.
* Default value : "z:\data\sounds\digital\alarm.aac"
**/
const long int KCalendarDefaultSoundFile = 0x00000003;

#endif  	// CALENDARDOMAINCRKEYS_H

// End of file	--Don't remove this.