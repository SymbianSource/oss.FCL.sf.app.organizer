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
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QTranslator>
#include <hbdocumentloader.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbinstance.h>
#include <hbextendedlocale.h>
#include <hbi18ndef.h>
#include <hbapplication.h>

// User includes
#include "alarmalertwidget_p.h"
#include "alarmalert.h"

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::AlarmAlertDialogPrivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
AlarmAlertDialogPrivate::AlarmAlertDialogPrivate(const QVariantMap &parameters):
	mClosedByClient(false),
	mIsSilenceButton(false)
{
    // Extract all the parameters sent by the client
    parseAndFetchParams(parameters);
    
	// Set the dismiss policy and timeout property
	setDismissPolicy(HbPopup::NoDismiss);
    setTimeout(HbPopup::NoTimeout);
	
    // Initialize the user response
	mUserResponse = Other;
	
	/*if (isScreenLocked()) {
	    setupSwipeUI();
	} else {
	    setupNormalUI();
	}*/
	// Load the translation file and install the editor specific translator
	mTranslator = new QTranslator;
	QString lang = QLocale::system().name();
	QString path = "Z:/resource/qt/translations/";
	bool loaded = mTranslator->load("alarmui_en_GB",":/translations");
	// TODO: Load the appropriate .qm file based on locale
	//bool loaded = mTranslator->load("alarmui_" + lang, path);
	HbApplication::instance()->installTranslator(mTranslator);
	
	setupNormalUI();
	
	// Find the content widget and set it to self
	QGraphicsWidget *contentWidget = mDocLoader->findWidget("content");
	Q_ASSERT_X(contentWidget, "alarmalertwidget_p.cpp", "Unable to find content widget");
    setContentWidget(contentWidget);
    
    // Resize the dialog to accomodate the content widget
    setPreferredSize(contentWidget->minimumSize());
    
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
    if (mDocLoader) {
        delete mDocLoader;
    }
    
    // Cancel any ongoing animations
    HbEffect::cancel(mSlider, "slideDownPortrait", false, false, false);
    HbEffect::cancel(mSlider, "slideDownLandscape", false, false, false);
    
    // Remove the effect
    HbEffect::remove(mSlider);

	// Remove the translator
	HbApplication::instance()->removeTranslator(mTranslator);
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
	//emit deviceDialogData(param);
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::closeEvent
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::closeEvent(QCloseEvent *event)
{
    // Forward the call to the base class
	HbDialog::closeEvent(event);
	
	// Do not notify the client back if the close was initiated by client itself
	if(!mClosedByClient) {
		// Package the user response and send it
		QVariantMap param;
		param.insert(alarmCommand, mUserResponse);
		emit deviceDialogData(param);
	}
	// This signal has to be emitted so that the dialog can be cleaned up later
	emit deviceDialogClosed();
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::animationComplete
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::animationComplete(const HbEffect::EffectStatus &status)
{
    Q_UNUSED(status);
    
    // Restart the animation effects
	if (Qt::Vertical == hbInstance->orientation()) {
		HbEffect::start(mSlider, "slideDownPortrait", this, "animationComplete");
	} else {
	    HbEffect::start(mSlider, "slideDownLandscape", this, "animationComplete");
	}
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::sceneEvent
// rest of the details are commented in the header
// ---------------------------------------------------------
//
bool AlarmAlertDialogPrivate::sceneEvent(QEvent *event)
{
    // TODO: Need to handle gestures once they are available
    if (event->type() == QEvent::Gesture) {
        QGestureEvent *gestureEvent =
                static_cast<QGestureEvent*>(event);
        if (const QGesture *gesture = gestureEvent->gesture(Qt::SwipeGesture)) {
            if (Qt::GestureFinished == gesture->state()) {
                event->accept();
                close();
                return true;
            }
            event->accept();
            return true;
        }
    }
    return HbWidget::sceneEvent(event);
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::handleOrientationChange
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::handleOrientationChange ()
{
    // TODO: Need to change this as per the UI concept
    // Might need to disable orientation changes
	bool loadSuccess(false);
	if (Qt::Vertical == hbInstance->orientation()) {
		mDocLoader->load(":/xml/alarmalert.docml", "portrait", &loadSuccess);
		Q_ASSERT_X(loadSuccess, "alarmalertwidget_p.cpp", "Unable to load portrait section");
	} else {
		mDocLoader->load(":/xml/alarmalert.docml", "landscape", &loadSuccess);
		Q_ASSERT_X(loadSuccess, "alarmalertwidget_p.cpp", "Unable to load landscape section");
	}
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
	if (mIsSilenceButton) {
		mUserResponse = Silence;
		mSnoozeButton->setText(hbTrId("txt_calendar_button_alarm_snooze"));
		mIsSilenceButton = false;
		QVariantMap param;
		param.insert(alarmCommand, mUserResponse);
		emit deviceDialogData(param);
	}else {
		mUserResponse = Snooze;
		close();
	}
		


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
        } else if (alarmDateTime == key) {
            mAlarmTime = iter.value().toDateTime();
        } else if (alarmType == key) {
            mIsClockAlarm = iter.value().toBool();
        } else if (alarmCanSnooze == key) {
            mCanSnooze = iter.value().toBool();
        } else if (alarmRingingType == key) {
            mIsSilent = iter.value().toBool();
        } else {
            break;
        }
        iter++;
    }
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::setupSwipeUI
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::setupSwipeUI()
{
    // Connect to orientation change signals
    QList<HbMainWindow*> windowList = hbInstance->allMainWindows();
    mMainWindow = windowList.at(0);
    Q_ASSERT_X(mMainWindow, "alarmalertwidget_p.cpp", "Unable to get the main window");
    connect(mMainWindow, SIGNAL(aboutToChangeOrientation()), this, SLOT(handleOrientationChange()));

    // Load the docml file to get the content widget for the dialog
    mDocLoader = new HbDocumentLoader();
    bool loadSuccess = false;
    mDocLoader->load(alarmSwipeUIDocml, &loadSuccess);
    Q_ASSERT_X(loadSuccess, "alarmalertwidget_p.cpp", "Unable to load docml");
    
    // Get the reference to the slider widget
    mSlider = mDocLoader->findWidget("sliderIcon");
    Q_ASSERT_X(mSlider, "alarmalertwidget_p.cpp", "Unable to find slider widget");
    
    mAlarmDescription = qobject_cast<HbLabel*> (mDocLoader->findWidget("alarmDescription"));
    Q_ASSERT_X(mAlarmDescription, "alarmalertwidget_p.cpp", "Unable to find alarm description label");
    mAlarmDescription->setPlainText(mSubject);
    
    mAlarmDateTime = qobject_cast<HbLabel*> (mDocLoader->findWidget("alarmTime"));
    Q_ASSERT_X(mAlarmDateTime, "alarmalertwidget_p.cpp", "Unable to find alarm time label");
    
    mAlarmIcon = qobject_cast<HbLabel*> (mDocLoader->findWidget("alarmIcon"));
    Q_ASSERT_X(mAlarmIcon, "alarmalertwidget_p.cpp", "Unable to find alarm icon");
        
    if (!mIsClockAlarm) {
        mAlarmIcon->setIcon(HbIcon(":/image/calendarAlarm"));
    }
    
    // Set the time to the time label
    HbExtendedLocale locale;
    mAlarmDateTime->setPlainText(locale.format(mAlarmTime.time(), r_qtn_time_usual));
    
    // Add the required effects.
    HbEffect::add(mSlider,
                  QString(":/effect/animate_portrait.fxml"),
                  "slideDownPortrait");
    HbEffect::add(mSlider,
                  QString(":/effect/animate_landscape.fxml"),
                  "slideDownLandscape");
    
    // Based on the orientation, load the appropriate sections and animations
    if (Qt::Vertical == hbInstance->orientation()) {
        mDocLoader->load(":/xml/alarmalert.docml", "portrait", &loadSuccess);
        Q_ASSERT_X(loadSuccess, "alarmalertwidget_p.cpp", "Unable to load portrait section");

        HbEffect::start(mSlider, "slideDownPortrait", this, "animationComplete");
    } else {
        mDocLoader->load(":/xml/alarmalert.docml", "landscape", &loadSuccess);
        Q_ASSERT_X(loadSuccess, "alarmalertwidget_p.cpp", "Unable to load landscape section");

        HbEffect::start(mSlider, "slideDownLandscape", this, "animationComplete");
    }
    
    // Get the reference to the snooze button
    HbPushButton *snoozeButton = qobject_cast<HbPushButton*> (mDocLoader->findWidget("snoozeButton"));
    Q_ASSERT_X(snoozeButton, "alarmalertwidget_p.cpp", "Unable to find snooze button");
    connect(snoozeButton, SIGNAL(clicked()), this, SLOT(snoozed()));
    
    if (!mCanSnooze) {
        snoozeButton->hide();
    }
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::setupNormalUI
// rest of the details are commented in the header
// ---------------------------------------------------------
//

void AlarmAlertDialogPrivate::setupNormalUI()
{
    // Load the docml file to get the content widget for the dialog
    mDocLoader = new HbDocumentLoader();
    bool loadSuccess = false;
    mDocLoader->load(alarmNormalUIDocml, &loadSuccess);
    Q_ASSERT_X(loadSuccess, "alarmalertwidget_p.cpp", "Unable to load docml");
    
    mAlarmDescription = qobject_cast<HbLabel*> (mDocLoader->findWidget("alarmDescription"));
    Q_ASSERT_X(mAlarmDescription, "alarmalertwidget_p.cpp", "Unable to find alarm description label");
    mAlarmDescription->setPlainText(mSubject);
    
    mAlarmDateTime = qobject_cast<HbLabel*> (mDocLoader->findWidget("alarmTime"));
    Q_ASSERT_X(mAlarmDateTime, "alarmalertwidget_p.cpp", "Unable to find alarm time label");
    
    mAlarmIcon = qobject_cast<HbLabel*> (mDocLoader->findWidget("alarmIcon"));
    Q_ASSERT_X(mAlarmIcon, "alarmalertwidget_p.cpp", "Unable to find alarm icon");
        
    if (!mIsClockAlarm) {
        mAlarmIcon->setIcon(HbIcon(":/image/calendarAlarm"));
    }
    
    // Set the time to the time label
    HbExtendedLocale locale = HbExtendedLocale::system();
    mAlarmDateTime->setPlainText(
    		hbTrId("txt_calendar_info_alarm_start_time").arg(
    				locale.format(mAlarmTime.time(), r_qtn_time_usual)));
    
    // TODO: Based on the orientation, load the appropriate sections and animations
    if (Qt::Vertical == hbInstance->orientation()) {
    } else {
    }
    

    
    // Get the reference to the stop button
    HbPushButton *stopButton = qobject_cast<HbPushButton*> (mDocLoader->findWidget("stopButton"));
    Q_ASSERT_X(stopButton, "alarmalertwidget_p.cpp", "Unable to find stop button");
    connect(stopButton, SIGNAL(clicked()), this, SLOT(dismissed()));
    
    if (mIsClockAlarm) {
    	handleClockAlarms();
    }else {
    	handleCalendarAlarms();
    }
    
    QList<HbMainWindow*> windowList = hbInstance->allMainWindows();
    mMainWindow = windowList.at(0);
    QRectF screenRect = mMainWindow->rect();
    setPreferredPos(QPointF(screenRect.width()/2, screenRect.height()/2), HbPopup::Center);
}


// ---------------------------------------------------------
// AlarmAlertDialogPrivate::handleClockAlarms
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::handleClockAlarms()
{
	// Get the reference to the snooze button
	HbPushButton *snoozeButton = qobject_cast<HbPushButton*> (mDocLoader->findWidget("snoozeButton"));
	Q_ASSERT_X(snoozeButton, "alarmalertwidget_p.cpp", "Unable to find snooze button");
	connect(snoozeButton, SIGNAL(clicked()), this, SLOT(snoozed()));
	
	if (!mCanSnooze) {
		snoozeButton->hide();
	}
}


// ---------------------------------------------------------
// AlarmAlertDialogPrivate::handleCalendarAlarms
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::handleCalendarAlarms()
{
	mSnoozeButton = qobject_cast<HbPushButton*> (mDocLoader->findWidget("snoozeButton"));
	Q_ASSERT_X(mSnoozeButton, "alarmalertwidget_p.cpp", "Unable to find snooze button");
	connect(mSnoozeButton, SIGNAL(clicked()), this, SLOT(silenced()));
	if(!mIsSilent) {
		mSnoozeButton->setText(hbTrId("txt_calendar_button_alarm_silence"));
		mIsSilenceButton = true;
	}
	// Hide the snooze button if the alarm cannot be snoozed
	if (!mCanSnooze) {
		mSnoozeButton->hide();
	}
}



// ---------------------------------------------------------
// AlarmAlertDialogPrivate::isScreenLocked
// rest of the details are commented in the header
// ---------------------------------------------------------
//
bool AlarmAlertDialogPrivate::isScreenLocked()
{
    // TODO: Replace with actual API which gets the screen lock status
    return false;
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::mousePressEvent
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // TODO: Must be removed once gestures are available
    mTapPoint = event->pos();
    qDebug() << "Tap event";
}

// ---------------------------------------------------------
// AlarmAlertDialogPrivate::mouseMoveEvent
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertDialogPrivate::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // TODO: THIS IS A HACK!!!
    // Must be removed once gestures are available
    QPointF curPos = event->pos();
    if (curPos.y() - mTapPoint.y() > 40) {
        qDebug() << "Sufficient drag";
        if (mUserResponse == Other) {
            dismissed();
        }
    }
}
