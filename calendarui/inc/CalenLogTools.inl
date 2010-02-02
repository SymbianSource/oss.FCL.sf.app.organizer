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
* Description: Log file of Calendar Application.
*
*/



#ifndef CALENLOGTOOLS_INL
#define CALENLOGTOOLS_INL
#include <e32std.h>
#include <flogger.h>

//#define __CALEN_LOG__
#ifdef __CALEN_LOG__

_LIT(KLogDir, "calendar");
_LIT(KLogFile, "calsvr.log");

inline TInt Micro2Milli(const TTimeIntervalMicroSeconds& aX)
    {
    return TInt( aX.Int64() / 1000 );
    }

inline void LogTimeStamp(const TDesC& aName, const TInt& aVal) 
    {
    TTime timestamp;
    timestamp.HomeTime();

    TTime dayBegin(0);
    dayBegin += timestamp.DaysFrom( dayBegin );
    TTimeIntervalMicroSeconds microSecs = timestamp.MicroSecondsFrom( dayBegin );

    RFileLogger log; 
    log.Connect();
    log.CreateLog( KLogDir, KLogFile, EFileLoggingModeAppend );
    log.SetDateAndTime(EFalse, EFalse);
    log.WriteFormat(_L("%S, %d, %d"), &aName, aVal, Micro2Milli(microSecs));
    log.Close();
    }

inline void LogTimeStamp(const TDesC& aName) 
    {
    TTime timestamp;
    timestamp.HomeTime();

    TTime dayBegin(0);
    dayBegin += timestamp.DaysFrom( dayBegin );
    TTimeIntervalMicroSeconds microSecs = timestamp.MicroSecondsFrom( dayBegin );

    RFileLogger log; 
    log.Connect();
    log.CreateLog( KLogDir, KLogFile, EFileLoggingModeAppend );
    log.SetDateAndTime(EFalse, EFalse);
    log.WriteFormat(_L("%S, %d"), &aName, Micro2Milli(microSecs));
    log.Close();
    }

#else

inline TInt Micro2Milli(const TTimeIntervalMicroSeconds&);
inline void LogTimeStamp(const TDesC&, const TInt&);
inline void LogTimeStamp(const TDesC&);

#endif 

#endif // CALENLOGTOOLS_INL
