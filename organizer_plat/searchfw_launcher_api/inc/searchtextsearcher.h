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
* Description:  Class handling the search of several keywords in a block of 
*                text.
*
*/


#ifndef      C_SEARCHTEXTSEARCHER_H
#define      C_SEARCHTEXTSEARCHER_H

//System Includes
#include <badesca.h>
#include <searchbooleancondition.h>

//Forward Declarations
class MSearchTextSearcherObserver;
class CSearchAlgorithm;
class CSearchCondition;
class CSearchSingleCondition;
class CSearchBooleanCondition;
class TKeywordSearchStatus;

/**
 * This class handles the search of several keywords 
 * in a block of text.
 * Data/text can be supplied in steps and it does not need 
 * to be known in advance.
 * @lib searchutilities.lib
 */
NONSHARABLE_CLASS( CSearchTextSearcher )  :  public CBase 
    {
    
public:

    enum TSearchCriteriaOperator
        {
        ESearchCriteriaAND,
        ESearchCriteriaOR,
        ESearchCriteriaNO  
        };
    
    /** 
     * Symbian OS 2 phased constructor.
     *
     * @param aObserver	- Text searcher's observer
     * @return - A pointer to the created instance of CSearchTextSearcher.
     */
     IMPORT_C static CSearchTextSearcher* NewL( 
                     MSearchTextSearcherObserver& aObserver );
        
    /** 
     * Symbian OS 2 phased constructor
     *
     * @param aObserver -	Text searcher's observer
     * @return - A pointer to the created instance of CSearchTextSearcher.
     */
     IMPORT_C static CSearchTextSearcher* NewLC( 
            MSearchTextSearcherObserver& aObserver );
		
    /**
     * Destructor.
     */
     virtual ~CSearchTextSearcher();
    
    /**
     * Cleans up internal data. Must be called before each new search is 
     * started. Does not clean up the parameters set via SetParametersL.
     * 
     * When same search parameters are needed with different search text, 
     * call Cleanup but not SetParametersL.
     *
     */
     IMPORT_C void Cleanup();
    
    /**
     * Sets the search parameters for the search.
     *
     * @param aKeywords - Keywords for the search.
     * @param aOperator - Determines the operation for the search(AND, OR, ...)
     * @param aCaseSensitive - Flag indicating case sensitivity
     */
     IMPORT_C void SetParametersL( const MDesCArray& aKeywords,
                   CSearchTextSearcher::TSearchCriteriaOperator aOperator, 
                   TBool aCaseSensitive );
								                        
    /**
     * Sets the search parameters for the search.
     *
     * @param	aCondition The condition of the search.
     */
     IMPORT_C void SetParametersL( const CSearchCondition& aCondition );
                                  
    /**
     * Starts the text search with the previous supplied parameters 
     * set by SetParametersL. 16 bit version.
     *
     * @param aToBeSearchedDes - 16 bit descriptor to be searched.
     */
    IMPORT_C void SearchL( const TDesC16& aToBeSearchedDes );
		
    /**
     * Starts the text search with the previous supplied parameters 
     * set by SetParametersL(). 8 bit version.
     *
     * @param aToBeSearchedDes - 8 bit descriptor to be searched.
     */
     IMPORT_C void SearchL( const TDesC8& aToBeSearchedDes );
     
     /**
     * Sets the flag for email search
     *
     * @param	aSearchEmail 
     */
     IMPORT_C void SearchEmailAddress( TBool aSearchEmail );
     
     /**
     * Return the search keywords
     *
     * @param   aSearchEmail 
     */
     IMPORT_C CDesC16ArrayFlat* SearchKeyword();
private:

    /** 
     * Performs the first phase of two phase construction.
     *
     * @param	aObserver	Text searcher's observer
     */
     CSearchTextSearcher( MSearchTextSearcherObserver& aObserver );
    
    /**
     * Performs the second phase construction.
     */
     void ConstructL();

    /**
     * Compares the to be searched string with the list of keywords. 
     *  16 bit version.
     *
     * @param aToBeSearchedDes - Text to be searched.
     * @param aKeywords - Array of keywords to be matched.
     * @param aKeywordSearchStatusArray Array storing the search 
     *        result for each individual search keyword
     * @param aOperator - Determines the operation for the search
     *                     (AND, OR, ...)
     * @param aCaseSensitive - Flag indicating case sensitivity
     */
     TBool DoStringCompareWithKeywordsL( const TDesC& aToBeSearchedDes,
                const CDesC16Array& aKeywords, 
                RArray<TKeywordSearchStatus>& aKeywordSearchStatusArray,
                TSearchCriteriaOperator aOperator, 
                TBool aCaseSensitive );

    /**
     * Compares the to be searched string with the list of keywords. 
     * 8 bit version.
     *
     *
     * @param aToBeSearchedDes -      Text to be searched.
     * @param aKeywords - Array of keywords to be matched.
     * @param aKeywordSearchStatusArray - Array storing the search result for 
     *					       				each individual search keyword
     * @param aOperator - Determines the operation for the search
     *		 			    (AND, OR, ...)
     * @param aCaseSensitive -  Flag indicating case sensitivity
     */
     TBool DoStringCompareWithKeywordsL( const TDesC8& aToBeSearchedDes,
                   const CDesC8Array& aKeywords, 
                   RArray<TKeywordSearchStatus>& aKeywordSearchStatusArray,
                   TSearchCriteriaOperator aOperator, 
                   TBool aCaseSensitive );
                                       
    
    /**
     * Sets data from singlecondition class
     *
     * @param aCondition - single condition info
     */
     void  SetSingleConditionDataL( 
              const CSearchSingleCondition& aCondition );   
    
    /**
     * Sets data from singlecondition class
     *
     * @param aKeywords	- Array of keywords to be matched.
     */
     void  DoUftTo8BitConversionL( const MDesC16Array&  aKeywords );  
    
    /**
     * Sets if boolean condition  
     */
     void  SetCriteriaOperator( );        
    
    /**
     * This method based on keyword status HitL() observer method will be  
     * called, this method is only called if condition type is of boolean
     *
     * @param aKeywordSearchStatusArray - keyword status array
     */
    void CheckForBooleanClausesL( RArray<TKeywordSearchStatus>& 
                                  aKeywordSearchStatusArray );                      
	
private:
    
    
    /**
     * Searchalgorith to use for searching
     * Own
     */
     CSearchAlgorithm* iSearchAlgorithm;
    
    /**
     * A file session for the unicode converter
     * Own
     */
     RArray<CSearchBooleanCondition::TBooleanClause>  iClause;   
	
   
    /**
     * Text searcher's observer
     */
     MSearchTextSearcherObserver& iObserver;

    
    /**
     * array that contains 16 bit keywords to search
     * Not owned
     */
     const CDesC16Array* iSearchKeywords;		
    
    
    /**
     * array that contains 8 bit versions from keywords to search
     * These are copied from the 16bit version when search is done 
     * in text that is available in 8 bit format
     *
     * Own
     */
     CDesC8ArrayFlat* iSearchKeywords8;	
    
    
    /**
     * array that contains 16 bit versions from keywords to search
     *
     * Own
     */
     CDesC16ArrayFlat* iSearchKeywords16; 	
    
    /**
     * Determines the operation for the search (AND, OR, ...)
     */
     TSearchCriteriaOperator iOperator;
    
    /**
     * Flag indicating case sensitivity
     */
     TBool iCaseSensitive;
    
    /**
     * Flag indicating whether search parameters where set.
     */
     TBool iHaveParameters;
    
    /**
     * Array storing the search result for each individual search keyword
     */
     RArray<TKeywordSearchStatus> iKeywordSearchStatusArray;
    
    /**
     * Character position of the 1st keyword match
     */
     TBool iIsBooleanCondition;
    
    /**
     * Keywords sount
     */
     TInt iCount;
    
    /**
     * To check wheather all the conditions are of same type
     */
     TBool iSameCondition;
    
    /**
     * Type of boolean clause ( EBooleanMust , EBooleanShould or 
     * EBooleanMustNot )
     */
     CSearchBooleanCondition::TBooleanClause  iBooleanClause;
    
    /**
     * Type of boolean clause 
     */
     CSearchBooleanCondition::TBooleanClause  iIntermediateClause;

    };
    
#endif  //C_SEARCHTEXTSEARCHER_H       
    
    
    
