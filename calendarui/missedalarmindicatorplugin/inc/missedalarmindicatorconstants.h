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
* Description:   constants for missed alarm indicator plugin
*
*/



#ifndef __MISSEDALARMINDICATORCONSTANTS_H
#define __MISSEDALARMINDICATORCONSTANTS_H

#include "CalenUid.h"

//Constants
#define  KAppUid KUidCalendar
#define  KAppMissedAlarmsViewId KUidCalenMissedAlarmsView
#define  KAppMissedEventViewId KUidCalenMissedEventView
#define  KMissedAlarmPluginImplementationUid  0x20011119

#define  UID(a) TUid::Uid(a)

_LIT( KIndicatorResourceFile,"z:missedalarmindicatorplugin.rsc" );

const TInt KListgranularity = 2;
const TInt KTextLength = 150;
const TInt KMaxNumberLength = 4;

#endif  // __MISSEDALARMINDICATORCONSTANTS_H
