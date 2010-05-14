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
* Description:  Clockwidget
*
*/

// System includes
#include <QGraphicsLinearLayout>

// User includes
#include "clockwidget.h"
#include "analogclockwidget.h"

/*!
    \class ClockWidget

    This is the generic wrapper for the clockwidget which manages analog and ditial clockwidgets.
 */

/*!
    Constructor.
 */
ClockWidget::ClockWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),
      mClockType(ClockTypeAnalog)
{    
    loadClockWidget();
}

/*!
    Destructor.
*/
ClockWidget::~ClockWidget()
{    
    
}

/*!
    Returns the clock type.
*/
ClockWidget::ClockType ClockWidget::clockType() const
{
    return mClockType;
}
  
/*!
    Sets the clock type;
*/
void ClockWidget::setClockType(const ClockType &type)
{
    if (type == ClockTypeDigital) {
        mClockType = ClockTypeDigital;
    } else {
        mClockType = ClockTypeAnalog;
    }
}

/*!
    Updates the clock time with every second
*/
void ClockWidget::updateTime()
{
    if (mClockType == ClockTypeAnalog) {  
        mAnalogClock->tick();    
    }
}

/*!
    Constructs the clockwidget based upon its type.
*/
void ClockWidget::loadClockWidget()
{
    if (mClockType == ClockTypeAnalog) {
        mAnalogClock = new AnalogClockWidget(this);        
        mLayout = new QGraphicsLinearLayout(Qt::Vertical);
        mLayout->setContentsMargins(0,0,0,0);
        mLayout->addItem(mAnalogClock);
        setLayout(mLayout);
    }
}

// End of file  --Don't remove this.
