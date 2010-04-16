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
* Description:  ?Description
*
*/


#ifndef CALENDARPRIVATECRKEYS_H
#define CALENDARPRIVATECRKEYS_H

#include <CalendarInternalCRKeys.h>

/** Calendar UI Local Variation API */
/** Provides access to the Calendar application's local variation setting */

const TUid KCRUidCalendarLV = {0x101F874C};


/**
* Local variation bitmask for Caledar configuration.
* For values refer to CalendarVariant.hrh.
*
**/
const TUint32 KCalenLVFlags = 0x00000007;


/** Calendar Thai Plugin  Settings API */
/** Provides access to the Calendar Thai Plugin's settings */
const TUid KCRUidCalenThaiPlugin = {0x102823D2};

/**
* User modifiable setting
* Is Thai regional Calendar information shown in Calendar
* (only available if feature is included in phone,
*  currnet display language is Thai)
* Value is either integer 0 or 1 
* 0 = Off 
* 1 = On (default)
**/
const TUint32 KCalenThaiPluginEnabled = 0x00000000;


/** Calendar Lunar Calendar Plugin Settings API */
/** Provides access to the Calendar Lunar Plugin's settings */
const TUid KCRUidCalenLunarPlugin = { 0x102823D1 };

/**
* User modifiable setting
* Is Chinese Lunar Calendar information shown in Calendar
* (only available if feature is included in phone,
*  currnet display language is some of Chinese languages)
* Value is either integer 0 or 1 
* 0 = Off 
* 1 = On (default)
**/
const TUint32 KCalenChineseLunarEnabled = 0x00000000;


/**
* User modifiable setting
* Is Vietnamese Lunar Calendar information shown in Calendar
* (only available if feature is included in phone,
*  currnet display language is Vietnamese languages)
* Value is either integer 0 or 1 
* 0 = Off 
* 1 = On (default)
**/
const TUint32 KCalenVietnameseLunarEnabled = 0x00000001;


/** Calendar UI Extensions Settings API */
/** Provides access to user settings that enable Calendar UI extensions and customizations */
const TUid KCRUidCalenUIExtensions = { 0x10282ED7 };

/**
* User modifiable setting to enable the AIW External Calendar.
* (only available if an AIW provider is included in phone)
* Value is either integer 0 or 1 
* 0 = Off 
* 1 = On (default)
**/
const TUint32 KCalenExternalCalendarEnabled = 0x00000000;


#endif  // CALENDARPRIVATECRKEYS_H
