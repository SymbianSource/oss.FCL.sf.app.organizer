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
* Description:   Class representing the search condition with clauses
*				 a file
*
*/



#include <e32def.h>
#include <e32cmn.h>
#include <searchsinglecondition.h>
#include <searchbooleancondition.h>

//------------------------------------------------------------------------
// two phased constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchBooleanCondition* CSearchBooleanCondition::NewL()
    {
    CSearchBooleanCondition* self = new ( ELeave ) CSearchBooleanCondition( 
	                  CSearchCondition::EConditionTypeBoolean);
    return self;
    }

//------------------------------------------------------------------------
// two phased constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchBooleanCondition* CSearchBooleanCondition::NewL( RReadStream& aStream )
    {
    TConditionType type = static_cast<TConditionType>( 0 );
    type = static_cast<TConditionType> ( aStream.ReadUint32L() );
		
    CSearchBooleanCondition* self = new ( ELeave ) CSearchBooleanCondition( type );
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;
    }

//------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------
//
CSearchBooleanCondition::~CSearchBooleanCondition()
    {
    iConditions.ResetAndDestroy();
    iClauses.Reset();
    }	

//------------------------------------------------------------------------
// Externalizes the CSearchBooleanCondition class to the stream
//------------------------------------------------------------------------
//
EXPORT_C void CSearchBooleanCondition::ExternalizeL( RWriteStream& aStream ) const
    {
    CSearchCondition::ExternalizeL( aStream );
    TInt count = iConditions.Count();
    CSearchCondition::TConditionType type = ( CSearchCondition::TConditionType )0;
    aStream.WriteUint32L( count );
    for ( TInt i = 0; i < count; i++ ) 
        {
        type = iConditions[i]->Type();
        aStream.WriteUint32L( ( TUint32 )type );
        iConditions[i]->ExternalizeL( aStream );
        }
	
    count = iClauses.Count();
    aStream.WriteUint32L( count );
	
    for ( TInt i = 0; i < iClauses.Count(); i++ ) 
        {
        aStream.WriteUint32L( ( TUint32 ) iClauses[i] );
        }
    }
	
//------------------------------------------------------------------------
// Adds a new condition and boolean clause to condition array.
//------------------------------------------------------------------------
//
EXPORT_C void CSearchBooleanCondition::AddConditionL( CSearchCondition* aCondition,
	     CSearchBooleanCondition::TBooleanClause aClause )
    {
    iConditions.Append( aCondition );
    iClauses.Append( aClause );
    }

//------------------------------------------------------------------------
// Gets the condition and the clause
//------------------------------------------------------------------------
//
EXPORT_C void CSearchBooleanCondition::GetConditions( 
			RPointerArray<CSearchCondition>& aConditions, 
    		RArray<CSearchBooleanCondition::TBooleanClause>& aClauses ) const
    {
    for ( TInt i = 0; i < iConditions.Count(); i++ )
        {
        aConditions.Append( iConditions[i] );
        }
    for ( TInt i = 0; i < iClauses.Count(); i++ ) 
        {
        aClauses.Append( iClauses[i] );
        }
    }

//------------------------------------------------------------------------
// Boolean condition constructor
//------------------------------------------------------------------------
//
CSearchBooleanCondition::CSearchBooleanCondition( CSearchCondition::TConditionType type ) : 
	CSearchCondition( type )
    {
	
    }

//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchBooleanCondition::ConstructL( RReadStream& aStream )
    {
    TInt count = aStream.ReadUint32L();//read count
    CSearchCondition::TConditionType type = ( CSearchCondition::TConditionType )0;

    CSearchCondition* tmpCondition( NULL );
    for (TInt i = 0; i < count; i++ ) 
        {
        type = ( CSearchCondition::TConditionType )aStream.ReadUint32L();
        switch ( type ) 
			{
            case EConditionTypeSimple:
                tmpCondition = CSearchSingleCondition::NewL( aStream );
                break;
				
            case EConditionTypeBoolean:
                tmpCondition = CSearchBooleanCondition::NewL( aStream );
                break;
			
            default:
                tmpCondition = NULL;
                break;
            }
        if ( tmpCondition ) 
            {
            iConditions.Append( tmpCondition );	
            }	
        }
	
        count = aStream.ReadUint32L();//read count
		
        for ( TInt i = 0; i < count; i++ ) 
            {
            CSearchBooleanCondition::TBooleanClause addr = 
                ( CSearchBooleanCondition::TBooleanClause )aStream.ReadUint32L();
             iClauses.Append( addr );
            }
    }






