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
#include "calendaymodel.h"
#include "calendaymodelmanager.h"

class TestCalenDayModelManager : public QObject
{
Q_OBJECT

public:
    TestCalenDayModelManager();
    virtual ~TestCalenDayModelManager();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testGetModel();
    void testRefreshAllModels();
    void testRefreshSingleModel();
    
    void testConstructors();

private:
	MCalenServices services;
    CalenDayModelManager *mManager;
};

/*!
 Constructor
 */
TestCalenDayModelManager::TestCalenDayModelManager() :
mManager(NULL)
{
	
}

/*!
 Destructor
 */
TestCalenDayModelManager::~TestCalenDayModelManager()
{

}

/*!
 Called before testcase
 */
void TestCalenDayModelManager::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayModelManager::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayModelManager::init()
{
	mManager = new CalenDayModelManager(services, false);
}

/*!
 Called after everyfunction
 */
void TestCalenDayModelManager::cleanup()
{
    if (mManager) {
        delete mManager;
        mManager = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if container is not initialized
 2. Test if container is correcty created
 */
void TestCalenDayModelManager::testConstructors()
{
    //1)
	MCalenServices services;

    CalenDayModelManager *testManager = 0;
    CalenDayModelManager *testManager2 = 0;
    QVERIFY(!testManager);
    QVERIFY(!testManager2);
    
    //2)
    testManager = new CalenDayModelManager(services, false, 0);
    testManager2 = new CalenDayModelManager(services, true, 0);
    QVERIFY(testManager);
    QVERIFY(testManager2);
    
    delete testManager;
    delete testManager2;
}

void TestCalenDayModelManager::testGetModel()
	{
		MCalenServices services;
		CalenDayModel* model = static_cast<CalenDayModel*>(&(mManager->getModel(CalenDayModelManager::CurrentDay)));
		QCOMPARE(model->modelDate(), services.Context().focusDateAndTime());
	}

void TestCalenDayModelManager::testRefreshAllModels()
	{
		
	}

void TestCalenDayModelManager::testRefreshSingleModel()
	{
		mManager->refreshSingleModel(CalenDayModelManager::CurrentDay);
		QVERIFY(mManager);
		mManager->refreshSingleModel(CalenDayModelManager::PreviousDay);
		QVERIFY(mManager);
		mManager->refreshSingleModel(CalenDayModelManager::NextDay);
		QVERIFY(mManager);
	}


QTEST_MAIN(TestCalenDayModelManager);
#include "unittest_calendaymodelmanager.moc"
