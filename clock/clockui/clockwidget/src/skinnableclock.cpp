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
*
*/

// skinnableclock.cpp

#include <QTimer>
#include <QTime>
#include <QGraphicsLinearLayout>
#include <hbaction.h>

#include "skinnableclock.h"
#include "analogclock.h"
#include "digitalclock.h"
#include "timezoneclient.h"
#include "settingsutility.h"

SkinnableClock::SkinnableClock(QGraphicsItem *parent)
:HbWidget(parent),
 mAnalogClock(0),
 mDigitalClock(0),
 mAnalog(true),
 mClockFormat24(true),
 mUpdateNeeded(true)
{
	// Enable the gestures for the widget and subscribe for "PAN" gestures.
/*	QApplication::setAttribute(Qt::AA_EnableGestures);
	grabGesture(Qt::PanGesture);*/

	qDebug("clock: SkinnableClock::SkinnableClock() -->");

	// Construct the settings utility.
	mSettingsUtility = new SettingsUtility(this);

	qDebug("clock: SkinnableClock::SkinnableClock() - constructed settingsutility.");

	// Construct the layout and the widgets.
	mLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
	mAnalogClock = new AnalogClock(this);
	mDigitalClock = new DigitalClock(this);

    // Set the clock type according to the seetings.
	// And construct the corresponding widget.
	QStringList clockTypeList;
	int typeIndex = mSettingsUtility->clockType(clockTypeList);
	if (0 == typeIndex) {
		setClockTypeAnalog(true);
    } else {
    	setClockTypeAnalog(false);
    }

    setLayout(mLayout);

    // Add the required effects.
    HbEffect::add(mAnalogClock,
                  QString(":/clock/disappear.fxml"),
                  "disappear");
    HbEffect::add(mDigitalClock,
                  QString(":/clock/disappear.fxml"),
                  "disappear");
    HbEffect::add(mAnalogClock,
                  QString(":/clock/appear.fxml"),
                  "appear");
    HbEffect::add(mDigitalClock,
                  QString(":/clock/appear.fxml"),
                  "appear");

    mClient = new TimezoneClient(this);
    qDebug("clock: SkinnableClock::SkinnableClock() - constructed tzclient");

    connect(mClient, SIGNAL(timechanged()),
            this, SLOT(updateDisplay()));
    connect(mClient, SIGNAL(timechanged()),
            this, SLOT(updateClockType()));

    // Start a timer.
    mTickTimer = new QTimer(this);
    QObject::connect(mTickTimer, SIGNAL(timeout()),
                     this, SLOT(updateDisplay()));
    // TODO: mTickTimer->start(60000 - 1000 * QTime::currentTime().second());
    mTickTimer->start(1000);

    qDebug("clock: SkinnableClock::SkinnableClock() <--");
}

SkinnableClock::~SkinnableClock()
{
    // No implementation yet.
}


bool SkinnableClock::clockTypeAnalog()
{
    return mAnalog;
}

bool SkinnableClock::clockFormat24()
{
    return mClockFormat24;
}

void SkinnableClock::updateDisplay(bool newTimer)
{
	if (newTimer) {
		// Start the timer again for 1 minute.
		// TODO: mTickTimer->start(60000 - 1000 * QTime::currentTime().second());
		mTickTimer->start(1000);
	}

	if (mAnalog) {
		mAnalogClock->updateDisplay();
	} else {
		mDigitalClock->updateDisplay();
	}
}

void SkinnableClock::updateClockType()
{
	// Update the clock type according to the seetings.
	QStringList clockTypeList;
	int typeIndex = mSettingsUtility->clockType(clockTypeList);

	if ((mAnalog && 1 == typeIndex) || (!mAnalog && 0 == typeIndex)) {
	    mUpdateNeeded = true;
	    mAnalog = !mAnalog;
		mDigitalClock->hide();
		mAnalogClock->hide();
    } else {
    	mUpdateNeeded = false;
    }
}

/*bool SkinnableClock::sceneEvent(QEvent *event)
{
	if (event->type() == QEvent::GraphicsSceneGesture) {
		QGraphicsSceneGestureEvent *gestureEvent =
			static_cast<QGraphicsSceneGestureEvent*>(event);
		if (const QGesture *gesture = gestureEvent->gesture(Qt::PanGesture)) {
			const QPanningGesture *panningGesture =
				static_cast<const QPanningGesture*>(gesture);

			if (Qt::GestureFinished == gesture->state()) {
				if (Qt::LeftDirection == panningGesture->direction()) {
					if (mAnalog) {
						mAnalogClock->showPrev();
					}
				} else if (Qt::RightDirection == panningGesture->direction()) {
					if (mAnalog) {
						mAnalogClock->showNext();
					}
				} else if (Qt::UpDirection == panningGesture->direction() ||
						Qt::DownDirection == panningGesture->direction()) {
					startChangeType();
				}
				event->accept();
				return true;
			}
			event->accept();
			return true;
		}
	}
	return HbWidget::sceneEvent(event);
}*/

void SkinnableClock::paint(QPainter *painter,
						   const QStyleOptionGraphicsItem *option,
						   QWidget *widget)
{
	Q_UNUSED(painter)
	Q_UNUSED(option)
	Q_UNUSED(widget)

	if (mUpdateNeeded) {
	    setClockTypeAnalog(mAnalog);
	    mUpdateNeeded = !mUpdateNeeded;
    }
}

void SkinnableClock::setClockTypeAnalog(bool analog)
{
	if (analog) {
		mDigitalClock->hide();
		mLayout->removeItem(mDigitalClock);
		mAnalogClock->show();
		mLayout->addItem(mAnalogClock);
	} else {
		mAnalogClock->hide();
		mLayout->removeItem(mAnalogClock);
		mDigitalClock->show();
		mLayout->addItem(mDigitalClock);
	}

	mAnalog = analog;

	// Update the display.
	updateDisplay(false);
}

void SkinnableClock::startChangeType()
{
	if (mAnalog) {
		HbEffect::start(mAnalogClock,
		                "disappear",
		                this,
		                "finishChangeType");
	} else {
		HbEffect::start(mDigitalClock,
		                "disappear",
		                this,
		                "finishChangeType");
	}
}

void SkinnableClock::finishChangeType(const HbEffect::EffectStatus &status)
{
	Q_UNUSED(status)

	setClockTypeAnalog(!mAnalog);

	if (mAnalog) {
		HbEffect::start(mAnalogClock,
		                "appear");
	} else {
		HbEffect::start(mDigitalClock,
		                "appear");
	}
}

// End of file
