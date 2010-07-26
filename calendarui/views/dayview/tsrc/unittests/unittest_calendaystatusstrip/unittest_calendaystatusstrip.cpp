/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>
#include <QGraphicsItem>

#include "calendaystatusstriptest.h"

#include <QPainter>
#include <QImage>
#include <QPair>

const qreal WIDGET_WIDTH = 50;
const qreal WIDGET_HEIGHT = 150;

class TestCalenStatusStrip : public QObject
{
Q_OBJECT

public:
    TestCalenStatusStrip();
    virtual ~TestCalenStatusStrip();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
    void testSetGetColor();
    void testSetGetRange();
    void testSetGetDrawingStyle_data();
    void testSetGetDrawingStyle();
    void testStartEndTime();
    void testCreatingDiagonalLine();
    void testCalculateStartEndTimePosition_data();
    void testCalculateStartEndTimePosition();
    void testCalculateMinuteHegiht();
    void testPaint();

private:
    CalenDayStatusStripTest *mStatusStrip;
};




/*!
 Constructor
 */
TestCalenStatusStrip::TestCalenStatusStrip() :
   mStatusStrip(NULL)
{

}

/*!
 Destructor
 */
TestCalenStatusStrip::~TestCalenStatusStrip()
{

}

/*!
 Called before testcase
 */
void TestCalenStatusStrip::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenStatusStrip::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenStatusStrip::init()
{
    mStatusStrip = new CalenDayStatusStripTest();
}

/*!
 Called after everyfunction
 */
void TestCalenStatusStrip::cleanup()
{
    if (mStatusStrip) {
        delete mStatusStrip;
        mStatusStrip = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if content widget is not initialized
 2. Test if content widget is correcty created
 */
void TestCalenStatusStrip::testConstructors()
{
    //1)
    CalenDayStatusStrip *testStatusStrip = 0;
    QVERIFY(!testStatusStrip);
    
    //2)
    testStatusStrip = new CalenDayStatusStrip();
    QVERIFY(testStatusStrip);
    delete testStatusStrip;
}

/*!
   Test function for geting and setting color
   1. Test setting red color
   2. Test change color to blue
 */
void TestCalenStatusStrip::testSetGetColor()
{
    //1)
    mStatusStrip->setColor(QColor(Qt::red));
    QColor testColor = mStatusStrip->color();
    QCOMPARE(QColor(Qt::red),testColor);
    
    //2)
    mStatusStrip->setColor(QColor(Qt::blue));
    testColor = mStatusStrip->color();
    QCOMPARE(QColor(Qt::blue),testColor);
}

/*!
   Test function for geting and setting range
   1. Test setting 5px range
   2. Test change range to 1px
 */
void TestCalenStatusStrip::testSetGetRange()
{
    //1)
    mStatusStrip->setRange(5);
    QCOMPARE((qreal)5,mStatusStrip->range());
    
    //2)
    mStatusStrip->setRange(1);
    QCOMPARE((qreal)1,mStatusStrip->range());
}

/*!
   Preapre data for \sa testSetGetDrawingStyle
   Add new enum \sa CalenDayStatusStrip::DrawingStyle
   1)OnlyFrame
   2)StripWithLines
   3)Filled
 */
void TestCalenStatusStrip::testSetGetDrawingStyle_data()
{
    QTest::addColumn<CalenDayStatusStrip::DrawingStyle>("testedValue");    
    QTest::addColumn<CalenDayStatusStrip::DrawingStyle>("testValue");
    
    QTest::newRow("OnlyFrame") << CalenDayStatusStrip::OnlyFrame 
                               << CalenDayStatusStrip::OnlyFrame;
    QTest::newRow("StripWithLines") << CalenDayStatusStrip::StripWithLines 
                                    << CalenDayStatusStrip::StripWithLines; 
    QTest::newRow("Filled") << CalenDayStatusStrip::Filled 
                            << CalenDayStatusStrip::Filled;   
}

/*!
   Test settting and getting drawing style on preapred data
   
   \sa testSetGetDrawingStyle_data
 */
void TestCalenStatusStrip::testSetGetDrawingStyle()
{ 
    //get data to test
    QFETCH(CalenDayStatusStrip::DrawingStyle, testedValue); 
    QFETCH(CalenDayStatusStrip::DrawingStyle, testValue); 
    
    //set data
    mStatusStrip->setDrawingStyle(testedValue);
    
    //check data
    QCOMPARE(mStatusStrip->drawingStyle(),testValue);
}

/*!
   Test setting and getting start and end time of event
   1)Set end and start of event
   2)Change values
 */
void TestCalenStatusStrip::testStartEndTime()
{
    QPair<QTime, QTime> startEndTime;
    startEndTime.first = QTime(12,12,12);
    startEndTime.second = QTime(15,15,15);
    
    mStatusStrip->setStartEndTime(startEndTime.first,startEndTime.second);
    
    QCOMPARE(mStatusStrip->startEndTime().first,startEndTime.first);
    QCOMPARE(mStatusStrip->startEndTime().second,startEndTime.second);
    
    startEndTime.first = QTime(1,1,1);
    startEndTime.second = QTime(5,5,5);
    
    mStatusStrip->setStartEndTime(startEndTime.first,startEndTime.second);
    
    QCOMPARE(mStatusStrip->startEndTime().first,startEndTime.first);
    QCOMPARE(mStatusStrip->startEndTime().second,startEndTime.second);
}

/*!
   Test creating polygon to draw diagonal line on widget
 */
void TestCalenStatusStrip::testCreatingDiagonalLine()
{
    qreal dx = 10;
    qreal dy = 1;
    QPointF point(0,60);
    QVector<QPointF> points;
    
    points << QPointF(0,60);
    points << QPointF(0,59);
    points << QPointF(10,58);
    points << QPointF(10,59);
    
    QPolygonF testValue(points);
    QPolygonF testedValue = mStatusStrip->diagonalLine(point,dx,dy);
    
    QCOMPARE(testedValue,testValue);
}

/*!
   Prepare data for \sa testCalculateStartEndTimePosition
   Add new start and end time of event.
   1)Full time start and end
   2)Exactly 30min time start and end
   3)More than 30min time start and end
   4)time start > 30m and end > 23h30m
 */
void TestCalenStatusStrip::testCalculateStartEndTimePosition_data()
{
    QTest::addColumn<QTime>("testedStartValue");
    QTest::addColumn<QTime>("testedEndValue");
    QTest::addColumn<QTime>("testStartValue");
    QTest::addColumn<QTime>("testEndValue");
    
    QTest::newRow("Full time start and end") <<  QTime(10,00,00)
                                             <<  QTime(11,00,00)
                                             <<  QTime(10,00,00)
                                             <<  QTime(11,00,00);
    QTest::newRow("Exactly 30min time start and end") <<  QTime(10,30,00)
                                             <<  QTime(11,30,00)
                                             <<  QTime(10,30,00)
                                             <<  QTime(11,30,00);
    QTest::newRow("More than 30min time start and end") <<  QTime(10,40,00)
                                             <<  QTime(12,35,00)
                                             <<  QTime(10,30,00)
                                             <<  QTime(13,00,00);
    QTest::newRow("time start > 30m and end > 23h30m") <<  QTime(20,35,00)
                                             <<  QTime(23,35,00)
                                             <<  QTime(20,30,00)
                                             <<  QTime(23,59,00);
    QTest::newRow("time start < 30m and end > 23h00m") <<  QTime(11,19,00)
                                             <<  QTime(23,10,00)
                                             <<  QTime(11,00,00)
                                             <<  QTime(23,30,00);
}

/*!
   Test calculating start and end time drawing of event bubble on prepared data
   \sa testCalculateStartEndTimePosition_data
 */
void TestCalenStatusStrip::testCalculateStartEndTimePosition()
{
    //get prepared data
    QFETCH(QTime, testedStartValue); 
    QFETCH(QTime, testedEndValue); 
    QFETCH(QTime, testStartValue); 
    QFETCH(QTime, testEndValue); 
    
    //run function
    QPair<QTime,QTime> testStartEndValue 
                     = mStatusStrip->calculateStartEndPostion(testedStartValue,
                                                               testedEndValue);
    //check data
    QCOMPARE(testStartEndValue.first,testStartValue);
    QCOMPARE(testStartEndValue.second,testEndValue);
}

/*!
   Test calculating minute height based on widget height and given time.
   1)Calculate on given time
   2)Change time of event
 */
void TestCalenStatusStrip::testCalculateMinuteHegiht()
{   
    qreal testedValue;
    qreal testValue;
    mStatusStrip->resize(WIDGET_WIDTH,WIDGET_HEIGHT);
    
    //1)
    testedValue = mStatusStrip->calculateMinuteHeight(QTime(10,00,0),
                                                      QTime(11,00,0));
    testValue = WIDGET_HEIGHT / (qreal)60;
    QCOMPARE(testedValue,testValue);
    
    //2)
    testedValue = mStatusStrip->calculateMinuteHeight(QTime(10,00,0),
                                                      QTime(11,30,0));
    testValue = WIDGET_HEIGHT / (qreal)90;
    QCOMPARE(testedValue,testValue);
}


/*!
   Test drawing widget.
   
   If there is no SAVE_IMAGES macro defined it will only run this function.
   If there is macro defined and c:\unittest folder created then image will be
   created so user can see if it draw in good way.
 */
void TestCalenStatusStrip::testPaint()
{      
    //preapre data needed to draw on widget
    mStatusStrip->setColor(QColor(Qt::red));
    QPair<QTime, QTime> startEndTime;
    startEndTime.first = QTime(10,20,0);
    startEndTime.second = QTime(15,10,00);
    
    //set event time
    mStatusStrip->setStartEndTime(startEndTime.first,startEndTime.second);
    
    //resize widget to be bigger than (0,0)
    mStatusStrip->resize(WIDGET_WIDTH,WIDGET_HEIGHT);
    QSize size = mStatusStrip->size().toSize();
    
    //create image that will simulate widget where painting should be done
    QImage img(size,QImage::Format_RGB32);
    //create painter which will be used to paint
    QPainter painter(&img);
    //fill image with white color to have better filings with look of "paper"
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::white));
    //run paint
    mStatusStrip->paint(&painter,0);
    
#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenStatusStrip_testPaint.jpg");
    
#endif

}

QTEST_MAIN(TestCalenStatusStrip);
#include "unittest_calendaystatusstrip.moc"
