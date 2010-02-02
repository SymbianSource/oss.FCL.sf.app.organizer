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
* Description:      Manager communicates with the client and srchuigenui
*
*/







#ifndef C_SEARCH_UI_MANAGER_H
#define C_SEARCH_UI_MANAGER_H
 
#include <e32base.h>
#include "searchrequirement.h"
#include <searchclientsession.h>
#include <srchuicustomservice.h>

#include "srchuiresulthandler.h"
#include "srchuiserviceplugin.h"
#include "srchuiresultviewerplugin.h"

class CFbsBitmap;
class CAknView;
class CSrchUiResFolder;
class MSrchUiFolder;
class CSrchUiResultViewerPlugin;


/**
* This structure contains the information that is needed by the 
* SearchMainView to be displayed.
*/
struct TServiceInfo
   {

public:

	/**
	 * Service Id
	 */	
	TUid iServiceId;
	
	/**
	 * Caption for service
	 */
	HBufC* iPrimaryCaption;
	
	/**
	 * Secondary Caption for service
	 */
	HBufC* iSecondaryCaption;
	
	/**
	 * Icon
	 */
	CFbsBitmap* iServiceIcon;
	
	/**
	 * Icon mask
	 */
	CFbsBitmap* iServiceIconMask;
	
	/**
	 * Visiblity
	 */
	TBool iVisible;	
    };


/**
 * Class is an interface for communication between srchgenui and 
 * search client. 
 *
 * And also constructs the result folder.
 */
NONSHARABLE_CLASS( CSrchUiManager ) : public CBase
  	{
  		
public:	
         /**
         * Symbian OS 2 phased constructor.
         *
         * @return	A pointer to the created instance of CSrchUiManager.
         */
         static CSrchUiManager* NewL();

         /**
         * Symbian OS 2 phased constructor.
         *
         * @return	A pointer to the created instance of CSrchUiManager.
         */
         static CSrchUiManager* NewLC();

         /**
         * Destructor
         */
         virtual ~CSrchUiManager();
	
	
         /**
	 * Loads the services that are implemented as the SearchUiCustomPlugIns
         *
         * @param aServiceInfoArray Service info array
         */
         void LoadUiServicesL( RPointerArray<TServiceInfo>& aServiceInfoArray );	

         /**
         * Handles the selection of the a particular service id from the Search 
         * Main View.If the Service plug in uses light customization, then InDevice
         * /ServiceMainView is constructed and launched using GeneralSearch Ui
         * Else, if the Service plug in uses heavy customization, then  
         * of the plug in is called... If the query string is not NULL, 
         * then the search is launched for the selected service, without launching
         * the Service/InDeviceMainView
         *
         * Called when the service has been selected by the user.
         *
         * @param aQueryString The query string entered by the user.
         * @param aServiceId The ServiceId of the service.
         * @param  aSelectedContentTypes All the selected content types
         *@param aObserver Observer
         */   
         void HandleSelectionL ( TDesC8& aQueryString,  TUid aServiceId, 
                                 RArray <TBool> aSelectedContentTypes, MSearchQueryObserver* aObserver );

         /**
         * Gets the supported content classes by the SearchFW
         * Used by the Service Main view of General Search Ui to display 
         * the content classes
         *
         * @param aSupportedContentClasses supported content classes will be 
         				placed in this variable
         * @param aServiceId Service Id of which to be 
         */
         void GetSupportedContentClassesL ( RPointerArray<CSearchContent>& 
	 		aSupportedContentClasses, const TUid& aServiceId );
	
         /**
         *
         * Sets the threshold
         * @param aThreshold
         */
         void SetThreshold( TSearchThreshold aThreshold );
   	
      
         /**
         * Gets the results
         *
         * @param aObserver Obsrever through which results are retrieved
         */
         void GetResultL( RPointerArray<CSearchDocumentId>& aResultIdArray,
     		 MSearchResultRetrieveObserver* aObserver );
    
 	
         /**
  	 * Getter for iAllContentClassesSelected flag
         */
         TBool AllContentClassesSelected() const;
	
         /**
         * Setter for iAllContentClassesSelected flag
         */
         void SetAllContentClassesSelected( TBool aSelected );
	
         /**
         * Get the selected content types
         *
         * @return Boolean values if the selected in same order as the content 
         * 			types
         */
         RArray<TBool>& SelectedContentTypes();
    
         /**
         * Set the selected content types
         *
         * @param aSelected
         */
         void SetSelectedContentTypesL( RArray<TBool>& aSelected );
    
         /**
         * Updates the supported content classes to the passed parameter
         *
         * @param aContentClass content classes to be updated
         */
         void UpdateContentClassesL( RPointerArray< CSearchContent> aContentClass );
	
         /**
         * returns max results allowed
         *
         */
         TInt MaxResults();
	
         /**
         * Getter for light result count
         *
         * @return The number of light result count
         */
         TInt LightResultCount();
	
         /**
         * Sets the last visited view id
         *
         * @param aViewId : the view id of the previous view
         */
         void SetLastLaunchViewId(const TUid aViewId);
	 
	 
         /**
         * Get the Query string
         *
         * @since S60 v3.2
         * @return : reference to the query string
         */
         TDesC8& GetQueryString();

         /**
         * From MSrchUiResultHandler
         *
         * Launches the application for given document id
         *
         * @param aResultId Document Id for which is to be launched
         */
         void LaunchApplicationL( const CSearchDocumentId& aResultId );
    
         /**
         * Cancels the search operation
         */
         TInt CancelSearch();
         /**
		 * Get the launch infor 
		 */         
         HBufC8* GetLaunchInfoL( const CSearchDocumentId& aDocumentID );
    					
private:
         // C++ constructor - not exported;
         // implicitly called from NewLC()
         /**
         * Constructor
         *
         */
         CSrchUiManager( );
	
         /**
         * second-phase constructor
         *
         */
         void ConstructL( ); 
	
         /**
         * Initializes the Ui Plugins
         *
         * @param aImpUid Implementation Uid
         */	
         CSrchUiServicePlugin* InitializeUiPlugInsL(TUid aImpUid );
    	
         /**
         * Loads the Plugin information
         *
         * @param aServiceInfoArray service info array
         */
         void LoadPluginInfoL( RPointerArray<TServiceInfo>& aServiceInfoArray );
    
         /**
         * fires a search
         *
         * @param aQueryString query string to be searched
         * @param aServiceId service id
         * @param aObserver observer
         */
         void FireSeacrhL( TDesC8& aQueryString,TUid aServiceId, 
                           MSearchQueryObserver* aObserver );
 
private: //data
	
         /**
         * Service Id
         */
         TUid iServiceId;
     
         /**
         * search requirement 
         * includes query string, where to search, what to search etc.,
         *
         * Own
         */
         CSearchRequirement* iSearchRequirement;
     
         /**
         * Array of Services available
         */
         RPointerArray<CSrchUiServicePlugin> iServices;
    
         /**
         * session with the search client
         *
         * Own
         */
         CSearchClientSession* iClientSession;
    
         /**
         * Supported content classes
         */
         RPointerArray<CSearchContent> iSupportedContentClasses;
        
         /**
         * Number of requests made for e.g, 3rd search request to client
         */
         TInt iRequest;
    
         /**
         * Array of selected content type
         */
         RArray <TBool> iSelectedContentTypes;
    
         /**
         * Boolean if all classes are selected
         */
         TBool iAllContentClassesSelected;
     
     
         /**
         * Threshold for the search : Max results, max time etc.,
         */
         TSearchThreshold iThreshold;
         };

#endif //C_SEARCH_UI_MANAGER_H