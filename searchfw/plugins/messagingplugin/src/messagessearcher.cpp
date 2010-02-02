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
* Description:   Plugin for messagingplugin information search.
*
*/


  
// Includes
#include <smuthdr.h>
#include <mtclreg.h>	
#include <smsclnt.h>
#include <mmsvattachmentmanager.h> 
#include <searchtextsearcher.h>
#include <searchpluginobserver.h>
#include <msgentrydetails.h>
#include <SenduiMtmUids.h>
#include <btmsgtypeuid.h>
#include <utf.h>
#include <msvids.h>
#include <mtmuibas.h>
#include <mtmuidef.hrh>
#include <searchdocumentid.h>
#include <searchlightresult.h>
#include <searchcondition.h>
#include <searchpluginobserver.h>
#include <searchresult.h>
#include <searchcommon.h>
#include <searchsnippetcreator.h>
#include <searchtextsnippet.h>

//USER INCLUDES
#include "messagessearcher.h"
#include "messagessearchplugindefines.h"

// Definition of service
const TUid KMuiuMsgEditorServiceUid = { 0x101F8820 };

// Service types
const TInt KMsgServiceView = 0x00000001;
const TInt KMsgServiceEdit = 0x00000002;
//

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::NewL()
// Symbian 1st phase constructor
// ------------------------------------------------------------------------------------------------
//

CMessagesSearcher*  CMessagesSearcher::NewL( const RArray<TUid>& aContentIdArray, 
								                    const CSearchCondition& aCondition, 
								                    const TUid& aPluginId,
								                    MSearchPluginObserver& aObserver )
{
    CMessagesSearcher* self = new( ELeave )CMessagesSearcher( aPluginId);

    CleanupStack::PushL ( self );
    self->ConstructL( aContentIdArray, aCondition,aObserver );
    CleanupStack::Pop( self );

    return self;
}


// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::ConstructL()
// Symbian 2nd phase constructor
// ------------------------------------------------------------------------------------------------
//   

void CMessagesSearcher::ConstructL( const RArray<TUid>& aContentIdArray, 
								    const CSearchCondition& aCondition,
								    MSearchPluginObserver& aObserver )
{
	iObserver = &aObserver;
	iSession = CMsvSession::OpenSyncL( *this );
	iMtmReg = CClientMtmRegistry::NewL( *iSession );

	iSmsMtm = STATIC_CAST ( CSmsClientMtm*, iMtmReg->NewMtmL( KUidMsgTypeSMS ) );
	iMmsMtm = STATIC_CAST ( CMmsClientMtm*, iMtmReg->NewMtmL( KUidMsgTypeMultimedia ) );
	
	SetPriority( EPriorityStandard );
	iTextSearcher = CSearchTextSearcher::NewL( *this );	
	iTextSearcher->SetParametersL( aCondition ); 
		    
    iFolderEntry = NULL;
    iSearchNotCancelled = ETrue;

	User::LeaveIfError( iFs.Connect() );		// Connect to the filesystem:RFileSystem
	iHitFound = EFalse;

} // ConstructL

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::CMessagesSearcher()
// C++ default constructor
// ------------------------------------------------------------------------------------------------
//
CMessagesSearcher::CMessagesSearcher( const TUid&  aPluginId)
: iPluginId( aPluginId ),
CActive(EPriorityStandard)
{
    CActiveScheduler::Add( this );
}

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::IsEntryDeleted()
// Checks the deltion of the entry before launching
// ------------------------------------------------------------------------------------------------
//
TBool CMessagesSearcher::IsEntryDeleted( TMsvId& aMsgEntryId )
{
	if( iEntryDeleted )
	{	
		if( iEntryDeleted->Find( aMsgEntryId ) == KErrNotFound )
			return EFalse;
		else
			return ETrue;
	}
	else
	{
		return EFalse;
	}
}
// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::~CMessagesSearcher()
// Destructor
// ------------------------------------------------------------------------------------------------
//
CMessagesSearcher::~CMessagesSearcher()
{	    


delete iMsvStore;
    
    iMsgDesCArray.ResetAndDestroy();
 	iHeavyResultsArray.ResetAndDestroy();  
 	if(iTextSearcher)
    {
    	delete iTextSearcher;
    	iTextSearcher = NULL;
    }
  
	if( iSmsMtm )
	    {
		delete iSmsMtm;
		iSmsMtm = NULL;
	    }
	
    if( iMmsMtm )
	    {
		delete iMmsMtm;
		iMmsMtm = NULL;
     	}

	if( iGenericMtm )
	{
		delete iGenericMtm;	
		iGenericMtm = NULL;
	}
     
    if( iSession )
	{
		delete iSession;
		iSession = NULL;	
	} 
    if( iEntryDeleted )
	    delete iEntryDeleted;
	
											// check for the members that are to be cleaned up...
}  // ~CMessagesSearcher

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::Destroy()
// Search FW calls this function when it no longer needs this searcher.
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::Destroy()
{
	iSearchNotCancelled = EFalse;
	
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

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::CancelSearch()
// Cancels the search operation and then does the Cleanup stuff...
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::CancelSearch()
{
	iSearchNotCancelled = EFalse;
	CleanUp();
}	

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::DoCancel()
// Cancels the search.
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::DoCancel()
{
CleanUp();
	
}


// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::Cleanup()
// For CleanUp resources
// ------------------------------------------------------------------------------------------------
//

void CMessagesSearcher::CleanUp()
{                	
	if( iFolderEntry )
	{
		delete iFolderEntry;
		iFolderEntry = NULL;
	}
	

	if( iMessageEntry )
	{	
		delete iMessageEntry;
		iMessageEntry = NULL;
	}
	
	if( iMessageEntries )
	{
		delete iMessageEntries;	
		iMessageEntries = NULL;
	}
					
	
	iToBeSearchedFolders.Close();


	iFs.Close();
    iMtmArray.Reset();
		
   if(iCurrentTextBeingSearched)
     {
     delete iCurrentTextBeingSearched;
     iCurrentTextBeingSearched = NULL;
	 } 
	if(iSenderReciever)
	{
	   	delete iSenderReciever;
	   	iSenderReciever = NULL;
	   }
	iMsgEntriesInFoldersArray.Reset();
}
    
// ------------------------------------------------------------------------------------------------
// Starts the search. Progress of the search is notified through aObserver. This call must be asynchronous.
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::SearchL( )
{

    
	iHeavyResultsArray.ResetAndDestroy();
	iMsgDesCArray.ResetAndDestroy();
    CMessagesSearcher::SetTargetFolderL();
    iHitFound = EFalse;

	
	__ASSERT_ALWAYS( this != NULL, User::Panic( KMessagesSearchPlugIn,
            EMessageSearchPlugInWrongConstructionParameters ) );

	 if( iTextSearcher == NULL) 
		 {
		   ReportFinishedL( KErrNone );
		    return;
		 }
	// condition that checks if ready for searching and if there's 
	// any message entry in any of the desired folders. If no entries, no results found
	if(!PrepareForSearchL())
		{
		iObserver->SearchCompletedL( KErrNone, KErrNone );
		return;
		}
				
	else	
		{   
		//start the search
		iTotalNumOfItems = 0;
	    iStatusItemCounter = 1;
	    TRequestStatus* status = &iStatus;
	    User::RequestComplete( status, KErrNone );
	    SetActive();
		}
}

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::NoEntriesInAllFolders()
// ------------------------------------------------------------------------------------------------
//

TBool CMessagesSearcher::AnyEntriesInAllFolders()
{

for(TInt i= 0; i <iMsgEntriesInFoldersArray.Count(); i++)	
{
	if(0 != iMsgEntriesInFoldersArray[i])	
		return ETrue;

}
return EFalse;
}


// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::GetResultsL( RPointerArray<CSearchResult> aResults )
// Gets the results got so far. A call to this function must flush the plugins internal buffer,
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::GetResultsL( const RPointerArray<CSearchDocumentId>& aDocumentIdArray )
{
    RPointerArray<CSearchResult> aSearchResultArray;  
      	

	TBufC8<KBufferLength> tempBuf(KText);
		
	 for(TInt i = 0 ; i < aDocumentIdArray.Count() ; i++ )
        {
           for(TInt j = 0 ; j < iHeavyResultsArray.Count() ; j++ )
            {
            
            	CSearchResult* tempHeavy =  	iHeavyResultsArray[j];
            if( (aDocumentIdArray[i]->DocumentId() == 
	    				 (iHeavyResultsArray[j]->Result()).DocumentId().DocumentId() ) && 
	    				 (aDocumentIdArray[i]->PluginId().iUid == iPluginId.iUid ) &&
	    				 (aDocumentIdArray[i]->RepositoryId() == KSearchCClassMessagesUid.iUid)
	    				 )
                {
               	if((&tempHeavy->Title() != NULL) && (&tempHeavy->Snippet() != NULL))
	                 {
	                        aSearchResultArray.Append( iHeavyResultsArray[j]);
	                        continue;	
	                 }
				if ( 0 < (*(iMsgDesCArray[j]->GetTitle())).Size() ) 	  
				{
					tempHeavy->SetTitleL( *(iMsgDesCArray[j]->GetTitle()));
				} 
			
		         	

					TInt KeyHitPos = iMsgDesCArray[j]->GetKeywordHitPosition();
					if(KeyHitPos != -1)
					{
						
					
					CSearchSnippetCreator* SnippetCreator = CSearchSnippetCreator::NewL();
			    	CleanupStack::PushL(SnippetCreator);
			    	CSearchTextSnippet* textSnippet = 
			    		SnippetCreator->CreateTextSnippetL(*( iMsgDesCArray[j]->GetBody()), KeyHitPos);
					

					
			    	tempHeavy->SetSnippetL( textSnippet->Snippet() );    		
			    	
			    	
			       	CleanupStack::PopAndDestroy(SnippetCreator);
			       	delete textSnippet;
			    	textSnippet = NULL;
					}
					else
					{
					HBufC8 * msgSnippet = HBufC8::NewL( iMsgDesCArray[j]->GetBody()->Des().Length() ) ;
                    TPtr8 ptr8(msgSnippet->Des()) ;
                    TPtr16 ptr16(iMsgDesCArray[j]->GetBody()->Des()) ;
	                CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr8,ptr16 );	
                    if( msgSnippet->Length() == 0 ) 
                    {
                        _LIT8( KSpace, " " );
                        TBufC8<5>  tmpBufTitle( KSpace );
                        msgSnippet = tmpBufTitle.AllocL();
                    }	
					tempHeavy->SetSnippetL( *msgSnippet );  
					delete msgSnippet;
					msgSnippet= NULL;  		
			    	
					}
					tempHeavy->SetSnippetFormatL( tempBuf.Des());	
					aSearchResultArray.AppendL( tempHeavy );
                }
		
		
            }
        }
	
	iObserver->ResultsRetrieveCompleteL( aSearchResultArray );
    aSearchResultArray.Reset();

}	

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::CancelResultsRetrieve( )
// Cancels the request for the result retrieval
// ------------------------------------------------------------------------------------------------
//	
void CMessagesSearcher::CancelResultsRetrieve ( )
{

}

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::SetTargetFolderL()
// Cancels the search.
// ------------------------------------------------------------------------------------------------
//

void CMessagesSearcher::SetTargetFolderL()
{
iToBeSearchedFolders.Reset();

// Find all subfolders under the "Root" folder
CMsvEntry* rootEntry;
rootEntry = CMsvEntry::NewL( *iSession, KMsvRootIndexEntryIdValue,
TMsvSelectionOrdering( KMsvGroupByStandardFolders, EMsvSortByDetails, EFalse ) );

CleanupStack::PushL( rootEntry );
ListSubFoldersL( *rootEntry );

CleanupStack::PopAndDestroy( rootEntry );
}


// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::ListSubFoldersL()
// Checks all folders that need to be searched.  Folders to be searched are
//	set via SetTargetFolderL.
//	Calls AddFolderWithSubFoldersL to add the folder to the array of folders to be
//	searched.
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::ListSubFoldersL(  const CMsvEntry& aFolderEntry )
{
// Only list "folder" children
CMsvEntrySelection* children = aFolderEntry.ChildrenL();
CleanupStack::PushL( children );

TInt sizeChildren = children->Count();


for ( TInt i=0; i < children->Count(); i++ )
	{
	TMsvId& childId = ( *children )[i];
	const TMsvEntry& childEntry ( aFolderEntry.ChildDataL( childId ) );

	if ( ( childEntry.iType == KUidMsvServiceEntry ) ||
    	 ( childEntry.iType == KUidMsvFolderEntry ) )
		{
        AddFolderWithSubFoldersL( childId );
		}
	}

CleanupStack::PopAndDestroy( children );  
}


// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::AddFolderWithSubFoldersL(  )
// Will add the specified folder to the folder array to be searched.
// Subfolders of the specified folder are checked by calling ListSubFoldersL.
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::AddFolderWithSubFoldersL( const TMsvId& aFolderId )
{
User::LeaveIfError( iToBeSearchedFolders.Append( aFolderId ) );

CMsvEntry* folderEntry = CMsvEntry::NewL( *iSession, aFolderId,
		TMsvSelectionOrdering( KMsvGroupByStandardFolders, EMsvSortByDetails, EFalse ) );
CleanupStack::PushL( folderEntry );

ListSubFoldersL( *folderEntry );

CleanupStack::PopAndDestroy( folderEntry );	// folderEntry
}

// ------------------------------------------------------------------------------------------------
// Prepares the object for the search. MoveOnToNextMessageL is called
// to find the 1st message in the to be searched folders.
// Returns:  ETrue: When messages were found.
//          EFalse: No messages to search for found.	
// ------------------------------------------------------------------------------------------------
//
TBool CMessagesSearcher::PrepareForSearchL()
{
iToBeSearchedFoldersIdx = 0;		// start searching on 1st folder item
iMessageEntriesIdx = 0;				// start searching on 1st message item

TInt foldersCount = iToBeSearchedFolders.Count();

if ( !foldersCount )
{
	return EFalse;
}

delete iFolderEntry;
iFolderEntry = NULL;

TMsvId& folderId = iToBeSearchedFolders[iToBeSearchedFoldersIdx];
TMsvSelectionOrdering order;
iFolderEntry = CMsvEntry::NewL( *iSession, folderId, order );
if(iMessageEntries)
    {
    delete iMessageEntries;
    iMessageEntries = NULL;
    iMessageEntries = iFolderEntry->ChildrenWithTypeL( KUidMsvMessageEntry );	
    }
    else
    {
        iMessageEntries = iFolderEntry->ChildrenWithTypeL( KUidMsvMessageEntry );	
    }

// no messages in this folder, go and find folder with messages
if ( !iMessageEntries->Count() )
{
	return MoveOnToNextMessageL();	// returns ETrue when messages are found,
								//  EFalse otherwise
}

return ETrue;	// Messages were found, ready to start searching
}

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::DoActualSearchL()
// Does the search of one message.  Called from RunL.  In one RunL call
// a limited number of messages are searched.
// Uses MoveOnToNextMessageL to get the next message to be searched.
// Returns:  ETrue: More seaching needed.
//          EFalse: No more items to search.
// ------------------------------------------------------------------------------------------------
//
TBool CMessagesSearcher::DoActualSearchL()
{
	TInt messagesInOneGoCounter( 0 );

	TBool messages ( ETrue );

	TTime startTime;
	startTime.HomeTime();
	TTime now;


	while( messages && ( messagesInOneGoCounter < KSearchThisManyMessagesInOneGo )  
														&& (iSearchNotCancelled) )
	{
		iHitFound = EFalse;
		
		TMsvId& messageId = iMessageEntries->At( iMessageEntriesIdx );
		
		if (iMessageEntry)
		{
			delete iMessageEntry;
			iMessageEntry = NULL;
		}
		
		iMessageEntry = iSession->GetEntryL( messageId );

		const TMsvEntry& messageEntry = iMessageEntry->Entry();
		const TUid id (messageEntry.iMtm);

		// Load message
		if ( messageEntry.iMtm == KUidMsgTypeSMS ) 
		{
			TRAP_IGNORE( SearchSmsL( messageId ));
		}

        else if ( ( messageEntry.iMtm == KUidMsgTypeMultimedia ||
	                messageEntry.iMtm == KUidMsvTechnologyGroupMMS ||
	                messageEntry.iMtm == KUidMsgMMSNotification ||
	                messageEntry.iMtm == KUidMsgTypeBt ||
	                messageEntry.iMtm == KSenduiMtmIrUid ||
	                messageEntry.iMtm == KSenduiMtmPushMtmUid ||
	                messageEntry.iMtm == KSenduiMtmUniMessageUid ) )
	              
        {          	
         SearchInMmsMessageL(messageId);
        	
        }
        
	    else if ( messageEntry.iMtm != KUidMsgTypeBt
	               && messageEntry.iMtm != KSenduiMtmBioUid
	               && messageEntry.iMtm != KSenduiMtmIrUid
	               && messageEntry.iMtm != KSenduiMtmFaxUid
	               && messageEntry.iMtm !=  KSenduiMtmAudioMessageUid
	               && messageEntry.iMtm !=  KSenduiMtmPostcardUid
	               && messageEntry.iMtm !=  KDrmLinkSender  )
	           
        {
				
			TRAP_IGNORE( SearchGenericEmailL( messageId, messageEntry.iMtm ) );
        
        }// if
        

		messagesInOneGoCounter++;
		iStatusItemCounter++;
		messages = MoveOnToNextMessageL();		// move to next message
	
		now.HomeTime();
	    if ( now.MicroSecondsFrom( startTime ) > KSearchTimeFrame )
		{
				break;
		}
	}

	if ( !messages )
		{

		ReportFinishedL( iStatusItemCounter, iTotalNumOfItems );
		}

	return messages;

}
// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::MoveOnToNextMessageL()
// Moves through all messages under those folders to be searched and returns
// with ETrue when a message is found.
// Returns:  ETrue: When messages were found.
//          EFalse: No messages to search for found.
// 	On return (with ETrue) iMessageEntries is pointing to the folder where
//	 the next message is to be searched.

//	On return (with ETrue) iMessageEntriesIdx is having the index of
//	 the message (index of iMessageEntries) next to be searched.
//
// ------------------------------------------------------------------------------------------------
//
TBool CMessagesSearcher::MoveOnToNextMessageL()
{
iMessageEntriesIdx++;

if ( iMessageEntriesIdx > iMessageEntries->Count()-1 )	// go to next folder
	{
	iMessageEntriesIdx = 0;
	iToBeSearchedFoldersIdx++;

	if ( iToBeSearchedFoldersIdx < iToBeSearchedFolders.Count() )
		{
		delete iFolderEntry;
		iFolderEntry = NULL;

		TMsvId& folderId = iToBeSearchedFolders[iToBeSearchedFoldersIdx];

		TMsvSelectionOrdering order( KMsvNoGrouping, EMsvSortByNone, EFalse );

    	iFolderEntry = CMsvEntry::NewL( *iSession, folderId, order );

		CMsvEntrySelection* msgEntry = iFolderEntry->ChildrenL();
		CleanupStack::PushL( msgEntry );

		TInt msgEntryCounts = msgEntry->Count();
		const TMsvEntry& msgNoEntry ( iFolderEntry->Entry()); //ChildDataL( folderId ) );
		
		_LIT16(KDocuments,"Documents");
		TPtrC ptrDocuments(KDocuments); 

		if(msgNoEntry.iDetails != ptrDocuments)
			iMsgEntriesInFoldersArray.Append(msgEntryCounts);

		if (ETrue) //msgEntryCounts)
		    {		             
    		delete iMessageEntries;
    		iMessageEntries = NULL;

    		iMessageEntries = iFolderEntry->ChildrenWithTypeL( KUidMsvMessageEntry );
    		}
    	else
    		{
    		iMessageEntries->Reset();
    		return EFalse;
    		}
    		
		CleanupStack::PopAndDestroy( 1 );
							
		// no messages in this folder, go and find folder with messages
		if ( !iMessageEntries->Count() )
			{
			return MoveOnToNextMessageL();
			}
		
		}
	else
		{
		return EFalse;	// nothing to be searched anymore
		}
	}
return ETrue;	// iMessageEntriesIdx is pointing to a valid message
}


// ------------------------------------------------------------------------------------------------
// Searches SMS message.
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::SearchSmsL( TMsvId& aMessageId ) // id of the message to search
{
   
	iHitFound = EFalse;
	
	iSmsMtm->SwitchCurrentEntryL( aMessageId );
	
	TRAPD(err, iSmsMtm->LoadMessageL());
	if(err) User::Leave( err );
	
	//search sender/receiver first
	iFieldCurrentlyUnderSearch = ESearchingSender;

	iHitFound = EFalse; 
	  

	delete iCurrentTextBeingSearched;
	iCurrentTextBeingSearched = NULL;
	iCurrentTextBeingSearched = HBufC::NewL(iSmsMtm->Entry().Entry().iDetails.Length());
	TPtr curTextPtr(iCurrentTextBeingSearched->Des());
    curTextPtr.Append(iSmsMtm->Entry().Entry().iDetails);

	iTextSearcher->SearchL( *iCurrentTextBeingSearched );
	if(iSenderReciever)	
	{
		delete iSenderReciever;
		iSenderReciever = NULL;
	}
	iSenderReciever = HBufC::NewL(iCurrentTextBeingSearched->Des().Length());
	iSenderReciever = iCurrentTextBeingSearched->Des().AllocL();
	// if still no results, then look up for other receivers, if any
	if (!iHitFound)
	{
	    
		delete iCurrentTextBeingSearched;
	    iCurrentTextBeingSearched = NULL;
	    const MDesCArray& addressee = iSmsMtm->AddresseeList().RecipientList();
        if (addressee.MdcaCount() == 0)
		{
			const CSmsHeader& header = iSmsMtm->SmsHeader();
			const TPtrC fromAddress = header.FromAddress();

			if ( fromAddress.Length() > 0 ) 
			{
			     
		        iCurrentTextBeingSearched = HBufC::NewL(fromAddress.Length());
	            TPtr curTextPtr(iCurrentTextBeingSearched->Des());
                curTextPtr.Append(fromAddress);
			    iTextSearcher->SearchL( *iCurrentTextBeingSearched );
			}			       
		}
		else
		{   
			for (TInt i = 0; i < addressee.MdcaCount(); i++)
		    {
			    
			    iCurrentTextBeingSearched = addressee.MdcaPoint( i ).AllocL();
			    
			    iTextSearcher->SearchL( *iCurrentTextBeingSearched );
			    if(iHitFound)
			       {
			       break;	
			       }
			       
		    } //for
		    
		}// else		   	
		
	} // if
	
	// for searching the body of the message
	if (!iHitFound)
	{
	    iFieldCurrentlyUnderSearch = ESearchingBody;
		delete iCurrentTextBeingSearched;
		iCurrentTextBeingSearched = NULL;
		iCurrentTextBeingSearched = HBufC::NewL(iSmsMtm->Body().DocumentLength() + 1 );
	    TPtr msgText = iCurrentTextBeingSearched->Des();
	    iSmsMtm->Body().Extract( msgText, 0 );
	    iTextSearcher->SearchL( msgText );
	    
	}

   
}

void CMessagesSearcher::SearchInMmsMessageL(const TMsvId& aMessageId)
    {
      _LIT8(KTextMimeType,"text/plain");
      iHitFound = EFalse;
      iMmsMtm->SwitchCurrentEntryL( aMessageId );
      TRAPD(err,iMmsMtm->LoadMessageL());
	
      CMsvEntry* messageEntry = iSession->GetEntryL(aMessageId);
      CleanupStack::PushL(messageEntry );
      CMsvAttachment* attachment = NULL;
      
      
      if( !iHitFound )
        {
            iFieldCurrentlyUnderSearch = ESearchingMMSSender;
            if(iCurrentTextBeingSearched)
            {
            delete iCurrentTextBeingSearched;
            iCurrentTextBeingSearched = NULL;
            }
            iCurrentTextBeingSearched  =  messageEntry->Entry().iDetails.AllocL(); 
            if(iSenderReciever)	
            {
            delete iSenderReciever;
            iSenderReciever = NULL;
            }
            iSenderReciever = iCurrentTextBeingSearched->Des().AllocL();	  
            iTextSearcher->SearchL( *iCurrentTextBeingSearched );
        }
      
      if (!iHitFound)
        {
            iFieldCurrentlyUnderSearch = ESearchingMMSBody;
            if(iCurrentTextBeingSearched)
            {
            delete iCurrentTextBeingSearched;
            iCurrentTextBeingSearched = NULL;
            }
            if(iSenderReciever)	
            {
            delete iSenderReciever;
            iSenderReciever = NULL;
            }
            iSenderReciever = messageEntry->Entry().iDetails.AllocL(); 
            iCurrentTextBeingSearched = messageEntry->Entry().iDescription.AllocL();
            	  
            iTextSearcher->SearchL( *iCurrentTextBeingSearched );
        } 
      
      if(!iHitFound ) 
    	  {
          CMsvEntrySelection* messageChildren = messageEntry->ChildrenL(); //Child ->Message
          CleanupStack::PushL(messageChildren);
          TBool ifBTorIR = EFalse;
          if (messageChildren->Count() == 1) // This is case of BT & IR messages with attachments
        	  {
              TMsvId childAtt = (*messageChildren)[0];
              messageEntry->SetEntryL(childAtt); // switch context to CHILD entry
              ifBTorIR = ETrue;  
        	  }
            
          if (messageEntry->HasStoreL() )
        	  {
              CMsvStore* store = messageEntry->ReadStoreL();
                CleanupStack::PushL(store);
                
                MMsvAttachmentManager& attMngr = store->AttachmentManagerL();
                TInt attcount = attMngr.AttachmentCount();
                for(TInt i = 0 ; i < attcount ; i++ )
                    {
                        attachment = attMngr.GetAttachmentInfoL( i );
                        CleanupStack::PushL( attachment );
                        
                        //break it
                        if(iCurrentTextBeingSearched)
                        {
                        delete iCurrentTextBeingSearched;
                        iCurrentTextBeingSearched = NULL;
                        }
                        if(iSenderReciever)	
                        {
                        delete iSenderReciever;
                        iSenderReciever = NULL;
                        }
                        
                        HBufC* buf16 = NULL;
                        if( KErrNotFound != attachment->MimeType().Find(KTextMimeType))
                        {
                        
						HBufC8* buf8 = NULL;
						RFile fsFile = attMngr.GetAttachmentFileL( i );
						TInt fileSize = 0;
						fsFile.Size( fileSize );
						
						if ( fileSize ) 
							{
							buf8 =  HBufC8::NewL( fileSize );
							buf16 =  HBufC::NewL( fileSize );
							TPtr8 bufPtr8 = buf8->Des();
							TPtr bufPtr16 = buf16->Des();
							fsFile.Read( 0, bufPtr8, fileSize );
							//TInt err = fs.ReadFileSection( filepath, 0, buf8, 10);
							CnvUtfConverter::ConvertToUnicodeFromUtf8( bufPtr16, bufPtr8 );	
							}
						if ( buf8 ) 
							{
							delete buf8;	
							}
						
						fsFile.Close();
						
						}
                        
                        iSenderReciever = messageEntry->Entry().iDetails.AllocL();	 
                        HBufC* fileName = attachment->AttachmentName().AllocL();
                        iCurrentTextBeingSearched = messageEntry->Entry().iDescription.AllocL();	
                        
                        TInt sizeOfAttachement = 0;
                        if ( ifBTorIR ) 
                        	{
                        	if ( iSenderReciever ) 
	                        	{
	                        	sizeOfAttachement += iSenderReciever->Size();
	  							}
	                        if ( fileName ) 
	                        	{
	                        	sizeOfAttachement += fileName->Size();
	                        	}
	                        sizeOfAttachement += 3;
                        	}
                        
                        if ( buf16 ) 
                        	{
                        	sizeOfAttachement += buf16->Size();
                        	}

                        if ( sizeOfAttachement ) 
                        	{
                        	iCurrentTextBeingSearched = HBufC::NewL( sizeOfAttachement );
                        	TPtr curPtr = iCurrentTextBeingSearched->Des();
                        	if ( ifBTorIR ) 
                        		{
                        		if ( iSenderReciever ) 
	                        		{
	                        		curPtr.Append( *iSenderReciever );
	                        		curPtr.Append( ' ' );
	                        		}
	                        	if ( fileName ) 
	                        		{
	                        		curPtr.Append( *fileName );
	                        		curPtr.Append( ' ' );
	                        		}
                        		}	
                        	
                        	if ( buf16 ) 
                        		{
                        		curPtr.Append( *buf16 );
                        		}
                        	}
                        
                        
                        iFieldCurrentlyUnderSearch = ESearchingMMSBody; 
                        
                        if (!iHitFound)
                        {
                        iTextSearcher->SearchL( *iCurrentTextBeingSearched );
                        }
                     CleanupStack::PopAndDestroy(attachment);     
                    }//for
              CleanupStack::PopAndDestroy(store); // store 	
            }//if
            CleanupStack::PopAndDestroy(messageChildren );   
         }
      CleanupStack::PopAndDestroy(messageEntry); // btEntry & btChildren
   }
// -------------------------------------------------------------------------------------------------
// void CMessagesSearcher::SearchGenericEmailL( ) //message id to search
// Searches Generic Email message.
// -------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::SearchGenericEmailL( TMsvId& aMessageId, TUid aMessageUid ) 
{

    delete iMsvStore;
    iMsvStore = NULL;
    
	iHitFound = EFalse;
    
    iGenericMtm = SearchFromMtmArrayL( aMessageUid );

	iGenericMtm->SwitchCurrentEntryL( aMessageId );
	
	TRAPD(err, iGenericMtm->LoadMessageL());
	if(err) User::Leave( err );
	
	
	TRAPD ( readStoreError, iMsvStore = iGenericMtm->Entry().ReadStoreL() );
        
    // if error occured during the store reading, message was unfetched and cannot
    // therefore be handled in the usual manner. Instead a different kind of approach is needed.
    if ( readStoreError != KErrNone ) 
       {
       // Stop searching the rest of the message. Since the msvStore is null, it will only 
       // result in Leaves.
       return;    
       }
       
       
	if (iGenericMtm->Body().LdDocumentLength() > 0  )
	{
	    
        delete iCurrentTextBeingSearched;
        iCurrentTextBeingSearched = NULL;
        iCurrentTextBeingSearched = HBufC::NewL( iGenericMtm->Body().LdDocumentLength() + 1 );
	    
	    iFieldCurrentlyUnderSearch = ESearchingGenericEmailBody;

	    TInt i( 0 );

	    while ( ETrue )
        {
	    	TPtrC msgText = iGenericMtm->Body().Read( Max ( i - 1, 0 ) );
	        iCurrentTextBeingSearched->Des().Append(msgText);
	        i = i + KEmailSegmentLength;
	        if ( i > iGenericMtm->Body().LdDocumentLength() )
	            {
	            break;
	            }
	    }

	    if (!iHitFound ) 
	        {
	        iTextSearcher->SearchL( *iCurrentTextBeingSearched );	
	        }
	    
	}

	iFieldCurrentlyUnderSearch = ESearchingGenericEmailSubject;
	if (!iHitFound )
	    {     
	    delete iCurrentTextBeingSearched;
	    iCurrentTextBeingSearched = NULL;
	    iCurrentTextBeingSearched = HBufC::NewL(iGenericMtm->Entry().Entry().iDescription.Length());
	    TPtr curTextPtr(iCurrentTextBeingSearched->Des());
        curTextPtr.Append(iGenericMtm->Entry().Entry().iDescription);
		
	
	    iTextSearcher->SearchL( *iCurrentTextBeingSearched );
	}
	iFieldCurrentlyUnderSearch = ESearchingGenericEmailSender;

	if (!iHitFound) 
	    { 
	    delete iCurrentTextBeingSearched;
	    iCurrentTextBeingSearched = NULL;
	     
	    iCurrentTextBeingSearched = HBufC::NewL(iGenericMtm->Entry().Entry().iDetails.Length());
	    TPtr curTextPtr(iCurrentTextBeingSearched->Des());
        curTextPtr.Append(iGenericMtm->Entry().Entry().iDetails);
		 
	
		iTextSearcher->SearchL( *iCurrentTextBeingSearched );
	}
		

	if (!iHitFound)
	{
	    iFieldCurrentlyUnderSearch = ESearchingGenericEmailSender;
		const CMsvRecipientList& list = iGenericMtm->AddresseeList();
        iHitFound = EFalse;
		for (TInt i = 0; i < list.Count(); i++)
			{
				if (!iHitFound) 
				{
				    delete iCurrentTextBeingSearched;
	                iCurrentTextBeingSearched = NULL; 
	             
	                iCurrentTextBeingSearched = HBufC::NewL(list.operator[](i).Length());
	                TPtr curTextPtr(iCurrentTextBeingSearched->Des());
                    curTextPtr.Append(list.operator[](i));
				   
				    
					iTextSearcher->SearchL(*iCurrentTextBeingSearched);	
					if(iHitFound)
					 break;
				}
			  
		    }
	}
				
}

// -------------------------------------------------------------------------------------------------
// Searches for Mtm -class from the array that can be used to open the message specified by aUid.
// If proper class is not found, it's created .
// -------------------------------------------------------------------------------------------------
//
CBaseMtm* CMessagesSearcher::SearchFromMtmArrayL( TUid aUid ) 
{
    
	for (TInt i = 0; i < iMtmArray.Count(); i++)
	    {
	    if (iMtmArray[i]->Type() == aUid)
	        {
	        CBaseMtm* baseMtm = iMtmArray[i];
	        return baseMtm;
	        }
	    }
	    
    CBaseMtm* newMtm;
	newMtm = iMtmReg->NewMtmL( aUid );
	iMtmArray.AppendL( newMtm );
	return newMtm;

}



// Implementation from the CActive Class
// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::RunL()
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::RunL()
{
if ( DoActualSearchL() )	// If more searching is needed, the active object is set active
	                        // again
	{
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrNone );
		SetActive();
	}
}


// ------------------------------------------------------------------------------------------------
// Called if error during execution of the RunL() 
// ------------------------------------------------------------------------------------------------
//
TInt CMessagesSearcher::RunError( TInt aError )
	{
	return aError;
	}

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::HandleSessionEventL()
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3 )
	{
	if( aEvent== EMsvEntriesDeleted )	
    	iEntryDeleted = (static_cast<CMsvEntrySelection*>(aArg1))->CopyL();
	}

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::HitL( )
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::HitL( TInt aKeyWordPos )
	{   
	iHitFound = ETrue;
   
	if(  aKeyWordPos!= KErrNotFound )
	{		
		 
		iTotalNumOfItems++;
		
		if (iFieldCurrentlyUnderSearch == ESearchingBody ||
		    iFieldCurrentlyUnderSearch == ESearchingSender )
		{
		    ReportHitInSmsL( aKeyWordPos );
		}
		else  
		    if (iFieldCurrentlyUnderSearch == ESearchingMMSBody ||
		    	iFieldCurrentlyUnderSearch == ESearchingMMSSender )
		    {
		        ReportHitInMmsL( aKeyWordPos );	    
		    }
		else 
		    if ( iFieldCurrentlyUnderSearch == ESearchingSMTPBody || 
		         iFieldCurrentlyUnderSearch == ESearchingPOP3Body ||
		         iFieldCurrentlyUnderSearch == ESearchingIMAP4Body || 
		         iFieldCurrentlyUnderSearch == ESearchingSMTPSender || 
		         iFieldCurrentlyUnderSearch == ESearchingSMTPSubject || 
		         iFieldCurrentlyUnderSearch == ESearchingPOP3Sender || 
		         iFieldCurrentlyUnderSearch == ESearchingPOP3Subject || 
		         iFieldCurrentlyUnderSearch == ESearchingIMAP4Sender ||
		         iFieldCurrentlyUnderSearch == ESearchingIMAP4Subject ||
		         iFieldCurrentlyUnderSearch == ESearchingGenericEmailSubject ||
		         iFieldCurrentlyUnderSearch == ESearchingGenericEmailBody ||
		         iFieldCurrentlyUnderSearch == ESearchingGenericEmailSender ||
		         iFieldCurrentlyUnderSearch == ESearchingGenericEmailSubject )

		      	{
		      	    ReportHitInEmailL( aKeyWordPos );	    
		       	}
	
	}
		       	
}
    
// -----------------------------------------------------------------------------
// Returns the launch info corresponding to a particular documentid
// -----------------------------------------------------------------------------
//     
HBufC8* CMessagesSearcher::LaunchInfoL( const CSearchDocumentId& aDocumentID )
{
     if(aDocumentID.PluginId() == iPluginId) 
         {
         
         HBufC8 * launchInfo = HBufC8::NewL(256);
         TPtr8 docIdPtr = launchInfo->Des();
         docIdPtr.AppendNum(aDocumentID.DocumentId());
         return launchInfo;
         
         }
     
     return NULL;
     
}
   
// -----------------------------------------------------------------------------
// Tell observer search finished
// -----------------------------------------------------------------------------
//  
void CMessagesSearcher::ReportFinishedL( TInt aSearchResult, TInt aTotalNumOfItems)
	{
    iObserver->SearchCompletedL( aSearchResult, aTotalNumOfItems );

	}

// ------------------------------------------------------------------------------------------------
// Generates a result from a hit in Sms message.
// ------------------------------------------------------------------------------------------------
//	 
void CMessagesSearcher::ReportHitInSmsL( TInt aKeyWordPos )
{

   	TInt Pos = aKeyWordPos;
	const TMsvEntry& entry = iMessageEntry->Entry();
    TBuf8<KEmailMessageItemIdLengthInHex> entryId;

    entryId.AppendNum( entry.Id(), EDecimal );
    
	// TMsvId 
	CSearchDocumentId* documentId = CSearchDocumentId::NewL( entry.Id(), iPluginId );
	documentId->SetRepositoryIdL( KSearchCClassMessagesUid.iUid );			
	CSearchLightResult* searchLightResult = CSearchLightResult::NewL( documentId );
	
	searchLightResult->SetContentClassId( KSearchCClassMessagesUid );
	
	if( entry.Parent() == KMsvSentEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderSentUid );
	}
	
	else if( entry.Parent() == KMsvGlobalOutBoxIndexEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderOutboxUid );
	}
	
	else if( entry.Parent() == KMsvGlobalInBoxIndexEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderInboxUid );
	}

	else if( entry.Parent() == KMsvDraftEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderDraftsUid );
	}
	
    searchLightResult->SetServiceId( KNullUid );	
		
	CMsgDesC* msgdetail = new(ELeave) CMsgDesC();		
    CleanupStack::PushL ( msgdetail );

	msgdetail->SetTitleL(entry.iDetails);
	
    if ((iFieldCurrentlyUnderSearch == ESearchingSender)||(iFieldCurrentlyUnderSearch == ESearchingBody)) 
	{	        	                    
	   msgdetail->SetKeywordHitPosL(-1);
	   msgdetail->SetBodyL(iSmsMtm->Entry().Entry().iDescription );
    }
    
	iMsgDesCArray.Append( msgdetail );	


    CleanupStack::Pop ( msgdetail );
    
   	CSearchResult* searchResult = CSearchResult::NewL( searchLightResult);

	searchResult->SetSnippetL(KNullDesC8);
	searchResult->SetTitleL(iSmsMtm->Entry().Entry().iDetails);
	
	iHeavyResultsArray.Append( searchResult );
  
	iObserver->ResultFoundL( searchLightResult, iStatusItemCounter, 
							iTotalNumOfItems );

}

// ------------------------------------------------------------------------------------------------
// Generates a result from a hit in Mms message.
// ------------------------------------------------------------------------------------------------
//    

void CMessagesSearcher::ReportHitInMmsL( TInt aKeyWordPos ) 
{

	const TMsvEntry& entry = iMessageEntry->Entry();
	
    TBuf8<KEmailMessageItemIdLengthInHex> entryId;
    entryId.AppendNum( entry.Id(), EDecimal );
    		
	CSearchDocumentId* documentId = CSearchDocumentId::NewL( entry.Id(), iPluginId );
		documentId->SetRepositoryIdL( KSearchCClassMessagesUid.iUid );		
	CSearchLightResult* searchLightResult = CSearchLightResult::NewL( documentId );
	
	searchLightResult->SetContentClassId( KSearchCClassMessagesUid );
	
	if( entry.Parent() == KMsvSentEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderSentUid );
	}
	
	else if( entry.Parent() == KMsvGlobalOutBoxIndexEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderOutboxUid );
	}
	
	else if( entry.Parent() == KMsvGlobalInBoxIndexEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderInboxUid );
	}

	else if( entry.Parent() == KMsvDraftEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderDraftsUid );
	}
	

    searchLightResult->SetServiceId( KNullUid );	

	CMsgDesC* msgdetail = new(ELeave) CMsgDesC();		
    CleanupStack::PushL ( msgdetail );

	msgdetail->SetTitleL( *iSenderReciever);  
	
  if (iFieldCurrentlyUnderSearch == ESearchingMMSSender) 
	{	      
	     msgdetail->SetKeywordHitPosL(-1);
	     	                    
		msgdetail->SetBodyL(iMmsMtm->Entry().Entry().iDescription );
	}
	else
	{
	     msgdetail->SetKeywordHitPosL(aKeyWordPos);
	     msgdetail->SetBodyL( *iCurrentTextBeingSearched );
	}
    
	iMsgDesCArray.Append( msgdetail );
	CleanupStack::Pop ( msgdetail );
  
   	CSearchResult* searchResult = CSearchResult::NewL( searchLightResult);

	searchResult->SetSnippetL(KNullDesC8);
	searchResult->SetTitleL(iMmsMtm->Entry().Entry().iDetails);
	
	iHeavyResultsArray.Append( searchResult );
 
	iObserver->ResultFoundL( searchLightResult, iStatusItemCounter, 
							iTotalNumOfItems );
}

// ------------------------------------------------------------------------------------------------
// CMessagesSearcher::ReportHitInEmailL()
// Generates a result from a hit in Email message.
// ------------------------------------------------------------------------------------------------
//
void CMessagesSearcher::ReportHitInEmailL( TInt aKeyWordPos ) 
{        
	const TMsvEntry& entry = iMessageEntry->Entry();
	
    TBuf8<KEmailMessageItemIdLengthInHex> entryId;
    entryId.AppendNum( entry.Id(), EDecimal );
    		
	CSearchDocumentId* documentId = CSearchDocumentId::NewL( entry.Id(), iPluginId );
	documentId->SetRepositoryIdL( KSearchCClassMessagesUid.iUid );		
	CSearchLightResult* searchLightResult = CSearchLightResult::NewL( documentId );
	
	searchLightResult->SetContentClassId( KSearchCClassMessagesUid );
	
	if( entry.Parent() == KMsvSentEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderSentUid );
	}
	
	else if( entry.Parent() == KMsvGlobalOutBoxIndexEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderOutboxUid );
	}
	
	else if( entry.Parent() == KMsvGlobalInBoxIndexEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderInboxUid );
	}
	
	else if( entry.Parent() == KMsvDraftEntryId )
	{
		searchLightResult->SetContentFolderId( KSearchCFolderDraftsUid );
	}
	else
	{
	searchLightResult->SetContentFolderId( KSearchCFolderInboxUid );
	}
	
    searchLightResult->SetServiceId( KNullUid );	
	
	CMsgDesC* msgdetail = new(ELeave) CMsgDesC();		
    CleanupStack::PushL ( msgdetail );

	msgdetail->SetTitleL( entry.iDetails);
    if (iFieldCurrentlyUnderSearch == ESearchingGenericEmailSender)
	{	        
	    msgdetail->SetKeywordHitPosL(-1);	          
	    msgdetail->SetBodyL(entry.iDescription );          
		
	}
	else 
	{       msgdetail->SetKeywordHitPosL(aKeyWordPos);
		    msgdetail->SetBodyL( *iCurrentTextBeingSearched );
	}
	    
	iMsgDesCArray.Append( msgdetail );		
	
    CleanupStack::Pop ( msgdetail );
  
   	CSearchResult* searchResult = CSearchResult::NewL( searchLightResult);

	searchResult->SetSnippetL(KNullDesC8);
	searchResult->SetTitleL(KNullDesC);
	iHeavyResultsArray.Append( searchResult );
 
	iObserver->ResultFoundL( searchLightResult, iStatusItemCounter, 
							iTotalNumOfItems );
									
}
	
	
void CMessagesSearcher::LaunchApplicationL(const TDesC8& aLaunchInfo)
	{
	TPtrC8 ptr = aLaunchInfo;
	
	TLex8 lex(ptr);
	
	TUid aMsgId;
	lex.Val(aMsgId.iUid);
	
	CMsvSession* session = CMsvSession::OpenSyncL( *this );
	CleanupStack::PushL( session );
	    	   
	CClientMtmRegistry* mtmReg = CClientMtmRegistry::NewL( *session );      
	CleanupStack::PushL( mtmReg );
	
	TMsvId id (aMsgId.iUid);
	
	CMsvEntry* MsvEnt;
	MsvEnt= session->GetEntryL(id);
	CleanupStack::PushL( MsvEnt );
	const TMsvEntry& entry = MsvEnt->Entry();
	
	CBaseMtm* clientMtm = mtmReg->NewMtmL( entry.iMtm);
	CleanupStack::PushL( clientMtm );    
	clientMtm->SwitchCurrentEntryL( MsvEnt->EntryId() );

    if ( MsvEnt->HasStoreL() )
        {                                  
        TEditorParameters temp;    

        if ( clientMtm->HasContext() )
            {
            temp.iId = clientMtm->Entry().EntryId();
            }

        TPckgC<TEditorParameters> paramPack( temp );    
      
        if( clientMtm->Entry().Entry().Parent() == KMsvDraftEntryId )
            {
            temp.iFlags &= ~EMsgReadOnly;
            }
        else
            {
            temp.iFlags|=EMsgReadOnly ;
            }
        
        TUid uid = KNullUid;
       	uid = DiscoverL( temp, clientMtm->Type() );
        if( uid == KNullUid )
        	{
        	User::Leave( KErrNotSupported );
        	}

        TApaAppInfo appInfo;
        RApaLsSession lsSession;
        User::LeaveIfError( lsSession.Connect() );
        CleanupClosePushL( lsSession );
        User::LeaveIfError( lsSession.GetAppInfo( appInfo, uid ) );

        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL( appInfo.iFullName );
        cmdLine->SetCommandL( EApaCommandViewActivate );
        // EApaCommandRun );

        cmdLine->SetTailEndL( paramPack );

        User::LeaveIfError( lsSession.StartApp( *cmdLine ) );

        CleanupStack::PopAndDestroy( cmdLine );
        CleanupStack::PopAndDestroy( &lsSession );
     
        }

    CleanupStack::PopAndDestroy( clientMtm );
    CleanupStack::PopAndDestroy( MsvEnt );
    CleanupStack::PopAndDestroy( mtmReg );    
    CleanupStack::PopAndDestroy( session );
	}

// -------------------------------------------------------------------------------------------------
// CIdsMessagesUiImpLauncher::DiscoverL
// -------------------------------------------------------------------------------------------------
//
TUid CMessagesSearcher::DiscoverL( const TEditorParameters aParams, TUid aMtmType )
	{
	// Let's first check if a special editor/viewer is set
    if( aParams.iSpecialAppId != KNullUid )
		{ // If set, simply return that appUid
		return aParams.iSpecialAppId;
		}
	
	CApaAppServiceInfoArray* services = NULL;	
	RApaLsSession ls;
	TUid appUid = KNullUid; // Set the uid as NULL
	
	if( ls.Connect() == KErrNone )
		{
	    CleanupClosePushL( ls );
		TInt ret = ls.GetServerApps( KMuiuMsgEditorServiceUid );

		if( ret != KErrNone )
			{
		    CleanupStack::PopAndDestroy( &ls );
		    return appUid; // return NULL appUid
			}

		services = ls.GetServiceImplementationsLC( KMuiuMsgEditorServiceUid );			
		
		if( services )
			{
			TArray<TApaAppServiceInfo> s = services->Array( );
			
			TInt count = s.Count( );
		
			if( count != 0)
				{	// 
				for( TInt i = 0; i<count; i++ )
					{
					if( TryMatchL( aParams, aMtmType, s[i], appUid ) )
						{ // If one of the services match, return the uid set by TryMatchL
						break;
						}
					}
				}		
			}
	    CleanupStack::PopAndDestroy( services );
	    CleanupStack::PopAndDestroy( &ls );
		}
	return appUid;
	}

// -------------------------------------------------------------------------------------------------
// CIdsMessagesUiImpLauncher::TryMatchL
// -------------------------------------------------------------------------------------------------
//
TBool CMessagesSearcher::TryMatchL( const TEditorParameters aParams, 
                                           TUid aMtmType, 
                                           const TApaAppServiceInfo aInfo, 
                                           TUid& aAppUid )
	{
	TBool useViewer = EFalse;

	if( ( aParams.iFlags & EMsgReadOnly ) || ( aParams.iFlags & EMsgPreviewMessage ) )
		{ // If readonly or preview flag is set -> look for "Viewer" service type
		useViewer = ETrue;
		}
	
	RResourceReader r;
	r.OpenLC( aInfo.OpaqueData() );
	
	TUid mtm = TUid::Uid( r.ReadInt32L( ) ); // read the mtm
	TInt serviceType = r.ReadInt16L( ); // the service type (view, edit)

    CleanupStack::PopAndDestroy( &r );

	if( aMtmType == mtm && useViewer && serviceType == KMsgServiceView )
		{
		aAppUid.iUid = aInfo.Uid( ).iUid;
		return ETrue;
		}
	else  if( aMtmType == mtm && !useViewer && serviceType == KMsgServiceEdit )
		{
		aAppUid.iUid = aInfo.Uid( ).iUid;
		return ETrue;			
		}
	
	return EFalse;	
	}
