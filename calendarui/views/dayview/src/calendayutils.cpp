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
* Description:  CalenDayUtils utility class implementation.
*
*/

// System includes
#include <hbdeviceprofile.h>
#include <hbstyle.h>
#include <hbinstance.h>
#include <qdatetime.h>

// User includes
#include "calendayutils.h"
#include "calendaycommonheaders.h"
#include "agendaentry.h"
#include "calendateutils.h"

// Initialization of static member
CalenDayUtils* CalenDayUtils::mInstance = 0;

/*!
 \class CalenDayUtils
 \brief Singleton utility class.
 
 Class cannot be used in console applications.
 
 Provided functionalities (getters):
 - screen width of device
 - default width of hour element
 - default height of hour element
 - default width of content area
 - current orientation of screen
 - pointer to the main window of application
 */

/*!
 \brief Returns the instance of CalenDayUtils class
 */
CalenDayUtils *CalenDayUtils::instance()
{
    if (!mInstance) {
        mInstance = new CalenDayUtils();
    }
    return mInstance;
}

/*!
 \brief Destructor
 */
CalenDayUtils::~CalenDayUtils()
{
    
}

/*!
 \brief screenWidth
 
 \return Width of main window's screen
 */
qreal CalenDayUtils::screenWidth() const
{
    ASSERT(mMainWindow);
    
    return mMainWindow->layoutRect().width();
}

/*!
 \brief hourElementWidth
 
 \return Width of hour element
 */
qreal CalenDayUtils::hourElementWidth() const
{
    return mHourElementWidth;
}

/*!
 \brief hourElementHeight
 
 \return Height of hour element
 */
qreal CalenDayUtils::hourElementHeight() const
{
    return mHourElementHeight;
}

/*!
 \brief contentWidth
 
 \return Width of content area
 */
qreal CalenDayUtils::contentWidth() const
{
    return (screenWidth() - mHourElementWidth);
}

/*!
 \brief orientation
 
 \return Orientation of main window
 */
Qt::Orientation CalenDayUtils::orientation() const
{
    ASSERT(mMainWindow);
        
    return mMainWindow->orientation();
}

/*!
 \brief mainWindow
 
 \return Pointer to main window of application
 */
HbMainWindow* CalenDayUtils::mainWindow()
{
    ASSERT(mMainWindow);
    
    return mMainWindow;
}

/*!
 \brief isHorizontalSwipe
 
 \return TRUE if horizontal swipe was recognized (angle in specific range)
 */
bool CalenDayUtils::isHorizontalSwipe(qreal angle) const
{
    bool isHSwipe = false;
    if ((angle < KCalenSwipeAngle) || 
        ((angle > 180 - KCalenSwipeAngle) && (angle < 180 + KCalenSwipeAngle)) ||
        (angle > 360 - KCalenSwipeAngle)) {
        isHSwipe = true;
    }
    
    return isHSwipe;
}

/*!
 \brief getEventValidStartEndTime
 \brief Get event's valid start/end time from agenda entry.
 */
void CalenDayUtils::getEventValidStartEndTime( QDateTime& start, QDateTime& end, 
                                    const AgendaEntry& entry, QDateTime& currentDate )
{
    start = entry.startTime();
    end = entry.endTime();
    
    if ( !CalenDateUtils::onSameDay( start, currentDate ) ) {
        start = CalenDateUtils::beginningOfDay( currentDate );
    }
    
    if ( !CalenDateUtils::onSameDay( end, currentDate ) ) {
       QDateTime tommorrow( currentDate.addDays( 1 ));
       end = CalenDateUtils::beginningOfDay( tommorrow ).addSecs( -60 );
    }
}

/*!
 \brief Constructor
 */
CalenDayUtils::CalenDayUtils() : mMainWindow(NULL)
{
    if (HbInstance::instance()->allMainWindows().count() > 0) {
        mMainWindow = HbInstance::instance()->allMainWindows().first();
    }
    mHourElementWidth = calculateHourElementWidth();
    mHourElementHeight = calculateHourElementHeight();
}

/*!
 \brief Calculates the width of hour element according to UI spec.
 
 \return Calculated width of hour element
 */
qreal CalenDayUtils::calculateHourElementWidth() const
{
    HbStyle style;
    HbDeviceProfile deviceProfile;
    qreal unitInPixels = deviceProfile.unitValue();

    // Calculate element's preferred width
    qreal prefWidth = 0.0;
    qreal textWidth = 0.0;
    qreal horizontalSpacing = 0.0;
    
    textWidth = 8.04 * unitInPixels; // pix (according to UI spec)   
    style.parameter(QString("hb-param-margin-gene-middle-horizontal"), 
        horizontalSpacing, deviceProfile);
    prefWidth = horizontalSpacing * 2 + textWidth;
    
    return prefWidth;
}

/*!
 \brief Calculates the height of hour element according to UI spec.
 
 \return Calculated height of hour element
 */
qreal CalenDayUtils::calculateHourElementHeight() const
{
    HbStyle style;
    HbDeviceProfile deviceProfile;
    
    qreal unitInPixels = deviceProfile.unitValue();
    
    // Calculate element's preferred height
    qreal prefHeight = 0.0;
    qreal textHeight = 0.0;
    qreal verticalSpacing = 0.0;
        
    qreal bottomSpacer = 3.0 * unitInPixels;
    style.parameter(QString("hb-param-text-height-secondary"), textHeight, 
        deviceProfile);
    style.parameter(QString("hb-param-margin-gene-middle-vertical"), 
        verticalSpacing, deviceProfile);

    prefHeight = textHeight * 2; //time + ampm
    prefHeight += verticalSpacing * 2;
    prefHeight += bottomSpacer;
    
    return prefHeight;
}
