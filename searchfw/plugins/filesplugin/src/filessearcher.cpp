/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Plugin for file names search 
*
*/



#include <e32cmn.h>
#include <e32def.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <utf.h>
#include <barsc.h>
#include <avkon.rsg>
#include <StringLoader.h>

#include <searchtextsearcher.h>
#include <searchdocumentid.h>
#include <searchlightresult.h>
#include <searchresult.h>
#include <searchcommon.h>
#include <filessearchresource.rsg>
#include <searchpluginobserver.h>

#include "filessearcher.h"
#include "filessearchplugindefines.h"

_LIT ( KSkipPrivate, "private" );
_LIT ( KSkipSys, "sys" );
_LIT ( KSkipRta, "Z:\\rta" );
_LIT ( KSkipSystem, "System" );
_LIT ( KSkipTef, "tef_centralrepository" );
_LIT ( KSkipSysTest, "Z:\\test" ); 
_LIT ( KSkipSmokeTest, "Z:\\smoketest" ); 
_LIT ( KSkipResource, "resource" );  
_LIT ( KSkipResourceDir, "Z:\\resource_files_dir" );
_LIT ( KSkipNokia, "Z:\\Nokia" );
_LIT ( KSkipCNokia, "C:\\Nokia" );
_LIT ( KSkipCDrive, "C:\\" );
_LIT ( KSkipCDriveTFW, "C:\\TestFramework" );
_LIT ( KSkipCDrivePredic, "C:\\Predic" );
_LIT ( KSkipCDriveLogs, "C:\\logs" );

const TInt constant_MB = 1024*1024;
const TInt constant_KB = 1024;


// ---------------------------------------------------------------------------------
// CFilesSearcher::NewL() 
// 1st phase constructor
// ---------------------------------------------------------------------------------
//
CFilesSearcher* CFilesSearcher::NewL( const RArray<TUid>& aContentIdArray, 
                                      const CSearchCondition& aCondition, 
                                      const TUid& aPluginId,
                                      MSearchPluginObserver& aObserver )
	{
	CFilesSearcher* self = new ( ELeave ) CFilesSearcher( aPluginId );
	CleanupStack::PushL( self );
	self->ConstructL( aContentIdArray, aCondition,aObserver );
	CleanupStack::Pop( self );
	
	return self;
	}
	
// ---------------------------------------------------------------------------------
// CFilesSearcher::~CFilesSearcher() 
// Destructor
// ---------------------------------------------------------------------------------
//
CFilesSearcher::~CFilesSearcher() 
	{
	
	if ( iTextSearcher ) 
		{
		delete iTextSearcher;
		iTextSearcher = NULL;
		}	
	if ( iSizeKB )
		{
		
		delete iSizeKB;
		iSizeKB = NULL;
		}
	if ( iSizeB )
		{
		
		delete iSizeB;
		iSizeB = NULL;
		}
	if ( iSizeMB )
		{
		
		delete iSizeMB;
		iSizeMB = NULL;
		}
	if(0 < 	iHeavyResultsArray.Count())
	{
	iHeavyResultsArray.ResetAndDestroy();
	}
	if( 0 < iCompleteFilePathArray.Count() )
	iCompleteFilePathArray.ResetAndDestroy();
	}


// ---------------------------------------------------------------------------
// Gets the complete file path of the item in the iCompleteFilePathArray
// ---------------------------------------------------------------------------
//   
HBufC* CFilesSearcher::GetFilePathArrayIndexValue(TInt aIndex)
{
    return  iCompleteFilePathArray[aIndex] ;  	
}

// ---------------------------------------------------------------------------------
// CFilesSearcher::Destroy() 
// should be called before deleting this object
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::Destroy() 
	{
	if ( IsActive() ) 
		{
		CActive::Cancel();
		}
	else
	    {
	    CleanUp();	
	    }	

	iFs.Close();
	delete this;
	
	}
	
// ---------------------------------------------------------------------------------
// CFilesSearcher::SearchL
// Starts the search
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::SearchL()
    {
    
    if(0 < 	iHeavyResultsArray.Count())
	{
	iHeavyResultsArray.ResetAndDestroy();
	//iLightResultsArray.Reset();
		
	}
		if( 0 < iCompleteFilePathArray.Count() )
	iCompleteFilePathArray.ResetAndDestroy();
	
	/*else
	{
	iLightResultsArray.ResetAndDestroy();
	}*/

	iHeavyResultsAsked = EFalse;
	iFolder = HBufC::NewL(KMaxFileName);
    iFileName = HBufC::NewL(KMaxFileName);
    //iResults.ResetAndDestroy(); ???
    
	TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
	SetActive();
    }

// ---------------------------------------------------------------------------------
// CFilesSearcher::IsSearching() 
// Returns TRUE  if search is in progress
// ---------------------------------------------------------------------------------
//
TBool CFilesSearcher::IsSearching() 
	{
	return IsActive();
	}
		
// ---------------------------------------------------------------------------------
// CFilesSearcher::CancelSearch() 
// Cancels the search
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::CancelSearch() 
	{
	CActive::Cancel();
	}

// ---------------------------------------------------------------------------------
// CFilesSearcher::GetResultsL
// Gets the results 
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::GetResultsL( 
	const RPointerArray<CSearchDocumentId>& aDocumentIdArray )
    {
    
	   TInt myArrayIndex;
	   TInt err;
    RPointerArray<CSearchResult> searchResultArray; 
    for(TInt i = 0 ; i < aDocumentIdArray.Count() ; i++ )
        {
        
        
        TInt myArrayIndex = aDocumentIdArray[i]->DocumentId();
        

	       if  (( aDocumentIdArray[i]->PluginId() == iPluginId ) &&
	           (KSearchCClassFilenamesUid.iUid == aDocumentIdArray[i]->RepositoryId()) )
	           {
	            if(myArrayIndex < iHeavyResultsArray.Count())
	                {
	                CSearchResult* tempHeavy = iHeavyResultsArray[myArrayIndex];
	                if(  (&tempHeavy->Title()) && (&tempHeavy->Snippet()))  
	                    {
	                     searchResultArray.Append( iHeavyResultsArray[myArrayIndex]);
	                     continue;	
	                     }
	                 
	                HBufC* path16 = iCompleteFilePathArray[myArrayIndex];
	
	                TChar backSlash( '\\' );
		   		//From the path extract the filename
			    	TInt pos1 = path16->LocateReverse( backSlash );
				    TInt len1 = path16->Length();
				    TInt diff = len1 - pos1 - 1; //bypass the backSlash
			    	TPtrC ptr( path16->Right( diff ) );
				    HBufC* hDoc = ptr.AllocL(); 
				    tempHeavy->SetTitleL( *hDoc );
	
	                TPtr docIdPtr = path16->Des();
	                TEntry entry; 
				    iFs.Entry( docIdPtr, entry );
				    TBuf<20> len;
			    	//If size is more than 1k size should be represented in KBs.
				    if ( 0 == entry.iSize / constant_MB ) 
					    {
					    if ( entry.iSize / constant_KB ) 
				    	    {

						    StringLoader::Format( len, *iSizeKB, -1,  
						    	entry.iSize / constant_KB  );
				    	    }
					    else 
				    	    {
						    StringLoader::Format( len, *iSizeB, -1,  
						    	entry.iSize );
				    	    }
					    }
				    else 
					    {
					    StringLoader::Format( len, *iSizeMB, -1,  
						    	entry.iSize / constant_MB );
					    }
				   TBuf8<20>	len8;
				   CnvUtfConverter::ConvertFromUnicodeToUtf8( len8,
												len );
				   tempHeavy->SetSnippetL( len8 );
				   delete hDoc;
				   hDoc= NULL;
				   			
			       searchResultArray.Append( tempHeavy );
                }
	
            }
            
        }
        iObserver->ResultsRetrieveCompleteL( searchResultArray );

	    searchResultArray.Reset();
    
    }

// ---------------------------------------------------------------------------------
// CFilesSearcher::CancelResultsRetrieve() 
// Cancels the result retrieval
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::CancelResultsRetrieve() 
	{
	
	}

// ---------------------------------------------------------------------------------
// CFilesSearcher::GetSearchProgressL
// Returns the search progress
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::GetSearchProgressL( TUid& /*aContentClassId*/, TInt& /*aCurrentDocument*/, TInt& /*aTotalDocuments*/ )
    {
	
    }

// ---------------------------------------------------------------------------------
// CFilesSearcher::HitL() 
// Called when a result is found matching the search criteria
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::HitL( TInt /*aResult*/ ) 
    {
	CSearchLightResult* searchResult = NULL;
	CSearchDocumentId* searchDocId = NULL;


	//File path is used as the DocumentId to uniquely identify
	//a file 
	TBuf8<KBufferLength> tempDes; 
	

	searchDocId = CSearchDocumentId::NewL( iTotalHits,iPluginId );
	searchDocId->SetRepositoryIdL( KSearchCClassFilenamesUid.iUid );
	
	
	
	CleanupStack::PushL( searchDocId );
	
	
	HBufC* path = HBufC::NewLC( KMaxFileName ) ;
	path->Des().Append( *iFolder );
	path->Des().Append( *iFileName );
	iCompleteFilePathArray.AppendL(path);
	
    //CnvUtfConverter::ConvertFromUnicodeToUtf8( refBuf, *path );
    
	
	
	searchResult = CSearchLightResult::NewL( searchDocId );
	searchResult->SetServiceId( KNullUid );	
	//Set the content class Id as KSearchCClassFilenamesUid
	searchResult->SetContentClassId( KSearchCClassFilenamesUid );
	//If file is found from memory card
	_LIT( KMemCard, "E:" );
	TInt found = path->Find( KMemCard );
	if ( !found )
	    {
		//Set subcontentId as KSearchCSubContentMMCUid
		searchResult->SetSubContentId( KSearchCSubContentMMCUid.iUid );
	    }	
	    
	CSearchResult* searchHeavyResult = CSearchResult::NewL( searchResult);
	searchHeavyResult->SetSnippetL(KNullDesC8);
	searchHeavyResult->SetTitleL(KNullDesC);
	iHeavyResultsArray.Append( searchHeavyResult );
	
	//iLightResultsArray.AppendL( searchResult );
	CleanupStack::Pop( path );
	CleanupStack::Pop( searchDocId );
	
	iTotalHits++;
	
//	delete docIdAsString;
	//docIdAsString = NULL;
	//Notify the observer about the found result
	iObserver->ResultFoundL( searchResult, iTotalHits, iCurrentItemCount );
    }




HBufC8* CFilesSearcher::LaunchInfoL( const CSearchDocumentId& aDocumentID )
    {
    if((aDocumentID.PluginId() == iPluginId) &&
       (aDocumentID.RepositoryId() == KSearchCClassFilenamesUid.iUid))
        {
        TInt myArrayIndex =aDocumentID.DocumentId() ;
	    TBuf8<KBufferLength> tempDes; 
	    HBufC8* launchData = HBufC8::NewL( KBufferLength ) ;
        CnvUtfConverter::ConvertFromUnicodeToUtf8( tempDes,*iCompleteFilePathArray[myArrayIndex] );
        launchData->Des().Append(tempDes);
	    return launchData;
        }
    return NULL;
    }

// ---------------------------------------------------------------------------------
// CFilesSearcher::CFilesSearcher() 
// Constructor
// ---------------------------------------------------------------------------------
//
CFilesSearcher::CFilesSearcher( const TUid& aPluginId ) :
	CActive( CActive::EPriorityStandard ), iPluginId( aPluginId )
	,iCurrentDriveIndex( 0 ),iHeavyResultsAsked(EFalse)
	{
	CActiveScheduler::Add( this );
	}
	
// ---------------------------------------------------------------------------------
// CFilesSearcher::ConstructL() 
// 2nd phase constructor
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::ConstructL( const RArray<TUid>& /*aContentIdArray*/, 
		                         const CSearchCondition& aCondition,
		                         MSearchPluginObserver& aObserver ) 
	{
	iObserver  = &aObserver;
	//SetPriority( EPriorityStandard );
	//Create a text searcher
	if(iFs.Connect() != KErrNone)
	{
	   return;
	}   
	iTextSearcher = CSearchTextSearcher::NewL( *this );
	iTextSearcher->SetParametersL( aCondition );
	iTotalHits = 0;
	//Read the process drive letter
	TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
	TBuf<KMaxFileName> resourceFileName;
	RResourceFile resourceFile;
	
	//Read the path of the resource file
	resourceFileName.Copy( driveLetter );
	resourceFileName.Append( KDC_RESOURCE_FILES_DIR );
	_LIT( KResFilename, "avkon.rsc" );
    resourceFileName.Append( KResFilename );
    
    //Open the resource file
    BaflUtils::NearestLanguageFile( iFs, resourceFileName );
    resourceFile.OpenL( iFs, resourceFileName );
  	resourceFile.ConfirmSignatureL(0);
	
	//Read "KB" localized string
	HBufC8* sizeKB = resourceFile.AllocReadLC( R_QTN_SIZE_KB );
	
	const TPtrC16 ptrReadBuffer( ( TText16* ) sizeKB->Ptr(),
								( sizeKB->Length()+1 )>>1 );
								
	TBufC8<KBufferLength> tempString;
	
//	TPtr8 tempPtr8( tempString.Des() );
	
//	CnvUtfConverter::ConvertFromUnicodeToUtf8( tempPtr8,
//												ptrReadBuffer );
	iSizeKB = ptrReadBuffer.AllocL();
	
	CleanupStack::PopAndDestroy( sizeKB );
//-----------------------------------------------------------------------	
//	tempPtr8.FillZ();
	
	//Read "B" localized string
	HBufC8* sizeB = resourceFile.AllocReadLC( R_QTN_SIZE_B );
	
	const TPtrC16 ptrReadBufferB( ( TText16* ) sizeB->Ptr(),
								( sizeB->Length()+1 )>>1 );
								
	
//	CnvUtfConverter::ConvertFromUnicodeToUtf8( tempPtr8,
//												ptrReadBufferB );
	iSizeB = ptrReadBufferB.AllocL();
	
	CleanupStack::PopAndDestroy( sizeB );
	
//	tempPtr8.FillZ();
//-----------------------------------------------------------------------	
	//Read "MB" localized string
	HBufC8* sizeMB = resourceFile.AllocReadLC( R_QTN_SIZE_MB );
	
	const TPtrC16 ptrReadBufferMB( ( TText16* ) sizeMB->Ptr(),
								( sizeMB->Length()+1 )>>1 );
									
//	CnvUtfConverter::ConvertFromUnicodeToUtf8( tempPtr8,
//												ptrReadBufferMB );
	iSizeMB = ptrReadBufferMB.AllocL();
	
	CleanupStack::PopAndDestroy( sizeMB );
	
	iFs.DriveList( iDriveList );
	
	resourceFile.Close();
	}
	
// ---------------------------------------------------------------------------------
// CFilesSearcher::ReportFinishedL
// Called when search is finished
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::ReportFinishedL()
    {
	iObserver->SearchCompletedL( KErrNone, iCurrentItemCount );
    }

// ---------------------------------------------------------------------------------
// CFilesSearcher::CleanUp() 
// Cleanup the resources held
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::CleanUp() 
	{	
	
	if ( iFiles )
		{
		delete iFiles;
		iFiles = NULL;
		}
	if ( iFolder ) 
		{
		delete iFolder;
		iFolder = NULL;
		}
	
	if ( iFileName ) 
		{
		delete iFileName;
		iFileName = NULL;
		}
	}
	
// ---------------------------------------------------------------------------------
// CFilesSearcher::IsSystemPath() 
// If path is system dir/file returns TRUE
// ---------------------------------------------------------------------------------	
//
TBool CFilesSearcher::IsSystemPath( TDesC& aPath )
    {

		return ! (
		KErrNotFound == aPath.Find( KDC_RESOURCE_FILES_DIR ) &&
		KErrNotFound == aPath.Find( KSkipPrivate ) && 
		KErrNotFound == aPath.Find( KSkipSys ) && 
		KErrNotFound == aPath.Find( KSkipRta ) &&
		KErrNotFound == aPath.Find( KSkipSystem ) &&
		KErrNotFound == aPath.Find( KSkipSysTest ) && 
		KErrNotFound == aPath.Find( KSkipTef ) &&
		KErrNotFound == aPath.Find( KSkipSmokeTest ) && 
		KErrNotFound == aPath.Find( KSkipResource ) && 
		KErrNotFound == aPath.Find( KSkipResourceDir ) &&
		KErrNotFound == aPath.Find( KSkipNokia ) && 
		KErrNotFound == aPath.Find( KSkipCDriveTFW ) &&
		KErrNotFound == aPath.Find( KSkipCDrivePredic ) &&
		KErrNotFound == aPath.Find( KSkipCNokia ) &&
		KErrNotFound == aPath.Find( KSkipCDriveLogs )  ); 
			
    }	
    
// ---------------------------------------------------------------------------------
// CFilesSearcher::GetFileNameArrayL
// Searches through all files in a given folder and it's subfolders recursively.
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::GetFileNameArrayL( 
    const TDesC& aFolder ) 
	{	
    if ( iFolder ) 
        {
	    delete iFolder;
    	iFolder = NULL;
        }
    iFolder = HBufC::NewL( KMaxFileName );
	iFolder->Des().Append( aFolder );
	
	iCurrentItemCount++;
	
	if ( BaflUtils::PathExists( iFs, aFolder ) )
		{		
		if ( iFiles )
			{
			delete iFiles;
			iFiles = NULL;
			}
		CDir* folders = NULL;
		User::LeaveIfError( iFs.GetDir( aFolder, KEntryAttNormal, 
			ESortByName, iFiles, folders ) );
		CleanupStack::PushL( folders );
		
		//Go through all the files in this folder
		for ( TInt i=0; i < iFiles->Count(); i++ )
			{						
			iFileName->Des().Zero();
			iFileName->Des().Append( iFiles->operator[](i).iName );
			iTextSearcher->Cleanup();
			TBool fileInCDrive = EFalse;
			TBool isFolder = EFalse;
			
			BaflUtils::IsFolder( iFs, *iFileName, isFolder );
			
			if ( 0 == iFileName->Find( KSkipCDrive ) && !isFolder ) 
				{
				fileInCDrive = ETrue;
				}
			if ( !fileInCDrive ) 
				{
				iTextSearcher->SearchL( *iFileName ); 
				}
	    	
    		}    			
		
		TInt folderCount = folders->Count();
		
		//Recursively search in the sub-folders 
		for ( TInt j=0; j < folderCount ; j++ )
			{
		    iFileName->Des().Zero();
			iFileName->Des().Append(folders->operator[](j).iName);
	    	
	    	/* //Since its a folder dont search for it..
	    	
		    	iTextSearcher->Cleanup();
    	    	iTextSearcher->SearchL( *iFileName );
	    	*/
			
			HBufC* tempBuf = HBufC::NewLC( KMaxFileName );
			TPtr fileName( tempBuf->Des() );
			fileName.Copy( aFolder );
			fileName.Append( folders->operator[](j).iName );
			_LIT ( KBackSlash, "\\" ); 
			fileName.Append( KBackSlash );
			
			/* Do not search system folders under any circumstances. */
			
			if ( !IsSystemPath( fileName ) )
			    {			        
			    GetFileNameArrayL( fileName );
			    }
			CleanupStack::PopAndDestroy( tempBuf );
			}

		CleanupStack::PopAndDestroy( folders );
		}
	
	}

// ---------------------------------------------------------------------------------
// CFileSearcher::DoActualSearchL()
// Does the search for the file.  Called from RunL.  In one RunL call
// a limited number of file names are searched.
// Return:  ETrue: if more note items need to be searched. 
//         EFalse: otherwise.
// ---------------------------------------------------------------------------------
//
TBool CFilesSearcher::DoActualSearchL()
	{

    // Get list of all files to be searched
    // Search is divided for better responsiveness
    
    while( ETrue ) 
	    {	
    	TChar driveLetter1 = iDriveList[iCurrentDriveIndex];
    	//If the Drive is present 
    	if ( iDriveList[iCurrentDriveIndex] ) 
    	    {
    		//Check for the BAD DRIVE
    		if ( iCurrentDriveIndex < KMaxDrives ) 
    		    {
    			//Get the drive name
    			TDriveUnit xyz( iCurrentDriveIndex );
    			TDriveName let = xyz.Name();

				//Path variable    			    			
    			HBufC* pathList = HBufC::NewLC( KMaxFileName );
        		pathList->Des().Append( let );
        		_LIT ( KBackSlash, "\\" ); 
        		pathList->Des().Append( KBackSlash );	
        		
        		//Search recursively all the folders and 
        		//subfolders in the drive
		        GetFileNameArrayL( *pathList );
        		CleanupStack::PopAndDestroy(pathList);	
        		
        		iCurrentDriveIndex++;
        		if ( KMaxDrives <= iCurrentDriveIndex ) 
	 		   	    {
	   				return EFalse; //If have searched all the drives
	    		    }
        		return ETrue;
    		    }
    		
    	    }
    	else 
    	    {
    		iCurrentDriveIndex++;
	    	if ( KMaxDrives <= iCurrentDriveIndex ) 
	    	    {
	    		return EFalse; //If have searched all the drives
	    	    }	
    	    }
    	
	    }
	}

// ---------------------------------------------------------------------------------
// CFilesSearcher::DoCancel() 
// Called by ECOM framework
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::DoCancel() 
	{
    CleanUp();
	}


// ---------------------------------------------------------------------------------
// CFilesSearcher::RunL
// Called by ECOM framework
// ---------------------------------------------------------------------------------
//
void CFilesSearcher::RunL() 
    {
    TBool continueSearch = EFalse;
    
    TRAPD( err, continueSearch = DoActualSearchL() );
    if ( err != KErrNone ) 
    	{
    	iObserver->SearchCompletedL( err, iCurrentItemCount );
    	}
    else 
   		{
   	 	if ( continueSearch )
    		{
        	TRequestStatus* status = &iStatus;
        	User::RequestComplete( status, KErrNone );
			SetActive();
    		}
		else 
	    	{
			ReportFinishedL();
    		}
    	}
	
    }


// ---------------------------------------------------------------------------------
// CFilesSearcher::RunError
// Called by ECOM framework
// ---------------------------------------------------------------------------------
//
TInt CFilesSearcher::RunError() 
    {
	return KErrNone;
    }

