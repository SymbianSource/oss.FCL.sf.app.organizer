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
* Description:   central repository keys for missed alarm store
*
*/



#ifndef MISSEDALARMSTORECRKEYS_H
#define MISSEDALARMSTORECRKEYS_H

/**
 * Missed alarm store repository uid
 */

const TUid KCRUidMissedAlarmStore = {0x2001111B};

/**
 * Minimum Value of the missed alarm cenrep key's range  
 */
const TUint32 KMissedAlarmsMinValue = 0x00000000;

/**
 * Maximum Value of the missed alarm cenrep key's range  
 */
const TUint32 KMissedAlarmsMaxValue = 0x000000FF;

#endif  // MISSEDALARMSTORECRKEYS_H
