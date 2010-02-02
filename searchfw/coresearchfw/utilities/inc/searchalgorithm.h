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
* Description:   Search algorithm class for searching keywords in given text.
*
*/





#ifndef C_SEARCHERALGORITHM_H
#define C_SEARCHERALGORITHM_H


#include <badesca.h>
#include <e32des16.h>

#include <searchtextsearcher.h>
#include "searchkeywordstatus.h"

class CSearchTextSearcher;

/**
 * Search algorithm class for searching keywords in given text.
 *
 * @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchAlgorithm ) : public CBase 
{
        
        
public: 
        /** 
         * Symbian OS 2 phased constructor.
         *
         * @return   A pointer to the created instance of CTextSearcher.
         */
        static CSearchAlgorithm* NewL();

        /**
         * C++ Destructor 
         **/
        virtual ~CSearchAlgorithm();
        
        /**
         * Searches through a simple text.
         *
         * @param  aToBeSearchedDesIn  Text to be searched.
         * @param  aKeywordsIn   Array of keywords to be matched.
         * @param  aKeywordSearchStatusArrayOut  Array storing 
         *         the search result for each individual search keyword
         * @param  aOperatorIn
         * @param  aCaseSensitiveIn
         * @param  aWordCounterOut Total number of word counter
         * @param  aHitCounterOut  Total number of hits counter
         */
        void SearchL( const TDesC& aToBeSearchedDesIn, 
        		const CDesCArray& aKeywordsIn, 
                RArray<TKeywordSearchStatus>& aKeywordSearchStatusArrayOut,
                CSearchTextSearcher::TSearchCriteriaOperator aOperatorIn,
                TBool aCaseSensitiveIn );

        /**
         * Searches through a simple text. 8 bit varient.
         *
         * @param aToBeSearchedDesIn    Text to be searched.
         * @param aKeywordsIn           Array of keywords to be matched.
         * @param aKeywordSearchStatusArrayOut  Array storing the search 
         *                       result for each individual search keyword
         * @param aOperatorIn
         * @param aCaseSensitiveIn
         * @param aWordCounterOut    Total number of word counter
         * @param aHitCounterOut     Total number of hits counter
         */                      
        void SearchL( const TDesC8& aToBeSearchedDesIn, const CDesC8Array& aKeywordsIn, 
                  RArray<TKeywordSearchStatus>& aKeywordSearchStatusArrayOut,
                  CSearchTextSearcher::TSearchCriteriaOperator aOperatorIn, 
                  TBool aCaseSensitiveIn );
    
    	/**
    	 * Sets the flag whether searchign for email or not
    	 */
        void SearchEmailAddress( TBool aSearchEmail );
    
    
 private:
    
        /**
         * C++ Constructor 
         *
         * Performs the first phase of two phase construction
         */
         CSearchAlgorithm( );
        
        /**
         * Performs the second phase construction 
         */
         void ConstructL();
      
        /**
         * Starts the actual search.
         */
         void DoSearchL();

        /*
         * Moves to start of the first word in the text. 
         * 
         * @param    aTextIn    Text to be searched
         * @param    iCharPositionOut pointer to the each character.   
         *
         */
        TBool MoveToStartOfWord( const TDesC& aTextIn, TInt& iCharPositionOut );

        /**
         * Moves to start of the next word in the text 
         *  
         * @param    aTextIn         Text to be searched
         * @param    iCharPositionOut   pointer to each character.   
         * 
         */
         TBool MoveToStartOfNextWord( const TDesC& aTextIn, 
         		TInt& iCharPositionOut );

        /**
         * Moves to end of the current word + 1 , to skip the space and 
         * point to start of next word in row. 
         *
         * @param    aTextIn  Text to be searched
         * @param    iCharPositionOut  pointer to each character.   
         *
         */
         void  MoveToEndOfCurrentWordPlusOne( const TDesC& aTextIn,
         	 TInt& iCharPositionOut );

        /**
         * Does the comparision/match for a single keyword.
         */
         TBool CompareSingleWordWithKeywords();

        /**
         * Starts the actual search.
         */
         void DoSearch8L();

        /**
         * Moves to start of the first word in the text. 
         * 
         * @param    aTextIn    Text to be searched
         * @param    iCharPositionOut  pointer to each character.   
         */
         TBool MoveToStartOfWord8( const TDesC8& aTextIn, TInt& iCharPositionOut );

        /**
         * Moves to start of the next word in the text 
         *  
         * @param    aTextIn  Text to be searched
         * @param    iCharPositionOut pointer to each character.   
         * 
         */
         TBool MoveToStartOfNextWord8( const TDesC8& aTextIn, 
         		TInt& iCharPositionOut );

        /**
         * Moves to end of the current word + 1 , to skip the space 
         * and point to start of next word in row. 
         *
         * @param    aTextIn    Text to be searched
         * @param    iCharPositionOut   pointer to each character.   
         *
         */
         void  MoveToEndOfCurrentWordPlusOne8( const TDesC8& aTextIn, 
         	TInt& iCharPositionOut );

        /**
         * Does the comparision/match for a single keyword.
         */
         TBool CompareSingleWordWithKeywords8();
    
    
private: 
        
        /**
        * Text to search through.
        * Not owned
        */
        const TDesC* iToBeSearchedDes; 

        
        /**
        * array that contains 16 bit keywords to search
        * Not owned  
        */
        const CDesCArray* iSearchKeywords;       

        
        /**
        * Array that hold information about which parameters have been found.
        * Not owned
        */
        RArray<TKeywordSearchStatus>*  iKeywordSearchStatusArray;  

        
        /**
        * determines if this is AND or OR search
        */
        CSearchTextSearcher::TSearchCriteriaOperator iOperator;

        
        /**
        * is this case sensitive search
        */
        TBool iCaseSensitive;

        
        /**
        * Contains the word currently under search.
        */
        TPtrC iSingleWord;
        
		/**
        * Previous word position, needed when creating snippet
        */
        TInt iStartOfPreviousWord; 
        
        /**
        * Snippet position
        */
        TInt startOfWord; 
		
		/**
        * Current char position
        */
        TInt iCharPosition;
                                                                                
        /**
        * array that contains 8 bit versions from keywords to search
        * Not own
        */
        const TDesC8* iToBeSearchedDes8Bit;
        
        /**
        * Contains the 8 bit word currently under search.
        */
        TPtrC8 iSingleWord8Bit;

        /**
        * array that contains 8 bit keywords to search
        * Own
        */
        const CDesC8Array* iSearchKeywords8Bit;// Not owned

        /**
        * do we search for email address. If so, then . and 
        * @ are not considered word separators.
        */
        TBool iSearchEmailAddress;

    };
    
    
    
#endif //C_SEARCHERALGORITHM_H