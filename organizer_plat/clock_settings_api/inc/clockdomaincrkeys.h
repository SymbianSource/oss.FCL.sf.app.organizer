/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The file contains the uid of keys used by cenrep sheet.
*
*/

#ifndef CLOCKDOMAINCRKEYS_H
#define CLOCKDOMAINCRKEYS_H

/** Clock Settings API */
/** Provides access to the Clock application's settings */

const TUid KCRUidClockApp = {0x101F874D};

/**
* Clock alarm sound file
* 
**/
const TUint32 KClockAppSoundFile = 0x00000000;

/**
* Default clock alarm sound file
* 
**/
const TUint32 KClockAppDefaultSoundFile = 0x00000001;

/**
* Clock alarm snooze time
* 
**/
const TUint32 KClockAppSnoozeTime = 0x00000002;

#endif  // CLOCKDOMAINCRKEYS_H
