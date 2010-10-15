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
* Description:  AlarmClient unit test class.
*
*/

// System includes
#include <QtTest/QtTest>

// User includes
#include "alarmclient.h"
#include "qdatetime.h"
#include "qstring.h"
#include "clockdatatypes.h"

/*!
	\class TestAlarmClient
	
	This object has functions to test the public apis in the class
	AlarmClient.
 */
class TestAlarmClient: public QObject
{
	Q_OBJECT

private slots:
	// Test case setup.
	void init();
	void cleanup();

	// Test cases.
	void testToggleAlarmStatus();
	void testDeleteAlarm();
	void testGetAlarmList();
	void testSetAlarmDaily();
	void testSetAlarmWeekly();
	void testSetAlarmWorkdays();
	void testSetAlarmOnceOnly(); 


private:
	AlarmClient *mAlarmClient;
};

/*!
	This function will be called before each testfunction is executed.
 */
void TestAlarmClient::init()
{
	mAlarmClient = new AlarmClient(this);
}

/*!
	This function will be called after every testfunction.
 */
void TestAlarmClient::cleanup()
{
	QList<AlarmInfo> alarmlist;
	mAlarmClient->getAlarmList(alarmlist);

	// Delete all the alarms.
	int numOfAlarms = alarmlist.count();
	for (int i = 0; i < numOfAlarms; i++) {
		mAlarmClient->deleteAlarm(alarmlist.at(i).alarmId);
	}
	
	if (mAlarmClient) {
		delete mAlarmClient;
	}
}

/*!
 * 	Test the API AlarmClient::toggleAlarmStatus. 
 */
void TestAlarmClient::testToggleAlarmStatus()
{	
	// Set an alarm.
	QTime alarmTime;
	QString descInfo("This is a daily alarm");

	AlarmInfo alarmInfoDaily;
	// First the description.
	alarmInfoDaily.alarmDesc = descInfo;
	// The time.
	alarmTime = QTime::fromString("12:30 pm", "hh:mm ap");
	// Construct the alarm info.
	alarmInfoDaily.origAlarmTime = alarmTime;
	// Fill the repeat type.
	alarmInfoDaily.repeatType = Daily;

	// Request the listener to set the alarm.
	mAlarmClient->setAlarm(alarmInfoDaily);
	// Get the Id of the alarm set.
	int tempId = alarmInfoDaily.alarmId;
	mAlarmClient->toggleAlarmStatus(tempId, Disabled);
	
	// Get the above alarminfo and see if its disabled.
	AlarmInfo retrievedAlarmInfoDaily;
	int error = mAlarmClient->getAlarmInfo(tempId, retrievedAlarmInfoDaily);

	if (0 == error) {
		QVERIFY(Disabled == 
			    retrievedAlarmInfoDaily.alarmStatus);
	}
	
	// Now toggle from disabled to enabled and check.
	mAlarmClient->toggleAlarmStatus(tempId, Enabled);
	error = mAlarmClient->getAlarmInfo(tempId, retrievedAlarmInfoDaily);

	if (0 == error) {
		QVERIFY(Enabled == 
				retrievedAlarmInfoDaily.alarmStatus);
	}

}

/*!
	Test the API AlarmClient::deleteAlarm.
 */
void TestAlarmClient::testDeleteAlarm()
{
	// Set an alarm.
	QTime alarmTime;
	QString descInfo("This is a daily alarm");

	AlarmInfo alarmInfoDaily;
	// First the description.
	alarmInfoDaily.alarmDesc = descInfo;
	// The time.
	alarmTime = QTime::fromString("12:30 pm", "hh:mm ap");
	// Construct the alarm info.
	alarmInfoDaily.origAlarmTime = alarmTime;
	// Fill the repeat type.
	alarmInfoDaily.repeatType = Daily;
	// Request the listener to set the alarm.
	mAlarmClient->setAlarm(alarmInfoDaily);
	
	QList<AlarmInfo> alarmList;
	mAlarmClient->getAlarmList(alarmList);

	// Delete all the alarms.
	int numOfAlarms = alarmList.count();
	for (int i = 0; i < numOfAlarms; i++) {
		mAlarmClient->deleteAlarm(alarmList.at(i).alarmId);
	}
	// TODO:Have to check the number of alarms returned.Ll do later
	// Check if there are no alarms.
	// mAlarmClient->getAlarmList(alarmList);
	// numOfAlarms = alarmList.size();
	// QVERIFY(0 == numOfAlarms); 
}

/*
	Test the API AlarmClient::getAlarmList.
 */
void TestAlarmClient::testGetAlarmList()
{
	QList<AlarmInfo> alarmlist;
	
	// Set an alarm.
	QTime alarmTime;
	QString descInfo("This is a daily alarm");

	AlarmInfo alarmInfoDaily;
	// First the description.
	alarmInfoDaily.alarmDesc = descInfo;
	// The time.
	alarmTime = QTime::fromString("12:30 pm", "hh:mm ap");
	// Construct the alarm info.
	alarmInfoDaily.origAlarmTime = alarmTime;
	// Fill the repeat type.
	alarmInfoDaily.repeatType = Daily;

	// Request the listener to set the alarm.
	mAlarmClient->setAlarm(alarmInfoDaily);
	
	// Check if the list has only one element.
	mAlarmClient->getAlarmList(alarmlist);
	int numOfAlarms = alarmlist.count();
	QVERIFY(1 == numOfAlarms);
}

/*!
	Tests the api AlarmClient::setAlarm with a daily alarm.
 */
void TestAlarmClient::testSetAlarmDaily()
{
	QTime alarmTime;
	QString descInfo("This is a daily alarm");
	
	AlarmInfo alarmInfoDaily;
	// First the description.
	alarmInfoDaily.alarmDesc = descInfo;
	// The time.
	alarmTime = QTime::fromString("12:30 pm", "hh:mm ap");
	// Construct the alarm info.
	alarmInfoDaily.origAlarmTime = alarmTime;
	// Fill the repeat type.
	alarmInfoDaily.repeatType = Daily;

	// Request the listener to set the alarm.
	mAlarmClient->setAlarm(alarmInfoDaily);
	// Get the Id of the alarm set.
	int tempId = alarmInfoDaily.alarmId;

	// Retrieve the alarm information and compare.
	AlarmInfo retrievedAlarmInfoDaily;
	int error = mAlarmClient->getAlarmInfo(tempId, retrievedAlarmInfoDaily);

	if (0 == error) {
		QVERIFY(alarmInfoDaily.origAlarmTime == 
			    retrievedAlarmInfoDaily.origAlarmTime);
		QVERIFY(alarmInfoDaily.alarmDesc == 
			    retrievedAlarmInfoDaily.alarmDesc);
		QVERIFY(alarmInfoDaily.repeatType == 
			    retrievedAlarmInfoDaily.repeatType);
		QVERIFY(alarmInfoDaily.alarmId == 
			    retrievedAlarmInfoDaily.alarmId);
	}
	else {
		QVERIFY(false);
	}
}

/*!
	Tests the api AlarmClient::setAlarm with a weekly alarm.
 */
void TestAlarmClient::testSetAlarmWeekly()
{
	QTime alarmTime;
	QString descInfo("This is a weekly alarm");

	AlarmInfo alarmInfoWeekly;
	// First the description.
	alarmInfoWeekly.alarmDesc = descInfo;
	// The time.
	alarmTime = QTime::fromString("12:30 pm", "hh:mm ap");
	// Construct the alarm info.
	alarmInfoWeekly.origAlarmTime = alarmTime;
	// Fill the repeat type.
	alarmInfoWeekly.repeatType = Weekly;
	// Fill the day.
	alarmInfoWeekly.alarmDay = 2;
	
	// Request the listener to set the alarm.
	mAlarmClient->setAlarm(alarmInfoWeekly);
	// Get the Id of the alarm set.
	int tempId = alarmInfoWeekly.alarmId;

	// Retrieve the alarm information and compare.
	AlarmInfo retrievedalarmInfoWeekly;
	int error = mAlarmClient->getAlarmInfo(tempId, retrievedalarmInfoWeekly);

	if (0 == error) {
		QVERIFY(alarmInfoWeekly.alarmDesc == 
			    retrievedalarmInfoWeekly.alarmDesc);
		QVERIFY(alarmInfoWeekly.repeatType == 
			    retrievedalarmInfoWeekly.repeatType);
		// TODO: have to check the alarm day. ll do later.
		// QVERIFY(alarmInfoWeekly.alarmDay == 
		// 	    retrievedalarmInfoWeekly.alarmDay);
		QVERIFY(alarmInfoWeekly.alarmId == 
			    retrievedalarmInfoWeekly.alarmId);
		QVERIFY(alarmInfoWeekly.origAlarmTime == 
			    retrievedalarmInfoWeekly.origAlarmTime);
	}
	else {
		QVERIFY(false);
	}
}

/*!
	Tests the api AlarmClient::setAlarmWorkdays with a workdays alarm.
 */
void TestAlarmClient::testSetAlarmWorkdays()
{
	QTime alarmTime;
	QString descInfo("This is a workdays alarm");

	AlarmInfo alarmInfoWorkdays;
	// First the description.
	alarmInfoWorkdays.alarmDesc = descInfo;
	// The time.
	alarmTime = QTime::fromString("12:30 pm", "hh:mm ap");
	// Construct the alarm info.
	alarmInfoWorkdays.origAlarmTime = alarmTime;
	// Fill the repeat type.
	alarmInfoWorkdays.repeatType = Workday;
	
	// Request the listener to set the alarm.
	mAlarmClient->setAlarm(alarmInfoWorkdays);
	// Get the Id of the alarm set.
	int tempId = alarmInfoWorkdays.alarmId;

	// Retrieve the alarm information and compare.
	AlarmInfo retrievedalarmInfoWorkdays;
	int error = mAlarmClient->getAlarmInfo(tempId, retrievedalarmInfoWorkdays);

	if (0 == error) {
		QVERIFY(alarmInfoWorkdays.origAlarmTime ==
			    retrievedalarmInfoWorkdays.origAlarmTime);
		QVERIFY(alarmInfoWorkdays.alarmDesc == 
			    retrievedalarmInfoWorkdays.alarmDesc);
		QVERIFY(alarmInfoWorkdays.repeatType == 
			    retrievedalarmInfoWorkdays.repeatType);
		QVERIFY(alarmInfoWorkdays.alarmId == 
			    retrievedalarmInfoWorkdays.alarmId);
	}
	else {
		QVERIFY(false);
	}
}

/*!
	Tests the api AlarmClient::setAlarmOncleOnly for a once only alarm.
 */
void TestAlarmClient::testSetAlarmOnceOnly()
{
	QTime alarmTime;
	QString descInfo("This is a onceonly alarm");

	AlarmInfo alarmInfoOnceonly;
	// First the description.
	alarmInfoOnceonly.alarmDesc = descInfo;
	// The time.
	alarmTime = QTime::fromString("12:30 pm", "hh:mm ap");
	// Construct the alarm info.
	alarmInfoOnceonly.origAlarmTime = alarmTime;
	// Fill the repeat type.
	alarmInfoOnceonly.repeatType = Once;
	// Fill the day.
	alarmInfoOnceonly.alarmDay = 6;

	// Request the listener to set the alarm.
	mAlarmClient->setAlarm(alarmInfoOnceonly);
	// Get the Id of the alarm set.
	int tempId = alarmInfoOnceonly.alarmId;

	// Retrieve the alarm information and compare.
	AlarmInfo retrievedalarmInfoOnceonly;
	int error = mAlarmClient->getAlarmInfo(tempId, retrievedalarmInfoOnceonly);

	if (0 == error) {
		QVERIFY(alarmInfoOnceonly.alarmDesc == 
			retrievedalarmInfoOnceonly.alarmDesc);
		QVERIFY(alarmInfoOnceonly.repeatType == 
			retrievedalarmInfoOnceonly.repeatType);
		// TODO: have to check the alarm day. ll do later.
		// QVERIFY(alarmInfoOnceonly.alarmDay == 
		//	retrievedalarmInfoOnceonly.alarmDay);
		QVERIFY(alarmInfoOnceonly.alarmId == 
			retrievedalarmInfoOnceonly.alarmId);
		QVERIFY(alarmInfoOnceonly.origAlarmTime == 
			retrievedalarmInfoOnceonly.origAlarmTime);
	}	
	else {
		QVERIFY(false);
	}	
}

QTEST_MAIN(TestAlarmClient)
#include "unittest_alarmclient.moc"

// End of file	--Don't remove this.
