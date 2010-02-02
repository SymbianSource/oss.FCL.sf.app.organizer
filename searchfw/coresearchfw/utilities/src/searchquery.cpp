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
* Description:   Class for search query
*
*/



#include <e32cmn.h>
#include <e32def.h>
#include <searchcondition.h>
#include <searchsinglecondition.h>
#include <searchbooleancondition.h>
#include <searchquery.h>

//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchQuery* CSearchQuery::NewL() 
    {
    CSearchQuery* self = new( ELeave ) CSearchQuery;
    return self;
    }

//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchQuery* CSearchQuery::NewL( RReadStream& aStream )
    {
    CSearchQuery* self = new( ELeave ) CSearchQuery;
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;
    }


//------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------
//
CSearchQuery::~CSearchQuery()
    {
    if ( iCondition )	 
        {
        delete iCondition;
        iCondition = NULL;
        }
    iContentIdArray.Close();
    }

//------------------------------------------------------------------------
// Extrenalizes the object
//------------------------------------------------------------------------
//
EXPORT_C void CSearchQuery::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( iServiceId.iUid );
    aStream.WriteUint32L( iContentIdArray.Count() );//Write size also
    for ( TInt i = 0; i < iContentIdArray.Count(); i++ )	
        {
        aStream.WriteUint32L( iContentIdArray[i].iUid );
        }
    if ( iCondition ) 
        {
        aStream.WriteUint32L( 1 );
        aStream.WriteUint32L( ( TUint32 )iCondition->Type() );
        iCondition->ExternalizeL( aStream );
        }
    else 
        {
        aStream.WriteUint32L( 0 );
        }
    }
	
//------------------------------------------------------------------------
// sets the condition
//------------------------------------------------------------------------
//
EXPORT_C void CSearchQuery::SetCondition( CSearchCondition* aCondition )
    {
    iCondition = aCondition;
    }
	
//------------------------------------------------------------------------
// returns condition
//------------------------------------------------------------------------
//
EXPORT_C const CSearchCondition& CSearchQuery::Condition() const
    {
    return *iCondition;	
    }

//------------------------------------------------------------------------
// copies into the contentidrray
//------------------------------------------------------------------------
//
EXPORT_C void CSearchQuery::SetTargetContent( const RArray<TUid>& aContentIdArray )
    {
    for ( TInt i = 0; i < aContentIdArray.Count(); i++ )	
        {
        iContentIdArray.Append( aContentIdArray[i] );
        }
    }

//------------------------------------------------------------------------
// returns the contentidrray
//------------------------------------------------------------------------
//
EXPORT_C void CSearchQuery::GetTargetContent( RArray<TUid>& aContentIdArray ) const
    {
    for ( TInt i = 0; i < iContentIdArray.Count(); i++ )	
        {
        aContentIdArray.Append( iContentIdArray[i] );
        }	
    }
	
//------------------------------------------------------------------------
// sets the service id
//------------------------------------------------------------------------
//
EXPORT_C void CSearchQuery::SetTargetService( TUid aServiceId )
    {
    iServiceId.iUid = aServiceId.iUid;		
    }

//------------------------------------------------------------------------
// returns the service id
//------------------------------------------------------------------------
//
EXPORT_C TUid CSearchQuery::TargetService() const
    {
    return iServiceId;
    }

//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
//
CSearchQuery::CSearchQuery() 
    {
	
    }

//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchQuery::ConstructL( RReadStream&  aStream ) 
    {
    iServiceId.iUid = aStream.ReadUint32L();
    TInt size = aStream.ReadUint32L();
    TUid tmpUid;
    tmpUid.iUid = 0;
    for ( TInt i = 0; i < size; i++ )	
        {
        tmpUid.iUid = aStream.ReadUint32L();
        iContentIdArray.Append( tmpUid );
        }
    size = aStream.ReadUint32L();
	
    if ( size ) 
        {
        CSearchCondition::TConditionType type = ( CSearchCondition::TConditionType )
            	aStream.ReadUint32L();
        switch ( type )	
            {
            case CSearchCondition::EConditionTypeSimple:
                iCondition = CSearchSingleCondition::NewL( aStream );
				break;
            case CSearchCondition::EConditionTypeBoolean:
                iCondition = CSearchBooleanCondition::NewL( aStream );
                break;
            default:
                iCondition = NULL;
                break;
            }
        }
    }

