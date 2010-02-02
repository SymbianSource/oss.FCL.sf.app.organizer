/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 *  Description : Util api for getting next available offset value
 *
 */

//Includes
#include "calendarui_debug.h"

#include <centralrepository.h>
#include <calenmulticalutil.h>
#include <CalendarInternalCRKeys.h>

//Constants
const TUint KCalendarOffSet = 0x000186A0; //50000*2
const TInt  KOffsetLength = 16;
// Default Calendar database path
_LIT( KCalendarDatabaseFilePath, "c:calendar" );

// -----------------------------------------------------------------------------
// CCalenMultiCalUtil::GetNextAvailableOffsetL
// Gets the next available calendar unique value
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CCalenMultiCalUtil::GetNextAvailableOffsetL()
    {
    TRACE_ENTRY_POINT

    TBuf<KOffsetLength> offSetBuf;

    CRepository* cenRep = CRepository::NewLC(KCRUidCalendar);
    User::LeaveIfError(cenRep->Get(KCalendarAvailableValue, offSetBuf));

    TUint availableOffsetValue = 0;
    TLex lex(offSetBuf);
    lex.Val(availableOffsetValue, EDecimal);

    TUint nextAvailableOffset = availableOffsetValue + KCalendarOffSet;

    offSetBuf.Zero(); //Reset
    offSetBuf.AppendFormat(_L("%u"), nextAvailableOffset);

    User::LeaveIfError(cenRep->Set(KCalendarAvailableValue, offSetBuf));

    CleanupStack::PopAndDestroy(cenRep);
    TRACE_EXIT_POINT
    return availableOffsetValue;
    }

// -----------------------------------------------------------------------------
// CCalenMultiCalUtil::GetNextAvailableCalFileL
// Gets the next available calendar filename
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CCalenMultiCalUtil::GetNextAvailableCalFileL()
	{
	TRACE_ENTRY_POINT;

	TInt nextAvailableCalNumber(0);
	// get the next available calendar value from cenrep and
	// append it to c:calendar
	CRepository* cenRep = CRepository::NewL(KCRUidCalendar);
	CleanupStack::PushL(cenRep);
	User::LeaveIfError(cenRep->Get(KCalendarFileNumber,
			nextAvailableCalNumber));
	nextAvailableCalNumber++;
	User::LeaveIfError(cenRep->Set(KCalendarFileNumber,
			nextAvailableCalNumber));
	CleanupStack::PopAndDestroy(cenRep);
	
	HBufC* calFileName = HBufC::NewL(KMaxFileName);
	TPtr fileNameptr(calFileName->Des());
	fileNameptr.Append(KCalendarDatabaseFilePath);
	fileNameptr.AppendNum(nextAvailableCalNumber);

	TRACE_EXIT_POINT;
	// ownership of calFileName is transferred to the caller
	return calFileName;
	}

// EOF
