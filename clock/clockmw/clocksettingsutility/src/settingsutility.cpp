/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *
 * Description:   Definition file for the class SettingsUtility
 *
 */

// System includes
#include <e32std.h>
#include <QTime>
#include <QDateTime>
#include <HbGlobal>

// User includes
#include "settingsutility.h"
#include "timezoneclient.h"
#include "clockdatatypes.h"

/*!
	\class SettingsUtility.
 */

/*!
	Default constructor.
 */
SettingsUtility::SettingsUtility(QObject *parent)
:QObject(parent)
{
	qDebug("clock: SettingsUtility::SettingsUtility -->");

	mTimeSeparatorList << tr(".") << tr(":");
	mClockTypeList << tr("Analog") << tr("Digital");
	mTimeFormatList << hbTrId("txt_clk_setlabel_val_24_hour") << hbTrId("txt_clk_setlabel_val_12_hour");
	mDateFormatList << hbTrId("txt_clk_setlabel_val_dd_mm_yyyy") << hbTrId("txt_clk_setlabel_val_mm_dd_yyyy") << hbTrId("txt_clk_setlabel_val_yyyy_mm_dd");
	mDateSeparatorList << tr(".") << tr(":") << tr("/") << tr("-");
	mAutoUpdateValueList << tr("ON") << tr("OFF");
	mSnoozeValueList << tr("5 minutes") << tr("15 minutes") << tr(" 30 minutes") << tr("1 hour");

	mTimeZoneClient = new TimezoneClient(this);

	qDebug("clock: SettingsUtility::SettingsUtility <--");

//	mSettingsMamager = new XQSettingsManager(this);
}

/*!
	Destructor.
 */
SettingsUtility::~SettingsUtility()
{
	// Nothing yet.
}

/*!

 */
void SettingsUtility::setTime(const QString &time)
{
	QTime newTime = QTime::fromString(time, timeFormatString());

	if (newTime.isValid()) {
		mTimeZoneClient->setDateTime(QDateTime(QDate::currentDate(), newTime));
	}
}

/*!

 */
QString SettingsUtility::time()
{
	return QTime::currentTime().toString(timeFormatString());
}

/*!

 */
void SettingsUtility::setTimeZone(const QString &timezone)
{
	Q_UNUSED(timezone)
}

/*!

 */
QString SettingsUtility::timeZone()
{
	QStringList dummyList;

	// Get the current zone info.
	LocationInfo currentZoneInfo = mTimeZoneClient->getCurrentZoneInfoL();

	// Construct the GMT +/- X string.
	QString gmtOffset(hbTrId("txt_common_common_gmt"));

	int utcOffset = currentZoneInfo.zoneOffset;
	int offsetInHours (utcOffset/60);
	int offsetInMinutes (utcOffset%60);

	// Check wether the offset is +ve or -ve.
	if (0 < utcOffset) {
		// We have a positive offset. Append the '+' character.
		gmtOffset += tr("+ ");
	} else if (0 > utcOffset) {
		// We have a negative offset. Append the '-' character.
		gmtOffset += tr("- ");
		offsetInHours = -offsetInHours;
	} else {
		// We dont have an offset. We are at GMT zone.
	}

	// Append the hour component.
	gmtOffset += QString::number(offsetInHours);

	// Append the time separator.
	gmtOffset += mTimeSeparatorList.at(timeSeparator(dummyList));

	// Append the minute component.
	// If minute component is less less than 10, append a '00'
	if (0 <= offsetInMinutes && offsetInMinutes < 10) {
		gmtOffset += tr("00");
	} else {
		gmtOffset += QString::number(offsetInMinutes);
	}

	gmtOffset += tr(" ");
	// TODO: append city name when more than one cities else country name.
	gmtOffset += currentZoneInfo.cityName;

	return gmtOffset;
}

/*!

 */
void SettingsUtility::setTimeFormat(const QString& format)
{
	TLocale locale;

	if (format == mTimeFormatList.at(0)) {
		locale.SetTimeFormat(ETime24);
	} else if (format == mTimeFormatList.at(1)) {
		locale.SetTimeFormat(ETime12);
	} else {
		// Nothing to do.
	}
	locale.Set();
}

/*!

 */
int SettingsUtility::timeFormat(QStringList &format)
{
	TLocale locale;
	int value = -1;

	if (ETime24 == locale.TimeFormat()) {
		value = 0;
	} else if (ETime12 == locale.TimeFormat()) {
		value = 1;
	}

	format = mTimeFormatList;

	return value;
}

/*!

 */
void SettingsUtility::setClockType(const QString &type)
{
	TLocale locale;

	if (type == mClockTypeList.at(0)) {
	    locale.SetClockFormat(EClockAnalog);
    } else if (type == mClockTypeList.at(1)) {
	    locale.SetClockFormat(EClockDigital);
    } else {
    	// Nothing to do.
    }
	locale.Set();
}

/*!

 */
int SettingsUtility::clockType(QStringList &list)
{
	TLocale locale;
	int value = -1;

	if (EClockAnalog == locale.ClockFormat()) {
	    value = 0;
    } else if (EClockDigital == locale.ClockFormat()){
    	value = 1;
    } else {
    	// Nothing to do.
    }

	list = mClockTypeList;

	return value;
}

/*!

 */
void SettingsUtility::setTimeSeparator(const QString &separator)
{
	if (mTimeSeparatorList.contains(separator)) {
	    TLocale locale;

		locale.SetTimeSeparator(TChar(separator.unicode()->unicode()), 1);
		locale.SetTimeSeparator(TChar(separator.unicode()->unicode()), 2);

		locale.Set();
    }
}

/*!

 */
int SettingsUtility::timeSeparator(QStringList &list)
{
	qDebug() << "clock: SettingsUtility::timeSeparator -->";

	TLocale locale;
	TChar separatorChar = locale.TimeSeparator(1);
	int value = -1;

	if (separatorChar == (mTimeSeparatorList.at(0).unicode())->unicode()) {
		value = 0;
	} else if (separatorChar == (mTimeSeparatorList.at(1).unicode())->unicode()) {
		value = 1;
	} else {
		// Nothing to do.
	}

	list = mTimeSeparatorList;

	qDebug() << "clock: SettingsUtility::timeSeparator <--";

	return value;
}

/*!

 */
void SettingsUtility::setAutoUpdate(const QString &value)
{
	// TODO: implement the changes after server is in place.
	// 1. Inform the server about the setting.
	// 2. Get the status of the change from server
	// 3. Emit the signal to inform the clients of the change in the settings item,
	// pass the enum of settings item changed and the updated value.

	if (0 == value.compare(tr("ON"), Qt::CaseInsensitive)) {
		mTimeZoneClient->setTimeUpdateOn(true);
    } else {
    	mTimeZoneClient->setTimeUpdateOn(false);
    }

	emit settingsChanged(AutoTimeUpdate, value);
}

/*!

 */
int SettingsUtility::autoUpdate(QStringList &list)
{
	// TODO: implement properly. this is jst a dummy implementation.
	int value = 1;

	bool autoUpdate = mTimeZoneClient->timeUpdateOn();

	if (autoUpdate) {
	    value = 0;
    }
	list = mAutoUpdateValueList;
	return value;
}

/*!

 */
void SettingsUtility::setDate(const QString &date)
{
	QDate newDate = QDate::fromString(date, dateFormatString());

	if (newDate.isValid()) {
		mTimeZoneClient->setDateTime(QDateTime(newDate, QTime::currentTime()));
    }
}

/*!

 */
QString SettingsUtility::date()
{
	qDebug() << "clock: SettingsUtility::date -->";

	qDebug() << "clock: SettingsUtility::date <--";

	return QDate::currentDate().toString(dateFormatString());
}

/*!

 */
void SettingsUtility::setDateFormat(const QString &format)
{
	TLocale locale;
	int index;

	for (index = 0; index < mDateFormatList.count(); ++index) {
		if (format == mDateFormatList.at(index)) {
			break;
		}
	}

	switch (index) {
		case 0:
			locale.SetDateFormat(EDateEuropean);
			break;
		case 1:
			locale.SetDateFormat(EDateAmerican);
			break;
		case 2:
			locale.SetDateFormat(EDateJapanese);
			break;
		default:
			//Nothing to do.
			break;
	}

	locale.Set();
}

/*!

 */
int SettingsUtility::dateFormat(QStringList &format)
{
	TLocale locale;
	int index = -1;

	switch (locale.DateFormat()) {
		case EDateEuropean:
			index = 0;
			break;
		case EDateAmerican:
			index = 1;
			break;
		case EDateJapanese:
			index = 2;
			break;
		default:
			// Nothing yet.
			break;
	}

	format = mDateFormatList;

	return index;
}

/*!

 */
void SettingsUtility::setDateSeparator(const QString &separator)
{
	TLocale locale;
	locale.SetDateSeparator(
			TChar(separator.unicode()->unicode()), 1);
	locale.SetDateSeparator(
			TChar(separator.unicode()->unicode()), 2);
	locale.Set();
}

/*!

 */
int SettingsUtility::dateSeparator(QStringList &separator)
{
	TLocale locale;
	TChar separatorChar = locale.DateSeparator(1);

	const int at0 = (mDateSeparatorList.at(0).unicode())->unicode();
	int at1 = (mDateSeparatorList.at(1).unicode())->unicode();
	int at2 = (mDateSeparatorList.at(2).unicode())->unicode();
	int at3 = (mDateSeparatorList.at(3).unicode())->unicode();
	int value = 0;

	if (separatorChar == (mDateSeparatorList.at(0).unicode())->unicode()) {
		value = 0;
	} else if (separatorChar ==
			(mDateSeparatorList.at(1).unicode())->unicode()) {
		value = 1;
	} else if (separatorChar ==
			(mDateSeparatorList.at(2).unicode())->unicode()) {
		value = 2;
	} else if (separatorChar ==
			(mDateSeparatorList.at(3).unicode())->unicode()) {
		value = 3;
	}

	separator = mDateSeparatorList;

	return value;
}

/*!
	Sets a new list of workdays.

	\param workdays The new list of workdays selected.
 */
void SettingsUtility::setWorkdays(const QString &workdays)
{
	Q_UNUSED(workdays)
}

/*!
	Returns the workdays list selected by the user.

	\param workdaysList A string list of workdays selected in the locale.
	\return int Index of the item selected.
 */
int SettingsUtility::workdays(QStringList &workdaysList)
{
	Q_UNUSED(workdaysList)

	return 0;
}

/*!
	Returns a string based on the locale set, to format time.

	\return QString String to be used to format time.
 */
QString SettingsUtility::timeFormatString()
{
	QString format;
	TLocale locale;
	QStringList dummyList;

	if (ETime24 == locale.TimeFormat()) {
		format = QString("h:mm");
	} else if (ETime12 == locale.TimeFormat()) {
		format = QString("h:mm ap");
	}

	QString separator = mTimeSeparatorList.at(timeSeparator(dummyList));

	format.replace(QString(":"), separator);

	return format;
}

/*!
	Returns a string based on the locale set, to format date.

	\return QString String to be used to format date.
 */
QString SettingsUtility::dateFormatString()
{
	QStringList dummyList;
	QString format(mDateFormatList.at(dateFormat(dummyList)));
	QString separator = mDateSeparatorList.at(dateSeparator(dummyList));

	format.replace(QString(" "), separator);
	format.replace(QString("mm"), QString("MM"));

	return format;
}

/*!
	Sets the snooze time value selected by the user.

	\param index The index of the preset snooze time values.
 */
void SettingsUtility::setSnoozeTime(int index)
{
	Q_UNUSED(index)
}

/*!
	Returns a list of default snooze time values.

	\param snoozeValueList A string list of preset values.
	\return int The index of the item in the list to be focused.
 */
int SettingsUtility::snoozeTime(QStringList &snoozeValueList)
{
	int index = -1;

	snoozeValueList = mSnoozeValueList;
	return index;
}

/*!
	Sets the start of week value selected by the user.

	\param index The index of the selected value.
 */
void SettingsUtility::setStartOfWeek(int index)
{
	TLocale locale;
	TDay day = (TDay)index;
	locale.SetStartOfWeek(day);
	locale.Set();
}
// End of file	--Don't remove this.
