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
* Description:     This class is used to adapt the data for display on CSrchUiResultContainer. 
*
*/






// System Includes
#include <srchuiresultview.rsg>
#include <searchdocumentid.h>
#include <searchlightresult.h>
#include <searchresult.h>
#include <utf.h>
#include <searchcommon.hrh>

// User Includes
#include "srchuiresfolder.h"
#include "srchuiresultmodel.h"
#include "srchuiresultviewdefines.h"


const TInt KSnippetLength = 50;
const TInt KListItemLength = 20;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSrchUiResultModel::NewL
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CSrchUiResultModel* CSrchUiResultModel::NewL()
	{
    CSrchUiResultModel* self = CSrchUiResultModel::NewLC();
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CSrchUiResultModel::NewLC
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CSrchUiResultModel* CSrchUiResultModel::NewLC()
	{
    CSrchUiResultModel* self = new ( ELeave ) CSrchUiResultModel();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSrchUiResultModel::~CSrchUiResultModel
// Destructor
// -----------------------------------------------------------------------------
//
CSrchUiResultModel::~CSrchUiResultModel()
	{
	delete iBuffer;
	}

// -----------------------------------------------------------------------------
// CSrchUiResultModel::MdcaCount
// Returns the number of descriptor elements in the array.
// -----------------------------------------------------------------------------
//
TInt CSrchUiResultModel::MdcaCount() const
	{
	return iOpenedFolder->GetHeavyResultsCount();
	}

// -----------------------------------------------------------------------------
// CSrchUiResultModel::MdcaPoint
// Returns a non-modifiable pointer descriptor representing the descriptor
//  element located at position aIndex within the array
// -----------------------------------------------------------------------------
//
TPtrC16 CSrchUiResultModel::MdcaPoint( TInt aIndex ) const
{
    TRAP_IGNORE(MdcaPointL(aIndex ));
    
	return(*iBuffer);
	
}
// -----------------------------------------------------------------------------
// CSrchUiResultModel::MdcaPointL
// Returns a non-modifiable pointer descriptor representing the descriptor
//  element located at position aIndex within the array
// -----------------------------------------------------------------------------
//
TPtrC16 CSrchUiResultModel::MdcaPointL( TInt aIndex ) const
	{
        
        CSearchResult* searchResult;
    
        // Files are listed below folders (if any folders), and the real index
        //  must be calculated
        searchResult = iOpenedFolder->GetHeavyResult(aIndex);
        TInt32 docId = searchResult->Result().SubContentId();
    
        // Fetch the icon index, knowing the file pointed to

        TInt iconIdx = aIndex;
    	HBufC* title = NULL;
        if ( 0 < searchResult->Title().Size() ) 
        	{
      		   title = searchResult->Title().AllocLC();	
   	        }	
        else 
        	{
        	_LIT ( KBuf, " " );
        	TBuf<2> buf ( KBuf );
        	title = buf.AllocLC();            
        	}
        
        TPtr titlePtr(title->Des());
    	
        for(TInt i = 0; i< titlePtr.Length(); i++)
    	{
    		if( titlePtr[i] == 0x2029)
    		{
    		   titlePtr[i] = ' ';	
    		}   		      		
    		
    	}
        
    	HBufC* snippet = NULL;
    	TRAP_IGNORE(snippet = HBufC::NewL( KSnippetLength ));
    	TPtr ptr(snippet->Des());
    	CnvUtfConverter::ConvertToUnicodeFromUtf8(ptr,searchResult->Snippet());
    	
    	for(TInt i = 0; i< ptr.Length(); i++)
    	{
    		if( ptr[i] == 0x2029)
    		{
    		   ptr[i] = ' ';	
    		}    		     		
    		  
    		
    	}
    	
    	TInt titleLen = Min( KMaxCharsOnUi, ( *title ).Length() );
        TInt snippetLen = Min( KMaxCharsOnUi, ( *snippet ).Length() );
        
        TPtrC titlePart = ( *title ).Mid( 0, titleLen );
        TPtrC snippetPart = ( *snippet ).Mid( 0, snippetLen );
        
		TBuf<KListItemLength> listBoxStringFormat;
		
        if ( KSearchCSubContentMMC == docId )
        	{
        	_LIT( KFormatWithDColumn, "%d\t%S\t%S\t\t%d" );	
        	listBoxStringFormat.Copy( KFormatWithDColumn );
        	// Fill in buffer
			iBuffer->Des().Format( listBoxStringFormat, iconIdx, &titlePart,
		               &snippetPart, iOpenedFolder->GetHeavyResultsCount() );
        	}
        else 
        	{
        	_LIT( KFormatWithoutDColumn, "%d\t%S\t%S\t" );
        	listBoxStringFormat.Copy( KFormatWithoutDColumn );
        	// Fill in buffer
			iBuffer->Des().Format( listBoxStringFormat, iconIdx, &titlePart,
		                                     &snippetPart );
        	}       	
        CleanupStack::PopAndDestroy(1);
        delete snippet;
        //CleanupStack::PopAndDestroy( title);
        
	return *iBuffer;
	
	}

// -----------------------------------------------------------------------------
// CSrchUiResultModel::SetOpenedFolder
// Setter for iOpenedFolder.
// -----------------------------------------------------------------------------
//
void CSrchUiResultModel::SetOpenedFolder(
                                        CSrchUiResFolder* aOpenedFolder )
    {
    iOpenedFolder = aOpenedFolder;
    }

// -----------------------------------------------------------------------------
// CSrchUiResultModel::CSrchUiResultModel
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//
CSrchUiResultModel::CSrchUiResultModel()
:   iSrchEikonEnv(CEikonEnv::Static())
	{
	}

// -----------------------------------------------------------------------------
// CSrchUiResultModel::ConstructL
// Performs the second phase construction.
// -----------------------------------------------------------------------------
//
void CSrchUiResultModel::ConstructL()
	{

    iBuffer = HBufC::NewL( KMaxCharsOnUi * 2
		         + KSearchTab().Length() * 3 + 10 );	// 10 for icon indexes

    }

// End of File
