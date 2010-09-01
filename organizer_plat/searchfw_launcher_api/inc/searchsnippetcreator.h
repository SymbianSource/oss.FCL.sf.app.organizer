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
* Description:  Creates a snippet from the buffer given the keyword position
*
*/


#ifndef C_SEARCHSNIPPETCREATOR_H
#define C_SEARCHSNIPPETCREATOR_H

//System Includes
#include <e32base.h>

//Forward Declaration
class CSearchTextSnippet;


/**
 * This class creates a snippet from the buffer,
 * using the input keyword position
 *
 * @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchSnippetCreator ) : public CBase
    {
public:
	
    /** 
     * Symbian OS 2 phased constructor.
     * 
     * @return	A pointer to the created instance of CSearchTextSearcher.
     */
     IMPORT_C static CSearchSnippetCreator* NewL();
		
    /**
     * Destructor.
     */
     virtual ~CSearchSnippetCreator();
		
    /**
     * Creates the snippet based on key position
     *
     * @param aOriginalText - The original text.
     * @param aKeywordCharPos - The position where the first keyword occurs.
     * @return - Pointer to text snippet.
     */
     IMPORT_C CSearchTextSnippet* CreateTextSnippetL( 
     	  const TDesC16& aOriginalText, TInt aKeywordCharPos );
								                                     
    /**
     * Sets the parameters for the search.
     *
     * @param aOriginalText - The original text.
     * @param aKeywordCharPos - The position where the first keyword occurs.
     * @return - Pointer to text snippet.
     */
     IMPORT_C CSearchTextSnippet* CreateTextSnippetL( 
     		const TDesC8& aOriginalText,  TInt aKeywordCharPos );
     		

private:
    
    /** 
     * Constructor
     */
     CSearchSnippetCreator();
		
    /**
     * Performs the second phase construction.
     */
     void ConstructL();	
     
    /**
     * Moves backward till the end of the word.
     *
     * @param aText - The original text.
     * @param aKeypos - The position of the word end.
     * @return - 0 if word is the last word to occur or 
     *           the position where the word begins.
     */
     TInt MoveBackwardToEndOfWord(const TPtrC& aText , TInt aKeypos );
    
    /**
     * Moves backward till the start of the word.
     *
     * @param aText - The original text.
     * @param aKeypos - The position of the word start.
     * @return - 0 if word is the first word to occur 
     *           or the position where the word begins.
     */
     TInt MoveBackwardToStartOfWord( const TPtrC& aText , TInt aKeypos );
    
    /**
     * Moves forward till the start of the word.
     *
     * @param aText - The original text.
     * @param aKeypos - The position of the word start.
     * @return - 0 if word is the first word to occur or 
     *         the position where the word begins.
     */
     TInt MoveForwardToStartOfWord( const TPtrC& aText , TInt aKeypos ); 
    
    /**
     * Moves forward till the end of the word.
     *
     * @param aText - The original text.
     * @param aKeypos - The position of the word start.
     * @return - 0 if word is the last word to occur or 
     *           the position where the word begins.
     */
     TInt MoveForwardToEndOfWord( const TPtrC& aText , TInt aKeypos );
    
    /**
     * Moves backward till the end of the word. 8 bit variant
     *
     * @param aText - The original text.
     * @param aKeypos - The position of the word end.
     * @return - 0 if word is the last word to occur or 
     *           the position where the word begins.
     */
     TInt MoveBackwardToEndOfWord(const TPtrC8& aText , TInt aKeypos );
    
    /**
     * Moves backward till the start of the word.8 bit variant
     *
     * @param aText - The original text.
     * @param aKeypos - The position of the word start.
     * @return - 0 if word is the first word to occur or 
     *           the position where the word begins.
     */
     TInt MoveBackwardToStartOfWord( const TPtrC8& aText , TInt aKeypos );
    
    /**
     * Moves forward till the start of the word.8 bit variant
     *
     * @param aText - The original text.
     * @param aKeypos - The position of the word start.
     * @return - 0 if word is the first word to occur or
     *           the position where the word begins.
     */
     TInt MoveForwardToStartOfWord( const TPtrC8& aText , TInt aKeypos ); 
    
    /**
     * Moves forward till the end of the word.8 bit variant
     *
     * @param aText - The original text.
     * @param aKeypos - The position of the word start.
     * @return - 0 if word is the last word to occur or 
     *           the position where the word begins.
     */
     TInt MoveForwardToEndOfWord( const TPtrC8& aText , TInt aKeypos );
   
    
private:    //data
    
    /**
     * Text snippet formed
     * Not own
     */
     CSearchTextSnippet*  iTextSnippet; 
    
    };

#endif		// C_SEARCHSNIPPETCREATOR_H

