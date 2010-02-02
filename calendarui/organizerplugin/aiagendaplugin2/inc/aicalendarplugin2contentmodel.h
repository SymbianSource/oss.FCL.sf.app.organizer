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


#ifndef AICALENDARPLUGIN2CONTENTMODEL_H
#define AICALENDARPLUGIN2CONTENTMODEL_H

#include <aicontentmodel.h>

// ================================= CONTENT ===================================

/**
 * Content items Ids
 */
enum TAICalendarPlugin2ContentIds
    {
    EAICalendarPlugin2EventIcon = 1,
    EAICalendarPlugin2EventTextFirstLine,
    EAICalendarPlugin2EventTextSecondLine,
    EAICalendarPlugin2EventTimeStart,
    EAICalendarPlugin2EventTimeEnd,
    EAICalendarPlugin2EventSubject,
    EAICalendarPlugin2EventLocation,
    EAICalendarPlugin2InfoText,
    EAICalendarPlugin2InfoIcon,
    EAICalendarPlugin2TimeStartAndSubject
    };

/**
 * Content that the plugin will publish
 * { local enum id, character id, mime type }
 */
const TAiContentItem KAICalendarPlugin2Content[] =
    {

    // Calendar plugin timed event icon as CGulIcon.
    { EAICalendarPlugin2EventIcon,          L"EventIcon",       KAiContentTypeBitmap },

    // Calendar plugin event first text line as plain text
    { EAICalendarPlugin2EventTextFirstLine, L"Event1stLine",    "text/plain" },

    // Calendar plugin event second text line as plain text
    { EAICalendarPlugin2EventTextSecondLine,L"Event2ndLine",    "text/plain" },

    // Calendar plugin event start time as plain text
    { EAICalendarPlugin2EventTimeStart,     L"EventStart",      "text/plain" },

    // Calendar plugin event end time as plain text
    { EAICalendarPlugin2EventTimeEnd,       L"EventEnd",        "text/plain" },

    // Calendar plugin event subject as plain text
    { EAICalendarPlugin2EventSubject,       L"EventSubject",    "text/plain" },

    // Calendar plugin event location as plain text
    { EAICalendarPlugin2EventLocation,      L"EventLocation",   "text/plain" },
    
    // Calendar plugin info text (like no more event for today) may be
    // published here. Depends on the theme settings.
    { EAICalendarPlugin2InfoText,      L"InfoText",   "text/plain" },
    
    // Calendar plugin info icon may be
    // published here. Depends on the theme settings.
    { EAICalendarPlugin2InfoIcon,      L"InfoIcon",   KAiContentTypeBitmap },
    
    // Calendar plugin event start time + subject in one row
    { EAICalendarPlugin2TimeStartAndSubject, L"StartTimeAndSubject", "text/plain" }

    };

// ================================ RESOURCES ==================================

/**
 * Resources used by Calendar plugin 2.
 * Localized strings and icons.
 */
enum TAICalendarPlugin2ResourceIds
    {
    EAICalendarPlugin2NoMoreEventsForToday = 1, // String "No More Events for Today"
    EAICalendarPlugin2NoEventsForToday, // String "No Events for Today"
    EAICalendarPlugin2NextEventTomorrow, // String "Next event tomorrow:"
    EAICalendarPlugin2NextEvent, // String "Next event:"
    EAICalendarPlugin2NextEvents, // String "Next events:"
    EAICalendarPlugin2IconAppt, // Normal meeting icon
    EAICalendarPlugin2IconMeetingAccepted, // Accepted meeting icon
    EAICalendarPlugin2IconMeetingNotAnswered, // Not answered meeting icon
    EAICalendarPlugin2IconMeetingTentative, // Tentatively accepted meeting icon
    EAICalendarPlugin2IconTodo, // Uncompleted To-Do icon
    EAICalendarPlugin2IconMemo, // Memo icon
    EAICalendarPlugin2IconAnniv // Anniversary icon
    };

/**
 * Mapping resources to UI elements.
 * { local enum id, character id, mime type }
 */
const TAiContentItem KAICalendarPlugin2Resources[] =
    {

    // This is published through EAICalendarPlugin2EventTextFirstLine
    { EAICalendarPlugin2NoMoreEventsForToday,     L"NoMoreEventsForToday",   "text/plain" },

    // This is published through EAICalendarPlugin2EventTextFirstLine
    { EAICalendarPlugin2NoEventsForToday,         L"NoEventsForToday",       "text/plain" },

    // This is published through EAICalendarPlugin2EventTextFirstLine
    { EAICalendarPlugin2NextEventTomorrow,        L"NextEventTomorrow",      "text/plain" },

    // This is published through EAICalendarPlugin2EventTextFirstLine
    { EAICalendarPlugin2NextEvent,                L"NextEvent",              "text/plain" },

    // This is published through EAICalendarPlugin2EventTextFirstLine
    { EAICalendarPlugin2NextEvents,               L"NextEvents",             "text/plain" },

    // This is published through EAICalendarPlugin2EventIcon
    { EAICalendarPlugin2IconAppt,                 L"IconAppt",               KAiContentTypeBitmap },

    // This is published through EAICalendarPlugin2EventIcon
    { EAICalendarPlugin2IconMeetingAccepted,      L"IconMeetAccepted",       KAiContentTypeBitmap },

    // This is published through EAICalendarPlugin2EventIcon
    { EAICalendarPlugin2IconMeetingNotAnswered,   L"IconMeetNotAnswered",    KAiContentTypeBitmap },

    // This is published through EAICalendarPlugin2EventIcon
    { EAICalendarPlugin2IconMeetingTentative,     L"IconMeetTentative",      KAiContentTypeBitmap },

    // This is published through EAICalendarPlugin2EventIcon
    { EAICalendarPlugin2IconTodo,                 L"IconTodo",               KAiContentTypeBitmap },

    // This is published through EAICalendarPlugin2EventIcon
    { EAICalendarPlugin2IconMemo,                 L"IconMemo",               KAiContentTypeBitmap },

    // This is published through EAICalendarPlugin2EventIcon
    { EAICalendarPlugin2IconAnniv,                L"IconAnniv",              KAiContentTypeBitmap }

    };

// ============================ SERVICES (Events) ==============================

/**
 * Calendar plugin 2's events
 */
enum TAICalendarPlugin2EventIds
    {
    EAICalendarPlugin2EventItemSelected = 1
    };

/**
 * Mapping events to UI events.
 * { local enum id, character id, type }
 */
const TAiContentItem KAICalendarPlugin2Events[] =
    {
    // Calendar view item selected - parameter is index of the selected item.
    // Index = zero = 0 signifies the title line. Indexses strating from 1 and up
    // are the actual calendar events or "No calendar events for today"/"No more calendar
    // events for today" lines.
    // SPEC:
    //
    //      - Selecting "No cal.." or Calendar-title lines launches 
    //        Calendar application in Today view.
    //
    //      - Selecting Calendar event opens Calendar application
    //        in that particular event.
    //
    { EAICalendarPlugin2EventItemSelected, L"ItemSelected", "int" }

    };

#endif // AICALENDARPLUGIN2CONTENTMODEL_H
