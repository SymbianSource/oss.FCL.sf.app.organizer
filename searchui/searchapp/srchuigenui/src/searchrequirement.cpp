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
* Description:      CSearchRequirement class used to form the Search Query
*
*/






#include <stringpool.h>

#include "searchrequirement.h"
#include <searchfield.h>
#include <searchsinglecondition.h>
#include <searchbooleancondition.h>


//------------------------------------------------------------------------------
//  CSearchRequirement::NewL
//  Symbian OS Constructor
//-------------------------------------------------------------------------------
//
CSearchRequirement* CSearchRequirement::NewL( )
    {
    CSearchRequirement* self = new ( ELeave ) CSearchRequirement;
    return self;
    }


// -----------------------------------------------------------------------------
//  CSearchRequirement::NewL
//  Symbian OS Constructor
// -----------------------------------------------------------------------------
//
CSearchRequirement* CSearchRequirement::NewL( RReadStream* aStream )
    {
    CSearchRequirement* self = new ( ELeave ) CSearchRequirement;
    CleanupStack::PushL( self );
    //Call to the appropriate ConstructL
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;
    }

//------------------------------------------------------------------------------
//  CSearchRequirement::CSearchRequirement
//  Symbian Phase2 Constructor
//-------------------------------------------------------------------------------
//
void CSearchRequirement::ConstructL( RReadStream* aStream )
    {
    InternalizeL( aStream );
    }

//-----------------------------------------------------------------------------
// CSearchRequirement::CSearchRequirement
// Symbian OS Destructor
//-----------------------------------------------------------------------------
//
 CSearchRequirement::~CSearchRequirement()
    {
    if ( iSearchQuery )
        {
        delete( iSearchQuery );
        iSearchQuery = NULL;	
        }	
    }

//-----------------------------------------------------------------------------
//  CSearchRequirement::ExternalizeL
//  Externalise function
//  (other items were commented in a header).
//-----------------------------------------------------------------------------
//
void CSearchRequirement::ExternalizeL( RWriteStream& aStream ) const
    {
    if ( iSearchQuery )
        {
        iSearchQuery->ExternalizeL( aStream );
        }
    }


//-----------------------------------------------------------------------------
//  CSearchRequirement::InternalizeL
//  Internalise function
//  (other items were commented in a header).
//-----------------------------------------------------------------------------
//
void CSearchRequirement::InternalizeL( RReadStream* aStream )
    {
    //Call the internalize of Query class
    iSearchQuery = CSearchQuery::NewL( *aStream );
    }

//-----------------------------------------------------------------------------
//  CSearchRequirement::CreateRequest
//  Function forms the Search Query
//  (other items were commented in a header).
//-----------------------------------------------------------------------------
//
CSearchQuery*  CSearchRequirement::CreateRequestL( TDesC8 &aKeyword,
    									 TUid aServiceId, 
    									 RArray<TUid> aContentIdArray )
    									 
    {
    HBufC8* rightWord( NULL );
    rightWord = HBufC8::NewLC( aKeyword.Length() + 1 );
	
    _LIT8( kTempString," " );
		
	// if '|' character if found, all word become OR -words. Meaning that 
	// if any one of them is found, a hit is registered. 
	
    TBool isOperatorOR = EFalse;
    CSearchBooleanCondition* boolCondition( NULL );	
    boolCondition = CSearchBooleanCondition::NewL();
    rightWord->Des().Append( aKeyword );
    while ( rightWord->Des().Locate('|') != -1 ) 
        {
        isOperatorOR = ETrue; 
        rightWord->Des().Replace(rightWord->Des().Locate('|'), 1,
             kTempString );
        }
        
    rightWord->Des().TrimAll();
		
    TInt location = 0;
    while ( location != -1 ) // goes through all the words
        {	        	   
        HBufC8* leftWord( NULL );
        leftWord = HBufC8::NewLC( aKeyword.Length() + 1 );
    
        HBufC8* tempWord( NULL );
        tempWord= HBufC8::NewLC( aKeyword.Length() + 1 );
        location = rightWord->Des().Locate(' ');
        if ( location != KErrNotFound )
            {	            
            leftWord->Des().Copy( rightWord->Des().Left( location ) );	    
            tempWord->Des().Copy( rightWord->Des().Right(
				rightWord->Des().Length() - location ) );
            rightWord->Des().Zero();	
            rightWord->Des().Append( *tempWord );
            rightWord->Des().TrimAll();	   
            }
        else 
            {	
            leftWord->Des().Zero();	            
            leftWord->Des().Append( *rightWord );
            }
        _LIT(KStar, "*");
  	    	
        leftWord->Des().Append( KStar );
	    	
	    CSearchField* field = CSearchField::NewL();
	    field->SetDataL( *leftWord, ESearchFieldDataTypeTDesC );
    		
        //Create the condition by adding the term
	    CSearchSingleCondition *singleCondition = 
	    		CSearchSingleCondition::NewL( field );    	
    		
        //now add this condition into the Boolean condition
    	
        CSearchBooleanCondition::TBooleanClause aClause;
        if ( isOperatorOR )
            {
            //Or condition
            aClause = CSearchBooleanCondition::EBooleanShould;
            }
        else
            {
            //And Condition
            aClause = CSearchBooleanCondition::EBooleanMust;
            }
        boolCondition->AddConditionL( singleCondition, aClause );
        CleanupStack::PopAndDestroy( tempWord );
        CleanupStack::PopAndDestroy( leftWord );
        } // while
	 
     CleanupStack::PopAndDestroy( rightWord );
	
    //Make the query
    iSearchQuery = CSearchQuery::NewL();
			
    if ( iSearchQuery)
        {
        iSearchQuery->SetTargetContent( aContentIdArray );
        iSearchQuery->SetTargetService( aServiceId );
        iSearchQuery->SetCondition( boolCondition );
        }
    return( iSearchQuery );
	}


//----------------------------------------------------------------------------
//  CSearchRequirement::GetRequest
//  Returns the SearchQuery
//  (other items were commented in a header).
//-----------------------------------------------------------------------------
//
CSearchQuery*  CSearchRequirement::GetRequest()
    {
    return(iSearchQuery);
    }
