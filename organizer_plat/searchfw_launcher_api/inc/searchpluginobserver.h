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
* Description:  Observer for search progress. 
*               Monitor when search is complete.
*
*/


#ifndef SEARCHPLUGINOBSERVER_H
#define SEARCHPLUGINOBSERVER_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS

class CSearchLightResult;
class CSearchResult;

/**
* Observer class for the search plug-ins.
*/
class MSearchPluginObserver
    {
public: // New functions
    /**
    * Called when a is result is found.
    *
    * @param aResult Pointer to the found result. Ownership is transferred.
    * @param aCurrentDocument The index of document that is currently being searched.
    * @param aTotalDocuments The total count of documents to be searched (from all content).
    */
    virtual void ResultFoundL( CSearchLightResult* aResult,                                
                               TInt aCurrentDocument,
                               TInt aTotalDocuments ) = 0;

    	
    /**
    * Called when the plug-in has completed the search process.
    *
    * @param aSearchResult Result of the search. KErrNone of OK, otherwise an error code. 
    * @param aTotalNumberOfResults The number of total search results.
    */
    virtual void SearchCompletedL( TInt aSearchResult, TInt aTotalNumberOfResults ) = 0;
    virtual void SearchPluginCompletedL( TUid aUid ) = 0;

    /**
    * Called when operation is complete.
    *
    * @param aResultArray The results returned by the operation. 
    *        Ownership of the items is transferred to the caller.
    */   
    virtual void ResultsRetrieveCompleteL( RPointerArray<CSearchResult>& aResultArray ) = 0;

    /**
    * Returns the extension for this interface. If no extension is available, return NULL.
    *
    * @param aExtensionUid Uid of the extension.
    */ 
    virtual TAny* SearchPluginObserverExtensionL( TUid /*aExtensionUid*/ ) { return NULL; }
    };
    
#endif		// SEARCHPLUGINOBSERVER_H

// End of File

