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
* Description: Implementation for meeting request string formatter  
*
*/




// INCLUDE FILES
#include "CMRStringFormatter.h"
#include "MRViewersPanic.h"
#include <coemain.h> 	            // CCoeEnv
#include <stringloader.h>           // StringLoader
#include <avkon.rsg>                // resouce identifiers
#include <meetingrequestviewersuires.rsg>
#include <calentry.h>				// Calendar entry API V2
#include <caltime.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

const TInt KMaxDateTimeTextLength = 100;

_LIT( KPanicMsg, "~CMRStringFormatter" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }

}  // namespace


// ============================ MEMBER FUNCTIONS ===============================

CMRStringFormatter* CMRStringFormatter::NewL( CCoeEnv& aCoeEnv )
    {
    CMRStringFormatter* self = new( ELeave ) CMRStringFormatter( aCoeEnv );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

CMRStringFormatter::~CMRStringFormatter()
    {
    delete iDateFormat;
    delete iTimeFormat;
    }

// -----------------------------------------------------------------------------
// CMRStringFormatter::CMRStringFormatter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMRStringFormatter::CMRStringFormatter( CCoeEnv& aCoeEnv ) : iCoeEnv( aCoeEnv )
    {
    }

// -----------------------------------------------------------------------------
// CMRStringFormatter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMRStringFormatter::ConstructL()
    {
    iDateFormat = StringLoader::LoadL( R_QTN_DATE_USUAL_WITH_ZERO, &iCoeEnv );
	iTimeFormat = StringLoader::LoadL( R_QTN_TIME_USUAL, &iCoeEnv );
    }

HBufC* CMRStringFormatter::DateStringLC( const TCalTime& aTime ) const
    {    
    return DateStringLC( aTime.TimeLocalL() );
    }

HBufC* CMRStringFormatter::DateStringLC( const TTime& aTime ) const
    {
    TBuf<KMaxDateTimeTextLength> tmpString;
    aTime.FormatL( tmpString, *iDateFormat );
    return tmpString.AllocLC();    
    }
	
HBufC* CMRStringFormatter::TimeStringLC( const TCalTime& aTime ) const
    {
    return TimeStringLC( aTime.TimeLocalL() );
    }
	
HBufC* CMRStringFormatter::TimeStringLC( const TTime& aTime ) const
    {
    TBuf<KMaxDateTimeTextLength> tmpString;
    aTime.FormatL( tmpString, *iTimeFormat );
    return tmpString.AllocLC();
    }
	
HBufC* CMRStringFormatter::WhenStringLC(
    const TCalTime& aStartTime,
    const TCalTime& aEndTime ) const
    {
	TBuf<KMaxDateTimeTextLength> tmpWhenStartDate;
	TBuf<KMaxDateTimeTextLength> tmpWhenStartTime;
	TBuf<KMaxDateTimeTextLength> tmpWhendEndTime;
	
	// use local time in ui
	TTime whenStartTime = aStartTime.TimeLocalL();
	TTime whenEndTime = aEndTime.TimeLocalL();
	
	whenStartTime.FormatL( tmpWhenStartDate, *iDateFormat );
	whenStartTime.FormatL( tmpWhenStartTime, *iTimeFormat );
	whenEndTime.FormatL( tmpWhendEndTime, *iTimeFormat );
	
	CDesCArrayFlat* timeStrings = new( ELeave ) CDesCArrayFlat( 3 );
	CleanupStack::PushL( timeStrings );
	
	timeStrings->AppendL( tmpWhenStartDate );
	timeStrings->AppendL( tmpWhenStartTime );
	timeStrings->AppendL( tmpWhendEndTime );
	HBufC* string = StringLoader::LoadL( R_QTN_MAIL_MTG_TIME_DATA,
	                                      *timeStrings,
	                                      &iCoeEnv );	
	CleanupStack::PopAndDestroy( timeStrings );
	CleanupStack::PushL( string );
	return string;
	}

//  End of File

