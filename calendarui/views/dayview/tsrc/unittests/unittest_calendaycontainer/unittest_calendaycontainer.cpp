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
 * Description: Test class for CalenDayContainer
 *
 */
#include <QtTest/QtTest>

#include <HbEffect>
#include <HbWidget>
#include <HbAbstractViewItem>

#include "calendayinfo.h"
#include "calendaymodel.h"
#include "calendayeventspane.h"

#define private public

#include "calendaycontainertest.h"

QVariantList MOCK_EVENTS_LIST; 
QDateTime    MOCK_EVENTS_DATE;

// Test variables
QRectF gTestWindowRect = QRectF(0, 0, 10, 20);
Qt::Orientation gTestOrientation = Qt::Vertical;
QSizeF gTestContainerSize = QSizeF(100, 500);

class TestCalenDayContainer : public QObject
{
Q_OBJECT

public:
    TestCalenDayContainer();
    virtual ~TestCalenDayContainer();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
    void testSetGetDayInfo();
    void testSetGetDate();
    void testItemAdded();
    void testItemRemoved();
    void testReset();
    void testViewResized();
    void testOrientationChanged();
    void testupdateFloatingItemsList();
    void testCreateDefaultPrototype();
    void testGetTimedEventLayoutValues();
    void testCreateTouchEventAbsorbers();
    void testCrateAbsorberBetweenSlots();
    void testDateTimeAtPos();
    

private:
    CalenDayContainerTest *mContainer;
    CalenDayInfo    *mInfo;
};

/*!
 Constructor
 */
TestCalenDayContainer::TestCalenDayContainer() :
    mContainer(NULL), mInfo(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayContainer::~TestCalenDayContainer()
{

}

/*!
 Called before testcase
 */
void TestCalenDayContainer::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayContainer::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayContainer::init()
{
    mContainer = new CalenDayContainerTest();
    mInfo = new CalenDayInfo(CalenDayInfo::EOne);
    
    
    MOCK_EVENTS_DATE = QDateTime(QDate(12,07,2010),QTime(6,0));
        
    QDateTime dateTimeStart1(QDate(12,07,2010),QTime(7,0));
    QDateTime dateTimeEnd1(QDate(12,07,2010),QTime(10,0));
    QDateTime dateTimeStart2(QDate(12,07,2010),QTime(11,0));
    QDateTime dateTimeEnd2(QDate(12,07,2010),QTime(12,0));
    QDateTime dateTimeStart3(QDate(12,07,2010),QTime(12,45));
    QDateTime dateTimeEnd3(QDate(12,07,2010),QTime(18,45));
    
    AgendaEntry entry1;
    AgendaEntry entry2;
    AgendaEntry entry3;
    
    entry1.setStartAndEndTime(dateTimeStart1,dateTimeEnd1);
    entry2.setStartAndEndTime(dateTimeStart2,dateTimeEnd2);
    entry3.setStartAndEndTime(dateTimeStart3,dateTimeEnd3);
    
    SCalenApptInfo calenInfo;
    calenInfo.iStatus = AgendaEntry::Confirmed;
    calenInfo.iId = TCalenInstanceId::create(entry1);
    mInfo->InsertAlldayEvent(calenInfo);
    mInfo->InsertAlldayEvent(calenInfo);
    
    MOCK_EVENTS_LIST << QVariant::fromValue(entry1);
    MOCK_EVENTS_LIST << QVariant::fromValue(entry2);
    MOCK_EVENTS_LIST << QVariant::fromValue(entry3);
}

/*!
 Called after everyfunction
 */
void TestCalenDayContainer::cleanup()
{
    if (mContainer) {
        delete mContainer;
        mContainer = NULL;
    }
    if (mInfo) {
        delete mInfo;
        mInfo = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if container is not initialized
 2. Test if container is correcty created
 */
void TestCalenDayContainer::testConstructors()
{
    //1)
    CalenDayContainerTest *testContainer = 0;
    QVERIFY(!testContainer);
    
    //2)
    testContainer = new CalenDayContainerTest();
    QVERIFY(testContainer);
    
    delete testContainer;
}

/*!
   Test function to check set and get day info
   Test after set if info day is good
 */
void TestCalenDayContainer::testSetGetDayInfo()
{
    QVERIFY(mContainer->dayInfo() ==0);
    mContainer->setDayInfo(mInfo);
    
    QVERIFY(mContainer->dayInfo()!=0);
}

/*!
   Test function to check set and get date
   1) Test setDate()
   2) Test date()
 */
void TestCalenDayContainer::testSetGetDate() 
{
    QDate today = QDate::currentDate();
    
    //1)
    mContainer->setDate(today);
    QCOMPARE(mContainer->mDate, today);
    
    //2)
    QCOMPARE(mContainer->date(), today);
}

/*!
   Function not implemented
 */
void TestCalenDayContainer::testItemAdded()
{
    // function does nothing - just check function call
    CalenDayItem *item = new CalenDayItem(mContainer);
    mContainer->itemAdded(1, item, false);
}

/*!
   Function not implemented
 */
void TestCalenDayContainer::testItemRemoved()
{
    // function does nothing - just check function call
    CalenDayItem *item = new CalenDayItem(mContainer);
    mContainer->itemRemoved(item, false);
}

/*!
   Test reset of caontainer based on absorders
 */
void TestCalenDayContainer::testReset()
{
   QVERIFY(mContainer->mAbsorbers.count() == 0);
   mContainer->mAbsorbers << mContainer->crateAbsorberBetweenSlots(0,1,false);
   QVERIFY(mContainer->mAbsorbers.count() == 1);
   mContainer->reset();
   QVERIFY(mContainer->mAbsorbers.count() == 0);
}

/*!
  Test is view of container is good resized.
  1)test if size is changed
  2)test if new size is good set
 */
void TestCalenDayContainer::testViewResized()
{
    QSizeF size = mContainer->size();
    //set new size
    QSizeF newSize(size.width()+100,size.height()+100);
    mContainer->viewResized(newSize);
    //1)
    QVERIFY(size != mContainer->size());
    //2)
    QCOMPARE(mContainer->size(),newSize);
}

/*!
   It tests change of orientation based on count of absorbers
 */
void TestCalenDayContainer::testOrientationChanged()
{
   mContainer->setDayInfo(mInfo);
   QVERIFY(mContainer->mAbsorbers.count() == 0);
    
   mContainer->orientationChanged(Qt::Vertical);
   
   QVERIFY(mContainer->mAbsorbers.count() != 0);
}

/*!
   Test function that maintains floating items list
   1) Null pointer check
   2) Floating item notification received
   3) The same floating item notification received
   4) Static item notification received
 */
void TestCalenDayContainer::testupdateFloatingItemsList()
{

    CalenDayItemViewTest *itemView = new CalenDayItemViewTest(mContainer, new HbModelIterator());
    //CalenDayContainer *container = itemView->mContainer;
    const int floatingItemsCount = mContainer->mFloatingItemsList.count();
    const int connectedSlotsCount = itemView->connectedSlots(SIGNAL(scrollPositionChanged(const QPointF&)));
    
    // 1) Test null pointer
    mContainer->updateFloatingItemsList(0);
    //Nothing happend, list remains unchanged
    QCOMPARE(mContainer->mFloatingItemsList.count(), floatingItemsCount);
    QCOMPARE(itemView->connectedSlots(SIGNAL(scrollPositionChanged(const QPointF&))), connectedSlotsCount);
    
    // 2) Floating item notification received
    CalenDayItemTest *item = new CalenDayItemTest(mContainer);
    item->mBackgroundType = CalenDayItem::EFloatingBackground;
    mContainer->updateFloatingItemsList(item);
    QCOMPARE(mContainer->mFloatingItemsList.count(), floatingItemsCount + 1);
    QCOMPARE(itemView->connectedSlots(SIGNAL(scrollPositionChanged(const QPointF&))), connectedSlotsCount + 1);
    
    // 3) The same floating item notification received
    mContainer->updateFloatingItemsList(item);
    QCOMPARE(mContainer->mFloatingItemsList.count(), floatingItemsCount + 1);
    QCOMPARE(itemView->connectedSlots(SIGNAL(scrollPositionChanged(const QPointF&))), connectedSlotsCount + 1);
    
    // 4) Static item notification received
    item->mBackgroundType = CalenDayItem::EStaticBackground;
    mContainer->updateFloatingItemsList(item);
    QCOMPARE(mContainer->mFloatingItemsList.count(), floatingItemsCount);
    QCOMPARE(itemView->connectedSlots(SIGNAL(scrollPositionChanged(const QPointF&))), connectedSlotsCount);
}

/*!
   Test creating item prototype
 */
void TestCalenDayContainer::testCreateDefaultPrototype()
{
    HbAbstractViewItem *testItem = 0;
    QVERIFY(!testItem);
    
    testItem = mContainer->createDefaultPrototype();
    
    QVERIFY(testItem);
    
    delete testItem;
}

/*!
   Test geting timed layouts.
 */
void TestCalenDayContainer::testGetTimedEventLayoutValues()
{
    CalenDayContainer::LayoutValues layoutValues;
    
    QVERIFY(layoutValues.eventAreaWidth == 0);
    QVERIFY(layoutValues.eventAreaX == 0);
    QVERIFY(layoutValues.eventMargin == 0);
    QVERIFY(layoutValues.maxColumns == 0);
    QVERIFY(layoutValues.slotHeight == 0);
    QVERIFY(layoutValues.unitInPixels == 0);
    
    mContainer->getTimedEventLayoutValues(layoutValues);
    
    QVERIFY(layoutValues.eventAreaWidth != 0);
    QVERIFY(layoutValues.eventAreaX == 0);
    QVERIFY(layoutValues.eventMargin != 0);
    QVERIFY(layoutValues.maxColumns != 0);
    QVERIFY(layoutValues.slotHeight != 0);
    QVERIFY(layoutValues.unitInPixels != 0);
}

/*!
   Test creating touch event absorbers.
 */
void TestCalenDayContainer::testCreateTouchEventAbsorbers()
{
    mContainer->setDayInfo(mInfo);
    QVERIFY(mContainer->mAbsorbers.count() == 0);
    mContainer->createTouchEventAbsorbers();
    QVERIFY(mContainer->mAbsorbers.count() != 0);
}

/*!
   test creating touch absorber beatween slots.
 */
void TestCalenDayContainer::testCrateAbsorberBetweenSlots()
{
    TouchEventAbsorber* testValue = 0;
    QVERIFY(!testValue);
    testValue = mContainer->crateAbsorberBetweenSlots(0,1,false);
    QVERIFY(testValue);
    delete testValue;
}

/*!
 Test dateTimeAtPos
 1. Position outside control
 2. Position in control, first half of hour (0-30 min)
 3. Position in control, second half of hour (30-60 min)
 */
void TestCalenDayContainer::testDateTimeAtPos()
{
    mContainer->setDate(QDate(2010, 10, 1));
    mContainer->viewResized(gTestContainerSize);

    QRectF geometry = mContainer->mEventsPaneElements.at(0)->geometry();
    qreal halfHour = geometry.height() / 2;
    
    //1)
    QPointF point(50, geometry.top() - 10);
    QDateTime dateTime = mContainer->dateTimeAtPos(point);
    QVERIFY(!dateTime.isValid());
    
    //2)
    point = QPointF(50, geometry.top() + halfHour - 1);
    dateTime = mContainer->dateTimeAtPos(point);
    QVERIFY(dateTime.isValid());
    QCOMPARE(dateTime.time(), QTime(0, 0));
    
    //3)
    point = QPointF(50, geometry.top() + halfHour + 1);
    dateTime = mContainer->dateTimeAtPos(point);
    QVERIFY(dateTime.isValid());
    QCOMPARE(dateTime.time(), QTime(0, 30));
}


QTEST_MAIN(TestCalenDayContainer);
#include "unittest_calendaycontainer.moc"
