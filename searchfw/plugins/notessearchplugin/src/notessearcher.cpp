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
* Description:   Plugin for note information search. 
*
*/




//System includes
#include <e32debug.h>
#include <e32const.h>
#include <e32base.h> 
#include <avkon.rsg>
#include <barsc.h> 
#include <utf.h>
#include <barsread.h> 
#include <bautils.h>
#include <e32std.h>

//Search FW Common includes  
#include <searchtextsearcher.h>
#include <searchresult.h>
#include <searchlightresult.h>
#include <searchcommon.h>
#include <searchdocumentid.h>
#include <searchtextsnippet.h>
#include <searchsnippetcreator.h>
#include <searchpluginobserver.h>
#include <data_caging_path_literals.hrh>

//User includes
#include "notessearchpluginpanic.h"
#include "notessearcher.h"

//Constants
const TUint     KNoteIdLengthInHex( 8 );
const TUid      KSecureUid = { 0x101F8878 };
const TInt      KBufferLength( 60 );
const TInt      KBigBufferLength( 120 );

_LIT(KSecure,"SECURE" );
_LIT(KNotepadFilenameAndPath,"c:Notepad.dat");
_LIT(KNotepadSqlSelectCmd,"SELECT * FROM Table1");
_LIT(KNotepadSqlFormatSeek,"SELECT * FROM Table1 WHERE key=%d");
_LIT(KAvkonResFilePath,"avkon.Rsc" );
_LIT8(KMimeType,"MIME/TEXT");




// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CNotesSearcher::CNotesSearcher(const TUid&  aPluginId) 
:   CActive(CActive::EPriorityStandard),
    iNotesPluginId(aPluginId),
    iSDbmsUp(EFalse)
    {
      CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// 1st Phase constructor
// -----------------------------------------------------------------------------
//
CNotesSearcher*  CNotesSearcher::NewL( const RArray<TUid>& aContentIdArray,
                                       const CSearchCondition& aCondition,
                                       const TUid& aPluginId,
                                       MSearchPluginObserver& aObserver )
    {
    CNotesSearcher* self = new( ELeave ) CNotesSearcher( aPluginId );
    CleanupStack::PushL(self);
    ( *self ).ConstructL(aContentIdArray,aCondition,aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Called to handle error when RunL leaves
// -----------------------------------------------------------------------------
// 
TInt CNotesSearcher::RunError( TInt /*aError*/ ) 
{
	return KErrNone;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//  
CNotesSearcher::~CNotesSearcher()
    {
	

	if ( iTextSearcher )
	{
		delete iTextSearcher;
		iTextSearcher = NULL;	
    	}
	
	if(iDateFormat)
	    {
		
		 delete iDateFormat;
		 iDateFormat = NULL;
     	}
	//if(0 < iHeavyResultsArray.Count())
	//{
	iHeavyResultsArray.ResetAndDestroy();
	/*iSearchLightResult.Reset();
		
	}
	else
	{
	iSearchLightResult.ResetAndDestroy();
	} */
	

	
	//Close the view
    iDbView.Close();
    
    //Close the database 
    iNamedDataBase.Close();
    
    //Close the session
    iDbSession.Close();
    
       	
	}

// -----------------------------------------------------------------------------
// CNotesSearcher::CheckIfEntryExists
// Check if the entry exists in notes
// -----------------------------------------------------------------------------

TBool CNotesSearcher::CheckIfEntryExistsL (TInt aNoteKey)
    {
    
    TBuf<KBufferLength> sqlStatement;
    sqlStatement.Format(KNotepadSqlFormatSeek, aNoteKey);
    
    RDbView dbView;
    CleanupClosePushL(dbView);
    dbView.Prepare( iNamedDataBase , TDbQuery( sqlStatement ) , RDbView::EReadOnly  );
    dbView.EvaluateAll();
        
    TInt isAtRow(dbView.FirstL());
    CleanupStack::PopAndDestroy(); // dbView
    
    if(isAtRow)
    {
    	
    	return ETrue;
    }
    return EFalse;
   

   }
// -----------------------------------------------------------------------------
// Releases the resources
// -----------------------------------------------------------------------------
//      
void CNotesSearcher::Destroy()
    {
    
    
    if( IsActive( ))
        {
        // If already active, then send cancel
         CActive::Cancel();
        }
     else
        {
        // Call the cleanup module
        CleanUp( ); 
        }  
        
    //if(0 < iHeavyResultsArray.Count())
	//{
//	iHeavyResultsArray.ResetAndDestroy();
/*	iSearchLightResult.Reset();
		
	}
	else
	{
	iSearchLightResult.ResetAndDestroy();
	}     
  */  
  delete this;
        
    }


// -----------------------------------------------------------------------------
// for internal cleanup
// -----------------------------------------------------------------------------
//  
void CNotesSearcher::CleanUp()
    {
    
    iKeyHitPos.Reset();  
    if(iContent)
        {
        delete iContent;
        iContent = NULL;
        }
    
    }

// -----------------------------------------------------------------------------
// second phase constructor.
// -----------------------------------------------------------------------------
//
void  CNotesSearcher::ConstructL(const RArray<TUid>& /*aContentIdArray*/,
                                 const CSearchCondition& aCondition,
                                 MSearchPluginObserver& aObserver )
    {
    //Create the text searcher and set the parameters
    iObserver  = &aObserver;
    iTextSearcher = CSearchTextSearcher::NewL( *this );
    iTextSearcher->SetParametersL( aCondition  ); 
  
    //Initilize the database
    InitDbL();  
   
    
    RFs 		    	fsSession;
    TFileName 		resourceFileName;
    RResourceFile     resourceFile;

    User::LeaveIfError( fsSession.Connect() );
    resourceFileName.Copy( TParsePtrC( RProcess().FileName() ).Drive() );  
    resourceFileName.Append( KDC_RESOURCE_FILES_DIR );
    resourceFileName.Append( KAvkonResFilePath );
    	
    // Get language local file name of resource file.
    BaflUtils::NearestLanguageFile( fsSession, resourceFileName );
	resourceFile.OpenL(fsSession, resourceFileName );
	resourceFile.ConfirmSignatureL();
    
    //Read the short date format resource file 	
    HBufC8* readDateBuffer=resourceFile.AllocReadLC( R_QTN_DATE_SHORT );
	const TPtrC16 ptrReadDateBuffer(( TText16*) readDateBuffer->Ptr(),
							( readDateBuffer->Length()+1 )>>1 );
								
								
    iDateFormat = ptrReadDateBuffer.AllocL();
   
    CleanupStack::PopAndDestroy( readDateBuffer );	
	resourceFile.Close();										


    }

// -----------------------------------------------------------------------------
// Initialize DB
// -----------------------------------------------------------------------------
//
void CNotesSearcher::InitDbL()
    {
    //Connect to the database session
    User::LeaveIfError( iDbSession.Connect() );

    TBuf<32> format;
    format.Copy(KSecure);
    format.Append(KSecureUid.Name());
    
    TInt err(iNamedDataBase.Open(iDbSession, KNotepadFilenameAndPath() ,format));
    if(err == KErrNone)
        {
        iNamedDataBase.Compact();
        User::LeaveIfError(iDbView.Prepare(iNamedDataBase,
                            TDbQuery(KNotepadSqlSelectCmd),RDbView::EReadOnly));
        User::LeaveIfError(iDbView.EvaluateAll());
        iSDbmsUp = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// from CActive
// -----------------------------------------------------------------------------
//  
void CNotesSearcher::RunL()
    {
    if( DoActualSearchL() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// Do actual search 
// -----------------------------------------------------------------------------
//  
TBool CNotesSearcher::DoActualSearchL()
    {
    
    TUint  resultCounter = 1;
    TBool  exitStatus  = ETrue;
    
    while( ETrue )
        {
        iDbView.GetL();
        iTextSearcher->Cleanup();
        RDbColReadStream  blob;
        
        // get one memo 
        blob.OpenLC( iDbView, CNotesSearcher::ENotepadMemo );
        TInt colLength(iDbView.ColLength(CNotesSearcher::ENotepadMemo) );
        if( iContent )
            {
            delete iContent;
            iContent = NULL;
            }
        iContent = HBufC16::NewL(colLength);
        TPtr16  contentptr(iContent->Des());
        blob.ReadL(contentptr,colLength);
        CleanupStack::PopAndDestroy(); //blob for full memo  

        iStatusItemCounter = resultCounter;
        iTextSearcher->SearchL(*iContent);
        
        if(iTotalNumOfItems == iStatusItemCounter )
        {
        iObserver->SearchCompletedL(KErrNone,iTotalNumOfItems); 
        exitStatus = EFalse;
        break;
        }
        
        if( !iDbView.AtRow() ) 
        {
        iObserver->SearchCompletedL(KErrNotFound,KErrNotFound); 
        exitStatus = EFalse;
        break;
        }
        
        iDbView.NextL();
        resultCounter++;
        }
        
     return exitStatus;  
    
    }



// -----------------------------------------------------------------------------
// called by framework , when Close function is called
// -----------------------------------------------------------------------------
//  
void CNotesSearcher::DoCancel()
    {
    //Call the clean up module
    CleanUp();
    }

// -----------------------------------------------------------------------------
// Does Searching of notes 
// -----------------------------------------------------------------------------
//  
void CNotesSearcher::SearchL(void)
    {
    __ASSERT_DEBUG(!IsActive(),User::Panic(KNotesAlreadyActive,EPanicAlreadyActive));   
    
    
  //  if(0 < iHeavyResultsArray.Count())
//	{
	iHeavyResultsArray.ResetAndDestroy();
/*	iSearchLightResult.Reset();
		
	}
	else
	{
	iSearchLightResult.ResetAndDestroy();
	}  
*/	iKeyHitPos.Reset();  
    //Check if DBMS is up or quit reporting search complete 
    if( !iSDbmsUp )
        {
        iObserver->SearchCompletedL(KErrNotFound,KErrNotFound);   
        return;
        }
    
    // Positions the cursor on the first row in the rowset.
    TRAPD( err , iDbView.FirstL() );
    if(err != KErrNone )
        {
        // Error, so complete the search
        iObserver->SearchCompletedL(KErrNotFound,KErrNotFound);     
        return;
        }
    if( iDbView.AtEnd() )
        {
        // Reached the end of the database view,
        // complete the search
        iObserver->SearchCompletedL(KErrNotFound,KErrNotFound); 
        return;   
        }
    
    iStatusItemCounter = 1;
    iTotalNumOfItems   = iDbView.CountL();
    
    if( 0 == iTotalNumOfItems )
        {
        // No items in the databse,complete the search
        iObserver->SearchCompletedL(KErrNotFound,KErrNotFound); 
        return; 
        }
       
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// This method gives final results , with snippet 
// -----------------------------------------------------------------------------
//  
void CNotesSearcher::GetResultsL( const RPointerArray<CSearchDocumentId>& 
                                                        aDocumentIdArray )
    {
    
     
 
      RPointerArray<CSearchResult> searchResultArray; 
       
      TBufC8<KBufferLength> mineType(KMimeType);
       
      for(TInt i = 0 ; i < aDocumentIdArray.Count() ; i++ )
        {
           for(TInt j = 0 ; j < iHeavyResultsArray.Count() ; j++ )
            {
            
            if( (aDocumentIdArray[i]->DocumentId() == 
	    				 (iHeavyResultsArray[j]->Result()).DocumentId().DocumentId() ) && 
	    				 (aDocumentIdArray[i]->PluginId().iUid == iNotesPluginId.iUid ) &&
	    				 (aDocumentIdArray[i]->RepositoryId() == KSearchCClassNotesUid.iUid)
	    				 )
                {
                
                
	            CSearchResult* tempHeavy =  	iHeavyResultsArray[j];
	            if((&tempHeavy->Title() != NULL) && (&tempHeavy->Snippet() != NULL))
	            {
	                 searchResultArray.Append( iHeavyResultsArray[j]);
	                 continue;	
	            }
	            //TLex8 myDocId(aDocumentIdArray[i]->DocumentId() );
	        
	        	TInt noteKey = aDocumentIdArray[i]->DocumentId();
	        //	TInt err = myDocId.Val(noteKey);
	   
              //  if( KErrNone == err)
	            {
	               		
        		//CSearchResult* searchResult = CSearchResult::NewL( iHeavyResultsArray[j] );
        		//CleanupStack::PushL( searchResult );
        		HBufC* contentData = NULL;
        		TTime  timeLine; 
        		GetContentAndDateByKeyL(noteKey ,contentData, timeLine);
        		
                TBufC<KBigBufferLength> dateString;
		        TPtr tempDateStringPtr( dateString.Des() );
		        timeLine.FormatL( tempDateStringPtr, *iDateFormat );
        		
				CSearchSnippetCreator* SnippetCreator	= CSearchSnippetCreator::NewL();
			    CleanupStack::PushL(SnippetCreator);
			    CSearchTextSnippet* textSnippet = 
			    	SnippetCreator->CreateTextSnippetL(*(contentData), iKeyHitPos[j] );
			    	
			    TBufC<KBufferLength> tempString1;
				TPtr titlePtr( tempString1.Des() );
				CnvUtfConverter::ConvertToUnicodeFromUtf8( titlePtr,
										textSnippet->Snippet() );
										
                if(iKeyHitPos[j]  < 15)
                {
                   
        		   TBufC8<KBufferLength> tempString;
	               TPtr8 tempPtr8( tempString.Des() );
                   CnvUtfConverter::ConvertFromUnicodeToUtf8( tempPtr8,tempDateStringPtr );
        		   iHeavyResultsArray[j]->SetSnippetL( tempPtr8 ); 	
                }
                else
                {
                    TBufC8<KBufferLength> tempString;
	                TPtr8 tempPtr8( tempString.Des() );
                    CnvUtfConverter::ConvertFromUnicodeToUtf8( tempPtr8,titlePtr );
        		 
                	iHeavyResultsArray[j]->SetSnippetL( tempPtr8 );  
                }
                
        		iHeavyResultsArray[j]->SetTitleL( *contentData );
        		
        		
        		iHeavyResultsArray[j]->SetSnippetFormatL( mineType );
        		
        		CleanupStack::PopAndDestroy( SnippetCreator );
        		  		
        		if( contentData)
        		    {
        		    delete contentData;
        		    contentData = NULL;
        		    }
        	
                //CleanupStack::Pop( searchResult );
        	    	
        	    searchResultArray.Append( iHeavyResultsArray[j] );
        	    //iHeavyResultsArray.Append( searchResult );
        	    if(textSnippet)
        	    {
        	    	delete textSnippet;
        	    	textSnippet = NULL;
        	    }
        	    
        	    
	            }
	            
    	    	}
            
            }
      }
      
      //Complete the results retrival
      iObserver->ResultsRetrieveCompleteL( searchResultArray ); 
      searchResultArray.Reset() ;
      searchResultArray.Close();

    }
    
// -----------------------------------------------------------------------------
// CNotesSearcher::GetSearchProgressL
// search progress
// -----------------------------------------------------------------------------
//      
void CNotesSearcher::GetSearchProgressL(TUid& aContentClassId, 
                                         TInt& aCurrentDocument, 
                                         TInt& aTotalDocuments  )
    {
    if( aContentClassId == KSearchCClassNotesUid )
        {
        aCurrentDocument = iStatusItemCounter;
        aTotalDocuments = iTotalNumOfItems;
        }
    }

// -----------------------------------------------------------------------------
// CNotesSearcher::CancelResultsRetrieve()
// -----------------------------------------------------------------------------
//      
void CNotesSearcher::CancelResultsRetrieve()
    {
      
    }
// -----------------------------------------------------------------------------
// CNotesSearcher::IsSearching
// return TBool 
// -----------------------------------------------------------------------------
//  
TBool CNotesSearcher::IsSearching()
    {
    return IsActive();  
    }

// -----------------------------------------------------------------------------
// CNotesSearcher::Cancel
// Cancels searching 
// -----------------------------------------------------------------------------
//  
void CNotesSearcher::CancelSearch()   
    {
    CActive::Cancel();    
    }


// -----------------------------------------------------------------------------
// notification from textsearcher , when keyword is found  
// -----------------------------------------------------------------------------
//      
void CNotesSearcher::HitL( TInt aKeywordCharPos )
    {
    // get the key for memo     
    TInt key ( iDbView.ColInt( CNotesSearcher::ENotepadKey ) );
    TBuf8<KNoteIdLengthInHex> entryId;
    entryId.AppendNum( key/*, EHex*/ );
    
	CSearchDocumentId*  documentid = CSearchDocumentId::NewL( key , iNotesPluginId );
	documentid->SetRepositoryIdL( KSearchCClassNotesUid.iUid );
	//Create the light result
	CSearchLightResult* searchLightResult = CSearchLightResult::NewL( documentid );
	searchLightResult->SetContentClassId( KSearchCClassNotesUid );
	searchLightResult->SetSubContentId( 0 );
	searchLightResult->SetServiceId( KNullUid );
	
    // Append the lighresult to the  iSearchLightResult array
	//iSearchLightResult.AppendL( searchLightResult );
	
	// Append the key hit position to the  to the  iKeyHitPos array
	iKeyHitPos.AppendL( aKeywordCharPos );
	
	
	CSearchResult* searchResult = CSearchResult::NewL( searchLightResult);

	searchResult->SetSnippetL(KNullDesC8);
	searchResult->SetTitleL(KNullDesC);
	iHeavyResultsArray.Append( searchResult );
	//Notify the observer about the found result
	iObserver->ResultFoundL(searchLightResult ,iStatusItemCounter, iTotalNumOfItems );
	
    }
    
    
 HBufC8* CNotesSearcher::LaunchInfoL( const CSearchDocumentId& aDocumentID )
 {
     if((aDocumentID.PluginId() == iNotesPluginId) &&
        (aDocumentID.RepositoryId() == KSearchCClassNotesUid.iUid))
         {
         
         
         //RAVI CHECK THIS 
         HBufC8 * launchInfo = HBufC8::NewL(256);
                  TPtr8 docIdPtr = launchInfo->Des();
         docIdPtr.AppendNum(aDocumentID.DocumentId());
         //docIdPtr = (aDocumentID.DocumentId()).AllocL();
         return launchInfo;
         }
     
     return NULL;
     
	
}
// -----------------------------------------------------------------------------
// To retrive note content and date of note from notepad db 
// -----------------------------------------------------------------------------
//      
void  CNotesSearcher::GetContentAndDateByKeyL(const TInt& aKey,HBufC*& aContentDes,
                                              TTime&  aTime)
    {
    TBuf<KBufferLength> sqlStatement;
    sqlStatement.Format(KNotepadSqlFormatSeek, aKey);
    
    RDbView dbView;
    CleanupClosePushL(dbView);
    dbView.Prepare( iNamedDataBase , TDbQuery( sqlStatement ) , RDbView::EReadOnly  );
    dbView.EvaluateAll();
    
    TTime  timeData;
    TInt isAtRow(dbView.FirstL());
    User::LeaveIfError(isAtRow);
    if ( isAtRow )
        {
        dbView.GetL();
        
        HBufC* contentDes;
        RDbColReadStream blob;
        blob.OpenLC(dbView, CNotesSearcher::ENotepadMemo);
        TInt colLength( dbView.ColLength( CNotesSearcher::ENotepadMemo ) );
        
        contentDes = HBufC::NewL(colLength);
        TPtr ptr ( contentDes->Des() );
        blob.ReadL(ptr, colLength);
                  
        aContentDes = HBufC::NewL( colLength );
        aContentDes->Des().Append( ptr ); 
        CleanupStack::PopAndDestroy(); // blob
        delete contentDes;
        contentDes = NULL;
        
        timeData = iDbView.ColTime( CNotesSearcher::ENotepadUpdateTime );
        aTime = timeData;
        }
    CleanupStack::PopAndDestroy(); // dbView
    }
    
    

//end of file

