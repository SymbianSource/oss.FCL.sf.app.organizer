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

#ifndef SKINNABLECLOCK_H
#define SKINNABLECLOCK_H

#include <QObject>
#include <hbwidget.h>
#include <hbeffect.h>

#include "clockwidgetdefines.h"

class QTimer;
class QEvent;
class QGraphicsLinearLayout;
class AnalogClock;
class DigitalClock;
class TimezoneClient;
class SettingsUtility;

class CLOCKWIDGET_EXPORT SkinnableClock : public HbWidget
{
    Q_OBJECT

public:
    SkinnableClock(QGraphicsItem *parent = 0);
    ~SkinnableClock();

public:
    bool clockTypeAnalog();
    bool clockFormat24();

public slots:
    void updateDisplay(bool newTimer = true);
    void updateClockType();
    void finishChangeType(const HbEffect::EffectStatus &status);

protected:
//	bool sceneEvent(QEvent *event);
	void paint(QPainter *painter,
			   const QStyleOptionGraphicsItem *option,
			   QWidget *widget = 0);

private:
    void setClockTypeAnalog(bool analog);
    void startChangeType();

private:
    QTimer *mTickTimer;
    QGraphicsLinearLayout *mLayout;

    AnalogClock *mAnalogClock;
    DigitalClock *mDigitalClock;
    TimezoneClient *mClient;
    SettingsUtility *mSettingsUtility;

    bool mAnalog;
    bool mClockFormat24;
    bool mUpdateNeeded;
};

#endif // SKINNABLECLOCK_H

// End of file
