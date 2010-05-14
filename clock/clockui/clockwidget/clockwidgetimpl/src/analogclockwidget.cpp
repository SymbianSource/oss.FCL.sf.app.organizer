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
AnalogClockWidget::AnalogClockWidget(QGraphicsItem *parent)
    : HbWidget(parent)
{
    HbStyleLoader::registerFilePath(":/resource");
    updatePrimitives();
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), SLOT(tick()));
    mTimer->start(clockUpdateInterval);
}

/*!
    Destructor.
 */
AnalogClockWidget::~AnalogClockWidget()
{    
    mTimer->stop(); 
    HbStyleLoader::unregisterFilePath(":/resource");
}

/*!
    Handles resize event from HbWidget
 */
void AnalogClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QGraphicsWidget::resizeEvent(event);
    updatePrimitives();   
}

/*!
    @copydoc HbWidget::updatePrimitives()
 */
void AnalogClockWidget::updatePrimitives()
{
    if (!mClockBackground) {
        mClockBackground = new HbIconItem(QLatin1String("qtg_graf_clock_day_bg"), this);
        HbStyle::setItemName(mClockBackground, QLatin1String("clock_background"));
    }

    // Calculate angles for clock hands.
    QTime time = QTime::currentTime();
    qreal s = 6 * time.second();
    qreal m = 6 * (time.minute() + s/360);
    qreal h = 30 * ((time.hour() % 12) + m/360);

	if (!mClockHourHand) {
        mClockHourHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_hour"), this);
        HbStyle::setItemName(mClockHourHand, QLatin1String("clock_hour_hand"));
    }

    int x = mClockHourHand->geometry().width()/2;
    int y = mClockHourHand->geometry().height()/2;
    mClockHourHand->setTransform(QTransform().translate(x, y).rotate(h).translate(-x, -y));

	if (!mClockMinuteHand) {
        mClockMinuteHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_min"), this);
        HbStyle::setItemName(mClockMinuteHand, QLatin1String("clock_minute_hand"));
    }

    x = mClockMinuteHand->geometry().width()/2;
    y = mClockMinuteHand->geometry().height()/2;
    mClockMinuteHand->setTransform(QTransform().translate(x, y).rotate(m).translate(-x, -y));
    
      
    if (!mClockSecondHand) {
         mClockSecondHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_sec"), this);
        HbStyle::setItemName(mClockSecondHand, QLatin1String("clock_second_hand"));
        }

    x = mClockSecondHand->geometry().width()/2;
    y = mClockSecondHand->geometry().height()/2;
    mClockSecondHand->setTransform(QTransform().translate(x, y).rotate(s).translate(-x, -y));

}

/*!
    Updates clock visualization according to current time
 */
void AnalogClockWidget::tick()
{
    updatePrimitives();
    update();
}

/*!
    Handles polish event
 */
void AnalogClockWidget::polish( HbStyleParameters& params ) 
{  
    HbWidget::polish(params); 
    updatePrimitives();
} 

// End of file  --Don't remove this.
