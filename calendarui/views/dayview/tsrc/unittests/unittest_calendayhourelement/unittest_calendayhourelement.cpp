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

#include "calendayhourelement.h"

class TestCalenDayHourElement : public QObject
{
Q_OBJECT

public:
    TestCalenDayHourElement();
    virtual ~TestCalenDayHourElement();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();

private:
    CalenDayHourElement *mHourElement;
    QTime   mTime;
};

/*!
 Constructor
 */
TestCalenDayHourElement::TestCalenDayHourElement() :
   mHourElement(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayHourElement::~TestCalenDayHourElement()
{

}

/*!
 Called before testcase
 */
void TestCalenDayHourElement::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayHourElement::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayHourElement::init()
{
    mHourElement = new CalenDayHourElement(mTime);
}

/*!
 Called after everyfunction
 */
void TestCalenDayHourElement::cleanup()
{
    if (mHourElement) {
        delete mHourElement;
        mHourElement = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if content widget is not initialized
 2. Test if content widget is correcty created
 */
void TestCalenDayHourElement::testConstructors()
{
    //1)
    CalenDayHourElement *testHourElement = 0;
    QVERIFY(!testHourElement);
    
    testHourElement = new CalenDayHourElement(mTime);
    
    QVERIFY(testHourElement);
    
    delete testHourElement;
}

QTEST_MAIN(TestCalenDayHourElement);
#include "unittest_calendayhourelement.moc"
