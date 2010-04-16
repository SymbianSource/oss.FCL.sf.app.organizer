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
* Definition file for class ClockAlarmListModel.
*
*/

// System includes
#include <QDebug>
#include <QTimer>

#include <QStandardItemModel>
#include <QStandardItem>
#include <HbIcon>

// User includes
#include "clockalarmlistmodel.h"
#include "alarmclient.h"
#include "timezoneclient.h"
#include "clockalarmlistitemprototype.h"
#include "clockappcontrollerif.h"
#include "settingsutility.h"

// Constants
const int KOneHourInMinute(60);
const int KOneMinuteInSecons(60);
const int KSecondsInOneDay(24 * 60 * 60);

/*!
	\class ClockAlarmListModel

	This is the source model class for the clock main view. This owns
	a QStandardItemModel which is a model for the data containing alarms.
 */

/*!
	Constructor.

	\param parent Parent of type QObject.
 */
ClockAlarmListModel::ClockAlarmListModel(
		ClockAppControllerIf &controllerIf, QObject *parent)
:QObject(parent),
 mSourceModel(0),
 mAppControllerIf(controllerIf)
{
	qDebug() << "clock: ClockAlarmListModel::ClockAlarmListModel -->";

	// Construct the source model.
	mSourceModel = new QStandardItemModel(0, 1, this);
	
	// Register to listen to the alarm state changes.
	mAppControllerIf.alarmClient()->startListener();
	connect(
			mAppControllerIf.alarmClient(), SIGNAL(alarmStateChanged(int)),
			this, SLOT(updateSourceModel(int)));
	
	connect(
			mAppControllerIf.timezoneClient(), SIGNAL(timechanged()),
			this, SLOT(updateRemainingTime()));
	// Start a timer. For updating the remaining alarm time.
	mTickTimer = new QTimer(this);
	connect(
			mTickTimer, SIGNAL(timeout()),
			this, SLOT(updateRemainingTime()));
	
	qDebug() << "clock: ClockAlarmListModel::ClockAlarmListModel <--";
}

/*!
	Destructor.
 */
ClockAlarmListModel::~ClockAlarmListModel()
{
	qDebug() << "clock: ClockAlarmListModel::~ClockAlarmListModel -->";

	if (mSourceModel) {
		delete mSourceModel;
		mSourceModel = 0;
	}

	qDebug() << "clock: ClockAlarmListModel::~ClockAlarmListModel <--";
}

/*!
	Populates the source model & starts the timer for 1 minute.
 */
void ClockAlarmListModel::populateModel()
{
	// Populate the model in a different thread.
	QTimer::singleShot(1, this, SLOT(populateSourceModel()));
}
/*!
	Returns the source model to be used with clock main view.

	\return QAbstractItemModel
	\sa QAbstractItemModel, HbListView.
 */
QAbstractItemModel *ClockAlarmListModel::sourceModel()
{
	qDebug() << "clock: ClockAlarmListModel::sourceModel -->";

	qDebug() << "clock: ClockAlarmListModel::sourceModel <--";

	return mSourceModel;
}

/*!
	Populates the source model.
 */
void ClockAlarmListModel::populateSourceModel()
{
	qDebug() << "clock: ClockAlarmListModel::populateSourceModel -->";

	// Clear the model if it has any data already.
	mSourceModel->clear();
	mSourceModel->setColumnCount(1);
	
	QList<AlarmInfo> alarmInfoList;
	
	// Get the list of pending clock alarms.
	mAppControllerIf.alarmClient()->getAlarmList(alarmInfoList);
	
	// First create a list of alarms with the order of active & inactive alarms.
	AlarmInfo alarminfo;
	for (int index = 0; index < alarmInfoList.count(); index++) {
		alarminfo = alarmInfoList[index];
		if (Enabled == alarminfo.alarmStatus) {
			// Append alarm into the source model.
			appendAlarmToModel(alarminfo);
		}
	}
	for (int index = 0; index < alarmInfoList.count(); index++) {
		alarminfo = alarmInfoList[index];
		if (Disabled == alarminfo.alarmStatus) {
			// Append alarm into the source model.
			appendAlarmToModel(alarminfo);
		}
	}
	
	if (0 < getActiveAlarmCount() && !mTickTimer->isActive()) {
		// Start the Timer for 1 minute.
		mTickTimer->start(60000 - 1000 * QTime::currentTime().second());
	}
	qDebug() << "clock: ClockAlarmListModel::populateSourceModel <--";
}

/*!
	Calculates the remaining time for the given alarm.

	\param alarmInfo AlarmInfo containing the active alarm.
 */
QString ClockAlarmListModel::calculateRemainingTime(AlarmInfo alarmInfo)
{
	qDebug() << "clock: ClockAlarmListModel::calculateRemainingTime -->";

	QDateTime currentDateTime = QDateTime::currentDateTime();
	QDateTime alarmTime = QDateTime(
			alarmInfo.alarmDateTime, alarmInfo.nextDueTime);
	QString timeNote;

	// TODO: method getRemainingSeconds is used since secsTo API is
	// not returning the proper value.
	// int remianSecs = currentDateTime.secsTo(alarmTime);
	int remainSecs = getRemainingSeconds(alarmTime);
	QDateTime timeLeft(QDate(2009,1,1), QTime(0,0)); //do -1 for day always.

	QDateTime temp = timeLeft.addSecs(remainSecs);

	int hourleft = temp.time().hour();
	int minleft = temp.time().minute();
	int secleft = temp.time().second();
	int dayleft = temp.date().day() - 1;

	if (dayleft >= 1) {
		hourleft += (dayleft * 24);
	}
	QString formatTimeNote;
	
	if (hourleft <= 48) {
		switch (hourleft) {
			case 1: {
				if (1 == minleft) {
					formatTimeNote =
						hbTrId("txt_clock_main_view_setlabel_in_1hr_2min");
				} else {
					formatTimeNote =
						hbTrId("txt_clock_main_view_setlabel_in_1hr_2mins");
				}
			}
			break;

			default: {
				if (1 == minleft) {
					formatTimeNote =
						hbTrId("txt_clock_main_view_setlabel_in_1hrs_2min");
				} else {
					formatTimeNote =
						hbTrId("txt_clock_main_view_setlabel_in_1hrs_2mins");
				}
			}
		}
		timeNote = formatTimeNote.arg(
				QString::number(hourleft), QString::number(minleft));
	} else {
		formatTimeNote = hbTrId("txt_clock_main_view_setlabel_in_1days");
		timeNote = formatTimeNote.arg(QString::number(dayleft));
	}
	qDebug() << "clock: ClockAlarmListModel::calculateRemainingTime <--";

	return timeNote;
}

/*!
	Gets remaining seconds to the alarm time from current date time.

	\param alarmDateTime reference to the alarm date time.
	\return int returns the remaining seconds calculated.
 */
int ClockAlarmListModel::getRemainingSeconds(QDateTime &alarmDateTime)
{
	int remainingSeconds;
	QDateTime currentDateTime = QDateTime::currentDateTime();

	int currentTimeInSec =
			(currentDateTime.time().hour()
					* KOneHourInMinute * KOneMinuteInSecons)
			+ (currentDateTime.time().minute() * KOneMinuteInSecons)
			+ currentDateTime.time().second();

	int alarmTimeInSec =
			alarmDateTime.time().hour() * KOneHourInMinute * KOneMinuteInSecons
			+ alarmDateTime.time().minute() * KOneMinuteInSecons
			+ alarmDateTime.time().second();

	int dayInterval =
			alarmDateTime.date().day() - currentDateTime.date().day();

	if (dayInterval) {
		alarmTimeInSec += (dayInterval*KSecondsInOneDay);
	}

	remainingSeconds = alarmTimeInSec - currentTimeInSec;
	return remainingSeconds;
}

/*!
	Slot which Updates the source model with the given alarm `id'.
	The alarm could be newly added or deleted or have undergone a status change
	or have just been modified.

	\param id of alarm, identifies the alarm which was changed.
 */
void ClockAlarmListModel::updateSourceModel(int alarmId)
{
	qDebug() << "clock: ClockAlarmListModel::updateSourceModel -->";
	
	Q_UNUSED(alarmId)
	int alarmInfoCount;
	int modelCount;
	bool newAlarm = false;
	bool alarmDeleted = false;
	
	// Get the list of pending clock alarms from server.
	QList<AlarmInfo> alarmInfoList;
	mAppControllerIf.alarmClient()->getAlarmList(alarmInfoList);
	modelCount = mSourceModel->rowCount();
	
	// First create a list of alarms with the order of active & inactive alarms.
	QList<AlarmInfo> alarmDisplayList;
	AlarmInfo alarminfo;
	for (int index = 0; index < alarmInfoList.count(); index++) {
		alarminfo = alarmInfoList[index];
		if (Enabled == alarminfo.alarmStatus
				&& Notified != alarminfo.alarmState) {
			// Append only active alarms to the alarmDisplayList.
			alarmDisplayList.append(alarminfo);
		}
	}
	for (int index = 0; index < alarmInfoList.count(); index++) {
		alarminfo = alarmInfoList[index];
		if (Disabled == alarminfo.alarmStatus) {
			// Append only active alarms to the alarmDisplayList.
			alarmDisplayList.append(alarminfo);
		}
	}
	alarmInfoCount = alarmDisplayList.count();
	
	AlarmInfo alarmInfo;
	int index = 0;
	
	if (alarmInfoCount > modelCount) {
		// If alarm count from server is more than in model then, a new alarm
		// has been queued into server.
		newAlarm = true;
	} else if (alarmInfoCount < modelCount) {
		// If alarm count from server is less than in model then, an alarm
		// has been deleted from server.
		alarmDeleted = true;
	}
	if (0 < modelCount) {
		int maxCount = 0;
		if (newAlarm) {
			maxCount = modelCount;
		} else if (alarmDeleted) {
			maxCount = alarmInfoCount;
		} else {
			maxCount = alarmInfoCount;
		}
		// Update the alarm details.
		for (; index < maxCount; index++) {
			alarmInfo = alarmDisplayList[index];
			QModelIndex modelIndex = mSourceModel->index(index,0);
			if (modelIndex.isValid()) {
				updateAlarmDetails(modelIndex, alarmInfo);
			}
		}
	}
	if (newAlarm) {
		emit changeAlarmListDisplay();
		alarmInfo = alarmDisplayList[index];
		appendAlarmToModel(alarmInfo);
	}
	if (alarmDeleted) {
		QModelIndex modelIndex = mSourceModel->index(index, 0);
		if (modelIndex.isValid()) {
			mSourceModel->removeRow(modelIndex.row());
		}
		// If alarm count is "0" then hide the alarmlist in the view.
		if (0 == mSourceModel->rowCount()) {
			emit changeAlarmListDisplay();
		}
	}
	if (0 < getActiveAlarmCount()) {
		if (!mTickTimer->isActive()) {
			// Start the Timer for 1 minute.
			mTickTimer->start(60000 - 1000 * QTime::currentTime().second());
		}
	} else {
		if (mTickTimer->isActive()) {
			// Stop the timer if no more active alarms are present.
			mTickTimer->stop();
		}
	}

	qDebug() << "clock: ClockAlarmListModel::updateSourceModel <--";
}

/*!
	Slot which Updates the remaining time field in the alarms list for all the
	alarms.
	
 */
void ClockAlarmListModel::updateRemainingTime()
{
	// Get the list of pending clock alarms from server.
	AlarmInfo alarmInfo;
	for (int row = 0; row < mSourceModel->rowCount(); row++) {
		// Get the data for the alarm.
		QList<QVariant> alarmData =
				mSourceModel->index(row, 0).data(AlarmDetails).toList();
		int alarmId = alarmData.at(0).value<int>();

		int error = mAppControllerIf.alarmClient()->getAlarmInfo(
				alarmId, alarmInfo);
		if (-1 != error) {
			if (Enabled == alarmInfo.alarmStatus) {
				// Get the model index.
				QModelIndex mdlIndex = mSourceModel->index(row, 0);
				if (Notified != alarmInfo.alarmState) {
					QStringList displayStringList =
							getDisplayStringListforAlarmItem(alarmInfo);
					mSourceModel->setData(
							mdlIndex, displayStringList, Qt::DisplayRole);
				}
			}
		}
	}
	// Start the Timer for 1 minute.
	mTickTimer->start(60000);
}

/*!
	Returns the stringList to be set for "displayRole" for the alarm item.

	\param alarmInfo AlarmInfo containing the alarm.
	
	\return QStringList contains the stringList to be set.
 */
QStringList ClockAlarmListModel::getDisplayStringListforAlarmItem(
		AlarmInfo alarmInfo)
{
	qDebug() << "clock: ClockAlarmListModel::getDisplayStringforAlarmItem -->";
	
	QStringList displayStringList;
	QString timeString;
	if (Snoozed == alarmInfo.alarmState) {
		timeString = alarmInfo.nextDueTime.toString(
				mAppControllerIf.settingsUtility()->timeFormatString());
	} else {
		timeString = alarmInfo.origAlarmTime.toString(
				mAppControllerIf.settingsUtility()->timeFormatString());
	}
	
	QStringList stringList = timeString.split(" ");
	QString time;
	time.append(stringList[0]);
	if (stringList.count() > 1) {
		time.append("\n");
		time.append(stringList[1]);
	}
	displayStringList.append(time);
	
	if (Enabled == alarmInfo.alarmStatus) {
		QString remainingTime = calculateRemainingTime(alarmInfo);
		displayStringList.append(remainingTime);
	} else {
		displayStringList.append(QString(" "));
	}

	QString alarmDescription = alarmInfo.alarmDesc;
	displayStringList.append(alarmDescription);

	QString repeatTypeString;
	// The repeat type string.
	if (Daily == alarmInfo.repeatType) {
		repeatTypeString.append(hbTrId("txt_clock_main_view_dblist_daily"));
	} else if (Workday == alarmInfo.repeatType) {
		repeatTypeString.append(
		    hbTrId("txt_clock_main_view_dblist_daily_val_workdays"));
	} else if (Weekly == alarmInfo.repeatType) {
		repeatTypeString.append(hbTrId("txt_clock_main_view_setlabel_every_1"));
		repeatTypeString =
			repeatTypeString.arg(alarmInfo.alarmDateTime.toString("dddd"));
	} else {
		if (QDate::currentDate() == alarmInfo.alarmDateTime) {
			repeatTypeString.append(
			    hbTrId("txt_clock_main_view_dblist_daily_val_today"));
		} else {
			repeatTypeString.append(
					alarmInfo.alarmDateTime.toString("dddd"));
		}
	}
	displayStringList.append(repeatTypeString);
	
	qDebug() << "clock: ClockAlarmListModel::getDisplayStringforAlarmItem <--";
	
	return displayStringList;
}

/*!
	Appends an alarm to the source model.

	\param alarmInfo AlarmInfo containing the alarm.
 */
void ClockAlarmListModel::appendAlarmToModel(AlarmInfo alarmInfo)
{
	// Append only pending alarms to the model.
	if (Notified != alarmInfo.alarmState) {
		QStandardItem *item = new QStandardItem();
		QStringList displayStringList =
				getDisplayStringListforAlarmItem(alarmInfo);
		
		item->setData(displayStringList, Qt::DisplayRole);
		
		// The repeat icon.
		QList<QVariant> displayiconList;
		if (Weekly == alarmInfo.repeatType ||
				Daily == alarmInfo.repeatType ||
				Workday == alarmInfo.repeatType) {
			displayiconList.append(HbIcon(":/clock/alarm_repeat"));
		} else {
			displayiconList.append(QVariant());
		}
		item->setData(displayiconList, Qt::DecorationRole);
		
		// The status.
		if (Enabled == alarmInfo.alarmStatus) {
			item->setData(QString(":/clock/alarm_active"), AlarmIcon);
		} else {
			item->setData(QString(":/clock/alarm_inactive"), AlarmIcon);
		}

		// If the alarm is snoozed.
		if (Snoozed == alarmInfo.alarmState) {
			// TODO : Change the alarm icon also to alarm snooze icon
		}

		// The model for the list item will have:
		// 1. Id of the alarm that the list item represents.
		// 2. The repeat type of the alarm (Recurrence type).
		// 3. TAlarmStatus value.
		QList<QVariant> alarmData;
		alarmData.append(alarmInfo.alarmId);
		alarmData.append(alarmInfo.repeatType);
		alarmData.append(alarmInfo.alarmStatus);
		item->setData(alarmData, AlarmDetails);
		
		mSourceModel->appendRow(item);
	}

}

/*!
	Updates the alarm details in the source model with updated 
	details from server.
	
	\param item Contains the alarm to be updated.
	\param alarmInfo AlarmInfo containing the alarm.
 */
void ClockAlarmListModel::updateAlarmDetails(
		QModelIndex modelIndex, AlarmInfo alarmInfo)
{
	if (Notified != alarmInfo.alarmState) {
		QStringList displayStringList =
				getDisplayStringListforAlarmItem(alarmInfo);
		
		mSourceModel->setData(
				modelIndex, displayStringList, Qt::DisplayRole);
		
		// The repeat icon.
		QList<QVariant> displayiconList;
		if (Weekly == alarmInfo.repeatType ||
				Daily == alarmInfo.repeatType ||
				Workday == alarmInfo.repeatType) {
			displayiconList.append(HbIcon(":/clock/alarm_repeat"));
		} else {
			displayiconList.append(QVariant());
		}
		mSourceModel->setData(
				modelIndex, displayiconList, Qt::DecorationRole);
		// The status.
		if (Enabled == alarmInfo.alarmStatus) {
			mSourceModel->setData(
					modelIndex, QString(":/clock/alarm_active"), AlarmIcon);
		} else {
			mSourceModel->setData(
					modelIndex, QString(":/clock/alarm_inactive"), AlarmIcon);
		}

		// If the alarm is snoozed.
		if (Snoozed == alarmInfo.alarmState) {
			// TODO : Change the alarm icon also to alarm snooze icon
		}

		// The model for the list item will have:
		// 1. Id of the alarm that the list item represents.
		// 2. The repeat type of the alarm (Recurrence type).
		// 3. TAlarmStatus value.
		QList<QVariant> alarmData;
		alarmData.append(alarmInfo.alarmId);
		alarmData.append(alarmInfo.repeatType);
		alarmData.append(alarmInfo.alarmStatus);
		mSourceModel->setData(modelIndex, alarmData, AlarmDetails);
	}
}

/*!
	Returns the number of active alarms present in the model.

	\return int number of active alarms present in the model.
 */
int ClockAlarmListModel::getActiveAlarmCount()
{
	int activeAlarmCount = 0;
	for (int index = 0; index < mSourceModel->rowCount(); index++) {
		// Get the data for the alarm.
		QList<QVariant> alarmData = mSourceModel->
				index(index, 0).data(AlarmDetails).toList();
		if (Enabled == alarmData.at(2).value<int>()) {
			activeAlarmCount++;
		}
	}
	return activeAlarmCount;
}
// End of file	--Don't remove this.
