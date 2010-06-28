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
#include <QPen>

#include "calendayeventspane.h"

class TestCalenEventsPane : public QObject
{
Q_OBJECT

public:
    TestCalenEventsPane();
    virtual ~TestCalenEventsPane();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();

private:
    CalenDayEventsPane *mEventsPane;
};

/*!
 Constructor
 */
TestCalenEventsPane::TestCalenEventsPane() :
   mEventsPane(NULL)
{

}

/*!
 Destructor
 */
TestCalenEventsPane::~TestCalenEventsPane()
{

}

/*!
 Called before testcase
 */
void TestCalenEventsPane::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenEventsPane::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenEventsPane::init()
{
    mEventsPane = new CalenDayEventsPane();
}

/*!
 Called after everyfunction
 */
void TestCalenEventsPane::cleanup()
{
    if (mEventsPane) {
        delete mEventsPane;
        mEventsPane = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if events pane is not initialized
 2. Test if events pane is correcty created
 */
void TestCalenEventsPane::testConstructors()
{
    //1)
    CalenDayEventsPane *testEventsPane = 0;
    QVERIFY(!testEventsPane);
    
    //2)
    testEventsPane = new CalenDayEventsPane();
    QVERIFY(testEventsPane);
    delete testEventsPane;
}

QTEST_MAIN(TestCalenEventsPane);
#include "unittest_calendayeventspane.moc"
