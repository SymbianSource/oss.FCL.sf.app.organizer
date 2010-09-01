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



#ifndef C_CCONTACTSSEARCHPLUGIN_H
#define C_CCONTACTSSEARCHPLUGIN_H

//SYSTEM INCLUDES
#include <searchplugin.h>
#include <searchcontentsearcher.h>
#include <searchcontent.h>

//USER INCLUDES
#include "contactssearcher.h"

//FORWARD DECLARATION
class CVwsSessionWrapper;
/**
 *  This class is the plugin interface class for the contacts pluging
 *
 *  This class searches the contacts using the virtual phonebook2 apis
 *
 *  @lib contactssearchplugin.lib
 */
class CContactSearchPlugin:public CSearchPlugin 
{
public: 

	/**
    * Ecom interface static factory method implementation.
    *
    * @return A pointer to the created instance of CContactSearchPlugin
    */
	static CContactSearchPlugin* NewL();
	
	
	/**
	* Destructor
	*/
	virtual ~CContactSearchPlugin();
	
public: //from base class CSearchPlugin	
	/**
	* From CSearchPlugin
    * Returns the reference of ContentSearcher.
    *
    * @param aContentIdArray Array of Content Ids to be searhed.
    * @param aCondition condition for Search.
    * @return MSearchContentSearcher pointer for Search.
    */
    MSearchContentSearcher* ContentSearcherL(  const RArray<TUid>& aContentIdArray, 
                                           	const CSearchCondition& aCondition ,
                                           	MSearchPluginObserver& aObserver);
                                           	
 	/**
 	* From CSearchPlugin
    * Returns the supported content of this plugin.
    *
    * @param aContent The content that this plugin can search.
    */
    void GetSupportedContentL( RPointerArray<CSearchContent>& aContent );
    
    /**
    * From CSearchPlugin
    * Indicates if this plugin supports the searching of the given content.
    *
    * @param aContentId The ID of the search content.
    * @return ETrue if search is supported, EFalse otherwise.
    */
    TBool IsSupportedContent( TUid aContentId );
    
    
    /**
    * From CSearchPlugin
	* Launches a viewer/editor application for the given document id.
	*
	* @param aDocumentId Id of the document.
	*/     
	void LaunchApplicationL( const TDesC8& aLaunchInfo );
	
private:
	 /**
    * Constructor
    *
    * 
    */
	CContactSearchPlugin();
	
	/** 
    * 2nd phase constructor
    *
    * 
    */
	void ConstructL();
	
private: //Data members
     
    /**
    * The Contact searcher. 
    * Not Own
    */	
	CContactsSearcher* iContactsSearcher;
	
	CVwsSessionWrapper* iViewSrvSession;
	/**
    * The caption for contacts. read from the resource file
    * Own
    */
    HBufC*	iContactsCaption;	
		
	/**
    * The  caption for phone memory. read from the resource file
    * Own
    */	
	HBufC*	 iPhoneMemoryCaption;
    
	/**
    * The  caption for sim memory. read from the resource file
    * Own
    */	
    HBufC*  iSimCaption;
};


#endif //C_CCONTACTSSEARCHPLUGIN_H