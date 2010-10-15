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
* Description: Class inherit CalenDayContainer needed to test
*
*/

#ifndef CALENDAYCONTAINERTEST_H
#define CALENDAYCONTAINERTEST_H

#include <QObject>
#include <HbModelIterator>
#include <HbAbstractItemView>

#include "calendayitem.h"
#include "calendaycontainer.h"

class CalenDayContainerTest : public CalenDayContainer
{
    Q_OBJECT

public:
    CalenDayContainerTest(QGraphicsObject *parent = 0)
    : CalenDayContainer(parent)
    {
        
    }
    
    
    friend class TestCalenDayContainer;
};

class CalenDayItemTest : public CalenDayItem
{
    Q_OBJECT

public:
    CalenDayItemTest(const CalenDayContainer *container)
    : CalenDayItem(container)
    {
        
    }
    
    friend class TestCalenDayContainer;
};

class CalenDayItemViewTest : public HbAbstractItemView
{
public:
    CalenDayItemViewTest(HbAbstractItemContainer *container,
                       HbModelIterator *iterator,
                       QGraphicsItem *parent = 0)
    : HbAbstractItemView(container, iterator, parent)
    {
    }
    
    int connectedSlots(const char * signal)
    {
        return receivers(signal);
    }
    
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible)
    {
        Q_UNUSED(index)
        Q_UNUSED(hint)
    }
    
    friend class TestCalenDayContainer;
};

#endif

