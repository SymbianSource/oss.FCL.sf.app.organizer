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

#include <hbabstractitemview.h>
#include <hbmenu.h>

#define private public

#include "calendayitemview.h"


// Test variables
QRectF gTestWindowRect = QRectF(0, 0, 10, 20);
Qt::Orientation gTestOrientation = Qt::Horizontal;
quint32 SELECTED_COMMAND = 0; 

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
    void testIssueCommandOnSelectedItem();
    void testOpenSelectedItem();
    void testEditSelectedItem();
    void testDeleteSelectedItem();
private:
    MCalenServices   mMCalenServices;
    CalenDayItemView *mCalenDayItemView;

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
    HbModelIterator *iterator = new HbModelIterator();
    mCalenDayItemView = new CalenDayItemView(mMCalenServices,iterator,0);
    SELECTED_COMMAND = 0;
}

/*!
 Called after everyfunction
 */
void TestCalenItemView::cleanup()
{
    if(mCalenDayItemView){
        delete mCalenDayItemView;
        mCalenDayItemView = NULL;
    }
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

void TestCalenItemView::testIssueCommandOnSelectedItem()
{
#ifndef __WINSCW__
    mCalenDayItemView->issueCommandOnSelectedItem((quint32)ECalenEventView);
    QCOMPARE(SELECTED_COMMAND,(quint32)ECalenEventView);
#endif
}

void TestCalenItemView::testOpenSelectedItem()
{
#ifndef __WINSCW__
    mCalenDayItemView->openSelectedItem();
    QCOMPARE(SELECTED_COMMAND,(quint32)ECalenEventView);
#endif 
}

void TestCalenItemView::testEditSelectedItem()
{
#ifndef __WINSCW__
    mCalenDayItemView->editSelectedItem();
    QCOMPARE(SELECTED_COMMAND,(quint32)ECalenEditCurrentEntry);
#endif
}

void TestCalenItemView::testDeleteSelectedItem()
{
#ifndef __WINSCW__
    mCalenDayItemView->deleteSelectedItem();
    QCOMPARE(SELECTED_COMMAND,(quint32)ECalenDeleteCurrentEntry);
#endif
}

QTEST_MAIN(TestCalenItemView);
#include "unittest_calendayitemview.moc"
