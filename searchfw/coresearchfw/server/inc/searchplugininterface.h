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
* Description:   The function definitine for creating plugin.
*
*/



#ifndef SEARCH_PLUGIN_INTERFACE_H
#define SEARCH_PLUGIN_INTERFACE_H

#include "searchpluginobserver.h"
#include <searchplugin.h>
#include <coemain.h>

// CLASS DECLARATION	

/**
* Class is used to instantiate all the plugins when the search has been
* asked. Acts as interface to all Plugins
*/
class CSearchPluginInterface: public CBase 
{
	public:
		
		/** 
		* Symbian OS 2 phased constructor.
		* @return	A pointer to the created instance of CSearchPluginInterface.
		*/
	  	static CSearchPluginInterface* NewL();

		/** 
		* Symbian OS 2 phased constructor.
		* @return	A pointer to the created instance of CSearchPluginInterface.
		*/
	  	static CSearchPluginInterface* NewLC();
	  
	   	/**
	    * Destructor.
	    */      
	    virtual ~CSearchPluginInterface();

		/**
		* Constructs all search plug-ins known by the ecom framework.
		*/
	  	void InstantiateAllPlugInsL( );
	  
	  	/**
		* Unloads all plugins
		*/
	  	void UnloadPlugIns();
	  
	  	/**
    	* Gets supported service ids by the search framework.
   	 	*
    	* @param aContent On return contains the list of supported service ids.
    	*/   
    	void GetSupportedServicesL( RArray<TUid>& aServiceIdArray );
    	
		  /**
	    * Returns the supported content of this plugin.
	    *
	    * @param aContent The content that this plugin can search.
	    * @param aServiceId The service id that the content should be limited to. That is, if the given id
	                        is e.g. 12345678, the aContent should contain only content that is provided for
	                        service id 12345678. If the given id is 0, all content should be given.
	    * @param aRealm The realm filter for the content.
	    */
	     void GetSupportedContentClassesL( RPointerArray<CSearchContent>& aContent,
	                                        TUid aServiceId  ) ;
		/**
	    * Indicates if this plugin supports the searching of the given content.
	    *
	    * @param aPluginList List of Plugins
	    * @param aContent The search content.
	    */   
		 void GetSupportingPlugInsL( RPointerArray<CSearchPlugin>& aPluginList, const RArray<TUid>& aContent );

	  	/**
	    * Indicates Plugins to Launch specified application.
	    * @param aDocumentId Document ID
	    */   
	    void LaunchApplicationL( const CSearchDocumentId& aDocumentId,const HBufC8* launchInfo );
      	
    private: //Functions not allowed to be used by objects
      	
	   	/** 
	    * Performs the first phase of two phase construction.
	    */
	    CSearchPluginInterface();  
    	
 		/** 
		* Symbian OS 2 phased constructor.
		*/
    	void ConstructL();
		
     //Member Functions
     
       /**
		* Instantiates a search plug-in, knowing the implementation uid.
		* @param aImpUid imp uID
		*/
	    CSearchPlugin* InstantiatePlugInFromImpUidL( const TUid& aImpUid );
	  
	  private: //Data
		RPointerArray<CSearchPlugin>  iSearch;
		
};
#endif


//End of File