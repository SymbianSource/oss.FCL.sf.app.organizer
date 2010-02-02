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
* Description:   Class handling the search of several keywords in a block of 
*                text.
*
*/



#include <utf.h>

#include <searchtextsearcherobserver.h>
#include <searchsinglecondition.h>
#include <searchcondition.h>
#include <searchfield.h>
#include <searchtextsearcher.h>
#include "searchkeywordstatus.h"
#include "searchalgorithm.h"

#include <featmgr.h>

_LIT(KColon,":");
const  TInt KNumberDataLength( 50 );
const  TInt KTimeDataLength( 50 );
const  TInt KArrayGranularity( 2 );
const  TInt KSingleKeywordInArray( 1 ); 


// -----------------------------------------------------------------------------
// CSearchTextSearcher::NewL()
// Symbian C++ 2  phase construction
// -----------------------------------------------------------------------------
//
EXPORT_C CSearchTextSearcher*  CSearchTextSearcher::NewL( 
		MSearchTextSearcherObserver& aObserver )
    {
    CSearchTextSearcher* self = CSearchTextSearcher::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSearchTextSearcher::NewLC()
// Symbian C++ 2  phase construction
// -----------------------------------------------------------------------------
//
EXPORT_C CSearchTextSearcher*  CSearchTextSearcher::NewLC( 
		MSearchTextSearcherObserver&  aObserver )
    {
    CSearchTextSearcher* self = new( ELeave ) CSearchTextSearcher( aObserver );
    CleanupStack::PushL( self );
    ( *self ).ConstructL();
    return self;
    }    
    
// -----------------------------------------------------------------------------
// CSearchTextSearcher::~CSearchTextSearcher()
// C++ destructor
// -----------------------------------------------------------------------------
//
CSearchTextSearcher::~CSearchTextSearcher()
    {
    iKeywordSearchStatusArray.Close();
    
    if ( iSearchKeywords8 )
        {
        iSearchKeywords8->Reset();
        delete iSearchKeywords8;
        iSearchKeywords8 = NULL;
        }
    
    if ( iHaveParameters != TRUE )
        {
        iSearchKeywords16->Reset();
        delete iSearchKeywords16;
        iSearchKeywords16 = NULL;
        }
  
    if ( iSearchAlgorithm )
        {
        delete iSearchAlgorithm;     
        iSearchAlgorithm = NULL;
        }
        iClause.Reset();
        iClause.Close();
        FeatureManager::UnInitializeLib();
    }
    
// -----------------------------------------------------------------------------
// CSearchTextSearcher::Cleanup()
// Cleanup Method
// -----------------------------------------------------------------------------
//
EXPORT_C void CSearchTextSearcher::Cleanup()
    {
    iKeywordSearchStatusArray.Reset();
    for ( TInt i = 0; i < iKeywordSearchStatusArray.Count() ; i++ )
        {
        iKeywordSearchStatusArray[i].iFound = EFalse;
        iKeywordSearchStatusArray[i].iFoundAtNthPosition = KErrNotFound;
        }
    }
// -----------------------------------------------------------------------------
// CSearchTextSearcher::SetParametersL()
// Sets the search parameters for the search.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSearchTextSearcher::SetParametersL( const MDesCArray& aKeywords,
              CSearchTextSearcher::TSearchCriteriaOperator 
              aOperator, TBool aCaseSensitive )
    {
    iSearchKeywords = static_cast<const CDesC16Array*>( &aKeywords );
    iOperator = aOperator;
    iCaseSensitive = aCaseSensitive;
      
    //convert from 16 - 8 bits for keyword
    iSearchKeywords8->Reset();
    iCount = iSearchKeywords->MdcaCount();
    
    DoUftTo8BitConversionL( *iSearchKeywords );    
    
    //initialize keyword status
    iKeywordSearchStatusArray.Reset();
    for ( TInt i = 0 ;i < iCount ; i++ )
        {
        TKeywordSearchStatus status;
        status.iFound  = EFalse;
        status.iFoundAtNthPosition = KErrNotFound;
        iKeywordSearchStatusArray.AppendL( status ); 
        }
    //Set it here to indicate client has called this method   
    iHaveParameters = ETrue; 
    if ( iCount == KSingleKeywordInArray) 
        {
          iIsBooleanCondition = EFalse;
        }
    else
        {
          iIsBooleanCondition = ETrue;
          //SetCriteriaOperator(); not to be called here , dont have clause yet          
        }    
    
    }

// -----------------------------------------------------------------------------
// CSearchTextSearcher::SetParametersL()
// Sets the search parameters based on conditions for the search.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSearchTextSearcher::SetParametersL( const CSearchCondition& 
                                                                aCondition )
    {
    if ( iHaveParameters )
        {
        return;
        }
    else
        {
        iSearchKeywords16 = new( ELeave ) CDesC16ArrayFlat( KArrayGranularity );     
        }
        
    switch( aCondition.Type() )
        {
        case CSearchCondition::EConditionTypeSimple:
            {
            iOperator = ESearchCriteriaAND; 
            iIsBooleanCondition = EFalse;
            SetSingleConditionDataL( 
            *(static_cast<const CSearchSingleCondition*>( &aCondition ) ) );
            }
            break;
        case CSearchCondition::EConditionTypeBoolean:
            {
            iOperator = ESearchCriteriaAND; 
            iIsBooleanCondition = ETrue;
            RPointerArray<CSearchCondition> conditions;
            (static_cast<const CSearchBooleanCondition*>( &aCondition ) )->
            GetConditions( conditions,iClause );
            
            for ( TInt i = 0; i < conditions.Count() ; i++ )
                {
                SetSingleConditionDataL( 
                *(static_cast<const CSearchSingleCondition*>( conditions[i] ) ) );
                }
            iCount = iSearchKeywords16->Count(); 
            conditions.Reset();
            conditions.Close();
            
            SetCriteriaOperator( );  
            }
            break;
            
        default:
            break;  
        }
      
    DoUftTo8BitConversionL( *iSearchKeywords16 );  
    }
    
// -----------------------------------------------------------------------------
// CSearchTextSearcher::SearchL()
// Starts the text search with the previous supplied parameters set 
// by SetParametersL. 16 bit version.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSearchTextSearcher::SearchL( const TDesC16& aToBeSearchedDes )
    {
    iKeywordSearchStatusArray.Reset();
	for ( TInt i = 0 ;i < iCount ; i++ )
	    {
		TKeywordSearchStatus status;
		status.iFound  = EFalse;
		status.iFoundAtNthPosition = KErrNotFound;
		iKeywordSearchStatusArray.AppendL( status ); 
	    }
    
    if ( iHaveParameters )
        {
         DoStringCompareWithKeywordsL( aToBeSearchedDes,*iSearchKeywords,
              iKeywordSearchStatusArray,iOperator,iCaseSensitive ); 
        }
    else
        {
        DoStringCompareWithKeywordsL( aToBeSearchedDes,*iSearchKeywords16,
              iKeywordSearchStatusArray,iOperator,iCaseSensitive );
        }
    }


// -----------------------------------------------------------------------------
// CSearchTextSearcher::SearchL()
// Starts the text search with the previous supplied parameters set by
// SetParametersL.  8 bit version.
// -----------------------------------------------------------------------------
//
EXPORT_C void CSearchTextSearcher::SearchL ( const TDesC8& aToBeSearchedDes )
    {
    DoStringCompareWithKeywordsL( aToBeSearchedDes,*iSearchKeywords8,
            iKeywordSearchStatusArray,iOperator,iCaseSensitive );
    }  

// -----------------------------------------------------------------------------
// Set whether searching for the email
// -----------------------------------------------------------------------------
//      
EXPORT_C void CSearchTextSearcher::SearchEmailAddress ( TBool aSearchEmail ) 
    {
	iSearchAlgorithm->SearchEmailAddress ( aSearchEmail );
    }

// -----------------------------------------------------------------------------
// Return the search keyword
// -----------------------------------------------------------------------------
//
EXPORT_C CDesC16ArrayFlat* CSearchTextSearcher::SearchKeyword()
    {
    return iSearchKeywords16; 	
    }

// -----------------------------------------------------------------------------
// CSearchTextSearcher::CSearchTextSearcher()
// C++ default constructor
// -----------------------------------------------------------------------------
//
CSearchTextSearcher::CSearchTextSearcher(
       MSearchTextSearcherObserver& aObserver )
:   iObserver( aObserver ) 
    {
    
    }
   
// -----------------------------------------------------------------------------
// CSearchTextSearcher::ConstructL()
// Symbian 2nd phase construction
// -----------------------------------------------------------------------------
//
void CSearchTextSearcher::ConstructL()
    {
    iSearchKeywords8 = new(ELeave) CDesC8ArrayFlat( KArrayGranularity ); 
    iSearchAlgorithm = CSearchAlgorithm::NewL();
    iSearchAlgorithm->SearchEmailAddress( ETrue );
    iHaveParameters = EFalse;
    iSameCondition  = EFalse;
    iIsBooleanCondition = EFalse;
    FeatureManager::InitializeLibL();
    }
    
// -----------------------------------------------------------------------------
// CSearchTextSearcher::DoStringCompareWithKeywords()
// Compares the to be searched string with the list of keywords, 16 bit version. 
// -----------------------------------------------------------------------------
//
TBool CSearchTextSearcher::DoStringCompareWithKeywordsL (
			 const TDesC& aToBeSearchedDes,
             const CDesC16Array& aKeywords, 
             RArray<TKeywordSearchStatus>& aKeywordSearchStatusArray,
             CSearchTextSearcher::TSearchCriteriaOperator aOperator,
             TBool aCaseSensitive )
    {
    iSearchAlgorithm->SearchL( aToBeSearchedDes,aKeywords,
                              aKeywordSearchStatusArray,
                              aOperator,
                              aCaseSensitive );
    if ( aKeywordSearchStatusArray.Count() >= 0 )
        {
        if ( iIsBooleanCondition )
            {
            CheckForBooleanClausesL( aKeywordSearchStatusArray );   
            }
        else
            {
            if ( !aKeywordSearchStatusArray[0].iFound )
                {
                iObserver.HitL( KErrNotFound );
                }
            else
                {
                iObserver.HitL( aKeywordSearchStatusArray[0].iFoundAtNthPosition );
                }   
            }
        }
      
   return TRUE;                           
   }
      
     
// -----------------------------------------------------------------------------
// CSearchTextSearcher::DoStringCompareWithKeywords()
// Compares the to be searched string with the list of keywords, 8 bit version. 
// -----------------------------------------------------------------------------
//
TBool CSearchTextSearcher::DoStringCompareWithKeywordsL (
          const TDesC8& aToBeSearchedDes,
          const CDesC8Array& aKeywords, 
          RArray<TKeywordSearchStatus>& aKeywordSearchStatusArray,
          CSearchTextSearcher::TSearchCriteriaOperator  aOperator,
          TBool aCaseSensitive )
    {
    iSearchAlgorithm->SearchL( aToBeSearchedDes,aKeywords,
             aKeywordSearchStatusArray,aOperator, aCaseSensitive );
    if ( aKeywordSearchStatusArray.Count() >= 0 )
        {
        if ( iIsBooleanCondition )
            {
            CheckForBooleanClausesL( aKeywordSearchStatusArray );   
            }
        else
            {
            if ( !aKeywordSearchStatusArray[0].iFound )
                {
                iObserver.HitL( KErrNotFound );
                }
            else
                {
                iObserver.HitL( aKeywordSearchStatusArray[0].iFoundAtNthPosition );
                }   
            }
        }
    return TRUE; 
    }
     
 // -----------------------------------------------------------------------------
// CSearchTextSearcher::SetSingleConditionDataL()
// Sets the singlecondition data for the search.
// -----------------------------------------------------------------------------
//    
void  CSearchTextSearcher::SetSingleConditionDataL ( const 
          CSearchSingleCondition& aSingleCondition )
    {
    const CSearchField& field = aSingleCondition.Term();
    switch ( field.DataType() )
        { 
        case ESearchFieldDataTypeUnknown:
            break;
        case ESearchFieldDataTypeTInt:
            {
            TInt intData;
            field.GetData( intData );
            HBufC* pbuf = HBufC::NewLC( KNumberDataLength );
            pbuf->Des().AppendNum( intData );
            iSearchKeywords16->AppendL( pbuf->Des() );
            CleanupStack::PopAndDestroy( pbuf );
            }
            break;
        case ESearchFieldDataTypeTReal:
            {  
            TReal realData;
            TRealFormat  realFormat( KRealFormatGeneral,KDefaultRealWidth );
            field.GetData( realData );
            HBufC* pbuf = HBufC::NewLC( KNumberDataLength );
            pbuf->Des().AppendNum( realData,realFormat );
            iSearchKeywords16->AppendL( pbuf->Des() );
            CleanupStack::PopAndDestroy( pbuf );
            }
            break;
        case ESearchFieldDataTypeTTime:
            {
            TTime timeData;
            field.GetData( timeData );
            TDateTime dateTime = timeData.DateTime();
                   
            HBufC* pbuf = HBufC::NewLC( KTimeDataLength) ;
                   
            //Set Time and inset into array
            pbuf->Des().AppendNum( dateTime.Hour() );
            pbuf->Des().Append( KColon );
            pbuf->Des().AppendNum( dateTime.Minute() );
            pbuf->Des().Append( KColon );
            pbuf->Des().AppendNum( dateTime.Second() );
            iSearchKeywords16->AppendL( pbuf->Des() );
            CleanupStack::PopAndDestroy( pbuf );
            }
            break;
        case ESearchFieldDataTypeTDesC:
            {
            TPtrC8  ptr;
            field.GetData( ptr );
            HBufC*  dataDes = HBufC::NewLC( ptr.Length() );
            TPtr  pdata = dataDes->Des();
            CnvUtfConverter::ConvertToUnicodeFromUtf8( pdata , ptr  );
            
            if ( FeatureManager::FeatureSupported( KFeatureIdJapanese ) ||
                 FeatureManager::FeatureSupported( KFeatureIdChinese )  ||
                 FeatureManager::FeatureSupported( KFeatureIdThai ) )
                {
                HBufC* tempBuffer = HBufC::NewLC( ptr.Length() + 1);
                TPtr pDataTemp = tempBuffer->Des();
                _LIT(KStar, "*");
                pDataTemp.Zero();
                pDataTemp.Append(KStar);
                pDataTemp.Append(pdata);
                            
                iSearchKeywords16->AppendL( pDataTemp );
                CleanupStack::PopAndDestroy( tempBuffer );
                }
            else
                {
                iSearchKeywords16->AppendL( pdata );
                }
            
            CleanupStack::PopAndDestroy( dataDes );
            }
            break;
        case ESearchFieldDataTypeCustom:
            break;
        default:
            break;
        }
    }


// -----------------------------------------------------------------------------
// CSearchTextSearcher::DoUftTo8BitConvertionL()
// 16 - 8 bit conversion 
// -----------------------------------------------------------------------------
//
void CSearchTextSearcher::DoUftTo8BitConversionL ( const MDesCArray& aKeywords )
    {
    for ( TInt i = 0 ;i < aKeywords.MdcaCount() ; i++ )
        {
        HBufC8* pBuf = HBufC8::NewLC( aKeywords.MdcaPoint( i ).Length() );
        TPtr8 ptr8 = pBuf->Des();
        CnvUtfConverter::ConvertFromUnicodeToUtf8( ptr8 ,aKeywords.MdcaPoint( i ) );
        iSearchKeywords8->AppendL( ptr8 );
        CleanupStack::PopAndDestroy( pBuf );
        }
    }


// -----------------------------------------------------------------------------
// CSearchTextSearcher::SetCriteriaOperator()
// -----------------------------------------------------------------------------
//
void CSearchTextSearcher::SetCriteriaOperator()
    {
    
    RArray<CSearchBooleanCondition::TBooleanClause>& clause = iClause;
    
    TInt condition = iBooleanClause = clause[0]; // take first clause out 
    
    TInt nextclause= 0; 
    for ( TInt i = 0 ;i < clause.Count() ; i++ )
        {
        if ( i>0 )
            { 
            nextclause = clause[i];
            condition ^=nextclause; //check if all are 
            				//the same clause category ( ANDs or ORs )
            if ( condition )
                {
                iIntermediateClause = clause[i];
                break;
                }
            }
        }
     if ( !condition ) // iSameCondition is initially set to FALSE 
        {
        iSameCondition = TRUE;
        }
   
    }    
     
// -----------------------------------------------------------------------------
// CSearchTextSearcher::CheckForBooleanClausesL()
// Compares the to be searched string with the list of keywords, 8 bit version. 
// -----------------------------------------------------------------------------
//
void CSearchTextSearcher::CheckForBooleanClausesL( RArray<TKeywordSearchStatus>& 
                                                            aKeywordSearchStatusArray )
    {
    
    TBool ifNoneFound = ETrue;
    TInt  recordFirstPos   = 0;   
    
    for ( TInt i = 0 ; i < iCount && i < aKeywordSearchStatusArray.Count() ; i++ )
        {
        if (  iBooleanClause == CSearchBooleanCondition::EBooleanMust
        		 && iSameCondition )
            {
            if ( !aKeywordSearchStatusArray[i].iFound )
                {
                ifNoneFound = ETrue;
                break;
                }
            else
                {
                ifNoneFound = EFalse;
                if ( !recordFirstPos )
                    {
                    recordFirstPos = 
                       aKeywordSearchStatusArray[i].iFoundAtNthPosition;	
                    }
                //iObserver.HitL( 
                //	aKeywordSearchStatusArray[i].iFoundAtNthPosition );    
                }
            }
        else if ( iBooleanClause == CSearchBooleanCondition::EBooleanShould 
        			&&  iSameCondition )
            {
            if ( !aKeywordSearchStatusArray[i].iFound )
                {
                //Do nothing ,just pass on
                }
            else
                {
                ifNoneFound = EFalse;
                iObserver.HitL(
                	 aKeywordSearchStatusArray[i].iFoundAtNthPosition );
                }    
            }
        else //if( !iSameCondition ) 
            {
            if ( !aKeywordSearchStatusArray[i].iFound )
                {
                if ( iBooleanClause  == CSearchBooleanCondition::EBooleanMust )
                    {
                    ifNoneFound = ETrue;
                    //break;  
                    }
                else if( iBooleanClause  == 
                         CSearchBooleanCondition::EBooleanShould 
                         && iIntermediateClause == 
                         CSearchBooleanCondition::EBooleanMust )
                    {
                    ifNoneFound = ETrue;
                    //break;     
                   } 
                }
            else 
                {
                ifNoneFound = EFalse;
                iObserver.HitL( aKeywordSearchStatusArray[i].iFoundAtNthPosition );
                }
            }
        }
    
    if ( !ifNoneFound && iBooleanClause == 
           CSearchBooleanCondition::EBooleanMust ) 
        {
        iObserver.HitL( recordFirstPos ); 
        }
    }
