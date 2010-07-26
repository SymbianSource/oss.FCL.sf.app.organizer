/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AnalogClockWidget
*
*/

// System includes
#include <HbIconItem>
#include <HbStyleLoader>
#include <QTime>
#include <QTimer>

// User includes
#include "analogclockwidget.h"

// Constants
const int clockUpdateInterval  (1000); // msec

/*!
    \class AnalogClockWidget

    This class implements the analogclock widget which gets displayed
    in the clockmainview when the clocktype is set to analog type.
 */

/*!
    Constructor.
    \param parent The parent of type QGraphicsItem.
*/
AnalogClockWidget::AnalogClockWidget(QGraphicsItem *parent) : 
		HbWidget(parent)
{
	bool result = HbStyleLoader::registerFilePath(
			":/resource/analogclockwidget.widgetml");
	result = HbStyleLoader::registerFilePath(
			":/resource/analogclockwidget.css");

	constructPrimitives();
	mTimer = new QTimer(this);
	connect(mTimer, SIGNAL(timeout()), SLOT(tick()));
}

/*!
    Destructor.
 */
AnalogClockWidget::~AnalogClockWidget()
{
	mTimer->stop(); 
	HbStyleLoader::unregisterFilePath(":/resource/analogclockwidget.widgetml");
	HbStyleLoader::unregisterFilePath(":/resource/analogclockwidget.css");
}

/*!
	Constructs the widget primitive items.
 */
void AnalogClockWidget::constructPrimitives()
{
	if (!mClockBackground) {
		mClockBackground = new HbIconItem(
				QLatin1String("qtg_graf_clock_day_bg"), this);
		HbStyle::setItemName(
				mClockBackground, QLatin1String("clock_background"));
	}
	
	if (!mClockHourHand) {
		mClockHourHand = new HbIconItem(
				QLatin1String("qtg_graf_clock_day_hour"), this);
		HbStyle::setItemName(
				mClockHourHand, QLatin1String("clock_hour_hand"));
	}
	
	if (!mClockMinuteHand) {
		mClockMinuteHand = new HbIconItem(
				QLatin1String("qtg_graf_clock_day_min"), this);
		HbStyle::setItemName(
				mClockMinuteHand, QLatin1String("clock_minute_hand"));
	}
	
	if (!mClockSecondHand) {
		mClockSecondHand = new HbIconItem(
				QLatin1String("qtg_graf_clock_day_sec"), this);
		HbStyle::setItemName(
				mClockSecondHand, QLatin1String("clock_second_hand"));
	}
}

/*!
	Called on the derived classes to notify in cases when 
	the style primitives need to be updated.
 */
void AnalogClockWidget::updatePrimitives()
{
	if (!mClockBackground) {
		mClockBackground = new HbIconItem(
				QLatin1String("qtg_graf_clock_day_bg"), this);
		HbStyle::setItemName(
				mClockBackground, QLatin1String("clock_background"));
	}
	
	if (!mClockHourHand) {
		mClockHourHand = new HbIconItem(
				QLatin1String("qtg_graf_clock_day_hour"), this);
		HbStyle::setItemName(
				mClockHourHand, QLatin1String("clock_hour_hand"));
	}
	
	if (!mClockMinuteHand) {
		mClockMinuteHand = new HbIconItem(
				QLatin1String("qtg_graf_clock_day_min"), this);
		HbStyle::setItemName(
				mClockMinuteHand, QLatin1String("clock_minute_hand"));
	}
	
	if (!mClockSecondHand) {
		mClockSecondHand = new HbIconItem(
				QLatin1String("qtg_graf_clock_day_sec"), this);
		HbStyle::setItemName(
				mClockSecondHand, QLatin1String("clock_second_hand"));
	}
	
	QRectF hourHandGeometry = mClockHourHand->geometry();
	QRectF minHandGeometry = mClockMinuteHand->geometry();
	QRectF secHandGeometry = mClockSecondHand->geometry();
	
	if ((hourHandGeometry.width() && hourHandGeometry.height()) &&
			(minHandGeometry.width() && minHandGeometry.height()) &&
			(secHandGeometry.width() && secHandGeometry.height())) {
		// Calculate angles for clock hands.
		QTime curTime = QTime::currentTime();
		qreal seconds = 6 * curTime.second();
		qreal minutes = 6 * (curTime.minute() + seconds / 360);
		qreal hours = 30 * ((curTime.hour() % 12) + minutes / 360);

		int x = hourHandGeometry.width()/2;
		int y = hourHandGeometry.height()/2;
		mClockHourHand->setTransform(
				QTransform().translate(x, y).rotate(hours).translate(-x, -y));
		
		x = minHandGeometry.width()/2;
		y = minHandGeometry.height()/2;
		mClockMinuteHand->setTransform(
				QTransform().translate(x, y).rotate(minutes).translate(-x, -y));

		x = secHandGeometry.width()/2;
		y = secHandGeometry.height()/2;
		mClockSecondHand->setTransform(
				QTransform().translate(x, y).rotate(seconds).translate(-x, -y));
	}
	
	if (!mTimer->isActive()) {
		mTimer->start(clockUpdateInterval);
	}
}

/*!
    Updates clock visualization according to current time.
 */
void AnalogClockWidget::tick()
{
	updatePrimitives();
	update();
}

/*!
	Sets the item's geometry to rect.
 */
void AnalogClockWidget::setGeometry(const QRectF &rect)
{
	HbWidget::setGeometry(rect);
	updatePrimitives();
}

// End of file  --Don't remove this.
