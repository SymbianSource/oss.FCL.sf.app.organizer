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
 * Description: Test class for CalenDayItem
 *
 */
#include <QGraphicsItem>
#include <QtTest/QtTest>
#include <QGraphicsSceneEvent>
#include <QDebug>


#include "agendaentry.h"
#include "calendaycontainer.h"

// System includes
#include <HbFrameItem>
#include <HbTextItem>
#include <HbStyle>
#include <HbColorScheme>
#include <agendaentry.h>

// User includes
#include "calendaycommonheaders.h"
#include "calendaystatusstrip.h"
#include "calendaymodel.h"
#include "calendayutils.h"
#include "calenagendautils.h"
#include "calendaycontainer.h"


#define private public
#ifndef __WINSCW__
#define protected public
#endif

#include "calendayitem.h"
#include "calendayitemtest.h"


QRectF gTestWindowRect;
Qt::Orientation gTestOrientation;

class TestCalenDayItem : public QObject
{
Q_OBJECT

public:
    TestCalenDayItem();
    virtual ~TestCalenDayItem();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testCreateItem();
    void testUpdateChildItems();
    void testHasEventDescription();
    void testHasBackgroundFrame();
    void testSetDescription();
    void testSetStatusStrip();
    void testFloatingBackground();
    void testScrollBackground();
    
    void testConstructors();
private:
    CalenDayItemTest *mItem;
    CalenDayContainer *mContainer;
};

/*!
 Constructor
 */
TestCalenDayItem::TestCalenDayItem() : mItem(NULL), mContainer(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayItem::~TestCalenDayItem()
{

}

/*!
 Called before testcase
 */
void TestCalenDayItem::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayItem::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayItem::init()
{
    mContainer = new CalenDayContainer();
    mItem = new CalenDayItemTest(mContainer);
#ifndef __WINSCW__
    mItem->mBg = new HbFrameItem();
    mItem->mEventDesc = new HbTextItem(0);
#endif
}

/*!
 Called after everyfunction
 */
void TestCalenDayItem::cleanup()
{
    if (mItem) {
        delete mItem;
        mItem = NULL;
    }
    
    if (mContainer) {
        delete mContainer;
        mContainer = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if content widget is not initialized
 2. Test if content widget is correcty created
 3. Test whether connection to container has been established properly
 */
void TestCalenDayItem::testConstructors()
{
    //1)
    CalenDayItemTest *testItem = 0;
    QVERIFY(!testItem);
    
    //2)
    testItem = new CalenDayItemTest(mContainer);
    QVERIFY(testItem);
    
    //3)
    QCOMPARE(testItem->connectedSlots(SIGNAL(backgroundTypeChanged(const CalenDayItem*))), 1);
    
    delete testItem;
}

/*!
 Test function for creating new abstract items.
 1. Test if item is created
 2. Test if is the same as orginal.
 */
void TestCalenDayItem::testCreateItem()
{
    HbAbstractViewItem *testItem = mItem->createItem();
    QVERIFY(testItem);
    delete testItem;
}

/*!
 Test function for updating child items
 1. Test if child items are updated 
 2. Just for coverage testing
 */
void TestCalenDayItem::testUpdateChildItems()
{
#ifndef __WINSCW__
    //1)
    mItem->mUpdated = false;
    mItem->updateChildItems();
    QCOMPARE(mItem->mUpdated, true);
    
    //2)
    mItem->mUpdated = true;
    mItem->updateChildItems();
    QCOMPARE(mItem->mUpdated, true);
#endif
}

/*!
 Test setting description of entry
 1. Setting description if only location is set
 2. Setting description if location and summary are set
 3. Setting description if location and summary are not set
 4. Setting description if summary is set and event is for all day
 5. Setting description if location and summary are set and event is 
 for all day
 */
void TestCalenDayItem::testSetDescription()
{
#ifndef __WINSCW__
    AgendaEntry entry;
    QString location("testLocation");
    QString summary("testSummary");

    //1)
    entry.setLocation(location);
    entry.setSummary("");
    mItem->setDescription(entry, false);
    QCOMPARE(mItem->mEventDesc->text(), location);

    //2)
    entry.setLocation(location);
    entry.setSummary(summary);
    mItem->setDescription(entry, false);
    QString expected(summary);
    expected.append(", ");
    expected.append(location);
    QCOMPARE(mItem->mEventDesc->text(), expected);

    //3)
    entry.setLocation("");
    entry.setSummary("");
    mItem->setDescription(entry, false);
    QCOMPARE(mItem->mEventDesc->text(), hbTrId("txt_calendar_dblist_unnamed"));

    //4)
    entry.setLocation("");
    entry.setSummary(summary);
    mItem->setDescription(entry, true);
    expected = "";
    for (int i = 0; i < summary.count(); i++) {
        expected.append(QString(summary.at(i)) + "\n");
    }
    QCOMPARE(mItem->mEventDesc->text(), expected);

    //5)
    entry.setLocation(location);
    entry.setSummary(summary);
    mItem->setDescription(entry, true);
    QString description(summary + ", " + location);
    expected = "";
    for (int i = 0; i < description.count(); i++) {
        expected.append(QString(description.at(i)) + "\n");
    }
    if (summary.count()) {
        expected.remove(2 * summary.count() - 1, 1);
    }
    QCOMPARE(mItem->mEventDesc->text(), expected);
#endif
}

/*!
 Test setting status strip
 1. Setting status strip for event 
 2. Setting status strip for all day event
 3. Setting status strip for confirmed event
 4. Setting status strip for tentative event
 5. Setting status strip for cancelled event
 */
void TestCalenDayItem::testSetStatusStrip()
{
#ifndef __WINSCW__
    AgendaEntry entry;
    QDateTime startTimeIn(QDate(12, 07, 2010), QTime(7, 0));
    QDateTime endTimeIn(QDate(12, 07, 2010), QTime(10, 0));

    //1)
    entry.setStartAndEndTime(startTimeIn, endTimeIn);
    mItem->setStatusStrip(entry, false);

    QDateTime startTime;
    QDateTime endTime;
    QDateTime currentDateTime;
    currentDateTime.setDate(mItem->container()->date());
    CalenDayUtils::instance()->getEventValidStartEndTime(startTime, endTime,
        entry, currentDateTime);
    QPair<QTime, QTime> startEndTime = mItem->mColorStripe->startEndTime();
    QCOMPARE(startEndTime.first, startTime.time());
    QCOMPARE(startEndTime.second, endTime.time());

    //2)
    mItem->setStatusStrip(entry, true);
    startEndTime = mItem->mColorStripe->startEndTime();
    QCOMPARE(startEndTime.first, startTime.time());
    QCOMPARE(startEndTime.second, endTime.time().addSecs(-1));

    //3)
    entry.setStatus(AgendaEntry::Confirmed);
    mItem->setStatusStrip(entry, false);
    QCOMPARE(CalenDayStatusStrip::Filled, 
        mItem->mColorStripe->drawingStyle());

    //4)
    entry.setStatus(AgendaEntry::Tentative);
    mItem->setStatusStrip(entry, false);
    QCOMPARE(CalenDayStatusStrip::StripWithLines, 
        mItem->mColorStripe->drawingStyle());

    //5)
    entry.setStatus(AgendaEntry::Cancelled);
    mItem->setStatusStrip(entry, false);
    QCOMPARE(CalenDayStatusStrip::OnlyFrame, 
        mItem->mColorStripe->drawingStyle());
#endif
}

void TestCalenDayItem::testHasEventDescription()
{
#ifndef __WINSCW__
    QGraphicsSceneResizeEvent *event = new QGraphicsSceneResizeEvent();
    qreal width = mItem->rect().width();

    qDebug() << "inited";

    mItem->mEventDescMinWidth = width - 4;

    qDebug() << "before resize";

    mItem->resizeEvent(event);

    qDebug() << "resize called";

    QVERIFY(mItem->hasEventDescription() == true);

    mItem->mEventDescMinWidth = width + 8;
    mItem->resizeEvent(event);

    qDebug() << "resize 2 called";

    QVERIFY(mItem->hasEventDescription() == false);
#endif
}

void TestCalenDayItem::testHasBackgroundFrame()
{
#ifndef __WINSCW__
    QGraphicsSceneResizeEvent *event = new QGraphicsSceneResizeEvent();
    qreal width = mItem->rect().width();

    mItem->mFrameMinWidth = width - 4;
    mItem->resizeEvent(event);

    QVERIFY(mItem->hasBackgroundFrame() == true);

    mItem->mFrameMinWidth = width + 8;
    mItem->resizeEvent(event);

    QVERIFY(mItem->hasBackgroundFrame() == false);
#endif
}


/*!
 Test floating background functionality 
 1. Event is higher then screen (background starts to float) 
 2. Orientation changed
 3. Event is higher then screen but item is already set as floating
 4. Event's layout switches from floating to static
 5. Static event remains smaller then screen height
 */
void TestCalenDayItem::testFloatingBackground()
{
#ifndef __WINSCW__
    qRegisterMetaType<CalenDayItemTest*>("const CalenDayItem*");
    
    QSignalSpy spy(mItem, SIGNAL(backgroundTypeChanged(const CalenDayItem*)));
    int emittedSignalsCount = 0;

    //1)Event is higher then screen (background starts to float)
    gTestWindowRect = QRectF(0, 0, 320, 640);
    mItem->mBackgroundType = CalenDayItem::EStaticBackground;
    mItem->resize(30, 1000);
    int bgHeight = mItem->mBg->rect().height();
    int screenHeight = CalenDayUtils::instance()->screenHeight();

    QCOMPARE(bgHeight, screenHeight);
    QCOMPARE(mItem->mBackgroundType, CalenDayItem::EFloatingBackground);
    QCOMPARE(spy.count(), ++emittedSignalsCount);
    
    //2)Orientation changed
    gTestWindowRect = QRectF(0, 0, 640, 320);
    mItem->mBackgroundType = CalenDayItem::EStaticBackground;
    mItem->resize(30, 1001);
    bgHeight = mItem->mBg->rect().height();
    screenHeight = CalenDayUtils::instance()->screenHeight();
    
    QCOMPARE(bgHeight, screenHeight);
    QCOMPARE(mItem->mBackgroundType, CalenDayItem::EFloatingBackground);
    QCOMPARE(spy.count(), ++emittedSignalsCount);
    
    //3)Event is higher then screen but item is already set as floating
    mItem->resize(30, 1000);
    bgHeight = mItem->mBg->rect().height();
    screenHeight = CalenDayUtils::instance()->screenHeight();
    
    QCOMPARE(bgHeight, screenHeight);
    QCOMPARE(mItem->mBackgroundType, CalenDayItem::EFloatingBackground);
    QCOMPARE(spy.count(), emittedSignalsCount);
    
   //4)Event's layout switches from floating to static
    mItem->setGeometry(0, 0, 30, 100);
    QCOMPARE(mItem->mBackgroundType, CalenDayItem::EStaticBackground);
    QCOMPARE(spy.count(), ++emittedSignalsCount);
    
    //5)Static event remains smaller then screen height
    mItem->setGeometry(0, 0, 30, 200);
    QCOMPARE(mItem->mBackgroundType, CalenDayItem::EStaticBackground);
    QCOMPARE(spy.count(), emittedSignalsCount);
#endif
}

/*!
 Test scrolling background slot 
 1. Position of background doesn't go out of events rectangle
 2. Position of background is out of the top edge
 3. Position of background is out of the bottom edge
 */
void TestCalenDayItem::testScrollBackground()
{
#ifndef __WINSCW__
    gTestWindowRect = QRectF(0, 0, 240, 480);
    mItem->setGeometry(0, 0, 30, 1000);
    
    //1)Position of background doesn't go out of events rectangle
    QPointF pos(0, 50);
    mItem->scrollBackground(pos);
    int screenHeight = CalenDayUtils::instance()->screenHeight();
    int bgHeight = mItem->mBg->rect().height();
    
    QCOMPARE(bgHeight, screenHeight);
    QVERIFY(gTestWindowRect.contains(mItem->mBg->rect()));
    
    //2)Position of background is out of the top edge
    pos = QPointF(0, -50);
    mItem->scrollBackground(pos);
    bgHeight = mItem->mBg->rect().height();
    
    QCOMPARE(bgHeight, screenHeight);
    QVERIFY(gTestWindowRect.contains(mItem->mBg->rect()));

    //3)Position of background is out of the bottom edge
    pos = QPointF(0, 300);
    mItem->scrollBackground(pos);
    bgHeight = mItem->mBg->rect().height();
    
    QCOMPARE(bgHeight, screenHeight);
    QVERIFY(gTestWindowRect.contains(mItem->mBg->rect()));   
#endif
}

QTEST_MAIN(TestCalenDayItem);

#include "unittest_calendayitem.moc"
