/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   missed alarm class description
*
*/



// Include files
#include "missedalarm.h"
#include "alarmenginesdebug.h"

// ----------------------------------------------------------------------------
// CMissedAlarm::NewL
// 1st phase of construction
// ----------------------------------------------------------------------------
//
EXPORT_C CMissedAlarm* CMissedAlarm::NewL( const TInt aLuid, const TTime aInstanceTime, const TDesC& aCalFileName )
	{
	TRACE_ENTRY_POINT;
	
	CMissedAlarm* self=new (ELeave) CMissedAlarm();
	CleanupStack::PushL(self);
	self->ConstructL(aLuid, aInstanceTime, aCalFileName);
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}

// ----------------------------------------------------------------------------
// CMissedAlarm::NewL
// Class destructor.Frees all resources used by this class
// ----------------------------------------------------------------------------
//
CMissedAlarm::~CMissedAlarm()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CMissedAlarm::CMissedAlarm
// Default construction
// ----------------------------------------------------------------------------
//
CMissedAlarm::CMissedAlarm()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CMissedAlarm::NewL
// Second stage constructor
// ----------------------------------------------------------------------------
//
void CMissedAlarm::ConstructL( const TInt aLuid, const TTime aInstanceTime, const TDesC& aCalFileName )
	{
	TRACE_ENTRY_POINT;
	
	iLuid = aLuid;
	iInstanceTime = aInstanceTime;
	iCalFileName = aCalFileName;
	
	TRACE_EXIT_POINT;
    }

// End of file
