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
* Description:   ECom search interface definition.
*
*/



#ifndef DEFINES_H
#define DEFINES_H

/**
* Maximum Length of the Uid.
*/
const TInt KUidLength = 18;

/**
* No of days in one go for searching the calendar entries.
* the bigger the number, the faster the search but less responsive ui.
*/ 
const TInt 		KSearchThisManyCalEntriesInOneGo 			= 50;

#endif // __DEFINES_H
