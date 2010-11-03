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




#include <utf.h>
#include "searchalgorithm.h"


const  TInt  KPeriod( 0x002E ); 
const  TInt  KEt( 64 ); 
const  TInt  KArrayGranularity( 2 ); 
const  TInt  KSpace(0x0020);
const  TInt  KSeparator(0x2029);



// -----------------------------------------------------------------------------
//  CSearchAlgorithm::NewL()
//  Symbian C++ 2 phased contruction
//  Returns Instance of CSearchAlgorithm.
// -----------------------------------------------------------------------------
//      
CSearchAlgorithm*  CSearchAlgorithm::NewL()
    {
    CSearchAlgorithm* self = new( ELeave ) CSearchAlgorithm();
    CleanupStack::PushL( self );
    ( *self ).ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CTextSearcher::~CTextSearcher()
// C++ destructor
// -----------------------------------------------------------------------------
//
CSearchAlgorithm:: ~CSearchAlgorithm()
    {
    if ( iSearchKeywords8Bit )
        {
        delete iSearchKeywords8Bit;
        iSearchKeywords8Bit = NULL;
        }
    }
    
// -----------------------------------------------------------------------------
// CSearchAlgorithm::SearchL(..)
// Initializes the search params to the class datamemebers and does search 
// -----------------------------------------------------------------------------
//      
void CSearchAlgorithm::SearchL( const TDesC& aToBeSearchedDesIn, 
               const CDesCArray& aKeywordsIn, 
               RArray<TKeywordSearchStatus>& aKeywordSearchStatusArrayOut,
               CSearchTextSearcher::TSearchCriteriaOperator aOperatorIn, 
               TBool aCaseSensitiveIn
               )
               
    {  
    iToBeSearchedDes = &aToBeSearchedDesIn;
    
    iSearchKeywords = &aKeywordsIn;
    iKeywordSearchStatusArray = &aKeywordSearchStatusArrayOut;
    
    
    iOperator = aOperatorIn;
    iCaseSensitive = aCaseSensitiveIn;
    
    iStartOfPreviousWord = KErrNotFound;
    startOfWord          = KErrNotFound;
    
    iCharPosition = 0;   // start searching in the beginning of the text
    
    DoSearchL();
    }

// -----------------------------------------------------------------------------
// CSearchAlgorithm::SearchL()
// Executes the search. 8 bit.
// -----------------------------------------------------------------------------
//
void CSearchAlgorithm::SearchL(const TDesC8& aToBeSearchedDesIn, 
              const CDesC8Array& aKeywordsIn,
              RArray<TKeywordSearchStatus>& aKeywordSearchStatusArrayOut,
              CSearchTextSearcher::TSearchCriteriaOperator aOperatorIn,
              TBool aCaseSensitiveIn )   
    {   
    
    iToBeSearchedDes8Bit = &aToBeSearchedDesIn;
    iSearchKeywords8Bit = &aKeywordsIn;
    iKeywordSearchStatusArray = &aKeywordSearchStatusArrayOut;

    iOperator = aOperatorIn;
    iCaseSensitive = aCaseSensitiveIn;
    iCharPosition = 0;   // start searching in the beginning of the text

    DoSearch8L();
    
    }
 
 
// -----------------------------------------------------------------------------
// CSearchAlgorithm::SearchEmailAddress()
// Sets whether . and @ are to be used as word separators.
// -----------------------------------------------------------------------------
//
void  CSearchAlgorithm::SearchEmailAddress( TBool aSearchEmail )
    {
    iSearchEmailAddress = aSearchEmail;
    }

// -----------------------------------------------------------------------------
// CTextSearcher::CTextSearcher()
// C++ paramatized constructor
// -----------------------------------------------------------------------------
//
CSearchAlgorithm::CSearchAlgorithm() 
:   iSingleWord( NULL, 0 ) 
    {
        
    }
    
void CSearchAlgorithm::ConstructL()
    {
    iSearchKeywords8Bit = new ( ELeave ) CDesC8ArrayFlat( KArrayGranularity );
    }


// -----------------------------------------------------------------------------
// CSearchAlgorithm::DoSearchL(..)
// Does actual search
// -----------------------------------------------------------------------------
//      

void CSearchAlgorithm::DoSearchL()
    {
    TBool keywordsFound ( EFalse );
    //TInt startOfWord;
    TInt endOfWord;
    
    if ( MoveToStartOfWord( *iToBeSearchedDes, iCharPosition ) )
        {
        startOfWord = iCharPosition;
        iStartOfPreviousWord = iCharPosition; // There is no previous 
                   							  // word yet since this is the 
        MoveToEndOfCurrentWordPlusOne( *iToBeSearchedDes, iCharPosition );
        endOfWord = iCharPosition-1;
        
        iSingleWord.Set( iToBeSearchedDes->Mid( startOfWord, 
        	endOfWord-startOfWord+1 ) );
        //( *iWordCounter )++;
        keywordsFound = CompareSingleWordWithKeywords();
        
        while ( !keywordsFound )
            {
            if ( MoveToStartOfNextWord( *iToBeSearchedDes, iCharPosition ) )
                {
                startOfWord = iCharPosition;
                MoveToEndOfCurrentWordPlusOne( *iToBeSearchedDes, 
                	iCharPosition );
                endOfWord = iCharPosition-1;
                
                iSingleWord.Set( iToBeSearchedDes->Mid( startOfWord, 
                	endOfWord-startOfWord+1 ) );
                //( *iWordCounter )++;
                }
            else
                {
                break;  // no more words to be searched
                }
            keywordsFound = CompareSingleWordWithKeywords();
            iStartOfPreviousWord = startOfWord;	// remember the previous word 
            		//position for snippet
            }
        }
    }  
      
// -----------------------------------------------------------------------------
// CSearchAlgorithm::MoveToStartOfWord()
// Moves to the start of next word.
// Returns  EFalse if no more words are available.
// -----------------------------------------------------------------------------
//
TBool CSearchAlgorithm::MoveToStartOfWord( const TDesC& aTextIn, 
                                            TInt& iCharPositionOut )
    {
        // iCharPositionOut may point to a AlphaDigit or a non AlphaDigit.
    TInt length ( aTextIn.Length() );
    if ( iCharPositionOut > length-1 )
        {
        iCharPositionOut = KErrNotFound;     // No more words available
        return EFalse;
        }
    //  iCharPositionOut may already point to the start of a word
    TChar character ( aTextIn[iCharPositionOut] );
    if(!( (character == KSpace) || (character == KSeparator)))
       return ETrue;
    
    // move until a AlphaDigit is found
    for ( ;; )
        {
        iCharPositionOut++;
        if ( iCharPositionOut > length-1 )
            {
            iCharPositionOut = KErrNotFound;     // No more words available
            return EFalse;
            }
        
        TChar character ( aTextIn[iCharPositionOut] );           
        if(!( (character == KSpace) || (character == KSeparator)))
              return ETrue;

        }
    }
    
 // -----------------------------------------------------------------------------
// CSearchAlgorithm::MoveToStartOfNextWord()
// Moves to the start of the word.
// -----------------------------------------------------------------------------
//
TBool CSearchAlgorithm::MoveToStartOfNextWord( const TDesC& aTextIn, 
                               TInt& iCharPositionOut )
    {
    // assume that iCharPositionOut is pointing to a non AlphaDigit character.
    TInt length ( aTextIn.Length() );
    
    for ( ;; )// forever
        {
        iCharPositionOut++;
        if ( iCharPositionOut > length-1 )
            {
            iCharPositionOut = KErrNotFound;     // No more words available
            return EFalse;
            }
        
        TChar character ( aTextIn[iCharPositionOut] );
        if(!( (character == KSpace) || (character == KSeparator)))
            return ETrue;
        
        }
    }
    
// -----------------------------------------------------------------------------
// CSearchAlgorithm::MoveToEndOfCurrentWordPlusOne()
// Moves to the end of the word.
// -----------------------------------------------------------------------------
//
void CSearchAlgorithm::MoveToEndOfCurrentWordPlusOne( const TDesC& aTextIn, 
                                        TInt& iCharPositionOut )
    {
    // assume that iCharPositionOut is pointing to a AlphaDigit character.
    TInt length ( aTextIn.Length() );
        
    FOREVER 
        {
        iCharPositionOut++;
        if ( iCharPositionOut > length-1 )
            {
            return;  
            }
        TChar character ( aTextIn[iCharPositionOut] );
        if( (character == KSpace) || (character == KSeparator))
            return ;
        }
    }

// -----------------------------------------------------------------------------
// CSearchAlgorithm::CompareSingleWordWithKeywords()
// Compares single word with keywords in the array
//  Returns ETrue if a hit has been found and the sarhing can end.
// -----------------------------------------------------------------------------
//                                        
TBool CSearchAlgorithm::CompareSingleWordWithKeywords()
    {
    // Compare single word with keywords (only those keywords that are 
    // not yet found are compared)
    for ( TInt i=0; i<iSearchKeywords->Count() &&
    	 i < iKeywordSearchStatusArray->Count() ; i++ )
        {
        if ( !( *iKeywordSearchStatusArray )[i].iFound )    // keyword not yet
            {
            TInt result;
            if ( iCaseSensitive )
                {               
                result = iSingleWord.Match( ( *iSearchKeywords )[i] );
                }
            else
                {
                result = iSingleWord.MatchF( ( *iSearchKeywords )[i] );
                }

            if ( result != KErrNotFound )
                {
                ( *iKeywordSearchStatusArray )[i].iFound = ETrue;
                ( *iKeywordSearchStatusArray )[i].iFoundAtNthPosition 
                	= startOfWord + result;
                break;
                }

            }
        }
    // Check if there is still need to continue searching.
    TBool keywordsFound;
    switch ( iOperator )
        {
        case CSearchTextSearcher::ESearchCriteriaAND:
            keywordsFound = ETrue;
            break;

        case CSearchTextSearcher::ESearchCriteriaOR:
            keywordsFound = EFalse;
            break;

        default:
            keywordsFound = ETrue;  // to avoid warning
            break;
        }
    
    for ( TInt j=0; j<iSearchKeywords->Count(); j++ )
        {
        switch ( iOperator )
            {
            case CSearchTextSearcher::ESearchCriteriaAND:
                {
                if ( !( *iKeywordSearchStatusArray )[j].iFound )
                    {
                    return EFalse;
                    }
                }
                break;
    
            case CSearchTextSearcher::ESearchCriteriaOR:
                {
                if ( ( *iKeywordSearchStatusArray )[j].iFound )
                    {
                    return ETrue;
                    }
                }
                break;
            }
        }
    return keywordsFound;
    }





// -----------------------------------------------------------------------------
// CSearchAlgorithm::DoSearch8()
// Does the searching. 8 bit.
// -----------------------------------------------------------------------------
//
void CSearchAlgorithm::DoSearch8L()
    {
    TBool keywordsFound ( EFalse );
    TInt endOfWord   = 0;
    
    if ( MoveToStartOfWord8( *iToBeSearchedDes8Bit, iCharPosition ) )
        {
        startOfWord = iCharPosition;
        iStartOfPreviousWord = iCharPosition;// There is no previous 
        //  word yet since this is the
        //  1st word in the search text
        MoveToEndOfCurrentWordPlusOne8( *iToBeSearchedDes8Bit, iCharPosition );
        endOfWord = iCharPosition-1;
        
        iSingleWord8Bit.Set( iToBeSearchedDes8Bit->Mid(startOfWord,
        			endOfWord-startOfWord+1 ) );
        //( *iWordCounter )++;
        keywordsFound = CompareSingleWordWithKeywords8();
        
        while ( !keywordsFound )
            {
            if ( MoveToStartOfNextWord8( *iToBeSearchedDes8Bit, iCharPosition ) )
                {
                startOfWord = iCharPosition;
                MoveToEndOfCurrentWordPlusOne8( *iToBeSearchedDes8Bit, 
                	iCharPosition );
                endOfWord = iCharPosition-1;
                iSingleWord8Bit.Set(iToBeSearchedDes8Bit->Mid( startOfWord, 
                	endOfWord-startOfWord+1));
                //( *iWordCounter )++;
                }
            else
                {
                break;  // no more words to be searched
                }
            keywordsFound = CompareSingleWordWithKeywords8();
            iStartOfPreviousWord = startOfWord;
            }
        }
    }
// -----------------------------------------------------------------------------
// CSearchAlgorithm::MoveToStartOfWord8()
// Moves to the start of next word. 8 bit.
// Returns  EFalse if no more words are available.
// -----------------------------------------------------------------------------
//
TBool CSearchAlgorithm::MoveToStartOfWord8( const TDesC8& aTextIn,
                                            TInt& iCharPositionOut )
    {
    // iCharPositionOut may point to a AlphaDigit or a non AlphaDigit.
    
    TInt length ( aTextIn.Length() );
    
    if ( iCharPositionOut > length-1 )
        {
        iCharPositionOut = KErrNotFound;     // No more words available
        return EFalse;
        }
    
    //  iCharPositionOut may already point to the start of a word
    TChar character ( aTextIn[iCharPositionOut] );
    if(!( (character == KSpace) || (character == KSeparator)))
       return ETrue;

    // move until a AlphaDigit is found
    FOREVER // forever
    {
        iCharPositionOut++;
        if ( iCharPositionOut > length-1 )
            {
            iCharPositionOut = KErrNotFound;     // No more words available
            return EFalse;
            }
        
        TChar character ( aTextIn[iCharPositionOut] );
        if(!( (character == KSpace) || (character == KSeparator)))
            return ETrue;

        }
    }
    
// -----------------------------------------------------------------------------
// CSearchAlgorithm::MoveToStartOfNextWord8()
// Moves to the start of next word 8 bit varient.
// -----------------------------------------------------------------------------
//  

TBool CSearchAlgorithm::MoveToStartOfNextWord8( const TDesC8& aTextIn, 
                                                TInt& iCharPositionOut )
    {
    // assume that iCharPositionOut is pointing to a non AlphaDigit character.
    TInt length ( aTextIn.Length() );
    for ( ;; )// forever
        {
        iCharPositionOut++;
        if ( iCharPositionOut > length-1 )
            {
            iCharPositionOut = KErrNotFound;     // No more words available
            return EFalse;
            }
        
        TChar character ( aTextIn[iCharPositionOut] );
        if(!( (character == KSpace) || (character == KSeparator)))
             return ETrue;

        }
    }
// -----------------------------------------------------------------------------
// CSearchAlgorithm::MoveToEndOfCurrentWordPlusOne8()
// Moves to the end of the word 8 bit varient.
// -----------------------------------------------------------------------------
//  
void CSearchAlgorithm::MoveToEndOfCurrentWordPlusOne8( const TDesC8& aTextIn, 
                                                      TInt& iCharPositionOut )
    {
    // assume that iCharPositionOut is pointing to a AlphaDigit character.
    TInt length ( aTextIn.Length() );
    for ( ;; )  // forever
        {
        iCharPositionOut++;
        if ( iCharPositionOut > length-1 )
            {
            return;
            }
        
        TChar character ( aTextIn[iCharPositionOut] );
        if( (character == KSpace) || (character == KSeparator))
           return ;

        }
    }
// -----------------------------------------------------------------------------
// CSearchAlgorithm::CompareSingleWordWithKeywords8()
// Compares current word with given keywords. 8 bit.
// Returns ETrue if a hit has been found and the sarhing can end.
// -----------------------------------------------------------------------------
//
TBool CSearchAlgorithm::CompareSingleWordWithKeywords8()
    {
    // Compare single word with keywords (only those keywords that 
    // are not yet found are compared)
    for ( TInt i=0; i<iSearchKeywords8Bit->Count() && 
    	i < iKeywordSearchStatusArray->Count() ; i++ )
        {
        if ( !( *iKeywordSearchStatusArray )[i].iFound )    // keyword not yet
            { 
            TBool result;
            if ( iCaseSensitive )
                {                                               
                result = iSingleWord8Bit.Match( (*iSearchKeywords8Bit)[i] );                
                }
            else
                {
                result = iSingleWord8Bit.MatchF( (*iSearchKeywords8Bit)[i] );
                }
            
            if ( result != KErrNotFound )
                {
                ( *iKeywordSearchStatusArray )[i].iFound = ETrue;
                ( *iKeywordSearchStatusArray )[i].iFoundAtNthPosition = 
                startOfWord;
                break;
                }
            }
        }
    // Check if there is still need to continue searching.
    TBool keywordsFound;
    switch ( iOperator )
        {
        case CSearchTextSearcher::ESearchCriteriaAND:
            keywordsFound = ETrue;
            break;
        
        case CSearchTextSearcher::ESearchCriteriaOR:
            keywordsFound = EFalse;
            break;
        
        default:
            keywordsFound = ETrue;  // to avoid warning
            break;
        }
    
    for ( TInt j=0; j<iSearchKeywords8Bit->Count(); j++ )
        {
        switch ( iOperator )
            {
            case CSearchTextSearcher::ESearchCriteriaAND:
                {
                if ( !( *iKeywordSearchStatusArray )[j].iFound )
                    {
                    return EFalse;
                    }
                }
            break;
            
            case CSearchTextSearcher::ESearchCriteriaOR:
            {
                if ( ( *iKeywordSearchStatusArray )[j].iFound )
                {
				return ETrue;
                }
            }
            break;
            }
        }
    return keywordsFound;
    }



