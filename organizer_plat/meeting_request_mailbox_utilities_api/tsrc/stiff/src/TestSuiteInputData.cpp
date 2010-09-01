/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


// INCLUDE FILES
#include <StifParser.h>
#include "TestSuiteInputData.h"


_LIT(KName, "Name");
_LIT(KEmailAddress, "EmailAddress");
_LIT(KMtmUid, "MtmUid");
_LIT(KEntryId, "EntryId");


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// Two Phase constructor
// ----------------------------------------------------------------------------
CTestSuiteInputData* CTestSuiteInputData::NewL(CStifItemParser& aItem)
{
	CTestSuiteInputData* self = new (ELeave) CTestSuiteInputData();	
    CleanupStack::PushL(self);
    self->ConstructL(aItem);
    CleanupStack::Pop();
    return self;
}

// ----------------------------------------------------------------------------
// Default constructor
// ----------------------------------------------------------------------------
CTestSuiteInputData::CTestSuiteInputData()
{
}

// ----------------------------------------------------------------------------
// Second phase construction
// ----------------------------------------------------------------------------
void CTestSuiteInputData::ConstructL(CStifItemParser& /*aItem*/)
{
   
}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CTestSuiteInputData::~CTestSuiteInputData()
{    	
  delete iMailBoxInfo.iName;
  delete iMailBoxInfo.iEmailAddress;
}

// -----------------------------------------------------------------------------
// This function parses the input parameters with result
// -----------------------------------------------------------------------------
void CTestSuiteInputData:: ParseInputL(CStifItemParser& aItem)
    {
    TPtrC string;

    // parsing type to be set before parsing a string 
    // EQuoteStyleParsing: information between quotes is handled as a one string. 
    aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
    aItem.GetNextString ( string );
    
    TLex data(string);
    // First word
    TPtrC token = data.NextToken();

    TChar colon = ':';
    // loop till there is a token in i/p string
    while ( token.Length() != 0  )
        {
        TInt colonStartPos = token.Locate(colon);
        TPtrC16 itemName = token.Left(colonStartPos);  
        TPtrC16 itemValue = token.Mid(colonStartPos + 1);
        if(itemName.Compare(KName) == 0)
            {
            iMailBoxInfo.iName = itemValue.Alloc();
            }
        else if(itemName.Compare(KEmailAddress) == 0)
            {
            iMailBoxInfo.iEmailAddress = itemValue.Alloc();
            }
        else if(itemName.Compare(KMtmUid) == 0)
            {
            TLex num(itemValue);
            TInt temp;
            num.Val(temp);
            TUid mtmuid = TUid::Uid(temp);
            
            iMailBoxInfo.iMtmUid = mtmuid;
            }
        else if(itemName.Compare(KEntryId) == 0)
            {
            TLex num(itemValue);
            TInt32 entryid;
            num.Val(entryid);
            iMailBoxInfo.iEntryId = entryid;
            }        
        else
            {
            // Wrong Input..
            User::Leave(KErrArgument);
            }
        // Next word
        token.Set(data.NextToken());

        }
    }

// -----------------------------------------------------------------------------
// returns TMailboxInfo
// -----------------------------------------------------------------------------
CTestSuiteInputData::TMailboxInfo& CTestSuiteInputData::GetMRParam()
    {
    return iMailBoxInfo;
    }

// -----------------------------------------------------------------------------
// This function parses the input parameters with result
// -----------------------------------------------------------------------------
TDesC& CTestSuiteInputData:: GetMailboxName()
    {
    return *(iMailBoxInfo.iName);
    }

// -----------------------------------------------------------------------------
// This function parses the input parameters with result
// -----------------------------------------------------------------------------
TDesC& CTestSuiteInputData:: GetMailboxAddress()
    {
    return *(iMailBoxInfo.iEmailAddress);
    }

// -----------------------------------------------------------------------------
// This function parses the input parameters with result
// -----------------------------------------------------------------------------
TUid CTestSuiteInputData:: GetMtmUid()
    {
    return iMailBoxInfo.iMtmUid;
    }

// -----------------------------------------------------------------------------
// This function parses the input parameters with result
// -----------------------------------------------------------------------------
TInt CTestSuiteInputData:: GetEntryId()
    {
    return iMailBoxInfo.iEntryId;
    }

