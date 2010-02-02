/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides methods for Attendee View's item list class.
*
*/



// INCLUDE FILES
#include "CAttendeeItemList.h"
#include "CAttendeeItem.h"
#include "AttendeeViewConsts.h"

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CAttendeeItemList::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAttendeeItemList* CAttendeeItemList::NewL()
    {
    CAttendeeItemList* self = new(ELeave)CAttendeeItemList;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self); // self
    return self;
    }

// ---------------------------------------------------------
// CAttendeeItemList::ConstructL
// ---------------------------------------------------------
//
void CAttendeeItemList::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CAttendeeItemList::CAttendeeItemList
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CAttendeeItemList::CAttendeeItemList() : iItems( KGranularity )
    {
    }

// ---------------------------------------------------------
// CAttendeeItemList::~CAttendeeItemList
// ---------------------------------------------------------
//
CAttendeeItemList::~CAttendeeItemList()
    {
	iItems.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CAttendeeItemList::ItemCount
// ---------------------------------------------------------
//
TInt CAttendeeItemList::ItemCount() const
	{
	return iItems.Count();
	}

// ---------------------------------------------------------
// CAttendeeItemList::ItemAt
// ---------------------------------------------------------
//
MAttendeeItem& CAttendeeItemList::ItemAt( const TInt aIndex ) const
	{
	return *( iItems[ aIndex ] );
	}

// ---------------------------------------------------------
// CAttendeeItemList::AppendL
// ---------------------------------------------------------
//
void CAttendeeItemList::AppendL( const CAttendeeItem* aItem )
	{        
    User::LeaveIfError( iItems.Append( aItem ) );
	}

// ---------------------------------------------------------
// CAttendeeItemList::InsertL
// ---------------------------------------------------------
//
void CAttendeeItemList::InsertL( const CAttendeeItem* aItem, 
                                 const TInt aPos )
	{
    User::LeaveIfError( iItems.Insert( aItem, aPos ) );
	}

// ---------------------------------------------------------
// CAttendeeItemList::Delete
// ---------------------------------------------------------
//
CAttendeeItem* CAttendeeItemList::Delete( const TInt aIndex )
	{
    CAttendeeItem* item = iItems[ aIndex ];
    iItems.Remove( aIndex );

    if ( item->Ownership() )        
        {        
        delete item;        
        item = NULL;
        }
    
	return item;
	}

// ---------------------------------------------------------
// CAttendeeItemList::Remove
// ---------------------------------------------------------
//
void CAttendeeItemList::Remove( const TInt aIndex )
    {
    iItems.Remove( aIndex );
    }

// ---------------------------------------------------------
// CAttendeeItemList::Count
// ---------------------------------------------------------
//
TInt CAttendeeItemList::Count() const
	{
	return iItems.Count();
	}

// ---------------------------------------------------------
// CAttendeeItemList::At
// ---------------------------------------------------------
//
CAttendeeItem* CAttendeeItemList::At( const TInt aIndex ) const
	{
	return iItems[ aIndex ];
	}

// ---------------------------------------------------------
// CAttendeeItemList::Sort
// ---------------------------------------------------------
//
void CAttendeeItemList::SortL()
    {
    iItems.Sort( TLinearOrder<CAttendeeItem>( CAttendeeItem::CompareL ) );
    }

// End of File
