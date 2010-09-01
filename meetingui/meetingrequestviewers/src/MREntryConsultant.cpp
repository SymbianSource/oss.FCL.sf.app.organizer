/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Static helper methods for analyzing entries
*
*/




// INCLUDE FILES
#include "MREntryConsultant.h"
#include "MRHelpers.h"
#include <calentry.h>
#include <caltime.h>
#include <calrrule.h>
#include <caluser.h>
#include <msvids.h>
#include "CMRUtilsInternal.h"
#include <MsgMailUIDs.h>
#include <cmrmailboxutils.h>

// CONSTANTS

/// Unnamed namespace for local definitions
namespace {
    
}  // namespace

// ============================ MEMBER FUNCTIONS ===============================


TMsvId MREntryConsultant::SendingMailBoxL(
    const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
    CMRMailboxUtils& aUtils )
	{
	if ( aInParams.iCallingApp.iUid == KUidMsgMailViewer )
	    {
        return aInParams.iMailBoxId;	    
	    }
    else
        {
        CMRMailboxUtils::TMailboxInfo info;
        // returns KMsvNullIndexEntryId if default box cannot be resolved:
        aUtils.GetDefaultMRMailBoxL( info );
        return info.iEntryId;
        }
	}

TBool MREntryConsultant::IsRepeatingMeetingL( const CCalEntry& aEntry )
    {
    TBool retVal( EFalse );
        
    if ( IsModifyingEntryL( aEntry ) )
        {
        retVal = ETrue;
        }
    else
        {
        TCalRRule dummyRule;
        if ( aEntry.GetRRuleL( dummyRule ) )
            {
            retVal = ETrue;
            }
        else
            {            
            RArray<TCalTime> dummyRDateList;
            CleanupClosePushL( dummyRDateList );
            aEntry.GetRDatesL( dummyRDateList );
            if ( dummyRDateList.Count() > 0 )
                {
                retVal = ETrue;
                }
            CleanupStack::PopAndDestroy(); // dummyRDateList
            }
        }
    return retVal;
    }

TBool MREntryConsultant::IsModifyingEntryL( const CCalEntry& aEntry )
    {
    return ( aEntry.RecurrenceIdL().TimeUtcL() !=
             Time::NullTTime() ) ? ETrue : EFalse;
    } 


TBool MREntryConsultant::IsEditedL(
    const CCalEntry& aEntry,
    CMRUtilsInternal& aUtils )
    {
    TBool retVal( EFalse );
    CCalEntry* dbEntry = aUtils.FetchEntryL( aEntry.UidL(), 
                                             aEntry.RecurrenceIdL() );
    CleanupStack::PushL( dbEntry );
    if ( dbEntry )
        { // edited if differs from the db entry
        retVal = !( aEntry.CompareL( *dbEntry ) );
        }
    else
        { // no db entry exists -> edited
        retVal = ETrue;
        }            
    CleanupStack::PopAndDestroy( dbEntry );
    return retVal;
    }

TBool MREntryConsultant::IsSentL( const CCalEntry& aEntry )
    {
    TBool retVal( EFalse );
    if ( aEntry.StatusL() != CCalEntry::ENullStatus )
        { // When we send a request for the first time we set it's status to
          // some other value than ENullStatus
        retVal = ETrue;
        }
    return retVal;
    }

TBool MREntryConsultant::IsLatestSavedSentL(
    const CCalEntry& aEntry )
    {
    TBool retVal( EFalse );
    if ( aEntry.DTStampL().TimeUtcL() == Time::NullTTime() )
        {
        // If entries are synchronized to phone we must always assume that
        // they have been sent already. In case of our own requests we keep the
        // DTSTAMP as null until sending.    

        retVal = ETrue;
        }    
    return retVal;
    }

TBool MREntryConsultant::IsCancelledL(
    const CCalEntry& aEntry,
    CMRMailboxUtils& aUtils )
    {
    TBool retVal( EFalse );
    // Check if either entire meeting is cancelled, or this attendee
    // has been removed from the participant list. According to RFC2446
    // in that case entry doesn't have status field set.
    if ( aEntry.StatusL() == CCalEntry::ECancelled ||
         ( aEntry.MethodL() == CCalEntry::EMethodCancel &&
         !aUtils.ThisAttendeeL( aEntry ) ) )
        {
        retVal = ETrue;
        }
    return retVal;
    }

TBool MREntryConsultant::ExistsInDbL(
    const CCalEntry& aEntry,
    CMRUtilsInternal& aUtils,
    TBool aStrictMode )
    {
    TInt retVal( EFalse );
    CCalEntry* dbEntry = aUtils.FetchEntryL( aEntry.UidL(), 
                                             aEntry.RecurrenceIdL() );
    if ( dbEntry )
        {
        CleanupStack::PushL( dbEntry );
        if ( aStrictMode )
            {
            TInt seq( aEntry.SequenceNumberL() );
            TInt dbSeq( dbEntry->SequenceNumberL() );
            TTime stamp = aEntry.DTStampL().TimeUtcL();
            TTime dbStamp = dbEntry->DTStampL().TimeUtcL();            
            if ( ( seq == dbSeq ) && ( stamp == dbStamp ) )
                {
                retVal = ETrue;
                }
            }
        else
            {
            retVal = ETrue;
            }
        CleanupStack::PopAndDestroy( dbEntry ); 
        }    
    return retVal;
    }

TBool MREntryConsultant::IsEntryOutOfDateL( const CCalEntry& aEntry )
	{
	TBool retVal( EFalse );
	TTime universalTime;
	universalTime.UniversalTime();
	TTime meetingUniversalTime = aEntry.StartTimeL().TimeUtcL();
	if ( universalTime  > meetingUniversalTime )
	    {	    
		retVal = ETrue;
	    }
	return retVal;
	}

const TDesC& MREntryConsultant::PhoneOwnerAddrL(
    const CCalEntry& aEntry,
    CMRMailboxUtils& aUtils )
	{
	if ( aUtils.IsOrganizerL( aEntry ) )
	    {
	    return aEntry.OrganizerL()->Address();	    
	    }
    else
	    {	        
    	CCalAttendee* thisAttendee = aUtils.ThisAttendeeL( aEntry  );	
    	if ( thisAttendee )
    	    {	    
    		return thisAttendee->Address();
    	    }
    	else
    	    {	    
    		return KNullDesC;
    	    }
	    }
	}

CCalAttendee* MREntryConsultant::EqualAttendeeL(
    const CCalAttendee& aAttendee,
    const CCalEntry& aEntry )
    {
	TPtrC addr = MRHelpers::AddressWithoutMailtoPrefix( aAttendee.Address() );
    RPointerArray<CCalAttendee>& attendees = aEntry.AttendeesL();
    TInt count( attendees.Count() );
	for ( TInt i ( 0 ); i < count; ++i )
		{
		TPtrC testAddr =
		    MRHelpers::AddressWithoutMailtoPrefix( attendees[i]->Address() );
		if ( addr.CompareF( testAddr ) == 0 )
		    {
		    return attendees[i];
		    }
		}
    return NULL;
    }

TBool MREntryConsultant::SpansManyDaysL(
    const TCalTime& aStartTime,
    const TCalTime& aEndTime )
	{
	TBool retVal( EFalse );
		
    // mustn't use UTC time here, local time specifies if the midnight
    // is crossed
	TTime localStartTime = aStartTime.TimeLocalL();
	TTime localEndTime = aEndTime.TimeLocalL();
	
	TTimeIntervalDays days = localEndTime.DaysFrom( localStartTime );
	
	if ( days.Int() > 0 )
		{
		retVal = ETrue;
		}
	return retVal;
	}

//  End of File
