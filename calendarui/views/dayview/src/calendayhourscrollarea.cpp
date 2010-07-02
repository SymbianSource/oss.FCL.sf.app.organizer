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
 * Description:  CalenDayHourScrollArea implementation.
 *
 */

// System includes
#include <QGraphicsLinearLayout>

// User includes
#include "calendayhourscrollarea.h"
#include "calendayhourelement.h"
#include "calendayutils.h"

/*!
 \class CalenDayHourScrollArea
 \brief Scrollable container class for hour elements.
 
 It handles vertical scrolling.
 */

/*!
 \brief Constructor
 
 Configures scroll area settings and resets internal stares of widget.
 Gets the width of device.
 
 \param parent The parent of scroll area widget
 */
CalenDayHourScrollArea::CalenDayHourScrollArea(QGraphicsItem *parent) :
    HbScrollArea(parent),
    mDateTime(QDateTime())
{
    // Set scroll settings
    setScrollDirections(Qt::Vertical);
    setScrollingStyle(HbScrollArea::PanWithFollowOn);
    setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
    grabGesture(Qt::PanGesture, Qt::ReceivePartialGestures);
    
    // Get height and width of hour elements and set fixed size policy
    qreal hourElementHeight = CalenDayUtils::instance()->hourElementHeight();
    qreal hourElementWidth = CalenDayUtils::instance()->hourElementWidth();

    // Create widget for hour elements
    HbWidget *hourWidget = new HbWidget();

    // Create and insert hour elements into vertical layout
    QGraphicsLinearLayout* hourLayout = new QGraphicsLinearLayout(Qt::Vertical,
        NULL);
    for (int i = 0; i < 24; i++) {
        CalenDayHourElement* element = new CalenDayHourElement(QTime(i, 0),
            this);
        element->setPreferredWidth(hourElementWidth);
        element->setPreferredHeight(hourElementHeight);
        element->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, 
            QSizePolicy::Fixed));
        hourLayout->addItem(element);
        
        mHourElements.append(element);
    }
    
    hourLayout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
    hourLayout->setSpacing(0.0);
    
    // Fix the size of scroll area
    setMinimumWidth(hourElementWidth);
    setMaximumWidth(hourElementWidth);

    // Apply hour layout for new widget and set content widget to scroll area
    hourWidget->setLayout(hourLayout);
    setContentWidget(hourWidget);
}

/*!
 \brief Destructor
 */
CalenDayHourScrollArea::~CalenDayHourScrollArea()
{
}

/*!
 \brief SLOT scrolls the view vertically to new position.
 
 \param newPosition New position
 */
void CalenDayHourScrollArea::scrollVertically(const QPointF &newPosition)
{
    QPointF currentPos = contentWidget()->pos();
    if (abs(newPosition.y()) != abs(currentPos.y())) {
        currentPos.setY(newPosition.y());
        scrollContentsTo(currentPos, 0);
    }
}

/*!
 \brief Sets date and time for this container.  
 
 \param dateTime new date and time
 */
void CalenDayHourScrollArea::setDateTime(const QDateTime &dateTime)
{
    mDateTime = dateTime;
}
/*!
 \brief Returns date and time assigned to current view.
*/
QDateTime CalenDayHourScrollArea::dateTime() const
{
    return mDateTime;
}

/*!
 \brief Scroll view to given hour.
 
  \param  An hour In 24 hour format (0 - 23)
*/
void CalenDayHourScrollArea::scrollToHour(int hour)
{
    CalenDayHourElement *hourElement = mHourElements.at(hour); 
    QRectF hourElementRect = hourElement->rect();
    hourElementRect = hourElement->mapRectToParent(hourElementRect);
    QPointF newPos = hourElementRect.topLeft();
    
    //Ensure that we won't scroll out of bound
    CalenDayHourElement *lastElement = mHourElements.last();
    qreal bottomEdge = lastElement->mapRectToParent(lastElement->rect()).bottom();
    qreal viewHeight = rect().height();

    if(bottomEdge - newPos.y() < viewHeight){
        newPos.setY(bottomEdge - viewHeight);
    }
    
    scrollContentsTo(newPos);
}

// End of File