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
* Description:     This class is the interface class between the UI and Search Framework. It implements the * functionality needed by Search Framework to perform a search operation. 
*
*/






#include <srchui.rsg>
#include <searchcontent.h>
#include <searchlightresult.h>
#include <searchresult.h>
#include <searchiconinfo.h>
#include <aknnotewrappers.h>
#include <AknsUtils.h>
#include <srchuiindevice.rsg>
#include <srchuiindevicesvg.mbg>
#include <searchcommon.hrh>

#include "srchuimanager.h"
#include "srchuidefines.h"
#include "srchuiresfolder.h"
#include "utf.h"

// ----------------------------------------------------------------------------
// CSrchUiManager::NewL
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
CSrchUiManager* CSrchUiManager::NewL()
    {
    CSrchUiManager* self = CSrchUiManager::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ----------------------------------------------------------------------------
// CSrchUiManager::NewLC
// Symbian OS 2 phased constructor.
// ----------------------------------------------------------------------------
//
CSrchUiManager* CSrchUiManager::NewLC()
    {
    CSrchUiManager* self = new ( ELeave ) CSrchUiManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CSearchClientSession::NewLC
// Destructor
// ----------------------------------------------------------------------------
//
CSrchUiManager::~CSrchUiManager()
    {

	delete( iClientSession );
	
	
	 
    iServices.ResetAndDestroy();
    iSelectedContentTypes.Reset();
     REComSession::FinalClose();

    //RMD
    iSupportedContentClasses.Reset/*AndDestroy*/();
      
    }

// ----------------------------------------------------------------------------
// CSrchUiManager::LoadServicesL
// Loads the services that are implemented as the SearchUiCustomPlugIns
// ----------------------------------------------------------------------------
//
void CSrchUiManager::LoadUiServicesL( RPointerArray<TServiceInfo>& aServiceInfoArray )
    {
	// Get list of all available ecom implementations information
	RImplInfoPtrArray infoArray;
	CSrchUiServicePlugin::ListAllImplementationsL( infoArray );

    // Scan through the list of available implementations
	for ( TInt i=0; i<infoArray.Count(); i++ )
	    {
	    CImplementationInformation& info( *infoArray[i] );
	    
        // Construct plug-in and pass it to the search session
	    TUid impUid ( info.ImplementationUid() );
	    CSrchUiServicePlugin* service = NULL;
	    TRAPD ( err, service = InitializeUiPlugInsL( impUid ) );
    
        if ( err == KErrNone && service )
            {
            CleanupStack::PushL( service );
            iServices.AppendL( service );
   	        CleanupStack::Pop(service);
            }
        }
          //Get the Plugin infos
    LoadPluginInfoL( aServiceInfoArray );
    infoArray.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CSrchUiManager::HandleSelectionL
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
void CSrchUiManager::HandleSelectionL( TDesC8& aQueryString,TUid aServiceId, 
	RArray <TBool> /*aSelectedContentTypes*/, MSearchQueryObserver* aObserver )
    {

    FireSeacrhL( aQueryString, aServiceId, aObserver );
    return ;
	}


// ----------------------------------------------------------------------------
// CSrchUiManager::GetSupportedContentClassesL
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
void CSrchUiManager::GetSupportedContentClassesL(
		RPointerArray<CSearchContent>& aSupportedContentClasses,
        const TUid& aServiceId )
                                                     
    {

    iSelectedContentTypes.Reset();
    
    TInt i = 0;
    for ( i=0; i<iSupportedContentClasses.Count(); i++ )
        {
        iSelectedContentTypes.Append( EFalse );
        }
	SetAllContentClassesSelected( EFalse );
    
    if ( iSupportedContentClasses.Count() > 0 )
        {
    	return;
        }
   
    iClientSession->GetSupportedContentL(iSupportedContentClasses,aServiceId);
    
    i = 0;
    RArray<TBool> supportedFound;
    //Set the all elements in array to false
    for ( i = 0; i < iSupportedContentClasses.Count(); i++ )
    	{
    	supportedFound.AppendL( EFalse );
    	}
    i = 0;
    TInt allPluginsCount = 0;
    
    RArray<TInt> pluginsArray; 
	//All plugins in the order given by UI Spec
	pluginsArray.AppendL( KSearchCClassMessages );
	pluginsArray.AppendL( KSearchCClassMusic );
	pluginsArray.AppendL( KSearchCClassImages );
	pluginsArray.AppendL( KSearchCClassVideos );
	pluginsArray.AppendL( KSearchCClassCalendar );
	pluginsArray.AppendL( KSearchCClassNotes );
	pluginsArray.AppendL( KSearchCClassContacts );
	pluginsArray.AppendL( KSearchCClassFilenames );

    //Arrange elements in iSupportedContentClasses in the order
    //of pluginsArray into aSupportedContentClasses
    for ( i = 0; i < pluginsArray.Count(); i++ ) 
        {
    	for ( TInt j = 0; j < iSupportedContentClasses.Count(); j++ ) 
    		{
    		CSearchContent* tmpContent = NULL;
    		tmpContent =  iSupportedContentClasses[j];
    		TInt pluginId = pluginsArray[i];
    		if ( tmpContent->ContentId().iUid == pluginId )
    			{
    			supportedFound[j] = ETrue;
    			aSupportedContentClasses.AppendL( tmpContent );
    			allPluginsCount++;
    			break;
    			}
    		}
        }
    pluginsArray.Reset();
    //Copy rest of the pluginIds that are not there in pluginsArray
    //into aSupportedContentClasses
    if( iSupportedContentClasses.Count() != allPluginsCount ) 
    	{
    	for ( i = 0; i < supportedFound.Count(); i++ ) 
    		{
    		if ( EFalse == supportedFound[i] )  
    			{
    			aSupportedContentClasses.AppendL( 
    				iSupportedContentClasses[i] );
    			}
    		}
    	}
    supportedFound.Reset();
    //Copy the same into iSupportedContentClasses
    i = 0;
    iSupportedContentClasses.Reset();
    iSelectedContentTypes.Reset();
    for ( ; i < aSupportedContentClasses.Count(); i++ )  
    	{
    	CSearchContent* tmpContent = NULL;
    	tmpContent = aSupportedContentClasses[i] ;
    	iSupportedContentClasses.AppendL( aSupportedContentClasses[i] );
    	tmpContent =  iSupportedContentClasses[i];
    	iSelectedContentTypes.Append( EFalse );
    	}

	SetAllContentClassesSelected( EFalse );

    }

// ----------------------------------------------------------------------------
// CSrchUiManager::SetThreshold
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
void CSrchUiManager::SetThreshold( TSearchThreshold aThreshold )
    {
    iThreshold = aThreshold;
    }

void CSrchUiManager::GetResultL( RPointerArray<CSearchDocumentId>& aResultIDArray,
		 MSearchResultRetrieveObserver* aObserver )
	{
    iClientSession->GetResultsL( aResultIDArray,*aObserver );
	}


// -------------------------------------------------------------------------------------------------
// CSrchUiManager::CancelSearch
// Cancels the search operation
// -------------------------------------------------------------------------------------------------
//
TInt CSrchUiManager::CancelSearch()
	{
	//TInt numberofResults = 0;
    iClientSession->CancelSearch();
    return(0);    
	}

// ----------------------------------------------------------------------------
// CSrchUiManager::AllContentClassesSelected
// Getter for iAllContentClassesSelected.
// ----------------------------------------------------------------------------
//
TBool CSrchUiManager::AllContentClassesSelected() const
    {
    return iAllContentClassesSelected;
    }
    
// ----------------------------------------------------------------------------
// CSrchUiGeneral::SetAllContentClassesSelected
// Setter for iAllContentClassesSelected
// ----------------------------------------------------------------------------
//
void CSrchUiManager::SetAllContentClassesSelected( TBool aSelected )
    {
    iAllContentClassesSelected = aSelected;
    }

// ----------------------------------------------------------------------------
// CSrchUiGeneral::SelectedContentTypes
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
RArray<TBool>& CSrchUiManager::SelectedContentTypes()
    {
    return iSelectedContentTypes;
    }
    
// ----------------------------------------------------------------------------
// CSrchUiGeneral::SelectedContentTypes
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
void CSrchUiManager::SetSelectedContentTypesL( RArray<TBool>&
				 aSelectedContentTypes )
    {
    iSelectedContentTypes.Reset();
    	
    for ( TInt i = 0; i < aSelectedContentTypes.Count(); i++ )
    	{
    	iSelectedContentTypes.Append( aSelectedContentTypes[i] );
    	} 
    }
    
// ----------------------------------------------------------------------------
// CSrchUiGeneral::UpdateContentClassesL
// Updates the content class ( for Context Sensitivity )
// ----------------------------------------------------------------------------
//
void CSrchUiManager::UpdateContentClassesL( 
	RPointerArray< CSearchContent> aContentClass)//, TInt aCheckedCount )
	{
	iSupportedContentClasses.Reset();
   	CSearchContent const * tmpContent;
   	for ( TInt i = 0; i < aContentClass.Count(); i++ )
    	{
    	tmpContent = aContentClass[i];
    	iSupportedContentClasses.Append( tmpContent );
    	} 
	}

TInt CSrchUiManager::MaxResults() 
    {
	return iThreshold.iMaxResults;
    }	
	
// ----------------------------------------------------------------------------
// CSearchClientSession::NewLC
// constructor
// ----------------------------------------------------------------------------
//
CSrchUiManager::CSrchUiManager()//:iSupportedContentClasses( 20 )
    {
    }


// ----------------------------------------------------------------------------
// CSrchUiManager::ConstructL
// Performs the second phase construction.
// ----------------------------------------------------------------------------
//
void CSrchUiManager::ConstructL()
    {
    iClientSession = CSearchClientSession::NewL();
    
    iThreshold.iMaxResults = 1000;
    iThreshold.iUpdateGranularity = 5 ;
    TTimeIntervalMicroSeconds32 ams( 1000000 );
    iThreshold.iUpdateInterval = ams;
    
    }

// ----------------------------------------------------------------------------
// CSrchUiManager::InitializeUiPlugInsL
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
CSrchUiServicePlugin* CSrchUiManager::InitializeUiPlugInsL( TUid aImpUid )
    {
    CSrchUiServicePlugin* service = CSrchUiServicePlugin::NewL( aImpUid );
    return service;
    }


// ----------------------------------------------------------------------------
// CSrchUiManager::LoadPluginInfoL
// Other items are commented in the header
// ----------------------------------------------------------------------------
//
void CSrchUiManager::LoadPluginInfoL( RPointerArray<TServiceInfo>& 
				aServiceInfoArray )
    {
    //
    //Get the service id that are supported
    RArray<TUid> serviceidarray;
    iClientSession->GetSupportedServicesL( serviceidarray );
    serviceidarray.Reset();
    serviceidarray.Close();

	for ( TInt i=0; i<iServices.Count(); i++ )
	    {
	    TServiceInfo* serviceInfo = new ( ELeave ) TServiceInfo;
	    CleanupStack::PushL( serviceInfo );

		serviceInfo->iServiceId = iServices[i]->ServiceId();
		serviceInfo->iPrimaryCaption = HBufC::NewL(
							iServices[i]->PrimaryCaptionL().Length() );	   										   
		*( serviceInfo->iPrimaryCaption ) = iServices[i]->PrimaryCaptionL();
		serviceInfo->iSecondaryCaption = HBufC::NewL(
							iServices[i]->SecondaryCaptionL().Length());
		*( serviceInfo->iSecondaryCaption ) = iServices[i]->SecondaryCaptionL();
		
		serviceInfo->iVisible = iServices[i]->IsVisible();
		   
		serviceInfo->iServiceIcon = iServices[i]->ServiceIconL();
		serviceInfo->iServiceIconMask = iServices[i]->ServiceIconMaskL();

		//The below comments will have to be removed once service id is finalised

	    aServiceInfoArray.AppendL( serviceInfo ); 
			CleanupStack::Pop( serviceInfo );
	    }
	
    }




// -----------------------------------------------------------------------
// CSrchUiManager::FireSeacrhL
// Calls clients searchL function with the query to be searched
// -----------------------------------------------------------------------
//	
void CSrchUiManager::FireSeacrhL( TDesC8& aQueryString,TUid aServiceId, 
			MSearchQueryObserver* aObserver )
	{
	//First get supported snippet formats
    //Create the search requirement
	iSearchRequirement = CSearchRequirement::NewL();
	
    RArray<TUid> contentidArray;
    
    for ( TInt i = 0; i<iSelectedContentTypes.Count() ; i++ )
	    {
		TUid tmpUid;
	    if ( iSelectedContentTypes[i] )
	        {
	      	tmpUid.iUid = iSupportedContentClasses[i]->ContentId().iUid;
    	    contentidArray.AppendL( tmpUid );
	        }
        }
	
    CSearchQuery* query = iSearchRequirement->CreateRequestL( aQueryString, 
                                             aServiceId, contentidArray );
    CleanupStack::PushL( query );
	
    //The search request
    iClientSession->SearchL( *query,*aObserver, iThreshold );
    CleanupStack::Pop( query );
	
    if ( iSearchRequirement )
        {
        delete( iSearchRequirement );
        iSearchRequirement = NULL;
        }
    contentidArray.Reset();
	
    }



// -----------------------------------------------------------------------
// Launches a document
// -----------------------------------------------------------------------
//
void CSrchUiManager::LaunchApplicationL( const CSearchDocumentId& aResultId ) 
	{
	iClientSession->LaunchApplicationL( aResultId ) ;
	}
// -----------------------------------------------------------------------------
// CSrchUiManager::GetLaunchInfoL
// Get the file launch information from Search Server
// -----------------------------------------------------------------------------
//
HBufC8* CSrchUiManager::GetLaunchInfoL( const CSearchDocumentId& aDocumentID )
	{
	return iClientSession->GetLaunchInfoL( aDocumentID );
	}
