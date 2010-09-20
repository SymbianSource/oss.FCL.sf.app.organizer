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
#include "calendateutils.h"
#include "calendaymodelmanager.h"

#define private public

#include "calendayview.h"

class TestCalenDayView : public QObject
{
Q_OBJECT

public:
    TestCalenDayView();
    virtual ~TestCalenDayView();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
    void testPopulationComplete();
    void testDoPopulation();
    void testOnBack();
    void testDayChangeStarted();
    void testRunChangeToAgendaView();
    void testRunLunarData();
    void testRunNewMeeting();
    void testRunGoToToday();
    void testChangeView();
    
    
private:
    CalenDayView *mView;
    MCalenServices mServices;
};

/*!
 Constructor
 */
TestCalenDayView::TestCalenDayView() :
	mView(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayView::~TestCalenDayView()
{

}

/*!
 Called before testcase
 */
void TestCalenDayView::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayView::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayView::init()
{
    mServices.IssueCommandL(0);
    mView = new CalenDayView(mServices);
}

/*!
 Called after everyfunction
 */
void TestCalenDayView::cleanup()
{

}

/*!
 Test function for constructors
 1. Test if content widget is not initialized
 2. Test if content widget is correcty created
 */
void TestCalenDayView::testConstructors()
{
	MCalenServices services;
    //1)
    CalenDayView *testView = 0;
    QVERIFY(!testView);
    
    //2)
    testView = new CalenDayView(services);
    QVERIFY(testView);
    
    delete testView;
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command population complete is send
 */
void TestCalenDayView::testPopulationComplete()
{
    //1)
    QVERIFY(mServices.lastCommand() == 0);
    
    //2)
    mView->doPopulation();
    
    QVERIFY(mServices.lastCommand() == ECalenNotifyViewPopulationComplete);
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command population complete is send
 */
void TestCalenDayView::testDoPopulation()
{
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->populationComplete();
    
    QVERIFY(mServices.lastCommand() == ECalenNotifyViewPopulationComplete);
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command  month view is send
 */
void TestCalenDayView::testOnBack()
{
#ifndef __WINSCW__
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->onBack();
    
    QVERIFY(mServices.lastCommand() == ECalenMonthView);
#endif /*__WINSCW__*/
}

/*!
   Test if population was done.
   1)Check if next date is set
   2)Check if priovor date is set
 */
void TestCalenDayView::testDayChangeStarted()
{
#ifndef __WINSCW__
    mView->mDate = QDateTime(QDate(2010,9,8),QTime(10,10,10));
    mView->dayChangeStarted(ECalenScrollToNext);
    
    QVERIFY(mView->mDate == QDateTime(QDate(2010,9,9),QTime(10,10,10)));
    
    mView->dayChangeStarted(ECalenScrollToPrev);
        
    QVERIFY(mView->mDate == QDateTime(QDate(2010,9,8),QTime(10,10,10)));
#endif /*__WINSCW__*/
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command go to agenda view is send
 */
void TestCalenDayView::testRunChangeToAgendaView()
{
#ifndef __WINSCW__
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->runChangeToAgendaView();
    
    QVERIFY(mServices.lastCommand() == ECalenAgendaView);
#endif /*__WINSCW__*/
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command regional info taped is send
 */
void TestCalenDayView::testRunLunarData()
{
#ifndef __WINSCW__
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->runLunarData();
    
    QVERIFY(mServices.lastCommand() == ECalenRegionalPluginTapEvent); 
#endif /*__WINSCW__*/  
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command new meeting is send
 */
void TestCalenDayView::testRunNewMeeting()
{
#ifndef __WINSCW__
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->runNewMeeting();
    
    QVERIFY(mServices.lastCommand() == ECalenNewMeeting);   
#endif /*__WINSCW__*/
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command go to today is send
 */
void TestCalenDayView::testRunGoToToday()
{
#ifndef __WINSCW__
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->runGoToToday();
    
    QVERIFY(mServices.lastCommand() == ECalenGotoToday); 
#endif /*__WINSCW__*/
}

/*!
   Test if population was done.
   1)Check if there is no command before
   2)Check if command givenn command is send
 */
void TestCalenDayView::testChangeView()
{
    QVERIFY(mServices.lastCommand() == 0);
    
    mView->changeView(ECalenAgendaView);
    
    QVERIFY(mServices.lastCommand() == ECalenAgendaView); 
}

QTEST_MAIN(TestCalenDayView);

#include "unittest_calendayview.moc"
