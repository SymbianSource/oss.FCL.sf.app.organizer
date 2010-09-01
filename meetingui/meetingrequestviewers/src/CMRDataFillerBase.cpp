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
* Description:   Implementation of base class for meeting request data fillers
*
*/




// INCLUDE FILES
#include "CMRDataFillerBase.h"
#include "MMRModelInterface.h"
#include "MRViewersPanic.h"
#include "CMRStringFormatter.h"
#include "MREntryConsultant.h"
#include <e32std.h>
#include <msvids.h>
#include <coemain.h> 	            // CCoeEnv
#include <stringloader.h>           // StringLoader
#include <avkon.rsg>                // resouce identifiers
#include <crichbio.h>               // CRichBio
#include <calentry.h>				// Calendar entry API V2
#include <calrrule.h>
#include <meetingrequestviewersuires.rsg>// Resource strings for meeting request 
#include <stringloader.h>			// resource string loading utils
#include <calalarm.h>               // CCalAlarm
#include <cmrmailboxutils.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "~CMRDataFillerBase" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

}  // namespace


// ============================ MEMBER FUNCTIONS ===============================

CMRDataFillerBase::~CMRDataFillerBase()
    {
    delete iStringFormatter;
    }

void CMRDataFillerBase::AddItemL(
    CRichBio& aViewer,
    TInt aLabelRes,
    const TDesC& aValue )
    {
    // Empty fields are not shown.
    if ( aValue.Length() )
        {
        aViewer.AddItemL( *StringLoader::LoadLC( aLabelRes, &iCoeEnv ),
                          aValue );
        CleanupStack::PopAndDestroy(); // (label text)
        }
    }

void CMRDataFillerBase::AddItemL(
    CRichBio& aViewer,
    TInt aLabelRes,
    TInt aValueRes )
    {
    aViewer.AddItemL( *StringLoader::LoadLC( aLabelRes, &iCoeEnv ),
                      *StringLoader::LoadLC( aValueRes, &iCoeEnv ) );
    CleanupStack::PopAndDestroy(2); // (label and value text)
    }

// -----------------------------------------------------------------------------
// ?classname::?classname
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMRDataFillerBase::CMRDataFillerBase(
    CRichBio& aRichBio,
    MMRModelInterface& aModel, 
    CCoeEnv& aCoeEnv,
    CMRMailboxUtils& aUtils,
    MAgnEntryUi::TAgnEntryUiInParams& aParams )
    : iCoeEnv( aCoeEnv ),
      iModel( aModel ),
      iRichBio( aRichBio ),
      iUtils( aUtils ),
      iParams( aParams )
    {
    }

// -----------------------------------------------------------------------------
// ?classname::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMRDataFillerBase::ConstructL()
    {
    iStringFormatter = CMRStringFormatter::NewL( iCoeEnv );
    }
    
void CMRDataFillerBase::FillViewerWithDataL()
    {
    iEntry = iModel.CombinedEntry();    
    __ASSERT_DEBUG( iEntry, Panic( ECombinedCalEntryNull ) );
    
    // call templated method
    DoFillViewerL();
    }
	
void CMRDataFillerBase::RefreshViewerL()
    {
    iRichBio.Reset();
    FillViewerWithDataL();
    }
	
TBool CMRDataFillerBase::MeetingOnSameDayL()
	{	
	return !MREntryConsultant::SpansManyDaysL( iEntry->StartTimeL(),
                                               iEntry->EndTimeL() );
	}
    
void CMRDataFillerBase::FillOrganiserDataL()
	{
	AddItemL( iRichBio,
	          R_QTN_MAIL_MTG_ORGANISER,
	          iEntry->OrganizerL()->Address() );
	}
	
void CMRDataFillerBase::FillWhenDataL()
	{
	HBufC* string = iStringFormatter->WhenStringLC( iEntry->StartTimeL(),
	                                                iEntry->EndTimeL() );
	AddItemL( iRichBio, R_QTN_MAIL_MTG_DATE, *string );	
	CleanupStack::PopAndDestroy( string );
	}
	
void CMRDataFillerBase::FillStartTimeDataL()
	{
	HBufC* string = iStringFormatter->DateStringLC( iEntry->StartTimeL() );
	AddItemL( iRichBio, R_QTN_SM_CAL_STARTTIME, *string );
	CleanupStack::PopAndDestroy( string );
	}
	
void CMRDataFillerBase::FillEndTimeDataL()
	{
	HBufC* string = iStringFormatter->DateStringLC( iEntry->EndTimeL() );
	AddItemL( iRichBio, R_QTN_SM_CAL_ENDTIME, *string );
	CleanupStack::PopAndDestroy( string );
	}
	
void CMRDataFillerBase::FillLocationDataL()
	{
	AddItemL( iRichBio,R_QTN_SM_CAL_LOCATION,iEntry->LocationL());
	}
	
void CMRDataFillerBase::FillDescriptionDataL()
	{
	AddItemL( iRichBio,R_QTN_SM_CAL_DESCRIPTION,iEntry->DescriptionL());
	}

TInt CMRDataFillerBase::GetAttendeeStatusL( const CCalAttendee& aAttendee )
	{
	TInt retVal( 0 );
	
	CCalAttendee::TCalStatus status = aAttendee.StatusL();
	
	switch( status )
		{
		case CCalAttendee::ENeedsAction:
			{
			retVal = R_QTN_MAIL_MTG_PLS_RESPOND;
			break;
			}
		case CCalAttendee::EAccepted: // fall through
		case CCalAttendee::EConfirmed:		
			{
			retVal = R_QTN_MAIL_MTG_ACCEPTED;
			break;
			}
		case CCalAttendee::ETentative:
			{
			retVal = R_QTN_MAIL_MTG_TENTATIVE;
			break;
			}
		case CCalAttendee::EDeclined:
			{
			retVal = R_QTN_MAIL_MTG_DECLINE;
			break;
			}
		default:
			{
			break;
			}
		}
		
	return retVal;
	}

void CMRDataFillerBase::FillStatusDataL()
	{	
	CCalAttendee* attendee = iUtils.ThisAttendeeL( *iEntry );	
	if ( attendee )
		{
		TInt retVal = GetAttendeeStatusL( *attendee );
		if ( retVal != 0 )
		    {		    
		    AddItemL( iRichBio, R_QTN_MAIL_MTG_STATUS, retVal );
		    }
		}
	}
	
TInt CMRDataFillerBase::GetAttendanceStatusL( const CCalAttendee& aAttendee )
	{
	TInt retVal( 0 );
	
	CCalAttendee::TCalRole role = aAttendee.RoleL();
	
	switch( role )
		{
		case CCalAttendee::EReqParticipant:
			{
			retVal = R_QTN_MAIL_MTG_REQUIRED;
			break;
			}
		case CCalAttendee::EOptParticipant:
			{
			retVal = R_QTN_MAIL_MTG_OPTIONAL;
			break;
			}
		case CCalAttendee::ENonParticipant:
			{
			retVal = R_QTN_MAIL_MTG_OPTIONAL;
			break;
			}
		case CCalAttendee::EChair:
			{
			retVal = R_QTN_MAIL_MTG_REQUIRED;
			break;
			}
		default:
			{
			break;
			}
		}		
	return retVal;
	}
	
	
void CMRDataFillerBase::FillAttendanceDataL()
	{
	CCalAttendee* attendee = iUtils.ThisAttendeeL( *iEntry );	
	if ( attendee )
		{
		TInt retVal = GetAttendanceStatusL( *attendee );
		if ( retVal != 0 )
		    {		    
			AddItemL( iRichBio, R_QTN_MAIL_MTG_ATTENDANCE, retVal );
		    }
		}
	}
	
void CMRDataFillerBase::FillAlarmTimeDataL()
	{
	CCalAlarm* alarm = iEntry->AlarmL(); // ownership transferred
	if ( alarm )
		{
		CleanupStack::PushL( alarm );
		TTime alarmTime = iEntry->StartTimeL().TimeLocalL() - 
		                  alarm->TimeOffset();
	    HBufC* string = iStringFormatter->TimeStringLC( alarmTime );
	    AddItemL( iRichBio, R_QTN_SM_CAL_ALARMTIME, *string );
	    CleanupStack::PopAndDestroy( 2, alarm ); // string, alarm
		}
	}
	
void CMRDataFillerBase::FillAlarmDateDataL()
	{
	CCalAlarm* alarm = iEntry->AlarmL(); // ownership transferred
	if ( alarm )
		{
		CleanupStack::PushL( alarm );
		TTime alarmTime = iEntry->StartTimeL().TimeLocalL() - 
		                  alarm->TimeOffset();
	    HBufC* string = iStringFormatter->DateStringLC( alarmTime );
	    AddItemL( iRichBio, R_QTN_SM_CAL_ALARMDATE, *string );
	    CleanupStack::PopAndDestroy( 2, alarm ); // string, alarm
		}
	}

TInt CMRDataFillerBase::GetRepeatingEntryRuleL()
	{
	TCalRRule calRule;
	
	TInt retVal( 0 );
	
	if ( iEntry->GetRRuleL( calRule ) )
		{
		TCalRRule::TType repeatType( calRule.Type() );
		
		switch ( repeatType )
			{
			case TCalRRule::EDaily:
				{
				retVal = R_QTN_SM_CAL_RPT_DAILY;
				break;
				}
			case TCalRRule::EWeekly:
				{
				retVal = R_QTN_SM_CAL_RPT_WEEKLY;
				break;
				}
			case TCalRRule::EMonthly:
				{
				retVal = R_QTN_SM_CAL_RPT_MONTHLY;
				break;
				}
			case TCalRRule::EYearly:
				{
				retVal = R_QTN_SM_CAL_RPT_YEARLY;
				break;
				}
			default:
				{
				retVal = R_QTN_SM_CAL_RPT_DAILY;
				break;
				}
			}
		}	
	return retVal;
	}
	
void CMRDataFillerBase::FillRepeatRuleDataL()
	{
	TInt retVal = GetRepeatingEntryRuleL();
	if ( retVal )
		{
		AddItemL( iRichBio,R_QTN_SM_CAL_REPEAT, retVal );
		}
	}
	
void CMRDataFillerBase::FillRepeatUntilDataL()
	{
	TCalRRule calRule;
	if ( iEntry->GetRRuleL( calRule ) )
		{
	    HBufC* string = iStringFormatter->DateStringLC( calRule.Until() );
	    AddItemL( iRichBio, R_QTN_SM_CAL_REPEAT_UNTIL, *string );
	    CleanupStack::PopAndDestroy( string );
		}
	}
	
void CMRDataFillerBase::FillSynchronisationDataL()
	{
	CCalEntry::TReplicationStatus replicationStatus;
	
	replicationStatus = iEntry->ReplicationStatusL();
	
	TInt replicationVal;
	
	switch( replicationStatus )
		{
		case CCalEntry::EOpen:
			{
			replicationVal = R_QTN_SM_CAL_CONF_PUBLIC;
			break;
			}
		case CCalEntry::EPrivate:
			{
			replicationVal = R_QTN_SM_CAL_CONF_PRIVATE;
			break;
			}
		case CCalEntry::ERestricted:
			{
			replicationVal = R_QTN_SM_CAL_CONF_NO_SYNC;
			break;
			}
		default:
			{
			replicationVal = R_QTN_SM_CAL_CONF_PUBLIC;
			}
		}
	AddItemL( iRichBio,R_QTN_SM_CAL_CONFIDENTIALITY, replicationVal );
	}

//  End of File

