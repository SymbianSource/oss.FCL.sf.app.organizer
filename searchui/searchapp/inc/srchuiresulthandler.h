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
* Description:     interface for users of MSearchClientSession to monitor the search progress 
*
*/





#ifndef C_SEARCHRESULTHANDLER
#define C_SEARCHRESULTHANDLER

#include <e32base.h>

class CSearchLightResult;
class CSearchResult;


NONSHARABLE_CLASS( MSrchUiResultHandler )
    {
public:
    /**
     * Launches application
     *
     * @param aResultId Document Id of the file to be launched
     */
     virtual void LaunchApplicationL( const CSearchDocumentId& aResultId ) = 0;
    
    /**
     * Launches the previous view
     */
     virtual void LaunchPreviousViewL() = 0;
     
    /**
     * Starts New search
     */
     virtual void StartNewSearchL() = 0;
     /**
     * cancels search
     */
     virtual TInt CancelSearch() = 0;
     
     /**
     * Returns ETrue if the search is in progress
     */
     virtual TBool IsSearching() = 0;
     /**
	 * Get the launch infor 
	 */       
     virtual HBufC8* GetLaunchInfoL( const CSearchDocumentId& aDocumentID ) = 0;
    };

#endif // C_SEARCHRESULTHANDLER
