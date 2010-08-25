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
 * Description:  Day view control of calendar
 *
 */

// System includes
#include <QPainter>
#include <QPen>
#include <HbColorScheme>

// User includes
#include "calendaystatusstrip.h"

// Constants
qreal const CalenDayStatusStrip::mMinute = 60;
qreal const CalenDayStatusStrip::mMinimumTime = 5;

/*!
  Constructor
 */
CalenDayStatusStrip::CalenDayStatusStrip(HbWidget *parent)
    : HbWidget(parent), mRange(2), 
      mDrawingStyle(CalenDayStatusStrip::Filled)
{
    setFlag(QGraphicsItem::ItemHasNoContents,false);

}

/*!
  Destructor
 */
CalenDayStatusStrip::~CalenDayStatusStrip()
{

}

/*!
   \brief Returns range between two filled lines in StripWithLines drawing 
   styles.
   
   \sa CalenDayStatusStrip::DrawingStyle, setRange
 */
qreal CalenDayStatusStrip::range() const
{
    return mRange;
}

/*!
   \brief Returns style of drawing.
   
   \sa CalenDayStatusStrip::DrawingStyle, setDrawingStyle
 */
CalenDayStatusStrip::DrawingStyle CalenDayStatusStrip::drawingStyle() const
{
    return mDrawingStyle;
}

/*!
   \brief Returns original start and entime fo event
   
   \sa setStartEndTime
 */
QPair<QTime,QTime> CalenDayStatusStrip::startEndTime() const
{
    return mStartEndEventTime;
}

/*!
   \brief It sets range between two filled lines in StripWithLines drawing 
   styles.
   
   \param range Range between two filled lines
   
   \sa CalenDayStatusStrip::DrawingStyle, range
 */
void CalenDayStatusStrip::setRange(qreal range)
{
    mRange = range;
}

/*!
   \brief It sets drawing style of strip 
   
   \param drawingStyle Style of strip drawing.
   
   \sa CalenDayStatusStrip::DrawingStyle, drawingStyle
 */
void 
CalenDayStatusStrip::setDrawingStyle(CalenDayStatusStrip::DrawingStyle drawingStyle)
{
    mDrawingStyle = drawingStyle;
}

/*!
   It sets start and end time of event
   
   \param startTime Start of event
   \param endTime End of event
 */
void CalenDayStatusStrip::setStartEndTime(const QTime &startTime, 
                                            const QTime &endTime)
{
    mStartEndEventTime.first = startTime;
    mStartEndEventTime.second = endTime;

    //check if startEndEvent is longer than mMinimumTimeminutes;
    if (mStartEndEventTime.first.secsTo(mStartEndEventTime.second) < mMinute * mMinimumTime) {
        mStartEndEventTime.second = QTime(mStartEndEventTime.first.hour(),
            mStartEndEventTime.first.minute() + mMinimumTime);
    }
}

/*!
   \brief Reimplemented function...
   
   Reimplemented function to draw status strip. 
   It is based on CalenDayStatusStrip::DrawingStyle, range and color
 */
void CalenDayStatusStrip::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    //calculate bubble start and end time of bubble
    QPair<QTime, QTime> startEndEvent = 
        calculateStartEndPostion(mStartEndEventTime.first,
                                 mStartEndEventTime.second
                                );
    //calculate how big is Minute
    qreal minuteHeight = calculateMinuteHeight(startEndEvent.first, 
                                               startEndEvent.second
                                              );

    painter->save();//   saves the painter state.
    

    //calculate how long is event in minutes
    qreal eventMinutes = 
           mStartEndEventTime.first.secsTo(mStartEndEventTime.second) / mMinute;
    //calculate height and width of status stripe
    qreal dx = size().width() - 1;
    qreal dy = eventMinutes * minuteHeight;

    //calculate time from wehre it should be drawed
    qreal startTime = 
                 startEndEvent.first.secsTo(mStartEndEventTime.first) / mMinute;
    //this is done because bubble can be drawed from half hour
    startTime = startTime > 30 ? startTime - 30 : startTime;
    //calculate status stripe height
    qreal startTimeHeight = startTime * minuteHeight;

    //set bounding rect of drawed area
    QRectF bounding(boundingRect());
    //set size smaller by 1px in each side
    bounding.setRect(bounding.left() + 1, bounding.top() + startTimeHeight, 
                     dx - 1, dy - 1
                    );

    //set clip region
    painter->setClipRect(bounding, Qt::IntersectClip);

    //prepare brush and paint
    QBrush brush(HbColorScheme::color("qtc_cal_month_current_day"));
    painter->setBrush(brush);
    QPen pen;
    pen.setWidth(1);
    pen.setBrush(brush);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);

    painter->setPen(pen);
    QPointF startPoint(0, dy + dx);
    
    switch (mDrawingStyle) {
        case StripWithLines:
            for (int i = 0; startPoint.y() > 0; i++) {
                painter->drawPolygon(diagonalLine(startPoint, dx, 3));
                startPoint.setY(startPoint.y() - 6 - mRange);
            }
        case OnlyFrame:
            painter->setBrush(Qt::NoBrush);
            break;
    }

    //draw rectangle
    painter->drawRect(bounding);

    // restore the painter
    painter->restore();
}

/*!
   \brief It preapres points to draw filled polygon when StripWithLines style is
   on.
 */
QPolygonF 
CalenDayStatusStrip::diagonalLine(QPointF startPoint, qreal dx, qreal dy)
{
    QPolygonF polygon;
    polygon << QPointF(startPoint.x(), startPoint.y());
    polygon << QPointF(startPoint.x(), startPoint.y() - dy);
    polygon << QPointF(startPoint.x() + dx, startPoint.y() - 2 * dy);
    polygon << QPointF(startPoint.x() + dx, startPoint.y() - dy);
    return polygon;
}

/*!
    Returns time for position 0 and height in widget 
    
    \param startTime Start of event
    \param endTime End of event
    \return Draw region of bubble
  */
 QPair<QTime,QTime> CalenDayStatusStrip::calculateStartEndPostion(
                                                 const QTime &startTime, 
                                                 const QTime &endTime
                                             )
{
     
    QTime start;
    QTime end;

    if (startTime.minute() < 30) {
        start = QTime(startTime.hour(), 0);
    }
    else {
        start = QTime(startTime.hour(), 30);
    }

    if (endTime.minute() == 0) {
        end = endTime;
    }
    else if (endTime.hour() == 23 and endTime.minute() > 30) {
        end = QTime(endTime.hour(), 59);
    }
    else if (endTime.minute() <= 30) {
        end = QTime(endTime.hour(), 30);
    }
    else {
        end = QTime(endTime.hour() + 1, 0);
    }

    return QPair<QTime, QTime> (start, end);
}

/*!
   Calculates height of one minute from widget height, and start/end time. 
 */
qreal CalenDayStatusStrip::calculateMinuteHeight(const QTime &start, 
                                                   const QTime &end)
{
    qreal min = start.secsTo(end) / mMinute;

    qreal height = size().height();

    return height / min;
}
