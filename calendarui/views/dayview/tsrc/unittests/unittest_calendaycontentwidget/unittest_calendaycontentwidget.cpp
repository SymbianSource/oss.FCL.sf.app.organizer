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


#include "calendaymodelmanager.h"
#include "calendaycontentwidget.h"

class TestCalenContentWidget : public QObject
{
Q_OBJECT

public:
    TestCalenContentWidget();
    virtual ~TestCalenContentWidget();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();

private:
    CalenDayModelManager *mModelMgr;
    CalenDayContentWidget *mContentWidget;
};

/*!
 Constructor
 */
TestCalenContentWidget::TestCalenContentWidget() :
    mModelMgr(NULL), mContentWidget(NULL)
{

}

/*!
 Destructor
 */
TestCalenContentWidget::~TestCalenContentWidget()
{

}

/*!
 Called before testcase
 */
void TestCalenContentWidget::initTestCase()
{
    mModelMgr = new CalenDayModelManager();
}

/*!
 Called after testcase
 */
void TestCalenContentWidget::cleanupTestCase()
{
    if (mModelMgr) {
        delete mModelMgr;
    }
}

/*!
 Called before every function
 */
void TestCalenContentWidget::init()
{

}

/*!
 Called after everyfunction
 */
void TestCalenContentWidget::cleanup()
{
    if (mContentWidget) {
        delete mContentWidget;
        mContentWidget = NULL;
    }
}

/*!
 Test class for constructors
 1. Test if content widget is correcty created
 */
void TestCalenContentWidget::testConstructors()
{
    QVERIFY(!mContentWidget);
    mContentWidget = new CalenDayContentWidget(*mModelMgr);
    QVERIFY(mContentWidget);
}

QTEST_MAIN(TestCalenContentWidget);
#include "unittest_calendaycontentwidget.moc"
