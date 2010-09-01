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
* Description:     CSrchUiGeneral class is the main UI class which creates UI Manager and also acts as an observer for the retrieved results 
*
*/






#include <eikenv.h>
#include <aknViewAppUi.h>
#include <aknview.h>
#include <searchlightresult.h>
#include <searchresult.h>
#include <aknnotewrappers.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <utf.h>
#include <srchui.rsg>
#include <FileManager.rsg>
#include <bautils.h>

#include "srchuigeneral.h"
#include "srchuiserviceview.h"
#include "srchuimainview.h"
#include "srchuiappuids.h"
#include "srchuiresfolder.h"
#include "srchuiresfolderview.h"
#include "srchuidefines.h"
		
// ----------------------------------------------------------------------------
// CSrchUiGeneral::NewL
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CSrchUiGeneral* CSrchUiGeneral::NewL( CAknViewAppUi* aAppUi )
    {
    CSrchUiGeneral* self = CSrchUiGeneral::NewLC( aAppUi);
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CSrchUiGeneral::NewL
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CSrchUiGeneral* CSrchUiGeneral::NewL( CAknViewAppUi* aAppUi, 
		RDesReadStream& aReadStream )
    {
    CSrchUiGeneral* self = CSrchUiGeneral::NewLC( aAppUi, aReadStream );
    CleanupStack::Pop( self );
    return self;
    }
    

// ----------------------------------------------------------------------------
// CSrchUiGeneral::NewLC
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
CSrchUiGeneral* CSrchUiGeneral::NewLC( CAknViewAppUi* aAppUi )
    {
    CSrchUiGeneral* self = new ( ELeave ) CSrchUiGeneral( );
    CleanupStack::PushL( self );
    self->ConstructL( aAppUi );
    return self;
    }
    
// ----------------------------------------------------------------------------
// CSrchUiGeneral::NewLC
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
CSrchUiGeneral* CSrchUiGeneral::NewLC( CAknViewAppUi* aAppUi, 
		RDesReadStream& aReadStream )
    {
    CSrchUiGeneral* self = new ( ELeave ) CSrchUiGeneral();
    CleanupStack::PushL( self );
    self->ConstructL( aAppUi, aReadStream );
    return self;
    }

// ----------------------------------------------------------------------------
// CSrchUiGeneral::~CSrchUiGeneral
// Destructor
// ----------------------------------------------------------------------------
//
CSrchUiGeneral::~CSrchUiGeneral() // destruct - virtual, so no export
    {
    iViewIds.Close();
    iResultViewers.Reset();
    delete( iQueryString );
    delete( iSrchUiResRootFolder );
   	iResultViewers.Reset();
    REComSession::FinalClose();

    //Remove the resource files from the stack...
    TInt count = iResFileOffset.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CEikonEnv::Static()->DeleteResourceFile( iResFileOffset[i] );			
        }
    iResFileOffset.Reset();
    iSelectedContentTypes.Close();
    
    for ( TInt i = 0; i<iServiceInfoArray.Count(); i++ )
        {
        TServiceInfo* service = iServiceInfoArray[i];
        delete( service->iPrimaryCaption );
        delete( service->iSecondaryCaption );
        }
    iServiceInfoArray.ResetAndDestroy();
    //RMD
    iSupportedContentClasses.ResetAndDestroy();
    
    //RMD
    if(iSearchString)
        {
        delete(iSearchString);
        iSearchString = NULL;
        }
    delete iManager;
    }
    
void CSrchUiGeneral::SearchPartiallyCompletedL( TUid aPluginId )
{
	
}
// ----------------------------------------------------------------------------
// CSrchUiGeneral::ConstructSearchMainViewL
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::ConstructSearchMainViewL()
    {
    LaunchViewL( KSearchViewUid );	
    }


// ----------------------------------------------------------------------------
// CSrchUiGeneral::ConstructServiceSpecificMainViewL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::ConstructServiceSpecificMainViewL( TUid /*aServiceUid*/ )
    {
	 LaunchViewL( KSearchServiceViewUid );
    }


// ----------------------------------------------------------------------------
// CGenralSrchUi::ConstructResultFolderViewL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::ConstructResultFolderViewL( TUid /*aServiceUid*/ )
    {
    // Give a name to the root folder
    //First form the result folders                                   
    iSelectedContentCount = 0;
    //iSelectedContentTypes ( iManager->SelectedContentTypes() )
    
    RArray<TBool>& selectedContentTypes ( iManager->SelectedContentTypes() );
    
    for ( TInt i = 0; i < selectedContentTypes.Count(); i++ ) 
    	{
    	iSelectedContentTypes[i] = selectedContentTypes[i];
    	}
    
    FormResultFoldersL( iSelectedContentTypes );

    

    for ( TInt i = 0; i < selectedContentTypes.Count(); i++ ) 
	    	{
	    	TBool boolean = selectedContentTypes[i];
	    	if ( boolean ) 
	        	{
	    			iSelectedContentCount++;	
	        	}
	    	}

  	if ( 1 < iSelectedContentCount ) 
  			{
				RPointerArray<CSrchUiResFolder> folders;
	    	if ( !iResultFolderView )
	      		{
	        	iResultFolderView = CSrchUiResFolderView::NewL( this, folders );
	        	( ( CAknViewAppUi* )iSrchEikonEnv->EikAppUi() )->AddViewL( 
	        		iResultFolderView );	
	        	}
				LaunchViewL( KSearchResultFolderViewUid );

				folders.Reset();
  			}
    }

// ----------------------------------------------------------------------------
// CGenralSrchUi::GetSupportedContentClassesL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::GetSupportedContentClassesL( const TUid& aServiceId )
    {
    //Establish connection with the client
    //RMD
    iSupportedContentClasses.ResetAndDestroy();
    iManager->GetSupportedContentClassesL( iSupportedContentClasses, 
    			aServiceId );
    for ( TInt i = 0; i < iSupportedContentClasses.Count(); i++ ) 
    	{
    	iSelectedContentTypes.AppendL( EFalse );
    	}
    }


// ----------------------------------------------------------------------------
// CSrchUiManager::AllContentClassesSelected
// Getter for iAllContentClassesSelected.
// ----------------------------------------------------------------------------
//
TBool CSrchUiGeneral::AllContentClassesSelected() const
    {
    return iManager->AllContentClassesSelected();
    }


// ----------------------------------------------------------------------------
// CSrchUiGeneral::SetAllContentClassesSelected
// Setter for iAllContentClassesSelected
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::SetAllContentClassesSelected( TBool aSelected )
    {
    iManager->SetAllContentClassesSelected( aSelected );
    }

// ----------------------------------------------------------------------------
// CSrchUiGeneral::GetContentTypes
// Returns the content type
// ----------------------------------------------------------------------------
//
RPointerArray<CSearchContent>& CSrchUiGeneral:: GetContentTypes()
    {
    return( iSupportedContentClasses );
    }                
  	
// ----------------------------------------------------------------------------
// CSrchUiGeneral::SelectedContentTypes
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
RArray<TBool>& CSrchUiGeneral::SelectedContentTypes()
    {
    return iManager->SelectedContentTypes();
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolderView::AiwFlag
// Checks whether the search application is launched from AIW Consumer
// -----------------------------------------------------------------------------
//
TBool CSrchUiGeneral::AiwFlag()
		{
		return iAiwFlag;
		}

// ----------------------------------------------------------------------------
// CSrchUiManager::HandleSelectionL
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::HandleSelectionL( TDesC8& aQueryString, TUid aServiceUid )
    {
		TUid uidtest;
    uidtest.iUid = 10;
    iSearchOn = ETrue;
    {
    	//Show result folder view only if more than one content class is selected
    ConstructResultFolderViewL( uidtest );
    }
    iNumOfResultsFound = 0;
    if(iQueryString)
    {
        delete(iQueryString);
        iQueryString = NULL;
    }
    iQueryString = aQueryString.AllocL();
    
    for ( TInt i = 0; i<iSrchUiResRootFolder->GetAllSubFoldersCount(); i++ )
        {
        MSrchUiFolder* subfolder = iSrchUiResRootFolder->GetSubFolder( i );
        
        RPointerArray<CSearchLightResult>& lightresults = subfolder->Results();
        
        lightresults.Reset();
        }
    iLightResultCount = 0;
	iManager->HandleSelectionL( *iQueryString, aServiceUid, 
					iSelectedContentTypes, this );
		
	if ( iSelectedContentCount == 1)
		{
		LoadResultViewerL( 0 );
  	    }
  	iIsNewSearch = EFalse;
  }    

// ----------------------------------------------------------------------------
// CSrchUiGeneral::SrchUiResRoot
//
//  Returns root of the result folder
// ----------------------------------------------------------------------------
//
MSrchUiFolder* CSrchUiGeneral::SrchUiResRoot() 
	{
	return iSrchUiResRootFolder;
	}	
	
// ----------------------------------------------------------------------------
// CSrchUiGeneral::LoadResultViewerL
// Getter for iSrchUiResRootFolder.
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::LoadResultViewerL( TInt aSelectedFolderIdx )
	{
    iSelectedFolderIdx = aSelectedFolderIdx;
    //iManager->GetResultL( this );
    GetResultsFromManagerL();
    iResRetriveComplete = EFalse;
    
    LaunchResultViewL( iSelectedFolderIdx, !iSearchOn );
    iIsResultViewerLaunched = ETrue;
	}
    
// ----------------------------------------------------------------------------
// CSrchUiGeneral::ActivatePreviousViewL
// Launches the previous view
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::ActivatePreviousViewL()
    {
    LaunchPreviousViewL();
    iIsResultViewerLaunched = EFalse;
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolderView::GetQueryString
// Gets the query string
// -----------------------------------------------------------------------------
//
TDesC8& CSrchUiGeneral::GetQueryString()
		{
    return ( *iQueryString );
 		}

// -----------------------------------------------------------------------------
// CSrchUiResFolderView::IsSearchOngoing
// return : ETrue if search is ongoing else EFalse
// -----------------------------------------------------------------------------
//
TBool CSrchUiGeneral::IsSearchOngoing()
		{
    return( iSearchOn );
		}
		
// ----------------------------------------------------------------------------
// CSrchUiGeneral::GoToMainViewL
// Takes to the main view
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::GoToMainViewL()
	{
	if ( iQueryString )
		{
	    delete ( iQueryString );
	    iQueryString = NULL;
		}
    iQueryString = KNullDesC8().AllocL();
   	iSelectedContentTypes.Reset();
    
    for ( TInt i=0; i<iSupportedContentClasses.Count(); i++ )
        {
        iSelectedContentTypes.Append( EFalse );
        }
	SetAllContentClassesSelected(EFalse);
    for ( TInt i = iViewIds.Count() - 1; i>=0;i-- )
    	{
        iViewIds.Remove(i);
    	}
    
    TUid ids;
    ids.iUid = 5;
    iViewIds.Append( ids );
    
    TVwsViewId viewid( TUid::Uid( 0x10282411 ),ids );
	CEikonEnv::Static()->AppUi()->ActivateViewL( viewid );     
	}				 						 
	
// ----------------------------------------------------------------------------
// CSrchUiGeneral::SetSearchStringL
// Sets the query string to be searched
// ----------------------------------------------------------------------------
//
EXPORT_C void CSrchUiGeneral::SetSearchStringL( const TDesC& aString )
    {
    if ( iSearchString )
        {
        delete iSearchString;
        iSearchString = NULL;
        iSearchString = HBufC8::NewL( aString.Length() );
        }
		TPtr8 ptr1( iSearchString->Des() );
    CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr1, aString );
    }		   


// ----------------------------------------------------------------------------
// CSrchUiGeneral::SearchString
// gets the query string 
// ----------------------------------------------------------------------------
//
EXPORT_C TDesC8& CSrchUiGeneral::SearchStringL()
    {
    if ( iIsNewSearch )
    	{
        HBufC* string = KNullDesC().Alloc();
        CleanupStack::PushL(string);
        SetSearchStringL( *string );
        CleanupStack::PopAndDestroy(string);
    	}
    return *iSearchString;
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolderView::SetLastLaunchViewId
//
// Sets the last visited view id
// -----------------------------------------------------------------------------
//
void CSrchUiGeneral::SetLastLaunchViewId( const TUid aViewId )
    {
    iLastLaunchViewId = aViewId;
    }
    
// ----------------------------------------------------------------------------
// CSrchUiGeneral::UpdateSearchProgressL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::UpdateSearchProgressL( RPointerArray<CSearchLightResult>& 
		aResultArray, TInt /*aSearchFinished*/, TInt /*aTotalProgressValue*/ )
    {
		//	iManager->UpdateContentClassesL( iSupportedContentClasses );
    iNumOfResultsFound +=  aResultArray.Count();
    AddResultsToFolderL( aResultArray );
    if ( 1 < iSelectedContentCount ) 
        {
        iResultFolderView->HandleItemAdditionL();	
        }
    if(iIsResultViewerLaunched)
    {
		
        if(iResRetriveComplete)
        {
        iResRetriveComplete = EFalse;
            
            iGetResAgain = EFalse;
        }
        else
        {
            iGetResAgain = ETrue;
        }
        
    }
    }
    
// ----------------------------------------------------------------------------
// CSrchUiGeneral::SearchCompletedL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::SearchCompletedL( TInt /*aSrchUiRes*/, 
		TInt aActuallyFinished )
    {
    
       
    iNumOfResultsFound = iLightResultCount;
       	
    if ( iManager->MaxResults() <= iNumOfResultsFound && !aActuallyFinished ) 
       	{
       	//Show a note here
        HBufC* infoNote = CCoeEnv::Static()->AllocReadResourceAsDes16L(
                          R_QTN_SEARCH_INFO_MAX );
	      CAknInformationNote* note = new ( ELeave ) CAknInformationNote ();
	      note->ExecuteLD( *infoNote );
       	}

    if(!aActuallyFinished )
    	{
    	CancelSearch();
    	iSelectedContentCount =0;
    	return ;
    	}
    
    iSearchOn = EFalse;
    
    if ( 1 < iSelectedContentCount ) 
        {
        iResultFolderView->SearchFinishedL( iNumOfResultsFound );
        iSelectedContentCount = 0;
        }
    
    if(iIsResultViewerLaunched) 
        {
       	//iManager->GetResultL(this);
       	GetResultsFromManagerL();
       	iIsResultViewerLaunched = EFalse;
       	for ( TInt i = 0; i < iResultViewers.Count(); i++ ) 
    		{
    		CSrchUiResFolder* openFolder = STATIC_CAST( CSrchUiResFolder* ,
                             iSrchUiResRootFolder->GetSubFolder( 
                             	iSelectedFolderIdx ) );
//   		iResultViewers[i]->UpdateViewL( openFolder, ETrue, EFalse );
    		}
        }
    }
    
// ----------------------------------------------------------------------------
// CSrchUiGeneral::ResultsRetrieveCompleteL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::ResultsRetrieveCompleteL( 
		RPointerArray<CSearchResult>& aResultArray )
		{
    AddHeavyResultsToFolderL( aResultArray );
    iResRetriveComplete = ETrue;
    UpdateResultViewL(!iSearchOn, iResRetriveComplete);
    
   

    if(iGetResAgain)
    {
        iResRetriveComplete = EFalse;
        iGetResAgain = EFalse;
    }

}

// -----------------------------------------------------------------------------
// CSrchUiGeneral::GetLaunchInfoL
// Get the file launch information from Search Server
// -----------------------------------------------------------------------------
//

HBufC8* CSrchUiGeneral::GetLaunchInfoL( const CSearchDocumentId& aDocumentID )
	{
	return iManager->GetLaunchInfoL( aDocumentID );
	}
// -------------------------------------------------------------------------------------------------
// CSrchUiGeneral::LaunchApplicationL
// Launches application for the given document id
// -------------------------------------------------------------------------------------------------
//
void CSrchUiGeneral::LaunchApplicationL( const CSearchDocumentId& aResultId )
    {
    TRAPD( err, iManager->LaunchApplicationL( aResultId ) );
    if ( KErrNotFound == err ) 
    	{
    	//Show a note here
        HBufC* infoNote = CCoeEnv::Static()->AllocReadResourceAsDes16L(
                                  R_QTN_SEARCH_ERROR_MISSING );
	    CAknInformationNote* note = new ( ELeave ) CAknInformationNote ();
	    note->ExecuteLD( *infoNote );
    	}
    else if	( KErrNotSupported == err ) 
    	{
    	//Show a note here
        _LIT(KFileMgrFilename,"filemanager.rsc");
	    RResourceFile resourceFile;
    	TFileName driveResFilename = TParsePtrC( RProcess().FileName() ).Drive();
        driveResFilename.Append(KDC_APP_RESOURCE_DIR);
    	driveResFilename.Append( KFileMgrFilename);
    	BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), driveResFilename  );
    	resourceFile.OpenL(CCoeEnv::Static()->FsSession(), driveResFilename );
    	resourceFile.ConfirmSignatureL(0);
    	
    	HBufC8* readBuffer=resourceFile.AllocReadLC( R_QTN_FMGR_ERROR_UNSUPPORT );
    	const TPtrC16 ptrReadBuffer(( TText16*) readBuffer->Ptr(),
    								( readBuffer->Length()+1 )>>1 );
    								
    	CAknErrorNote* note = new ( ELeave ) CAknErrorNote (ETrue);
	    note->ExecuteLD( ptrReadBuffer.AllocLC()->Des() );
	    CleanupStack::PopAndDestroy(2);
	    resourceFile.Close();


	    
	    

    	}
    }
// -------------------------------------------------------------------------------------------------
// CSrchUiGeneral::LaunchPreviousViewL
// Launches the previous view
// -------------------------------------------------------------------------------------------------
//
void CSrchUiGeneral::LaunchPreviousViewL()
    {
    TInt count = iViewIds.Count();
    if ( count == 1 )
        {
        iAvkonAppUi->ProcessCommandL( EAknCmdExit );
        }
    else
        {
        TUid prevviewid = iViewIds[count-2];
 
        iViewIds.Remove( count - 1 );
        TVwsViewId viewid( TUid::Uid( 0x10282411 ),prevviewid );
	    CEikonEnv::Static()->AppUi()->ActivateViewL( viewid );
    	if ( 1 < iSelectedContentCount ) 
        	{
	        if ( iResultFolderView ) 
	        	{
	        	iResultFolderView->SearchFinishedL( iNumOfResultsFound );
	        	iSelectedContentCount = 0;	
	        	}        
        	}
        }
        
    }

// -----------------------------------------------------------------------------
// CSrchUiResFolderView::StartNewSearchL
// Starts the new search operation
// -----------------------------------------------------------------------------
//
void CSrchUiGeneral::StartNewSearchL()
    {
    HBufC* string = KNullDesC().Alloc();
    CleanupStack::PushL(string);
    SetSearchStringL( *string );
    CleanupStack::PopAndDestroy(string);
    
	if(iQueryString)
	{
	    delete(iQueryString);
	    iQueryString = NULL;
	    
	}
    iQueryString = KNullDesC8().AllocL();
    iSelectedContentTypes.Reset();
    
    TInt i = 0;
    for ( i=0; i<iSupportedContentClasses.Count(); i++ )
        {
        iSelectedContentTypes.Append( EFalse );
        }
	SetAllContentClassesSelected( EFalse );
	iManager->SetSelectedContentTypesL( iSelectedContentTypes );	
    i = iViewIds.Count() - 1;
    for ( ; i>0;i-- )
	    {
		if ( iViewIds[i].iUid != iLastLaunchViewId.iUid )
		    {
		    iViewIds.Remove(i);
		    }
		else
		    {
		    break;
		    }
	    }
    
    TVwsViewId viewid( TUid::Uid( 0x10282411 ),iViewIds[i] );
	CEikonEnv::Static()->AppUi()->ActivateViewL( viewid );
	iIsNewSearch = ETrue;	
	iSearchOn = ETrue;
	}

// -------------------------------------------------------------------------------------------------
// CSrchUiGeneral::CancelSearch
// Cancels the search operation
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiGeneral::CancelSearch()
	{
	//TInt numberofResults = 0;
	if ( iSearchOn ) 
		{
		iManager->CancelSearch();
		iSearchOn = EFalse;
   		if ( iIsResultViewerLaunched ) 
   			{
   			TRAPD(LeaveCode,GetResultsFromManagerL());
   			if(LeaveCode == KErrNone)
   			{   			 			
   			iIsResultViewerLaunched = EFalse;
   			for ( TInt i = 0; i<iResultViewers.Count();i++ )
        		{
        		CSrchUiResultViewerPlugin* temp = iResultViewers[i];
            			TRAP_IGNORE(temp->UpdateViewL( iSrchUiResRootFolder, ETrue, ETrue ));
    	    		}	
   			}   			
   			}   	
		}
    return(0);
	}

// ----------------------------------------------------------------------------
// CSrchUiGeneral::LaunchUiSettingsViewL
//
//  Launches the search settings view.
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::LaunchUiSettingsViewL()
    {
        TUid gsAppImpUid;
        //For parent use 0x10282E08
    	gsAppImpUid.iUid = 0x10282E06; // 0x10282E06-> Child view//0x10282E04->Parentview

		// 0x10207239-PARENT OF PARENT PLUGIN & 0x100058EC-> gsapp.exe ; 0x10282411-:Searchapp.exe
        TVwsViewId viewid( TUid::Uid( 0x100058EC ),gsAppImpUid ); // 0x1020723B -> Parentof GSApp plugin 
        
		CEikonEnv::Static()->AppUi()->ActivateViewL( viewid ); 
		
    }
    
// ----------------------------------------------------------------------------
// CSrchUiGeneral::IsSearching
//
//  Returns True if search is in progress
// ----------------------------------------------------------------------------
//    
TBool CSrchUiGeneral::IsSearching() 
	{
	return iSearchOn;
	}

// ----------------------------------------------------------------------------
// CSrchUiGeneral::CSrchUiGeneral
// constructor support
// don't export these, because used only by functions in this DLL, eg our NewLC()
// ----------------------------------------------------------------------------
//
CSrchUiGeneral::CSrchUiGeneral( ) :iAllContentClassesSelected( ETrue ),
									iAiwFlag( FALSE ) 
	  {
	  iSrchEikonEnv = CEikonEnv::Static();
	  }
	          

// ----------------------------------------------------------------------------
// CSrchUiGeneral::ConstructL
// constructor
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::ConstructL( CAknViewAppUi* aAppUi ) 
    {
    iAppUi = aAppUi;
		iManager = CSrchUiManager::NewL();
	    iIsResultViewerLaunched = EFalse;
	    iGetResAgain = EFalse;
	    iResRetriveComplete = ETrue;
		TParse* fp = new( ELeave ) TParse ();
		fp->Set ( KSearchResourceFile, &KDC_APP_RESOURCE_DIR, NULL );
		TBuf<254> resourceFileName;
		resourceFileName.Copy( fp ->FullName() );
		delete fp;
		fp = NULL;
		
    iQueryString = KNullDesC8().AllocL();
    iSrchUiResRootFolder = NULL;
    
    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
    TFileName fileName( driveLetter );
    fileName.Append( resourceFileName );
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), fileName  );
    TInt offset  = iSrchEikonEnv->AddResourceFileL( fileName ); 
		iResFileOffset.Append( offset );
	
		if ( !iSearchString )
				{
				iSearchString = KNullDesC8().Alloc();
				}
	
		iSearchView = CSrchUiSearchView::NewL(this);
		iAppUi->AddViewL( iSearchView );	
	
		iManager->LoadUiServicesL( iServiceInfoArray );
		iSearchMainView = CSrchUiMainView::NewL( iServiceInfoArray,this );
		iAppUi->AddViewL( iSearchMainView );

		TInt searchType = 0;
	
		if ( iAiwFlag )
				{
				searchType = iLaunchType ? 2: 1;
				}
	
		switch ( searchType ) 
				{
				case 0:
						{
	    			//iManager->LoadUiServicesL(iServiceInfoArray);
	    			if ( iServiceInfoArray.Count() > 1 )
	        			{
	        			//iSearchMainView = CSrchUiMainView::NewL(iServiceInfoArray,this);
		      			//iAppUi->AddViewL(iSearchMainView);
		      			iSrchEikonEnv->AppUi()->SetDefaultViewL( *iSearchMainView );    
	        			ConstructSearchMainViewL();     
	        			}
	    			else
	        			{
	        			TUid uidtest;
	        			uidtest.iUid = 10;
	        			GetSupportedContentClassesL( uidtest );
	        			ConstructServiceSpecificMainViewL( uidtest );
	        			}
	   		
	    			iSearchOn = EFalse;
						}
						break;
		
				case 1: 
						{
						TUid uidtest;
	        	uidtest.iUid = 10;
	        	GetSupportedContentClassesL( uidtest );
	        	TUid testUidMsg;
	        	testUidMsg.iUid = iContextUids[0].iUid;
	        	FindAndReplaceTheClassWithFoldersL( testUidMsg, 1 );
	        	//iManager->UpdateContentClassesL( iSupportedContentClasses );
						ConstructServiceSpecificMainViewL( uidtest );
						break;
						}
		
				case 2: 
						{
						TUid uidtest;
	        	uidtest.iUid = 10;
	        	GetSupportedContentClassesL( uidtest );
	        	TUid testUidMsg;
	        	testUidMsg.iUid = iContextUids[0].iUid;
	        	FindAndReplaceTheClassWithFoldersL( testUidMsg, 2 );
						HandleSelectionL( *iSearchString, uidtest );
						}
						break;
				default:
						break;
				}
		iIsNewSearch = EFalse;
		LoadUiResultViewerL();
		}
    
// ----------------------------------------------------------------------------
// CSrchUiGeneral::ConstructL
// constructor
// second-phase constructor
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::ConstructL( CAknViewAppUi* aAppUi, RDesReadStream& aReadStream )
    {
    InternalizeL( aReadStream );
    ConstructL( aAppUi );
    }


// ----------------------------------------------------------------------------
// CSrchUiGeneral::FindAndReplaceTheClassWithFoldersL
//
//  Other Items are mentioned in the Header
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::FindAndReplaceTheClassWithFoldersL( const TUid aUid, 
		const TInt aFrom ) 
	  {
		RPointerArray<const CSearchContent> contentClass;
		CSearchContent const * content = NULL;
		TInt classFoundAtPos = 0;
		
		//Note the position to Replace the class with folders
		for ( TInt i = 0; i < iSupportedContentClasses.Count(); i++ )
		    {
				content = iSupportedContentClasses[i];
				if ( aUid.iUid == ( content->ContentId() ).iUid )
						{
						classFoundAtPos = i;
						break;
						}
				content = NULL;
				}
		//Count of selected items
		TInt checkedCount = 0;
		
		if ( content ) 
				{
				CSearchContent const * tmpContent;
				//Get the folders
				content->GetChildrenL( contentClass );
			
				//Array of selected items
				iSelectedContentTypes.Reset();
			
				if ( 2 == aFrom ) 
						{
						//If result view should be launched directly
						iSupportedContentClasses.Reset();
						for ( TInt i = 0; i < contentClass.Count(); i++ )
								{
								tmpContent = contentClass[i];
								iSelectedContentTypes.Append( ETrue );
								iSupportedContentClasses.Append( tmpContent );
								}
						iManager->SetSelectedContentTypesL( iSelectedContentTypes );
						iManager->UpdateContentClassesL( iSupportedContentClasses );
						return;
						}
			
				//Remove the class
				iSupportedContentClasses.Remove( classFoundAtPos );
			
				//Append the corresponding folders
				if ( 0 == contentClass.Count() ) 
						{
						contentClass.Append( content );
						}
		
				///To mark as selected items	
				checkedCount = contentClass.Count();
			
			  //Append rest of the content classes
				for ( TInt i = 0; i < iSupportedContentClasses.Count(); i++ ) 
						{
						tmpContent = iSupportedContentClasses[i];
						if ( aUid.iUid != ( tmpContent->ContentId() ).iUid )
								{
								contentClass.AppendL( tmpContent );
								}
						}
			
			
				iSupportedContentClasses.Reset();
				//Update the iSupportedContentClasses and iSelectedContentTypes
				//iSelectedContentTypes are only the content folders of the class
				//from where search is launched
				for ( TInt i = 0; i < contentClass.Count(); i++ ) 
						{
						if ( i < checkedCount ) 
								{
								iSelectedContentTypes.Append( ETrue );
								}
						else 
								{
								iSelectedContentTypes.Append( EFalse );
								}
						CSearchContent const * tmpContent = contentClass[i];
						iSupportedContentClasses.AppendL( tmpContent );
						}
				//Update the iSelectedContentTypes and iSupportedContentClasses of the 
				//manager
				iManager->SetSelectedContentTypesL( iSelectedContentTypes );
				iManager->UpdateContentClassesL( iSupportedContentClasses );
				}
		}
		    
// ----------------------------------------------------------------------------
// CSrchUiGeneral::InternalizeL
// Constructs object from a stream
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::InternalizeL( RDesReadStream& aStream )
    {
    iAiwFlag = ETrue;
    
    TInt32 consumerAppUid = aStream.ReadInt32L();
    TInt32 sizeOfStr = aStream.ReadInt32L();
    iSearchString = HBufC8::NewL( sizeOfStr );
    TPtr8 ptr1( iSearchString->Des() );
	
    aStream >> ptr1;
    sizeOfStr = aStream.ReadInt32L();
    HBufC8* str1 = HBufC8::NewL( sizeOfStr );
    TPtr8 ptr2( str1->Des() );
    aStream >> ptr2;
	
    iLaunchType = aStream.ReadInt32L();
    TInt32 count = aStream.ReadInt32L();
	
    TUid id;
    for ( TInt i = 0; i < count; i++ )
        {
        id.iUid = aStream.ReadInt32L();
        iContextUids.Append(id);
        }
    }
    
// -------------------------------------------------------------------------------------------------
// CSrchUiGeneral::LaunchViewL
// Launches the view of the specified view id
// -------------------------------------------------------------------------------------------------
//
void CSrchUiGeneral::LaunchViewL( TUid aNewViewId )
    {
    iViewIds.AppendL( aNewViewId );
    
    TVwsViewId viewid( TUid::Uid( 0x10282411 ),aNewViewId );
	CEikonEnv::Static()->AppUi()->ActivateViewL( viewid );
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiManager::LoadUiResultViewerL
//
// Loads the result viewer plugins
// -------------------------------------------------------------------------------------------------
//
void CSrchUiGeneral::LoadUiResultViewerL()
    {
    if ( iResultViewers.Count() == 0 )
    	{
        // Get list of all available ecom implementations information
    	RImplInfoPtrArray infoArray;
    	CSrchUiResultViewerPlugin::ListAllImplementationsL( infoArray );
     
        // Scan through the list of available implementations
    	for ( TInt i=0; i<infoArray.Count(); i++ )
    	    {
    	    CImplementationInformation& info( *infoArray[i] );
    	    
            // Construct plug-in and pass it to the search session
    	    TUid impUid ( info.ImplementationUid() );
    	    CSrchUiResultViewerPlugin* resview = NULL;
    	    TRAPD ( err, resview = InitializeUiResPlugInsL( impUid ) );
            if ( err == KErrNone && resview )
            	{
                CleanupStack::PushL( resview );
                iResultViewers.AppendL( resview );
       	        CleanupStack::Pop(resview);
            	}
            }
        infoArray.ResetAndDestroy();
        }
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiManager::UpdateResultViewL
// Updates the result viewer plugins of the selected content class
// -------------------------------------------------------------------------------------------------
//
void CSrchUiGeneral::UpdateResultViewL(TBool aSearchFinished, TBool aResultRetrieve )
{
    for ( TInt i = 0; i<iResultViewers.Count();i++ )
        {
        CSrchUiResultViewerPlugin* temp = iResultViewers[i];
           TRAPD(LeaveCode,temp->UpdateViewL( iSrchUiResRootFolder, 
           	aSearchFinished, aResultRetrieve));
           	if(LeaveCode != KErrNone)
           	{
           		User::Leave(LeaveCode);	
           	}
    	}
}

    
// -------------------------------------------------------------------------------------------------
// CSrchUiManager::LaunchResultViewL
// Launches the result viewer plugins of the selected content class
// -------------------------------------------------------------------------------------------------
//
void CSrchUiGeneral::LaunchResultViewL( TInt aSelectedFolderIdx, TBool aSearchFinished )
    {
    for ( TInt i = 0; i<iResultViewers.Count();i++ )
        {
        iResultViewers[i]->CreateManagerObserver( this );
        iViewIds.AppendL( TUid::Uid( 0x1020D906 ) ); 
        
        iResultViewers[i]->LaunchViewL( iSrchUiResRootFolder, 
        		aSelectedFolderIdx, *iQueryString , aSearchFinished );
    	}
    CSrchUiResFolder* openedFolder =  STATIC_CAST( CSrchUiResFolder* ,
                             iSrchUiResRootFolder->GetSubFolder( 
                             	aSelectedFolderIdx ) );
    TUint resCount = openedFolder->GetHeavyResultsCount();
    if ( resCount >= iManager->MaxResults() 
      && !aSearchFinished)
        {
        //Show a note here
        HBufC* infoNote = CCoeEnv::Static()->AllocReadResourceAsDes16L(
                                  R_QTN_SEARCH_INFO_MAX );
	    CAknInformationNote* note = new ( ELeave ) CAknInformationNote ();
	    note->ExecuteLD( *infoNote );	
        }
	}

// ----------------------------------------------------------------------------
// CSrchUiManager::InitializeUiResPlugInsL
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
CSrchUiResultViewerPlugin* CSrchUiGeneral::InitializeUiResPlugInsL( TUid aImpUid )
    {
    CSrchUiResultViewerPlugin* resview = CSrchUiResultViewerPlugin::NewL( 
    	aImpUid/*,this,NULL*/ );
    return resview;
    }

// ----------------------------------------------------------------------------
// CSrchUiManager::FormResultFoldersL
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::FormResultFoldersL(RArray <TBool> 
	/*aSelectedContentTypes */ )
    {
    //First construct the CSrchUiResFolders
    // Create root folder for all search content type folders
    if ( iSrchUiResRootFolder )
        {
        delete iSrchUiResRootFolder;   
        iSrchUiResRootFolder = NULL;  
        }
    HBufC *contentTypeCaption = HBufC::NewL( 25 );
    _LIT( KRootFolder, "Root Folder" );
    contentTypeCaption->Des().Copy( KRootFolder );
 
    iSrchUiResRootFolder = CSrchUiResFolder::NewL();
    iSrchUiResRootFolder->SetFolderNameL( *contentTypeCaption );
    delete contentTypeCaption;
    for ( TInt i= 0; i<iSupportedContentClasses.Count();i++ )
        {
        if ( iSelectedContentTypes[i] )
            {
        	CSearchContent* tmpContent = iSupportedContentClasses[i];
            CSrchUiResFolder* folder = ConstructResultFolderL(
								tmpContent ,iSrchUiResRootFolder );
            if ( folder )
                {
                CleanupStack::PushL( folder );
                iSrchUiResRootFolder->AddChildL( folder );
                CleanupStack::Pop( folder );    
                }    
            }
        
        }
	}

// ----------------------------------------------------------------------------
// CSrchUiManager::AddResultsToFolderL
// Adds the search light results into the folder structure.
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::AddResultsToFolderL( RPointerArray<CSearchLightResult> aResultArray )
    {

    // Scan through all results and put each result into the appropriate
    //  folder.  Array is scanned from bottom to top.
    for ( TInt i = aResultArray.Count() - 1; i>=0; i-- )
        {
        // Extract the content type id
        CSearchLightResult* srchuires = aResultArray[i];
        TUid contentTypeId( srchuires->ContentClassId() );
        TUid contentTypeFolderIdOfResult( srchuires->ContentFolderId() );
	         
        if ( IsClassSelected( contentTypeFolderIdOfResult, contentTypeId ) )
            {         
            iLightResultCount++; 
            
            // Find the (sub) folder of the extracted content type id
    	    CSrchUiResFolder* contentTypeClass ( FindClass( contentTypeId, 
    	         contentTypeFolderIdOfResult ) );

    	    if ( contentTypeClass ) 
    	        {	
	        	TUid contentTypeFolderIdOfClass = contentTypeClass->ContentFolderType() ;
	    	    if ( ( KSearchCClassMessages == contentTypeId.iUid || 
	    	    	KSearchCClassCalendar == contentTypeId.iUid  || 
	    	    	KSearchCClassContacts == contentTypeId.iUid ) && 
	    	    	KNullUidValue == contentTypeFolderIdOfClass.iUid )
	    			{
	    			contentTypeClass = FindFolder( contentTypeClass, 
	    					contentTypeFolderIdOfResult );
	    			}
    			}
    	
    		//contentTypeClass this might have been changed inside the if
    		//above scope
    		if ( contentTypeClass )
    	    	{
            	contentTypeClass->iSearchStatus = ESearchStatusOngoing;
        	
	        // Append the single result to the correct contentTypeClass
    	        RPointerArray<CSearchLightResult>& results = contentTypeClass->Results();
                   
        	    results.AppendL( srchuires );
    	    	}
    		}
        }
    }


// ----------------------------------------------------------------------------
// CSrchUiManager::AddHeavyResultsToFolderL
// Adds heavy result to root folder
// ----------------------------------------------------------------------------
//
void CSrchUiGeneral::AddHeavyResultsToFolderL( RPointerArray<CSearchResult> 
					aResultArray )
    {
    // Scan through all results and put each result into the appropriate
    //  folder.  Array is scanned from bottom to top.
    iSrchUiResRootFolder->ResetAllFolders();
    for ( TInt i=aResultArray.Count()-1; i>=0; i-- )
        {
        // Extract the content type id
        CSearchResult* srchuires = aResultArray[i];
        TUid contentTypeId = srchuires->Result().ContentClassId();
        TUid contentTypeFolderIdOfResult( srchuires->Result().ContentFolderId() );

    	CSrchUiResFolder* contentTypeClass ( FindClass( contentTypeId, 
    				contentTypeFolderIdOfResult ) );
    	 	
    	//To put in the folder
    	if ( contentTypeClass ) 
    	    {
	    	TUid contentTypeFolderIdOfClass = contentTypeClass->ContentFolderType() ;
	    	if ( ( KSearchCClassMessages == contentTypeId.iUid || 
	    		KSearchCClassCalendar == contentTypeId.iUid ||
	    		KSearchCClassContacts == contentTypeId.iUid ) && 
	    		KNullUidValue == contentTypeFolderIdOfClass.iUid ) 
	    		{
	    		contentTypeClass = FindFolder( contentTypeClass, contentTypeFolderIdOfResult );
	    		}
    		}
    	
    	if ( contentTypeClass )
    	    {
            contentTypeClass->iSearchStatus = ESearchStatusOngoing;
        	
            // Append the single result to the correct contentTypeClass
            RPointerArray<CSearchResult>& results = contentTypeClass->HeavyResults();
            
            // Add the single file to the array of files
            results.AppendL(srchuires);
            //CleanupStack::Pop( resultFile );    // resultFile
    	    }
    	}
    	
    }



// -------------------------------------------------------------------------------------------------
// CSrchUiManager::FindClass
// Returns the class which is having the specified content type id.
// -------------------------------------------------------------------------------------------------
//
CSrchUiResFolder* CSrchUiGeneral::FindClass( const TUid& aContentid, 
	const TUid& aContentFolderid )
	{

	// Only 1 level of nesting supported so far
	RPointerArray<CSrchUiResFolder> subFolders;
	iSrchUiResRootFolder->SubFolders( subFolders );
	
	for ( TInt i( 0 ); i < subFolders.Count(); i++ )
		{
		CSrchUiResFolder& subFolder( *subFolders[i] );
		
		if( KNullUidValue == subFolder.ContentFolderType().iUid ) 
			{
			if ( subFolder.ContentClassType() == aContentid )
			    {
			    subFolders.Reset();
				return &subFolder;
			    }
			}
		else 
			{
			if ( subFolder.ContentFolderType() == aContentFolderid )
			    {
			    subFolders.Reset();
				return &subFolder;
			    }
			}
		}
	subFolders.Reset();
    return NULL;
	}
	
 // -----------------------------------------------------------------------
// Checks if a content class is selected
// -----------------------------------------------------------------------
//
TBool CSrchUiGeneral::IsClassSelected( const TUid aFolderId, 
				const TUid aClassId )
	{
	CSearchContent* content = NULL;
	TUid classId;
	TUid folderId;
	for ( TInt i = 0; i < iSupportedContentClasses.Count(); i++ ) 
	    {
		content = iSupportedContentClasses[i];
		if ( content ) 
		    {
			classId.iUid = content->ContentId().iUid;
			folderId.iUid = content->SubContentId();
			if ( ( aClassId.iUid == classId.iUid ) && ( KNullUid.iUid 
					== folderId.iUid ) )
			    {
				return ETrue;
			    }
			
			if ( ( aClassId.iUid == classId.iUid ) && ( aFolderId.iUid == 
				folderId.iUid ) )
			    {
				return iSelectedContentTypes[i];
			    }
		    }
		
	    }
	return EFalse;
    }
// ----------------------------------------------------------------------------
// CSrchUiManager::ConstructResultFolderL
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
CSrchUiResFolder* CSrchUiGeneral::ConstructResultFolderL(
					const CSearchContent *content, CSrchUiResFolder* aparent ) 
	{
    RPointerArray<const CSearchContent> childcontent;
    CSrchUiResFolder* parentpointer = CSrchUiResFolder::NewL( aparent );
	CleanupStack::PushL( parentpointer );
	
	TInt captionSize = ( content->Caption() ).Size();
	HBufC* des = HBufC::NewL( captionSize );
		
	CleanupStack::PushL( des );
    TPtr ptr1( des->Des() );
    
    ptr1.Copy( content->Caption() );

    parentpointer->SetFolderNameL( *des );
    CleanupStack::PopAndDestroy( des );
    parentpointer->SetContentClassType( content->ContentId() );
    parentpointer->SetContentFolderType( TUid::Uid(content->SubContentId() ));
    parentpointer->SetIconInfo( content->IconInfo() );
    content->GetChildrenL( childcontent );
    CleanupStack::Pop( parentpointer );
    if ( childcontent.Count() == 0 )
    	{
        return parentpointer;
    	}
    else
    	{
        for ( TInt i = 0; i<childcontent.Count(); i++ )
        	{
            CSrchUiResFolder* child = ConstructResultFolderL(
            	 childcontent[i],parentpointer );
            CleanupStack::PushL( parentpointer );
            parentpointer->AddChildL( child );
            CleanupStack::Pop( parentpointer );
        	}
        
    	}
	childcontent.Reset();
    return ( parentpointer );
	}
	
// -----------------------------------------------------------------------
// CSrchUiManager::FindFolder
// Returns the folder which is having the specified content type id.
// -----------------------------------------------------------------------
//		
CSrchUiResFolder* CSrchUiGeneral::FindFolder( CSrchUiResFolder* aClass,
	  const TUid& aContentid )	
	{
		// Only 1 level of nesting supported so far
	RPointerArray<CSrchUiResFolder> subFolders;
	aClass->SubFolders( subFolders );
	
	for ( TInt i( 0 ); i < subFolders.Count(); i++ )
		{
		CSrchUiResFolder& subFolder( *subFolders[i] );
		
		if ( subFolder.ContentFolderType() == aContentid )
			{
			subFolders.Reset();
			return &subFolder;
			}
		}
	subFolders.Reset();
	return NULL;
	}
    


// -----------------------------------------------------------------------
// CSrchUiManager::GetResultsFromManager
// Gets heavy reults corresponding to the light results
// -----------------------------------------------------------------------
//
void CSrchUiGeneral::GetResultsFromManagerL() 
{
	RPointerArray<CSearchDocumentId> resultIDArray;
    
    for ( TInt i = 0; i<iSrchUiResRootFolder->GetAllSubFoldersCount(); i++ )
    	{
        MSrchUiFolder* subfolder = iSrchUiResRootFolder->GetSubFolder( i );
        
        RPointerArray<CSearchLightResult>& lightresults = subfolder->Results();
        
        for ( TInt i = 0; i<lightresults.Count(); i++ )
        	{
            resultIDArray.AppendL( &lightresults[i]->DocumentId() );
        	}
    	}
	iSrchUiResRootFolder->ResetAllFolders();
    iManager->GetResultL( resultIDArray, this );
	resultIDArray.Reset();
	resultIDArray.Close();
}

		  
// 
// DLL entry point
// 
GLDEF_C TInt E32Dll() 

		{
    return( KErrNone );
		}
