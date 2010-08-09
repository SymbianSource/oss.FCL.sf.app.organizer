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
#include <QDebug>


#include <HbMainWindow>
#include "calendaycommonheaders.h"
#include "calendayutils.h"

#define private public

#include "calendayinfo.h"

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

    void testReset();
    void testInsertTimedEvent();
    void testInsertUntimedEvent();
    void testInsertAlldayEvent();
    void testSuggestedUntimedSlotPos();
    void testNeededUntimedSlotCount();
    void testFirstOccupiedSlot();
    void testLastOccupiedSlot();
    void testEarliestEndSlot();
    void testLastStartSlot();
    void testSlotIndexForStartTime();
    void testSlotIndexForEndTime();
    void testAlldayCount();
    void testTodoCount();
    
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
	qDebug() << "test start";
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

void TestCalenDayInfo::testReset()
	{
		SCalenApptInfo info;
		mInfo->InsertAlldayEvent(info);
		mInfo->InsertTimedEvent(info);
		
		mInfo->Reset();
		
		QCOMPARE (mInfo->iAlldayEvents.length(), 0);
		QCOMPARE (mInfo->iRegionList.length(), 0);
	}

void TestCalenDayInfo::testInsertTimedEvent()
	{
		SCalenApptInfo info;
		info.iStartTime = QDateTime(QDate(2010, 7, 10), QTime(23, 45, 45));
			
		info.iEndTime = QDateTime(QDate(2010, 7, 10), QTime(23, 49, 45));
		mInfo->InsertTimedEvent(info);
		
		QCOMPARE (mInfo->iRegionList.count(), 1);
	}

void TestCalenDayInfo::testInsertUntimedEvent()
	{

	}

void TestCalenDayInfo::testInsertAlldayEvent()
	{
		SCalenApptInfo info;
		info.iId = TCalenInstanceId::nullInstanceId();
		info.iStatus = AgendaEntry::StatusUnknown; 
	
		mInfo->InsertAlldayEvent(info);
		
		QCOMPARE(mInfo->iAlldayEvents.length(), 1);
	}

void TestCalenDayInfo::testSuggestedUntimedSlotPos()
	{
		mInfo->iSlotsInHour = CalenDayInfo::ETwo;
		//iregionlist.length  =  0
		mInfo->iRegionList.clear();
		QCOMPARE(mInfo->SuggestedUntimedSlotPos(), 16);
		
		//iregionlist.length  <>  0
		CalenTimeRegion region;
		region.iStartSlot = 10;
		mInfo->iRegionList.append(region);
		
		QCOMPARE(mInfo->SuggestedUntimedSlotPos(), 10);
	}

void TestCalenDayInfo::testNeededUntimedSlotCount()
	{
		mInfo->iUntimedEvents.append(TCalenInstanceId());
		QCOMPARE(mInfo->iUntimedEvents.count(), mInfo->NeededUntimedSlotCount());
		
		mInfo->iTodoEvents.append(TCalenInstanceId());
		QCOMPARE(mInfo->iUntimedEvents.count() + 1, mInfo->NeededUntimedSlotCount());
	}

void TestCalenDayInfo::testFirstOccupiedSlot()
	{
		mInfo->iUntimedSlotCount = 0;
		QCOMPARE(mInfo->FirstOccupiedSlot(), -1);
		
		mInfo->iUntimedSlotCount = 1;
		mInfo->iFirstUntimedSlot = 8;
		QCOMPARE(mInfo->FirstOccupiedSlot(), 8);
		
		mInfo->iUntimedSlotCount = 0;
		CalenTimeRegion region;
		region.iStartSlot = 16;
		
		mInfo->iRegionList.append(region);
		QCOMPARE(mInfo->FirstOccupiedSlot(), 16);
	}

void TestCalenDayInfo::testLastOccupiedSlot()
	{
		mInfo->iUntimedSlotCount = 0;
		QCOMPARE(mInfo->LastOccupiedSlot(), -1);
		
		mInfo->iUntimedSlotCount = 1;
		mInfo->iFirstUntimedSlot = 8;
		QCOMPARE(mInfo->LastOccupiedSlot(), 8);
		
		CalenTimeRegion region;
		region.iEndSlot = 16;	
		mInfo->iRegionList.append(region);
		QCOMPARE(mInfo->LastOccupiedSlot(), 16);
	}

void TestCalenDayInfo::testEarliestEndSlot()
	{
		mInfo->iUntimedSlotCount = 10;
		mInfo->iEmptyUntimedSlots = 10;
		mInfo->iEarliestEndSlot = 2;
		
		QCOMPARE(mInfo->EarliestEndSlot(), -1);
		
		CalenTimeRegion region;
		region.iEndSlot = 16;	
		mInfo->iRegionList.append(region);
		QCOMPARE(mInfo->EarliestEndSlot(), 12);
		
		mInfo->iUntimedSlotCount = 15;
		mInfo->iEmptyUntimedSlots = 10;
		mInfo->iFirstUntimedSlot = 2;
		
		QCOMPARE(mInfo->EarliestEndSlot(), 13);
	}

void TestCalenDayInfo::testLastStartSlot()
	{
		
		mInfo->iUntimedSlotCount = 5; 
		mInfo->iEmptyUntimedSlots = 5;
		QCOMPARE(mInfo->LastStartSlot(), -1);
		
		mInfo->iUntimedSlotCount = 8; 
		mInfo->iEmptyUntimedSlots = 5;
		mInfo->iFirstUntimedSlot = 10;
		QCOMPARE(mInfo->LastStartSlot(), 17);
		
		CalenTimeRegion region;
		region.iEndSlot = 16;	
		mInfo->iRegionList.append(region);
		mInfo->iLastStartSlot = 5;
		mInfo->iUntimedSlotCount = 8; 
		QCOMPARE(mInfo->LastStartSlot(), 13);
	}

void TestCalenDayInfo::testSlotIndexForStartTime()
	{
		QDateTime dt;
		dt.setTime(QTime(10, 5, 0));
		mInfo->iSlotsInHour = CalenDayInfo::EOne;
		mInfo->iUntimedSlotCount = 0;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 10);
		
		qDebug() << dt.time().hour() << ":" << dt.time().minute();
		
		mInfo->iSlotsInHour = CalenDayInfo::ETwo;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 20);
		
		mInfo->iSlotsInHour = CalenDayInfo::EThree;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 30);
		
		mInfo->iSlotsInHour = CalenDayInfo::EFour;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 40);
		//---------
		
		dt.setTime(QTime(10, 20, 0));
		
		mInfo->iSlotsInHour = CalenDayInfo::EOne;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 10);
		
		mInfo->iSlotsInHour = CalenDayInfo::ETwo;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 20);
		
		mInfo->iSlotsInHour = CalenDayInfo::EThree;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 31);
		
		mInfo->iSlotsInHour = CalenDayInfo::EFour;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 41);
		
		//---------
		
		dt.setTime(QTime(10, 35, 0));
		
		mInfo->iSlotsInHour = CalenDayInfo::EOne;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 10);
		
		mInfo->iSlotsInHour = CalenDayInfo::ETwo;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 21);
		
		mInfo->iSlotsInHour = CalenDayInfo::EThree;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 31);
		
		mInfo->iSlotsInHour = CalenDayInfo::EFour;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 42);
		
		//---------
		dt.setTime(QTime(10, 45, 0));
		
		mInfo->iSlotsInHour = CalenDayInfo::EOne;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 10);
		
		mInfo->iSlotsInHour = CalenDayInfo::ETwo;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 21);
		
		mInfo->iSlotsInHour = CalenDayInfo::EThree;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 32);
		
		mInfo->iSlotsInHour = CalenDayInfo::EFour;
		mInfo->SlotIndexForStartTime(dt);
		
		QCOMPARE(mInfo->SlotIndexForStartTime(dt), 43);
		
	}

void TestCalenDayInfo::testSlotIndexForEndTime()
	{

	}

void TestCalenDayInfo::testAlldayCount()
	{
		QCOMPARE(mInfo->AlldayCount(), 0);
		mInfo->iAlldayEvents.append(CalenTimedEventInfo());
		QCOMPARE(mInfo->AlldayCount(), 1);
	}

void TestCalenDayInfo::testTodoCount()
	{
		QCOMPARE(mInfo->TodoCount(), 0);
		mInfo->iTodoEvents.append(TCalenInstanceId());
		QCOMPARE(mInfo->TodoCount(), 1);
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
