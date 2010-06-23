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
#include <hbactivitymanager.h> //Activity Manager
#include <hbapplication.h> //hbapplication

//user includes
#include <CalenUid.h>
#include "calennativeview.h"
#include "calenservices.h"
#include "calencontext.h"
#include "calensettingsview.h"
#include "calendateutils.h"
#include "calenconstants.h"

/*!
 \class CalenNativeView
 Base class for all calendar views
 */

/*!
 Default constructor.
 */
CalenNativeView::CalenNativeView(MCalenServices &services) :
	mServices(services), mIsCapturedScreenShotValid(false)
{
	setTitle(hbTrId("txt_calendar_title_calendar"));

	// Create services API and register for notifications
	RArray<TCalenNotification> notificationArray;

	notificationArray.Append(ECalenNotifySystemTimeChanged);
	notificationArray.Append(ECalenNotifySystemLocaleChanged);
	notificationArray.Append(ECalenNotifyContextChanged);

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
 Issues populcaiton complete to the framework
 */
void CalenNativeView::populationComplete()
{
	// Population is complete, issue the notification
	mServices.IssueNotificationL(ECalenNotifyViewPopulationComplete);
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
	// Create a popup with datepicker for the user to select date.
	HbDialog *popUp = new HbDialog();
	popUp->setDismissPolicy(HbDialog::NoDismiss);
	popUp->setTimeout(HbDialog::NoTimeout);
	popUp->setAttribute( Qt::WA_DeleteOnClose, true );
	popUp->setHeadingWidget(new HbLabel(hbTrId("txt_calendar_opt_go_to_date")));
	
	// Get the current date.
	QDateTime currentDateTime = CalenDateUtils::today();
	QDate currentDate = currentDateTime.date();
	if(mDatePicker) {
		mDatePicker = NULL;
	}
	mDatePicker = new HbDateTimePicker(currentDate, popUp);
	// Set the date range.
	mDatePicker->setMinimumDate(CalenDateUtils::minTime().date());
	mDatePicker->setMaximumDate(CalenDateUtils::maxTime().date());
	mDatePicker->setDate(currentDate);
	
	popUp->setContentWidget(mDatePicker);
	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"));
	popUp->addAction(okAction);
	connect(okAction, SIGNAL(triggered()), this, SLOT(goToSelectedDate()));
	popUp->addAction(new HbAction(hbTrId("txt_common_button_cancel"), popUp));
	popUp->open();
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
	}
	refreshViewOnGoToDate();
}

/*!
 Virtual function to refresh the current view upon selecting a date
 from GoToDate popup
 */
void CalenNativeView::refreshViewOnGoToDate()
{
	mServices.IssueCommandL(ECalenStartActiveStep);
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
		case ECalenNotifyContextChanged: {
			onContextChanged();
		}
		break;
		default:
			break;
	}
}

/*!
 Returns true if plugin is loaded
 */
TBool CalenNativeView::pluginEnabled()
{
	QString *pluginInfo = mServices.InfobarTextL();
	if (!pluginInfo) {
		return false;
	} else {
		return true;
	}
}

QString *CalenNativeView::pluginText()
{
	return mServices.InfobarTextL();
}

// ----------------------------------------------------------------------------
// captureScreenshot caltures screen shot for the given viewId
// @param viewId view for which screenshot needs to be captured
// ----------------------------------------------------------------------------
// 
void CalenNativeView::captureScreenshot(bool captureScreenShot)
    {
    // get a screenshot for saving to the activity manager. It's done for once
    // to optimize the performance
    if (captureScreenShot) {
        mScreenShotMetadata.clear(); // remove any screenshot captured earlier
        mScreenShotMetadata.insert("screenshot", QPixmap::grabWidget(mainWindow(), mainWindow()->rect()));
        }
    mIsCapturedScreenShotValid = captureScreenShot; // set the validity of the screenshot captured
    }

// ----------------------------------------------------------------------------
// saveActivity saves the activity for current view
// ----------------------------------------------------------------------------
// 
void CalenNativeView::saveActivity()
 {
   // Get a pointer to activity manager 
   HbActivityManager* activityManager = qobject_cast<HbApplication*>(qApp)->activityManager();
 
   // check if alerady a valid screen shot is captured
   if (!mIsCapturedScreenShotValid) {
       mScreenShotMetadata.clear(); // remove any screenshot captured earlier
       mScreenShotMetadata.insert("screenshot", QPixmap::grabWidget(mainWindow(), mainWindow()->rect()));
       }
   
   // Save any data necessary to save the state
   QByteArray serializedActivity;
   QDataStream stream(&serializedActivity, QIODevice::WriteOnly | QIODevice::Append);
   stream << mActivityId;
 
   bool ok(false);
   // Save activity
   ok = activityManager->addActivity(activityName, serializedActivity, mScreenShotMetadata);

   // Check is activity saved sucessfully
   if ( !ok )  {
       qFatal("Add failed" ); // Panic is activity is not saved successfully
       }
 }
//End Of File
