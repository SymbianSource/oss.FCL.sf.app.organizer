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



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "calenmrutilsapitest.h"
#include <cmrmailboxinfo.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ccalenmultipledbapitest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ccalenmrutilsapitest::Delete() 
    {
    delete iMailBoxInfo;
    }

// -----------------------------------------------------------------------------
// Ccalenmrutilsapitest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ccalenmrutilsapitest::RunMethodL( CStifItemParser& aItem ) 
    {
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CreateMRUtilsInfo", Ccalenmrutilsapitest::CreateMRUtilsInfo),
        ENTRY( "CheckEntryId", Ccalenmrutilsapitest::CheckEntryId),
        ENTRY( "CheckMtmUid", Ccalenmrutilsapitest::CheckMtmUid),
        ENTRY( "CheckMailboxName", Ccalenmrutilsapitest::CheckMailboxName),
        ENTRY( "CheckMailboxAddress", Ccalenmrutilsapitest::CheckMailboxAddress)
        
        
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );
		return RunInternalL( KFunctions, count, aItem );
	}

// -----------------------------------------------------------------------------
// This test case gets the day and verifies whether it lies in the specific range
// -----------------------------------------------------------------------------
TInt Ccalenmrutilsapitest::CreateMRUtilsInfo( CStifItemParser& aItem )
    {
	iReturnStatus = KErrNone;
		
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem);
			
	iInputParsedData->ParseInputL(aItem) ;
	
	CTestSuiteInputData::TMailboxInfo temp = iInputParsedData->GetMRParam();
		
	TRAPD(error,iMailBoxInfo = CMRMailBoxInfo::NewL(*(temp.iName),*(temp.iEmailAddress),temp.iMtmUid,temp.iEntryId));
	if(error != KErrNone)
	    {
	    iReturnStatus = error;
	    }
    		
	return iReturnStatus;
    }

// -----------------------------------------------------------------------------
// This test case gets the day and verifies whether it lies in the specific range
// -----------------------------------------------------------------------------
TInt Ccalenmrutilsapitest::CheckEntryId( CStifItemParser& aItem )
    {
    iReturnStatus = KErrNone;
        
    // Create the input parser
    iInputParsedData = CTestSuiteInputData::NewL(aItem);
            
    iInputParsedData->ParseInputL(aItem) ;
    
    CTestSuiteInputData::TMailboxInfo temp = iInputParsedData->GetMRParam();
        
    iMailBoxInfo = CMRMailBoxInfo::NewL(*(temp.iName),*(temp.iEmailAddress),temp.iMtmUid,temp.iEntryId);
    
    TInt result = iInputParsedData->GetEntryId();
    
    if(result != iMailBoxInfo->EntryId())
        {
        iReturnStatus = KErrGeneral;
        }
    
    return iReturnStatus;
    }

// -----------------------------------------------------------------------------
// This test case gets the day and verifies whether it lies in the specific range
// -----------------------------------------------------------------------------
TInt Ccalenmrutilsapitest::CheckMtmUid( CStifItemParser& aItem )
    {
    iReturnStatus = KErrNone;
        
    // Create the input parser
    iInputParsedData = CTestSuiteInputData::NewL(aItem);
            
    iInputParsedData->ParseInputL(aItem) ;
    
    CTestSuiteInputData::TMailboxInfo temp = iInputParsedData->GetMRParam();
        
    iMailBoxInfo = CMRMailBoxInfo::NewL(*(temp.iName),*(temp.iEmailAddress),temp.iMtmUid,temp.iEntryId);
    
    TUid result = iInputParsedData->GetMtmUid();
    
    if(result != iMailBoxInfo->MtmUid())
        {
        iReturnStatus = KErrGeneral;
        }
    
    return iReturnStatus;
    }

// -----------------------------------------------------------------------------
// This test case gets the day and verifies whether it lies in the specific range
// -----------------------------------------------------------------------------
TInt Ccalenmrutilsapitest::CheckMailboxName( CStifItemParser& aItem )
    {
    iReturnStatus = KErrNone;
        
    // Create the input parser
    iInputParsedData = CTestSuiteInputData::NewL(aItem);
            
    iInputParsedData->ParseInputL(aItem) ;
    
    CTestSuiteInputData::TMailboxInfo temp = iInputParsedData->GetMRParam();
        
    iMailBoxInfo = CMRMailBoxInfo::NewL(*(temp.iName),*(temp.iEmailAddress),temp.iMtmUid,temp.iEntryId);
    
    TDesC& result = iInputParsedData->GetMailboxName();
    
    if(result != iMailBoxInfo->Name())
        {
        iReturnStatus = KErrGeneral;
        }
    
    return iReturnStatus;
    }

// -----------------------------------------------------------------------------
// This test case gets the day and verifies whether it lies in the specific range
// -----------------------------------------------------------------------------
TInt Ccalenmrutilsapitest::CheckMailboxAddress( CStifItemParser& aItem )
    {
    iReturnStatus = KErrNone;
        
    // Create the input parser
    iInputParsedData = CTestSuiteInputData::NewL(aItem);
            
    iInputParsedData->ParseInputL(aItem) ;
    
    CTestSuiteInputData::TMailboxInfo temp = iInputParsedData->GetMRParam();
        
    iMailBoxInfo = CMRMailBoxInfo::NewL(*(temp.iName),*(temp.iEmailAddress),temp.iMtmUid,temp.iEntryId);
    
    TDesC& result = iInputParsedData->GetMailboxAddress();
    
    if(result != iMailBoxInfo->Address())
        {
        iReturnStatus = KErrGeneral;
        }
    
    return iReturnStatus;
    }
//  [End of File] - Do not remove
