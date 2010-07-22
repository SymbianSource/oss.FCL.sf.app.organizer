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
 * Description: Test class for CalenDayModel
 *
 */
#include <QtTest/QtTest>

#include "calendaymodel.h"
#include "calenservices.h"

class TestCalenDayModel : public QObject
{
Q_OBJECT

public:
    TestCalenDayModel();
    virtual ~TestCalenDayModel();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();

private:
    QDateTime mDateTime;
    MCalenServices *mServices;
    CalenDayModel *mModel;
};

/*!
 Constructor
 */
TestCalenDayModel::TestCalenDayModel() :
    mServices(NULL), mModel(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayModel::~TestCalenDayModel()
{

}

/*!
 Called before testcase
 */
void TestCalenDayModel::initTestCase()
{
    mDateTime = QDateTime::currentDateTime();
    mServices = new MCalenServices();
}

/*!
 Called after testcase
 */
void TestCalenDayModel::cleanupTestCase()
{
    if (mServices) {
        delete mServices;
    }
}

/*!
 Called before every function
 */
void TestCalenDayModel::init()
{
    mModel = new CalenDayModel(mDateTime, *mServices);
}

/*!
 Called after everyfunction
 */
void TestCalenDayModel::cleanup()
{
    if (mModel) {
        delete mModel;
        mModel = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if model is not initialized
 2. Test if model is correcty created
 */
void TestCalenDayModel::testConstructors()
{
    //1)
    CalenDayModel *testModel = 0;
    QVERIFY(!testModel);
    
    //2)
    testModel = new CalenDayModel(mDateTime, *mServices);
    QVERIFY(testModel);
    
    delete testModel;
}

QTEST_MAIN(TestCalenDayModel);
#include "unittest_calendaymodel.moc"
