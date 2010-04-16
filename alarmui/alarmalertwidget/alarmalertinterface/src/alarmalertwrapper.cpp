/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
// alarmalertwrapper.h

// User includes
#include "alarmalertwrapper.h"
#include "alarmalertwidget.h"

// ---------------------------------------------------------
// AlarmAlert::AlarmAlert
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C AlarmAlert::AlarmAlert(AlarmAlertObserver *observer)
{
	// Construct the client interface
    mAlarmWidget = new AlarmAlertWidget(observer);
}

// ---------------------------------------------------------
// AlarmAlert::~AlarmAlert
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C AlarmAlert::~AlarmAlert()
{
    // Cleanup
	if (mAlarmWidget) {
		delete mAlarmWidget;
	}
}

// ---------------------------------------------------------
// AlarmAlert::showAlarmAlert
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C bool AlarmAlert::showAlarmAlert(SAlarmInfo *alarmInfo)
{
    // Forward the call to the client
	return mAlarmWidget->showAlarmDialog(alarmInfo);
}

// ---------------------------------------------------------
// AlarmAlert::dismissAlarmAlert
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C bool AlarmAlert::dismissAlarmAlert()
{
    // Forward the call to the client
	return mAlarmWidget->dismissAlarmDialog();
}

// ---------------------------------------------------------
// AlarmAlert::updateAlarmAlert
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C bool AlarmAlert::updateAlarmAlert(SAlarmInfo *alarmInfo)
{
    // Forward the call to the client
	return mAlarmWidget->updateAlarmDialog(alarmInfo);
}
