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
* Source file for the class AlarmClient
*
*/

// System includes
#include <e32cmn.h>
#include <QDebug>

// User includes
#include "alarmclient.h"
#include "alarmlistener.h"

/*!
	\class AlarmClient
 */

/*!
	Default constructor.
 */
AlarmClient::AlarmClient(QObject* parent)
:QObject(parent),
mListener(0)
{
	qDebug("clock: AlarmClient::AlarmClient() -->");

	// Connect to the alarm server.
	User::LeaveIfError(mAlarmSrvSession.Connect());

	qDebug("clock: AlarmClient::AlarmClient() - Connection done, looks ok.");

	// Construct the listener, but do not start it.
	mListener = new AlarmListener(this, mAlarmSrvSession);

	qDebug("clock: AlarmClient::AlarmClient() <--");
}

/*!
	Destructor.
 */
AlarmClient::~AlarmClient()
{
	qDebug("clock: AlarmClient::~AlarmClient() -->");

	if (mListener) {
		mListener->stop();
		delete mListener;
		mListener = 0;
	}
	mAlarmSrvSession.Close();

	qDebug("clock: AlarmClient::~AlarmClient() <--");
}

/*!
	Returns the list of alarms currently set on the phone.

	\param alarmList Reference to a list of alarms.
 */
void AlarmClient::getAlarmList(QList<AlarmInfo>& alarmList)
{
	qDebug() << "clock: AlarmClient::getAlarmList -->";

	// This will hold the alarm ids returned from alarm server.
	RArray<TAlarmId> alarmIdArray;
	AlarmInfo alarmInfo;
	TASShdAlarm tempSharedAlarm;

	// Get the ids from alarmserver.
	mAlarmSrvSession.GetAlarmIdListForCategoryL(KAlarmClockOne, alarmIdArray);

	// Get the alarm count
	int alarmCount = alarmIdArray.Count();

	for (int index(0); index < alarmCount; index++) {
		// Fetch the details of the alarm.
		int error = mAlarmSrvSession.GetAlarmDetails(
				alarmIdArray[index], tempSharedAlarm);

		if (KErrNone == error) {
			// Alarm description
			alarmInfo.alarmDesc = QString::fromUtf16(
					tempSharedAlarm.Message().Ptr(),
					tempSharedAlarm.Message().Length());

			// Original expiry time
			alarmInfo.origAlarmTime.setHMS(
					tempSharedAlarm.OriginalExpiryTime().DateTime().Hour(),
					tempSharedAlarm.OriginalExpiryTime().DateTime().Minute(),
					tempSharedAlarm.OriginalExpiryTime().DateTime().Second());

			// Next due time
			alarmInfo.nextDueTime.setHMS(
					tempSharedAlarm.NextDueTime().DateTime().Hour(),
					tempSharedAlarm.NextDueTime().DateTime().Minute(),
					tempSharedAlarm.NextDueTime().DateTime().Second());

			// Alarm day
			alarmInfo.alarmDateTime.setYMD(
					tempSharedAlarm.OriginalExpiryTime().DateTime().Year(),
					tempSharedAlarm.OriginalExpiryTime().DateTime().Month()+1,
					tempSharedAlarm.OriginalExpiryTime().DateTime().Day()+1);

			// The alarm status
			if (EAlarmStatusEnabled == tempSharedAlarm.Status()) {
				alarmInfo.alarmStatus = Enabled;
			} else {
				alarmInfo.alarmStatus = Disabled;
			}

			// The alarm state
			setAlarmState(tempSharedAlarm.State(), alarmInfo.alarmState);

			// The repeat type
			setAlarmRepeatType(
					tempSharedAlarm.RepeatDefinition(), alarmInfo.repeatType);

			// The alarm id
			alarmInfo.alarmId = alarmIdArray[index];
			alarmList.append(alarmInfo);
		}
	}
	// Cleanup.
	alarmIdArray.Close();

	qDebug() << "clock: AlarmClient::getAlarmList <--";
}

/*!
	Sets a new alarm with the alarmserver.
 */
void AlarmClient::setAlarm(AlarmInfo& alarmInfo)
{
	qDebug() << "clock: AlarmClient::setAlarm -->";

	// Get the current home time
	TTime homeTime;
	homeTime.HomeTime();
	TDateTime homeDateTime = homeTime.DateTime();

	// Set the alarm time
	TDateTime alarmDateTime = TDateTime(homeDateTime.Year(),
			TMonth(homeDateTime.Month()),
			homeDateTime.Day(),
			alarmInfo.origAlarmTime.hour(),
			alarmInfo.origAlarmTime.minute(),
			alarmInfo.origAlarmTime.second(),
			0);
	TTime alarmTime(alarmDateTime);

	TInt dayIndex = 0;
	TInt currentDayIndex = 0;

	// Decide the day for 'Once' and 'Repeat Weekly' alarms.
	switch (alarmInfo.repeatType) {
		case Once:
		case Weekly:
			currentDayIndex = homeTime.DayNoInWeek();

			if (currentDayIndex < alarmInfo.alarmDay) {
				dayIndex = alarmInfo.alarmDay - currentDayIndex;
			} else if (currentDayIndex > alarmInfo.alarmDay) {
				dayIndex = 7 - (currentDayIndex - alarmInfo.alarmDay);
			} else {
				if (alarmTime > homeTime) {
					dayIndex = 0;
				} else {
					dayIndex = 7;
				}
			}
			break;

		default:
			break;
	}

	// Add corresponding no. of days based on the type of alarm.
	alarmTime += TTimeIntervalDays(dayIndex);

	// Set the details of the alarm and set it thru the alarm server.
	TASShdAlarm alarmDetails;
	alarmDetails.Category() = KAlarmClockOne;
	setAlarmRepeatType(alarmInfo.repeatType, alarmDetails.RepeatDefinition());
	alarmDetails.OriginalExpiryTime() = alarmTime;
	alarmDetails.NextDueTime() = alarmTime;
	alarmDetails.Message() = alarmInfo.alarmDesc.utf16();
	alarmDetails.SetWakeup(true);
	alarmDetails.ClientData2() = alarmInfo.volumeStatus;

	TInt error(mAlarmSrvSession.AlarmAdd(alarmDetails));

	if( KErrNone == error ) {
		mAlmObj = alarmDetails;
		mAlarmIdDetail = alarmDetails.Id();
		alarmInfo.alarmId = alarmDetails.Id();
		// The alarm status.
		if (EAlarmStatusEnabled == alarmDetails.Status()) {
			alarmInfo.alarmStatus = Enabled;
		} else {
			alarmInfo.alarmStatus = Disabled;
		}
		TDateTime nextDueDateTime(alarmDetails.NextDueTime().DateTime());
		QTime nextDueTime(nextDueDateTime.Hour(),nextDueDateTime.Minute(),
				nextDueDateTime.Second(),nextDueDateTime.MicroSecond());
		alarmInfo.nextDueTime = nextDueTime;

		QDate alarmDate(
		nextDueDateTime.Year(),nextDueDateTime.Month()+1,nextDueDateTime.Day()+1);

		alarmInfo.alarmDateTime = alarmDate;
	}

	qDebug() << "clock: AlarmClient::setAlarm <--";
}

void AlarmClient::setAlarmState(TAlarmState state, AlarmState& alarmState)
{
	qDebug() << "clock: AlarmClient::setAlarmState -->";

	switch (state) {
		case EAlarmStateInPreparation:
			alarmState = InPreparation;
			break;
		case EAlarmStateQueued:
			alarmState = Queued;
			break;
		case EAlarmStateSnoozed:
			alarmState = Snoozed;
			break;
		case EAlarmStateWaitingToNotify:
			alarmState = WaitingToNotify;
			break;
		case EAlarmStateNotifying:
			alarmState = Notifying;
			break;
		case EAlarmStateNotified:
			alarmState = Notified;
			break;
		default:
			break;
	}

	qDebug() << "clock: AlarmClient::setAlarmState <--";
}

void AlarmClient::setAlarmState(AlarmState state, TAlarmState& alarmState)
{
	qDebug() << "clock: AlarmClient::setAlarmState -->";

	switch (state) {
		case InPreparation:
			alarmState = EAlarmStateInPreparation;
			break;
		case Queued:
			alarmState = EAlarmStateQueued;
			break;
		case Snoozed:
			alarmState = EAlarmStateSnoozed;
			break;
		case WaitingToNotify:
			alarmState = EAlarmStateWaitingToNotify;
			break;
		case Notifying:
			alarmState = EAlarmStateNotifying;
			break;
		case Notified:
			alarmState = EAlarmStateNotified;
			break;
		default:
			break;
	}

	qDebug() << "clock: AlarmClient::setAlarmState <--";
}

void AlarmClient::setAlarmRepeatType(
		TAlarmRepeatDefinition repeat, AlarmRepeatType& repeatType)
{
	qDebug() << "clock: AlarmClient::setAlarmRepeatType -->";

	switch (repeat) {
		case EAlarmRepeatDefintionRepeatOnce:
			repeatType = Once;
			break;
		case EAlarmRepeatDefintionRepeatDaily:
			repeatType = Daily;
			break;
		case EAlarmRepeatDefintionRepeatWorkday:
			repeatType = Workday;
			break;
		case EAlarmRepeatDefintionRepeatWeekly:
			repeatType = Weekly;
			break;
		default:
			break;
	}

	qDebug() << "clock: AlarmClient::setAlarmRepeatType <--";
}

void AlarmClient::setAlarmRepeatType(
		AlarmRepeatType repeat, TAlarmRepeatDefinition& repeatType)
{
	qDebug() << "clock: AlarmClient::setAlarmRepeatType -->";

	switch (repeat) {
		case Once:
			repeatType = EAlarmRepeatDefintionRepeatOnce;
			break;
		case Daily:
			repeatType = EAlarmRepeatDefintionRepeatDaily;
			break;
		case Workday:
			repeatType = EAlarmRepeatDefintionRepeatWorkday;
			break;
		case Weekly:
			repeatType = EAlarmRepeatDefintionRepeatWeekly;
			break;
		default:
			break;
	}

	qDebug() << "clock: AlarmClient::setAlarmRepeatType <--";
}

void AlarmClient::deleteAlarm(int alarmId)
{
	qDebug() << "clock: AlarmClient::deleteAlarm -->";

	// Request the alarmserver to delete the alarm.
	mAlarmSrvSession.AlarmDelete(alarmId);

	qDebug() << "clock: AlarmClient::deleteAlarm <--";
}

int AlarmClient::getAlarmInfo(int alarmId, AlarmInfo& alarmInfo)
{
	qDebug() << "clock: AlarmClient::getAlarmInfo -->";

	TASShdAlarm tempSharedAlarm;

	// Get the requested alarm info from the alarm server.
	int error = mAlarmSrvSession.GetAlarmDetails(alarmId, tempSharedAlarm);

	if (KErrNone == error) {
		// Alarm description
		alarmInfo.alarmDesc = QString::fromUtf16(
				tempSharedAlarm.Message().Ptr(),
				tempSharedAlarm.Message().Length());

		// Original expiry time
		alarmInfo.origAlarmTime.setHMS(
				tempSharedAlarm.OriginalExpiryTime().DateTime().Hour(),
				tempSharedAlarm.OriginalExpiryTime().DateTime().Minute(),
				tempSharedAlarm.OriginalExpiryTime().DateTime().Second());

		// Next due time
		alarmInfo.nextDueTime.setHMS(
				tempSharedAlarm.NextDueTime().DateTime().Hour(),
				tempSharedAlarm.NextDueTime().DateTime().Minute(),
				tempSharedAlarm.NextDueTime().DateTime().Second());

		// Alarm day
		alarmInfo.alarmDateTime.setYMD(
				tempSharedAlarm.OriginalExpiryTime().DateTime().Year(),
				tempSharedAlarm.OriginalExpiryTime().DateTime().Month()+1,
				tempSharedAlarm.OriginalExpiryTime().DateTime().Day()+1);

		// The alarm status
		if (EAlarmStatusEnabled == tempSharedAlarm.Status()) {
			alarmInfo.alarmStatus = Enabled;
		} else {
			alarmInfo.alarmStatus = Disabled;
		}

		// The alarm state
		setAlarmState(
				tempSharedAlarm.State(), alarmInfo.alarmState);

		// The repeat type
		setAlarmRepeatType(
				tempSharedAlarm.RepeatDefinition(), alarmInfo.repeatType);

		// The alarm id
		alarmInfo.alarmId = alarmId;

		// Alarm volume status.
		if(tempSharedAlarm.ClientData2()) {
			alarmInfo.volumeStatus = AlarmVolumeOn;
		} else {
			alarmInfo.volumeStatus = AlarmVolumeOff;
		}
	}

	qDebug() << "clock: AlarmClient::getAlarmInfo <--";

	return error;
}

void AlarmClient::toggleAlarmStatus(int alarmId, int alarmStatus)
{
	qDebug() << "clock: AlarmClient::toggleAlarmStatus -->";

	mAlarmSrvSession.SetAlarmStatus(alarmId, (TAlarmStatus)alarmStatus);

	qDebug() << "clock: AlarmClient::toggleAlarmStatus <--";
}

void AlarmClient::startListener()
{
	qDebug("clock: AlarmClient::startListener() -->");

	mListener->start();

	qDebug("clock: AlarmClient::startListener() <--");
}

void AlarmClient::stopListener()
{
	qDebug("clock: AlarmClient::stopListener() -->");

	mListener->stop();

	qDebug("clock: AlarmClient::stopListener() <--");
}

void AlarmClient::notifyChange(int alarmId)
{
	qDebug() << "clock: AlarmClient::notifyChange -->";

	emit alarmStateChanged(alarmId);

	qDebug() << "clock: AlarmClient::notifyChange <--";
}

// End of file	--Don't remove this.
