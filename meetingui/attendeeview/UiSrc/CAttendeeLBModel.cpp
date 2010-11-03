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
* Description:  Implementation methods for listbox model.
*
*/



// INCLUDE FILES
#include "CAttendeeLBModel.h"
#include "AttendeeViewConsts.h"
#include "MAttendeeEng.h"
#include "AttendeeView.hrh"
#include <AttendeeView_res.rsg>
#include <AknUtils.h>
#include <caluser.h>

/// Unnamed namespace for local definitions
namespace {
_LIT( KPanicMsg, "CAttendeeLBModel" );

enum TPanicCode
    {    
    KAttPanicUnknownOperation = 1
    };

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CAttendeeLBModel::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAttendeeLBModel* CAttendeeLBModel::NewLC( MAttendeeEng& aEngine )
    {
    CAttendeeLBModel* self = new(ELeave)CAttendeeLBModel( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

// ---------------------------------------------------------
// CAttendeeLBModel::ConstructL
// ---------------------------------------------------------
//
void CAttendeeLBModel::ConstructL()
    {
    CEikonEnv* env = CEikonEnv::Static();
    iBuffer = HBufC::NewL( KAttendeeBufferSize );       
    
    iTitles = new(ELeave)CAttendeeTitles;
    iTitles->iOrganizer =env->AllocReadResourceL( R_ATTENDEE_ORGANIZER_TITLE );
    iTitles->iRequire = env->AllocReadResourceL( R_ATTENDEE_REQUIRED_TITLE );
    iTitles->iOptional = env->AllocReadResourceL( R_ATTENDEE_OPTIONAL_TITLE );
    }

// ----------------------------------------------------------------------------
// CAttendeeLBModel::CAttendeeLBModel
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CAttendeeLBModel::CAttendeeLBModel( MAttendeeEng& aEngine ) : 
                                    iEngine( aEngine )
    {
    }

// ---------------------------------------------------------
// CAttendeeLBModel::~CAttendeeLBModel
// ---------------------------------------------------------
//
CAttendeeLBModel::~CAttendeeLBModel()
    {
    delete iBuffer;    
    delete iTitles;
    }

// ---------------------------------------------------------
// CAttendeeLBModel::MdcaCount
// ---------------------------------------------------------
//
TInt CAttendeeLBModel::MdcaCount() const
    {    
    if ( CheckIfOnlyOrganizers() )
        {
        //Show empty listbox if listbox
        //contain only organizers
        return 0;
        }
    return (iEngine.NumberOfItems()); //Attendees (and organizer)
    }

// ---------------------------------------------------------
// CAttendeeLBModel::MdcaPoint
// ---------------------------------------------------------
//
TPtrC16 CAttendeeLBModel::MdcaPoint( TInt aIndex ) const
    {
    TPtr ptr( iBuffer->Des() );
    ptr.Zero();
       
    CAttendeeItem& item = iEngine.At( aIndex);
       
    TRAPD( ignore, AppendIconL( ptr, item ) );
       
    ptr.Append( EKeyTab );
    
    TRAP( ignore, AppendAttendanceL( ptr, item ) );
    
    ptr.Append( EKeyTab );    
    AppendAddressOrTitle( ptr, item );    
    ptr.Append( EKeyTab );               

    
    AknTextUtils::LanguageSpecificNumberConversion( ptr );    
    return iBuffer->Des();
    }

// ---------------------------------------------------------
// CAttendeeLBModel::CheckIfOnlyOrganizers
// ---------------------------------------------------------
//
TBool CAttendeeLBModel::CheckIfOnlyOrganizers() const
    {
    
    const TInt count( iEngine.NumberOfItems() );
    
    for ( TInt i( 0 ); i < count; ++i )
        {
        if (!iEngine.At( i ).IsOrganizer())
            {
            return EFalse;
            }
        }
    return ETrue;    
    }

// ---------------------------------------------------------
// CAttendeeLBModel::AppendIcon
// ---------------------------------------------------------
//
void CAttendeeLBModel::AppendIconL( TPtr& aPtr, CAttendeeItem& aItem ) const
    {
    if ( aItem.IsOrganizer() )
        {
        aPtr.AppendNum( EAttOrganizer );
        }
    else
        {
        switch( aItem.AgnAttendee()->StatusL() )
            {
            case CCalAttendee::EAccepted:
                {
                aPtr.AppendNum( EAttAccepted );
                break;
                }
            case CCalAttendee::ETentative:
                {
                aPtr.AppendNum( EAttTentative );
                break;
                }
            case CCalAttendee::EDeclined:
                {
                aPtr.AppendNum( EAttRejected );
                break;
                }
            case CCalAttendee::ENeedsAction: //fall trought
            default:
                {
                aPtr.AppendNum( EAttNotAnswered );
                break;
                }
            }    
        }
    }

// ---------------------------------------------------------
// CAttendeeLBModel::AppendAttendance
// ---------------------------------------------------------
//
void CAttendeeLBModel::AppendAttendanceL( TPtr& aPtr, 
                                         CAttendeeItem& aItem ) const
    {
     
    if ( aItem.IsOrganizer() )
        {
        aPtr.Append( *iTitles->iOrganizer );
        }
    else
        {    
        switch( aItem.AttendanceL() )
            {
            
            case CAttendeeItem::ERequire:
                {
                aPtr.Append( *iTitles->iRequire );
                break;
                }
            case CAttendeeItem::EOptional:
                {
                aPtr.Append( *iTitles->iOptional );
                break;
                }
            default:
                {
                Panic( KAttPanicUnknownOperation );
                break;
                }
            }
        }        
    }

// ---------------------------------------------------------
// CAttendeeLBModel::AppendAddressOrTitle
// ---------------------------------------------------------
//
void CAttendeeLBModel::AppendAddressOrTitle( TPtr& aPtr, 
                                             CAttendeeItem& aItem ) const
    {
    
    //Check if contact id exists (one match found from pbk)
    if ( ( aItem.ContactId() != KNullContactId ) &&
           aItem.ContactTitle() )
        {                  
        aPtr.Append( *aItem.ContactTitle() );        
        }
    else 
        {        
        //Check if we have alias name
        if(aItem.AgnAttendee()->CommonName() != KNullDesC)
            {
            //Alias name is shown before email address
            aPtr.Append( KAttendeeViewQuotationMark );
            aPtr.Append( aItem.AgnAttendee()->CommonName() );
            aPtr.Append( KAttendeeViewQuotationMark );
            }
        
        //Append email address
        aPtr.Append( KAttendeeViewLessThat );
        aPtr.Append( aItem.AgnAttendee()->Address() );
        aPtr.Append( KAttendeeViewGreaterThan );                
        }    
    }
// End of file
