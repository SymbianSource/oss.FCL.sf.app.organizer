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
* Description:   misssed alarm store constants
*
*/



#ifndef __MISSEDALARMSTORECONSTANTS_H
#define __MISSEDALARMSTORECONSTANTS_H

static const TUid KUidRepository = { 0x2001111B };
static const TUint32 KLuidPartialKey = 0x00000000;
static const TUint32 KLuidMask = 0xC0000000;
static const TUint32 KTTimeShift = 0x40000000;
static const TUint32 KCalFileNameShift = 0x80000000;
static const TUint32 KMaxKeys = 10;

#endif  // __MISSEDALARMSTORECONSTANTS_H

