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
* Description:  This class is the base class of all Calendar views.
 *
 */

// system includes
#include <QtGui>
#include <hbwidget.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbdialog.h>
#include <hbmainwindow.h>
#include <hbdatetimepicker.h>
#include <vwsdef.h>

//user includes
#include <CalenUid.h>
#include "calennativeview.h"
#include "calenservices.h"
#include "calencontext.h"
#include "calensettingsview.h"
#include "calendateutils.h"

/*!
 \class CalenNativeView
 Base class for all calendar views
 */

/*!
 Default constructor.
 */
CalenNativeView::CalenNativeView(MCalenServices& services) :
	mServices(services), mPluginEnabled(false)//,mGoToDateForm(NULL)
{
	setTitle(hbTrId("txt_calendar_title_calendar"));

	// Create services API and register for notifications
	RArray<TCalenNotification> notificationArray;

	notificationArray.Append(ECalenNotifySystemTimeChanged);
	notificationArray.Append(ECalenNotifySystemLocaleChanged);

	mServices.RegisterForNotificationsL(this, notificationArray);

	notificationArray.Reset();
}

/*!
 Destructor
 */
CalenNativeView::~CalenNativeView()
{
}

/*!
 Checks to see if an info bar is available from a customisation plugin
 */
void CalenNativeView::checkInfobarL()
{
	HbWidget* pluginInfobar = mServices.Infobar();

	if (mInfobar && mInfobar != pluginInfobar) {
		mServices.IssueNotificationL(ECalenNotifyCheckPluginUnloading);
		mInfobar = NULL;
	}

	if (pluginInfobar) {
		mInfobar = pluginInfobar;
	}

}

/*!
 Returns true if infobar is there
 */
bool CalenNativeView::isPlugin()
{
	HbWidget* pluginInfobar = mServices.Infobar();
	if (pluginInfobar) {
		return true;
	} else {
		return false;
	}

}

/*!
 Issues populcaiton complete to the framework
 */
void CalenNativeView::populationComplete()
{
	// Population is complete, issue the notification
	mServices.IssueNotificationL(ECalenNotifyViewPopulationComplete);
}

/*!
 Returns a pointer to the available secondary control, or NULL if none is
 available. The secondary control may be either an info bar or a preview pane
 */
HbWidget* CalenNativeView::ControlOrNull()
{
	HbWidget* pluginInfobar = NULL;
	if (mInfobar) {
		pluginInfobar = mInfobar;

	} else {
		pluginInfobar = mServices.Infobar();
	}
	return pluginInfobar;
}

/*!
 Slot for delete before date
 */
void CalenNativeView::deleteBeforeDate()
{
	mServices.IssueCommandL(ECalenDeleteEntriesBeforeDate);
}

/*!
 Slot for delete all entries
 */
void CalenNativeView::deleteAllEntries()
{
	mServices.IssueCommandL(ECalenDeleteAllEntries);
}

/*!
 Slot to handle gotodate
 */
void CalenNativeView::goToDate()
{
	// Get the current date.
	QDateTime currentDateTime = CalenDateUtils::today();
	QDate currentDate = currentDateTime.date();
	mDatePicker = new HbDateTimePicker(currentDate, this);

	// Set the date range.
	mDatePicker->setMinimumDate(CalenDateUtils::minTime().date());
	mDatePicker->setMaximumDate(CalenDateUtils::maxTime().date());

	// Create a popup with datepicker for the user to select date.
	HbDialog popUp;
	popUp.setDismissPolicy(HbDialog::NoDismiss);
	popUp.setTimeout(HbDialog::NoTimeout);
	popUp.setHeadingWidget(new HbLabel(hbTrId("txt_calendar_opt_go_to_date")));
	popUp.setContentWidget(mDatePicker);
	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"));
	popUp.setPrimaryAction(okAction);
	connect(okAction, SIGNAL(triggered()), this, SLOT(goToSelectedDate()));
	connect(okAction, SIGNAL(triggered()), &popUp, SLOT(close()));
	popUp.setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), &popUp));
	popUp.exec();
}

/*
 Slot to handle date selected on gotodate popup
 */
void CalenNativeView::goToSelectedDate()
{
	QDate selectedDate = mDatePicker->date();

	// Check if the selected date is within the range.
	if (selectedDate.isValid() &&
	        selectedDate >= CalenDateUtils::minTime().date() &&
	        selectedDate <= CalenDateUtils::maxTime().date()) {
		MCalenContext& context = mServices.Context();
		QDateTime contextDate = context.focusDateAndTimeL();

		//Set the selected date to contextDate.
		contextDate.setDate(selectedDate);
		context.setFocusDateAndTimeL(contextDate, KCalenMonthViewUidValue);
		mServices.IssueCommandL(ECalenStartActiveStep);
	}
}

/*!
 Slot to handle setting item in options menu
 */
void CalenNativeView::launchSettingsView()
{
	mServices.IssueCommandL(ECalenShowSettings);
}

/*!
 Slot to handle to orientation change
 */
void CalenNativeView::changeOrientation(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
    // Nothing, derived classes will implement it
}

/*!
 Handles the interested notifications from the calendar framework
 */
void CalenNativeView::HandleNotification(const TCalenNotification notification)
{
	switch (notification) {
		case ECalenNotifySystemLocaleChanged: {
			onLocaleChanged(EChangesLocale);
		}
			break;
		default:
			break;
	}
}

/*!
 Returns true if plugin is enabled
 */
TBool CalenNativeView::pluginEnabled()
{
	return mPluginEnabled;
}

//End Of File
