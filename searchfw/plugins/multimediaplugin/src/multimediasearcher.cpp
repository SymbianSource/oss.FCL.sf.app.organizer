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
* Description:   Plugin for Multi-Media information search.
*
*/



//SYSTEM INCLUDES
#include <e32cmn.h>
#include <e32def.h>
#include<barsc.h>
#include <e32des8.h> 
#include <cntdef.h>
#include <e32std.h>
#include <e32base.h>
#include <bautils.h>
#include <utf.h>
#include <searchcondition.h>
#include <searchtextsearcher.h>
#include <searchdocumentid.h>
#include <searchlightresult.h>
#include <searchresult.h>
#include <searchcommon.h>
#include <ContentListingFactory.h>
#include <MCLFItem.h>
#include <CLFContentListing.hrh>
#include <searchpluginobserver.h>
#include <data_caging_path_literals.hrh>

//USER INCLUDES
#include "multimediasearchresource.rsg"
#include "multimediasearcher.h"
#include "multimediasearchplugindefines.h"


//Constants
_LIT( KKiloBytes, " KB" );
_LIT( KBytes, " B" );

// ---------------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------------
//
CMultimediaSearcher* CMultimediaSearcher::NewL( const RArray<TUid>& aContentIdArray, 
                                const CSearchCondition& aCondition, 
                                const TUid& aPluginId,
                                 MSearchPluginObserver& aObserver  )
    {
	CMultimediaSearcher* self = new ( ELeave ) CMultimediaSearcher( aPluginId );
	CleanupStack::PushL( self );
	self->ConstructL( aContentIdArray, aCondition ,aObserver);
	CleanupStack::Pop( self );
	return self;
    }
    
    
// ---------------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------------
//
CMultimediaSearcher::~CMultimediaSearcher() 
{

	ifSession.Close();
	CleanUp();
  
	if(iHeavyResultsArray.Count())
 {
	iHeavyResultsArray.ResetAndDestroy();
 }
 
if(iCompleteFilePathArray.Count())
 {	
   iCompleteFilePathArray.ResetAndDestroy();
	}
  if( iTextSearcher ) 
	{
		delete iTextSearcher;
		iTextSearcher = NULL;
	}
	
}

// ---------------------------------------------------------------------------
// Sets the value of iCurrentContentClassUid
// ---------------------------------------------------------------------------
//
void CMultimediaSearcher::SetContentClassUid(const TUid aSearchClassUid)
{
	
	iCurrentContentClassUid.iUid = aSearchClassUid.iUid;
	
}

// ---------------------------------------------------------------------------
// Sets the value of iCurrentContentClassUid
// ---------------------------------------------------------------------------
//
void CMultimediaSearcher::SetMediaType(TCLFMediaType aMediaType)
{
	
	iCurrentMediaTypeToBeUsed = aMediaType;
	
}
   
// ---------------------------------------------------------------------------
// Gets the complete file path of the item in the iCompleteFilePathArray
// ---------------------------------------------------------------------------
//   
HBufC* CMultimediaSearcher::GetFilePathArrayIndexValue(TInt aIndex)
{
    return  iCompleteFilePathArray[aIndex] ;  	
}

// ---------------------------------------------------------------------------------
// should be called before deleting this object
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::Destroy() 
{

    if(IsActive())
    {
    CActive::Cancel();
    }
    else
    {
    	CleanUp();
    }

	
	if(iHeavyResultsArray.Count())
 {
	iHeavyResultsArray.ResetAndDestroy();
 }
 
	
if(iCompleteFilePathArray.Count())
 {	
   iCompleteFilePathArray.ResetAndDestroy();
 }
 delete this;
}

// ---------------------------------------------------------------------------------
// Starts the search
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::SearchL( void )
    {
        
    //Reset all the flags
    iCancelSearch = EFalse;
    iAllMultimediaFilesSearched = EFalse;
    iRefreshStarted = EFalse;
    iRefreshDone = EFalse;
     
    if( 0 < iHeavyResultsArray.Count() )
		iHeavyResultsArray.ResetAndDestroy();
	
	
	
	if( 0 < iCompleteFilePathArray.Count() )
	iCompleteFilePathArray.Reset();
	
	
	// Create the content listing framework engine
    iCLFengine = ContentListingFactory::NewContentListingEngineLC();
    CleanupStack::Pop();
    iCLFmodel = iCLFengine->CreateListModelLC( *this );
    CleanupStack::Pop();
    
    
    //Create the WantedMediaTypes array and set it to CLF Engine
    RArray<TInt> array;
    CleanupClosePushL( array );
    array.AppendL( iCurrentMediaTypeToBeUsed );
    iCLFmodel->SetWantedMediaTypesL( array.Array() );
    
    CleanupStack::PopAndDestroy( &array );
    //Create the MimeTypeArray array and set it to CLF Engine
     
    if ( iMimeTypeArray )
    {
        iMimeTypeArray->Reset();
 		delete iMimeTypeArray;
		iMimeTypeArray = NULL;	
    }
    iMimeTypeArray = new (ELeave) CDesCArrayFlat(KArrayLength );
    iMimeTypeArray->AppendL( KAllMimeTypeMatchString );
    iCLFmodel->SetWantedMimeTypesL( *iMimeTypeArray );
   
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
	SetActive(); 
    }

// ---------------------------------------------------------------------------------
// Returns TRUE  if search is in progress
// ---------------------------------------------------------------------------------
TBool CMultimediaSearcher::IsSearching() 
{
	return IsActive();
}

// ---------------------------------------------------------------------------------
// Cancels the search
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::CancelSearch() 
{
    iCancelSearch = ETrue;
    
    // If the iCLFmodel refresh has started and not complete yet,
    // then cancel the refresh
    if(iRefreshStarted  &&  !iRefreshDone)
    {
    	iCLFmodel->CancelRefresh();
    }
	CActive::Cancel();
	CleanUp();
}

// ---------------------------------------------------------------------------------
// Gets the results 
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::GetResultsL( 
	const RPointerArray<CSearchDocumentId>& aDocumentIdArray )
{
	
    // Check the aDocumentIdArray
    // For all the matching documentIds, fetch the corresponding
    // Heavy Result from iHeavyResultsArray and append it to heavyResultResponse
	RPointerArray<CSearchResult> heavyResultResponse; 
	for( TInt j = 0; j < aDocumentIdArray.Count(); j++ ) 
	{
	      
	    if  (( aDocumentIdArray[j]->PluginId() == iPluginId ) &&
	        (iCurrentContentClassUid.iUid == aDocumentIdArray[j]->RepositoryId())
	        )
	    {
		     TInt myArrayIndex = aDocumentIdArray[j]->DocumentId();
	          heavyResultResponse.AppendL(iHeavyResultsArray[myArrayIndex]);        
	    }             
	}
		
	// Results retrival complete	
	iObserver->ResultsRetrieveCompleteL( heavyResultResponse );
	heavyResultResponse.Reset();
}

// ---------------------------------------------------------------------------------
// Cancels the result retrieval
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::CancelResultsRetrieve() 
{
	
}

// ---------------------------------------------------------------------------------
// Returns the search progress
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::GetSearchProgressL( TUid& /*aContentClassId*/, 
                                              TInt& /*aCurrentDocument*/, 
                                              TInt& /*aTotalDocuments*/ )
{
	
}


// ---------------------------------------------------------------------------------
// Called when a result is found matching the search criteria
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::HitL( TInt /*aResult*/ ) 
{
	
    
	CSearchDocumentId* searchDocId = NULL;
	
	TBuf8<KBufferLength> tempDes; 

	searchDocId = CSearchDocumentId::NewL( iTotalHits,iPluginId );
	
		searchDocId->SetRepositoryIdL( iCurrentContentClassUid.iUid );
	
	// Set the repository id
	searchDocId->SetRepositoryIdL( iCurrentContentClassUid.iUid );

	
	CSearchLightResult* searchLightResult = CSearchLightResult::NewL( searchDocId );
	searchLightResult->SetContentClassId( iCurrentContentClassUid );
	searchLightResult->SetServiceId( KNullUid );
    
	// Append iCompletefilePath to the iCompleteFilePathArray. 
	// This is needed for launching the specific media item.
	if( iCompletefilePath )
	{  	
	  HBufC* string1 = iCompletefilePath->Des().AllocL();
      iCompleteFilePathArray.AppendL(string1);     
	}
	
  TParse fileParse;
	fileParse.Set( iCompletefilePath->Des(), NULL, NULL );
	TPtrC driveLetter = fileParse.Drive();
	_LIT(KMemoryCard, "E:");
	//If found at 0th position file is in mem card
	if ( 0 == driveLetter.Find( KMemoryCard ) ) 
	    {
	    searchLightResult->SetSubContentId( KSearchCSubContentMMCUid.iUid );
	    }
    // Form the heavy result and append it to the iHeavyResultsArray
    CSearchResult* tmpHeavyRes = NULL;
    tmpHeavyRes = CSearchResult::NewL(searchLightResult);
    
    //Set the title of the result
    if ( iCurrentTitle )
    {
	    tmpHeavyRes->SetTitleL( *iCurrentTitle );
        delete iCurrentTitle; iCurrentTitle = NULL;
    }
    else
    {
         // The title is not set. Set it to null string
         TPtr ptr( iCurrentTitle->Des() );
      	 ptr.Append( KNullDesC );
      	 tmpHeavyRes->SetTitleL( *iCurrentTitle );
         delete iCurrentTitle; iCurrentTitle = NULL;
     }
     
    //Set the snippet 
    if ( iCurrentSnippet )
    {
	    tmpHeavyRes->SetSnippetL(*iCurrentSnippet);
        delete iCurrentSnippet;iCurrentSnippet = NULL;
    }
    else
    {   
         // The Snippet is not set. Set it to null string 
         iCurrentSnippet =HBufC8::NewL(KSpace().Length()); 
         iCurrentSnippet->Des().Append( KSpace );
         tmpHeavyRes->SetSnippetL(*iCurrentSnippet);
         delete iCurrentSnippet;iCurrentSnippet = NULL;
    }
    
    //Set the SetSnippetFormatL
    HBufC8 *snippetFormat = HBufC8::NewL( KBufferLength ) ;
    snippetFormat->Des().Append(KSpace);
	tmpHeavyRes->SetSnippetFormatL( snippetFormat->Des() );
	delete snippetFormat; 
	snippetFormat = NULL;
	
    
    iHeavyResultsArray.AppendL(tmpHeavyRes);
	//Delete the iDataToSearch, since already searched
	if ( iDataToSearch )
    {
 		delete iDataToSearch;
		iDataToSearch = NULL;	
		
    }
    
  
	// Increment the iTotalHits and call ResultFoundL()
	iTotalHits++;              
    iObserver->ResultFoundL( searchLightResult, iTotalHits, iTotalNumOfItemsSearched );

}


HBufC8* CMultimediaSearcher::LaunchInfoL( const CSearchDocumentId& aDocumentID )
{
     if((aDocumentID.PluginId() == iPluginId) &&
        (aDocumentID.RepositoryId() == iCurrentContentClassUid.iUid))
         {
        
         TInt myArrayIndex = aDocumentID.DocumentId();
	         TBuf8<KBufferLength> tempDes; 
	         HBufC8* launchData = HBufC8::NewL( KBufferLength ) ;
	         
	         
	         
             CnvUtfConverter::ConvertFromUnicodeToUtf8( tempDes,*iCompleteFilePathArray[myArrayIndex] );
              launchData->Des().Append(tempDes);
	            return launchData;
         }
     
     return NULL;
     
	
}


// ---------------------------------------------------------------------------
// Callback method of MCLFOperationObserver for refresh .
// Called when the refersh operation is complete
// ---------------------------------------------------------------------------
//
void CMultimediaSearcher::HandleOperationEventL( TCLFOperationEvent aOperationEvent,
                                    TInt /*aError*/ )
{
        if( aOperationEvent == ECLFRefreshComplete )
            {
            // The refresh is complete, so set the iRefreshDone flag to true
            iRefreshDone = ETrue;
            }
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
	    SetActive(); 
}




// ---------------------------------------------------------------------------------
// Implements cancellation of an outstanding request.
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::DoCancel() 
{
   iCancelSearch = ETrue;
   CleanUp();

}


// ---------------------------------------------------------------------------------
// The function is called by the active scheduler 
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::RunL()  
    {
	if(  !DoActualSearchL() )
	    {
		
	    ReportFinishedL();   
    	
	    }
	
    }

// ---------------------------------------------------------------------------------
// Called if there are any leaves in the RunL()
// ---------------------------------------------------------------------------------
//
TInt CMultimediaSearcher::RunError(TInt /*aError*/) 
{
	return KErrNone;
}



  

    
// ---------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------
//
CMultimediaSearcher::CMultimediaSearcher( const TUid& aPluginId ) :
	CActive( CActive::EPriorityStandard ), 
	iPluginId( aPluginId ),
	iTotalNumOfItemsSearched(0),
	iTotalHits(0),
	iAllMultimediaFilesSearched(EFalse),
	iSearchAlreadyStarted(EFalse),
    iRefreshStarted (EFalse),
    iRefreshDone(EFalse)
    {
	CActiveScheduler::Add( this );
    }
// ---------------------------------------------------------------------------------
// CMultimediaSearcher::ConstructL() 
// 2nd phase constructor
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::ConstructL( const RArray<TUid>& /*aContentIdArray*/, 
		const CSearchCondition& aCondition,MSearchPluginObserver& aObserver ) 
    {
    iObserver  = &aObserver;
    User::LeaveIfError( ifSession.Connect() );
    SetPriority( EPriorityStandard );
    //Create the text searcher and set the parameters
	iTextSearcher = CSearchTextSearcher::NewL( *this );
    iTextSearcher->SetParametersL( aCondition );
    
    }

// ---------------------------------------------------------------------------------
// Called when search is finished
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::ReportFinishedL()
    {
		iObserver->SearchCompletedL( iTotalHits, iTotalNumOfItemsSearched );
       
    }

// ---------------------------------------------------------------------------------
// Will be called when the CActive::Cancel() is called.. by DoCancel()
// ---------------------------------------------------------------------------------
//
void CMultimediaSearcher::CleanUp() 
{
  //Clean up the resources
  
  if(iCLFmodel)
  {
  	delete iCLFmodel;
  	iCLFmodel = NULL;
  }
  if ( iCLFengine )
    {
 		delete iCLFengine;
		iCLFengine = NULL;	
    }
    
    if ( iMimeTypeArray )
    {
        iMimeTypeArray->Reset();
 		delete iMimeTypeArray;
		iMimeTypeArray = NULL;	
    }

   if ( iCurrentTitle )
    {
 		delete iCurrentTitle;
		iCurrentTitle = NULL;	
    }

    if ( iCurrentSnippet )
    {
 		delete iCurrentSnippet;
		iCurrentSnippet = NULL;	
    }

    if ( iCompletefilePath )
    {
 		delete iCompletefilePath;
		iCompletefilePath = NULL;	
    }
    
    if ( iDataToSearch )
    {
 		delete iDataToSearch;
		iDataToSearch = NULL;	
    }

    if(iSizeBytes)
    {
    delete iSizeBytes;
    iSizeBytes = NULL;
    }

    if(iSizeKiloBytes )
    {
    
    delete iSizeKiloBytes;
    iSizeKiloBytes = NULL;
   	
    }

	   
}

// ---------------------------------------------------------------------------------
// Does the search for the file.  Called from RunL. 
// Return:  ETrue: if more note items need to be searched. 
//         EFalse: otherwise.
// ---------------------------------------------------------------------------------
//
TBool CMultimediaSearcher::DoActualSearchL()
	{
	   if(iCancelSearch)
	       {
	       //If the search has been cancelled, return EFalse  
	       return EFalse;
	       }
	     
	    
	    if (iAllMultimediaFilesSearched)
	        {
	        // If all the files have been serched, then return EFalse
		    return EFalse; 
	        }
	    
	    // If Refresh not yet done, then do it
	    if(!iRefreshStarted)
	        {
	         
	         iRefreshStarted = ETrue;
	   	     iCLFmodel->RefreshL();  
		    }
	    if(iRefreshDone)
	        {
	        // IF refresh done, then start the search  
	        
	        if(!iSearchAlreadyStarted)
	            {
	            //Start searching if already not started  
	        	iSearchAlreadyStarted = ETrue;
	        	SearchMediaL();
	            }
	    	
	        }
	    
	    return ETrue;

	
	}

// ---------------------------------------------------------------------------
//  Search Media (Images, Videos. Music) from the list of multimedia items
// ---------------------------------------------------------------------------
//

void CMultimediaSearcher::SearchMediaL(void)
    {
        
    TInt count( iCLFmodel->ItemCount() );
    TBool ret = EFalse;
	for( TInt i( 0 ); i < count; ++i )
	    {
	    if(iCancelSearch)
	        {
	        	return;
	        }
	    const MCLFItem& myItem = iCLFmodel->Item( i );
	    TInt32 mediaType( 0 );
	    if( myItem.GetField( ECLFFieldIdMediaType, mediaType ) == KErrNone )
            {
            // If media type is not same as current, 
            // then continue with the next item
            if( iCurrentMediaTypeToBeUsed != mediaType)
                {
               	continue;
                }
            }
            
        // Get the data for music, video , images
        // depending on the media type
        ret = EFalse;
        if(ECLFMediaTypeMusic ==mediaType)
	        {
	        // Music 
	        ret = FetchSearchDataForMusicL(myItem);	
	        }
	    else if (ECLFMediaTypeImage == mediaType)
	        {
	        //Images
	       	ret = FetchSearchDataForImagesL(myItem);
	        }
	        
	    else if(ECLFMediaTypeVideo ==mediaType)
	        {
	        //Videos
	       	ret = FetchSearchDataForVideosL(myItem);
	        }
	        
	        
	    // Send the data for searching only if the
	    // return value is ETrue(i.e valid multimedia item)   	        
	   	if(ret)
	   	    {
	   	    	iTextSearcher->SearchL( *iDataToSearch );
	   	    }
	            
	    }

       // All data searched, set the iAllMultimediaFilesSearched flag and complete the request
	   iAllMultimediaFilesSearched = ETrue;
	   TRequestStatus* status = &iStatus;
       User::RequestComplete( status, KErrNone );
	   SetActive(); 
}


// ---------------------------------------------------------------------------
//  Creates Search Data for Videos for one Media Gallery item
// ---------------------------------------------------------------------------
//
TBool  CMultimediaSearcher::FetchSearchDataForVideosL(const MCLFItem& aItem)
    {
    //Get the file name of the Images item.
	TPtrC fileName;
	TInt error( aItem.GetField( ECLFFieldIdFileName, fileName ) );
	      
	if(fileName.Length())
	    {
	    // The file exists, get the complete path of the file
	   	iTotalNumOfItemsSearched++;
	    delete iCompletefilePath;
	    iCompletefilePath = NULL;
	    TPtrC filePath;
	    TInt error( aItem.GetField( ECLFFieldIdFileNameAndPath, filePath ) );
	    iCompletefilePath = HBufC::NewL(filePath.Length()); 
	    TPtr ptr( iCompletefilePath->Des() );
	    ptr.Append(filePath);
	    }
	else
	    {
	    //The file name length is <= 0. So return false
	   	return EFalse;
	    }
	    delete iDataToSearch; 
	    iDataToSearch = NULL;
	    iDataToSearch = HBufC::NewL( fileName.Length());
	    TPtr ptr1( iDataToSearch->Des() );
	    ptr1.Append(fileName);
	    
	    //set the title   
	    delete iCurrentTitle;iCurrentTitle = NULL;   
	    iCurrentTitle =HBufC::NewL(fileName.Length()); 
        iCurrentTitle->Des().Append( fileName );
         	
	    //Set the Snippet
	    delete iCurrentSnippet;iCurrentSnippet = NULL;   
	    if(fileName.Length())
	        {   
	        //Get the file size, to be set for the snippet
	        TInt32 fileSize ;
	        aItem.GetField( ECLFFieldIdFileSize,fileSize);
	        iCurrentSnippet =HBufC8::NewL(KBufferLength); 
             
            if( fileSize / KKiloByteDivisor ) 
				{   
				// If the filesize > 1024, then append "KB" (for kilobytes)
				iCurrentSnippet->Des().AppendNum( fileSize / KKiloByteDivisor );
				iCurrentSnippet->Des().Append(KBytes);
				}
			else 
			    {
		         // Filesize < 1024, so just append "B" (for bytes)	 
				 iCurrentSnippet->Des().AppendNum( fileSize );
				 iCurrentSnippet->Des().Append( KKiloBytes );
		        }
         	
	         }
	        
	   return ETrue;   
    }

 // ---------------------------------------------------------------------------
//  Creates Search Data for Images for one Media Gallery item
// ---------------------------------------------------------------------------
//
TBool  CMultimediaSearcher::FetchSearchDataForImagesL(const MCLFItem& aItem)
    {
    //Get the file name of the Images item.
	TPtrC fileName;
	TInt error( aItem.GetField( ECLFFieldIdFileName, fileName ) );
	if(fileName.Length())
	    {
	    // The file exists, get the complete path of the file
	   	iTotalNumOfItemsSearched++;
	    delete iCompletefilePath;
	    iCompletefilePath = NULL;
	    TPtrC filePath;
	    TInt error( aItem.GetField( ECLFFieldIdFileNameAndPath, filePath ) );
	    iCompletefilePath = HBufC::NewL(filePath.Length()); 
	    TPtr ptr( iCompletefilePath->Des() );
	    ptr.Append(filePath);
	    }
	else
	   {
	   //The file name length is <= 0. So return false
	   return EFalse;
	   }
	
	delete iDataToSearch; 
	iDataToSearch = NULL;
	iDataToSearch = HBufC::NewL( fileName.Length());
	TPtr ptr( iDataToSearch->Des() );
	if(fileName.Length())  ptr.Append(fileName);
	        
	//Set the title
	delete iCurrentTitle;iCurrentTitle = NULL;   
	iCurrentTitle =HBufC::NewL(fileName.Length()); 
    iCurrentTitle->Des().Append( fileName );
         	
	//Set the Snippet
	delete iCurrentSnippet;iCurrentSnippet = NULL;   
	if(fileName.Length())
	    {   
	    TInt32 fileSize ;
	    //Get the file size, to be set for the snippet
	    aItem.GetField( ECLFFieldIdFileSize,fileSize);
	    iCurrentSnippet =HBufC8::NewL(KBufferLength); 
            
        if( fileSize / KKiloByteDivisor ) 
		    {
		    // If the filesize > 1024, then append "KB" (for kilobytes)
			iCurrentSnippet->Des().AppendNum( fileSize / KKiloByteDivisor );
			iCurrentSnippet->Des().Append( KBytes );
			}
		else 
		    {
		    // Filesize < 1024, so just append "B" (for bytes)	 
			iCurrentSnippet->Des().AppendNum( fileSize );
			iCurrentSnippet->Des().Append( KKiloBytes );
			
			}
         	
         	
	     }
    
	return ETrue;       
    }


// ---------------------------------------------------------------------------
//  Creates Search Data for Music for one Media Gallery item
// ---------------------------------------------------------------------------
//
TBool CMultimediaSearcher::FetchSearchDataForMusicL(const MCLFItem& aItem)
    {
    //Get the file name of the music item.
    TPtrC fileName;
	TInt error( aItem.GetField( ECLFFieldIdFileName, fileName ) );
	delete iDataToSearch; 
	iDataToSearch = NULL;
	iDataToSearch = HBufC::NewL(fileName.Length()); 
	        
	if(fileName.Length())
	    {
	    // The file exists, get the complete path of the file
	   	iTotalNumOfItemsSearched++;
	    delete iCompletefilePath;
	    iCompletefilePath = NULL;
	    TPtrC filePath;
	    TInt error( aItem.GetField( ECLFFieldIdFileNameAndPath, filePath ) );
	    iCompletefilePath = HBufC::NewL(filePath.Length()); 
	    TPtr ptr( iCompletefilePath->Des() );
	    ptr.Append(filePath);
	    }
	else
	    {
	    //The file name length is <= 0. So return false
	    return EFalse;
	    }
	//Get the other fileds of the music item.
	// (songname,artistname,album name, genrename, composername)        
	TPtrC   songName, artistName,albumName,genreName,composerName;
    aItem.GetField( ECLFFieldIdSongName, songName);
    aItem.GetField(ECLFFieldIdArtist,artistName);
	aItem.GetField(ECLFFieldIdAlbum,albumName);
	aItem.GetField(ECLFFieldIdGenre,genreName);
	aItem.GetField(ECLFFieldIdComposer,composerName);
	
	// Create the iDataToSearch buffer big enough to hold the 
	// data ((songname,artistname,album name, genrename, composername),
	// all separated by spaces
	delete iDataToSearch; 
	iDataToSearch = NULL;
	    
	iDataToSearch = HBufC::NewL( fileName.Length() + KSpace().Length() +
	                             songName.Length() + KSpace().Length() +
	                             artistName.Length()+KSpace().Length() +
	                             albumName.Length()+KSpace().Length() +
	                             genreName.Length()+KSpace().Length() +
	                             composerName.Length() + KSpace().Length() 
	                                );
	// Append the data to the iDataToSearch
	TPtr ptr( iDataToSearch->Des() );
	if(fileName.Length())
	{
		  ptr.Append(fileName);
		  ptr.Append( KSpace );
	}	  
	if(songName.Length())
	{
		  ptr.Append(songName);
		  ptr.Append( KSpace );
	}	  
	if(artistName.Length())
	{
		  ptr.Append(artistName);                         
		  ptr.Append( KSpace );
	}	  
	if(albumName.Length())
	{
		  ptr.Append(albumName);
		  ptr.Append( KSpace );
	}	  
	if(genreName.Length())
	{
		  ptr.Append(genreName);
		  ptr.Append( KSpace );
	}	  
	
	if(composerName.Length())
	{
		ptr.Append(composerName);
	  ptr.Append( KSpace );
	}	     
	//Set the title
    delete iCurrentTitle;iCurrentTitle = NULL;   
	if(artistName.Length())
	    {   
	    iCurrentTitle =HBufC::NewL(artistName.Length()); 
        iCurrentTitle->Des().Append( artistName );
         	
	    }
	else
	    {
	    // The artist name does not exits, set it to "Unknown Artist"
	    _LIT( unnamed, "Unknown Artist" );
	   	iCurrentTitle =HBufC::NewL(unnamed().Length()); 
        iCurrentTitle->Des().Append( unnamed );
	    }
	     
    //Set the Snippet
    delete iCurrentSnippet;iCurrentSnippet = NULL;   
	if(songName.Length())
	    {   
	    iCurrentSnippet =HBufC8::NewL(songName.Length()); 
        iCurrentSnippet->Des().Append( songName );
        }
	else
	    {
	    // Songname does not exist, set the file name as the snippet
	   	iCurrentSnippet =HBufC8::NewL(fileName.Length()); 
        iCurrentSnippet->Des().Append( fileName );
        }
	     	     
	 return ETrue;
	        
    }



// End of file
