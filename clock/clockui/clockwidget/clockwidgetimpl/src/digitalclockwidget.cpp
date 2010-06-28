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
* Description:  DigitalClockWidget
*
*/

// System includes
#include <HbStyleLoader>
#include <HbLabel>
#include <HbTextItem>
#include <hbextendedlocale>
#include <QGraphicsSceneMouseEvent>
#include <QSizePolicy>
#include <QTimer>
#include <QTime>

// User includes
#include "digitalclockwidget.h"

// Constants
const int clockUpdateInterval  (60000); // msec

/*!
    \class DigitalClockWidget

    This class implements the digitalclock widget which gets displayed
    in the clockmainview when the clocktype is set to digital type.
*/

/*!
    Constructor.
    \param useAmPm bool for setting 12 hour format
    \param parent The parent of type QGraphicsItem.
*/
DigitalClockWidget::DigitalClockWidget(bool useAmPm, QGraphicsItem *parent)
  : HbWidget(parent),
    mUseAmPm(useAmPm)
{
    bool result = HbStyleLoader::registerFilePath(":/resource/digitalclockwidget.widgetml");
    result = HbStyleLoader::registerFilePath(":/resource/digitalclockwidget.css");
    result = HbStyleLoader::registerFilePath(":/resource/digitalclockwidget_color.css");

    createPrimitives();
    
    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), SLOT(tick()));
    
    QTime time = QTime::currentTime();
    int initialIntervalTime = (clockUpdateInterval - ( time.msec() + (time.second() * 1000)));
    mTimer->start(initialIntervalTime);
    
}

/*!
    Destructor.
*/
DigitalClockWidget::~DigitalClockWidget()
{   
    mTimer->stop();
    HbStyleLoader::unregisterFilePath(":/resource/digitalclockwidget.widgetml");
    HbStyleLoader::unregisterFilePath(":/resource/digitalclockwidget.css");
    HbStyleLoader::unregisterFilePath(":/resource/digitalclockwidget_color.css");

}

/*!
    Updates clock visualization according to current time
 */
void DigitalClockWidget::tick()
{
    mTimer->setInterval(clockUpdateInterval);
    updatePrimitives();
    update();
}

/*!
    Toggles time format
 */
void DigitalClockWidget::setAmPm(bool useAmPm)
{
    
    bool old = mUseAmPm;
    mUseAmPm = useAmPm;
    
    if (mUseAmPm != old) { // change am/pm label
	    QTime time = QTime::currentTime();
	    QString timeString;
	    if (mUseAmPm) {
	    	mAmPmLabel->setText(time.toString("ap")); // set am/pm label
	    } else {
	    	mAmPmLabel->setText(""); // set am/pm label as null
	    }
    }
    
}

/*!
    @copydoc HbWidget::resizeEvent()
 */
void DigitalClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    HbWidget::resizeEvent(event);
}

/*!
    Creates all widget primitives.
 */
void DigitalClockWidget::createPrimitives()
{
    mClockLabel = new HbTextItem(this);    
    mClockLabel->setTextWrapping( Hb::TextNoWrap );
    HbStyle::setItemName(mClockLabel, QLatin1String("clockLabel"));
    
    mAmPmLabel = new HbTextItem(this);
    mClockLabel->setTextWrapping( Hb::TextNoWrap );
    HbStyle::setItemName(mAmPmLabel, QLatin1String("amPmLabel"));
}

/*!
    @copydoc HbWidget::updatePrimitives()
    updates all widget primitives
 */
void DigitalClockWidget::updatePrimitives()
{

    QTime time = QTime::currentTime();
    QString timeString;
    
    if (mUseAmPm) {
	    timeString = time.toString("hh:ap"); // covert time in 12 hours format
	    timeString = timeString.section(":", 0, 0); // it would remove :ap section from timeString
	    mAmPmLabel->setText(time.toString("ap")); // set am/pm label
	} else {
	    mAmPmLabel->setText(""); // set am/pm label as null
	    timeString = time.toString("hh"); // append hour part
    }

    int index(1);
    HbExtendedLocale locale;
    QChar timeSeparator(locale.timeSeparator(index)) ;
    timeString.append(timeSeparator); // append time separator
    timeString.append(time.toString("mm")); //append minute part

    mClockLabel->setText(timeString);

}
