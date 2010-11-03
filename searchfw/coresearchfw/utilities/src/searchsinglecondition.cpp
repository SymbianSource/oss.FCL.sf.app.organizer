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
* Description:   Class for search condition
*
*/




#include <e32def.h>
#include <s32strm.h>
#include <e32cmn.h>

#include <searchsinglecondition.h>
#include <searchfield.h>


//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchSingleCondition* CSearchSingleCondition::NewL(
			 CSearchField* aTerm ) 
    {
    CSearchSingleCondition* self = new ( ELeave )
                             CSearchSingleCondition( aTerm );				
    return self;
    }

//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchSingleCondition* CSearchSingleCondition::NewL( 
			RReadStream& aStream ) 
    {
    TConditionType type = static_cast<TConditionType>( 0 );
    type = static_cast<TConditionType> ( aStream.ReadUint32L() );
    CSearchSingleCondition* self = new ( ELeave ) 
                                 CSearchSingleCondition( type );
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;
    }
	
//------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------
//
CSearchSingleCondition::~CSearchSingleCondition() 
    {
    if ( iTerm )	
        {
        delete iTerm;
        iTerm = NULL;
        }
    }


//------------------------------------------------------------------------
// Extrenalizes the object to stream
//------------------------------------------------------------------------
//
EXPORT_C void CSearchSingleCondition::ExternalizeL( 
			RWriteStream& aStream ) const
    {
    CSearchCondition::ExternalizeL( aStream );
    if ( iTerm ) 
        {
        aStream.WriteUint32L( 1 );
        iTerm->ExternalizeL( aStream );
        }
    else 
        {
        aStream.WriteUint32L( 0 );
        }
    }

//------------------------------------------------------------------------
// Returns term of the condition
//------------------------------------------------------------------------
//
EXPORT_C const CSearchField& CSearchSingleCondition::Term() const
    {
    return *iTerm;	
    }

//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
//
CSearchSingleCondition::CSearchSingleCondition( 
	CSearchCondition::TConditionType type ) :
	 CSearchCondition( type )
    {
	
    }

//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
//
CSearchSingleCondition::CSearchSingleCondition( CSearchField* aTerm  ) : 
			CSearchCondition( CSearchCondition::EConditionTypeSimple )
    {
    iTerm = aTerm;
    }

//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
void CSearchSingleCondition::ConstructL( RReadStream& aStream ) 
    {
    TInt size = aStream.ReadUint32L();
    if ( size )  
        {
        iTerm = CSearchField::NewL( aStream );
        }
    }



