/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file is header file for searchserverdefines
*
*/



#ifndef DEFINES_H
#define DEFINES_H

/**
* Server side common defination
*/


//  CONSTANTS


// needed for creating server thread.
const TUint KDefaultHeapSize = 0x10000;

const TUint KSearchServerSessionGranularity = 8;
const TUint KSearchEventArrayGranularity = 8;
const TInt KMemory = 1024;
const TInt KTwokMemory = 2048;
_LIT(KServerName,"SearchServer");

#endif  // DEFINES_H

// End of File
