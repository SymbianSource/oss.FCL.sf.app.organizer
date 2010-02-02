/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



#ifndef CALENMRUTILSAPITEST_H
#define CALENMRUTILSAPITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <msvids.h>
#include "TestSuiteInputData.h"


// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KcalenmrutilsapitestLogPath, "//logs//testframework//calencustomapitest//" ); 
// Log file
_LIT( KcalenmrutilsapitestLogFile, "calencustomapitest.txt" ); 
_LIT( KcalenmrutilsapitestLogFileWithTitle, "calencustomapitest_[%S].txt" );



// CLASS DECLARATION
class CMRMailBoxInfo;


/**
*  Ccalenmultipledbapitest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Ccalenmrutilsapitest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Ccalenmrutilsapitest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Ccalenmrutilsapitest();

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
        Ccalenmrutilsapitest( CTestModuleIf& aTestModuleIf );

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
			  * This test case checks whether the day is valid
			  *
			  * @param aItem Script line containing parameters.
			  */
        
        TInt CreateMRUtilsInfo(CStifItemParser& aItem);
        
        TInt CheckMailboxName(CStifItemParser& aItem);
        
        TInt CheckMailboxAddress(CStifItemParser& aItem);
        
        TInt CheckEntryId(CStifItemParser& aItem);
        
        TInt CheckMtmUid(CStifItemParser& aItem);
        
    private:    // Data
        
    	 /**
        * The input data parser
        * own
        */ 
       CTestSuiteInputData* iInputParsedData;
        
     
       
         
         /**
	      * The error return status
	      */
	     TInt iReturnStatus;
	     
	     CMRMailBoxInfo* iMailBoxInfo;

	    
    };

#endif      // CALENCUSTOMAPITEST_H

// End of File
