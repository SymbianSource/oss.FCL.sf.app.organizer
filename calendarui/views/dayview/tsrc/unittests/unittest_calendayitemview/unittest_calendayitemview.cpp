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
 * Description:
 *
 */
#include <QGraphicsItem>
#include <hbmodeliterator.h>
#include <QtTest/QtTest>

#include "calenservices.h"
#include "calendayitemview.h"

// Test variables
QRectF gTestWindowRect = QRectF(0, 0, 10, 20);
Qt::Orientation gTestOrientation = Qt::Horizontal;

class TestCalenItemView : public QObject
{
Q_OBJECT

public:
    TestCalenItemView();
    virtual ~TestCalenItemView();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
private:
    MCalenServices   mMCalenServices;

};

/*!
 Constructor
 */
TestCalenItemView::TestCalenItemView()
{

}

/*!
 Destructor
 */
TestCalenItemView::~TestCalenItemView()
{

}

/*!
 Called before testcase
 */
void TestCalenItemView::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenItemView::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenItemView::init()
{

}

/*!
 Called after everyfunction
 */
void TestCalenItemView::cleanup()
{

}

/*!
 Test function for constructors
 1. Test if content widget is not initialized
 2. Test if content widget is correcty created
 */
void TestCalenItemView::testConstructors()
{
    //1)
    CalenDayItemView *testItemView = 0;
    QVERIFY(!testItemView);
    
    //2)
    HbModelIterator *iterator = new HbModelIterator();
    testItemView = new CalenDayItemView(mMCalenServices,iterator,0);
    QVERIFY(testItemView);
    delete testItemView;
}

QTEST_MAIN(TestCalenItemView);
#include "unittest_calendayitemview.moc"
