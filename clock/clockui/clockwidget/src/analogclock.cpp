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

// analogclock.cpp

#include <QTime>
#include <QTimer>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsWidget>
#include <hbiconitem.h>
#include <hbstackedlayout.h>

#include "analogclock.h"

const QString KPrefix(":/clock/");
const QString KUnderScore("_");
const QString KFaceDay(":/clock/analog_face_day");
const QString KHourDay(":/clock/hour_hand_day");
const QString KMinDay(":/clock/minute_hand_day");
const QString KFaceNight("analog_face_night");
const QString KHourNight("hour_hand_night");
const QString KMinNight("minute_hand_night");

AnalogClock::AnalogClock(QGraphicsWidget *parent)
:HbWidget(parent),
 mIsDay(true),
 mFirstDraw(true),
 mCurrentClock(0)
{
	// Add the required effects.
	HbEffect::add(this, 
	              QString(":/clock/hide_widget.fxml"), 
	              "hide_widget");
	
	// Construct the images here.
	constructImages();

	// A single shot timer after which the hands are updated to show the
	// correct time.
	// TODO: QTimer::singleShot(500, this, SLOT(updateDisplay()));
}

AnalogClock::~AnalogClock()
{
	// No implementation yet.
}

void AnalogClock::showPrev()
{
	mCurrentClock--;
	if (mCurrentClock < 0) {
		mCurrentClock = 3;
	}
	// Start the effect.
	HbEffect::start(this, "hide_widget", this, "widget_hidden");
}

void AnalogClock::showNext()
{
	mCurrentClock++;
	mCurrentClock = mCurrentClock % 4;
	
	// Start the effect.
	HbEffect::start(this, "hide_widget", this, "widget_hidden");
}

void AnalogClock::updateDisplay()
{
	// Take the current time.
	bool changeClock = false;
	QTime timeNow = QTime::currentTime();
	int hourNow = timeNow.hour();

	if (17 < hourNow || 6 > hourNow) {
		if (mIsDay) {
			// It was previously day. We need to change the clock
			changeClock = true;
		}
		mIsDay = false;
	} else {
		if (!mIsDay) {
			// It was previously night. We need to change the clock
			changeClock = true;
		}
		mIsDay = true;
	}

	if (changeClock) {
		if (mIsDay) {
			mClockFace->setIconName(mFaceName);
			mHourHand->setIconName(mHourName);
			mMinuteHand->setIconName(mMinuteName);
		} else {
			mClockFace->setIconName(mFaceName);
			mHourHand->setIconName(mHourName);
			mMinuteHand->setIconName(mMinuteName);
		}
	}

	// Calculate the rotation angle for hour hand.
	qreal hourAngle = 30.0*((timeNow.hour()+timeNow.minute()/60.0)); 
	hourAngle += 270;

	// Rotate the hour hand.
	mHourHand->setTransform(
			QTransform().translate(mHourHand->iconItemSize().width()/2, 
					mHourHand->iconItemSize().height()/2).rotate(hourAngle)
					.translate(-mHourHand->iconItemSize().width()/2,
							-mHourHand->iconItemSize().height()/2));

	// Rotate the minute hand.
	mMinuteHand->setTransform(
			QTransform().translate(mMinuteHand->iconItemSize().width()/2, 
					mMinuteHand->iconItemSize().height()/2)
					.rotate(6.0*(timeNow.minute()+timeNow.second()/60.0))
					.translate(-mMinuteHand->iconItemSize().width()/2,
							-mMinuteHand->iconItemSize().height()/2));
}

void AnalogClock::paint(QPainter *painter, 
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget)
{
	Q_UNUSED(painter)
	Q_UNUSED(option)
	Q_UNUSED(widget)
	
	if (mFirstDraw) {
		mFirstDraw = false;
		
		QTime timeNow = QTime::currentTime();
		int hourNow = timeNow.hour();

		// Calculate the rotation angle for hour hand.
		qreal hourAngle = 30.0 * ((timeNow.hour() + timeNow.minute() / 60.0)); 
		hourAngle += 270;

		// Rotate the hour hand.
		mHourHand->setTransform(
				QTransform().translate(mHourHand->iconItemSize().width()/2, 
						mHourHand->iconItemSize().height()/2).rotate(hourAngle)
						.translate(-mHourHand->iconItemSize().width()/2,
								-mHourHand->iconItemSize().height()/2));

		// Rotate the minute hand.
		mMinuteHand->setTransform(
				QTransform().translate(mMinuteHand->iconItemSize().width()/2, 
						mMinuteHand->iconItemSize().height()/2)
						.rotate(6.0*(timeNow.minute()+timeNow.second()/60.0))
						.translate(-mMinuteHand->iconItemSize().width()/2,
								-mMinuteHand->iconItemSize().height()/2));
		
	}
}

void AnalogClock::constructImages()
{
	mFaceName.append(KFaceDay);
	mHourName.append(KHourDay);
	mMinuteName.append(KMinDay);
	
	mParentLayout = new HbStackedLayout(this);

	mClockFace = new HbIconItem(mFaceName, this);
	mHourHand = new HbIconItem(mHourName, this);
	mMinuteHand = new HbIconItem(mMinuteName, this);

	mParentLayout->addItem(mClockFace);
	mParentLayout->addItem(mHourHand);
	mParentLayout->addItem(mMinuteHand);

	setLayout(mParentLayout);
}

void AnalogClock::widget_hidden(const HbEffect::EffectStatus &status)
{
	Q_UNUSED(status)
	
	QString clockNum;
	clockNum = clockNum.setNum(mCurrentClock);
	
	mFaceName.clear();
	mHourName.clear();
	mMinuteName.clear();
	
	if (mIsDay) {
		mFaceName.append(KPrefix);
		mFaceName.append(clockNum);
		mFaceName.append(KUnderScore);
		mFaceName.append(KFaceDay);

		mHourName.append(KPrefix);
		mHourName.append(clockNum);
		mHourName.append(KUnderScore);
		mHourName.append(KHourDay);

		mMinuteName.append(KPrefix);
		mMinuteName.append(clockNum);
		mMinuteName.append(KUnderScore);
		mMinuteName.append(KMinDay);
	} else {
		mFaceName.append(KPrefix);
		mFaceName.append(clockNum);
		mFaceName.append(KUnderScore);
		mFaceName.append(KFaceNight);

		mHourName.append(KPrefix);
		mHourName.append(clockNum);
		mHourName.append(KUnderScore);
		mHourName.append(KHourNight);

		mMinuteName.append(KPrefix);
		mMinuteName.append(clockNum);
		mMinuteName.append(KUnderScore);
		mMinuteName.append(KMinNight);
	}
	
	mClockFace->setIconName(mFaceName);
	mHourHand->setIconName(mHourName);
	mMinuteHand->setIconName(mMinuteName);
	
	HbEffect::add(this, QString(":/clock/show_widget.fxml"), "show_widget");
	HbEffect::start(this, "show_widget");
}

// End of file
