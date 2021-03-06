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
* Description: TCalenThaiPanic Class to show panics.
*
*/


#ifndef CALENTHAIPANIC_H
#define CALENTHAIPANIC_H

enum TCalenThaiPanic
    {
    EPanicThaiResourceLoading = 1
    };

void Panic(TCalenThaiPanic aReason)
    {
    _LIT(KPanicText, "CalenThaiPlugin");
    User::Panic(KPanicText,aReason);
    }

#endif
