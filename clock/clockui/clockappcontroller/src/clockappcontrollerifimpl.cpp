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
* This file contains the ClockAppControllerIfImpl class definition.
*
*/

// User includes
#include "clockappcontrollerifimpl.h"
#include "clockappcontroller.h"
#include "clockviewmanager.h"
#include "timezoneclient.h"
#include "settingsutility.h"
#include "alarmclient.h"

/*!
	\class ClockAppControllerIfImpl

	This class implements the ClockAppControllerIf interface which is used
	by the views and other componets to access the services of
	clockviewmanager.
 */

/*!
	Constructor.

	\param controller Pointer to an object of ClockAppController.
	\param parent QObject pointer.
 */
ClockAppControllerIfImpl::ClockAppControllerIfImpl(
		ClockAppController *controller, QObject *parent)
:QObject(parent),
 mAppController(controller)
{
	// Construct the timezone client.
	mTimeZoneClient = TimezoneClient::getInstance();
	// Construct the settings utility object.
	mSettingsUtility = new SettingsUtility(this);
	// Construct the alarm client object.
	mAlarmClient = new AlarmClient(this);
}

/*!
	Destructor.
 */
ClockAppControllerIfImpl::~ClockAppControllerIfImpl()
{
	if (!mTimeZoneClient->isNull()) {
		mTimeZoneClient->deleteInstance();
	}
	if (mSettingsUtility) {
		delete mSettingsUtility;
		mSettingsUtility = 0;
	}
	if(mAlarmClient) {
		delete mAlarmClient;
		mAlarmClient = 0;
	}
}


/*!
	From ClockAppControllerIf.
	Returns a pointer to timezoneClient.

	\return TimezoneClient* Pointer to TimezoneClient object.
	\sa ClockAppControllerIf
 */
TimezoneClient* ClockAppControllerIfImpl::timezoneClient()
{
	return mTimeZoneClient;
}

/*!
	From ClockAppControllerIf.
	Returns a pointer to SettingsUtility.

	\return SettingsUtility* Pointer to SettingsUtility object.
	\sa ClockAppControllerIf
 */
SettingsUtility* ClockAppControllerIfImpl::settingsUtility()
{
	return mSettingsUtility;
}

/*!
	From ClockAppControllerIf.
	Returns a pointer to AlarmClient.

	\return AlarmClient* Pointer to AlarmClient object.
	\sa ClockAppControllerIf
 */
AlarmClient* ClockAppControllerIfImpl::alarmClient()
{
	return mAlarmClient;
}

/*!
	From ClockAppControllerIf.
	Issues a request to ClockViewManager to switch to a given view.

	\param viewId The id of the view to be switched to.
	\sa ClockAppControllerIf
 */
void ClockAppControllerIfImpl::switchToView(ClockViews viewId)
{
	mAppController->mViewManager->showView(viewId);
}

// End of file	--Don't remove this.
