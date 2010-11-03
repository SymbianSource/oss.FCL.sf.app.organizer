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




#ifndef		C_CNOTESSEARCHPLUGIN_H
#define     C_CNOTESSEARCHPLUGIN_H     

//SYSTEM INCLUDES
#include <searchplugin.h>



//Forwared declarations
class MSearchContentSearcher;
class CNotesSearcher;

/**
 *  This class is the plugin interface class for the notes pluging
 *
 *  This class searches the phonebook notes
 *
 *  @lib notessearchplugin.lib
 */
class CNotesSearchPlugin : public CSearchPlugin
	{

public: 
    /**
    * 
    * First phase construction
    * 
    *
    * @return constructed CNotesSearchPlugin object.
    */
    static CNotesSearchPlugin*  NewL();
      	
    /**
    * 
    * First phase construction
    * returned value is also kept on the CleanupStack.
    *
    *
    * @return constructed CNotesSearchPlugin object.
    */
    static CNotesSearchPlugin*  NewLC();
      	
    /**
    * Destructor.
    */
    virtual ~CNotesSearchPlugin();
        
        
public: // from the base class CSearchPlugin
      
    /**
    * From  CSearchPlugin
    * Returns the supported content of this plugin.
    *
    * @param aContent The content that this plugin can search.
    */
    void GetSupportedContentL( RPointerArray<CSearchContent>& aContent );
                                       
    /**
    * Indicates if this plugin supports the searching of the given content.
    *
    * @param aContentId Id of the search content. Can be either class or a folder.
    * @return ETrue if search is supported, EFalse otherwise.
    */   
    TBool IsSupportedContent( TUid aContentId );
    	

    /**
    * Get a content searcher.
    *
    * @param aContentIdArray Content to be searched for.
    * @param aCondition The conditions that must be fullfilled.        
    * @param aRealm The realm where the search should be limited to.
    * @return Pointer to the content searcher.
    */       
    MSearchContentSearcher* ContentSearcherL( const RArray<TUid>& aContentIdArray, 
                                                      const CSearchCondition& aCondition,
                                                      MSearchPluginObserver& aObserver );
    /**
    * Launches a viewer/editor application for the given document id.
    *
    * @param aDocumentId Id of the document.
    */     
    void LaunchApplicationL( const TDesC8& aLaunchInfo );
	     
        
private:
    /**
    * Second phase construction
    */
    void ConstructL();
        
    /**
    * Constructor.
    */
    CNotesSearchPlugin();
        
private: // data members
	
	/**
    * The content searcher
    * Not Own
    */  
    MSearchContentSearcher*   iContentSearcher; 
    
	/**
    * The content searcher
    * Not Own
    */      
    CNotesSearcher* iNotesSearcher;
    };
	
#endif 	//C_CNOTESSEARCHPLUGIN_H

