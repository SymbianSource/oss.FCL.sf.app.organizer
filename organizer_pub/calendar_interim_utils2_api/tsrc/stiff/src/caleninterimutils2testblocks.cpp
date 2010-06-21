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

#include "caleninterimutils2test.h"
#include "CalProgressCallBackListener.h"
#include "calenglobaldata.h"


//  LOCAL CONSTANTS AND MACROS
const TInt KDefaultStartTime( 8 );

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::Delete() 
    {
    // Nothing.
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCalenInterimUtils2Test::RunMethodL( 
        CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        // Example: ENTRY( "Example", CCalenInterimUtils2Test::ExampleL ),

        /** Test functions for API CalenDateUtils */
        ENTRY( "TestGlobalUUID", 
                CCalenInterimUtils2Test::TestGlobalUUIDL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::TestGlobalUUIDL
// Test funtion to test the API CCalenInterimUtils2::GlobalUidL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenInterimUtils2Test::TestGlobalUUIDL( CStifItemParser& aItem )
    {
    CDesC8ArrayFlat* uidArray = new ( ELeave ) CDesC8ArrayFlat( KTestMax );
    TInt retValue = KErrNone;
    // Print to UI
    _LIT( KCalenInterimUtils2Test, "CalenInterimUtils2Test" );
    _LIT( KDuplicateUids, "Uids are duplicate" );
    _LIT( KTestGlobalUUID, "In TestGlobalUUIDL" );
    _LIT( KTestGlobalUUIDs, "TestGlobalUUIDs are " );
    
    TestModuleIf().Printf( 0, KCalenInterimUtils2Test, KTestGlobalUUID );
    // Print to log file
    iLog->Log( KTestGlobalUUID );
    iLog->Log( KTestGlobalUUIDs );
        // Construct the calendar global data.
    CCalProgressCallBackListener *callBack = NULL;
    callBack = CCalProgressCallBackListener::NewL();
    CCalenGlobalData* globalData = CCalenGlobalData::NewL( *callBack );
    // Make the call to the API.
    // Create unique ID.   
    TBuf8<100> outputBuffer;
    TInt i = 0;
    
    for( ; i<2000; i++ )
        {
        HBufC8* guid = globalData->InterimUtilsL().GlobalUidL();
        CleanupStack::PushL(guid);
        iLog->Log( *guid );       
        TRAPD( err, uidArray->InsertIsqL( *guid ) );
        
        if ( err == KErrAlreadyExists )
            {
            outputBuffer.Zero();
            outputBuffer.Append( _L8( "\n err == KErrAlreadyExists\n" ) );
            iLog->Log( outputBuffer );
            RDebug::Print(_L("failed %d"),i);
            retValue = KErrNotFound;            
            }

        CleanupStack::PopAndDestroy( guid );
        }
   
    globalData->Release();
    delete uidArray; //cleanup uid array
    // Validate the result.

    return retValue;

    }



// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
