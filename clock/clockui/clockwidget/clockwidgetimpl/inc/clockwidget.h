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
* Description:  Clock widget
*
*/

#ifndef CLOCKWIDGET_H
#define CLOCKWIDGET_H

// System includes
#include <hbwidget.h>

// User includes
#include "clockwidgetdefines.h"

//Forward declarations

class QGraphicsLinearLayout;
class AnalogClockWidget;

class CLOCKWIDGET_EXPORT  ClockWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(ClockType clockType READ clockType WRITE setClockType)

public:
    enum ClockType
    {
        ClockTypeAnalog,
        ClockTypeDigital
    };

    ClockWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~ClockWidget();
    ClockType clockType() const;
    void setClockType(const ClockType &type);
    void loadClockWidget();

public slots:
    void updateTime();
    
private:
    ClockType mClockType;
    AnalogClockWidget *mAnalogClock;
    QGraphicsLinearLayout *mLayout;
};

#endif // CLOCKWIDGET_H

// End of file  --Don't remove this.
