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
* Description:   Plugin for contactsplugin information search.
*
*/


//SYSTEM INCLUDES
#include <utf.h>
#include <e32std.h>
#include <searchcondition.h>
#include <searchtextsearcher.h>
#include <searchdocumentid.h>
#include <searchlightresult.h>
#include <searchresult.h>
#include <searchcommon.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactFieldIterator.h>
#include <CVPbkFieldTypeRefsList.h>
#include <CVPbkContactLinkArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactLink.h>
#include <VPbkEng.rsg>
#include <VPbkContactStoreUris.h>
#include <TVPbkWordParserCallbackParam.h>
#include <TVPbkContactStoreUriPtr.h>
#include <searchpluginobserver.h>

#include <MVPbkContactViewBase.h>
#include <MVPbkContactStoreInfo.h>
#include <CVPbkContactViewDefinition.h>
#include <CVPbkSortOrder.h>
#include <MVPbkContactViewFiltering.h>
#include <CPbk2SortOrderManager.h>
#include <barsc.h>
#include <barsread.h>
#include <contactssearchresource.rsg>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
//USER INCLUDES
#include "contactssearcher.h"
#include "contactssearchplugindefines.h"
#include <featmgr.h>
//FORWARD DECLARATION
TInt VPbkParseDataL( TAny* aParam );


// ---------------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------------
//
CContactsSearcher* CContactsSearcher::NewL( const RArray<TUid>& aContentIdArray, 
                                const CSearchCondition& aCondition, 
                                const TUid& aPluginId,
                                MSearchPluginObserver& aObserver )
    {
	CContactsSearcher* self = new ( ELeave ) CContactsSearcher( aPluginId );
	CleanupStack::PushL( self );
	self->ConstructL( aContentIdArray, aCondition,aObserver );
	CleanupStack::Pop( self );
	return self;
    }


// ---------------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------------
//
CContactsSearcher::~CContactsSearcher() 
    {
    delete iFilterView;
    delete iContactViewBase;
	
	ifSession.Close();
	if( iCurrentlyOpenStores)
        {	
        // Close all the stores opened
        if(iContactManager)
        {
            TRAP_IGNORE(iContactManager->ContactStoresL().CloseAll( *this ));
        }
        if(iCurrentlyOpenStores)
        {
            const TInt count = iCurrentlyOpenStores->Count();
            for ( TInt i = count - 1; i >= 0; --i )
                {
                iCurrentlyOpenStores->Remove( (*iCurrentlyOpenStores)[i] );
                }
           delete iCurrentlyOpenStores;
		       iCurrentlyOpenStores = NULL;
        	
        }
        }
    if ( iContactManager )
        {
 		delete iContactManager;
		iContactManager = NULL;	
        }

    if ( iTextSearcher )
        {
 		delete iTextSearcher;
		iTextSearcher = NULL;	
        }
        
   
    
   
    if(iAllContactItemsFromDb)
    {
        iAllContactItemsFromDb->ResetAndDestroy();   	
    	delete iAllContactItemsFromDb;
    	iAllContactItemsFromDb = NULL;
    }
    iContactArrayMapper.Reset();
    iCotnactDeletedList.Reset();
   	iHeavyResultsArray.ResetAndDestroy();
	
    FeatureManager::UnInitializeLib();
	
}


// ---------------------------------------------------------------------------
// A function that returns the contact link at given index of the iContactLinkArray
// ---------------------------------------------------------------------------
// 
//
MVPbkContactLink* CContactsSearcher::GetContactLinkAtIndexLC(TInt aIndex)
    {
    if(aIndex < iContactArrayMapper.Count()  && (EFalse == iCotnactDeletedList[aIndex] ))
        {
        if ( iAllContactItemsFromDb ) 
        	{
        	return iAllContactItemsFromDb->At(iContactArrayMapper[aIndex]).CloneLC();
        	}
        }
    else
        {
  	    return NULL;
        }

return NULL;
}

// ---------------------------------------------------------------------------------
// Releases the resources
// ---------------------------------------------------------------------------------
//
void CContactsSearcher::Destroy() 
    {
	if( IsActive() ) 
	    {
		CActive::Cancel();
	    }
	 else
	    {
	    CleanUp(); 	
	    }   
	    delete this; 
}

// ---------------------------------------------------------------------------------
// Starts the search
// ---------------------------------------------------------------------------------
//
void CContactsSearcher::SearchL()
    { 
    iSearchCancelled = EFalse;

    
   	iHeavyResultsArray.ResetAndDestroy();
   //	iContactLinkIndexArray.Reset();
   
    if(iAllContactItemsFromDb)
    {
     
	   iAllContactItemsFromDb->ResetAndDestroy();   
    }
    iContactManager->ContactStoresL().OpenAllL( *this );
   
    }

// ---------------------------------------------------------------------------------
// Returns true,f search is in progress
// ---------------------------------------------------------------------------------
//
TBool CContactsSearcher::IsSearching() 
    {
	return IsActive();
    }

// ---------------------------------------------------------------------------------
// Cancels the search
// ---------------------------------------------------------------------------------
//
void CContactsSearcher::CancelSearch() 
    {
    iSearchCancelled = ETrue;
	CActive::Cancel();
	
    }

// ---------------------------------------------------------------------------------
// Gets the results 
// ---------------------------------------------------------------------------------
//
void CContactsSearcher::GetResultsL( 
	const RPointerArray<CSearchDocumentId>& aDocumentIdArray )
    {
	
    //Check the aDocumentIdArray
    // For all the matching documentIds, fetch the corresponding
    // Heavy Result from iHeavyResultsArray and append it to heavyResultResponse
	
	RPointerArray<CSearchResult> heavyResultResponse; 
	//Search through the aDocumentIdArray, and select those heavy results
	// which belong to this plugin with the corresponding document id
	for( TInt j = 0; j < aDocumentIdArray.Count(); j++ ) 
	    {
	    TUid pluginId;
	    pluginId.iUid = aDocumentIdArray[j]->PluginId().iUid;
	    if ( (  pluginId.iUid == iPluginId.iUid ) &&
	         (aDocumentIdArray[j]->RepositoryId() == KSearchCClassContactsUid.iUid)
	       )  
	        {
	        
            TInt myArrayIndex =aDocumentIdArray[j]->DocumentId()  ;

	        heavyResultResponse.AppendL(iHeavyResultsArray[myArrayIndex]);        

	        }             
	    }
		
	iObserver->ResultsRetrieveCompleteL( heavyResultResponse );
	heavyResultResponse.Reset();
    }

// ---------------------------------------------------------------------------------
// Cancels the result retrieval
// ---------------------------------------------------------------------------------
//
void CContactsSearcher::CancelResultsRetrieve() 
    {
	
    }


//---------------------------------------------------------------------------------
// Returns the search progress
//---------------------------------------------------------------------------------
//
void CContactsSearcher::GetSearchProgressL( TUid& /*aContentClassId*/, TInt& /*aCurrentDocument*/, TInt& /*aTotalDocuments*/ )
    {
	
    }



// ---------------------------------------------------------------------------------
// Called when a result is found matching the search criteria
// ---------------------------------------------------------------------------------
//
void CContactsSearcher::HitL( TInt /*aResult*/ ) 
    {
	// If search is still in progress, then process the search result
	if( !iSearchCancelled ) 
	    {
		CSearchDocumentId* searchDocId = NULL;
		
		TBuf8<KBufferLength> tempDes; 
		
	    
	    // Set the repository id
		searchDocId = CSearchDocumentId::NewL( iTotalHits,iPluginId );
		searchDocId->SetRepositoryIdL( KSearchCClassContactsUid.iUid );
		CSearchLightResult* searchLightResult = CSearchLightResult::NewL( searchDocId );
        CleanupStack::PushL(searchLightResult);		
		searchLightResult->SetContentClassId( KSearchCClassContactsUid );
		searchLightResult->SetServiceId( KNullUid );
		
		 //HBufC8 *repositoryId = HBufC8::NewL( KBufferLength ) ;
         //repositoryId->Des().Append(KSpace);
	     searchDocId->SetRepositoryIdL( KSearchCClassContactsUid.iUid );
         //delete repositoryId; 
         //repositoryId = NULL;
	
		iContactArrayMapper.Append(iCurrentDatatBaseItemIndex - 1);
		
	    // Form the heavy result and append it to the iHeavyResultsArray
	    CSearchResult* tmpHeavyRes = NULL;
	    tmpHeavyRes = CSearchResult::NewL(searchLightResult);
	    CleanupStack::PushL(tmpHeavyRes);
	    //Set the Title for the heavy result
	    if ( iCurrentTitle )
	        {
		    tmpHeavyRes->SetTitleL( *iCurrentTitle );
	        delete iCurrentTitle; iCurrentTitle = NULL;
	        }
	    
	    //Set the snippet for the heavy result
	    if ( iCurrentSnippet )
	        {
		    tmpHeavyRes->SetSnippetL(*iCurrentSnippet);
	        delete iCurrentSnippet;iCurrentSnippet = NULL;
	        }
	    
	    // Check if the contact is a phone contact or a sim contact
	    // Accordingly set the folder id
	    
	    TInt tempIndex = iContactArrayMapper[iTotalHits];
	    TVPbkContactStoreUriPtr uri = iAllContactItemsFromDb->At(tempIndex).ContactStore().StoreProperties().Uri();       
	    TVPbkContactStoreUriPtr phoneMemoryUri( VPbkContactStoreUris::DefaultCntDbUri() );
 
        if ( uri.Compare( phoneMemoryUri,TVPbkContactStoreUriPtr::EContactStoreUriStoreType) == 0 )
            {
             searchLightResult->SetContentFolderId( KSearchCFolderPhoneUid );  
            }
        else
            {
         
			searchLightResult->SetContentFolderId( KSearchCFolderSIMUid );         	
            }
	    //Set the SetSnippetFormatL
	    HBufC8 *snippetFormat = HBufC8::NewL( KBufferLength ) ;
	    snippetFormat->Des().Append(KSpace);
		tmpHeavyRes->SetSnippetFormatL( snippetFormat->Des() );
		delete snippetFormat; 
		snippetFormat = NULL;
	    
	    //Append the heavy result to the iHeavyResultsArray
	    iHeavyResultsArray.AppendL(tmpHeavyRes);
				
		// Increment the iTotalHits and call ResultFoundL()
		iTotalHits++;              
	    iObserver->ResultFoundL( searchLightResult, iTotalHits, iTotalNumOfContactsSearched );


	   CleanupStack::Pop(tmpHeavyRes); 
	   CleanupStack::Pop(searchLightResult); 
	   

	   }
	
}

HBufC8* CContactsSearcher::LaunchInfoL( const CSearchDocumentId& aDocumentID )
{
     if((aDocumentID.PluginId() == iPluginId) &&
       (aDocumentID.RepositoryId() == KSearchCClassContactsUid.iUid))
         {
           //HBufC *temp = HBufC::NewL(KMaxFileName) ;
           //TPtr docIdPtr = temp->Des();
								
           TInt myArrayIndex = aDocumentID.DocumentId();
		   //CnvUtfConverter::ConvertToUnicodeFromUtf8( docIdPtr,aDocumentID.DocumentId() );
		   //TLex16 myDocId(docIdPtr);
	       if(myArrayIndex < iContactArrayMapper.Count()  && (EFalse == iCotnactDeletedList[myArrayIndex] ))
	    	   {
                 if ( iAllContactItemsFromDb ) 
                	 {
	        	     MVPbkContactLink* launchContact = iAllContactItemsFromDb->At(iContactArrayMapper[myArrayIndex]).CloneLC();
	                 
	                 HBufC8 *launchInfo = launchContact->PackLC();
	                 CleanupStack::Pop();
	                 CleanupStack::Pop(); 
	                 delete launchContact;
	                 launchContact= NULL;
	                 return launchInfo;
	                 }
	    	   }
	       
           else
        	   {
        	   return NULL;
        	   }
         
         }
     
     return NULL;
     
	
}
// ---------------------------------------------------------------------------
// From class MVPbkContactStoreListObserver.
// ---------------------------------------------------------------------------
//
void CContactsSearcher::OpenComplete()
    {
     // If atleast one store is ready, 
     //       set iStoreReadyForAccessing = ETrue so that we can start the serach    
     // else
     //       Finish the search as no stores are available 
     if(iAtLeastOneStoreReady)
        {
        iStoreReadyForAccessing = ETrue;
        TRequestStatus* status = &iStatus;
    	User::RequestComplete( status, KErrNone );
    	SetActive();
    
        }
    else
        {
        //No stores available, so set the appropriate flag
        iNoContactStoreAvailable = ETrue;	
        }

  
    }
    
// ---------------------------------------------------------------------------
// From class MVPbkContactStoreObserver.
// ---------------------------------------------------------------------------
//
void CContactsSearcher::StoreReady(MVPbkContactStore& aStore)
    {
    // This function is called every time a store is ready to be 
    // accessed.
    const TDesC& uri = aStore.StoreProperties().Uri().UriDes();
    if ( !iCurrentlyOpenStores->IsIncluded( uri ) )
        {
        TRAP_IGNORE(iCurrentlyOpenStores->AppendL( uri ));
        }
    // We know that atleast one store is ready, so we can set this flag to ETrue
    iAtLeastOneStoreReady = ETrue;
     
    
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreObserver.
// ---------------------------------------------------------------------------
//
void CContactsSearcher::StoreUnavailable(
        MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/)
    {
        //The store is not availble..so do notthing....
        //TRequestStatus* status = &iStatus;
        //User::RequestComplete( status, KErrNone );
     	//SetActive();
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreObserver.
// ---------------------------------------------------------------------------
//
void CContactsSearcher::HandleStoreEventL(MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent)
    {
    
    //if there is deletetion of contact, then we need to update
    MVPbkContactStore* storeInList = iContactManager->ContactStoresL().Find(aContactStore.StoreProperties().Uri());
	  TInt contactLocation = 0;
	if (storeInList !=NULL)
	{
		switch(aStoreEvent.iEventType)
		{
			case TVPbkContactStoreEvent::EContactDeleted:
			contactLocation =  iAllContactItemsFromDb->Find(*aStoreEvent.iContactLink);	
			if( KErrNotFound != contactLocation)
			{
				TInt tempLoc = iContactArrayMapper.Find(contactLocation);
				if( KErrNotFound != tempLoc)
				{
					iCotnactDeletedList[tempLoc] = ETrue;
				}
			}
			break;
			
			default:
				break;
		}
			
	}
      // Dont't show store events
     TRequestStatus* status = &iStatus;
     User::RequestComplete( status, KErrNone );
     SetActive();
    }

// ---------------------------------------------------------------------------
//  Callback Method.Called when one Retrieve operation is complete
// ---------------------------------------------------------------------------
//   
void CContactsSearcher::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/, MVPbkStoreContact* aContact)
    {

    delete iVPbkOperation;
    iVPbkOperation = NULL;
    
    // Delete the retrieved link
    //iAllContactItemsFromDb->Delete( iCurrentDatatBaseItemIndex );
    
    // Handle the fetched contact....
    TRAPD( res, HandleRetrievedContactL( aContact ) );
    if ( res != KErrNone )
        {
    
        }
    
    if(iAllContactsSearched)
        {
        TRequestStatus* status = &iStatus;
	    User::RequestComplete( status, KErrNone );
	    SetActive();
	   
        }
        
    }
    
// ---------------------------------------------------------------------------
//  Callback Method.Called when one Retrieve operation fails.
// ---------------------------------------------------------------------------
//   
void CContactsSearcher::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, TInt /*aError*/ )
    {
    //delete the operation, since its completed
    delete iVPbkOperation;
    iVPbkOperation = NULL;
    
    }



// ---------------------------------------------------------------------------------
// Implements cancellation of an outstanding request.
// ---------------------------------------------------------------------------------
//
void CContactsSearcher::DoCancel() 
    {
	
    }


// ---------------------------------------------------------------------------------
// The function is called by the active scheduler 
// ---------------------------------------------------------------------------------
//
void CContactsSearcher::RunL() 
    {
    
	if(  !DoActualSearchL() )
	    {      
	    ReportFinishedL();   
    	
	    }
	
    }

// ---------------------------------------------------------------------------------
// Called in case of any errros 
// ---------------------------------------------------------------------------------
//
TInt CContactsSearcher::RunError(TInt /*aError*/) 
    {
	return KErrNone;
    }

// ---------------------------------------------------------------------------------
// CContactsSearcher::CContactsSearcher() 
// Constructor
// ---------------------------------------------------------------------------------
//
CContactsSearcher::CContactsSearcher( const TUid& aPluginId ) :
	CActive( CActive::EPriorityStandard ), 
	iPluginId( aPluginId ),
	iTotalNumOfContactsSearched(0),
	iTotalHits(0),
	iStoreReadyForAccessing(EFalse),
    iAtLeastOneStoreReady(EFalse),
    iAllContactsSearched (EFalse),
    iNoContactStoreAvailable (EFalse),
    iSearchStarted (EFalse),
    iSearchCancelled(EFalse)
    {
	CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------------
// CContactsSearcher::ConstructL() 
// 2nd phase constructor
// ---------------------------------------------------------------------------------
//
void CContactsSearcher::ConstructL( const RArray<TUid>& /*aContentIdArray*/, 
		                             const CSearchCondition& aCondition,
		                             MSearchPluginObserver& aObserver ) 
    {

    iObserver = &aObserver;
	User::LeaveIfError( ifSession.Connect() );

	// initialize virtual phonebook with phonebook and sim contact uris.
	CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();
	uriArray->AppendL( TVPbkContactStoreUriPtr(VPbkContactStoreUris::DefaultCntDbUri()));
    uriArray->AppendL( TVPbkContactStoreUriPtr( VPbkContactStoreUris::SimGlobalAdnUri() ) );
    
    
    iContactManager = CVPbkContactManager::NewL(*uriArray);
    CleanupStack::PopAndDestroy(uriArray); 
    
    // Create the container for currently open stores
    iCurrentlyOpenStores = CVPbkContactStoreUriArray::NewL();
     
    // create containers for holding the contacts data      
    iAllContactItemsFromDb = CVPbkContactLinkArray::NewL();
   
    
    //Create the text searcher
 	SetPriority( EPriorityStandard );
	iTextSearcher = CSearchTextSearcher::NewL( *this );
    iTextSearcher->SetParametersL( aCondition );
	
    iIsFilterView = EFalse;
    FeatureManager::InitializeLibL();
    }

// ---------------------------------------------------------------------------------
// Called when search is finished
// ---------------------------------------------------------------------------------
//
void CContactsSearcher::ReportFinishedL()
    {
        //Search is completed, call the appropriate function
        for( TInt j = 0; j < iHeavyResultsArray.Count(); j++ ) 
        {
        	
           iCotnactDeletedList.Append(EFalse);  
        }
		iObserver->SearchCompletedL( iTotalHits, iTotalNumOfContactsSearched );
       
    }

// ---------------------------------------------------------------------------------
// Will be called when the CActive::Cancel() is called.. by DoCancel()
// ---------------------------------------------------------------------------------
//
void CContactsSearcher::CleanUp() 
    {	
    //Cleanup the resources
    // delete all the data members owned.
    
    delete iCurrentTitle;
	iCurrentTitle = NULL;	
    
    delete iCurrentSnippet;
	iCurrentSnippet = NULL;	
    
    delete iContactsDataTobeSearched ;
	iContactsDataTobeSearched  = NULL;	
    
    delete iCurrentContactDataString;
	iCurrentContactDataString = NULL;	
    
     
    if( iAllContactItemsFromDb)
        {
    	delete iAllContactItemsFromDb;
    	iAllContactItemsFromDb = NULL ;
        }
    
    
    if(iFieldTypeRefList )
        {    
        iFieldTypeRefList->Reset();
    	delete iFieldTypeRefList;
		iFieldTypeRefList = NULL;	
        }
     delete iVPbkOperation;
	 iVPbkOperation = NULL;
	   
	 delete iSearchStrings;
	 iSearchStrings = NULL;
	   
	 delete iBufferForQuery;
	 iBufferForQuery = NULL;
    
   
    
    }


// ---------------------------------------------------------------------------------
// Does the search for the file.  Called from RunL. 
// Return:  ETrue: if more note items need to be searched. 
//          EFalse: otherwise (i.e if iAllContactsSearched is set to true)
// ---------------------------------------------------------------------------------
//
TBool CContactsSearcher::DoActualSearchL()
	{
	
	    if (iNoContactStoreAvailable || iAllContactsSearched )
	        { 
	        // Either store is not available, or all data has been searched
	        // so retrun EFalse
		    return EFalse; 
	        }
	
    	if(iStoreReadyForAccessing ) 
	        {   
            // Start the search if not yet started 	
	        if(!iSearchStarted)
	           {
	   	       iSearchStarted = ETrue;
		       FetchContactItemsFromDbAndSearchL();
	 	       }
	       
	        }
	    
	    return ETrue;

	}

// ---------------------------------------------------------------------------
// From class MVPbkContactFindObserver.
// When using the FindL, check that the field type of the find result is correct
// since the underlying db find isnt perfect, ex. uses SQL to optimize the find.
// Test application doesn't check the contact fields, it might be possible
// the a field whose type was not given in Find has been matched.
// ---------------------------------------------------------------------------
//

// ---------------------------------------------------------------------------
//  Fetches the contact items from the database.
// ---------------------------------------------------------------------------
//
void CContactsSearcher::FetchContactItemsFromDbAndSearchL(void)
    {
    // We just fire a find function with null value to get all the links
    // in the contact store. Once we get the links, we extract data from them 
    // to be searched, and then pass the same to the text searcher
    
	CPbk2SortOrderManager* sortOrderManager = CPbk2SortOrderManager::NewL( iContactManager->FieldTypes() );
	CleanupStack::PushL( sortOrderManager );
        
    CVPbkContactViewDefinition* viewDef = CVPbkContactViewDefinition::NewL();
    CleanupStack::PushL( viewDef );
    viewDef->SetType( EVPbkCompositeView );
         
    MVPbkContactStoreList& stores = iContactManager->ContactStoresL();
    TInt totalNumberOfContacts = 0;
    for ( TInt i(0); i < iCurrentlyOpenStores->Count(); ++i)
        {
   
        TInt numberOfContacts = stores.Find((*iCurrentlyOpenStores)[i])->StoreInfo().NumberOfContactsL();
        totalNumberOfContacts += numberOfContacts;
        if ( numberOfContacts != 0)
            {
            CVPbkContactViewDefinition* subViewDef = CVPbkContactViewDefinition::NewL();
            CleanupStack::PushL( subViewDef );
            subViewDef->SetUriL( (*iCurrentlyOpenStores)[i].UriDes() );
            viewDef->AddSubViewL( subViewDef );
            CleanupStack::Pop( subViewDef ); // Ownership changed.
            }
        }
    if (totalNumberOfContacts == 0)
        {
        iAllContactsSearched = ETrue;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    iContactViewBase = iContactManager->CreateContactViewLC( 
                   *this, *viewDef, sortOrderManager->SortOrder() );
                
    CleanupStack::Pop(); // CreateContactViewLC
    CleanupStack::PopAndDestroy( viewDef );
    CleanupStack::PopAndDestroy( sortOrderManager );
    }
    

    
// ---------------------------------------------------------------------------
// Handles the operations for a single contact after it is fetched
// ---------------------------------------------------------------------------
//   
void CContactsSearcher::HandleRetrievedContactL( 
        MVPbkStoreContact* aContact )
    {
    
    // Take the ownership
    aContact->PushL();
    
    MVPbkContactGroup* myContactGroup= aContact->Group();
    //The retrieved contact can be a contact item or a contact group, Handle accordingly
    if(NULL == myContactGroup)
        {
        // The fetched contact item (and not a contact group.)
        //Fetch all the editable data of the fetched contact
        // and pass it to the iTextSearcher for searching	
        GetDataForSingleContactL( *aContact );
        iTotalNumOfContactsSearched++;
        if(!iSearchCancelled)
            {
            iTextSearcher->SearchL( *iCurrentContactDataString );	
            }
         
        }
    else
        {
    
        }
    
    CleanupStack::PopAndDestroy(); // aContact
    if(iCurrentContactDataString)
       {
       delete iCurrentContactDataString;
       iCurrentContactDataString = NULL;
       }
   
   
    // Start reading next contact
    if ( iAllContactItemsFromDb)  
    	{	
    	if ( !iSearchCancelled && (iAllContactItemsFromDb->Count() > iCurrentDatatBaseItemIndex ))
	        {
	        const MVPbkContactLink& link = iAllContactItemsFromDb->At( iCurrentDatatBaseItemIndex );
	        iCurrentDatatBaseItemIndex++;
	        
	        iVPbkOperation = iContactManager->RetrieveContactL( link, *this );
	        
	        }
	   		else
	        {
	        // All the contacts have been read so set the iAllContactsSearched flag
	        iAllContactsSearched = ETrue;
	       
		    } 
    	}
      
     
    }
   
    
// ---------------------------------------------------------------------------
// Fetches the data from a particular contact 
// ---------------------------------------------------------------------------
//     
void CContactsSearcher::GetDataForSingleContactL( MVPbkBaseContact& aContact )
    {
     // Fetches all the editable fields for a particular contact. The AddContactFieldsL() 
     // function fetches the data for a particular entry of the contact.
     //  iCurrentContactDataString will contain the final space separated string 
    if(iCurrentContactDataString)
        {
      	delete iCurrentContactDataString;
      	iCurrentContactDataString = NULL;
        }
    delete iCurrentSnippet;
   	iCurrentSnippet = NULL;
    
    // First fetch the last name and first name
    AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_LASTNAME);
    AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_FIRSTNAME);
    AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_COMPANYNAME);
    
    // Now the iCurrentContactDataString contains last name and first name. 
    // This forms  the title for our contact.
    if(iCurrentContactDataString)
        {
        if(iCurrentTitle)
            {
        	delete iCurrentTitle;
        	iCurrentTitle = NULL;
            }
        iCurrentTitle = HBufC::NewL(iCurrentContactDataString->Des().Length()); 
        TPtr ptr( iCurrentTitle->Des() );
        ptr.Append(iCurrentContactDataString->Des());
        //delete iCurrentContactDataString; iCurrentContactDataString = NULL;
        }
    else
        {
        iCurrentTitle = HBufC::NewL(KBufferLength);
      	TPtr ptr( iCurrentTitle->Des() );
      	ptr.Append( KUnnamedContact );
        }
        
    if(iCurrentSnippet)
            {
        	delete iCurrentSnippet;
        	iCurrentSnippet = NULL;
            }
    
     // Add the Phone number
    AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_MOBILEPHONEGEN); 
          
	//AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_LASTNAME);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_LASTNAMEREADING);
	//AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_FIRSTNAME);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_FIRSTNAMEREADING);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_PREFIX);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_SUFFIX);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_SECONDNAME);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_LANDPHONEHOME);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_MOBILEPHONEHOME);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_VIDEONUMBERHOME);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_FAXNUMBERHOME);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_VOIPHOME); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_EMAILHOME);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_URLHOME); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRLABELHOME);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRPOHOME  );
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDREXTHOME); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRSTREETHOME); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRLOCALHOME);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRREGIONHOME); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRPOSTCODEHOME );
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRCOUNTRYHOME);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_JOBTITLE); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_COMPANYNAME);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_LANDPHONEWORK); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_MOBILEPHONEWORK);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_VIDEONUMBERWORK);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_FAXNUMBERWORK);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_VOIPWORK); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_EMAILWORK);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_URLWORK);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRLABELWORK);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRPOWORK);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDREXTWORK); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRSTREETWORK); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRLOCALWORK);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRREGIONWORK); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRPOSTCODEWORK );
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRCOUNTRYWORK);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_LANDPHONEGEN);
	//AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_MOBILEPHONEGEN); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_VIDEONUMBERGEN); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_FAXNUMBERGEN);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_VOIPGEN);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_POC);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_SWIS );
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_SIP);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_EMAILGEN); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_URLGEN);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRLABELGEN);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRPOGEN);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDREXTGEN); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRSTREETGEN);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRLOCALGEN);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRREGIONGEN);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRPOSTCODEGEN);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ADDRCOUNTRYGEN); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_PAGERNUMBER);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_DTMFSTRING);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_WVADDRESS);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_DATE);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_NOTE);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_THUMBNAILPIC);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_RINGTONE); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_CALLEROBJIMG);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_CALLEROBJTEXT);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_MIDDLENAME);  
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_DEPARTMENT ); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ASSTNAME); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_SPOUSE);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_CHILDREN); 
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ASSTPHONE);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_CARPHONE); 
    AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_ANNIVERSARY);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_SYNCCLASS);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_LOCPRIVACY);
	AddContactFieldsL( aContact,R_VPBK_FIELD_TYPE_GENLABEL);
    
    //Set the snippet if not set yet
    if(!iCurrentSnippet) 
        {
        iCurrentSnippet =HBufC8::NewL(KSpace().Length()); 
        iCurrentSnippet->Des().Append( KSpace );
        }        
   
}


// ---------------------------------------------------------------------------
// Add the data from contact fields
// ---------------------------------------------------------------------------
//     
void CContactsSearcher::AddContactFieldsL(MVPbkBaseContact& aContact,TInt afieldtype)
    {
	 
	 const MVPbkFieldType*  myContactDataField = 
        iContactManager->FieldTypes().Find( afieldtype );

    CVPbkBaseContactFieldTypeIterator* itr = 
        CVPbkBaseContactFieldTypeIterator::NewLC( *myContactDataField, 
            aContact.Fields() );
    while ( itr->HasNext() )
        {
        
        const MVPbkBaseContactField* field = itr->Next();
        // last name field is text data
        HBufC* fieldToAdd = NULL;
        
       //Check if the contact filed is a date field or a text field
        if( (field->FieldData()).DataType() == EVPbkFieldStorageTypeDateTime)
            {
        
            const MVPbkContactFieldDateTimeData& myDate = MVPbkContactFieldDateTimeData::Cast( field->FieldData() );
            TTime dateValue = myDate.DateTime();
            TDateTime  xyz = dateValue.DateTime();
            TInt yearDate = xyz.Year();
            TInt monthDate = xyz.Month() + 1; //Add 1,since month starts from 0
            TInt dayDate = xyz.Day() + 1;//Add 1,since day starts from 0
                
            //Form the date filed .. DD/MM/YYYY
            fieldToAdd = HBufC::NewLC( KMaxFileName );
		    TPtr fieldToAddPtr( fieldToAdd->Des() );
		    fieldToAddPtr.AppendNum( dayDate );
		    fieldToAddPtr.Append( KForwardSlash );
	    	fieldToAddPtr.AppendNum( monthDate );
		    fieldToAddPtr.Append( KForwardSlash );
		
	    	//The year is shown as YYYY. So append appropriate 
		   // number of zeros in the year
		   if(yearDate < 10)
		       {
			   fieldToAddPtr.AppendNum(0);
			   fieldToAddPtr.AppendNum(0);
			   fieldToAddPtr.AppendNum(0);
		       }
		  else if(yearDate < 100)
		      {
			  fieldToAddPtr.AppendNum(0);
			  fieldToAddPtr.AppendNum(0);
		      }
		 else if(yearDate < 1000)
		      {
		      fieldToAddPtr.AppendNum(0);
		
		      }
		
		fieldToAddPtr.AppendNum( yearDate );
		
			
		}
        else if( (field->FieldData()).DataType() == EVPbkFieldStorageTypeText)
            {
        	
            const MVPbkContactFieldTextData& data = 
            MVPbkContactFieldTextData::Cast( field->FieldData() );
        
            fieldToAdd = data.Text().AllocLC();
            }
       else
           {
           CleanupStack::PopAndDestroy( itr );
            return;	
           }

      if(!iCurrentSnippet)
         {
        
         switch(afieldtype)
		     {
			
			 case R_VPBK_FIELD_TYPE_LANDPHONEGEN:
			 case R_VPBK_FIELD_TYPE_LANDPHONEWORK:
             case R_VPBK_FIELD_TYPE_LANDPHONEHOME:	
             case R_VPBK_FIELD_TYPE_MOBILEPHONEWORK:
             case R_VPBK_FIELD_TYPE_MOBILEPHONEGEN:
             case R_VPBK_FIELD_TYPE_MOBILEPHONEHOME:		
		 
		     if(fieldToAdd->Des().Length())
                 {
                 iCurrentSnippet = HBufC8::NewL(KSpace().Length() + fieldToAdd->Des().Length() ) ;
                 TPtr8 ptr8(iCurrentSnippet->Des()) ;
                 TPtr16 ptr16(fieldToAdd->Des()) ;
	             CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr8,ptr16 );
                 }
                 break;
			default:
	    	     break;
	     	}
            
         	
        }
        if(iCurrentContactDataString)
            {
            iCurrentContactDataString = iCurrentContactDataString->ReAllocL( 
                                        iCurrentContactDataString->Length() + 
                                        KSpace().Length() + fieldToAdd->Des().Length() );	
                                
            }
        else
            {
    	    iCurrentContactDataString =HBufC::NewL(KSpace().Length() + fieldToAdd->Des().Length() ) ;
    
            }
   	
        iCurrentContactDataString->Des().Append( fieldToAdd->Des() );
        iCurrentContactDataString->Des().Append( KSpace );
        CleanupStack::PopAndDestroy( fieldToAdd );
        }
    
    CleanupStack::PopAndDestroy( itr );

    }

void CContactsSearcher::ContactViewReadyL(
                MVPbkContactViewBase& aView )
    {
    aView.RemoveObserver( *this );
    if ( !iIsFilterView )
        {    
        iSearchStrings = new( ELeave ) CDesCArrayFlat( 2 );
        for ( TInt i = 0; i < iTextSearcher->SearchKeyword()->MdcaCount(); ++i )
            {
            TPtrC searchWord = iTextSearcher->SearchKeyword()->MdcaPoint( i );
            TVPbkWordParserCallbackParam param( &searchWord, iSearchStrings );
            VPbkParseDataL( &param );
            }
    
        if (iSearchStrings->MdcaCount() == 0)
    	    {
    	    iSearchStrings->AppendL(KNullDesC);
    	    }
		 
        iFilterView = iContactViewBase->ViewFiltering()->CreateFilteredViewLC(*this, *iSearchStrings, NULL );
        CleanupStack::Pop();
        iIsFilterView = ETrue;
        return;
        }
    delete iVPbkOperation;
    iVPbkOperation = NULL;
	   
    delete iSearchStrings;
    iSearchStrings = NULL;
	   
    delete iBufferForQuery;
    iBufferForQuery = NULL;
	
    TInt cnt = aView.ContactCountL();
    for ( TInt i = 0; i < cnt; ++i )
        {
        if ( iAllContactItemsFromDb ) 
            {
            iAllContactItemsFromDb->AppendL( aView.CreateLinkLC(i) );	
            CleanupStack::Pop();
            }
        }
		
    if ( iAllContactItemsFromDb ) 
        {
        if ( iAllContactItemsFromDb->Count() >0)
            {
            // If there are still contacts available, then retrieve them one by one
            const MVPbkContactLink& link = iAllContactItemsFromDb->At( iCurrentDatatBaseItemIndex );
            iCurrentDatatBaseItemIndex++;
		        //Retrieve the contact 
            TRAP_IGNORE( iVPbkOperation = iContactManager->RetrieveContactL( link, *this ) ); 
            }
        else
            {
            // All contacts searched, set the appropriate flag
            iAllContactsSearched = ETrue;
            
            // Active object needs to be activated.
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
            SetActive();
            }
        }
    }
// --------------------------------------------------------------------
//Called when a view is ready for use.
// --------------------------------------------------------------------
//
void CContactsSearcher::ContactViewReady(
                MVPbkContactViewBase& aView )
    {
    TRAP_IGNORE(ContactViewReadyL(aView));
    }

// --------------------------------------------------------------------
// Called when a view is unavailable for a while. 
// --------------------------------------------------------------------
//
void CContactsSearcher::ContactViewUnavailable(
                MVPbkContactViewBase& aView )
    {
	TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
	SetActive();
	return;
    }

// --------------------------------------------------------------------
// Called when a contact has been added to the view. 
// --------------------------------------------------------------------
//
void CContactsSearcher::ContactAddedToView(
                MVPbkContactViewBase& aView, 
                TInt aIndex, 
                const MVPbkContactLink& aContactLink )
    {
    }

// --------------------------------------------------------------------
// Called when an error occurs in the view.
// --------------------------------------------------------------------
//
void CContactsSearcher::ContactRemovedFromView(
                MVPbkContactViewBase& aView, 
                TInt aIndex, 
                const MVPbkContactLink& aContactLink )
    {
    }

// --------------------------------------------------------------------
// Called when a contact has been removed from a view.
// --------------------------------------------------------------------
//
void CContactsSearcher::ContactViewError(
                MVPbkContactViewBase& aView, 
                TInt aError, 
                TBool aErrorNotified )
    {
    }

// ---------------------------------------------------------------------------
// A function that parser field data into words
// This function is used as a callback in the iContactManager->FindL()
// ---------------------------------------------------------------------------
// 
TInt VPbkParseDataL( TAny* aParam )
    {
    TVPbkWordParserCallbackParam* param = 
    static_cast<TVPbkWordParserCallbackParam*>( aParam );
    const TText* ptr = param->iStringToParse->Ptr();
	const TText* end = ptr + param->iStringToParse->Length();
	const TText* startOfWord = NULL;
	
	if ( FeatureManager::FeatureSupported( KFeatureIdJapanese ) ||
	     FeatureManager::FeatureSupported( KFeatureIdChinese )  ||
	     FeatureManager::FeatureSupported( KFeatureIdThai ) )
	    {
	    while(ptr != end)
	        {
	        if (*ptr >= 0x4e00)
	            {
	            TPtrC addWord(ptr, 1);
	            param->iWordArray->AppendL(addWord);	
	            }
		    ptr++;
	        }
	    }
	else
		{
	    FOREVER
		    {
		    // Other than alpha digit are separators
		    if (ptr==end || !TChar(*ptr).IsAlphaDigit())
			    {
			    if (startOfWord)
				    {
				    TPtrC addWord(startOfWord,ptr-startOfWord);
				    param->iWordArray->AppendL(addWord);
				    startOfWord=NULL;
				    }
			    if (ptr==end)
				    break;
			    }
		    else if (!startOfWord)
			    startOfWord=ptr;
		    ptr++;
		    }
		}
	return KErrNone;
    }    


