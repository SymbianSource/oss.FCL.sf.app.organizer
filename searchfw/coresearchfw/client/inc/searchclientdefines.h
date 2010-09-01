/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Client side common parameters
*
*/



#ifndef DEFINES_H
#define DEFINES_H

//  CONSTANTS

// The number of message slots available to this session.
// This determines the number of outstanding requests the 
// client may have with the server at any one time

const TUint KDefaultMessageSlots = 4;
const TInt KMemory = 1024;
const TInt KService = 30;
const TInt KLightResult = 300;
const TInt KDefaultBufSize = 100 ;
const TInt KMaxDocIDSize = 100;

#endif  // DEFINES_H

// End of File
