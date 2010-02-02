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
* Description:   Implementation for meeting request cancel data filler
*
*/



// INCLUDE FILES
#include "CMRDataFillerCancel.h"
#include <meetingrequestviewersuires.rsg>

// ============================ MEMBER FUNCTIONS ===============================

CMRDataFillerCancel::~CMRDataFillerCancel()
    {
    }

void CMRDataFillerCancel::DoFillViewerL()
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

CMRDataFillerCancel* CMRDataFillerCancel::NewL(
    CRichBio& aRichBio,
    MMRModelInterface& aModel,
    CCoeEnv& aCoeEnv,
    CMRMailboxUtils& aUtils,
    MAgnEntryUi::TAgnEntryUiInParams& aParams )
    {
    CMRDataFillerCancel* self =
        new( ELeave ) CMRDataFillerCancel( aRichBio,
                                           aModel,
                                           aCoeEnv,
                                           aUtils,
                                           aParams );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CMRDataFillerCancel::CMRDataFillerCancel(
    CRichBio& aRichBio,
    MMRModelInterface& aModel,
    CCoeEnv& aCoeEnv,
    CMRMailboxUtils& aUtils,
    MAgnEntryUi::TAgnEntryUiInParams& aParams )
    : CMRDataFillerBase( aRichBio, aModel, aCoeEnv, aUtils, aParams )
    {
    }

void CMRDataFillerCancel::ConstructL()
    {
    CMRDataFillerBase::ConstructL();
    }

void CMRDataFillerCancel::FillStatusDataL()
    {
	AddItemL( iRichBio, R_QTN_MAIL_MTG_STATUS, R_QTN_MAIL_MTG_CANCELLED );
    }
    
//  End of File
