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
 * Description: Test class for CalenDayContentWidget
 *
 */
#include <QGraphicsItem>
#include <QtTest/QtTest>


#include "calendayitem.h"
#include "calendaycontainer.h"

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

    void testConstructors();
    void testCreateItem();

private:
    CalenDayItem *mItem;
    CalenDayContainer *mContainer;
};

/*!
 Constructor
 */
TestCalenDayItem::TestCalenDayItem() :
   mItem(NULL)
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
    mItem = new CalenDayItem(mContainer);
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
    CalenDayItem *testItem = 0;
    QVERIFY(!testItem);
    
    //2)
    testItem = new CalenDayItem(mContainer);
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

QTEST_MAIN(TestCalenDayItem);
#include "unittest_calendayitem.moc"
