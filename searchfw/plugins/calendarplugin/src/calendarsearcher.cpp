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
* Description:   Plugin for calendarplugin information search.
*
*/



//System Includes
#include <utf.h>
#include <calinstance.h>
#include <calentry.h>
#include <calalarm.h>
#include <searchcondition.h>
#include <searchtextsearcher.h>
#include <searchpluginobserver.h>
#include <searchresult.h>
#include <searchlightresult.h>
#include <searchcommon.h>
#include <searchdocumentid.h>
#include <avkon.rsg>
#include <barsc.h>
#include <barsread.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

#include "calendarsearcher.h"
#include "calentrydetails.h"
#include "calendarsearchpluginpanic.h"
#include "defines.h"

const int KBufferLength = 60;
const int KBigBufferLength = 120;
_LIT( KAvkonResoureFilePath, "avkon.Rsc" );
_LIT( KEmptySpace, " " );
const int KHour = 23;
const int KMinute = 59;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalendarSearcher::NewL
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CCalendarSearcher* CCalendarSearcher::NewL( const RArray<TUid>& aContentIdArray, 
                                            const CSearchCondition& aCondition, 
                                            const TUid& aPluginId,
                                            MSearchPluginObserver& aObserver  )
    {
        CCalendarSearcher* self = new( ELeave )CCalendarSearcher( aPluginId );
        CleanupStack::PushL ( self );
        self->ConstructL( aContentIdArray, aCondition,aObserver);
        CleanupStack::Pop( self );
        return self;
    }

// -----------------------------------------------------------------------------
// CCalendarSearcher::CCalendarSearcher
// Destructor.
// -----------------------------------------------------------------------------
//
CCalendarSearcher::~CCalendarSearcher()
    {
        iFsSession.Close();
        
       
        delete iTextSearcher;
        iTextSearcher = NULL;
        
        delete iReadDateBuffer;
        iReadDateBuffer = NULL;
        
        
        delete iReadTimeBuffer;
        iReadTimeBuffer = NULL;  
        
        if(0 < iHeavyResultsArray.Count())
	    {
	    iHeavyResultsArray.Reset();		
	    }
	    
	    iCalDesCArray.ResetAndDestroy();   
	    iCalLocalUidArray.Close();   
	    
	    delete iEntry;	    
   			
		delete iEntryView;		
		delete iInstanceView;
        delete iSession;
        iSession = NULL;  
    }

// -----------------------------------------------------------------------------
// CCalendarSearcher::GetIfEntryExists
// Check if the entry exists in calander
// -----------------------------------------------------------------------------

TBool CCalendarSearcher::GetIfEntryExistsL (TCalLocalUid aId)
    {
    
    CCalEntry * temp = iEntryView->FetchL(aId);
    if(temp)
    {
    	delete temp;
    	temp = NULL;
    	return ETrue;
    }
    return EFalse;
   

   }
// CCalendarSearcher::StartSearchL
// Start search, set active
// -----------------------------------------------------------------------------
// 
void CCalendarSearcher::StartSearchL() 
    {
        if( iNeedToCallReportFinishedL )
        {
            ReportFinishedL( iErrorCode );
            return;
        }
 		
		// set the active object in pending state until the CCalEntryView is created
		iStatus = 	KRequestPending; // change Kiran
		SetActive();        
    }

// -----------------------------------------------------------------------------
// CCalendarSearcher::Destroy
// Performs the Cleanup operations.
// -----------------------------------------------------------------------------
//
void CCalendarSearcher::Destroy()
    {
        iSearchNotCancelled = EFalse;
    if(IsActive())
    {
    CActive::Cancel();
    }
    else
    {
    	Cleanup();
    }
    
    
    if(0 < iHeavyResultsArray.Count())
	    {
	    iHeavyResultsArray.ResetAndDestroy();		
	    }
	    
	iCalDesCArray.ResetAndDestroy();            
	delete this;
    } 

// -----------------------------------------------------------------------------
// CCalendarSearcher::SearchL
// Performs a Search Operation
// -----------------------------------------------------------------------------
//
void CCalendarSearcher::SearchL()
{
    
    iSearchNotCancelled = ETrue;
    iCalDesCArray.ResetAndDestroy();
       
    if(0 < iHeavyResultsArray.Count())
	    {
	    iHeavyResultsArray.ResetAndDestroy();		
	    }
    
    // Check if plug-in was constructed for searching and if not panic
    __ASSERT_ALWAYS( this, User::Panic( KCalendarSearchPlugIn,
                ECalendarSearchPlugInWrongConstructionParameters ) );

    if( !iFilter )
         {
            ReportFinishedL( KErrNone );
            return;
         }

    // ASSERT_ here when already searching
    __ASSERT_ALWAYS( !IsSearching(), User::Panic( KCalendarSearchPlugIn, 
                                                ECalendarSearchPlugInActiveSearchingExist) );

    if( iSetParamsDone )
    {
        InitializationL();
    }
    else
    {
        ReportFinishedL( iErrorCode );
    }
}
           
// -----------------------------------------------------------------------------
// CCalendarSearcher::IsSearching
// Checks wheather a Search Operation is going or not.
// -----------------------------------------------------------------------------
//
TBool CCalendarSearcher::IsSearching()
    {
        return IsActive();
    }
 
// -----------------------------------------------------------------------------
// CCalendarSearcher::CancelSearch
// Canels the Search operation.
// -----------------------------------------------------------------------------
//
void CCalendarSearcher::CancelSearch()
{
    iSearchNotCancelled = EFalse;
    CActive::Cancel();
   	Cleanup();
}

// -----------------------------------------------------------------------------
// CCalendarSearcher::GetResultsL
// Results are passed in aDocumentIdArray.
// ---------------------------------------------------------f--------------------
//      
void CCalendarSearcher::GetResultsL( const RPointerArray<CSearchDocumentId>& aDocumentIdArray )
    {
        RPointerArray<CSearchResult> aSearchResultArray;
        _LIT8(KText,"MIME/TEXT");
        TBufC8<KBufferLength> tempBuf(KText);
            
        for( TInt i = 0 ; i < aDocumentIdArray.Count() ; i++ )
        {
            for( TInt j = 0 ; j < iHeavyResultsArray.Count() ; j++ )
                {     
                
                if( (aDocumentIdArray[i]->DocumentId() == 
	    				 (iHeavyResultsArray[j]->Result()).DocumentId().DocumentId() ) && 
	    				 (aDocumentIdArray[i]->PluginId().iUid == iPluginId.iUid ) &&
	    				 (aDocumentIdArray[i]->RepositoryId() == KSearchCClassCalendarUid.iUid)
	    				 )
                
                {	
                    CSearchResult* tempHeavy = 	iHeavyResultsArray[j];
                       if((&tempHeavy->Title() != NULL) && (&tempHeavy->Snippet() != NULL))
                     {
                        aSearchResultArray.Append( iHeavyResultsArray[j]);
                        continue;	
                     }
                   
                    TBufC8<KBufferLength> tempString;
                    TPtr8 tempPtr8( tempString.Des() );
                    CnvUtfConverter::ConvertFromUnicodeToUtf8( tempPtr8,
                                                 *( iCalDesCArray[j]->GetSnippet()) );                     
                            
                    HBufC* tmpTitlte = (iCalDesCArray[j]->GetSummary())->AllocL();
                    if( tmpTitlte->Length() == 0 ) 
                    {
                        _LIT(KUnNamed, "<Unnamed>");
                        TBufC<20>  tmpBufTitle(KUnNamed);
                        tmpTitlte = tmpBufTitle.AllocL();
                    }
                                    
                    iHeavyResultsArray[j]->SetTitleL( *tmpTitlte );
                    delete tmpTitlte;
                    tmpTitlte = NULL;                                           
                    iHeavyResultsArray[j]->SetSnippetL( tempPtr8 );          
                    iHeavyResultsArray[j]->SetSnippetFormatL( tempBuf.Des() );
                    aSearchResultArray.AppendL( iHeavyResultsArray[j] );
                }
                
                }
          }
        
        iObserver->ResultsRetrieveCompleteL( aSearchResultArray );
        aSearchResultArray.Reset();
    }

// -----------------------------------------------------------------------------
// CCalendarSearcher::CancelResultsRetrieve
// Cancels the operation of result retrieval.
// -----------------------------------------------------------------------------
//
void CCalendarSearcher::CancelResultsRetrieve()
{
    
} 

// -----------------------------------------------------------------------------
// CCalendarSearcher::GetSearchProgressL
// 
// -----------------------------------------------------------------------------
//  
void CCalendarSearcher::GetSearchProgressL( TUid& aContentClassId,
                                            TInt& aCurrentDocument,
                                            TInt& aTotalDocuments )   
    {
        if( aContentClassId == KSearchCClassCalendarUid )
        {
            aCurrentDocument = iCurrentDocumentSearching;
            aTotalDocuments = iTotalNumOfDocumentsSearched;
        }
    }

// -----------------------------------------------------------------------------
// CCalendarSearcher::HitL
// Called by TextObserver when Keyword is found.
// -----------------------------------------------------------------------------
//
void CCalendarSearcher::HitL( TInt aKeyWordPos )
{
    if( isFirstKeyWordHit && aKeyWordPos!= KErrNotFound )
    {   
        
        _LIT8( KNumber,               "%d" ); 
        HBufC8* buf8 = HBufC8::NewLC( KUidLength );
        buf8->Des().Format( KNumber,  iEntry->LocalUidL() );
        CSearchDocumentId* documentId = CSearchDocumentId::NewL( iEntry->LocalUidL(), iPluginId );
        documentId->SetRepositoryIdL( KSearchCClassCalendarUid.iUid);
        
        CSearchLightResult* searchLightResult = CSearchLightResult::NewL( documentId );     
        searchLightResult->SetContentClassId( KSearchCClassCalendarUid );       
        
        iTotalNumOfDocumentsSearched++;
        
        if( iEntry->EntryTypeL() == CCalEntry::EAnniv )
        {
            searchLightResult->SetContentFolderId( KSearchCFolderAnniversaryUid );
        }
        
        if( ( iEntry->EntryTypeL() == CCalEntry::EAppt ) ||
            ( iEntry->EntryTypeL() == CCalEntry::EReminder ) )
        {
            searchLightResult->SetContentFolderId( KSearchCFolderMeetingsUid );
        }
        
        if(  iEntry->EntryTypeL() == CCalEntry::EEvent )
        {
            searchLightResult->SetContentFolderId( KSearchCFolderMemoUid );
        }
        
        if(  iEntry->EntryTypeL() == CCalEntry::ETodo )
        {
            searchLightResult->SetContentFolderId( KSearchCFolderToDoUid );
        }
        
        searchLightResult->SetServiceId( KNullUid );    
        isFirstKeyWordHit = 0;
        
        CCalDesC* caldetail = new(ELeave) CCalDesC();       
        CleanupStack::PushL ( caldetail );
                    
        TTime startDateTime = iEntry->StartTimeL().TimeLocalL();    
        
        const TPtrC16 ptrReadDateBuffer(( TText16*) iReadDateBuffer->Ptr(),
                                    ( iReadDateBuffer->Length()+1 )>>1 );
        HBufC16* dateBuffer=HBufC16::NewLC(ptrReadDateBuffer.Length());
        *dateBuffer=ptrReadDateBuffer;
        
        TBufC<KBigBufferLength> dateString;
        TPtr tempDateStringPtr( dateString.Des() );
        startDateTime.FormatL( tempDateStringPtr, *dateBuffer );
        CCalAlarm* pAlarm = iEntry->AlarmL();
        
        if( ( iEntry->EntryTypeL() == CCalEntry::EAppt ) || 
            ( iEntry->EntryTypeL() == CCalEntry::EReminder ) ||
            ( iEntry->EntryTypeL() == CCalEntry::EEvent && pAlarm )||
            ( iEntry->EntryTypeL() == CCalEntry::ETodo && pAlarm ) )
        {
            if( iEntry->EntryTypeL() == CCalEntry::EEvent ||
            	iEntry->EntryTypeL() == CCalEntry::ETodo )
            {
            	TTimeIntervalMinutes timeInterval = pAlarm->TimeOffset();
            	startDateTime = startDateTime - timeInterval;
            }
            
            const TPtrC16 ptrReadTimeBuffer(( TText16*) iReadTimeBuffer->Ptr(),
                                        ( iReadTimeBuffer->Length()+1 )>>1 );
            HBufC16* timeBuffer=HBufC16::NewLC( ptrReadTimeBuffer.Length() );
            *timeBuffer=ptrReadTimeBuffer;
                
            TBufC<KBufferLength> timeString;
            TPtr tempTimeStringPtr( timeString.Des() );
            startDateTime.FormatL( tempTimeStringPtr, *timeBuffer );        
            tempDateStringPtr.Append( KEmptySpace );
            tempDateStringPtr.Append( tempTimeStringPtr );
            
            CleanupStack::PopAndDestroy( timeBuffer );
        }
                
        caldetail->SetSnippetL( tempDateStringPtr );
        caldetail->SetSummaryL(  iEntry->SummaryL() );
        caldetail->SetUidL( *buf8 );
        iCalDesCArray.AppendL( caldetail );
        
        CleanupStack::PopAndDestroy( dateBuffer );
        CleanupStack::Pop( caldetail );
        CleanupStack::PopAndDestroy( buf8 );
        
        
        
	CSearchResult* searchResult = CSearchResult::NewL( searchLightResult);

	searchResult->SetSnippetL(KNullDesC8);
	searchResult->SetTitleL(KNullDesC);
	iHeavyResultsArray.Append( searchResult );
        
        iObserver->ResultFoundL( searchLightResult, iCurrentDocumentSearching, 
                                iTotalNumOfDocumentsSearched );
    }
}


 HBufC8* CCalendarSearcher::LaunchInfoL( const CSearchDocumentId& aDocumentID )
 {
 	
 	if((aDocumentID.PluginId() == iPluginId) &&
        (aDocumentID.RepositoryId() == KSearchCClassCalendarUid.iUid))
         {                  
          	HBufC8 * launchInfo = HBufC8::NewL(256);
          	TPtr8 docIdPtr = launchInfo->Des();
         	docIdPtr.AppendNum(aDocumentID.DocumentId());
         	return launchInfo;
         }
     
     return NULL;
     
 }
   
// -----------------------------------------------------------------------------
// CCalendarSearcher::InitializationL
// Initializes Search. Causes Search to start when contact to calendar server finished.
// -----------------------------------------------------------------------------
//
void CCalendarSearcher::InitializationL()
    {    
        delete iEntryView;
        iEntryView = NULL;

        delete iInstanceView;
        iInstanceView = NULL;

        delete iSession;
        iSession = NULL;
             
        iSession = CCalSession::NewL();
        
        TRAPD( error, iSession->OpenL( iSession->DefaultFileNameL() ));        
        if ( error != KErrNone )
        {
            ReportFinishedL( error );
            return;
        }
                
        iCalendarState = ECreatingCalendarEntryView;
        iEntryView = CCalEntryView::NewL( *iSession, *this );

        TRAPD( err, StartSearchL() );
        if ( err != KErrNone )
            {
                iNeedToCallReportFinishedL = ETrue;
            }
        
    }
 
// -----------------------------------------------------------------------------
// CCalendarSearcher::Completed
// Is called when classes related to Calendar connection are finished initializing.
// This is called after CCalInstanceView::NewL and CCalEntryView::NewL.
// When initialization is complete starts the search automatically.
// -----------------------------------------------------------------------------
//
void CCalendarSearcher::Completed(TInt aError)
    {   
		switch ( iCalendarState )
			{
			case ECreatingCalendarEntryView:
				{
				// CCalEntryView is constructed -> Active object must complete
				TRequestStatus* status = &iStatus;
				User::RequestComplete( status, aError );
				break;
				}

			case ECreatingCalendarInstanceView:
				{
				// CCalInstanceView is constructed -> Active object must complete
				TRequestStatus* status = &iStatus;
				User::RequestComplete( status, aError );
				break;
				}

			default:    // If we get here, it's a program error
				{
				User::Panic( KCalendarSearchPlugIn, ECalendarSearchPlugWrongState );
				}
			}
        
    }
 
// -----------------------------------------------------------------------------
// CCalendarSearcher::ReportFinishedL
// Tell observer search finished
// -----------------------------------------------------------------------------
//  
void CCalendarSearcher::ReportFinishedL( TInt aSearchResult, TInt aTotalNumOfItems)
    {
        iObserver->SearchCompletedL( aSearchResult, aTotalNumOfItems );
    }
 
// -----------------------------------------------------------------------------
// CCalendarSearcher::ConstructL
// Performs the second phase construction.
// -----------------------------------------------------------------------------
//
void CCalendarSearcher::ConstructL( const RArray<TUid>& aContentIdArray, 
                                        const CSearchCondition& aCondition,
                                        MSearchPluginObserver& aObserver )
    {         
        iObserver = &aObserver;                    
        for( TInt ctr= 0 ; ctr < aContentIdArray.Count() ; ctr++ )
        {   
            if( aContentIdArray[ctr] == KSearchCClassCalendarUid )
                {
                    iFilter = CalCommon::EIncludeAll;
                    break;
                }
                else
                {
                    if ( aContentIdArray[ctr] == KSearchCFolderAnniversaryUid )
                        iFilter = iFilter || ( CalCommon::EIncludeAnnivs 
                        | CalCommon::EIncludeReminder );
                    
                    if ( aContentIdArray[ctr] == KSearchCFolderMeetingsUid )
                        iFilter = iFilter || CalCommon::EIncludeAppts ;
                    
                    if ( aContentIdArray[ctr] == KSearchCFolderMemoUid )
                        iFilter = iFilter || CalCommon::EIncludeEvents;
                    
                    if ( aContentIdArray[ctr] == KSearchCFolderToDoUid )
                        iFilter = iFilter || ( CalCommon::EIncludeCompletedTodos 
                                | CalCommon::EIncludeIncompletedTodos );
                }
            }
            
        SetPriority( EPriorityStandard );        
        iTextSearcher = CSearchTextSearcher::NewL( *this );
        TRAPD(ParameterError, iTextSearcher->SetParametersL( aCondition ));
        User::LeaveIfError( iFsSession.Connect() );
        
        
        TFileName       resourceFileName;
        RResourceFile resourceFile;
        
        resourceFileName.Copy( TParsePtrC( RProcess().FileName() ).Drive() );  
        resourceFileName.Append( KDC_RESOURCE_FILES_DIR );
        resourceFileName.Append( KAvkonResoureFilePath );
        
        // Get language local file name of resource file.
        BaflUtils::NearestLanguageFile( iFsSession, resourceFileName );
        resourceFile.OpenL(iFsSession, resourceFileName );
        resourceFile.ConfirmSignatureL();
        
        
        iReadDateBuffer = resourceFile.AllocReadL( R_QTN_DATE_SHORT );
        iReadTimeBuffer = resourceFile.AllocReadL( R_QTN_TIME_USUAL );
        
        
        if( ParameterError == KErrNone)
        {
            iSetParamsDone = ETrue;
        }
        else
        {
            iErrorCode = ParameterError;
        }
        resourceFile.Close();
        
    }
              
// -----------------------------------------------------------------------------
// CCalendarSearcher::CCalendarSearcher
// Performs the first phase of two phase construction.
// -----------------------------------------------------------------------------
//
CCalendarSearcher::CCalendarSearcher( const TUid&  aPluginId ) : 
    CActive( CActive::EPriorityStandard ), iCurrentDocumentSearching( 0 ),iFilter( 0 ),
     iPluginId( aPluginId ), isFirstKeyWordHit( 0 ),
     iSetParamsDone( EFalse ), iNeedToCallReportFinishedL( EFalse ),iSearchNotCancelled( EFalse )
    {
        CActiveScheduler::Add( this );
    }
          
// -----------------------------------------------------------------------------
// CCalendarSearcher::IsSearching
// Does Cleanup for this class.
// -----------------------------------------------------------------------------
//    
void CCalendarSearcher::Cleanup()
    { 

	switch ( iCalendarState )
		{
		case ECreatingCalendarEntryView:
			{
			// CCalEntryView does not have a Cancel function, thus delete is the only option
			// CCalendarSearcher::Completed will get called with KErrCancel
			delete iEntryView;
			iEntryView = NULL;
			break;
			}

		case ECreatingCalendarInstanceView:
			{
			// CCalInstanceView does not have a Cancel function, thus delete is the only option			
			// CCalendarSearcher::Completed will get called with KErrCancel
			delete iInstanceView;
			iInstanceView = NULL;
			break;
			}
		
  		case ESearching:
	  		{
	  		// Outstanding request is cancelled and search will not continue.
	  		break;
	  		}
	  		
  		default:    // If we get here, it's a program error
	  		{
			User::Panic( KCalendarSearchPlugIn, ECalendarSearchPlugWrongState );
	  		}
		}
        
        //iInstanceArray.ResetAndDestroy();
         
    }
    
// -----------------------------------------------------------------------------
// CCalendarSearcher::DoActualSearchL
// Search calendar database
// -----------------------------------------------------------------------------
// 
TBool CCalendarSearcher::DoActualSearchL()
    {
        TBool       calendarItems( ETrue );
        TInt calEntriesInOneGoCounter(0);   
	while ( calendarItems && iSearchNotCancelled && (calEntriesInOneGoCounter < KSearchThisManyCalEntriesInOneGo))
    	{
	    	delete iEntry;
	    	iEntry = NULL;
	    	iEntry = iEntryView->FetchL( iCalLocalUidArray[0] );
    	 calEntriesInOneGoCounter++;
            isFirstKeyWordHit = 1;
            HBufC16* textToBeSearched = NULL;
            iTextSearcher->Cleanup();
            iCurrentDocumentSearching++;
            TPtrC summaryLine = iEntry->SummaryL();
            
            if( ( iEntry->EntryTypeL() == CCalEntry::EAppt ) ||
                ( iEntry->EntryTypeL() == CCalEntry::EReminder ) )
                {
                    TPtrC locationLine = iEntry->LocationL();
                    TPtrC descriptionLine = iEntry->DescriptionL();                                                            
                    textToBeSearched = HBufC16::NewLC( summaryLine.Length()
                                        + descriptionLine.Length()
                                        + locationLine.Length() + 2 );                             
                    textToBeSearched->Des().Append(summaryLine);
                    textToBeSearched->Des().Append( KEmptySpace );
                    textToBeSearched->Des().Append(descriptionLine);
                    textToBeSearched->Des().Append( KEmptySpace );
                    textToBeSearched->Des().Append(locationLine);
            
                    iTextSearcher->SearchL( *textToBeSearched );                      
                    CleanupStack::PopAndDestroy( textToBeSearched );
                }
            else
                 { 
                    iTextSearcher->SearchL( summaryLine );                              
                 } 
	        // Remove the element that was just searched
	        iCalLocalUidArray.Remove ( 0 );
	            
	 		// Check hometime and check if more items need searching
	        //now.HomeTime();
	        calendarItems = iCalLocalUidArray.Count() > 0;
        
    	}
        
        if( !calendarItems )
        {
            ReportFinishedL( KErrNone, iTotalNumOfDocumentsSearched );
            return EFalse;
        }
        return ETrue;
    }

TBool CCalendarSearcher::FetchAllCalendarIdsL() 
	{
    // Minimum time allowed in the API
    TCalTime minTime;
    minTime.SetTimeLocalL( TCalTime::MinTime() );   

	// Check if there are items available for the search
	TCalTime nextInstanceTime = iInstanceView->NextInstanceL( iFilter, minTime );
   	if ( nextInstanceTime.TimeUtcL() == Time::NullTTime() )
		{
			ReportFinishedL( KErrNone, iTotalNumOfDocumentsSearched );            
			return EFalse;			// Indicate that no search is needed
		}

    // Because the minimum time allowed is used in the query, we get all items available in the db
    iCalLocalUidArray.Reset();
    iCalLocalUidArray.Compress();
    iEntryView->GetIdsModifiedSinceDateL( minTime, iCalLocalUidArray );
    
    return ETrue;		// Indicated that search must start
    }

// -----------------------------------------------------------------------------
// CCalendarSearcher::RunL
// Continue search
// -----------------------------------------------------------------------------
// 
void CCalendarSearcher::RunL() 
    {         
	switch ( iCalendarState )
		{
		case ECreatingCalendarEntryView:
			{
            delete iInstanceView;
            iInstanceView = NULL;
			// Next create the CCalInstanceView
			iInstanceView = CCalInstanceView::NewL( *iSession, *this );
		    iCalendarState = ECreatingCalendarInstanceView;

			iStatus = KRequestPending;
			SetActive();
			break;
			}
			
		case ECreatingCalendarInstanceView:
			{
			// Fetch all calendar UIDs
			if ( FetchAllCalendarIdsL() )
				{
				// Next we start the actual search
				iCalendarState = ESearching;	
		        TRequestStatus* status = &iStatus;
		        User::RequestComplete( status, KErrNone );
		    	SetActive();
				}
			break;
			}
		
  		case ESearching:
	  		{
			if ( DoActualSearchL() )
			    {
				// We keep searching until nothing to search anymore
				iCalendarState = ESearching;
				
		        TRequestStatus* status = &iStatus;
		        User::RequestComplete( status, KErrNone );
		    	SetActive();		
			    }
	  		break;
	  		}
	  		
		default:    // If we get here, it's a program error
			{
			User::Panic( KCalendarSearchPlugIn, ECalendarSearchPlugWrongState );
			}
		}
        
    }
    
// -----------------------------------------------------------------------------
// CCalendarSearcher::DoCancel
// Implements cancellation of an outstanding request.
// This function is called as part of the active object's Cancel().
// -----------------------------------------------------------------------------
//
void CCalendarSearcher::DoCancel()
    {
     Cleanup();
    }

// -----------------------------------------------------------------------------
// CCalendarSearcher::RunError
// Called by system when RunL leave
// -----------------------------------------------------------------------------
// 
TInt CCalendarSearcher::RunError( TInt aError )
    {
        return aError;
    }
 

