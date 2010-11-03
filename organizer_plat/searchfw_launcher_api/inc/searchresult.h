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
* Description:  Class representing a search result with title, snippet
*
*/


#ifndef C_SEARCHRESULT_H
#define C_SEARCHRESULT_H

#include <e32base.h>
#include <s32strm.h>

class CSearchDocumentId;
class CSearchLightResult;

/**
 * Class representing a search result with title, snippet
 *
 * This class represents heavy result that will be displayed
 * with icon, snippet and title
 *
 * @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchResult ) : public CBase 
    {
	
public:
	
    /**
     * 1st phase constructor - Creates new object CSearchResult from 
     * the CSearchLightResult pointer
     * 
     * @param aLightResult
     * @return CSearchResult
     */
	 IMPORT_C static CSearchResult* NewL( CSearchLightResult* aLightResult );
		
    /**
     * 1st phase constructor - Creates new object CSearchResult from 
     * the CSearchResult reference
     * 
     * @param aSearchResult
     * @return CSearchResult
     */
     IMPORT_C static CSearchResult* NewL( const CSearchResult& aSearchResult );

    /**
     * 1st phase constructor - Creates new object CSearchResult from 
     * the CSearchResult reference
     * 
     * @param aSearchResult
     * @return CSearchResult
     */	
     IMPORT_C static CSearchResult* NewL( RReadStream& aStream );
	
    /**
     * virtual destructor
     */
     virtual ~CSearchResult();
	
    /**
     * Externalizes the class to the stream 
     * @param aStream - Stream to be written to
     */
     IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
	
    /**
     * This function returns const reference to the CSearchLightResult
     * @return const CSearchLightResult&
     */
     IMPORT_C const CSearchLightResult& Result() const;
	
    /**
     * Sets the human readable title of this result.
     *
     * @param aTitle The title.
     */
     IMPORT_C void SetTitleL( const TDesC& aTitle );
    
    /**
     * Gets the human readable title of this result.
     *
     * @return Title of the result.
     */
     IMPORT_C const TDesC& Title() const;
    
    /**
     * Sets the snippet of this result.
     * @param aSnippet Snippet of the result.
     */
     IMPORT_C void SetSnippetL( const TDesC8& aSnippet );
    
    /**
     * Gets the snippet of this result.
     * @return Snippet of the result.
     */
     IMPORT_C const TDesC8& Snippet() const;
    
    /**
     * Sets the snippet format of this result.
     * @param aSnippetFormat Snippet format of the result.
     */
     IMPORT_C void SetSnippetFormatL( const TDesC8& aSnippetFormat );
    
    /**
     * Gets the snippet format of this result.
     * @return Snippet format of the result.
     */
     IMPORT_C const TDesC8& SnippetFormat() const;

private:
	
    /**
     * Constructor
     *
     * @param aResult 
     */
     CSearchResult();
	
    /**
     * Constructor
     *
     * @param aResult 
     */
     CSearchResult( CSearchLightResult* aResult );
	
    /**
     * 2nd phase constructor
     *
     * @param aSearchResult 
     */	
     void ConstructL( const CSearchResult& aSearchResult );
	
    /**
     * 2nd phase constructor
     *
     * @param aStream 
     */	
     void ConstructL( RReadStream& aStream );
	
private:
	
    /**
     * Pointer to light result
     * Own
     */
     CSearchLightResult* iLightResult;  
	
    /**
     * Pointer to Snippet
     * Own
     */
     HBufC8* iSnippet; 
	
    /**
     * Pointer to Snippet format
     * Own
     */
     HBufC8* iSnippetFormat;
	
    /**
     * Pointer to title
     * Own
     */
     HBufC* iTitle; 
    
    };

#endif //C_SEARCHRESULT_H
