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
* Description:     Used in creating the folders for the content classes and the corresponding results. 
*
*/






// User Includes
#include "srchuiresfolder.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CSrchUiResFolder::NewL
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CSrchUiResFolder* CSrchUiResFolder::NewL()
	{
	CSrchUiResFolder* self = CSrchUiResFolder::NewLC();
    CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSrchUiResFolder::NewLC
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//		
EXPORT_C CSrchUiResFolder* CSrchUiResFolder::NewLC()
	{
	CSrchUiResFolder* self = new( ELeave )CSrchUiResFolder();
	CleanupStack::PushL( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolder::NewL
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSrchUiResFolder* CSrchUiResFolder::NewL(
                                       CSrchUiResFolder* aParentFolder )
	{
	CSrchUiResFolder* self = CSrchUiResFolder::NewLC( aParentFolder );
    CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSrchUiResFolder::NewLC
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//	
EXPORT_C CSrchUiResFolder* CSrchUiResFolder::NewLC(
                                         CSrchUiResFolder* aParentFolder )
	{
	CSrchUiResFolder* self = new( ELeave )CSrchUiResFolder( aParentFolder );
	CleanupStack::PushL( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolder::~CSearchModel
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSrchUiResFolder::~CSrchUiResFolder()
    {
    delete iName;
    //delete iFolders;
    iFolders.ResetAndDestroy();
    iLightResults.Close();
    iHeavyResults.Close();
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetFolderName
// Getter for the folder name.
// -----------------------------------------------------------------------------
//    
const TDesC& CSrchUiResFolder::GetFolderName() const
    {
    return *iName;    
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::SetFolderNameL
// Setter for the folder name.
// -----------------------------------------------------------------------------
//        
void CSrchUiResFolder::SetFolderNameL( const TDesC& aName )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetSubFolder
// Returns the subfolder, knowing the (child) folder index.
// -----------------------------------------------------------------------------
//        
MSrchUiFolder* CSrchUiResFolder::GetSubFolder( TInt aIndex )
    {
        return iFolders[aIndex];
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetSubFolderIndex
// Returns the subfolder index knowing the (child) subfolder pointer.
// -----------------------------------------------------------------------------
//        
TInt CSrchUiResFolder::GetSubFolderIndex( MSrchUiFolder* aFolder )
    {
	for ( TInt i(0); i<iFolders.Count(); i++ )
		{
		if ( iFolders[i] == aFolder )
			{
			    return i;
			}
		}
	return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetParentFolder
// Getter for the parent folder.
// -----------------------------------------------------------------------------
//        
MSrchUiFolder* CSrchUiResFolder::GetParentFolder()
    {
    return iParentFolder;
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetFile
// Retuns the file, knowing the (child) file index.
// -----------------------------------------------------------------------------
//        
EXPORT_C CSearchLightResult* CSrchUiResFolder::GetResult( TInt aIndex )
    {
		if(aIndex<iLightResults.Count())
		{
			return iLightResults[ aIndex ];
		}
		else
		{
			return(NULL);
		}
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetSubFoldersCount
// Retuns the number of (child) subfolders.
// -----------------------------------------------------------------------------
//        
TInt CSrchUiResFolder::GetSubFoldersCount() const
    {
    return iFolders.Count();
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetAllSubFoldersCount
// Returns the number of ALL (child) subfolders.
// This means that folders inside folders are added to the count.
// -----------------------------------------------------------------------------
//        
TInt CSrchUiResFolder::GetAllSubFoldersCount() const
    {
        //Need to be changed        
    return iFolders.Count();
    }


// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetResultsCount
// Retuns the number of (child) Results.
// -----------------------------------------------------------------------------
//        
TInt CSrchUiResFolder::GetResultsCount() const
    {
    return iLightResults.Count();
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetAllResultsCount
// Retuns the number of ALL (child) Results.
// This means that Results inside folders are added to the count.
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt CSrchUiResFolder::GetAllResultsCount() const
    {
    if( !( iFolders.Count() ) ) 
    {
    	return iLightResults.Count();
    }
    
    TInt count = 0;
    
    for( TInt i = 0; i < iFolders.Count(); i++ ) 
    {
    	CSrchUiResFolder* temp = iFolders[i];
    	count += temp->GetAllResultsCount();
	    	
    }
    return count;
    }


// -----------------------------------------------------------------------------
// CSrchUiResFolder::SubFolders
// Getter for subfolders
// -----------------------------------------------------------------------------
//    
EXPORT_C  void CSrchUiResFolder::SubFolders(
                                   RPointerArray<CSrchUiResFolder>& aFolders )
    {
     
     for ( TInt i = 0; i < iFolders.Count(); i++ ) 
     	{
     	aFolders.Append( iFolders[i] );
     	}
    }


// -----------------------------------------------------------------------------
// CSrchUiResFolder::Results
// Getter for filess
// -----------------------------------------------------------------------------
//        
EXPORT_C RPointerArray<CSearchLightResult>& CSrchUiResFolder::Results()
    {
    //This is a temporary solution as in CS View results of
    // a particular folder can shown
    if( iFolders.Count() ) 
    {
    	iLightResults.Reset();
    }
    for( TInt i = 0; i < iFolders.Count(); i++ ) 
    {
    	CSrchUiResFolder* temp = iFolders[i];
    	RPointerArray<CSearchLightResult> results = temp->Results();
    	for( TInt j = 0; j < results.Count(); j++ )
    	{
    		iLightResults.Append( results[j] );
    	}
    }
    return iLightResults;
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::HeavyResults
// Getter for filess
// -----------------------------------------------------------------------------
//        
EXPORT_C RPointerArray<CSearchResult>& CSrchUiResFolder::HeavyResults()
    {
    return iHeavyResults;
    }
    
// -----------------------------------------------------------------------------
// CSrchUiResFolder::SetFolders
// Setter for subfolders
// -----------------------------------------------------------------------------
//        
EXPORT_C void CSrchUiResFolder::AddChildL( CSrchUiResFolder* aFolders )
    {
        iFolders.AppendL(aFolders);
    }
// -----------------------------------------------------------------------------
// CSrchUiResFolder::SetFiles
// Setter for files
// -----------------------------------------------------------------------------
//        
EXPORT_C void CSrchUiResFolder::AddChildFileL( CSearchLightResult* aResults )
    {
        iLightResults.AppendL(aResults);
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::SetContentType
// Setter for ContentType
// -----------------------------------------------------------------------------
//    
EXPORT_C void CSrchUiResFolder::SetContentClassType( const TUid& aContentType )
	{
	iContentClass.iUid = aContentType.iUid;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolder::ContentType
// Getter for ContentType
// -----------------------------------------------------------------------------
//    
EXPORT_C const TUid& CSrchUiResFolder::ContentClassType() const 
	{
	return iContentClass;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolder::SetContentType
// Setter for ContentType
// -----------------------------------------------------------------------------
//    
EXPORT_C void CSrchUiResFolder::SetContentFolderType( const TUid& aContentType )
	{
	iContentFolder.iUid = aContentType.iUid;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolder::ContentType
// Getter for ContentType
// -----------------------------------------------------------------------------
//    
EXPORT_C const TUid& CSrchUiResFolder::ContentFolderType() const 
	{
	return iContentFolder;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolder::ResetAndDestroy
// Reset subfolders and Results
// -----------------------------------------------------------------------------
//    
EXPORT_C void CSrchUiResFolder::ResetAndDestroy()
    {
    iFolders.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::SetIconInfo
// Sets the Icon Info
// -----------------------------------------------------------------------------
//
EXPORT_C void CSrchUiResFolder::SetIconInfo( const CSearchIconInfo* aIconInfo )
{
    iIconInfo = aIconInfo;
}

// -----------------------------------------------------------------------------
// CSrchUiResFolder::IconInfo
// Retuns the icon info
// -----------------------------------------------------------------------------
//        
EXPORT_C const CSearchIconInfo* CSrchUiResFolder::IconInfo() const
{
    return(iIconInfo);
}

// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetHeavyResult
// Retuns the file, knowing the (child) file index.
// -----------------------------------------------------------------------------
//    
EXPORT_C CSearchResult* CSrchUiResFolder::GetHeavyResult( TInt aIndex )
	{
		if(aIndex<iHeavyResults.Count())
		{
			return(iHeavyResults[aIndex]);
		}
		else
		{
			return(NULL);
		}
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetHeavyResultsCount
// Retuns the number of heavy results
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSrchUiResFolder::GetHeavyResultsCount()
    {
    //If a there are no folders in class(e.g, file names) 
    //return the heavy result count 
    
    if( !( iFolders.Count() ) )
    {
    	return iHeavyResults.Count();
    }
    
    //Reset before dynamically constructing the heavy result from folder
    iHeavyResults.Reset();
    
    //e.g., calendar, messages
    for( TInt i = 0; i < iFolders.Count(); i++ ) 
    {
    	CSrchUiResFolder* temp = iFolders[i];
    	RPointerArray<CSearchResult> results = temp->HeavyResults();
    	for( TInt j = 0; j < results.Count(); j++ )
    	{
    		iHeavyResults.Append( results[j] );
    	}
    }
    return iHeavyResults.Count();
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::ResetAllFolders
// Reset all folders
// -----------------------------------------------------------------------------
//
EXPORT_C void CSrchUiResFolder::ResetAllFolders() 
	{
		//Resets the heavy result of the folders.. 
		//So that results wont get appended 
		//Its a recursive call
		if( 0 == iFolders.Count() )
		{
			//Termination part
			iHeavyResults.Reset();
			return;
		}
		for( TInt i = 0; i < iFolders.Count(); i++ ) 
	    {
	    	//Navigation part
	    	CSrchUiResFolder* temp = iFolders[i];
	    	temp->ResetAllFolders();
		    	
	    }
	    
		iHeavyResults.Reset();
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolder::GetAllHeavyResultsCount
// Retuns the number of heavy results
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSrchUiResFolder::GetAllHeavyResultsCount() const
     {
    return iHeavyResults.Count();
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::CSrchUiResFolder
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//
CSrchUiResFolder::CSrchUiResFolder()
    {
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolder::CSrchUiResFolder
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//    
CSrchUiResFolder::CSrchUiResFolder( CSrchUiResFolder* aParentFolder )
    {
    iParentFolder = aParentFolder;
    }
 
// -----------------------------------------------------------------------------
// CSrchUiResFolder::ConstructL
// Performs the second phase of two phase construction.
// -----------------------------------------------------------------------------
//   
void CSrchUiResFolder::ConstructL()
    {
	//for later use..if needed    
    }

// End of File

