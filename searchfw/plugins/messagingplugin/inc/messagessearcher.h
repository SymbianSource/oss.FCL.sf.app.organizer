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
* Description:   ECom search interface definition
*
*/




#ifndef C_MESSAGESSEARCHER_H
#define C_MESSAGESSEARCHER_H

//SYSTEM INCLUDES
#include <smtcmtm.h>
#include <popcmtm.h>
#include <impcmtm.h>

#include <mmsclient.h>
#include <sendas2.h>
#include <msvreg.h>
#include <searchcontentsearcher.h>


#include <apgcli.h>
#include <apacmdln.h>
#include <barsread2.h>
#include <MuiuMsgEditorLauncher.h>


// FORWARD DECLARATIONS
class CSmsClientMtm;
class CClientMtmRegistry;
class MSearchPluginObserver;
class CSearchTextSearcher;
class CSearchCriteriaArray;
class CSearchResult;
class CSearchResultArray;
class CSearchDocumentId;
class CSearchSnippet;
class CSearchSnippetInfo;
class MSearchFScanObserver;
class CMsgDesC;
class CSearchSnippetCreator;
class MSearchContentSearcher;
class CSearchCondition;
class TMsvSessionEvent;

class TEditorParameters;

/**
 *  This class is the searcher class for the messages
 *
 *  This class searches the contacts using the message server apis
 *
 *  @lib 
 */
class CMessagesSearcher : public CActive, 
						  public MSearchContentSearcher, 
						  public MMsvSessionObserver
{	
public:

enum TSearchField 				// what fields to search
	{
	ESearchFieldAll = 0,    	// all
	ESearchFieldTitleOnly,  	// title only
	ESearchFieldBodyOnly    	// doby only
	};

enum TSearchType 				// what type of message to search
	{
	ESearchTypeSms = 0, 		// sms
	ESearchTypeEmail,   		// email
	ESearchTypeAll,     		// all
	ESearchTypeNone     		// none
	};

enum TSearching 				// what is currently under search
{
	ESearchingBody = 0,     	// sms body
	ESearchingSender,       	// sms sender
	ESearchingSMTPSubject,  	// smtp subject
	ESearchingSMTPSender,   	// smtp sender
	ESearchingSMTPBody,     	// smtp body
	ESearchingPOP3Subject,  	// pop3 subject
	ESearchingPOP3Sender,   	// pop3 sender
	ESearchingPOP3Body,     	// pop3 body
	ESearchingIMAP4Subject, 	// imap4 subject
	ESearchingIMAP4Sender,  	// imap4 sender
	ESearchingIMAP4Body,    	// imap4 body
	ESearchingMMSBody,      	// mms body
	ESearchingMMSSender,     	// mms sender
	ESearchingGenericEmailSubject,  // pop3 subject
	ESearchingGenericEmailSender,   // pop3 sender
	ESearchingGenericEmailBody     // pop3 body
	};



/**
* 1st phase constructor 
*
* @param aContentIdArray - content id
* @param aCondition - condition
* @param aPluginId - Implementation id
* @return returns pointer to the constructed object of type CContactsSearcher
*/
static CMessagesSearcher* NewL( const RArray<TUid>& aContentIdArray, 
								                    const CSearchCondition& aCondition, 
								                    const TUid& aPluginId,
								                    MSearchPluginObserver& aObserver );
/**
* Destructor
*/
~CMessagesSearcher();
   
/**
* Checks the deltion of the entry before launching.
*
* @param aDocumentId Id of the document.
* @ return ETrue if entry is deleted.
*/
TBool IsEntryDeleted( TMsvId& aMsgEntryId );
    
public: //from base class MSearchContentSearcher    
/**
* From MSearchContentSearcher
* Releses the object.
*
*/
void Destroy(); 

/**
 * From MSearchContentSearcher
 * Starts the search. Progress of the search is notified through aObserver. 
 * Notice that this call must be asynchronous.
 *
 * @param aObserver Observer for search progress.
*/
void SearchL( );


/**
* From MSearchContentSearcher 
* Cancels the ongoing search. This call must complete synchronously and no calls for
* observers must be made after this call.
*
*/
void CancelSearch();

/**
* From MSearchContentSearcher 
* Gets the results indicated by the given document ids. Asynchronous. Results will be given
* through callback in MSearchPlugInObserver given in Search request.
* 
* @param aResults Results of the search process. Ownership transfers to caller.
*/
void GetResultsL( const RPointerArray<CSearchDocumentId>& aDocumentIdArray );

/**
* From MSearchContentSearcher 
* Cancels the result retrieval process.
*
*/
void CancelResultsRetrieve();


/**
* From MSearchContentSearcher , through MSearchTextSearcherObserver
* Called when all search criteria are met.
*
* @param aKeyWordPos The character position of the keyword match within the original text
*/
void HitL( TInt aKeyWordPos );


/**
* This funcion returns the launch info corresponding to a particular document id.
*
* @param aDocumentID The document id for which launch info is required.
*/       
HBufC8* LaunchInfoL( const CSearchDocumentId& aDocumentID );
          

/**
* Is called when contentsearcher finishes it's operation or it is unable to
* search further.
* @param aTotalNumOfItems
* @param aSearchResult
*/
void ReportFinishedL( TInt aTotalNumOfItems, TInt aSearchResult = 0 );


public: //from base class MMsvSessionObserver

/**
* From MMsvSessionObserver
*Provides the interface for notification of events from a Message Server session.
* The types of event are given in the enumeration TMsvSessionEvent. Clients must provide an object that implements
* the interface, and set it to be notified through CMsvSession::OpenSyncL() or CMsvSession::OpenASyncL().
* Additional observers can also be added and removed through CMsvSession.
*
* @TMsvSessionEvent  the session event
* @param 
*/
void HandleSessionEventL( TMsvSessionEvent, TAny*, TAny*, TAny* );


void LaunchApplicationL(  const TDesC8& aLaunchInfo );

/**
* Finds the right application id to be started and returns it.
*
* @param        aParams Editor parameters
* @param        aMtmType Result's Mtm type
*
* @return Editor application's uid
*
*/
TUid DiscoverL( const TEditorParameters aParams, TUid aMtmType );


/**
* Reads the parameters ( service type (View/Edit), MtmType) from 
* the opaque data. If type and mtm match, sets aAppUi as
* the application id and returns ETrue. If not found, returns EFalse.
*/ 
TBool TryMatchL( const TEditorParameters aParams, 
                                   TUid aMtmType, 
                                   const TApaAppServiceInfo aInfo, 
                                   TUid& aAppUid );      


protected: // From base class CActive

/**
* From CActive
* Implements cancellation of an outstanding request.
*
* This function is called as part of the active object's Cancel().
*/
void DoCancel() ;

/**
* From CActive
* Handles an active object's request completion event.
*
* The function is called by the active scheduler when a request
* completion event occurs, i.e. after the active scheduler's
* WaitForAnyRequest() function completes.
*
* Before calling this active object's RunL() function, the active scheduler 
* has:
* 	
* 1. decided that this is the highest priority active object with
*   a completed request
*
* 2. marked this active object's request as complete (i.e. the request is no 
*   longer outstanding)
*
* RunL() runs under a trap harness in the active scheduler. If it leaves,
* then the active scheduler calls RunError() to handle the leave.
*
* Note that once the active scheduler's Start() function has been called, 
* all user code is run under one of the program's active object's RunL() or 
* RunError() functions.
*
*/
void RunL();


/**
* From CActive 
* If the RunL function leaves,
* then the active scheduler calls RunError() to handle the leave.
*
*/
TInt RunError( TInt aError );


private:	
/**
* Constructor
*
*
* @param aPluginId
*/
CMessagesSearcher( const TUid&  aPluginId);

/** 
* 2nd phase constructor
*
* @param aContentIdArray
* @param aCondition
*/
void ConstructL( const RArray<TUid>& aContentIdArray, 
				 const CSearchCondition& aCondition,
				 MSearchPluginObserver& aObserver );

/**
* Cleans up the memory held
*
*/
void CleanUp();

/**
* Does actual search
*
*
*/
TBool DoActualSearchL();



/**
* Sets the folder(s) to be searched by the plug-in.
*
*/
void SetTargetFolderL(  );




/**
* Checks all folders that need to be searched.  Folders to be searched are
*  set via SetTargetFolderL.
* Calls AddFolderWithSubFoldersL to add the folder to the array of folders to be
*  searched.
*
* @param
*/
void ListSubFoldersL( const CMsvEntry& aFolderEntry );

/**
* Will add the specified folder to the folder array to be searched.
*  Subfolders of the specified folder are checked by calling ListSubFoldersL.
*
* @param
*/
void AddFolderWithSubFoldersL( const TMsvId& aFolderId );

/**
* Prepares the object for the search. MoveOnToNextMessageL is called
*  to find the 1st message in the to be searched folders.
*
* @return		ETrue		When messages are available
*				EFalse		When 0 messages are available
*/
TBool PrepareForSearchL();

/**
* Moves through all messages under those folders to be searched and returns
*  with ETrue when a message is found.
*
* @return 	ETrue		When a message is found.
*			EFalse		When no more messages are available under the to be
*						 searched folders.
*/
TBool MoveOnToNextMessageL();

/**
* Searches SMS message.
*
* @param
*/
void SearchSmsL( TMsvId& aMessageId );

/**
* Searches MMS message.
*
* @param
*/
void SearchInMmsMessageL(const TMsvId& aMessageId);

/**
* Searches Email message.
*
* @param
*/
void SearchGenericEmailL(  TMsvId& aMessageId, TUid aMessageUid);

/**
* Generates results from a hit in sms message.
*
* @param
*/
void ReportHitInSmsL( TInt aKeyWordPos );

/**
* Generates results from a hit in mms message.
*
* @param
*/
void ReportHitInMmsL( TInt aKeyWordPos );

/**
* Generates results from a hit in email message.
*
* @param
*/
void ReportHitInEmailL( TInt aKeyWordPos );

/**
* Searches for Mtm -class from the array that can be used to open the message specified by aUid
* If proper class is not found, it's created by using method AddIntoMtmArrayL().
*
* @param aUid
*/
CBaseMtm* SearchFromMtmArrayL( TUid aUid );

/**
* Checks if there's any entry in at least one folder
*
* 	
*/
TBool AnyEntriesInAllFolders();

private: // Data

/**
*client for opening sms
*/
CSmsClientMtm*	iSmsMtm;                    

/**
*client for opening mms
*/
CMmsClientMtm*	iMmsMtm;                    

/**
*client for opening emails
*/
CBaseMtm*		iGenericMtm; 
		       
/**
*pointer to ClientMtmRegistry
*/		
CClientMtmRegistry*	iMtmReg;                    

/**
*pointer to message session
*/		
CMsvSession*	iSession;                   

/**
*array containing Mtm -objects used for opening messages.
*/		
RPointerArray<CBaseMtm>	iMtmArray; 

/**
*Array of folders to be searched.
*/		
RArray<TMsvId>  	iToBeSearchedFolders;    
   
/**
*index of folder currently being
*/		
TInt	iToBeSearchedFoldersIdx;	 
																
/**
*reference to the file system
*/		
RFs iFs;                                                   

/**
*folder currently being searched
*/		
CMsvEntry*	iFolderEntry;				 

/**
* Message Entry
*/
CMsvEntrySelection* iEntryDeleted;
/**
*message currently being searched
*/		
CMsvEntry*		iMessageEntry; 

/**
*message currently being searched
*/		
CMsvEntrySelection*	iMessageEntries;			

/**
*Id of messageentry
*/		
TInt	iMessageEntriesIdx;			


/**
*no of matches found
*/		
TInt  iStatusItemCounter;

/**
*no of max entires found
*/		
TInt iTotalNumOfItems;           

/**
*What field is under search. Affects snippet handling.
*/		
TSearching iFieldCurrentlyUnderSearch;  

/**
*arrray contains heavyResults
*/	
RPointerArray<CSearchResult> iHeavyResultsArray; 



/**
*pluginID 
*/		
const TUid	iPluginId;

/**
*Is search cancelled
*/		
TBool	iSearchNotCancelled;  	//when cancelled search..


/**
*Message Description array 
*/		
RPointerArray<CMsgDesC> iMsgDesCArray;

/**
*array informing num messge entries in a folder
*/		
RArray<TInt>  iMsgEntriesInFoldersArray;


/**
* Text searcher
* Own.  
*/
CSearchTextSearcher* iTextSearcher; // Text searcher

/**
* Search Plugin Observer
* Own.  
*/
MSearchPluginObserver*      iObserver;

/**
* The current text being searched 
* Own.  
*/
HBufC* iCurrentTextBeingSearched;
HBufC*  iSenderReciever;
	

/**
* Flags for checking searchcontent
*/
TBool  iHitFound;

CMsvStore* iMsvStore;


};





#endif //C_MESSAGESSEARCHER_H



