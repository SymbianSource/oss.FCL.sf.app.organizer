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
* Description:   Implementation for meeting request reply data filler
*
*/



// INCLUDE FILES
#include "CMRDataFillerReply.h"
#include <meetingrequestviewersuires.rsg>
#include <CalEntry.h>

// ============================ MEMBER FUNCTIONS ===============================

CMRDataFillerReply::~CMRDataFillerReply()
    {
    }

void CMRDataFillerReply::DoFillViewerL()
    {
	FillDescriptionDataL();
	if(MeetingOnSameDayL())
		{
		FillWhenDataL();
		}
	else
		{
		FillStartTimeDataL();
		FillEndTimeDataL();
		}
	FillLocationDataL();
	FillStatusDataL();
	FillAttendanceDataL();
    }


CMRDataFillerReply* CMRDataFillerReply::NewL(
    CRichBio& aRichBio,
    MMRModelInterface& aModel,
    CCoeEnv& aCoeEnv,
    CMRMailboxUtils& aUtils,
    MAgnEntryUi::TAgnEntryUiInParams& aParams )
    {
    CMRDataFillerReply* self =
        new( ELeave ) CMRDataFillerReply( aRichBio,
                                          aModel,
                                          aCoeEnv,
                                          aUtils,
                                          aParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CMRDataFillerReply::CMRDataFillerReply(
    CRichBio& aRichBio,
    MMRModelInterface& aModel,
    CCoeEnv& aCoeEnv,
    CMRMailboxUtils& aMRUtils,
    MAgnEntryUi::TAgnEntryUiInParams& aParams )
    : CMRDataFillerBase( aRichBio, aModel, aCoeEnv, aMRUtils, aParams )
    {
    }

void CMRDataFillerReply::ConstructL()
    {
    CMRDataFillerBase::ConstructL();
    }

void CMRDataFillerReply::FillStatusDataL()
	{	
	// status of respondent (i.e. the first and only attendee in the entry)
	CCalAttendee* attendee = iEntry->AttendeesL()[0];
	TInt retVal = GetAttendeeStatusL( *attendee );
	if ( retVal != 0 )
	    {		    
	    AddItemL( iRichBio, R_QTN_MAIL_MTG_STATUS, retVal );
	    }
	}

void CMRDataFillerReply::FillAttendanceDataL()
	{
    // attendance of respondent (i.e. the first and only attendee in the entry)
	CCalAttendee* attendee = iEntry->AttendeesL()[0];	
	TInt retVal = GetAttendanceStatusL( *attendee );
	if ( retVal != 0 )
	    {	    
		AddItemL( iRichBio, R_QTN_MAIL_MTG_ATTENDANCE, retVal );
	    }
	}

//  End of File
