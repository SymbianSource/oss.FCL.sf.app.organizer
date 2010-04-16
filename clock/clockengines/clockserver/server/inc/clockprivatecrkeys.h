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
* Description:   The file contains the uid of keys used by cenrep sheet
*
*/

#ifndef __CLOCKPRIVATECRKEYS_H__
#define __CLOCKPRIVATECRKEYS_H__

/** Nitz Settings API */
/** Provides access to the Nitz status.*/
const TUid KCRUidNitz = { 0x101F8876 };

/** Nitz state key */
const TUint32 KNitzState = 0x00000000;

/** Active Protocol key */
const TUint32 KActiveProtocol = 0x00000001;

/** Previous alarm time high */
const TUint32 KPreviousAlarmTime = 0x00000002;

/** CountDownTimer  alert file */
const TUint32 KCountDownSoundFile = 0x00000003;

/** Valid Nitz Info key */
const TUint32 KValidNitz = 0x00000004;

/** FTU date time plugin edited date */
const TUint32 KFtuPluginDate = 0x00000005;
#endif  // __CLOCKPRIVATECRKEYS_H__

// End of file

