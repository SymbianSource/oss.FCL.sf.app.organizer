/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  SettingsUtility unit test class.
*
*/


#include <QtTest/QtTest>
#include "settingsutility.h"

class TestSettingsUtility: public QObject
{
    Q_OBJECT

private slots:
    // Test case setup.
    void init();
    void cleanup();

    // Test cases.
    void testTime();
    
    void testSetTimeFormat();
    void testTimeFormat();

    void testSetClockType();
    void testClockType();

    void testSetTimeSeparator();
    void testTimeSeparator();

    void testDate();
    
    void testSetDateFormat();
    void testDateFormat();

    void testSetDateSeparator();
    void testDateSeparator();

private:
	SettingsUtility *mSettingsUtility;
};

/*!
	This function will be called before each testfunction is executed.
 */
void TestSettingsUtility::init()
{
	mSettingsUtility = new SettingsUtility();
}

/*!
	This function will be called after every testfunction.
 */
void TestSettingsUtility::cleanup()
{
	if (mSettingsUtility) {
	    delete mSettingsUtility;
    }
}

/*!
	Tests the api SettingsUtility::time
 */
void TestSettingsUtility::testTime()
{
	QString expectedTime = 
		QTime::currentTime().toString(mSettingsUtility->timeFormatString());
	QString testTime = mSettingsUtility->time();
	QVERIFY(expectedTime == testTime);
}

/*!
	Tests the api SettingsUtility::setTimeFormat
 */
void TestSettingsUtility::testSetTimeFormat()
{
	QStringList dummyList;
	mSettingsUtility->setTimeFormat(qtTrId("txt_clk_setlabel_val_24_hour"));
	QCOMPARE(mSettingsUtility->timeFormat(dummyList), 0);

	mSettingsUtility->setTimeFormat(qtTrId("txt_clk_setlabel_val_12_hour"));
	QCOMPARE(mSettingsUtility->timeFormat(dummyList), 1);

	mSettingsUtility->setTimeFormat("hour");
	QCOMPARE(mSettingsUtility->timeFormat(dummyList), 1);
}

/*!
	Tests the api SettingsUtility::timeFormat
 */
void TestSettingsUtility::testTimeFormat()
{
	QStringList expectedResult;
	expectedResult << qtTrId("txt_clk_setlabel_val_24_hour") << qtTrId("txt_clk_setlabel_val_12_hour");
	QStringList result;

	mSettingsUtility->timeFormat(result);
	QCOMPARE(result, expectedResult);

	mSettingsUtility->setTimeFormat(qtTrId("txt_clk_setlabel_val_24_hour"));
	QVERIFY(mSettingsUtility->timeFormat(expectedResult) == 0);

	mSettingsUtility->setTimeFormat(qtTrId("txt_clk_setlabel_val_12_hour"));
	QVERIFY(mSettingsUtility->timeFormat(expectedResult) == 1);

	mSettingsUtility->setTimeFormat("hour");
	QVERIFY(mSettingsUtility->timeFormat(expectedResult) == 1);
}

/*!
	Tests the api SettingsUtility::setClockType
 */
void TestSettingsUtility::testSetClockType()
{
	QStringList dummyList;

	mSettingsUtility->setClockType(qtTrId("txt_clock_button_analog"));
	QCOMPARE(mSettingsUtility->clockType(dummyList), 0);

	mSettingsUtility->setClockType(qtTrId("txt_clock_button_digital"));
	QCOMPARE(mSettingsUtility->clockType(dummyList), 1);

	mSettingsUtility->setClockType("clock");
	QCOMPARE(mSettingsUtility->clockType(dummyList), 1);
	
	mSettingsUtility->setClockType("&%#$");
	QCOMPARE(mSettingsUtility->clockType(dummyList), 1);
}

/*!
	Tests the api SettingsUtility::clockType
 */
void TestSettingsUtility::testClockType()
{
	QStringList expectedResult;
	expectedResult << qtTrId("txt_clock_button_analog") << qtTrId("txt_clock_button_digital");
	QStringList result;

	mSettingsUtility->clockType(result);
	QCOMPARE(result, expectedResult);

	mSettingsUtility->setClockType(qtTrId("txt_clock_button_analog"));
	QVERIFY(mSettingsUtility->clockType(expectedResult) ==  0);

	mSettingsUtility->setClockType(qtTrId("txt_clock_button_digital"));
	QVERIFY(mSettingsUtility->clockType(expectedResult) == 1);
	
	// Negative test case
	mSettingsUtility->setClockType("clock");
	QVERIFY(mSettingsUtility->clockType(expectedResult) == 1);

}

/*!
	Tests the api SettingsUtility::setTimeSeparator
 */
void TestSettingsUtility::testSetTimeSeparator()
{
	QStringList expectedResult;
	expectedResult << tr(".") << tr(":");
	QStringList result;
	
	mSettingsUtility->timeSeparator(result);
	QVERIFY(result == expectedResult);

	mSettingsUtility->setTimeSeparator(".");
	QCOMPARE(mSettingsUtility->timeSeparator(result), 0);

	mSettingsUtility->setTimeSeparator(":");
	QCOMPARE(mSettingsUtility->timeSeparator(result), 1);
	
	// Negative case
	mSettingsUtility->setTimeSeparator("/");
	QCOMPARE(mSettingsUtility->timeSeparator(result), 1);

}

/*!
	Tests the api SettingsUtility::timeSeparator
 */
void TestSettingsUtility::testTimeSeparator()
{
	QStringList expectedResult;
	expectedResult << tr(".") << tr(":");
	QStringList result;

	mSettingsUtility->timeSeparator(result);
	QCOMPARE(result, expectedResult);
	
	mSettingsUtility->setTimeSeparator(":");
	int index = mSettingsUtility->timeSeparator(result);
	QVERIFY(index == 1);
	
	// Negative case
	mSettingsUtility->setTimeSeparator("$");
	index = mSettingsUtility->timeSeparator(result);
	QVERIFY(index == 1);
}

/*!
	Tests the api SettingsUtility::date
 */
void TestSettingsUtility::testDate()
{
	QStringList dummyList;
	int index = mSettingsUtility->dateFormat(dummyList);
	QString dateFormat = mSettingsUtility->dateFormatString();
	QString actualDate = QDate::currentDate().toString(dateFormat);
	QString currentDate = mSettingsUtility->date();
	QVERIFY(actualDate == currentDate);
}

/*!
	Tests the api SettingsUtility::setDateFormat
 */
void TestSettingsUtility::testSetDateFormat()
{
	QStringList dummyList;

	mSettingsUtility->setDateFormat(qtTrId("txt_clk_setlabel_val_dd_mm_yyyy"));
	QCOMPARE(mSettingsUtility->dateFormat(dummyList), 0);

	mSettingsUtility->setDateFormat(qtTrId("txt_clk_setlabel_val_mm_dd_yyyy"));
	QCOMPARE(mSettingsUtility->dateFormat(dummyList), 1);

	mSettingsUtility->setDateFormat(qtTrId("txt_clk_setlabel_val_yyyy_mm_dd"));
	QCOMPARE(mSettingsUtility->dateFormat(dummyList), 2);

	// Negative test case
	mSettingsUtility->setDateFormat("dd yyyy mm");
	QCOMPARE(mSettingsUtility->dateFormat(dummyList), 2);
}

/*!
	Tests the api SettingsUtility::dateFormat
 */
void TestSettingsUtility::testDateFormat()
{
	QStringList expectedResult;
	expectedResult << qtTrId("txt_clk_setlabel_val_dd_mm_yyyy") << qtTrId("txt_clk_setlabel_val_mm_dd_yyyy") << qtTrId("txt_clk_setlabel_val_yyyy_mm_dd");
	QStringList result;

	mSettingsUtility->dateFormat(result);
	QCOMPARE(result, expectedResult);
	
	mSettingsUtility->setDateFormat(qtTrId("txt_clk_setlabel_val_dd_mm_yyyy"));
	int index = mSettingsUtility->dateFormat(result);
	QVERIFY(index == 0);
	
	//Negative test case
	mSettingsUtility->setDateFormat("hello");
	index = mSettingsUtility->dateFormat(result);
	QVERIFY(index == 0);
		
}

/*!
	Tests the api SettingsUtility::setDateSeparator
 */
void TestSettingsUtility::testSetDateSeparator()
{
	QStringList dummyList;

	mSettingsUtility->setDateSeparator(".");
	QCOMPARE(mSettingsUtility->dateSeparator(dummyList), 0);

	mSettingsUtility->setDateSeparator(":");
	QCOMPARE(mSettingsUtility->dateSeparator(dummyList), 1);

	mSettingsUtility->setDateSeparator("/");
	QCOMPARE(mSettingsUtility->dateSeparator(dummyList), 2);

	mSettingsUtility->setDateSeparator("-");
	QCOMPARE(mSettingsUtility->dateSeparator(dummyList), 3);

	// Negative test case
	mSettingsUtility->setDateSeparator("?");
	QCOMPARE(mSettingsUtility->dateSeparator(dummyList), 3);
}

/*!
	Tests the api SettingsUtility::dateSeparator
 */
void TestSettingsUtility::testDateSeparator()
{
	QStringList expectedResult;
	expectedResult << tr(".") << tr(":") << tr("/") << tr("-");
	QStringList result;

	mSettingsUtility->dateSeparator(result);
	QCOMPARE(result, expectedResult);
	
	mSettingsUtility->setDateSeparator("-");
	int index = mSettingsUtility->dateSeparator(result);
	QVERIFY(index == 3);
}

QTEST_MAIN(TestSettingsUtility)
#include "unittest_settingsutility.moc"

// End of file.
