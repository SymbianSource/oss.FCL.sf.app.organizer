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
 * Description: Test class for CalenDayItem
 *
 */
#include <QGraphicsItem>
#include <QtTest/QtTest>
#include <QGraphicsSceneEvent>
#include <QDebug>


#include "agendaentry.h"
#include "calendaycontainer.h"

// System includes
#include <HbFrameItem>
#include <HbTextItem>
#include <HbStyle>
#include <HbColorScheme>
#include <agendaentry.h>

// User includes
#include "calendaycommonheaders.h"
#include "calendaystatusstrip.h"
#include "calendaymodel.h"
#include "calendayutils.h"
#include "calenagendautils.h"
#include "calendaycontainer.h"

#ifndef __WINSCW__
#define private public
#define protected public
#endif

#include "calendayitem.h"
#include "calendayitemtest.h"


QRectF gTestWindowRect;
Qt::Orientation gTestOrientation;

class TestCalenDayItem : public QObject
{
Q_OBJECT

public:
    TestCalenDayItem();
    virtual ~TestCalenDayItem();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testCreateItem();
    void testUpdateChildItems();
    void testHasEventDescription();
    void testHasBackgroundFrame();
    
    void testConstructors();
private:
    CalenDayItemTest *mItem;
    CalenDayContainer *mContainer;
};

/*!
 Constructor
 */
TestCalenDayItem::TestCalenDayItem() : mItem(NULL), mContainer(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayItem::~TestCalenDayItem()
{

}

/*!
 Called before testcase
 */
void TestCalenDayItem::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayItem::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayItem::init()
{
    mContainer = new CalenDayContainer();
    mItem = new CalenDayItemTest(mContainer);
#ifndef __WINSCW__
    mItem->mBg = new HbFrameItem();
    mItem->mEventDesc = new HbTextItem(0);
#endif
}

/*!
 Called after everyfunction
 */
void TestCalenDayItem::cleanup()
{
    if (mItem) {
        delete mItem;
        mItem = NULL;
    }
    
    if (mContainer) {
        delete mContainer;
        mContainer = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if content widget is not initialized
 2. Test if content widget is correcty created
 */
void TestCalenDayItem::testConstructors()
{
    //1)
    CalenDayItemTest *testItem = 0;
    QVERIFY(!testItem);
    
    //2)
    testItem = new CalenDayItemTest(mContainer);
    QVERIFY(testItem);
    delete testItem;
}

/*!
 Test function for creating new abstract items.
 1. Test if item is created
 2. Test if is the same as orginal.
 */
void TestCalenDayItem::testCreateItem()
{
    HbAbstractViewItem *testItem = mItem->createItem();
    QVERIFY(testItem);
    delete testItem;
}

void TestCalenDayItem::testUpdateChildItems()
{

}

void TestCalenDayItem::testHasEventDescription()
{
#ifndef __WINSCW__
    QGraphicsSceneResizeEvent *event = new QGraphicsSceneResizeEvent();
    qreal width = mItem->rect().width();

    qDebug() << "inited";

    mItem->mEventDescMinWidth = width - 4;

    qDebug() << "before resize";

    mItem->resizeEvent(event);

    qDebug() << "resize called";

    QVERIFY(mItem->hasEventDescription() == true);

    mItem->mEventDescMinWidth = width + 8;
    mItem->resizeEvent(event);

    qDebug() << "resize 2 called";

    QVERIFY(mItem->hasEventDescription() == false);
#endif
}

void TestCalenDayItem::testHasBackgroundFrame()
{
#ifndef __WINSCW__
    QGraphicsSceneResizeEvent *event = new QGraphicsSceneResizeEvent();
    qreal width = mItem->rect().width();

    mItem->mFrameMinWidth = width - 4;
    mItem->resizeEvent(event);

    QVERIFY(mItem->hasBackgroundFrame() == true);

    mItem->mFrameMinWidth = width + 8;
    mItem->resizeEvent(event);

    QVERIFY(mItem->hasBackgroundFrame() == false);
#endif
}

QTEST_MAIN(TestCalenDayItem);
#include "unittest_calendayitem.moc"