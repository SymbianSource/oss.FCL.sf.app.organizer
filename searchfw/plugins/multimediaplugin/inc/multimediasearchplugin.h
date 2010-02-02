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



#ifndef C_CMULTIMEDIASEARCHPLUGIN_H
#define C_CMULTIMEDIASEARCHPLUGIN_H

//SYSTEM INCLUDES
#include <searchplugin.h>
#include <searchcontentsearcher.h>
#include <searchcontent.h>

//USER INCLUDES
#include "multimediasearcher.h"


/**
 *  This class is the plugin interface class for the multimedia pluging
 *
 *  This class searches the multimedia items  using 
 *  the Content listing framework of the mediagallery2
 *
 *  @lib multimediasearchplugin.lib
 */
class CMutimediaSearchPlugin:public CSearchPlugin 
{
	public: 
	
	/**
    * Ecom interface static factory method implementation.
    *
    * @return A pointer to the created instance of CContactSearchPlugin
    */
	static CMutimediaSearchPlugin* NewL();
	
	
	/**
	* Destructor
	*/
	virtual ~CMutimediaSearchPlugin();
	
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
	void LaunchApplicationL( const TDesC8& aLaunchInfo );
	
	
private:

   /**
    * Constructor
    *
    * 
    */
	CMutimediaSearchPlugin();
	
	/** 
    * 2nd phase constructor
    *
    * 
    */
	void ConstructL();
	
	/**
	* Adds Music, Images,Video Content Classes
	*
	* @param aSearchClassUid - Id of the the search class.
	* @param aContent - Result array 
	*/     
	void AddSearchContentClassL(const TUid aSearchClassUid,
	                         	RPointerArray<CSearchContent>& aContent );


private: // Data members
     
    /**
    * The Music searcher for searching music items
    * Not Own.  
    */
	CMultimediaSearcher* iMusicSearcher;
	
	/**
    * The Image searcher for searching Images
    * Not Own.  
    */
	CMultimediaSearcher* iImagesSearcher;
	
	/**
    * The Video searcher for searching videos
    * Not Own.  
    */
	CMultimediaSearcher* iVideoSearcher;
	
		/**
    * The caption for "Images", read from the resource file
    * Own
    */
    HBufC*	iImagesCaption;	
		
	/**
    * The  caption for "Music", read from the resource file
    * Own
    */	
	HBufC*	 iMusicCaption;
    
	/**
    * The  caption for "Videos", read from the resource file
    * Own
    */	
    HBufC*  iVideosCaption;
	
};


#endif //C_CMULTIMEDIASEARCHPLUGIN_H
