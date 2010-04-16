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

// digitalclock.h

#ifndef DIGITALCLOCK_H
#define DIGITALCLOCK_H

#include <QObject>

#include <hbwidget.h>
#include <hbeffect.h>

class QGraphicsLinearLayout;
class QGraphicsWidget;
class HbIconItem;
class HbLabel;

class DigitalClock : public HbWidget
{
    Q_OBJECT

public:
    DigitalClock(QGraphicsWidget *parent = 0);
    ~DigitalClock();

public:
    void updateDisplay();

public slots:
	void fadeBackIn(const HbEffect::EffectStatus &status);

private:
    void constructImages();

private:
    QGraphicsLinearLayout *mDigitalClockLayout;
    QGraphicsLinearLayout *mWidgetLayout;
    QGraphicsWidget *mDummyWidget;

    HbIconItem *mPositionOne;
    HbIconItem *mPositionTwo;
    HbIconItem *mPositionThree;
    HbIconItem *mPositionFour;
    HbIconItem *mSeperator;
    HbLabel *mAmPmLabel;

    bool m24HourFormat;
};

#endif // DIGITALCLOCK_H

// End of file
