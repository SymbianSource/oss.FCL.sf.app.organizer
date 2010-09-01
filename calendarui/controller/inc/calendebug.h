/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Debug tools for Calendar 
*
*/



#ifndef CALENDEBUG_H
#define CALENDEBUG_H

//
// Debug macros are compiled empty in non-debug builds
//

#ifndef _DEBUG

#define LOGTIME(m,t)
#define LOGRRULE(r)
#define LOGINSTANCE(x)
#define LOGENTRY(x)

#else

#define LOGTIME(m,t)    CalenDebug::LogTimeL(m, t)
#define LOGRRULE(r)     CalenDebug::LogRRuleL(r)
#define LOGINSTANCE(x)  CalenDebug::LogInstanceL(x)
#define LOGENTRY(x)     CalenDebug::LogEntryL(x)

#include <e32std.h>
class TCalRRule;
class CCalInstance;
class CCalEntry;

namespace CalenDebug
    {
    void LogTimeL(const TDesC& aMsg, const TTime& aTime);

    void LogRRuleL(const TCalRRule& aRule);

    void LogInstanceL(CCalInstance& aInstance);

    void LogEntryL(CCalEntry& aEntry);
    }

#endif // _DEBUG

#endif // CALENDEBUG_H
