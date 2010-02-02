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
* Description:   constants for missed alarm manager
*
*/



#ifndef __CALENMISSEDALARMCONSTANTS_H
#define __CALENMISSEDALARMCONSTANTS_H

#include <calenlauncher.h>
#include "CalenUid.h"

 // CONSTANTS
#define  UID(a) TUid::Uid(a)

const TInt KNoMissedAlarms = 0 ;
const TInt KMissedAlarmNotificationPriority = 1200;
const TAknSoftNotificationType aNotificationType = ECustomSoftNotification;
const TVwsViewId KMissedAlarmsViewId(KUidCalendar,KUidCalenMissedAlarmsView);
const TVwsViewId KMissedEventViewId(KUidCalendar,KUidCalenMissedEventView);
const TUid KCommandUid(UID(0x01));

_LIT( KMissedAlarmResourceFile, "z:\\resource\\CalenSvrMissedAlarmManagerResource.rsc");

#endif  // __CALENMISSEDALARMCONSTANTS_H
