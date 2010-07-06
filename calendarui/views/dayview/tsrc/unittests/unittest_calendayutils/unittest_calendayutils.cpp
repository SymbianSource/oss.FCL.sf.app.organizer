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

#include "calendayutils.h"
#include "calendaycommonheaders.h"

// Test variables
QRectF gTestWindowRect = QRectF(0, 0, 10, 20);
Qt::Orientation gTestOrientation = Qt::Horizontal;

class TestCalenDayUtils : public QObject
{
Q_OBJECT

public:
    TestCalenDayUtils();
    virtual ~TestCalenDayUtils();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
    void testHorizontalSwipe();
    void testOrientation();

private:
    CalenDayUtils *mUtils;
};

/*!
 Constructor
 */
TestCalenDayUtils::TestCalenDayUtils() :
   mUtils(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayUtils::~TestCalenDayUtils()
{

}

/*!
 Called before testcase
 */
void TestCalenDayUtils::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayUtils::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayUtils::init()
{
}

/*!
 Called after everyfunction
 */
void TestCalenDayUtils::cleanup()
{
}

/*!
 Test function for constructors
 1. Test if content widget is not initialized
 2. Test if content widget is correcty created
 */
void TestCalenDayUtils::testConstructors()
{
    //1)
    CalenDayUtils *testUtils = 0;
    QVERIFY(!testUtils);
    
    //2)
    testUtils = CalenDayUtils::instance();
    QVERIFY(testUtils);
}

/*!
   Test function for checking if swipe is horizontal or not
   1)test Horizontal swipe
   2)test no horizontal swipe
 */
void TestCalenDayUtils::testHorizontalSwipe()
{
    //1)
    QCOMPARE(CalenDayUtils::instance()->isHorizontalSwipe(KCalenSwipeAngle-5), 
        true);
    
    //2)
    QCOMPARE(CalenDayUtils::instance()->isHorizontalSwipe(KCalenSwipeAngle+5),
        false);
}

/*!
   Test function for getting main window's orientation
   1)test if horizontal orientation is returned
   2)test if vertical orientation is returned
 */
void TestCalenDayUtils::testOrientation()
{
    //1)
    gTestOrientation = Qt::Horizontal;
    QCOMPARE(CalenDayUtils::instance()->orientation(), Qt::Horizontal);
    
    //2)
    gTestOrientation = Qt::Vertical;
    QCOMPARE(CalenDayUtils::instance()->orientation(), Qt::Vertical);
}

QTEST_MAIN(TestCalenDayUtils);
#include "unittest_calendayutils.moc"
