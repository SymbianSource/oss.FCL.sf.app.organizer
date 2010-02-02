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
* Description:   Class representing a search condition
*
*/



#include <e32def.h>
#include <e32cmn.h>
#include <s32strm.h>

#include <searchcondition.h>

//------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchCondition::~CSearchCondition() 
    {
    if ( iReservedExtension ) 
        {
        delete iReservedExtension;
        iReservedExtension = NULL;
        }
    }

//------------------------------------------------------------------------
// returns condition type
//------------------------------------------------------------------------
//
EXPORT_C CSearchCondition::TConditionType CSearchCondition::Type() const 
    {
    return iType;
    }

//------------------------------------------------------------------------
// Externalize function
//------------------------------------------------------------------------
//
EXPORT_C void CSearchCondition::ExternalizeL( RWriteStream& aStream ) const 
    {
    aStream.WriteUint32L( iType );
    }

//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
//
CSearchCondition::CSearchCondition( const TConditionType& aType ) 
    {
    iType = aType;
    }

