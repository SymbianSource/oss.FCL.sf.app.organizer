/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*  Description : Class looking after alarm fields for forms.
*
*/

#ifndef CALENLUNARPANIC_H
#define CALENLUNARPANIC_H

enum TCalenLunarPanic
    {
    EPanicLunarResourceLoading = 1
    };

void Panic( TCalenLunarPanic aReason )
    {
    _LIT(KPanicText, "CalenLunarPlugin");
    User::Panic( KPanicText, aReason );
    }

#endif
