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
* Description:  Provides methods for CAttendeeItem class.
*
*/



// INCLUDE FILES
#include "CAttendeeItem.h"
#include <caluser.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
_LIT( KPanicMsg, "CAttendeeItem" );

enum TPanicCode
    {
    KAttPanicNullPointer = 1,
    KAttPanicUnknownOperation
    };

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace
// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CAttendeeItem::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAttendeeItem* CAttendeeItem::NewL( CCalAttendee* aCalAttendee )
    {
    CAttendeeItem* self = NewLC( aCalAttendee );
    CleanupStack::Pop(self); // self
    return self;
    }

// ---------------------------------------------------------
// CAttendeeItem::NewLC
// ---------------------------------------------------------
//
CAttendeeItem* CAttendeeItem::NewLC( CCalAttendee* aCalAttendee )
    {
    CAttendeeItem* self = new(ELeave)CAttendeeItem( aCalAttendee );
    CleanupStack::PushL( self );    
    return self;
    }

// ----------------------------------------------------------------------------
// CAttendeeItem::CAttendeeItem
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CAttendeeItem::CAttendeeItem( CCalAttendee* aCalAttendee ) :                                  
                                 iContactID( KNullContactId ),
                                 iCalAttendee( aCalAttendee ),
                                 iOwnership( EFalse ),
                                 iOrganizerSatus( EFalse )
    {
    }

// ---------------------------------------------------------
// CAttendeeItem::~CAttendeeItem
// ---------------------------------------------------------
//
CAttendeeItem::~CAttendeeItem()
    {
    delete iContactTitle;   
    if ( iOwnership )
        {
        delete iCalAttendee;        
        }
    }

// ---------------------------------------------------------
// CAttendeeItem::ContactId
// ---------------------------------------------------------
//
TContactItemId CAttendeeItem::ContactId() const
    {
    return iContactID;
    }

// ---------------------------------------------------------
// CAttendeeItem::ContactTitle
// ---------------------------------------------------------
//
HBufC* CAttendeeItem::ContactTitle() const
    {
    return iContactTitle;
    }

// ---------------------------------------------------------
// CAttendeeItem::AgnAttendee
// ---------------------------------------------------------
//
CCalAttendee* CAttendeeItem::AgnAttendee() const
    {
    return iCalAttendee;
    }

// ---------------------------------------------------------
// CAttendeeItem::SetContactId
// ---------------------------------------------------------
//
void CAttendeeItem::SetContactId( TContactItemId aId )
    {
    iContactID = aId;
    }

// ---------------------------------------------------------
// CAttendeeItem::SetContactTitle
// ---------------------------------------------------------
//
void CAttendeeItem::SetContactTitle( HBufC* aTitle )
    {
    if ( iContactTitle )
        {
        delete iContactTitle;
        iContactTitle = NULL;
        }
    iContactTitle = aTitle;
    }

// ---------------------------------------------------------
// CAttendeeItem::SetOwnership
// ---------------------------------------------------------
//
void CAttendeeItem::SetOwnership( TBool aOwnership )
    {
    iOwnership = aOwnership;
    }

// ---------------------------------------------------------
// CAttendeeItem::Ownership
// ---------------------------------------------------------
//
TBool CAttendeeItem::Ownership() const
    {
    return iOwnership;
    }

// ---------------------------------------------------------
// CAttendeeItem::SetAgnAttendee
// ---------------------------------------------------------
//
void CAttendeeItem::SetAgnAttendee( CCalAttendee* aCalAttendee )
    {
    __ASSERT_DEBUG( aCalAttendee, Panic( KAttPanicNullPointer ) );
    if ( iOwnership && iCalAttendee )
        {
        delete iCalAttendee;
        iCalAttendee = NULL;
        }

    iCalAttendee = aCalAttendee;
    }

// ---------------------------------------------------------
// CAttendeeItem::SetAttendance
// ---------------------------------------------------------
//
void CAttendeeItem::SetAttendanceL( TAttendance aAttendance )
    {    
    __ASSERT_DEBUG( iCalAttendee, Panic( KAttPanicNullPointer ) );

    switch( aAttendance )
        {
        case ERequire:
            {
            iCalAttendee->SetRoleL( CCalAttendee::EReqParticipant );
            break;
            }
        case EOptional: 
            {
            iCalAttendee->SetRoleL( CCalAttendee::EOptParticipant );
            break;
            }
        /*case ENone:
            {
            iCalAttendee->SetRoleL( CCalAttendee::ENonParticipant );
            break;
            }               
        */            
        default:
            {            
            iCalAttendee->SetRoleL( CCalAttendee::EReqParticipant );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CAttendeeItem::Attendance
// ---------------------------------------------------------
//
CAttendeeItem::TAttendance CAttendeeItem::AttendanceL() const
    {
    __ASSERT_DEBUG( iCalAttendee, Panic( KAttPanicNullPointer ) );

    TAttendance attendance( ERequire );        

    switch( iCalAttendee->RoleL() )
        {
        case CCalAttendee::EChair:
            {
            attendance = ERequire;
            break;
            }
        case CCalAttendee::EReqParticipant:
            {
            attendance = ERequire;                
            break;
            } 
        case CCalAttendee::EOptParticipant:
            {
            attendance = EOptional;
            break;
            }
        /*case CCalAttendee::ENonParticipant:
            {
            attendance = ENone;
            break;
            }
        */    
        default:
            {   
            attendance = ERequire;                
            break;
            }
        }
    return attendance;
    }

// ---------------------------------------------------------
// CAttendeeItem::Compare
// ---------------------------------------------------------
//
TInt CAttendeeItem::CompareL( const CAttendeeItem& aLhs, 
                             const CAttendeeItem& aRhs )
    {
    TInt result( 0 );

    result = CompareAttendancesL( aLhs, aRhs );
    if ( result == 0 )
        {
        result = CompareStatusL( aLhs, aRhs );        
        }
    return result;
    }

// ---------------------------------------------------------
// CAttendeeItem::CompareAttendances
// ---------------------------------------------------------
//
TInt CAttendeeItem::CompareAttendancesL( const CAttendeeItem& aLhs, 
                                        const CAttendeeItem& aRhs )
    {
    __ASSERT_DEBUG( aLhs.iCalAttendee && aRhs.iCalAttendee, 
                    Panic( KAttPanicNullPointer ) );
    TInt result( 0 );       
        
    if (( aLhs.AttendanceL() == aRhs.AttendanceL()) && 
        (aRhs.IsOrganizer() == aLhs.IsOrganizer()))
        {
        result = 0;    
        }
    else if ( ( aLhs.IsOrganizer() ) ||
              ( aLhs.AttendanceL() == ERequire  && !aRhs.IsOrganizer() ))
        {
        result = -1;
        }
    else
        {
        result = 1;
        }
        
    return result;
    }

// ---------------------------------------------------------
// CAttendeeItem::CompareStatus
// ---------------------------------------------------------
//
TInt CAttendeeItem::CompareStatusL( const CAttendeeItem& aLhs, 
                                   const CAttendeeItem& aRhs )
    {
    __ASSERT_DEBUG( aLhs.iCalAttendee && aRhs.iCalAttendee, 
                    Panic( KAttPanicNullPointer ) );
    
    TInt result( 0 );

    if ( aLhs.iCalAttendee->StatusL() == aRhs.iCalAttendee->StatusL() ) 
        {
        result = 0;
        }
    else if ( ( aLhs.iCalAttendee->StatusL() == CCalAttendee::EAccepted ) ||           
              ( aLhs.iCalAttendee->StatusL() == CCalAttendee::ETentative && 
                aRhs.iCalAttendee->StatusL() != CCalAttendee::EAccepted  ) || 
              ( aLhs.iCalAttendee->StatusL() == CCalAttendee::EDeclined && 
                ( aRhs.iCalAttendee->StatusL() != CCalAttendee::EAccepted &&
                  aRhs.iCalAttendee->StatusL() != CCalAttendee::ETentative  ) )
            )
        {
        result = -1;
        }
    else
        {
        result = 1;
        }

    return result;
    }

// ---------------------------------------------------------
// CAttendeeItem::IsOrganizer
// ---------------------------------------------------------
//
TBool CAttendeeItem::IsOrganizer() const
    {
    return iOrganizerSatus;
    }
    
// ---------------------------------------------------------
// CAttendeeItem::SetOrganizerStatus
// ---------------------------------------------------------
//
void CAttendeeItem::SetOrganizerStatus(TBool aOrganizerStatus)
    {
    iOrganizerSatus = aOrganizerStatus;
    }
    
// End of File
