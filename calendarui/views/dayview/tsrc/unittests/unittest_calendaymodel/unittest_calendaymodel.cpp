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

	void testRowCount();
    void testData();
	void testRefreshModel();
	void testModelDate();
    
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

void TestCalenDayModel::testRowCount()
	{
		mModel->refreshModel(QDateTime());
		QCOMPARE(mModel->rowCount(QModelIndex()), 1);
	}

void TestCalenDayModel::testData()
	{
		mModel->refreshModel(QDateTime());
		
		QVariant var = mModel->data(QModelIndex(), Qt::UserRole + 1);
		QString typeName(var.typeName());
		QCOMPARE(typeName, QString());
		mModel->refreshModel(QDateTime());
		
		var = mModel->data(QModelIndex(), Qt::UserRole + 1);
		typeName = QString(var.typeName());
		QCOMPARE(typeName, QString(""));
		
	}

void TestCalenDayModel::testRefreshModel()
	{
		QDateTime date(QDate(2001, 1, 2), QTime(1, 0, 0));
		mModel->refreshModel(date);
	
		QCOMPARE(date, mModel->modelDate());
	}

void TestCalenDayModel::testModelDate()
	{
		QDateTime date(QDate(2001, 1, 1), QTime(0, 0, 0));
		mModel->refreshModel(date);
		
		QCOMPARE(date, mModel->modelDate());
	}


QTEST_MAIN(TestCalenDayModel);
#include "unittest_calendaymodel.moc"
