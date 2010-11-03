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
* Description:  ECom search interface definition
*
*/


#ifndef C_SEARCHPLUGIN_H
#define C_SEARCHPLUGIN_H

//  INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>

// FORWARD DECLARATIONS
class CSearchContent;
class CSearchCondition;
class MSearchContentSearcher;
class CSearchDocumentId;
class MSearchPluginObserver;
// CLASS DECLARATION

/**
* Search plug-ins (Ecom) interface definition.
*/
class CSearchPlugin : public CBase
    {

public: // Constructors and destructor

    /**
    * Ecom interface static factory method implementation.
    *
    * @param aImpUid Ecom's implementation uid
    * @return A pointer to the created instance of CSearchPlugin
    */
    static inline CSearchPlugin* NewL( TUid aImpUid );
    
    /**
    * Ecom interface static factory method implementation.
    *
    * @param aImpUid Ecom's implementation uid
    * @return A pointer to the created instance of CSearchPlugin
    */
    static inline CSearchPlugin* NewLC( TUid aImpUid );
    
    /**
    * Destructor
    */
    virtual ~CSearchPlugin();

public: // New functions

    /**
    * Lists all implementations which satisfy this ecom interface.
    *
    * @param aImplInfoArray On return, contains the list of available implementations
    */
    static inline void ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray );

    /**
    * Gets the plugin id.
    *
    * @return Id of the plugin.
    */   
    inline TUid PluginId() const;
    
    /**
    * Gets the service id.
    *
    * @return Id of the service this plug-in handles.
    */ 
    virtual TUid ServiceId() const
    {    	
		TUid tempUid;
		tempUid.iUid = 0;
		return tempUid;
    }
    
   /**
    * Returns the supported content of this plugin.
    *
    * @param aContent The content that this plugin can search.
    */
    virtual void GetSupportedContentL( RPointerArray<CSearchContent>& aContent ) = 0;

    /**
    * Indicates if this plugin supports the searching of the given content.
    *
    * @param aContentId Id of the search content. Can be either class or a folder.
    * @return ETrue if search is supported, EFalse otherwise.
    */   
    virtual TBool IsSupportedContent( TUid aContentId ) = 0;
   
    /**
    * Get a content searcher.
    *
    * @param aContentIdArray Content to be searched for.
    * @param aCondition The conditions that must be fullfilled.        
    * @param aRealm The realm where the search should be limited to.
    * @return Pointer to the content searcher.
    */       
    virtual MSearchContentSearcher* ContentSearcherL( const RArray<TUid>& aContentIdArray, 
                                                      const CSearchCondition& aCondition,
                                                      MSearchPluginObserver& aObserver ) = 0;

    /**
    * Launches a viewer/editor application for the given document id.
    *
    * @param aDocumentId Id of the document.
    */     
    virtual void LaunchApplicationL( const TDesC8& aLaunchInfo ) = 0;
	 
    /**
    * Returns the extension for this interface. If no extension is available, return NULL.
    *
    * @param aExtensionUid Uid of the extension.
    */ 
    virtual TAny* SearchPluginExtensionL( TUid /*aExtensionUid*/ ) { return NULL; }

private:    // Data 
    
    TUid iDtor_ID_Key;
    TUid iPluginId;
    TInt32 iExtension1;
    TInt32 iExtension2;     
    };

#include "searchplugin.inl"

#endif		// SEARCH_ECOM_INTERFACE_H

// End of File

