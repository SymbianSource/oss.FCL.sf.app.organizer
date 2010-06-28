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
* Definition file for class DateTimeSettingsView.
*
*/

// System includes
#include <QTimer>
#include <QDateTime>
#include <HbExtendedLocale>
#include <cpitemdatahelper.h>

// User includes
#include "datetimesettingsview.h"
#include "clocksettingsview.h"
#include "timezoneclient.h"

/*!
	\class DateTimeSettingsView

	This class launches the clock settings view from control panel.
 */

/*!
	Constructor.

	\param itemDataHelper CpItemDataHelper object.
	\param text text to be displayed in first line.
	\param description test to be displayed in second line.
	\param icon to be displayed.
	\param parent Parent of type HbDataFormModelItem
 */
DateTimeSettingsView::DateTimeSettingsView(
		CpItemDataHelper &itemDataHelper, const QString &text,
		const QString &description, const HbIcon &icon,
		const HbDataFormModelItem *parent):
		CpSettingFormEntryItemData(
				itemDataHelper, text, description, icon, parent)
{
	// Construct the timezone client.
	mTimezoneClient = TimezoneClient::getInstance();
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateDisplayTime()));

	// Start a timer. For updating the displayed time.
	mTickTimer = new QTimer(this);
	// Start the Timer for 1 minute.
	mTickTimer->start(60000 - 1000 * QTime::currentTime().second());
	connect(
			mTickTimer, SIGNAL(timeout()),
			this, SLOT(updateDisplayTime()));
}

/*!
	Destructor.
 */
DateTimeSettingsView::~DateTimeSettingsView()
{
	if (mTickTimer) {
		mTickTimer->stop();
		delete mTickTimer;
		mTickTimer = 0;
	}
	
	if (!mTimezoneClient->isNull()) {
		mTimezoneClient->deleteInstance();
	}
}

/*!
	Launches the clock settings view.
 */
void DateTimeSettingsView::onLaunchView()
{
	ClockSettingsView *settingsView = new ClockSettingsView(this);
	settingsView->loadSettingsView();
}

/*!
	Updates the second line i.e date & time.
 */
void DateTimeSettingsView::updateDisplayTime()
{
	HbExtendedLocale locale = HbExtendedLocale::system();
	QString timeInfo = locale.format(
			QTime::currentTime(), r_qtn_time_usual_with_zero);
	QString dateinfo = locale.format(
			QDate::currentDate(), r_qtn_date_usual_with_zero);
	QString displayString;
	displayString.append(timeInfo);
	displayString.append(" ");
	displayString.append(dateinfo);
	setDescription(displayString);
	// Start the Timer for 1 minute.
	mTickTimer->start(60000);
}

/*!
	createSettingView()
 */
CpBaseSettingView *DateTimeSettingsView::createSettingView() const
{
	return 0;
}

// End of file	--Don't remove this
