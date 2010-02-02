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
* Description:  Handles text snippet creation
*				 
*
*/


#ifndef C_SEARCHTEXTSNIPPET_H
#define C_SEARCHTEXTSNIPPET_H

#include <e32base.h>
#include <s32strm.h>

/**
 *  class for snippet 
 *
 *  Represnts text snippet with info of whether snippet is incomplete
 *
 *  @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchTextSnippet ) : public CBase
    {

public:
    
    /** 
     * Symbian OS 2 phased constructor.
     *
     * @param None
     * @return A pointer to the created instance of CSearchTextSnippet.
     */
     IMPORT_C static CSearchTextSnippet* NewL();
    
   
    /** 
     * Symbian OS 2 phased constructor.
     *
     * @param Stream where from to construct the object
     * @return A pointer to the created instance of CSearchSnippetInfo
     */
     IMPORT_C static CSearchTextSnippet* NewL( TBool startIncomplete, TBool endIncomplete);
    
    /**
     * Destructor of the class.
     */
     virtual ~CSearchTextSnippet();

    
    /**
     * For Future Implementation
     *
     * @return 
     */
     IMPORT_C HBufC8* PackL() const;       
   
    /**
     * Sets the snippet.
     *
     * @param aSnippet The snippet.
     */
     IMPORT_C void SetSnippetL( const TDesC8& aSnippet );

    /**
     * Gets the snippet.
     *
     * @param aSnippet The snippet.
     * @return The snippet.
     */   
     IMPORT_C const TDesC8& Snippet() const;

    /**
     * Setter for iSnippetStartIncomplete.
     *
     * @param aSnippetStartIncomplete.
     */
     IMPORT_C void SetSnippetStartIncomplete( TBool aSnippetStartIncomplete );

    /**
     * Getter for iSnippetStartIncomplete.
     *
     * return iSnippetStartIncomplete.
     */
     IMPORT_C TBool SnippetStartIncomplete() const;

    /**
     * Setter for iSnippetEndIncomplete.
     *
     * @param aSnippetEndIncomplete.
     */
     IMPORT_C void SetSnippetEndIncomplete( TBool aSnippetEndIncomplete );
    
    /** 
     * Getter for iSnippetEndIncomplete.
     *
     * @return iSnippetEndIncomplete.
     */
     IMPORT_C TBool SnippetEndIncomplete() const;
    
private:

    /**
     * Default Constructor.
     */
     CSearchTextSnippet();
    
    /**
     * Constructor of the class.
     *
     * @param Read Data Stream Buffer.
     */
     void ConstructL( TBool startIncomplete, TBool endIncomplete);

private: //data
    
    /**
     * Snippet.
     *
     * Own
     */
     HBufC8* iSnippet;
    
    /**
     * Indicates that the snippet start is incomplete.
     */
     TBool iSnippetStartIncomplete;
		
    /**
     * Indicates that the snippet end is incomplete.
     */
     TBool iSnippetEndIncomplete;
    
    };

#endif // C_SEARCHTEXTSNIPPET_H
