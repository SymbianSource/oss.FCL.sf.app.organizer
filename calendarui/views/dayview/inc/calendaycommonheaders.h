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
* Description:  CalenDay commons.
*
*/

#ifndef CALENDAYCOMMONHEADERS_H_
#define CALENDAYCOMMONHEADERS_H_

// Constants

/*!
 Default timeout for scrolling between days [ms]
 */
const int KCalenScrollDaysTimeout = 600;

/*!
 Value [%] defines how long (depending on content area width) should horizontal
 pan gesture be to change day to previous/next.
 If the gesture is shorter - current view is not changed.
 */
const int KCalenHScrollMoveParam = 30;  //!< Percentage

/*!
 Value [degree] defines the max. angle of swipe gesture which should change day.
 */
const qreal KCalenSwipeAngle = 30;


// Enumerations

/*!
 \enum CalenPanDirection
 \brief Enumeration identifies pan gesture directions.
 */
enum CalenPanDirection
{
    ECalenPanNotSet = 0,
    ECalenPanVertical,
    ECalenPanHorizontal
};

/*!
 \enum CalenScrollDirection
 \brief Enumeration identifies horizontal scroll directions of scroll area.
 
 ECalenScrollNoDayChange means that move does not change the current day.
 */
enum CalenScrollDirection
{
    ECalenScrollNoDayChange = 0,
    ECalenScrollToNext,
    ECalenScrollToPrev
};

#endif /* CALENDAYCOMMONHEADERS_H_ */
