/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:Implementation for meeting request info popup 
*
*/




// INCLUDE FILES
#include "CMRInfoPopup.h"
#include "MRViewersPanic.h"
#include "MREntryConsultant.h"
#include "CMRStringFormatter.h"
#include <meetingrequestviewersuires.rsg>
#include <CMRMailboxUtils.h>
#include <calentry.h> 		//CCalEntry (Calendar entry API V2)
#include <caluser.h> 		//caluser and attendee
#include <AknInfoPopupNoteController.h>
#include <stringloader.h>
#include <eikenv.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CMRInfoPopup" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

// Avkon takes milliseconds as a parameter, but timers use microseconds,
// therefore to must avoid overflow the maximum time is KMaxInt / 1000.
const TInt KMaxMilliseconds = KMaxTInt / 1000;
const TInt KBufferSize = 512;

}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMRInfoPopup::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMRInfoPopup* CMRInfoPopup::NewL(
    CEikonEnv* aEnv,
    CMRMailboxUtils& aMBoxUtils,
    const CCalEntry& aEntry,
    TBool aViewMode )
    {
    CMRInfoPopup* self =
        new( ELeave ) CMRInfoPopup( aEnv, aMBoxUtils, aViewMode );

    CleanupStack::PushL( self );
    self->ConstructL( aEntry );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CMRInfoPopup::CMRInfoPopup
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMRInfoPopup::CMRInfoPopup(
     CEikonEnv* aEnv,
     CMRMailboxUtils& aMBoxUtils,
     TBool aViewMode )
    : iEnv( aEnv ),
      iMBoxUtils( aMBoxUtils ),
      iViewMode( aViewMode ), 
      iIsValid( ETrue ),
      iIsSent( EFalse ),
      iRowIndex( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CMRInfoPopup::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMRInfoPopup::ConstructL( const CCalEntry& aEntry )
    {
    iBuffer.CreateL( KBufferSize );
    
    iStringFormatter = CMRStringFormatter::NewL( *iEnv );
    
    iNoteController = CAknInfoPopupNoteController::NewL();
    // no delay:
    iNoteController->SetTimeDelayBeforeShow( 0 );
    // according to spec info popup is shown all the time in MR Viewers,
    // when we give KMaxMilliseconds it will be shown for ~35 hours:
    iNoteController->SetTimePopupInView( KMaxMilliseconds );
    
    RefreshTextL( aEntry, 0 ); // row index 0 used at startup    
    }

// Destructor
CMRInfoPopup::~CMRInfoPopup()
    {
    delete iStringFormatter;
    delete iNoteController;
    iBuffer.Close();
    }

// -----------------------------------------------------------------------------
// CMRInfoPopup::ShowInfoPopupL
// 
// -----------------------------------------------------------------------------
//
void CMRInfoPopup::ShowInfoPopupL()
    {
    iNoteController->ShowInfoPopupNote();
    }

// -----------------------------------------------------------------------------
// CMRInfoPopup::HideInfoPopup
// 
// -----------------------------------------------------------------------------
//
void CMRInfoPopup::HideInfoPopup()
    {
    iNoteController->HideInfoPopupNote();
    }

// -----------------------------------------------------------------------------
// CMRInfoPopup::SetMeetingValidity
// 
// -----------------------------------------------------------------------------
//        
void CMRInfoPopup::SetMeetingValidity( TBool aIsValid )
    {
    iIsValid = aIsValid;
    }

// -----------------------------------------------------------------------------
// CMRInfoPopup::SetSendingStatus
// 
// -----------------------------------------------------------------------------
//        
void CMRInfoPopup::SetSendingStatus( TBool aIsSent )
    {
    iIsSent = aIsSent;
    }

// -----------------------------------------------------------------------------
// CMRInfoPopup::RefreshTextL
// 
// -----------------------------------------------------------------------------
//        
void CMRInfoPopup::RefreshTextL( const CCalEntry& aEntry, TInt aRowIndex )
    {
    __ASSERT_DEBUG( aRowIndex == 0 || aRowIndex == 1,
                    Panic( EIllegalParamValue ) );

    iRowIndex = aRowIndex;
    // update data in iBuffer and in note controller, info popup may be
    // currently visible or not
    if ( iRowIndex == 0 )
        {
        FillWithMeetingInfoL( aEntry );
        }
    else
        {
        if ( iViewMode )
            {            
            FillWithViewerAttendeeInfoL( aEntry );
            }
        else
            {
            FillWithEditorAttendeeInfoL( aEntry );
            }
        }        
    iNoteController->SetTextL( TextL() );
    ShowInfoPopupL();
    }

const TDesC& CMRInfoPopup::TextL()
    {
    return iBuffer;
    }

void CMRInfoPopup::FillWithMeetingInfoL( const CCalEntry& aEntry )
    {
    iBuffer.Zero();
    if ( MREntryConsultant::IsRepeatingMeetingL( aEntry ) )
        {
        StringLoader::Load( iBuffer, R_QTN_CALE_INFOPOPUP_RECURRING, iEnv );
        iBuffer.Append( EKeyLineFeed );
        }

    HBufC* timeString( NULL );     
// TODO: check day span this when UI spec is corrected
//        if ( MREntryConsultant::SpansManyDaysL( aEntry.StartTimeL(),
//                                                aEntry.EndTimeL() ) )
    timeString = iStringFormatter->WhenStringLC( aEntry.StartTimeL(),
                                                 aEntry.EndTimeL() );
    iBuffer.Append( *timeString );
    CleanupStack::PopAndDestroy( timeString );
    }

void CMRInfoPopup::FillWithViewerAttendeeInfoL( const CCalEntry& aEntry )
    {
    iBuffer.Zero();
    TInt resource( 0 );    
    CCalAttendee* thisAtt = iMBoxUtils.ThisAttendeeL( aEntry );        
    
    if ( aEntry.StatusL() == CCalEntry::ECancelled )
        {
        resource = R_QTN_CALE_STATUS_CANCELLED;
        }
    else if ( MREntryConsultant::IsEntryOutOfDateL( aEntry ) ||
              !iIsValid )
        {
        resource = R_QTN_CALE_STATUS_OUT_OF_DATE;
        }
    else if ( thisAtt )
        {
        switch ( thisAtt->StatusL() )
            {
            case CCalAttendee::ENeedsAction:
                {
                resource = R_QTN_CALE_STATUS_PLEASE_RESPOND;
                break;
                }
            case CCalAttendee::EAccepted: // fall through
            case CCalAttendee::EConfirmed:
                {
                resource = R_QTN_CALE_STATUS_ACCEPTED;
                break;
                }                    
            case CCalAttendee::ETentative:
                {
                resource = R_QTN_CALE_STATUS_TENTATIVE;
                break;
                } 
            case CCalAttendee::EDeclined:
                {
                resource = R_QTN_CALE_STATUS_DECLINED;
                break;
                }
            default:
                {
                break;
                }                    
            }
        }
    if ( resource != 0 )
        {                
        StringLoader::Load( iBuffer, resource, iEnv );
        }
    }
    
void CMRInfoPopup::FillWithEditorAttendeeInfoL( const CCalEntry& aEntry )
    {
    iBuffer.Zero();

    if ( iIsSent )
        {
        RPointerArray<CCalAttendee>& attendees = aEntry.AttendeesL();
        TInt count( attendees.Count() );
        TInt countAccepted( 0 );
        TInt countTentative( 0 );        
        TInt countDeclined( 0 );
        TInt countNeedsAction( 0 );
        for ( TInt i( 0 ); i < count; ++i )
            {
            switch ( attendees[i]->StatusL() )
                {
                case CCalAttendee::EAccepted: // fall through
                case CCalAttendee::EConfirmed:
                    {
                    countAccepted++;
                    break;
                    }                    
                case CCalAttendee::ETentative:
                    {
                    countTentative++;
                    break;
                    } 
                case CCalAttendee::EDeclined:
                    {
                    countDeclined++;
                    break;
                    }
                case CCalAttendee::ENeedsAction:
                    {
                    countNeedsAction++;
                    break;
                    }                    
                default:
                    {
                    break;
                    }                    
                }
            
            }
        if ( countAccepted > 0 )
            {
            HBufC* accepts = StringLoader::LoadLC(
                R_QTN_CALE_INFOPOPUP_ACCEPTED, countAccepted, iEnv );
            iBuffer.Append( *accepts );
            CleanupStack::PopAndDestroy( accepts );
            iBuffer.Append( EKeyLineFeed );            
            }
        if ( countDeclined > 0 )
            {
            HBufC* declines = StringLoader::LoadLC(
                R_QTN_CALE_INFOPOPUP_DECLINED, countDeclined, iEnv );
            iBuffer.Append( *declines );
            CleanupStack::PopAndDestroy( declines );
            iBuffer.Append( EKeyLineFeed );
            }
        if ( countTentative > 0 )
            {
            HBufC* tentatives = StringLoader::LoadLC(
                R_QTN_CALE_INFOPOPUP_TENTATIVE, countTentative, iEnv );
            iBuffer.Append( *tentatives );
            CleanupStack::PopAndDestroy( tentatives );
            iBuffer.Append( EKeyLineFeed );
            }
        if ( countNeedsAction > 0 )
            {
            HBufC* noresponses = StringLoader::LoadLC(
                R_QTN_CALE_INFOPOPUP_NO_RESPONSE, countNeedsAction, iEnv );
            iBuffer.Append( *noresponses );
            CleanupStack::PopAndDestroy( noresponses );
            iBuffer.Append( EKeyLineFeed );
            }                                    
        }
    else
        {
        StringLoader::Load( iBuffer, R_QTN_CALE_INFOPOPUP_NOT_SENT, iEnv );
        }
    }


//  End of File

