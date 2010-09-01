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
* Description:   Plugin for file names search 
*
*/





#ifndef C_FILESSEARCHPLUGIN_H
#define C_FILESSEARCHPLUGIN_H

#include <searchplugin.h>
#include <searchcontentsearcher.h>
#include <searchcontent.h>

#include "filessearcher.h"

/**
 * This class is the file search plugin class
 *
 * Provides functionalities such as getting the supported
 * content classes, launching a file etc.,
 *
 */
class CFilesSearchPlugin : public CSearchPlugin 
    {
    
public: 
	
	/**
     * Ecom interface static factory method implementation.
     *
     * @return A pointer to the created instance of CFilesSearchPlugin
     */
	static CFilesSearchPlugin* NewL();
	
	
	/**
	 * Destructor
	 */
	virtual ~CFilesSearchPlugin();
	
	//From base class CSearchPlugin
	
	/**
	 * From CSearchPlugin
     * Returns the pointer to ContentSearcher.
     *
     * @param aContentIdArray Array of Content Ids to be searhed.
     * @param aCondition condition for Search.
     * @return MSearchContentSearcher pointer for Search.
     */
    MSearchContentSearcher* ContentSearcherL(  const RArray<TUid>& aContentIdArray, 
                                           	const CSearchCondition& aCondition,
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
	void LaunchApplicationL(const TDesC8& aLaunchInfo);
	
private:
	
	/**
	 * Constructor
	 */ 
	CFilesSearchPlugin();
	
	/**
	 * 2nd phase constructor
	 */ 
	void ConstructL();

private:
	/**
	 * Caption 
	 */		
	HBufC* iCaption;
	
	/**
    * The Image searcher for searching Images
    * Not Own.  
    */
		/**
    * The Image searcher for searching Images
    * Not Own.  
    */
	CFilesSearcher* iFileSearcher;
	
	
	
    };


#endif //C_FILESSEARCHPLUGIN_H