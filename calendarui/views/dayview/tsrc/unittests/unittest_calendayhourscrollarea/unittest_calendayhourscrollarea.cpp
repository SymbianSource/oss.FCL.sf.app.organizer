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

#include "calendayhourscrollarea.h"

class TestCalenDayHourScrollArea : public QObject
{
Q_OBJECT

public:
    TestCalenDayHourScrollArea();
    virtual ~TestCalenDayHourScrollArea();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();

private:
    CalenDayHourScrollArea *mHourScrollArea;
};

/*!
 Constructor
 */
TestCalenDayHourScrollArea::TestCalenDayHourScrollArea() :
   mHourScrollArea(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayHourScrollArea::~TestCalenDayHourScrollArea()
{

}

/*!
 Called before testcase
 */
void TestCalenDayHourScrollArea::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayHourScrollArea::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayHourScrollArea::init()
{
    mHourScrollArea = new CalenDayHourScrollArea();
}

/*!
 Called after everyfunction
 */
void TestCalenDayHourScrollArea::cleanup()
{
    if (mHourScrollArea) {
        delete mHourScrollArea;
        mHourScrollArea = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if hour scroll area is not initialized
 2. Test if hour scroll area is correcty created
 */
void TestCalenDayHourScrollArea::testConstructors()
{
    //1)
    CalenDayHourScrollArea *testHourScrollArea = 0;
    QVERIFY(!testHourScrollArea);
    
    testHourScrollArea = new CalenDayHourScrollArea();
    
    QVERIFY(testHourScrollArea);
    
    delete testHourScrollArea;
}

QTEST_MAIN(TestCalenDayHourScrollArea);
#include "unittest_calendayhourscrollarea.moc"
