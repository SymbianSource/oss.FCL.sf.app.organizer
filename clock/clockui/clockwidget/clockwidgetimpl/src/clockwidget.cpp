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
#include "digitalclockwidget.h"

/*!
    \class ClockWidget

    This is the generic wrapper for the clockwidget which manages analog and ditial clockwidgets.
 */

/*!
    Constructor.
 */
ClockWidget::ClockWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),
      mClockType(ClockTypeDigital)
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
    if (type == ClockTypeAnalog) {
        if(type != mClockType){
	        mClockType = ClockTypeAnalog;
	        updateClockWidget();
        }
    } else {
        if(type != mClockType){
            mClockType = ClockTypeDigital;
            updateClockWidget();
        }      
    }
}

/*!
    Updates the clock time with every second
*/
void ClockWidget::updateTime()
{
    if (mClockType == ClockTypeAnalog) {  
        mAnalogClock->tick();    
    } else {
    	mDigitalClock->updatePrimitives();
    }
}

/*!
    Constructs the clockwidget based upon its type.
*/
void ClockWidget::loadClockWidget()
{
    mLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mLayout->setContentsMargins(0,0,0,0); 

    if (mClockType == ClockTypeAnalog) {
	    mAnalogClock = new AnalogClockWidget(this);        
	    mLayout->addItem(mAnalogClock);
    } else {
	    bool useAmPm = false;
	    mDigitalClock = new DigitalClockWidget(useAmPm, this);
	    mLayout->addItem(mDigitalClock);
    }
    setLayout(mLayout);  
}

/*!
    Constructs the clockwidget based upon its type.
*/
void ClockWidget::updateClockWidget()
{
    if (mClockType == ClockTypeAnalog) {        
        mLayout->removeItem(mDigitalClock);
        delete mDigitalClock;
        if (!mAnalogClock) {
        	mAnalogClock = new AnalogClockWidget(this);
        }
        mLayout->addItem(mAnalogClock);
    } else {
        mLayout->removeItem(mAnalogClock);
        delete mAnalogClock;
        if(!mDigitalClock){
        bool useAmPm = false; // find out this fronm the settings utility
        mDigitalClock = new DigitalClockWidget(useAmPm, this);
    }
    mLayout->addItem(mDigitalClock);
    }           
}

ClockWidget::TimeFormat ClockWidget::timeFormat() const
{
    return mTimeFormat;
}

void ClockWidget::setTimeFormat(const TimeFormat &timeFormat)
{
    if(mDigitalClock){
	    mTimeFormat = timeFormat;
		if (timeFormat == ClockWidget::TimeFormat12Hrs) {
			mDigitalClock->setAmPm(true);
		} else {
			mDigitalClock->setAmPm(false);
		}
    }
}
// End of file  --Don't remove this.
