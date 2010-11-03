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
* Description: STIF testclass CCalendarMultiCalUtilsTest declaration.
*
*/

#ifndef CALENDARMULTICALUTILSTEST_H
#define CALENDARMULTICALUTILSTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>


// For CalenMultiCalUtils.
#include <calenmulticalutil.h>

class CCalCalendarInfo;
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define MULTICALUTILS_TEST_CLASS_VERSION_MAJOR 1
#define MULTICALUTILS_TEST_CLASS_VERSION_MINOR 0
#define MULTICALUTILS_TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KCalendarMultiCalUtilsTestLogPath, 
        "\\logs\\testframework\\CalendarMultiCalUtilsTest\\" ); 
// Log file
_LIT( KCalendarMultiCalUtilsTestLogFile, 
        "CalendarMultiCalUtilsTest.txt" ); 
_LIT( KCalendarMultiCalUtilsTestLogFileWithTitle, 
        "CalendarMultiCalUtilsTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CCalendarMultiCalUtilsTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib CalendarCommonUtilsTest.lib
*  @since ?Series60_version
*/
NONSHARABLE_CLASS( CCalendarMultiCalUtilsTest ) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCalendarMultiCalUtilsTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CCalendarMultiCalUtilsTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CCalendarMultiCalUtilsTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Method used to log version of test class
        */
        void SendTestClassVersion();
        
        /**
        * Test methods are listed below. 
        */
        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        
        /**
        * Test funtion to test the API CalendarMultiCalUtils::GetNextAvailableCalFileL	
        * @param aItem Script line containing parameters.
        */
        TInt TestGetNextAvailableCalFileL( CStifItemParser& aItem );
        
        /*
         * Test function to test the API CalendarMultiCalUtils::GetNextAvailableOffsetL
         * @param aItem Script line containing parameters.
         * @return Symbian OS error code.
         */
        TInt TestGetNextAvailableOffsetL( CStifItemParser& aItem);
        
        TInt TestCalendarPropertiesL( CStifItemParser& aItem);
        
        TInt GetPropertyL(CCalCalendarInfo* aCalendarInfo,TInt aPropertyKey, TAny** aOutputParam );
        
    private:    // New functions
        
        /**
        * The parser utility, used to Parse input from configuration file.
        * @param aItem Script line containing parameters.
        */
        TBool ParseInputL(CStifItemParser& aItem, TDateTime& aTime);
        
       
        /**
        * The parser utility, used to Parse input from configuration file.
        * @param aItem Script line containing parameters.
        */
        TBool ParseInputWithMultipleTTimesL( 
                CStifItemParser& aItem, 
                RArray< TDateTime >& aTime );
        
        TBool ParseInputForCalendarPropertiesL(CStifItemParser& aItem);
        
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        TUint iFolderLUID;
        TUint iDeviceSyncServiceOwner;
        TBool iSyncStatus;
        TBool iIsSharedFolder;
        HBufC8* iGlobalUUID;
        HBufC8* iOwnerName;
        TTime iCreationTime;
        TTime iModificationTime;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // CALENDARMULTICALUTILSTEST_H

// End of File
