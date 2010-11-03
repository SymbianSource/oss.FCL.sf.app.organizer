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
* Description:  Client observer for results
*
*/


#ifndef M_SEARCHSESSIONOBSERVERS_H
#define M_SEARCHSESSIONOBSERVERS_H

#include <e32base.h>

//Forward declaration
class CSearchLightResult;
class CSearchResult;

/**
 *  This class is used to notify search progress to user.
 *  Users of MSearchClientSession must implement this interface to be able to
 *  monitor the search progress.
 *  This class is used to notify search progress to user.
 *  @lib    searchclient.lib
 */
class MSearchQueryObserver
    {
public:
   
   /**
    * This function is called when framework notifies about the search progress. 
    * The user of CSearchClientSession  can define the time how often this will be 
    * called when requesting for search.
    * @param aResultIdArray - The array of results found between this call and last update.
    *                       Ownership of the items is transfered to the user .
    * @param aCurrentProgressValue - The current progress of the search process.
    * @param aTotalProgressValue - Final progress value of the search process.
    */   
    virtual void UpdateSearchProgressL( RPointerArray<CSearchLightResult>& aResultArray,
                                        TInt aCurrentProgressValue, 
                                        TInt aTotalProgressValue ) = 0;
    virtual void SearchPartiallyCompletedL( TUid aPluginId ) = 0;

   /**
    * This function is called when the search has been completed.
    *
    * @param aResult - If the process was completed succesfully KErrNone, otherwise
    *                  appropriate error code.
    * @param aNumberOfDocuments - The number of found documents in the search.
    */   
    virtual void SearchCompletedL( TInt aSearchResult, TInt aNumberOfFoundDocuments ) = 0;

   /**
    * Returns the extension for this interface. If no extension is available, return NULL.
    *
    * @param aExtensionUid Uid of the extension.
    */ 
    virtual TAny* QueryObserverExtensionL( TUid /*aExtension*/ ) { return NULL; }
    };


   /**
    *  This class is observer for retrieving the results.
    *  Users of MSearchResultRetrieveObserver must implement this interface 
    *  to be able to get heavy results.
    */
class MSearchResultRetrieveObserver 
    {
public:
   
    /**
    * This function is called when results retrival operation is complete.
    *
    * @param aResultArray - The results returned by the operation. 
    *        Ownership of the items is transferred to the caller.
    */   
    virtual void ResultsRetrieveCompleteL( RPointerArray<CSearchResult>& aResultArray ) = 0; 

    /**
    * Returns the extension for this interface. If no extension is available, return NULL.
    *
    * @param aExtensionUid - Uid of the extension.
    */ 
    virtual TAny* ResultRetrieveObserverExtensionL( TUid /*aExtension*/ ) { return NULL; }
    
    };

#endif // SEARCHSESSIONOBSERVERS_H
