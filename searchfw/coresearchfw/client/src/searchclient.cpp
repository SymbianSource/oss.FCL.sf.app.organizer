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
* Description:   Client for search service Client/Server.
*
*/


#include <e32base.h>
#include <s32mem.h>

// INCLUDE FILES
#include <searchclientsession.h>
#include <searchquery.h>
#include <searchlightresult.h>
#include "searchclient.h"
#include "searchclientdefines.h"
const int KBufferLength = 256;
// ====================================== MEMBER FUNCTIONS =========================================

// -------------------------------------------------------------------------------------------------
// RSearchClient::RSearchClient
// Performs the first phase of two phase construction.
// -------------------------------------------------------------------------------------------------
//
RSearchClient::RSearchClient()
    {
    }

// -------------------------------------------------------------------------------------------------
// RSearchClient::Connect
// Connects to the search server
// -------------------------------------------------------------------------------------------------
//
TInt RSearchClient::Connect()
    {
    //Try connecting to the server
    TInt clientsessionError = CreateSession( KSearchServerName, Version(), KDefaultMessageSlots );
    if ( clientsessionError != KErrNone )
        {
        // Could not connect to server, must create thread for server and start server
        RProcess process;
        process.Create( KSearchServerFileName, KNullDesC ); 
            
        TRequestStatus status;
        process.Rendezvous( status );
        // status must be KRequestPending at this stage, if not return some error
        if ( status != KRequestPending )
            {
            process.Kill( 0 );      // abort startup
            process.Close();
            return KErrGeneral;   // Dont return status.Int() here cause it can be KErrNone
            }
        else    // going well, wait for completion
            {
            process.Resume();   // logon OK - start the server
            }
        User::WaitForRequest( status );

        // Some error occured, return error code
        if ( status != KErrNone )
            {
            process.Close();
            return status.Int();
            }

            // New thread was created and server was started, connect to server and return error (if any)
        
            clientsessionError = CreateSession( KSearchServerName, Version(), KDefaultMessageSlots );
        
            #ifdef _DEBUG
            #endif
            process.Close();
            }
        return( clientsessionError );
        
    }

// -------------------------------------------------------------------------------------------------
// RSearchClient::VersionL
// Returns the version number of the server's client.
// -------------------------------------------------------------------------------------------------
//
TVersion RSearchClient::Version() const
    {
    return( TVersion( KSearchServerMajorVersionNumber, KSearchServerMinorVersionNumber,
                                         KSearchServerBuildVersionNumber ) );
    }

// -------------------------------------------------------------------------------------------------
// RSearchClient::SearchL
// (other items were commented in a header).
// -------------------------------------------------------------------------------------------------
//
TInt RSearchClient::SearchL( const CSearchQuery& aSearchQuery,
                    TPckgBuf<TInt>& aSearchFinished, const TSearchThreshold& aThreshold,
                    TRequestStatus& aStatus,TPckgBuf<TInt>& aSearchCount, TPckgBuf<TInt>& aSearchRes )
    {
    //Put iMaxResults,iUpdateGranularity,iUpdateInterval, and aSearchQuery
    //into Dynamic Buf
    CBufFlat* tmpBuf = CBufFlat::NewL( KDefaultBufSize );
    CleanupStack::PushL( tmpBuf );
    RBufWriteStream writestream( *tmpBuf );
    writestream.PushL();    
    
    writestream.WriteInt32L(aThreshold.iMaxResults);
    writestream.WriteInt32L(aThreshold.iUpdateGranularity);
    
    writestream.WriteInt32L((aThreshold.iUpdateInterval.Int()));
    aSearchQuery.ExternalizeL( writestream );
    writestream.CommitL();
    
    //copy contents of Dynamic buf into Des
    HBufC8* searchQueryDes = HBufC8::NewLC( tmpBuf->Size()  ); //to send the query
    TPtr8 searchQueryDesPtr = searchQueryDes->Des();
    tmpBuf->Read( 0, searchQueryDesPtr, tmpBuf->Size() );
    // Send search query to server synchronous
    User::LeaveIfError( SendReceive ( ESetSearchQuery, TIpcArgs( &searchQueryDesPtr ) ) );
    // Give signal to start searching, asynchronous
    SendReceive( ESearch, TIpcArgs(&aSearchFinished, &aSearchCount, 
                                                        &aSearchRes),  aStatus );
    CleanupStack::PopAndDestroy( 3, tmpBuf );
    return KErrNone;   
    }

// -------------------------------------------------------------------------------------------------
// RSearchClient::GetLightResultsL
// (other items were commented in a header).
// -------------------------------------------------------------------------------------------------
//
 void RSearchClient::GetLightResultsL(RPointerArray<CSearchLightResult>& aLightResult)
    {
    //send des of optimal size to server to store light results
    HBufC8* searchLightResultDes = HBufC8::NewLC( KLightResult  );
    TPtr8 searchLightResultDesPtr = searchLightResultDes->Des();
    TInt result = ( SendReceive( EGetLightResults, TIpcArgs( &searchLightResultDesPtr)));       
    
    //if Des is not enough to store light results in server side
    if ( result == KErrOverflow )
        {
        TInt sizeNeeded;
        TPckg<TInt> sizeNeededPackage(sizeNeeded);
        sizeNeededPackage.Copy( *searchLightResultDes );
        CleanupStack::PopAndDestroy( searchLightResultDes );
        //send Des of sufficeint size
        HBufC8* searchResizedLightResultDes = HBufC8::NewLC( sizeNeeded );
        TPtr8 searchResizedLightResultDesPtr = searchResizedLightResultDes->Des();
        result = ( SendReceive( EGetLightResults, TIpcArgs( &searchResizedLightResultDesPtr) ) );                                           
        RDesReadStream readStream;
        CleanupClosePushL( readStream );
        readStream.Open( *searchResizedLightResultDes );
        TInt count = readStream.ReadInt32L();
       //Go into the loop and get results one by one
        for ( TInt i=0 ; i<count; i++ )
            {
            CSearchLightResult *result = CSearchLightResult::NewL(readStream);
            aLightResult.AppendL(result);
        
            }
            CleanupStack::PopAndDestroy( 2,searchResizedLightResultDes );
            User::LeaveIfError(result);
        }
        //if des is enough to store light res
    else
        {
        RDesReadStream readStream;
        CleanupClosePushL( readStream );
        readStream.Open( *searchLightResultDes );
        TInt count = readStream.ReadUint32L();

        //Go into the loop and get results one by one
        for ( TInt i=0 ; i<count; i++ )
            {
            CSearchLightResult *result = CSearchLightResult::NewL(readStream);
            aLightResult.AppendL( result );
            }
        CleanupStack::PopAndDestroy( 2,searchLightResultDes );
        }
    }

// -------------------------------------------------------------------------------------------------
// RSearchClient::ContinueSearch
// (other items were commented in a header).
// -------------------------------------------------------------------------------------------------
//

void RSearchClient::ContinueSearchingLightL( TPckgBuf<TInt>& aSearchFinished, 
                    TRequestStatus& aStatus,TPckgBuf<TInt>& aSearchCount, TPckgBuf<TInt>& aSearchRes )
    {
    // Give signal to continue searching, asynchronous
    SendReceive( EContinueSearch, TIpcArgs( &aSearchFinished, &aSearchCount, 
                                            &aSearchRes ),  aStatus );
    }
 

// -------------------------------------------------------------------------------------------------
// RSearchClient::GetResultsL
// (other items were commented in a header).
// -------------------------------------------------------------------------------------------------
//

TInt RSearchClient::GetResultsL(const RPointerArray<CSearchDocumentId>& aResultIDArray,
                                 TPckgBuf<TInt>& aSearchFinished, TRequestStatus& aStatus
                                 )
    {
    
    //Put aResultIDArray into Dynamic Buff
    CBufFlat* tmpBuf = CBufFlat::NewL( KDefaultBufSize );
    CleanupStack::PushL( tmpBuf );
    RBufWriteStream writestream( *tmpBuf );
    writestream.PushL();    
    writestream.WriteInt32L(aResultIDArray.Count());
    for( TInt i = 0; i < aResultIDArray.Count(); i++ )
        {
            aResultIDArray[i]->ExternalizeL(writestream);
        }
    writestream.CommitL();
    //Copy contents of dynamic Buff into Des
    HBufC8* searchDocumentIDDes = HBufC8::NewLC( tmpBuf->Size()  );
    TPtr8 searchDocumentIDDesPtr = searchDocumentIDDes->Des();
    tmpBuf->Read( 0, searchDocumentIDDesPtr, tmpBuf->Size() );
      //Send aResultIDArray to server  
    SendReceive( EGetDocumentID, TIpcArgs( &searchDocumentIDDesPtr));
    // Ask server to write search results, synchronous 
    SendReceive( EGetSearchResults, TIpcArgs( &aSearchFinished ), aStatus);
    
    CleanupStack::PopAndDestroy( 3, tmpBuf );
    return EResultId;
    }

// -------------------------------------------------------------------------------------------------
// RSearchClient::GetSupportedContentL
// (other items were commented in a header).
// -------------------------------------------------------------------------------------------------
//

void RSearchClient::GetSupportedContentL(RPointerArray<CSearchContent>& aContent,
                                        TUid aServiceId)
    {
    //send des of optimal size to server to store searchcontents.
    TPckgBuf<TInt> serviceIdPckg( aServiceId.iUid );
    HBufC8* searchSupportedTypesDes = HBufC8::NewLC( KMemory );
    TPtr8 searchSupportedTypesDesPtr = searchSupportedTypesDes->Des();
    //send req to sevrver for searchcontents
    TInt result = ( SendReceive( EGetContentTypes, TIpcArgs( &serviceIdPckg, 
                                        &searchSupportedTypesDesPtr) ) );
    //if Des is not enough to store searchcontents in server side
    if ( result == KErrOverflow )
        {
        TInt sizeNeeded;
        TPckg<TInt> sizeNeededPackage(sizeNeeded);
        sizeNeededPackage.Copy( *searchSupportedTypesDes );
        CleanupStack::PopAndDestroy( searchSupportedTypesDes );
        //send Des of sufficeint size
        HBufC8* searchSuppTypResizedDes = HBufC8::NewLC( sizeNeeded );
        TPtr8 searchSuppTypResizedDesPtr = searchSuppTypResizedDes->Des();
        result = SendReceive( EGetContentTypes, TIpcArgs( &serviceIdPckg, 
                                        &searchSuppTypResizedDesPtr) );
        RDesReadStream readStream;
        CleanupClosePushL( readStream );
        readStream.Open( *searchSuppTypResizedDes );
        TInt count = readStream.ReadUint32L();
        //Go into the loop and get supported contents one by one
        for( TInt i=0 ; i<count; i++ )
            {
            CSearchContent *contentresource = CSearchContent::NewL(readStream);
            aContent.AppendL(contentresource);
            }
            CleanupStack::PopAndDestroy( 2,searchSuppTypResizedDes );
            User::LeaveIfError(result);         
            }
    //if des is enough to store contents sent by server
    else
       {
        RDesReadStream readStream;
        CleanupClosePushL( readStream );
        readStream.Open( *searchSupportedTypesDes );
        TInt count = readStream.ReadUint32L();
        //Go into the loop and get supported contents one by one
        for( TInt i=0 ; i<count; i++ )
            {
                CSearchContent *contentresource = CSearchContent::NewL(readStream);
                aContent.AppendL(contentresource);
            }
        CleanupStack::PopAndDestroy( 2,searchSupportedTypesDes );
        }

    }


// -------------------------------------------------------------------------------------------------
// RSearchClient::GetSupportedServicesL
// (other items were commented in a header).
// -------------------------------------------------------------------------------------------------
//

void RSearchClient::GetSupportedServicesL(RArray<TUid>& aServiceIdArray)
    {   
    //send des of optimal size to server to store serviceIDs.
    HBufC8* serviceIDDes = HBufC8::NewLC( KService );
    TPtr8 serviceIDDesPtr = serviceIDDes->Des();
    //Ask server supported services
    TInt result = ( SendReceive( EGetService, TIpcArgs( &serviceIDDesPtr) ) ); 
    //if Des is not enough to store serviceIDs in server side
    if ( result == KErrOverflow )
        {
        TInt sizeNeeded;
        TPckg<TInt> sizeNeededPackage(sizeNeeded);
        sizeNeededPackage.Copy( *serviceIDDes );
        CleanupStack::PopAndDestroy( serviceIDDes );
        //send Des of sufficeint size
        HBufC8* serviceResizedIDDes = HBufC8::NewLC( sizeNeeded );
        TPtr8 serviceResizedIDDesPtr = serviceResizedIDDes->Des();
        result = ( SendReceive( EGetService, TIpcArgs( &serviceResizedIDDesPtr) ) );                                            
        RDesReadStream readStream;
        CleanupClosePushL( readStream );
        readStream.Open( *serviceResizedIDDes );
        TInt count = readStream.ReadInt32L();
        //Go into the loop and get supported services one by one 
        for(TInt i=0 ; i<count; i++)
            {
            TInt num = readStream.ReadInt32L();
            TUid uNum;
            uNum.iUid = num;
            aServiceIdArray.Append(uNum);
            }
        CleanupStack::PopAndDestroy( 2,serviceResizedIDDes );
        User::LeaveIfError(result);
        }
        //if des is enough to store serviceIDs sent by server        
    else
        {
        RDesReadStream readStream;
        CleanupClosePushL( readStream );
        readStream.Open( *serviceIDDes );
        TInt count = readStream.ReadInt32L();
        //Go into the loop and get supported services one by one 
        for(TInt i=0 ; i<count; i++)
        {
            TInt num = readStream.ReadInt32L();
            TUid uNum;
            uNum.iUid = num;
            aServiceIdArray.Append(uNum);
        }
        CleanupStack::PopAndDestroy( 2,serviceIDDes );
        }
    }
     

// -------------------------------------------------------------------------------------------------
// RSearchClient::GetHeavyResultsL
// (other items were commented in a header).
// -------------------------------------------------------------------------------------------------
//

void RSearchClient::GetHeavyResultsL(RPointerArray<CSearchResult>& aSearchHeavyRes)
    {
    
    //send des of optimal size to server to store Heavyresults.
       
    HBufC8* searchResultDes = HBufC8::NewLC( KMemory );
    TPtr8 searchResultDesPtr = searchResultDes->Des();

   //Ask searver to write heavy results
    TInt result = SendReceive( EGetHeavyResults, TIpcArgs( &searchResultDesPtr ));
    //if Des is not enough to store serviceIDs in server side
    if ( result == KErrOverflow )
        {
        TInt sizeNeeded;
        TPckg<TInt> sizeNeededPackage(sizeNeeded);
        sizeNeededPackage.Copy( *searchResultDes );
        CleanupStack::PopAndDestroy( searchResultDes );
        //send Des of sufficeint size
        HBufC8* searchResizedResultDes = HBufC8::NewLC( sizeNeeded );
        TPtr8 searchResizedResultDesPtr = searchResizedResultDes->Des();
        result = SendReceive( EGetHeavyResults, TIpcArgs( &searchResizedResultDesPtr ));
        RDesReadStream readStream;
        CleanupClosePushL( readStream );
        readStream.Open( *searchResizedResultDes );
        TInt count = readStream.ReadInt32L();
       //Go into the loop and get results one by one
        for ( TInt i=0 ; i<count; i++ )
            {
            CSearchResult *res = CSearchResult::NewL(readStream);
            aSearchHeavyRes.AppendL(res);
            }
        CleanupStack::PopAndDestroy( 2,searchResizedResultDes );
        User::LeaveIfError(result);
        }
     //if des is enough to store Heavyresults sent by server        
    else
        {
        RDesReadStream readStream;
        CleanupClosePushL( readStream );
        readStream.Open( *searchResultDes );
        TInt count = readStream.ReadUint32L();
        //Go into the loop and get results one by one
        for ( TInt i=0 ; i<count; i++ )
            {
            CSearchResult *res = CSearchResult::NewL(readStream);
            aSearchHeavyRes.AppendL(res);
            }
        CleanupStack::PopAndDestroy( 2,searchResultDes );
        
        }
   }

// -------------------------------------------------------------------------------------------------
// RSearchClient::LaunchApplicationL
// (other items were commented in a header).
// -------------------------------------------------------------------------------------------------
//

void RSearchClient::LaunchApplicationL( const CSearchDocumentId& aResultId )
    {
    //put aResultId into dynamic buffer
    CBufFlat* tmpBuf = CBufFlat::NewL( KDefaultBufSize );
    CleanupStack::PushL( tmpBuf );
    RBufWriteStream writestream( *tmpBuf );
    writestream.PushL();    
    aResultId.ExternalizeL( writestream );
    writestream.CommitL();
    //copy contents of Dynai=mic buffer into Des
    HBufC8* documentIDDes = HBufC8::NewLC( tmpBuf->Size() );
    TPtr8 documentIDDesPtr = documentIDDes->Des();
    tmpBuf->Read( 0, documentIDDesPtr, tmpBuf->Size() );
    
    //Ask server to launch the application of sepcified Id  
    User::LeaveIfError( SendReceive( ELaunchApp, TIpcArgs( &documentIDDesPtr ) ) ); 
    CleanupStack::PopAndDestroy( 3, tmpBuf );
    }


// -------------------------------------------------------------------------------------------------
// RSearchClient::CancelSearch
// (other items were commented in a header).
// -------------------------------------------------------------------------------------------------
//
void RSearchClient::CancelSearch()
    {
    // Cancel ongoing search, synchronous
    SendReceive( ECancelSearch, TIpcArgs() );
    
    }

// -------------------------------------------------------------------------------------------------
// RSearchClient::CancelResult
// (other items were commented in a header).
// -------------------------------------------------------------------------------------------------
//
void RSearchClient::CancelResult()
    {
    // Cancel ongoing search results, synchronous
    SendReceive( ECancelResults, TIpcArgs() );
    }
    
// -------------------------------------------------------------------------------------------------
// RSearchClient::GetLaunchInfoL
// Get the launch information from search server
// -------------------------------------------------------------------------------------------------
//
HBufC8* RSearchClient::GetLaunchInfoL( const CSearchDocumentId& aDocumentID )
	{
    CBufFlat* tmpBuf = CBufFlat::NewL( KDefaultBufSize );
    CleanupStack::PushL( tmpBuf );
    RBufWriteStream writestream( *tmpBuf );
    writestream.PushL();    
    aDocumentID.ExternalizeL( writestream );
    writestream.CommitL();
    //copy contents of Dynai=mic buffer into Des
    HBufC8* documentIDDes = HBufC8::NewLC( tmpBuf->Size() );
    TPtr8 documentIDDesPtr = documentIDDes->Des();
    tmpBuf->Read( 0, documentIDDesPtr, tmpBuf->Size() );
    HBufC8* launchInfo = HBufC8::NewL( KBufferLength );
    TPtr8 launchInfoPtr = launchInfo->Des();
    SendReceive( EGetLaunchInfo, TIpcArgs( &documentIDDesPtr, &launchInfoPtr ) ) ; 
    TPtr8 info = launchInfoPtr;
    CleanupStack::PopAndDestroy( 3, tmpBuf );
    return launchInfo;
	}

// End of File
