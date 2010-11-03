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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <centralrepository.h>
#include <calcalendarinfo.h>
#include <calendarinternalcrkeys.h>
#include "CalendarMultiCalUtilsTest.h"
#include <calenmulticaluids.hrh>
#include <CalenInterimUtils2.h>

const TInt KBuffLength = 24;

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

//  LOCAL CONSTANTS AND MACROS
const TInt KDefaultStartTime( 8 );

_LIT(KCalendarDatabaseFilePath,"c:calendar");

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CCalendarMultiCalUtilsTest::Delete() 
    {
    // Nothing.
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCalendarMultiCalUtilsTest::RunMethodL( 
        CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        // Example: ENTRY( "Example", CCalendarMultiCalUtilsTest::ExampleL ),

        /** Test functions for API CalenMultiCalUtils */
        ENTRY( "TestGetNextAvailableCalFileL", 
                CCalendarMultiCalUtilsTest::TestGetNextAvailableCalFileL ),
                
        ENTRY( "TestGetNextAvailableOffsetL",
                CCalendarMultiCalUtilsTest::TestGetNextAvailableOffsetL ) ,

        ENTRY( "TestCalendarPropertiesL",
                CCalendarMultiCalUtilsTest::TestCalendarPropertiesL ) ,

        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CCalendarMultiCalUtilsTest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarMultiCalUtilsTest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KCalendarMultiCalUtilsTest, "CalendarMultiCalUtilsTest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KCalendarMultiCalUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt index = 0;
    TPtrC parseString;
    _LIT( KParam, "Param[%i]: %S" );
    while( KErrNone == aItem.GetNextString ( parseString ) )
        {
        TestModuleIf().Printf( index, KCalendarMultiCalUtilsTest, 
                                KParam, index, &parseString );
        index++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCalendarMultiCalUtilsTest::TestGetNextAvailableCalFileL
// Test case to verify if a given day is within a specified range or not.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalendarMultiCalUtilsTest::TestGetNextAvailableCalFileL(
                                            CStifItemParser& /*aItem*/ )
    {
    
    // Print to UI
    _LIT( KCalendarMultiCalUtilsTest, "CalendarMultiCalUtilsTest" );
    _LIT( KExample, "In TestGetNextAvailableCalFileL" );
    TestModuleIf().Printf( 0, KCalendarMultiCalUtilsTest, KExample );
    // Print to log file
    iLog->Log( KExample );
    
    TInt nextAvailableCalNumber(0);
    CRepository* cenRep = CRepository::NewL(KCRUidCalendar);
    CleanupStack::PushL(cenRep);
    User::LeaveIfError(cenRep->Get(KCalendarFileNumber,
            nextAvailableCalNumber));
    nextAvailableCalNumber++;
    CleanupStack::PopAndDestroy(cenRep);

    TBuf<KMaxFileName> caleFileNameBuffer;
    caleFileNameBuffer.Append(KCalendarDatabaseFilePath);
    caleFileNameBuffer.AppendNum(nextAvailableCalNumber);
    
    TBool actualCenrepValue(EFalse);
    // get the file name from the api
    HBufC16* fileNameFromApi = CCalenMultiCalUtil::GetNextAvailableCalFileL();
    CleanupStack::PushL(fileNameFromApi);
    // do the comparision with the local value
    if(!fileNameFromApi->Des().Compare(caleFileNameBuffer))
        {
        actualCenrepValue = ETrue;
        }
    
    TBool previousValue(EFalse);
    HBufC16* secondFileName = CCalenMultiCalUtil::GetNextAvailableCalFileL();
    CleanupStack::PushL(secondFileName);
    if(secondFileName->Des().Compare(fileNameFromApi->Des()))
        {
        previousValue = ETrue;
        }
    CleanupStack::PopAndDestroy(secondFileName);
    CleanupStack::PopAndDestroy(fileNameFromApi);
    
    if( !(actualCenrepValue && previousValue) )
        {
        return KErrGeneral;
        }

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::TestGetNextAvailableOffsetL
// This function tests for next available offset used for folder LUID 
// -----------------------------------------------------------------------------
//

TInt CCalendarMultiCalUtilsTest::TestGetNextAvailableOffsetL(
   CStifItemParser& /*aItem*/ )
    {
    // Print to UI
    _LIT( KCalendarMultiCalUtilsTest, "CalendarMultiCalUtilsTest" );
    _LIT( KPrintText, "In TestGetNextAvailableOffsetL" );
    TestModuleIf().Printf(0, KCalendarMultiCalUtilsTest, KPrintText);
    // Print to log file
    iLog->Log( KPrintText );

    TInt errorCode = KErrNone;
    TUint previousOffsetvalue = 0;
    
    for(TInt iter = 0 ; iter < 10 ; iter ++)
        {
        TUint nextOffsetValue = 0; 
        TRAPD(err,nextOffsetValue = CCalenMultiCalUtil::GetNextAvailableOffsetL());
        
        if(err != KErrNone)
            {
            errorCode = KErrGeneral;
            iLog->Log( _L("No key in cenRep") );
            }
        
        if(nextOffsetValue == 0 )
            {
            errorCode = KErrGeneral;
            iLog->Log( _L("equal to zero") );
            }

        //Value from api should be divisible by offsetValue(100000) with reminder 0
        //Indicates api always returns value multiple of 100000
        TInt offsetValue = 100000; 
        TInt rem = nextOffsetValue % offsetValue;
        if( rem )
            {
            errorCode = KErrGeneral;
            iLog->Log( _L("Not a multiple of 100000") );
            }
        
        if(offsetValue == 0xffffffff)
            {
            errorCode = KErrGeneral;
            iLog->Log( _L("Value is -ve") );
            }
        
        if(previousOffsetvalue == nextOffsetValue)
            {
            errorCode = KErrGeneral;
            iLog->Log( _L("value == previous value") );
            }
        if(iter > 0)
            {
            previousOffsetvalue = nextOffsetValue;
            }
        }
    
    return errorCode;
    }

TInt CCalendarMultiCalUtilsTest::TestCalendarPropertiesL( CStifItemParser& aItem)
{
    TInt retValue = KErrNone;
    //Initialization
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);
    calendarInfo->SetNameL(KNullDesC16);
    calendarInfo->SetColor(255);
    calendarInfo->SetEnabled(ETrue);

    //Execution
    ParseInputForCalendarPropertiesL( aItem );

    TBuf8<KBuffLength> keyBuff;
    keyBuff.AppendNum(EFolderLUID);
    TPckgC<TUint> calValuePckg(iFolderLUID);
    calendarInfo->SetPropertyL(keyBuff, calValuePckg);

    keyBuff.Zero();
    keyBuff.AppendNum(ECreationTime);
    TPckgC<TTime> pkgCreationTime(iCreationTime);
    calendarInfo->SetPropertyL(keyBuff, pkgCreationTime);

    keyBuff.Zero();
    keyBuff.AppendNum(EModificationTime);
    TPckgC<TTime> pkgModificationTime(iModificationTime);
    calendarInfo->SetPropertyL(keyBuff, pkgModificationTime);

    keyBuff.Zero();
    keyBuff.AppendNum(ESyncStatus);
    TPckgC<TBool> pkgSyncStatus(iSyncStatus);
    calendarInfo->SetPropertyL(keyBuff, pkgSyncStatus);

    keyBuff.Zero();
    keyBuff.AppendNum(EIsSharedFolder);
    TPckgC<TBool> pkgIsSharedFolder(iIsSharedFolder);
    calendarInfo->SetPropertyL(keyBuff, pkgIsSharedFolder);
    
    keyBuff.Zero();
    keyBuff.AppendNum(EGlobalUUID);
    TPtr8 guuidPtr = iGlobalUUID->Des();
    calendarInfo->SetPropertyL(keyBuff, guuidPtr);
    
    keyBuff.Zero();
    keyBuff.AppendNum(EDeviceSyncServiceOwner);
    TPckgC<TUint> pkgdeviceSyncOwner(iDeviceSyncServiceOwner);
    calendarInfo->SetPropertyL(keyBuff, pkgdeviceSyncOwner);

    keyBuff.Zero();
    keyBuff.AppendNum(EOwnerName);
    TPtr8 ptr = iOwnerName->Des();
    calendarInfo->SetPropertyL(keyBuff, ptr);
    
 
    //Validation
    TAny* valuePtr = NULL;
    TInt err = GetPropertyL(calendarInfo, EFolderLUID, &valuePtr );
    if(!( KErrNone == err && *static_cast<TUint*>(valuePtr) == iFolderLUID ))
        {
        retValue = KErrNotFound;
        }
    delete valuePtr;
    valuePtr = NULL;
    err = GetPropertyL(calendarInfo, ECreationTime, &valuePtr );
    if(!( KErrNone == err && *static_cast<TTime*>(valuePtr) == iCreationTime ))
        {
        retValue = KErrNotFound;
        }
    delete valuePtr;
    valuePtr = NULL;
    err = GetPropertyL(calendarInfo, EModificationTime, &valuePtr );
    if(!( KErrNone == err && *static_cast<TTime*>(valuePtr) == iModificationTime ))
        {
        retValue = KErrNotFound;
        }
    delete valuePtr;
    valuePtr = NULL;
    err = GetPropertyL(calendarInfo, ESyncStatus, &valuePtr );
    if(!( KErrNone == err && *static_cast<TBool*>(valuePtr) == iSyncStatus ))
        {
        retValue = KErrNotFound;
        }
    delete valuePtr;
    valuePtr = NULL;
    err = GetPropertyL(calendarInfo, EIsSharedFolder, &valuePtr );
    if(!( KErrNone == err && *static_cast<TBool*>(valuePtr) == iIsSharedFolder ))
        {
        retValue = KErrNotFound;
        }
    delete valuePtr;
    valuePtr = NULL;
    err = GetPropertyL(calendarInfo, EGlobalUUID, &valuePtr );
    if(!( KErrNone == err && !static_cast<HBufC8*>(valuePtr)->CompareF(*iGlobalUUID)))
        {
        retValue = KErrNotFound;
        }
    delete valuePtr;
    valuePtr = NULL;
    err = GetPropertyL(calendarInfo, EDeviceSyncServiceOwner, &valuePtr );
    if(!( KErrNone == err && *static_cast<TUint*>(valuePtr) == iDeviceSyncServiceOwner ))
        {
        retValue = KErrNotFound;
        }
    delete valuePtr;

    valuePtr = NULL;
    err = GetPropertyL(calendarInfo, EOwnerName, &valuePtr );
    if(!( KErrNone == err && !static_cast<HBufC8*>(valuePtr)->CompareF(*iOwnerName)))
        {
        retValue = KErrNotFound;
        }
    delete valuePtr;
    valuePtr = NULL;
    CleanupStack::PopAndDestroy(calendarInfo);
    
    return retValue;
}

TInt CCalendarMultiCalUtilsTest::GetPropertyL(CCalCalendarInfo* aCalendarInfo,TInt aPropertyKey, TAny** aOutputParam )
    {
    TBuf8<128> key;
    key.AppendNum(aPropertyKey);
    
    TInt retVal = KErrNone;

    switch(aPropertyKey)
        {
        case 1:
        case 7:
            {
            TUint value;
            TPckgC<TUint> _value(value);
            TRAPD(err,_value.Set(aCalendarInfo->PropertyValueL(key)));
            TUint* valuePtr = new(ELeave) TUint;
            if (err == KErrNone)
                {
                *valuePtr = _value();;
                *aOutputParam = valuePtr; 
                }
            else
                {
                retVal = err;
                }
            break;
            }
        case 2:
        case 3:
            {
            TTime time;
            TPckgC<TTime> _value(time);
            TRAPD(err,_value.Set(aCalendarInfo->PropertyValueL(key)));
            TTime* valuePtr = new(ELeave) TTime;
            if (err == KErrNone)
                {
                *valuePtr = _value();
                *aOutputParam = valuePtr;
                }
            else
                {
                retVal = err;
                }
            break;
            }
        case 4:
        case 5:
            {
            TBool value;
            TPckgC<TBool> _value(value);
            TRAPD(err,_value.Set(aCalendarInfo->PropertyValueL(key)));
            value = _value();
            TBool* valuePtr = new(ELeave) TBool;
            if (err == KErrNone)
                {
                *valuePtr = _value();
                *aOutputParam = valuePtr;
                }
            else
                {
                retVal = err;
                }
            break;
            }
        case 6:
        case 8:
            {
            HBufC8* name = HBufC8::NewL(25);
            TRAPD(err,*name = aCalendarInfo->PropertyValueL(key));
            if (err == KErrNone)
                {
                *aOutputParam = name;
                }
            else
                {
                retVal = err;
                }

            break;
            }
        default:
            {
            break;
            }
        }
    return retVal;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
