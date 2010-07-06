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


#include <HbMainWindow>

#include "calendayinfo.h"
#include "calendaycommonheaders.h"

class TestCalenDayInfo : public QObject
{
Q_OBJECT

public:
    TestCalenDayInfo();
    virtual ~TestCalenDayInfo();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();

private:
    CalenDayInfo *mInfo;
};

/*!
 Constructor
 */
TestCalenDayInfo::TestCalenDayInfo() :
   mInfo(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayInfo::~TestCalenDayInfo()
{

}

/*!
 Called before testcase
 */
void TestCalenDayInfo::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayInfo::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayInfo::init()
{
    mInfo = new CalenDayInfo(CalenDayInfo::ETwo);
}

/*!
 Called after everyfunction
 */
void TestCalenDayInfo::cleanup()
{
    if (mInfo) {
        delete mInfo;
        mInfo = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if CalenDayInfo is not initialized
 2. Test if CalenDayInfo is correcty created
 */
void TestCalenDayInfo::testConstructors()
{
    //1)
    CalenDayInfo *testCalenDayInfo = 0;
    QVERIFY(!testCalenDayInfo);
    
    //2)
    testCalenDayInfo = new CalenDayInfo(CalenDayInfo::ETwo);
    QVERIFY(testCalenDayInfo);
}


QTEST_MAIN(TestCalenDayInfo);

#include "unittest_calendayinfo.moc"
