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
* Description:     This class is responsible for the display of the results in snippets for a particular content class selected by the user. 
*
*/






// System Includes
#include <eikmenup.h>
#include <srchuiresultview.rsg>
#include <StringLoader.h>
#include <FileManager.rsg>
#include <data_caging_path_literals.hrh>
#include <bautils.h>
#include <searchdocumentid.h>
#include <utf.h>
#include <apgcli.h>
#include <aknnotewrappers.h>
#include <bautils.h>
#include <filessearchresource.rsg>
#include <bautils.h>
#include <e32debug.h>
#include <DocumentHandler.h>
#include <AknServerApp.h>
#include <w32std.h>
#include <hlplch.h> // help launcher
#include <csxhelp/search.hlp.hrh>

// User Includes
#include "srchuiresfolder.h"
#include "srchuiresultview.h"
#include "srchuiresultcontainer.h"
#include "srchuiresultviewuids.h"
#include "srchuiresult.hrh"
#include "srchuiresultviewdefines.h"

#include<ccafactory.h>
#include<mccaparameter.h>
#include<mccaconnection.h>

#define KSearchFileUid 0x10282400
#define KSearchContact 0x102823FE   //Plug-in uid ,using in launch a contact with new API
// =========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSrchUiResultView::NewL
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
CSrchUiResultView* CSrchUiResultView::NewL()
	{
    CSrchUiResultView* self = CSrchUiResultView::NewLC();
    CleanupStack::Pop( self );
    return self;
	}

// ----------------------------------------------------------------------------
// CSrchUiResultView::NewLC
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
CSrchUiResultView* CSrchUiResultView::NewLC()
	{
    CSrchUiResultView* self = new ( ELeave ) CSrchUiResultView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// ----------------------------------------------------------------------------
// CSrchUiResultView::~CSearchContainer
// Destructor.
// ----------------------------------------------------------------------------
//
CSrchUiResultView::~CSrchUiResultView()
    {
	if( iContainer )
		{
		delete iContainer;
		iContainer = NULL;
		}

	if( iQueryString )
		{
		delete iQueryString;
		iQueryString = NULL;
		}

	for(TInt i = 0; i < iResFileOffset.Count(); i++)
	    {
	    iSrchEikonEnv->DeleteResourceFile(iResFileOffset[i]);			
	    }
	    
	if(iPeriodicTimer)
	{
	    iPeriodicTimer->Cancel();
	    delete iPeriodicTimer;
	    iPeriodicTimer = NULL;
	}
	
	iResFileOffset.Reset();
    if( iHandler )
    	{
    	delete iHandler;
    	iHandler = NULL;
    	}
    
    if( iConnection )
    	{
    	iConnection->Close();
    	}
    }

// ----------------------------------------------------------------------------
// CSrchUiResultView::ResultsAddedL
// Called from the application ui class when results were added.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::ResultsAddedL()
	{
	if ( iContainer )
	    {
        iContainer->ResultsAddedL();
	    }
	}

// ----------------------------------------------------------------------------
// CSrchUiResultView::ResultsClearedL
// Called from the application ui class when the previous results were deleted.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::ResultsClearedL()
    {
    SetContainerListBoxIndex( KErrNotFound );
	if ( iContainer )
	    {
        iContainer->ResultsClearedL();
	    }
    }

// ----------------------------------------------------------------------------
// CSrchUiResultView::SearchStatusChangedL
// Called from the application ui class after some search status change.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::SearchStatusChangedL()
    {
    
   	if ( iContainer )
	    {
        iContainer->SearchStatusChangedL();
	    }
	    
    }

// ----------------------------------------------------------------------------
// CSrchUiResultView::SearchFinishedL
// Called from the application ui class after the search has finished.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::SearchFinishedL()
    {
   	if ( iContainer )
	    {
        iContainer->SearchFinishedL();
	    }
    }

// ----------------------------------------------------------------------------
// CSrchUiResultView::SetContainerListBoxIndex
// Setter for iContainerListBoxIndex.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::SetContainerListBoxIndex( TInt aIndex )
    {
    iContainerListBoxIndex = aIndex;
    }

// ----------------------------------------------------------------------------
// CSrchUiResultView::ContainerListBoxIndex
// Getter for iContainerListBoxIndex.
// ----------------------------------------------------------------------------
//
TInt CSrchUiResultView::ContainerListBoxIndex()    
    {
    return iContainerListBoxIndex;
    }

// ----------------------------------------------------------------------------
// CSrchUiResultView::UpdateCommandButtons
// This function updates the Command buttons when called from container
// when tabbed
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::UpdateCommandButtonsL( TInt aResultCount ) 
	{
	if ( Cba() ) 
		{
		if ( iResHandler->IsSearching() )
			{

			Cba()->SetCommandSetL(R_SOFTKEYS_OPTIONS_MSKEMPTY_CANCEL);
				
			}
		else 
			{
			if ( !aResultCount ) 
				{
				Cba()->SetCommandSetL( R_SOFTKEYS_OPTIONS_MSKEMPTY_BACK );		
				}
			else 
				{
				Cba()->SetCommandSetL( R_SOFTKEYS_OPTIONS_OPEN_BACK );		
				}	
			}
		
		Cba()->DrawDeferred();	
		}
	
	
	}
	
// ----------------------------------------------------------------------------
// CSrchUiResultView::SupportedFormat
// Getter for supported format
// ----------------------------------------------------------------------------
//
const TDesC& CSrchUiResultView::SupportedFormat()
	{
    return KSrchUiResultViewerSupportsText;
	}

// ----------------------------------------------------------------------------
// CSrchUiResultView::HandleSelectionL
// called when one of the results is selected. 
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::HandleSelectionL(const CSearchDocumentId& aDocumentId )
	{
    if(iOpenedFolder->GetHeavyResultsCount()>0)
        {
        TInt selectedItemId = aDocumentId.PluginId().iUid ;
        if( selectedItemId == KSearchFileUid )
    	    {
    	    HBufC8* launchInfo = iResHandler->GetLaunchInfoL(aDocumentId);
    	    LaunchApplicationL( *launchInfo );
    	    if( launchInfo )
    		    {
    		    delete launchInfo;
    		    launchInfo = NULL;
    		    }
    	    return;
    	    }
		//new add ,using launch a contact with new API	
        if( selectedItemId == KSearchContact )
        	{
        	HBufC8* launchInfo = iResHandler->GetLaunchInfoL(aDocumentId);
        	LaunchContactApplicationL( *launchInfo );
			if( launchInfo )
				{
				delete launchInfo;
				launchInfo = NULL;
				}
        	return ;
        	}
			
        iResHandler->LaunchApplicationL( aDocumentId );   
        }
	}

// ----------------------------------------------------------------------------
// CSrchUiResultView::OpenFolder
// called when one of the results is selected. 
// ----------------------------------------------------------------------------
//
MSrchUiFolder* CSrchUiResultView::OpenFolder()
	{
    //  return STATIC_CAST( MSrchUiFolder*, iOpenedFolder );
    //to be implemented
    return NULL;
	}

// ----------------------------------------------------------------------------
// CSrchUiResultView::GetQueryString
// This function is called to get the query string
// ----------------------------------------------------------------------------
//
TDesC8& CSrchUiResultView::GetQueryString()
	{
	return(*iQueryString);
	}

// ----------------------------------------------------------------------------
// CSrchUiResultView::Id
// Returns the unique id of the view.
// ----------------------------------------------------------------------------
//
TUid CSrchUiResultView::Id() const
    {
    return KSearchResultViewUid;
    }

// ----------------------------------------------------------------------------
// CSrchUiResultView::HandleCommandL
// Handles the commands that are passed from softkey generated commands and
// pop up's.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case ESearchCmdOpen:
            if(iOpenedFolder->GetHeavyResultsCount()>0)
            {
                TInt selectedItemId = iContainer->HandleItemSelectionL().PluginId().iUid ;
                if( selectedItemId == KSearchFileUid )
            	    {
            	    HBufC8* launchInfo = iResHandler->GetLaunchInfoL(iContainer->HandleItemSelectionL());
            	    LaunchApplicationL( *launchInfo );
            	    if( launchInfo )
            		    {
            		    delete launchInfo;
            		    launchInfo = NULL;
            		    }
            	    break;
            	    }
					
                if( selectedItemId == KSearchContact )
					{
					HBufC8* launchInfo = iResHandler->GetLaunchInfoL( iContainer->HandleItemSelectionL() );
					LaunchContactApplicationL( *launchInfo );
					if( launchInfo )
						{
						delete launchInfo;
						launchInfo = NULL;
						}
					break ; 
					}
										
             iResHandler->LaunchApplicationL( iContainer->HandleItemSelectionL() );   
            }
             
            break;

        case ESearchCmdSearch:
        	{
        	 iResHandler->StartNewSearchL();	
        	}
        	break;
        	
         case ESearchCmdSettings:
        	{
        		
        	}
        	break;
         case ESearchCmdHelp:
        	{
        	TCoeHelpContext helpContext;
        	helpContext.iMajor = KSearchAppUid ;
        	helpContext.iContext = KSEARCH_HLP_SINGLE_RE; // add correct help context when available
        	CArrayFix<TCoeHelpContext>* hlpContextArray = 
        	                    new ( ELeave ) CArrayFixFlat<TCoeHelpContext> ( 1 );
        	CleanupStack::PushL( hlpContextArray );
        	hlpContextArray->AppendL( helpContext );
        	CleanupStack::Pop( hlpContextArray );
        	//Launch help - takes ownership of hlpContextArray
        	HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
        	                                                 hlpContextArray ); 
        		
        	}
        	break;
        case EAknSoftkeyCancel:
        	iResHandler->CancelSearch();
       	break;
        case EAknSoftkeyBack:
            iResHandler->LaunchPreviousViewL();
            break;
            //Do nothing(Ignoring)
        case EAknSoftkeyEmpty:
        	break;
        default:
			((CAknAppUi*)iSrchEikonEnv->EikAppUi())->HandleCommandL( aCommand );
			break;
        }
    }

// ----------------------------------------------------------------------------
// CSrchUiResultView::DoActivateL
// Called by the framework when the view is activated.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                  TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/ )
    {
    if ( !iContainer )
        {
        
        
        
        iContainer = new( ELeave )CSrchUiResultContainer( *this );
		iContainer->SetMopParent( this );
		iContainer->SetRoot(iOpenedFolder);
		iContainer->ConstructL( ClientRect(),iSelectedFolderIdx,
		                                               iSrchUiResRootFolder );
		                                               
		iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityHigh );
    	StartTimer();		                                            
		                                               
		UpdateViewL(iSrchUiResRootFolder, !iResHandler->IsSearching(), EFalse );
        ((CAknAppUi*)iSrchEikonEnv->EikAppUi())->AddToStackL( iContainer );
        iContainer->MakeVisible( ETrue );
        
        iContainer->UpdateTitlePaneL();
        
        
        }
        {
        	
	   if ( Cba() )
			{
			TInt resultsCount = 0;
			iOpenedFolder =  STATIC_CAST( CSrchUiResFolder* ,
	                      iSrchUiResRootFolder->GetSubFolder(iSelectedFolderIdx) );
			resultsCount = iOpenedFolder->GetHeavyResultsCount();
			if ( iResHandler->IsSearching() )
				{
				Cba()->SetCommandSetL(R_SOFTKEYS_OPTIONS_MSKEMPTY_CANCEL);
				}
			else
				{
				if ( !resultsCount ) 
					{
					Cba()->SetCommandSetL( R_SOFTKEYS_OPTIONS_MSKEMPTY_BACK );		
					}
				else 
					{
					Cba()->SetCommandSetL( R_SOFTKEYS_OPTIONS_OPEN_BACK );		
					}
				}
			Cba()->DrawNow();
			}
        }

    }

// ----------------------------------------------------------------------------
// CSrchUiResultView::DoDeactivate
// Called by the framework when the view is de-activated.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::DoDeactivate()
    {
    if ( iPeriodicTimer && iPeriodicTimer->IsActive() )
	    {
	    iPeriodicTimer->Cancel();
	    delete iPeriodicTimer; 
	    iPeriodicTimer = NULL; 
	    }
	    
    if ( iContainer )
		{
		((CAknAppUi*)iSrchEikonEnv->EikAppUi())->RemoveFromStack( iContainer );

        iContainer->MakeVisible( EFalse );
		delete iContainer;
		iContainer = NULL;
		}
    }

// ----------------------------------------------------------------------------
// CSrchUiResultView::DynInitMenuPaneL
// Called by the framework for dynamically initializing the menu pane.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::DynInitMenuPaneL(TInt aResourceId,
                                                    CEikMenuPane* aMenuPane)
    {
    if(!iContainer->IsElementValid())
        {
        if ( aResourceId == R_SEARCH_RESULT_VIEW_MENU_PANE )
            {
        	aMenuPane->DeleteMenuItem( ESearchCmdOpen );
            }
        }
    }

// ----------------------------------------------------------------------------
// CSrchUiResultView::LaunchViewL
// This function is called to launch the single result view
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::LaunchViewL(CSrchUiResFolder* aSrchUiResRootFolder,
                           TInt aSelectedFolderIdx, const TDesC8& aQueryString, TBool aSearchFinished)
	{
    iSrchUiResRootFolder = NULL;
	iSrchUiResRootFolder = aSrchUiResRootFolder;
    iOpenedFolder =  STATIC_CAST(CSrchUiResFolder* ,
                      iSrchUiResRootFolder->GetSubFolder(aSelectedFolderIdx));
    iSelectedFolderIdx = aSelectedFolderIdx;
	TVwsViewId viewid(TUid::Uid(KSearchAppUid.iUid),KSearchResultViewUid);
	iSrchEikonEnv->AppUi()->ActivateViewL(viewid);
    
    if(iQueryString)
    {
        delete(iQueryString);
        iQueryString = NULL;
    }
    iQueryString = aQueryString.AllocL();
	}

// ----------------------------------------------------------------------------
// CSrchUiResultView::IncrementFolderIndex
// This function is to increment the selected folders.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::IncrementFolderIndex() 
	{
	iSelectedFolderIdx++;
	}

// ----------------------------------------------------------------------------
// CSrchUiResultView::DecrementFolderIndex
// This function is to decrement the folder count.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::DecrementFolderIndex() 
	{
	iSelectedFolderIdx--;
	}	
void CSrchUiResultView::UpdateViewL(CSrchUiResFolder* aSrchUiResRootFolder, TBool aSearchFinished, TBool aResultRetrieve)
	{
	//If incase this function is called, before the view has been activated
	//Activate the view
	if( !iContainer )
		{
		return;
		}
           
            iSrchUiResRootFolder = aSrchUiResRootFolder;
            if ( Cba() )
		{
		TInt resultsCount = 0;
		iOpenedFolder =  STATIC_CAST(CSrchUiResFolder* ,
                      iSrchUiResRootFolder->GetSubFolder(iSelectedFolderIdx));
		resultsCount = iOpenedFolder->GetHeavyResultsCount();
		iSearchFinished = !iResHandler->IsSearching();
		if(!aSearchFinished)
		{
		Cba()->SetCommandSetL( R_SOFTKEYS_OPTIONS_MSKEMPTY_CANCEL );	
		}
		else
		{
			if ( !resultsCount ) 
				{
				Cba()->SetCommandSetL( R_SOFTKEYS_OPTIONS_MSKEMPTY_BACK );		
				}
			else 
				{
				Cba()->SetCommandSetL( R_SOFTKEYS_OPTIONS_OPEN_BACK );		
				}
		}
		Cba()->DrawDeferred();
		}
    
		if ( iContainer ) 
			{
			iContainer->UpdateViewL( iSrchUiResRootFolder, 
				aSearchFinished, aResultRetrieve );	
			}
    	
    
	    if ( aSearchFinished )
	   		
	    	{
	        // Stop the timer if it is active
		    if ( iPeriodicTimer && iPeriodicTimer->IsActive() )
		    	{
		    	iPeriodicTimer->Cancel();
		    	if ( iPeriodicTimer )
					{
				    delete iPeriodicTimer;
				    iPeriodicTimer = NULL;
					}
		    	}
	    	
	    	}
	    UpdateNaviPaneL();	
	}
	
// ----------------------------------------------------------------------------
// CSrchUiResultView::CreateManagerObserver
// This function is called to initialize the observer
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::CreateManagerObserver(
                                      MSrchUiResultHandler* aResHandler )
	{
    iResHandler = aResHandler;
	}

// ----------------------------------------------------------------------------
// CSrchUiResultView::CSrchUiResultView
// Performs the first phase of two phase construction.
// ----------------------------------------------------------------------------
//
CSrchUiResultView::CSrchUiResultView()
:   iSrchEikonEnv(CEikonEnv::Static())
	{
	}

// ----------------------------------------------------------------------------
// CSrchUiResultView::ConstructL
// Performs the second phase construction.
// ----------------------------------------------------------------------------
//
void CSrchUiResultView::ConstructL()
    {
    iHandler = CDocumentHandler::NewL();
    TParse* fp = new(ELeave) TParse ();
	fp->Set (KSearchResourceFile, &KDC_APP_RESOURCE_DIR, NULL);
	TBuf<254> resourceFileName;
	resourceFileName.Copy(fp ->FullName());
	delete fp;
	fp=NULL;
    
    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
    TFileName fileName( driveLetter );
    fileName.Append( resourceFileName );
    
    //TInt offset  = iSrchEikonEnv->AddResourceFileL(KSearchResourceFile); 
    BaflUtils::NearestLanguageFile( iSrchEikonEnv->FsSession(), fileName  );
    TInt offset  = iSrchEikonEnv->AddResourceFileL(fileName); 
    iResFileOffset.Append(offset);		
    BaseConstructL( R_SEARCH_RESULT_VIEW );
    
    ((CAknViewAppUi*)iSrchEikonEnv->EikAppUi())->AddViewL(this);


    iConnection = TCCAFactory::NewConnectionL();

    }
    
    
// -----------------------------------------------------------------------------
// CSrchUiResultView::PeriodL
// This function is called by the periodic timer.
// -----------------------------------------------------------------------------
//
TInt CSrchUiResultView::PeriodL( TAny* aPtr )
	{
  ( static_cast<CSrchUiResultView*>( aPtr ) )->SetNaviTextL();
    // Returning a value of TRUE indicates the callback should be done again
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSrchUiResFolderView::SetNaviTextL
// Updates the navigation with the text about the staus.
// -----------------------------------------------------------------------------
//
void CSrchUiResultView::SetNaviTextL()
{
	HBufC16 *pbuf;	
	pbuf = StringLoader::LoadL(R_QTN_SEARCH_INPROGRESS);
	
	HBufC16* tmpbuf = HBufC::NewL( pbuf->Size() + 3 );
	TPtr bufPtr = tmpbuf->Des();
	bufPtr.Copy( *pbuf );
	
	
	for ( TInt i = 0; i <= iCount_val%3; i++ ) 
		{
		_LIT ( KDot, "." );
		bufPtr.Append( KDot );
		}
	
    iCount_val++;
    
    if ( iContainer )
    	{
        iContainer->UpdateNaviPaneL( *tmpbuf );
    	}
    
    delete pbuf;
    delete tmpbuf;
}
    
// -------------------------------------------------------------------------------------------------
// CSrchUiResFolderView::StartTimer
// Starts the timer.
// -------------------------------------------------------------------------------------------------
//    
void CSrchUiResultView::StartTimer()
	{
	// If the timer is not already running, start it
	if ( !iPeriodicTimer->IsActive() )
		{
		iPeriodicTimer->Start( 10, 300000, 
                               TCallBack( CSrchUiResultView::PeriodL, this ) );
		}

	}
    
    
// -------------------------------------------------------------------------------------------------
// CSrchUiResultView::UpdateNaviPaneL
// Updates the navigation Pane.
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResultView::UpdateNaviPaneL()
    {
        if( iResHandler->IsSearching() )
        {
        	SetNaviTextL();
        }
        else
        {
           if ( iContainer ) 
           	   {
           	   iContainer->UpdateNaviResultPaneL();
           	   }
            
        }
       
        
    }

// -------------------------------------------------------------------------------------------------
// CSrchUiResultView::LaunchApplicationL
// Launch the app for a file
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResultView::LaunchApplicationL( const TDesC8& aLaunchInfo )
    {
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	User::LeaveIfError( fs.ShareProtected() );
	iHandler->SetExitObserver( this );
	TEntry fileEntry;
	HBufC *path16 = HBufC::NewL( KMaxFileName ) ;
	TPtr docIdPtr = path16->Des();
	CnvUtfConverter::ConvertToUnicodeFromUtf8( docIdPtr,aLaunchInfo ) ;
	CAiwGenericParamList& paramList( iHandler->InParamListL() );
	RWsSession ws;
        User::LeaveIfError( ws.Connect() );
	TFindFile AufFolder(fs);
	fs.Entry( *path16, fileEntry );
	TBool testPath = BaflUtils::FileExists( fs, *path16 );
	RFile file;
        TInt error(0);
        TRAP(error, file.Open( fs, docIdPtr, EFileShareReadersOnly ) );
	CleanupClosePushL( file );
	if ( !( fileEntry.IsDir() ) && testPath  ) 
	    {
		RApaLsSession session;
		if( session.Connect() != KErrNone )
	        {
			return;
			}
		 TUid uid;
		 TDataType dataType;
		 TInt err = session.AppForDocument( *path16, uid, dataType );
		 session.Close();
		 TDataType datatype/*(Kpng)*/;
			
		 // open file item with embeded mode
		 TInt err1( KErrNone );
		 TInt err2( KErrNone );
		 TRAP(err1, err2 = iHandler->OpenFileL( file, dataType ););
		 if ( err2 == KErrNotSupported || err2 == KMimeNotSupported ||
				 err1 == KErrNotSupported || err1 == KMimeNotSupported )
				{

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
		 CleanupStack::PopAndDestroy( &file );
		 }
    fs.Close();
	ws.Close();
	if( path16 )
		{
		delete path16;
		path16 = NULL;
		}
	}
	
// -------------------------------------------------------------------------------------------------
// CSrchUiResultView::LaunchContactApplicationL
// Open a contact with new API
// -------------------------------------------------------------------------------------------------
//
void CSrchUiResultView::LaunchContactApplicationL( const TDesC8& aLaunchInfo )
	{

	MCCAParameter* parameter = TCCAFactory::NewParameterL();
	CleanupClosePushL( *parameter );
	
	parameter->SetConnectionFlag(MCCAParameter::ENormal);
	parameter->SetContactDataFlag(MCCAParameter::EContactLink);
	
	HBufC* buf=HBufC::NewLC(aLaunchInfo.Length());
	buf->Des().Copy(aLaunchInfo);

	parameter->SetContactDataL(*buf);
	CleanupStack::PopAndDestroy();
 	 
	iConnection->LaunchAppL( *parameter );
	
	CleanupStack::Pop(); //param

	}
// End of File
