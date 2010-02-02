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



//debug
#include "calendarui_debug.h"

#include "calendebug.h"

#ifdef _DEBUG

#include <calalarm.h>
#include <calentry.h>
#include <calinstance.h>
#include <calrrule.h>
#include <e32debug.h>
#include <utf.h> 

namespace CalenDebug 
    {
    void LogTimeL(const TDesC& aMsg, const TTime& aTime)
        {
        TBuf<30> tmp;
        if ( aTime == Time::NullTTime() )
            {
            tmp = _L("null time");
            }
        else 
            {
            _LIT(KDtFmt, "%F%H:%T %D.%M.%Y");
            aTime.FormatL( tmp, KDtFmt ); 
            }
        RDebug::Print(aMsg, &tmp);
        }

    void LogRRuleL(const TCalRRule& aRule)
        {
        _LIT(KYearly,  "Yearly");
        _LIT(KMonthly, "Monthly");
        _LIT(KWeekly,  "Weekly");
        _LIT(KDaily,   "Daily");
        _LIT(KInvalid, "Invalid");

        TBuf<20> typeStr;
        switch ( aRule.Type() )
            {
            case TCalRRule::EInvalid:
                typeStr = KInvalid;
                break;
            case TCalRRule::EDaily:
                typeStr = KDaily;
                break;
            case TCalRRule::EWeekly:
                typeStr = KWeekly;
                break;
            case TCalRRule::EMonthly:
                typeStr = KMonthly;
                break;
            case TCalRRule::EYearly:
                typeStr = KYearly;
                break;
            }
        RDebug::RawPrint( typeStr );
        RDebug::Print( _L("Count   %d"), aRule.Count() );
        RDebug::Print( _L("Interv. %d"), aRule.Interval() );

        RDebug::RawPrint( _L("UTC:") );
        LOGTIME(       _L("DtStart %S"), aRule.DtStart().TimeUtcL() );
        LOGTIME(       _L("Until   %S"), aRule.Until().TimeUtcL() );

        RDebug::RawPrint( _L("Local:") );
        LOGTIME(       _L("DtStart %S"), aRule.DtStart().TimeLocalL() );
        LOGTIME(       _L("Until   %S"), aRule.Until().TimeLocalL() );
        }

    void LogGuidL( const TDesC8& aGuid )
        {
        HBufC16* guid = CnvUtfConverter::ConvertToUnicodeFromUtf8L( aGuid );
        CleanupStack::PushL( guid );
        RDebug::Print( _L("Guid:\"%S\""), guid );
        CleanupStack::PopAndDestroy( guid );
        }

    void LogCalTimeL( TCalTime& aCalTime )
        {
        LOGTIME(_L("UTC   %S"), aCalTime.TimeUtcL());
        LOGTIME(_L("Local %S"), aCalTime.TimeLocalL());
        }

    void LogEntryRecurrenceIdL( CCalEntry& aEntry )
        {
        TCalTime recurrenceId;
        TRAPD( err, recurrenceId = aEntry.RecurrenceIdL() );
        if ( err == KErrNone )
            {
            if ( recurrenceId.TimeUtcL() == Time::NullTTime() )
                {
                RDebug::RawPrint( _L("No recurrence id") );
                }
            else
                {
                RDebug::RawPrint( _L("Recurrence id:") );
                LogCalTimeL( recurrenceId );
                }
            }
        else
            {
            RDebug::Print( _L("Recurrence id: reading leaved with %d"), err );
            }
        }

    void LogRecurrenceRangeL( CalCommon::TRecurrenceRange aRange )
        {
        switch ( aRange )
            {
            case CalCommon::EThisOnly:
                RDebug::RawPrint( _L("ThisOnly") );
                break;
            case CalCommon::EThisAndAll:
                RDebug::RawPrint( _L("ThisAndAll") );
                break;
            case CalCommon::EThisAndFuture:
                RDebug::RawPrint( _L("ThisAndFuture") );
                break;
            case CalCommon::EThisAndPrior:
                RDebug::RawPrint( _L("ThisAndPrior") );
                break;
            }
        }

    void LogDatesL( RArray<TCalTime>& aExDateList )
        {

        if ( aExDateList.Count() > 0 )
            {
            RDebug::RawPrint( _L("START                    ***") );
            for (TInt i=0; i < aExDateList.Count(); i++) 
                {
                LogCalTimeL( aExDateList[i] );
                }
            RDebug::RawPrint( _L("END                      ***") );
            }
        }

    void LogEntryDetailsL(CCalEntry& aEntry)
        {
        RDebug::RawPrint( aEntry.SummaryL() );
        RDebug::RawPrint( aEntry.LocationL() );

        LogGuidL( aEntry.UidL() );
        LogEntryRecurrenceIdL( aEntry );
        LogRecurrenceRangeL( aEntry.RecurrenceRangeL() );

        RArray< TCalTime > exceptionDates;
        CleanupClosePushL( exceptionDates );
        aEntry.GetExceptionDatesL( exceptionDates );
        RDebug::Print( _L("Exception dates %d"), exceptionDates.Count() );
        LogDatesL( exceptionDates );
        CleanupStack::PopAndDestroy( & exceptionDates );

        // Guid
        //     HBufC* guid = HBufC16::NewLC(500);
        //     guid->Copy( aEntry.UidL() );
        //     RDebug::Print( *guid );
        //     CleanupStack::PopAndDestroy( guid );

        // Entry Start and end times
        // Note %S modifier is very important!
        // UTC
        RDebug::RawPrint( _L("Entry UTC:" ) );
        LOGTIME(_L("Start %S"), aEntry.StartTimeL().TimeUtcL());
        LOGTIME(_L("End   %S"), aEntry.EndTimeL().TimeUtcL());

        // Local 
        RDebug::RawPrint( _L("Entry Local:" ) );
        LOGTIME(_L("Start %S"), aEntry.StartTimeL().TimeLocalL());
        LOGTIME(_L("End   %S"), aEntry.EndTimeL().TimeLocalL());

        // Alarm 
        CCalAlarm* alarm = aEntry.AlarmL();
        CleanupStack::PushL( alarm );
        if ( alarm )
            {
            RDebug::RawPrint( _L("Has alarm" ) );
            RDebug::Print( _L(" offset: %d"), alarm->TimeOffset().Int() );
            }
        else
            {
            RDebug::RawPrint( _L("No alarm") );
            }
        CleanupStack::PopAndDestroy(alarm);

        // Repeat rule 
        TCalRRule rrule;
        if ( aEntry.GetRRuleL(rrule) )
            {
            RDebug::RawPrint( _L("Has rrule" ) );
            LOGRRULE( rrule );
            }
        else
            {
            RDebug::RawPrint( _L("No rrule" ) );
            }
        }

    void LogInstanceDetailsL(CCalInstance& aInstance)
        {
        // Instance times
        // UTC
        RDebug::RawPrint( _L("Instance UTC:") );
        LOGTIME(_L("time  %S"), aInstance.Time().TimeUtcL());
        LOGTIME(_L("start %S"), aInstance.StartTimeL().TimeUtcL());
        LOGTIME(_L("end   %S"), aInstance.EndTimeL().TimeUtcL());

        // Local 
        RDebug::RawPrint( _L("Instance Local:") );
        LOGTIME(_L("time  %S"), aInstance.Time().TimeLocalL());
        LOGTIME(_L("start %S"), aInstance.StartTimeL().TimeLocalL());
        LOGTIME(_L("end   %S"), aInstance.EndTimeL().TimeLocalL());
        }

    void LogEntryL(CCalEntry& aEntry)
        {
        RDebug::RawPrint( _L("----- ENTRY -----") );
        LogEntryDetailsL( aEntry );
        }

    void LogInstanceL(CCalInstance& aInstance)
        {
        RDebug::RawPrint( _L("----- INSTANCE -----") );
        LogEntryDetailsL( aInstance.Entry() );
        LogInstanceDetailsL( aInstance );
        }
    }

#endif // _DEBUG

