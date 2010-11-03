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
* Description:   Implementation of base class for meeting request data fillers
*
*/



// INCLUDE FILES
#include "CMRDataFillerMeetingRequest.h"
#include <cmrmailboxutils.h>

// ============================ MEMBER FUNCTIONS ===============================

CMRDataFillerMeetingRequest::~CMRDataFillerMeetingRequest()
    {
    }

void CMRDataFillerMeetingRequest::DoFillViewerL()
    {    
    FillOrganiserDataL();
	if ( MeetingOnSameDayL() )
		{
		FillWhenDataL();
		}
	else
		{
		FillStartTimeDataL();
		FillEndTimeDataL();
		}
	FillLocationDataL();
	FillDescriptionDataL();
	FillStatusDataL();
	FillAttendanceDataL();
	FillAlarmTimeDataL();
	FillAlarmDateDataL();
	FillRepeatRuleDataL();
	FillRepeatUntilDataL();
	FillSynchronisationDataL();	
    }

CMRDataFillerMeetingRequest* CMRDataFillerMeetingRequest::NewL(
    CRichBio& aRichBio,
    MMRModelInterface& aModel,
    CCoeEnv& aCoeEnv,
    CMRMailboxUtils& aUtils,
    MAgnEntryUi::TAgnEntryUiInParams& aParams )
    {
    CMRDataFillerMeetingRequest* self =
        new( ELeave ) CMRDataFillerMeetingRequest( aRichBio,
                                                   aModel,
                                                   aCoeEnv,
                                                   aUtils,
                                                   aParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CMRDataFillerMeetingRequest::CMRDataFillerMeetingRequest(
    CRichBio& aRichBio,
    MMRModelInterface& aModel,
    CCoeEnv& aCoeEnv,
    CMRMailboxUtils& aUtils,
    MAgnEntryUi::TAgnEntryUiInParams& aParams )
    : CMRDataFillerBase( aRichBio, aModel, aCoeEnv, aUtils, aParams )
    {
    }

void CMRDataFillerMeetingRequest::ConstructL()
    {
    CMRDataFillerBase::ConstructL();
    }
    

//  End of File
