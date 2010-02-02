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
* Description:   Search Server session
*
*/



// INCLUDE FILES
#include <s32mem.h>
#include <searchresult.h>
#include <searchtextsnippet.h>
#include "searchserversession.h"
#include "searchserverdefines.h"
#include "searchserver.h"
#include "searchdocumentid.h"
#include "searchlightresult.h"


// ====================================== MEMBER FUNCTIONS =========================================

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::NewL
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSearchServerSession* CSearchServerSession::NewL( CSearchServer& aServer, CSearchPluginInterface * aSearchPluginInterface )
    {
    CSearchServerSession* self = CSearchServerSession::NewLC( aServer, aSearchPluginInterface );
    CleanupStack::Pop( self );
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::NewLC
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSearchServerSession* CSearchServerSession::NewLC( CSearchServer& aServer, CSearchPluginInterface * aSearchPluginInterface )
    {
    CSearchServerSession* self = new( ELeave ) CSearchServerSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL( aSearchPluginInterface );
    return self;
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::~CSearchServerSession
// Destructor
// -------------------------------------------------------------------------------------------------
//
CSearchServerSession::~CSearchServerSession()
    {
    TBool isHeavyResults = EFalse;
    //Check for heavy res asked
    if ( iSearchManager )
        isHeavyResults = iSearchManager->IsHeavyResultsAsked();

    //delete manager
    if ( iSearchManager )
        {
        delete iSearchManager;
        iSearchManager = NULL;		
        }

    //delete query
    idocIdArray.ResetAndDestroy();

        iLightResultBackup.Reset();
        
     
   
    //Reset iLightResult
    iLightResult.Reset();

    //Reset iResultArray
    iResultArray.Reset();

    //Reset iSupportedContent
    iSupportedContent.ResetAndDestroy(); 

    //Reset iServiceIdArray
    iServiceIdArray.Reset();

    //if there is a Message complete 
    if ( !iMessage.IsNull() )
        {
        iMessage.Complete( KErrNone );
        }

    //Inform that session has been destroyed
    TRAP_IGNORE(iSearchServer.SessionDestroyedL());
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::ErrorHappened
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//
void CSearchServerSession::ErrorHappened( const TInt& aErrorCode )
    {
    // must do cleanup if error happend while results are being fetched casue old results 
    // must be deleted
    iErrorBuffer = aErrorCode;
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::ServiceL
// Handles the servicing of a client request that has been passed to the server.
// Other items are included in a header
// -------------------------------------------------------------------------------------------------
//
void CSearchServerSession::ServiceL( const RMessage2& aMessage )
    {
    DispatchMessageL( aMessage );
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::ResultFoundL
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//   
void CSearchServerSession::ResultFoundL( RPointerArray<CSearchLightResult>& aResult, TInt aCurrentDocument,
                       TInt aTotalDocuments  )
    {
    //Copy the currentDoc and Total Document
    iCurrentDocument = aCurrentDocument;
    iTotalDocuments = aTotalDocuments;
    // Inform client that data is available
    SignalClientL( aResult, aCurrentDocument ); 
    //Inform the manager that Message is Available/NotAvailable
    iSearchManager->SetFlagL( !iMessage.IsNull() );
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::SearchFinishedL
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//   
void CSearchServerSession::SearchFinishedL( TInt aSearchResult, TInt aTotalNumberOfResults  )
    {

    iFullyFinishedBuffer = EFinished;
    //Inform the client that search has been completed
    SignalClientL(aSearchResult,aTotalNumberOfResults);
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::ResultsRetrieveCompleteL
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//   
void CSearchServerSession::ResultsRetrieveCompleteL( RPointerArray<CSearchResult>& aResultArray )
    {
    //Copy the Heavy results into iResultArray
    for (TInt i =0 ;i< aResultArray.Count(); i++)
        {
        iResultArray.AppendL(aResultArray[i]);
        }
    //Send heavy results to client
    SignalClientL( ); // Inform client that data is available
    //Inform the manager that Message is Available/NotAvailable
    iSearchManager->SetFlagL( !iMessage.IsNull() );
    }
    
// -------------------------------------------------------------------------------------------------
// CSearchServerSession::CSearchServerSession
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
CSearchServerSession::CSearchServerSession( CSearchServer& aServer )
:   iFullyFinishedBuffer(ENotFinished),
    iGetContentTypes(ETrue),
    iGetServiceId(ETrue),
    iSearchServer( aServer )
    {

    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::ConstructL
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//
void CSearchServerSession::ConstructL( CSearchPluginInterface * aSearchPluginInterface )
    {
    //Create Manager
    iSearchManager = CSearchManager::NewL( (*this ), aSearchPluginInterface );
    //Create the server session
    iSearchServer.SessionCreatedL( );

    }


// -------------------------------------------------------------------------------------------------
// CSearchServerSession::DispatchMessageL
// Services a clients request.
// -------------------------------------------------------------------------------------------------
//
void CSearchServerSession::DispatchMessageL( const RMessage2& aMessage )
    {
    TInt error = KErrNone ;
    //Get the Value of the Function requested by the client
    switch ( aMessage.Function() )
        {
        //To set the Query
        case ESetSearchQuery:   
            {
            SetSearchQueryL( aMessage );	

            }
            break;

        //Start Search Has been requested..so invoke the start search function	
        case ESearch:   
            {
            SearchL( aMessage );	
            }
            return; // Return here so that Complete() isn't called on the message

            //The client wants to get the results, hence give the results back

        case EGetSearchResults: // Synchronous
            {
            GetSearchResultsL( aMessage );
            }
            return; //Should not call Complete() as we will have to send ResultRetrieve

        //Client requests Content types
        case EGetContentTypes:  // Synchronous
            {
            GetContentTypesL( aMessage );
            }
            return;

        //Client requests Services
        case EGetService:
            {
            GetServiceIdL( aMessage );
            }
            return;

        //Client requests Cancel ongoing search
        case ECancelSearch: //Synchronous
            {
            CancelSearchL( aMessage);	
            }
            break;

        //Client requests Cancel heavy results asked
        case ECancelResults:
            CancelResult( aMessage );
            break;

        //Client askes to continue search
        case EContinueSearch:   
            {
            ContinueSearchL( aMessage );
            }
            return; 

        //Client asks light results
        case EGetLightResults:
            {
            GetLightResultsL(aMessage);   
            }
            return;

        //Client asks haevy results
        case EGetHeavyResults:
            {
            GetHeavyResultsL(aMessage);   
            }
            return;

        //Client asks DocID
        case EGetDocumentID:
            GetDocumentIDArrayL(aMessage);
            break;

        //Client asks Launch application
        case ELaunchApp:
            TRAP( error, LaunchApplicationL(aMessage) );
            break;
        //Client request Launch information
        case EGetLaunchInfo:
        	GetLaunchInfoL( aMessage );
        	break;
        //In case Search Query has to be set, then the SearchQuery function is involked
        default:	// Dont know this request, thus panic client
            //PanicClient( EBadRequest );
            //return;
            break;
        }
    //Duplicate the specified handle in the client thread, with the completion code
    aMessage.Complete( error );
    }
    
// -------------------------------------------------------------------------------------------------
// CSearchServerSession::GetServiceIdL
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
// 
void CSearchServerSession::GetServiceIdL( const RMessage2& aMessage ) 
    {       
    //dont ask supported services if already got
    if ( iGetServiceId )
        {
        iGetServiceId = EFalse;	
        //Ask supported services to manager
        iSearchManager->GetSupportedServicesL( iServiceIdArray );		
        }

    TInt numofresults = iServiceIdArray.Count();

    //Put all IDs into Des
    CBufFlat* tmpBuf = CBufFlat::NewL( KDefaultSerBufSize );
    CleanupStack::PushL( tmpBuf );
    RBufWriteStream writestream( *tmpBuf );
    writestream.PushL();	
    writestream.WriteInt32L(numofresults);
    for (TInt index = 0; index< numofresults; index++)
        {
        writestream.WriteInt32L( iServiceIdArray[index].iUid );
        }
    writestream.CommitL();

    HBufC8* serviceIDDes = HBufC8::NewLC( tmpBuf->Size() );
    TPtr8 serviceIDDesPtr = serviceIDDes->Des();
    tmpBuf->Read( 0, serviceIDDesPtr, tmpBuf->Size() );

    if (aMessage.GetDesMaxLengthL(0) < serviceIDDes->Size())
        {
        //if Des of Client is too small to send the IDs send length instead
        TInt bufferSize = serviceIDDes->Size();
        TPckgC<TInt> bufferSizePackage(bufferSize);
        aMessage.WriteL(0, bufferSizePackage);
        aMessage.Complete(KErrOverflow);
        }
    else
        {
        //send service IDs to client
        aMessage.WriteL(0, *serviceIDDes);
        aMessage.Complete(KErrNone);
        }

    // tempBuffer, stream, buffer	    
    CleanupStack::PopAndDestroy(3, tmpBuf); 

    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::GetContentTypesL
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
// 
void CSearchServerSession::GetContentTypesL( const RMessage2& aMessage )
    {
    //dont ask supported contents if already got
    if ( iGetContentTypes )
        {
        iGetContentTypes = EFalse;	
        TPckgBuf<TInt> serviceIdPckg;	
        aMessage.ReadL( 0, serviceIdPckg );	
        TUid serviceid;
        serviceid.iUid =  serviceIdPckg();
        //Ask supported Contents to manager       
        iSearchManager->GetSupportedContentClassesL( iSupportedContent, serviceid );		
        }

    //Put all Ids contents into Des
    CBufFlat* tmpBuf = CBufFlat::NewL( KDefaultSerBufSize );
    CleanupStack::PushL( tmpBuf );
    RBufWriteStream writestream( *tmpBuf );
    writestream.PushL();	

    TInt numofresults = iSupportedContent.Count();
    writestream.WriteInt32L(numofresults);
    for(TInt index = 0; index< numofresults; index++)
        {
        iSupportedContent[index]->ExternalizeL( writestream );
        }

    writestream.CommitL();

    HBufC8* searchContentArrayDes = HBufC8::NewLC( tmpBuf->Size() );
    TPtr8 searchContentArrayDesPtr = searchContentArrayDes->Des();
    tmpBuf->Read( 0, searchContentArrayDesPtr, tmpBuf->Size() );

    TPtr8 pbuffer(searchContentArrayDes->Des());


    if (aMessage.GetDesMaxLengthL(1) < searchContentArrayDes->Size())
        {
        //If clent Buffer length is too less to put contents send size instead
        TInt bufferSize = searchContentArrayDes->Size();
        TPckgC<TInt> bufferSizePackage(bufferSize);
        aMessage.WriteL(1, bufferSizePackage);
        aMessage.Complete(KErrOverflow);
        }
    else
        {
        //Put contents into Client buffer
        aMessage.WriteL(1, *searchContentArrayDes);
        aMessage.Complete(KErrNone);
        }
    CleanupStack::PopAndDestroy(3, tmpBuf); // tempBuffer, stream, buffer	
    }
    
// -------------------------------------------------------------------------------------------------
// CSearchServerSession::SetSearchQueryL
// Other Items are commented in a header
// -------------------------------------------------------------------------------------------------
// 
void CSearchServerSession::SetSearchQueryL( const RMessage2& aMessage )
    {
    iMessage = aMessage;
    //First check if the input is valid   	
    TInt len = aMessage.GetDesLength( 0 ); 

    if ( len < 0 )
        {
        PanicClient( KErrBadDescriptor );	
        }

    //Get the Query First    
    //Create a buffer to restore the query from the Message		 
    HBufC8* searchQueryDes = HBufC8::NewLC( len);

    //Get the pointer to the descriptor of the buffer
    TPtr8 searchQueryDesPtr = searchQueryDes->Des();

    aMessage.ReadL( 0, searchQueryDesPtr );	

    // Internalize the search result array.
    RDesReadStream readStream;
    CleanupClosePushL( readStream );
    readStream.Open( searchQueryDesPtr );


    iMaxResults = readStream.ReadInt32L();
    iGranularity = readStream.ReadInt32L();
    iInterval = readStream.ReadInt32L();
    //Make the Query
    iSearchQuery = CSearchQuery::NewL( readStream );    

    //Delete the allocated memory
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy(searchQueryDes);
    //CleanupStack::PopAndDestroy( searchQueryDes );


    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::SearchL
// Other Items are commented in a header
// -------------------------------------------------------------------------------------------------
// 
void CSearchServerSession::SearchL( const RMessage2& aMessage )
    {

    //Clean up the resources needed before the next search
    iLightResultBackup.Reset();	
    idocIdArray.ResetAndDestroy();
    iLightResult.Reset();
    iResultArray.Reset();
    iMessage = aMessage;
    //Ask Manager to cleanup the resources for Next Search
    iSearchManager->CleanupResourcesL();
    //Inform the manager that Message is Available/NotAvailable
    iSearchManager->SetFlagL( !iMessage.IsNull() );
    //Ask Manager to satrt the Search
    iSearchManager->SearchL( iSearchQuery, iGranularity ,iMaxResults, iInterval );
	if(iSearchQuery)
	{
	
	 delete iSearchQuery;
     iSearchQuery = NULL;	
	}
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::GetLightResultsL
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
// 
void CSearchServerSession::GetLightResultsL( const RMessage2& aMessage )
    {
    //Put number of LigthRes, LightRes into Dynamic buffer  
    TInt numofresults = iLightResult.Count();
    CBufFlat* tmpBuf = CBufFlat::NewL( KDefaultSerBufSize );
    CleanupStack::PushL( tmpBuf );
    RBufWriteStream writestream( *tmpBuf );
    writestream.PushL();	
    writestream.WriteInt32L(numofresults);
    for (TInt index = 0; index< numofresults; index++)
        {
        iLightResult[index]->ExternalizeL( writestream );
        }
    writestream.CommitL();
    //Copy LigthRes, LightRes into Des from Dynamic buffer
    HBufC8* searchLightResultsDes = HBufC8::NewLC( tmpBuf->Size() );
    TPtr8 searchLightResultsDesPtr = searchLightResultsDes->Des();
    tmpBuf->Read( 0, searchLightResultsDesPtr, tmpBuf->Size() );

    if (aMessage.GetDesMaxLengthL(0) < searchLightResultsDes->Size())
        {   
        //Send the size of the Light Res if size of the Des provided by
        //client is not enough	     
        TInt bufferSize = searchLightResultsDes->Size();
        TPckgC<TInt> bufferSizePackage(bufferSize);
        aMessage.WriteL(0, bufferSizePackage);
        aMessage.Complete(KErrOverflow);
        }
    else
        {
        //send the light results to cleint
        aMessage.Write( 0, *searchLightResultsDes );
        aMessage.Complete(KErrNone);
        iLightResult.Reset();	    	
        }
    CleanupStack::PopAndDestroy( 3, tmpBuf );


    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::ContinueSearchL
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
// 
void CSearchServerSession::ContinueSearchL( const RMessage2& aMessage )
    {
    iMessage = aMessage;

    if (iFullyFinishedBuffer == EFinished)
        {
        //if seach is finished inform client
        TPckgBuf<TInt> fullyFinishedPckg( iFullyFinishedBuffer );

        TPckgBuf<TBool> searchresultPckg( 0 );

        iMessage.Write(0,fullyFinishedPckg);
        iMessage.Write(1,searchresultPckg);
        iMessage.Complete(iErrorBuffer);
        }
    //Inform the manager that Message is Available/NotAvailable
    iSearchManager->SetFlagL( !iMessage.IsNull() );
    }

	// -------------------------------------------------------------------------------------------------
// CSearchServerSession::GetDocumentIDArrayL
// Other Items are commented in a header
// -------------------------------------------------------------------------------------------------
// 
void CSearchServerSession::GetDocumentIDArrayL( const RMessage2& aMessage )
    {
    iMessage = aMessage;

       idocIdArray.ResetAndDestroy();

    //First check if the input is valid   	
    TInt len = aMessage.GetDesLength( 0 ); 

    if ( len < 0 )
        {
        PanicClient( KErrBadDescriptor );	
        }

    //Get the Query First    
    //Create a buffer to restore the query from the Message		 
    HBufC8* searchQueryDes = HBufC8::NewLC( len);

    //Get the pointer to the descriptod of the buffer
    TPtr8 searchQueryDesPtr = searchQueryDes->Des();

    aMessage.ReadL( 0, searchQueryDesPtr );	

    // Internalize the search result array.
    RDesReadStream readStream;
    CleanupClosePushL( readStream );
    readStream.Open( searchQueryDesPtr );
    TInt count = readStream.ReadInt32L();
    for(TInt i = 0; i<count; i++)
        {
        CSearchDocumentId* docId = CSearchDocumentId::NewL( readStream );
        idocIdArray.AppendL( docId );
        }
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy(searchQueryDes);

    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::GetSearchResultsL
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
// 
void CSearchServerSession::GetSearchResultsL( const RMessage2& aMessage )
    {
    iMessage = aMessage;
    //Send docIDs to manager to get Heavy results
    iSearchManager->GetResultsL( idocIdArray );
    }
    

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::GetHeavyResultsL
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
// 
void CSearchServerSession::GetHeavyResultsL( const RMessage2& aMessage )
    {
    //Put count and heavy results in Dynamic buffer    
    TInt numofresults = iResultArray.Count();	
    CBufFlat* tmpBuf = CBufFlat::NewL( KDefaultSerBufSize );
    CleanupStack::PushL( tmpBuf );
    RBufWriteStream writestream( *tmpBuf );
    writestream.PushL();	
    writestream.WriteInt32L(numofresults);

    for ( TInt index = 0; index< numofresults; index++ )
        {
        //resultStreamSize += aSupportedContentClasses[index]->StreamSize();
        iResultArray[index]->ExternalizeL( writestream );
        }
    writestream.CommitL();

    HBufC8* searchHeavyResultsDes = HBufC8::NewLC( tmpBuf->Size() );

    //Get the pointer to the descriptor of the buffer
    TPtr8 searchHeavyResultsDesPtr = searchHeavyResultsDes->Des();

    tmpBuf->Read( 0, searchHeavyResultsDesPtr, tmpBuf->Size() );

    if (aMessage.GetDesMaxLengthL(0) < searchHeavyResultsDes->Size())
        {
        //If size of the Des is too small to send the res to client
        //send size
        TInt bufferSize = searchHeavyResultsDes->Size();
        TPckgC<TInt> bufferSizePackage(bufferSize);
        aMessage.WriteL(0, bufferSizePackage);
        aMessage.Complete(KErrOverflow);
        }
    else
        {
        //send heavy res to client
        aMessage.Write( 0, *searchHeavyResultsDes );
        aMessage.Complete(KErrNone);
        iResultArray.Reset();
        }

    CleanupStack::PopAndDestroy( 3, tmpBuf );


    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::CancelSearchL
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//   
void CSearchServerSession::CancelSearchL( const RMessage2&)
    {

    //Ask manager to cancel the search
    iSearchManager->Cancel();
    //Ask manager to complette the seach after cancelling it  
    iSearchManager->SearchCompleteAfterCancelL();
    if( !iMessage.IsNull() )
    iMessage.Complete( KErrNone );  
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::CancelResult
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//   
void CSearchServerSession::CancelResult( const RMessage2& )
    {

    //Ask manager to cancel the heavy res
    iSearchManager->CancelResultsRetrieve();
    if( !iMessage.IsNull() )
    iMessage.Complete( KErrNone );

    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::LaunchApplication
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//    
void CSearchServerSession::LaunchApplicationL( const RMessage2& aMessage )
    {
    TInt len = aMessage.GetDesLength( 0 ); 

    if ( len < 0 )
        {
        PanicClient( KErrBadDescriptor );	
        }
    //Create a buffer to restore the query from the Message		 
    HBufC8* searchContentDes = HBufC8::NewLC( len );

    //Get the pointer to the descriptod of the buffer
    TPtr8 searchContentDesPtr = searchContentDes->Des();
    aMessage.ReadL( 0, searchContentDesPtr );
    // Internalize the search result array.
    RDesReadStream readStream;
    CleanupClosePushL( readStream );
    readStream.Open( searchContentDesPtr );
    CSearchDocumentId* documentId = CSearchDocumentId::NewL(readStream);
    //Ask manager to Lauch the application        
    iSearchManager->LaunchApplicationL( *documentId );
    delete documentId;

    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy(searchContentDes);

    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::SignalClient
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//
void CSearchServerSession::SignalClientL(TInt aSearchResult, TInt aTotalDocument)
    {
    //If there is an outstanding req complete the request 
    if ( !iMessage.IsNull() )
        {   
        //Send back iFullyFinishedBuffer and search result count to client
        iFullyFinishedBuffer = EFinished;
        TPckgBuf<TInt> fullyFinishedPckg( iFullyFinishedBuffer );

        TPckgBuf<TBool> searchresultPckg( aSearchResult );

        TPckgBuf<TInt> totaldocPckg( aTotalDocument );

        RDesWriteStream writestream;

        CleanupClosePushL( writestream );
        //fullyFinishedPckg for search finished or not
        iMessage.Write( 0, fullyFinishedPckg );         // Search fully finished flag
        //searchresultPckg for search res count
        iMessage.Write( 1, searchresultPckg);         
        iMessage.Complete( iErrorBuffer );      // If error occured, we inform about it
        CleanupStack::PopAndDestroy( &writestream );
        iRequestOutstanding = EFalse;

        iErrorBuffer = KErrNone;
        }

    }


// -------------------------------------------------------------------------------------------------
// CSearchServerSession::SignalClient
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//
void CSearchServerSession::SignalClientL( RPointerArray<CSearchLightResult>& aResult, TInt aSearchResult)
    {
    //If there is an outstanding req complete the request
    if ( !iMessage.IsNull() )
        {
        iFullyFinishedBuffer = ENotFinished;
        TPckgBuf<TInt> fullyFinishedPckg( iFullyFinishedBuffer );
        TPckgBuf<TInt> searchresPckg( aSearchResult );
        for(TInt i =0 ;i<aResult.Count();i++)
            {
            iLightResult.AppendL(aResult[i]);
            iLightResultBackup.AppendL(aResult[i]);
            }

        TInt count = aResult.Count();
        TPckgBuf<TInt> resultcountPckg( count );
        //fullyFinishedPckg for search finished or not
        iMessage.Write( 0, fullyFinishedPckg );  
        //search res count
        iMessage.Write( 1, resultcountPckg ); 
        //total res
        iMessage.Write( 2, searchresPckg );  
        iMessage.Complete( iErrorBuffer );      // If error occured, we inform about it
        iRequestOutstanding = EFalse;
        iErrorBuffer = KErrNone;
        }
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::SignalClient
// Other Items are commented in the header
// -------------------------------------------------------------------------------------------------
//
void CSearchServerSession::SignalClientL( )
    {
    //If there is an outstanding req complete the request
    if ( !iMessage.IsNull() )
        {

        iFullyFinishedBuffer = EResult;
        TPckgBuf<TInt> fullyFinishedPckg( iFullyFinishedBuffer );
        //fullyFinishedPckg for search finished or not
        iMessage.Write( 0, fullyFinishedPckg ); 

        iMessage.Complete( iErrorBuffer );      // If error occured, we inform about it

        iRequestOutstanding = EFalse;

        iFullyFinishedBuffer = EFalse;

        iErrorBuffer = KErrNone;
        }
    }	
    
// -------------------------------------------------------------------------------------------------
// CSearchServerSession::PanicClient
// Panics the client.
// -------------------------------------------------------------------------------------------------
//

void CSearchServerSession::PanicClient( TInt /*aPanic*/ )
    {
    //Message().Panic( KSearchServerPanic , aPanic );// Message()????
    }

// -------------------------------------------------------------------------------------------------
// CSearchServerSession::GetLaunchInfoL
// Get the launch infor of specified file
// -------------------------------------------------------------------------------------------------
//
void CSearchServerSession::GetLaunchInfoL( const RMessage2& aMessage )
	{
    TInt len = aMessage.GetDesLength( 0 ); 
    if ( len < 0 )
        {
        PanicClient( KErrBadDescriptor );	
        }
    HBufC8* searchContentDes = HBufC8::NewLC( len );
    TPtr8 searchContentDesPtr = searchContentDes->Des();
    aMessage.ReadL( 0, searchContentDesPtr );
    RDesReadStream readStream;
    CleanupClosePushL( readStream );
    readStream.Open( searchContentDesPtr );
    CSearchDocumentId* documentId = CSearchDocumentId::NewL(readStream);
    TPtr8 launchInfoPtr  = iSearchManager->GetLaunchInfoL( *documentId )->Des();
    aMessage.WriteL( 1, launchInfoPtr );
    HBufC8* launchInfo = iSearchManager->GetLaunchInfoL( *documentId );
    delete launchInfo ;
    launchInfo = NULL;
    delete documentId;
    documentId = NULL;
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy( searchContentDes );
	}
// End of File
