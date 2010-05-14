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
* Description:
* Source file for the class AlarmListener
*
*/

// User includes
#include "alarmlistener.h"
#include "alarmclient.h"

AlarmListener::AlarmListener(AlarmClient* client, RASCliSession& session)
:CActive(EPriorityStandard),
mClient(client),
mSession(session)
{
		// Nothing to do.
}

AlarmListener::~AlarmListener()
{
	if (IsActive()) {
		Cancel();
	}

}

void AlarmListener::start()
{
	// Add the AO to the scheduler.
	CActiveScheduler::Add(this);

	// Subscrive for async notification from alarm server.
	mSession.NotifyChange(iStatus, mAlarmId);

	// Set the AO active.
	SetActive();

}

void AlarmListener::stop()
{
	if (IsActive()) {
		Cancel();
	}
}

void AlarmListener::RunL()
{
	if (iStatus != KRequestPending) {
		// We get notification for various changes with the alarm server.
		// Only the required Events are used to emit a signal.
		if (iStatus == EAlarmChangeEventAlarmDeletion
				|| iStatus == EAlarmChangeEventTimerExpired
				|| iStatus == EAlarmChangeEventState
				|| iStatus == EAlarmChangeEventStatus) {
			// Notify the client.
			mClient->notifyChange(mAlarmId);
		}

		// Cancel and outstanding requests.
		mSession.NotifyChangeCancel();
		// Re-subscribe for change.
		iStatus = KRequestPending;
		mSession.NotifyChange(iStatus, mAlarmId);

		SetActive();
	}
}

void AlarmListener::DoCancel()
{
	// Cancel async request.
	mSession.NotifyChangeCancel();

}

// End of file
