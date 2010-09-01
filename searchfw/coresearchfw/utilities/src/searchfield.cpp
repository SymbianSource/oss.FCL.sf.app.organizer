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
* Description:   Class representing a search field
*
*/



#include <e32cmn.h>
#include <e32def.h>

#include <searchfield.h>

//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchField* CSearchField::NewL() 
    {
    CSearchField* self = new ( ELeave ) CSearchField;
    return self;
    }
	
//------------------------------------------------------------------------
// 1st phase constructor
//------------------------------------------------------------------------
//
EXPORT_C CSearchField* CSearchField::NewL( RReadStream& aStream )
    {
    CSearchField* self = new ( ELeave ) CSearchField();
    CleanupStack::PushL( self );
    self->ConstructL( aStream );
    CleanupStack::Pop( self );
    return self;
    }
	
//------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------
//
CSearchField::~CSearchField() 
    {
    if ( iField ) 
        {
        delete iField;
        iField = NULL;
        }
    if ( iCustomFieldId ) 
        {
        delete iCustomFieldId;
        iCustomFieldId = NULL;
        }
    if ( iBuf16Data ) 
        {
        delete iBuf16Data;
        iBuf16Data = NULL;
        }
    if ( iBuf8Data ) 
        {
        delete iBuf8Data;
        iBuf8Data = NULL;
        }
    }

//------------------------------------------------------------------------
// Externalizes the object to a stream
//------------------------------------------------------------------------
//
EXPORT_C void CSearchField::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( iType );
    if ( iField ) 
        {
        TUint32 size = iField->Size();
        aStream.WriteUint32L( size );
        aStream << *iField;
        }
    else 
        {
        aStream.WriteUint32L( 0 );
        }
    aStream.WriteUint32L( iFieldId );
		
    if ( iBuf8Data ) 
        {
        TUint32 size = iBuf8Data->Size();
        aStream.WriteUint32L( size );
        aStream << *iBuf8Data;
        }
    else 
        {
        aStream.WriteUint32L( 0 );
        }
    }

//------------------------------------------------------------------------
// Sets fieldId
//------------------------------------------------------------------------
//
EXPORT_C void CSearchField::SetFieldId( TSearchFieldId aFieldId )
    {
    iFieldId = aFieldId;
    }

//------------------------------------------------------------------------
// gets fieldId
//------------------------------------------------------------------------
//
EXPORT_C TSearchFieldId CSearchField::FieldId() const 
    {
    return iFieldId;
    }

//------------------------------------------------------------------------
// Gets DataType
//------------------------------------------------------------------------
//
EXPORT_C TSearchFieldDataType CSearchField::DataType( ) const
    {
    return iType;
    }

//------------------------------------------------------------------------
// Sets data
//------------------------------------------------------------------------
//
EXPORT_C void CSearchField::SetDataL( TDesC8& aData, TSearchFieldDataType aDataType )
    {
    iType = aDataType;
    if ( iBuf8Data ) 
        {
        delete iBuf8Data;
        iBuf8Data = NULL;
        }
    iBuf8Data = aData.AllocL();
    }


//------------------------------------------------------------------------
// Gets Data for Int
//------------------------------------------------------------------------
//
EXPORT_C TInt CSearchField::GetData( TInt& aData ) const
    {
    if ( iType != ESearchFieldDataTypeTInt ) 
        {
        return KErrNotSupported;
        }
    TLex8 lex( *iBuf8Data );
    lex.Val( aData );
    return KErrNone;
    }

//------------------------------------------------------------------------
// Gets data for TReal
//------------------------------------------------------------------------
//
EXPORT_C TInt CSearchField::GetData( TReal& aData ) const
    {
    if ( iType != ESearchFieldDataTypeTReal ) 
        {
        return KErrNotSupported;
        }
    TLex8 lex( *iBuf8Data );
    lex.Val( aData );
    return KErrNone;
    }

//------------------------------------------------------------------------
// Gets data for TTime
//------------------------------------------------------------------------
//
EXPORT_C TInt CSearchField::GetData( TTime& aData ) const
    {
    if ( iType != ESearchFieldDataTypeTTime ) 
        {
        return KErrNotSupported;
        }
    aData = iTimeData;
    return KErrNone;
    }
	
//------------------------------------------------------------------------
// gets data
//------------------------------------------------------------------------
//
EXPORT_C TInt CSearchField::GetData( TPtrC8& aData ) const
    {
    if ( iType != ESearchFieldDataTypeTDesC ) 
        {
        return KErrNotSupported;
        }
    aData.Set( *iBuf8Data );
    return KErrNone;
    }
	
//------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------
//
CSearchField::CSearchField()
    {
	
    }

//------------------------------------------------------------------------
// 2nd phase constructor
//------------------------------------------------------------------------
//
EXPORT_C void CSearchField::ConstructL( RReadStream& aStream )
    {
    TUint32 size = 0;
    iType = static_cast<TSearchFieldDataType> ( aStream.ReadUint32L() );
		
    size = aStream.ReadUint32L();
    if ( size ) 
        {
        iField = HBufC8::NewL( size );
        TPtr8 ptr = iField->Des();
        aStream >> ptr;	
        }
    iFieldId = static_cast<TSearchFieldId> ( aStream.ReadUint32L() );
    size = aStream.ReadUint32L();
    if ( size ) 
        {
        iBuf8Data = HBufC8::NewL( size );
        TPtr8 ptr = iBuf8Data->Des();
        aStream >> ptr;	
        }
    }


