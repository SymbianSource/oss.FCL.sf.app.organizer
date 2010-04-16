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

// System includes
#include <QDebug>

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
	qDebug() << "clock: ClockAppControllerIfImpl::ClockAppControllerIfImpl -->";

	// Construct the timezone client.
	mTimeZoneClient = new TimezoneClient(this);
	// Construct the settings utility object.
	mSettingsUtility = new SettingsUtility(this);
	// Construct the alarm client object.
	mAlarmClient = new AlarmClient(this);

	qDebug() << "clock: ClockAppControllerIfImpl::ClockAppControllerIfImpl <--";
}

/*!
	Destructor.
 */
ClockAppControllerIfImpl::~ClockAppControllerIfImpl()
{
	qDebug() << "clock: ClockAppControllerIfImpl::~ClockAppControllerIfImpl -->";

	if (mTimeZoneClient) {
		delete mTimeZoneClient;
		mTimeZoneClient = 0;
	}
	if (mSettingsUtility) {
		delete mSettingsUtility;
		mSettingsUtility = 0;
	}
	if(mAlarmClient) {
		delete mAlarmClient;
		mAlarmClient = 0;
	}

	qDebug() << "clock: ClockAppControllerIfImpl::~ClockAppControllerIfImpl <--";
}


/*!
	From ClockAppControllerIf.
	Returns a pointer to timezoneClient.

	\return TimezoneClient* Pointer to TimezoneClient object.
	\sa ClockAppControllerIf
 */
TimezoneClient* ClockAppControllerIfImpl::timezoneClient()
{
	qDebug() << "clock: ClockAppControllerIfImpl::timezoneClient -->";

	qDebug() << "clock: ClockAppControllerIfImpl::timezoneClient <--";

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
	qDebug() << "clock: ClockAppControllerIfImpl::settingsUtility -->";

	qDebug() << "clock: ClockAppControllerIfImpl::settingsUtility <--";

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
	qDebug() << "clock: ClockAppControllerIfImpl::alarmClient -->";

	qDebug() << "clock: ClockAppControllerIfImpl::alarmClient <--";

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
	qDebug() << "clock: ClockAppControllerIfImpl::switchToView -->";

	mAppController->mViewManager->showView(viewId);

	qDebug() << "clock: ClockAppControllerIfImpl::switchToView <--";
}

// End of file	--Don't remove this.
