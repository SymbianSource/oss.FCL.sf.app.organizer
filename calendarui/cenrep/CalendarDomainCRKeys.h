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
* Description: Calendar Domain Cenrep Keys.
*
*/

#ifndef CALENDARDOMAINCRKEYS_H
#define CALENDARDOMAINCRKEYS_H

//#include <CalendarSDKCRKeys.h>

/** Calendar UI Settings API */
/** Provides access to the Calendar application's settings */

const TUid KCRUidCalendar = {0x101F874B};

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

#endif