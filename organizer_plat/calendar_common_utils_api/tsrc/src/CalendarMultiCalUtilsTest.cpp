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
#include <SettingServerClient.h>

#include "CalendarMultiCalUtilsTest.h"

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
// CCalendarMultiCalUtilsTest::CCalendarMultiCalUtilsTest
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCalendarMultiCalUtilsTest::CCalendarMultiCalUtilsTest( CTestModuleIf& aTestModuleIf ): 
CScriptBase( aTestModuleIf )
    {
	
    }

// -----------------------------------------------------------------------------
// CCalendarMultiCalUtilsTest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCalendarMultiCalUtilsTest::ConstructL()
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
        logFileName.Format(KCalendarMultiCalUtilsTestLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KCalendarMultiCalUtilsTestLogFile);
        }

    iLog = CStifLogger::NewL( KCalendarMultiCalUtilsTestLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    
    }

// -----------------------------------------------------------------------------
// CCalendarMultiCalUtilsTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCalendarMultiCalUtilsTest* CCalendarMultiCalUtilsTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CCalendarMultiCalUtilsTest* self = new (ELeave) CCalendarMultiCalUtilsTest( 
            aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CCalendarMultiCalUtilsTest::~CCalendarMultiCalUtilsTest()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

//-----------------------------------------------------------------------------
// CCalendarMultiCalUtilsTest::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CCalendarMultiCalUtilsTest::SendTestClassVersion()
	{
	
	TVersion moduleVersion;
	moduleVersion.iMajor = MULTICALUTILS_TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = MULTICALUTILS_TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = MULTICALUTILS_TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("CalendarCommonUtilsTest.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, 
	        moduleName, 
	        newVersionOfMethod);
	
	}

// End of File
