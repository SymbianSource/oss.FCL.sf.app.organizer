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
 * Description: Test class for CalenDayContainerWidget
 *
 */
#include <QtTest/QtTest>

#include "calendaycontainer.h"

// Test variables
QRectF gTestWindowRect = QRectF(0, 0, 10, 20);
Qt::Orientation gTestOrientation = Qt::Horizontal;

class TestCalenDayContainer : public QObject
{
Q_OBJECT

public:
    TestCalenDayContainer();
    virtual ~TestCalenDayContainer();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();

private:
    CalenDayContainer *mContainer;
};

/*!
 Constructor
 */
TestCalenDayContainer::TestCalenDayContainer() :
    mContainer(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayContainer::~TestCalenDayContainer()
{

}

/*!
 Called before testcase
 */
void TestCalenDayContainer::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayContainer::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayContainer::init()
{
    mContainer = new CalenDayContainer();
}

/*!
 Called after everyfunction
 */
void TestCalenDayContainer::cleanup()
{
    if (mContainer) {
        delete mContainer;
        mContainer = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if container is not initialized
 2. Test if container is correcty created
 */
void TestCalenDayContainer::testConstructors()
{
    //1)
    CalenDayContainer *testContainer = 0;
    QVERIFY(!testContainer);
    
    //2)
    testContainer = new CalenDayContainer();
    QVERIFY(testContainer);
    
    delete testContainer;
}

QTEST_MAIN(TestCalenDayContainer);
#include "unittest_calendaycontainer.moc"
