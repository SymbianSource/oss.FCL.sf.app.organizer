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
* Description:   UIDS used by the S60 Calendar
*
*/



#ifndef CALENUID_H
#define CALENUID_H

// UID of the S60 Calendar application
const TUid KUidCalendar         = { 0x10005901 };

// UIDS of the S60 Calendar native views
const TInt KCalenMonthViewUidValue        = 0x10283360 ;
const TInt KCalenWeekViewUidValue         = 0x10283361;
const TInt KCalenDayViewUidValue          = 0x10283362;
const TInt KCalenTodoViewUidValue         = 0x10283363;
const TInt KCalenEventViewUidValue   	  = 0x10283364;
const TInt KCalenMissedAlarmsViewUidValue = 0x10283365;
const TInt KCalenMissedEventViewUidValue  = 0x10283366;
const TInt KCalenDummyViewUidValue        = 0x10283367;  

const TUid KUidCalenMonthView   	 = { KCalenMonthViewUidValue };
const TUid KUidCalenWeekView    	 = { KCalenWeekViewUidValue };
const TUid KUidCalenDayView     	 = { KCalenDayViewUidValue };
const TUid KUidCalenTodoView    	 = { KCalenTodoViewUidValue };
const TUid KUidCalenEventView   	 = { KCalenEventViewUidValue };
const TUid KUidCalenMissedAlarmsView = { KCalenMissedAlarmsViewUidValue };
const TUid KUidCalenMissedEventView  = { KCalenMissedEventViewUidValue };
const TUid KUidCalenDummyView        = { KCalenDummyViewUidValue };

const TUid KUidCalenGSPlugin         = { 0x1020E8B8 };
const TUid KUidCalenBCDayViewPlugin	 = { 0x20021379 };

// View Message UIDS for Calendar views
const TUid KUidCalenShowCloseCba  = { 0x00000001 };
const TUid KUidCalenShowBackCba   = { 0x00000002 };
const TUid KCalenHideInBackGround = { 0x00000003 };
const TUid KCalenDummyUid = { 0x0000000A };
const TUid KUidCalenShowAlarmCba = { 0x00000004 };
const TUid KUidCalenShowAlarmStopOnlyCba = { 0x00000005 };

const TUid KCalenThaiImplUid = {0x102823A7};
const TUid KCalenChineseImplUid = {0x102823A5};
const TUid KCalenVietnameseImplUid = {0x102823A6};
const TUid KCalenKoreanImplUid = {0x200100A2};

#endif // CALENUID_H

// End of file
