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
// alarmalertwidget_p.cpp

// System includes
#include <QApplication>
#include <QGesture>
#include <QGestureEvent>
#include <QGraphicsSceneMouseEvent>

#include <hbdocumentloader.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbinstance.h>
#include <hbextendedlocale.h>
#include <hbi18ndef.h>
#include <hbaction.h>
#include <hbtranslator.h>

// User includes
#include "alarmalertwidget_p.h"
#include "alarmalert.h"
#include "alarmalertdocloader.h"

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::AlarmAlertDialogPrivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
AlarmAlertDialogPrivate::AlarmAlertDialogPrivate(const QVariantMap &parameters):
	mTranslator(new HbTranslator("alarmui")),
	mClosedByClient(false),
	mIsSilenceKey(false)
	{
	// Extract all the parameters sent by the client
	parseAndFetchParams(parameters);

	// Set the dismiss policy and timeout property
	setDismissPolicy(HbPopup::NoDismiss);
	setTimeout(HbPopup::NoTimeout);
	
	// Listen the signal when alarmui is about to display so that we can start playing the alarm tone.
	connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToDisplay()) );

	// Initialize the user response
	mUserResponse = Other;

	// TODO: Gestures not working. Integrate once support is available from Qt
	grabGesture(Qt::SwipeGesture);
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::~AlarmAlertDialogPrivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
AlarmAlertDialogPrivate::~AlarmAlertDialogPrivate()
{
    // Cleanup
	if (mAlertDocLoader) {
		delete mAlertDocLoader;
	}
    // Cancel any ongoing animations
    HbEffect::cancel(mSlider, "slideDownPortrait", false, false, false);
    HbEffect::cancel(mSlider, "slideDownLandscape", false, false, false);
    
    // Remove the effect
    HbEffect::remove(mSlider);

	// Remove the translator
	if (mTranslator) {
		delete mTranslator;
		mTranslator = 0;
	}
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::setDeviceDialogParameters
// rest of the details are commented in the header
// ---------------------------------------------------------
//
bool AlarmAlertDialogPrivate::setDeviceDialogParameters(const QVariantMap &parameters)
{
    Q_UNUSED(parameters);
    
    //TODO: Handle parameters to be set
    return 0;
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::deviceDialogError
// rest of the details are commented in the header
// ---------------------------------------------------------
//
int AlarmAlertDialogPrivate::deviceDialogError() const
{
    // TODO: Return any errors that might have occured
    return 0;
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::AlarmAlertDialogPrivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::closeDeviceDialog(bool byClient)
{
    Q_UNUSED(byClient);
    mClosedByClient = byClient;
	close();
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::deviceDialogWidget
// rest of the details are commented in the header
// ---------------------------------------------------------
//
HbDialog *AlarmAlertDialogPrivate::deviceDialogWidget() const
{
    return const_cast<AlarmAlertDialogPrivate*> (this);
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::showEvent
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::showEvent(QShowEvent *event)
{
	HbDialog::showEvent(event);
	QVariantMap param;
	param.insert(alarmCommand, mUserResponse);
	emit deviceDialogData(param);
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::closeEvent
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::closeEvent(QCloseEvent *event)
{
    
	// Do not notify the client back if the close was initiated by client itself
	if(!mClosedByClient) {
		// Package the user response and send it
		QVariantMap param;
		param.insert(alarmCommand, mUserResponse);
		emit deviceDialogData(param);
	}
	// This signal has to be emitted so that the dialog can be cleaned up later
	emit deviceDialogClosed();
	
	// Forward the call to the base class
	HbDialog::closeEvent(event);
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::handleOrientationChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::handleOrientationChange ()
{
    // TODO: Need to change this as per the UI concept
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::snoozed
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::snoozed()
{
	mUserResponse = Snooze;
	close();
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::silenced
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::silenced()
{
	if (mIsSilenceKey) {
		mUserResponse = Silence;
		mSnoozeAction->setText(hbTrId("txt_calendar_button_alarm_snooze"));
		mIsSilenceKey = false;
		QVariantMap param;
		param.insert(alarmCommand, mUserResponse);
		emit deviceDialogData(param);
	}else {
		mUserResponse = Snooze;
		close();
	}
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::aboutToDisplay
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::aboutToDisplay()
    {
    mUserResponse = Shown;
    QVariantMap param;
    param.insert(alarmCommand, mUserResponse);
    emit deviceDialogData(param);
    }
	
// ---------------------------------------------------------
// AlarmAlertDialogPrivate::dismissed
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::dismissed()
{
	mUserResponse = Stop;
	close();
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::parseAndFetchParams
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::parseAndFetchParams(const QVariantMap &parameters)
{
    // Iterate thru the list and extract all relevant parameters
    QVariantMap::const_iterator iter = parameters.constBegin();
    int count = parameters.size();
    while (iter != parameters.constEnd()) {
        QString key(iter.key());
        if (alarmSubject == key) {
            mSubject = iter.value().toString();
        } else if (alarmLocation == key) {
            mLocation = iter.value().toString();
        } else if (alarmTime == key) {
            mAlarmTime = iter.value().toString();
        } else if (alarmDate == key) {
            mAlarmDate = iter.value().toString();
        } else if (alarmType == key) {
            mAlarmAlertType = static_cast <AlarmType> (iter.value().toInt());
        } else if (alarmCanSnooze == key) {
            mCanSnooze = iter.value().toBool();
        } else if (alarmRingingType == key) {
            mIsSilent = iter.value().toBool();
        } else if (alarmIsTimed == key) {
            mIsTimedAlarm = iter.value().toBool();
        }else {
            break;
        }
        iter++;
    }
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::setupNormalUI
// rest of the details are commented in the header
// ---------------------------------------------------------
//

void AlarmAlertDialogPrivate::setupNormalUI(AlarmAlertDocLoader *alertDocLoader)
    {
	if(!alertDocLoader) {
		// Nothing can be done. Simply return
		return;
	}
	mAlertDocLoader = alertDocLoader;

	if (mAlarmAlertType == ClockAlarm) {
        handleClockAlarms();
    }else if(mAlarmAlertType == TodoAlarm) {
        handleToDoAlarms();
    }else if(mAlarmAlertType == CalendarAlarm) {
    	handleCalendarAlarms();
    }
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::handleClockAlarms
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::handleClockAlarms()
{
	QGraphicsWidget *headingWidget = mAlertDocLoader->findWidget("heading");
	if (!headingWidget) {
		qFatal("Unable to load the heading Widget");
	}

	mAlarmDateTime = qobject_cast<HbLabel*> (
			mAlertDocLoader->findWidget("alarmTime"));
	if (!mAlarmDateTime) {
		qFatal("Unable to load the alarmTime label");
	}
	HbExtendedLocale locale = HbExtendedLocale::system();
	mAlarmDateTime->setPlainText(
			hbTrId("txt_calendar_info_alarm_start_time").arg(mAlarmTime));

	mAlarmDescription = qobject_cast<HbLabel*> (
			mAlertDocLoader->findWidget("alarmDescription"));
	if (!mAlarmDescription) {
		qFatal("Unable to load the alarmDescription label");
	}
	mAlarmDescription->setPlainText(mSubject);

	mAlarmIcon = qobject_cast<HbLabel*> (
				mAlertDocLoader->findWidget("alarmIcon"));
	if (!mAlarmIcon) {
		qFatal("Unable to load the alarm icon");
	}
	//TODO: Add the proper icon for clock alarms in the docml
	mAlarmIcon->setIcon(HbIcon(":/image/clockAlarm.svg"));

	HbAction *snoozeAction = qobject_cast<HbAction*> (
			mAlertDocLoader->findObject("snoozeAction"));
	if (!snoozeAction) {
		qFatal("Unable to load the snoozeAction softkey");
	}
	snoozeAction->setText(hbTrId("txt_calendar_button_alarm_snooze"));
	disconnect(snoozeAction, SIGNAL(triggered()), this, SLOT(close()));
	connect(snoozeAction, SIGNAL(triggered()), this, SLOT(snoozed()));

	HbAction *stopAction = qobject_cast<HbAction*> (
			mAlertDocLoader->findObject("stopAction"));
	if (!stopAction) {
		qFatal("Unable to load the stopAction softkey");
	}
	stopAction->setText(hbTrId("txt_calendar_button_alarm_dialog_snooze"));
	disconnect(stopAction, SIGNAL(triggered()), this, SLOT(close()));
	connect(stopAction, SIGNAL(triggered()), this, SLOT(dismissed()));

	if (!mCanSnooze) {
		snoozeAction->setVisible(false);
	}
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::handleCalendarAlarms
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::handleCalendarAlarms()
{
	bool success = false;
	HbExtendedLocale locale = HbExtendedLocale::system();
	// Check if the alarm has the time info or not.
	// For all day events the time label has to be hidden
	// So load the proper sections accordingly
	if ( mIsTimedAlarm ) {
		mAlertDocLoader->load(
				alarmNormalUICalendarDocml, "calendarTimed",&success);
		if (!success) {
			qFatal("Unable to load the calendarTimed section");
		}
		mAlarmDateTime = qobject_cast<HbLabel*> (
				mAlertDocLoader->findWidget("alarmTime"));
		if (!mAlarmDateTime) {
			qFatal("Unable to find the alarmTime label");
		}
		mAlarmDateTime->setPlainText(
				hbTrId("txt_calendar_info_alarm_start_time").arg(mAlarmTime));
		HbLabel *alarmDate = qobject_cast<HbLabel*> (
				mAlertDocLoader->findWidget("alarmDate"));
		if (!alarmDate) {
			qFatal("Unable to find the alarmDate label");
		}
		alarmDate->setPlainText(
				hbTrId("txt_calendar_info_alarm_start_date").arg(mAlarmDate));
		HbLabel *alarmDateNonTimed = qobject_cast<HbLabel*> (
				mAlertDocLoader->findWidget("nonTimedAlarmDate"));
		if (!alarmDateNonTimed) {
			qFatal("Unable to find the nonTimedAlarmDate label");
		}
		alarmDateNonTimed->hide();
	}else {
		mAlertDocLoader->load(
				alarmNormalUICalendarDocml, "nonTimedAlarm",&success);
		if (!success) {
			qFatal("Unable to load the nonTimedAlarm section");
		}
		mAlarmDateTime = qobject_cast<HbLabel*> (
				mAlertDocLoader->findWidget("alarmTime"));
		if (!mAlarmDateTime) {
			qFatal("Unable to find the alarmTime label");
		}
		mAlarmDateTime->hide();
		HbLabel *alarmDate = qobject_cast<HbLabel*> (
				mAlertDocLoader->findWidget("alarmDate"));
		if (!alarmDate) {
			qFatal("Unable to find the alarmDate label");
		}
		alarmDate->hide();
		HbLabel *alarmDateNonTimed = qobject_cast<HbLabel*> (
				mAlertDocLoader->findWidget("nonTimedAlarmDate"));
		if (!alarmDateNonTimed) {
			qFatal("Unable to find the alarmDateNonTimed label");
		}
		alarmDateNonTimed->setPlainText(
				hbTrId("txt_calendar_info_alarm_start_date").arg(mAlarmDate));
	}

	QGraphicsWidget *headingWidget = mAlertDocLoader->findWidget("heading");
	if (!headingWidget) {
		qFatal("Unable to find the heading widget");
	}
	mAlarmDescription = qobject_cast<HbLabel*> (
			mAlertDocLoader->findWidget("alarmDescription"));
	if (!mAlarmDescription) {
		qFatal("Unable to find the alarmDescription label");
	}
	mAlarmDescription->setPlainText(mSubject);

	HbLabel *alarmLocation = qobject_cast<HbLabel*> (
			mAlertDocLoader->findWidget("alarmLocation"));
	if (!alarmLocation) {
		qFatal("Unable to find the alarmLocation label");
	}
	alarmLocation->setPlainText(mLocation);
	
	mAlarmIcon = qobject_cast<HbLabel*> (
			mAlertDocLoader->findWidget("alarmIcon"));
	if (!mAlarmIcon) {
		qFatal("Unable to find the alarm Icon");
	}

	mSnoozeAction = qobject_cast<HbAction*> (
			mAlertDocLoader->findObject("snoozeAction"));
	if (!mSnoozeAction) {
		qFatal("Unable to find the snoozeAction softkey");
	}
	disconnect(mSnoozeAction, SIGNAL(triggered()), this, SLOT(close()));
	if (!mCanSnooze) {
		mSnoozeAction->setVisible(false);
	}else { 
		if(!mIsSilent) {
			mSnoozeAction->setText(hbTrId("txt_calendar_button_alarm_silence"));
			connect(mSnoozeAction, SIGNAL(triggered()), this, SLOT(silenced()));
			mIsSilenceKey = true;
		} 	else {
			mSnoozeAction->setText(hbTrId("txt_calendar_button_alarm_snooze"));
			connect(mSnoozeAction, SIGNAL(triggered()), this, SLOT(snoozed()));
		}
	}
	HbAction *stopAction = qobject_cast<HbAction*> (
			mAlertDocLoader->findObject("stopAction"));
	if (!stopAction) {
		qFatal("Unable to find the stopAction softkey");
	}
	stopAction->setText(hbTrId("txt_calendar_button_alarm_dialog_snooze"));
	disconnect(stopAction, SIGNAL(triggered()), this, SLOT(close()));
	connect(stopAction, SIGNAL(triggered()), this, SLOT(dismissed()));
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::handleToDoAlarms
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::handleToDoAlarms()
{
	QGraphicsWidget *headingWidget = mAlertDocLoader->findWidget("heading");
	if (!headingWidget) {
		qFatal("Unable to load the heading widget");
	}
	HbExtendedLocale locale = HbExtendedLocale::system();
	
	HbLabel *alarmDate = qobject_cast<HbLabel*> (
							mAlertDocLoader->findWidget("alarmDate"));
	if (!alarmDate) {
		qFatal("Unable to load the alarmDate label");
	}
	alarmDate->setPlainText(
			hbTrId("txt_calendar_info_alarm_start_date").arg(mAlarmDate));
	
	mAlarmDescription = qobject_cast<HbLabel*> (
							mAlertDocLoader->findWidget("alarmDescription"));
	if (!mAlarmDescription) {
		qFatal("Unable to load the alarmDescription label");
	}
	mAlarmDescription->setPlainText(mSubject);
	
	mAlarmIcon = qobject_cast<HbLabel*> (
									mAlertDocLoader->findWidget("alarmIcon"));
	if (!mAlarmIcon) {
		qFatal("Unable to load the alarm Icon");
	}
	mSnoozeAction = qobject_cast<HbAction*> (
								mAlertDocLoader->findObject("snoozeAction"));
	if (!mSnoozeAction) {
		qFatal("Unable to load the snoozeAction softkey");
	}
	disconnect(mSnoozeAction, SIGNAL(triggered()), this, SLOT(close()));
	if (!mCanSnooze) {
		mSnoozeAction->setVisible(false);
	}else { 
		if(!mIsSilent) {
			mSnoozeAction->setText(hbTrId("txt_calendar_button_alarm_silence"));
			connect(mSnoozeAction, SIGNAL(triggered()), this, SLOT(silenced()));
			mIsSilenceKey = true;
		} 	else {
			mSnoozeAction->setText(hbTrId("txt_calendar_button_alarm_snooze"));
			connect(mSnoozeAction, SIGNAL(triggered()), this, SLOT(snoozed()));
		}
	}
	HbAction *stopAction = qobject_cast<HbAction*> (
									mAlertDocLoader->findObject("stopAction"));
	if (!stopAction) {
		qFatal("Unable to load the stopAction softkey");
	}
	stopAction->setText(hbTrId("txt_calendar_button_alarm_dialog_snooze"));
	disconnect(stopAction, SIGNAL(triggered()), this, SLOT(close()));
	connect(stopAction, SIGNAL(triggered()), this, SLOT(dismissed()));
}

// End of file  --Don't remove this.
