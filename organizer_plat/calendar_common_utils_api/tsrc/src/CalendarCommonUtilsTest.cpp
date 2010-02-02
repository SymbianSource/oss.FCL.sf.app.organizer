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

// INCLUDE FILES
#include <Stiftestinterface.h>
#include "CalendarCommonUtilsTest.h"
#include <SettingServerClient.h>

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
// CCalendarCommonUtilsTest::CCalendarCommonUtilsTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCalendarCommonUtilsTest::CCalendarCommonUtilsTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCalendarCommonUtilsTest::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;
    
    if(loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KCalendarCommonUtilsTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KCalendarCommonUtilsTestLogFile);
        }

    iLog = CStifLogger::NewL( KCalendarCommonUtilsTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCalendarCommonUtilsTest* CCalendarCommonUtilsTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CCalendarCommonUtilsTest* self = new (ELeave) CCalendarCommonUtilsTest( 
            aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CCalendarCommonUtilsTest::~CCalendarCommonUtilsTest()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

//-----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CCalendarCommonUtilsTest::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = COMMONUTILS_TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = COMMONUTILS_TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = COMMONUTILS_TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("CalendarCommonUtilsTest.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, 
	        moduleName, 
	        newVersionOfMethod);
	}

//  End of File
