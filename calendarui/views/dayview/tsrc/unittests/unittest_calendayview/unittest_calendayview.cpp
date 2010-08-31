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

#include "calenservices.h"
#include "calendateutils.h"
#include "calendaymodelmanager.h"
#include "calendayview.h"

class TestCalenDayView : public QObject
{
Q_OBJECT

public:
    TestCalenDayView();
    virtual ~TestCalenDayView();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();

private:
    CalenDayView *mView;
};

/*!
 Constructor
 */
TestCalenDayView::TestCalenDayView() :
	mView(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayView::~TestCalenDayView()
{

}

/*!
 Called before testcase
 */
void TestCalenDayView::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayView::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayView::init()
{

}

/*!
 Called after everyfunction
 */
void TestCalenDayView::cleanup()
{

}

void TestCalenDayView::testConstructors()
{
	MCalenServices services;
    //1)
    CalenDayView *testView = 0;
    QVERIFY(!testView);
    
    //2)
    testView = new CalenDayView(services);
    QVERIFY(testView);
    
    delete testView;
}

QTEST_MAIN(TestCalenDayView);

#include "unittest_calendayview.moc"
