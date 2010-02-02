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
* Description:  ECom search interface definition
*
*/


#ifndef SEARCHCONTENTSEARCHER_H
#define SEARCHCONTENTSEARCHER_H

//  INCLUDES
#include <e32std.h>
# include <searchtextsearcherobserver.h>

// FORWARD DECLARATIONS
class MSearchPluginObserver;
class CSearchResult;
class CSearchDocumentId;

/**
* Parameters for search.
* This class is used for content searcher
*
*/
class MSearchContentSearcher : public MSearchTextSearcherObserver
    {
public:
    
    /**
    * Releses the object.
    */
    virtual void Destroy() = 0;
    
    /**
    * Starts the search. Progress of the search is notified through aObserver. Notice that
    * this call must be asynchronous.
    * @param aObserver Observer for search progress.
    */
    virtual void SearchL( ) = 0;
    
    /**
    * Function returning ETrue when a search is ongoing.  EFalse otherwise.
    * 
    * @return ETrue when a search is ongoing.  EFalse otherwise.
    */
    //virtual TBool IsSearching() = 0;
    
    /**
    * Cancels the ongoing search. This call must complete synchronously and no calls for
    * observers must be made after this call.
    *
    */
    virtual void CancelSearch() = 0;
    
    /**
    * Gets the results indicated by the given document ids. Asynchronous. Results will be given
    * through callback in MSearchPlugInObserver given in Search request.
    * @param aResults Results of the search process. Ownership transfers to caller.
    */
    virtual void GetResultsL( const RPointerArray<CSearchDocumentId>& aDocumentIdArray ) = 0;

    /**
    * Cancels the result retrieval process.
    */
    virtual void CancelResultsRetrieve() = 0;

    /**
    * Gets the current search progress. 
    *
    * @param aContentClassId On return contains the content class id, which is currently searched.
    * @param aCurrentDocument On return contains the index document that is currently being searched.
    * @param aTotalDocuments On return contains the total count of documents to be searched.
    */         
    //virtual void GetSearchProgressL( TUid& aContentClassId, TInt& aCurrentDocument, TInt& aTotalDocuments ) = 0;

		virtual HBufC8* LaunchInfoL( const CSearchDocumentId& aID ) = 0;
    /**
    * Returns the extension for this interface. If no extension is available, return NULL.
    *
    * @param aExtensionUid Uid of the extension.
    */ 
    virtual TAny* ContentSearcherExtensionL( TUid /*aExtensionUid*/ ) { return NULL; }
    
    
    };

#endif		// SEARCHCONTENTSEARCHER_H

// End of File

