/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Parser of the data received from configuration file
*
*/


#ifndef TEST_SUITE_INPUT_DATA_H
#define TEST_SUITE_INPUT_DATA_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <StifParser.h>
#include <msvids.h>

// FORWARD DECLARATIONS
class CMRMailboxUtils;

/**
 *  This class performs parsing of the data input from the configuration file
 *
 *  @since S60 v3.2
 */
class CTestSuiteInputData :public CBase
{
	public:  // Constructors and destructor
					struct TMailboxInfo
        					{
        					HBufC* iName;
        					HBufC* iEmailAddress;
        					TUid  iMtmUid;        
        					TMsvId iEntryId;
        					};
        /**
        * Two-phased constructor.
        */
        static CTestSuiteInputData* NewL(CStifItemParser& aItem);

        /**
        * Destructor.
        */
        virtual ~CTestSuiteInputData();	
   
	public: // Functions
	
				/*
				* Parses the input data while creating new calendars
				* 
				* @param aItem - The input test case name and parmeters
				*/
			  void ParseInputL(CStifItemParser& aItem);
		
			 /*
        * Parses the input data  when verification is expected
        * 
        * @param aItem - The input test case name and parmeters
        */
			 
			 
			 TMailboxInfo& GetMRParam();
			 
			 TDesC& GetMailboxName();
			 
			 TDesC& GetMailboxAddress();
			 
			 TUid GetMtmUid();
			 
			 TInt GetEntryId();
			 
			 
		
			 
			 
		
	private: // functions
	
			 /**
			 * Default Constructor
			 */
			 CTestSuiteInputData();

			 /**
			 * Second phase constructor
			 */
			void ConstructL(CStifItemParser& aItem);
			
	private:
    
    TMailboxInfo iMailBoxInfo;
      
      
};

#endif // TEST_SUITE_INPUT_DATA_H

// End of file
