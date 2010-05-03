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
 * Description: AgendaUtil unit test class.
 *
 */

// System includes
#include <QtTest/QtTest>
#include <QtGui>
#include <e32base.h>

// User includes
#include "agendautil.h"

class TestAgendaUtil : public QObject
{
	Q_OBJECT
	
public slots:
	void handleEntriesDeletion(int error);

private:
	int createMultipleEntries();
	
private slots:
	// Test case setup.
	void init();
	void cleanup();
	void initTestCase();
	void cleanupTestCase();
	
	// Test cases.
	void test_addEntry();
	void test_deleteEntry();
	void test_deleteRepeatedEntry();
	void test_fetchById();
	void test_updateEntry();
	void test_createEntryIdListForDay();
	void test_entryIds();
	void test_fetchAllEntries();
	void test_deleteEntries();
	void test_fetchEntriesInRange();

private:
	AgendaUtil *mAgendaUtil;
	CActiveSchedulerWait* mWait;
};

/*!
	\class TestAgendaUtil
	
	This is unittest class for AgendaUtil apis.
 */

/*!
	This function is called to before every testcase.
 */
void TestAgendaUtil::init()
{
	/*QT_TRAP_THROWING(mAgendaUtil = new AgendaUtil);

	Q_ASSERT_X(mAgendaUtil, "TestAgendaUtil::test_addEntry", "No agendautil");
	
	// Connect to the required signals
	connect(
			mAgendaUtil, SIGNAL(entriesDeleted(int)), this,
			SLOT(handleEntriesDeletion(int)));

	mWait = NULL;*/
}

/*!
	This function is called after every testcase.
 */
void TestAgendaUtil::cleanup()
{
		
	//User::Exit(0);
    if (mWait && mWait->IsStarted()) {
                mWait->AsyncStop();
    }
    delete mWait;
    mWait = NULL;
}
/*!
	This function is called to init the testcase.
 */
void TestAgendaUtil::initTestCase()
{
	QT_TRAP_THROWING(mAgendaUtil = new AgendaUtil);
	
	Q_ASSERT_X(mAgendaUtil, "TestAgendaUtil::test_addEntry", "No agendautil");
	
	// Connect to the required signals
	connect(
			mAgendaUtil, SIGNAL(entriesDeleted(int)), this,
			SLOT(handleEntriesDeletion(int)));
	
	mWait = NULL;
}

void TestAgendaUtil::cleanupTestCase()
{
    //User::Exit(0);
    QCoreApplication::processEvents();
    if (mAgendaUtil) {
            delete mAgendaUtil;
    }
}
/*
	Tests the AgendaUtil::addEntry.
 */
void TestAgendaUtil::test_addEntry()
{
	// Test case setup.
	AgendaEntry entry;
	QDate dateSet(2010,12,1);
	QTime timeSet(10,10,10,10);
	QDateTime dateTimeSet(dateSet,timeSet);
	entry.setType(AgendaEntry::TypeEvent);
	entry.setSummary("Test case");
	entry.setStartAndEndTime(dateTimeSet, dateTimeSet);
	
	// Run the case.
	ulong id = mAgendaUtil->addEntry(entry);
	
	// Verify.
	AgendaEntry newEntry = mAgendaUtil->fetchById(id);
	QDateTime dateTimeRetrieved = newEntry.startTime();
	QDate dateRetrieved = dateTimeRetrieved.date();
	QTime timeRetrieved = dateTimeRetrieved.time();
	QVERIFY(entry.summary() == newEntry.summary());
	QVERIFY(dateRetrieved == dateSet);
	QVERIFY(entry.type() == newEntry.type());
	// Cleanup.
	QVERIFY(mAgendaUtil->deleteEntry(id));
}

/*!
	Tests the api AgendaUtil::deleteEntry.
 */
void TestAgendaUtil::test_deleteEntry()
{
	// Test case setup.
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeEvent);
	entry.setSummary("Test case");
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(), 
			QDateTime::currentDateTime());
	ulong id = mAgendaUtil->addEntry(entry);
	
	// Run the case.
	QVERIFY(mAgendaUtil->deleteEntry(id));
	
	// Verify.
	QVERIFY(mAgendaUtil->fetchById(id).isNull());
}

/*!
	Test the api AgendaUtil::deleteRepeatedEntry.
 */
void TestAgendaUtil::test_deleteRepeatedEntry()
{
	// Test case setup.
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeAppoinment);
	entry.setSummary("Test case");
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(), 
			QDateTime::currentDateTime());
	AgendaRepeatRule repeatRule;
	repeatRule.setType(AgendaRepeatRule::DailyRule);
	QDateTime ruleStartDateTime(QDate(2019, 11, 3));
	repeatRule.setRepeatRuleStart(ruleStartDateTime);
	QDateTime until(QDate(2019, 12, 3));
	repeatRule.setUntil(until);
	repeatRule.setInterval(1);
	entry.setRepeatRule(repeatRule);
	
	ulong id = mAgendaUtil->addEntry(entry);
	AgendaEntry savedEntry = mAgendaUtil->fetchById(id);
	QVERIFY(!savedEntry.isNull());
	QVERIFY(savedEntry.isRepeating());
	
	// Run the case.
	mAgendaUtil->deleteRepeatedEntry(savedEntry, AgendaUtil::ThisAndAll);
	
	// Verify.
	QVERIFY(mAgendaUtil->fetchById(id).isNull());
}

/*!
    Test the api AgendaUtil::fetchById.
 */
void TestAgendaUtil::test_fetchById()
{
	// Test case setup.
	AgendaEntry entry;
	QDate dateSet(2010, 12, 1);
	QTime timeSet(10, 10, 10, 10);
	QDateTime dateTimeSet(dateSet, timeSet);
	entry.setType(AgendaEntry::TypeEvent);
	entry.setSummary("Test case");
	entry.setStartAndEndTime(dateTimeSet, dateTimeSet);

	// Run the case.
	ulong id = mAgendaUtil->addEntry(entry);

	// Verify.
	AgendaEntry retrievedEntry = mAgendaUtil->fetchById(id);
	
	// Check if the fetched entry is same st the added entry
	QDateTime dateTimeRetrieved = retrievedEntry.startTime();
	QDate dateRetrieved = dateTimeRetrieved.date();
	QTime timeRetrieved = dateTimeRetrieved.time();
	QVERIFY(dateRetrieved == dateSet);
	QVERIFY(entry.type() == retrievedEntry.type());
	QVERIFY(entry.summary() == retrievedEntry.summary());
	mAgendaUtil->deleteEntry(id);
	
	// Fetch the same again and check if its null.
	AgendaEntry checkEntryIsNull = mAgendaUtil->fetchById(id);
	QVERIFY(checkEntryIsNull.isNull());
}

/*!
    Test the api AgendaUtil::updateEntry.
 */
void TestAgendaUtil::test_updateEntry()
{
	// Test case setup.
	AgendaEntry entry;
	entry.setType(AgendaEntry::TypeAppoinment);
	entry.setSummary("Test case");
	entry.setDescription("Entry to test the updateEntry API");
	entry.setLocation("Nokia");
	entry.setStartAndEndTime(
			QDateTime::currentDateTime(), 
			QDateTime::currentDateTime());

	// Save the entry
	ulong id = mAgendaUtil->addEntry(entry);

	// Fetch the same entry 
	AgendaEntry savedEntry = mAgendaUtil->fetchById(id);
	QVERIFY(!savedEntry.isNull());
	savedEntry.setSummary("Fetced Entry");
	savedEntry.setLocation("India");
	savedEntry.setStartAndEndTime(
			QDateTime::currentDateTime().addDays(1), 
			QDateTime::currentDateTime().addDays(1));

	// Update the entry
	bool success = mAgendaUtil->updateEntry(savedEntry);
	QVERIFY(success);

	// Fetch the updated entry again
	AgendaEntry updatedEntry = mAgendaUtil->fetchById(id); 

	// Verify
	QVERIFY(savedEntry.summary() == updatedEntry.summary());
	QVERIFY(savedEntry.type() == updatedEntry.type());

	// Cleanup.
	QVERIFY(mAgendaUtil->deleteEntry(id));
}

/*!
	Test the api AgendaUtil::createEntryIdListForDay.
 */
void TestAgendaUtil::test_createEntryIdListForDay()
{
	// Clean up all the entries.
	QDateTime startRange(QDate(1900, 01, 1), QTime(0, 0, 0, 0));
	QDateTime endRange(QDate(2100, 01, 01), QTime(23, 59, 59, 0));
	mAgendaUtil->deleteEntries(startRange, endRange);
	// Start the timer as the above operation is asynchronous   
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}
	// Test for futur day.
	AgendaEntry futureEntry;
	// Create the day for which entries are needed
	QDateTime futureDay(QDate(2009, 05, 01), QTime(0, 0, 0, 0));
	futureEntry.setStartAndEndTime(futureDay, futureDay);
	futureEntry.setType(AgendaEntry::TypeEvent);
	futureEntry.setSummary("Test case");
	ulong id = mAgendaUtil->addEntry(futureEntry);
	// Run the test case
	QList<AgendaEntry> entriesList;
	entriesList.append(mAgendaUtil->createEntryIdListForDay(futureDay));

	// Verify
	int num = entriesList.count();
	QVERIFY( 1 == entriesList.count());
	QString summary(entriesList.at(0).summary());
	QVERIFY(summary == "Test case");
	QVERIFY(entriesList.at(0).type() == AgendaEntry::TypeEvent);

	// Cleanup
	mAgendaUtil->deleteEntry(id);
}

/*!
	Test the api AgendaUtil::entryIds.
 */
void TestAgendaUtil::test_entryIds()
{
	// Clean up all the entries.
	QDateTime startRange(QDate(1900, 01, 1), QTime(0, 0, 0, 0));
	QDateTime endRange(QDate(2100, 01, 01), QTime(23, 59, 59, 0));
	mAgendaUtil->deleteEntries(startRange, endRange);

	qDebug("Request for deletion made.");

	// Start the timer as the above operation is asynchronous   
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}

	qDebug("Waiting for confirmation.");

	QList<AgendaEntry> entriesRetrieved =
	        mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	int numOfEntriesRetrieved = entriesRetrieved.count();
	QList<ulong> checkList = mAgendaUtil->entryIds();
	int checkNum = checkList.count();

	// Fetch the entries and see if none are there
	QList<ulong> testIds;
	testIds.clear();
	int count = testIds.count();
	testIds = mAgendaUtil->entryIds();
	count = testIds.size();
	int numOfEntries = 0;

	// Appointment.
	AgendaEntry firstEntry;
	firstEntry.setType(AgendaEntry::TypeAppoinment);
	firstEntry.setSummary("first entry");
	QDateTime firstEntryStart(QDate(2020, 05, 01), QTime(9, 0, 0, 0));
	QDateTime firstEntryEnd(QDate(2020, 05, 01), QTime(10, 0, 0, 0));
	firstEntry.setStartAndEndTime(firstEntryStart, firstEntryEnd);
	ulong id = mAgendaUtil->addEntry(firstEntry);
	numOfEntries++;

	// ToDo.
	AgendaEntry firstToDo;
	firstToDo.setType(AgendaEntry::TypeTodo);
	firstToDo.setSummary("first ToDo");
	QDateTime firstToDoStart(QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	QDateTime firstToDoEnd(QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	firstToDo.setStartAndEndTime(firstToDoStart, firstToDoEnd);
	id = mAgendaUtil->addEntry(firstToDo);
	numOfEntries++;

	// Run the test case
	QList<ulong> ids;
	int num = ids.count();
	ids = mAgendaUtil->entryIds(AgendaUtil::IncludeAppointments);

	num = ids.count();
	QVERIFY(1 == num);
}
/*!
	Test the api AgendaUtil::fetchAllEntries.
 */
void TestAgendaUtil::test_fetchAllEntries()
{
	// Clean up all the entries.
	QDateTime startRange(QDate(1900, 01, 1), QTime(0, 0, 0, 0));
	QDateTime endRange(QDate(2100, 12, 30), QTime(23, 59, 59, 0));
	mAgendaUtil->deleteEntries(startRange, endRange);

	qDebug("Request for deletion made.");

	// Start the timer as the above operation is asynchronous   
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}

	qDebug("Waiting for confirmation.");

	QList<AgendaEntry> entriesRetrieved =
	        mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	int numOfEntriesRetrieved = entriesRetrieved.count();
	QList<ulong> checkList = mAgendaUtil->entryIds();
	int checkNum = checkList.count();

	if (numOfEntriesRetrieved != 0 || checkNum != 0) {
		mAgendaUtil->deleteEntries(startRange, endRange);

		qDebug("Request for deletion made.");

		// Start the timer as the above operation is asynchronous   
		if (!mWait) {
			QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
			if (!mWait->IsStarted()) {
				mWait->Start();
			}
		}

		qDebug("Waiting for confirmation.");
	}

	int numOfEntries = 0;

	// Appointment.
	AgendaEntry firstEntry;
	firstEntry.setType(AgendaEntry::TypeAppoinment);
	firstEntry.setSummary("first entry");
	QDateTime firstEntryStart(QDate(2020, 05, 01), QTime(9, 0, 0, 0));
	QDateTime firstEntryEnd(QDate(2020, 05, 01), QTime(10, 0, 0, 0));
	firstEntry.setStartAndEndTime(firstEntryStart, firstEntryEnd);
	ulong id = mAgendaUtil->addEntry(firstEntry);
	numOfEntries++;

	// ToDo.
	AgendaEntry firstToDo;
	firstToDo.setType(AgendaEntry::TypeTodo);
	firstToDo.setSummary("first ToDo");
	QDateTime firstToDoStart(QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	QDateTime firstToDoEnd(QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	firstToDo.setStartAndEndTime(firstToDoStart, firstToDoEnd);
	id = mAgendaUtil->addEntry(firstToDo);
	numOfEntries++;

	// Run the test case
	QList<AgendaEntry> entries;
	entries.clear();
	entries = mAgendaUtil->fetchAllEntries();

	// Verfiy
	numOfEntriesRetrieved = entries.count();
	QVERIFY(numOfEntries == numOfEntriesRetrieved);
}

/*!
	Test the api AgendaUtil::deleteEntries.
 */
void TestAgendaUtil::test_deleteEntries()
{
	// The range required for the deletion.
	QDateTime startRange(QDate(1900, 01, 01), QTime(0, 0, 0, 0));

	QDateTime endRange(QDate(2100, 01, 01), QTime(23, 59, 59, 0));

	QList<AgendaEntry> entriesRetrieved =
	        mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	int numOfEntriesRetrieved = entriesRetrieved.count();
	QList<ulong> checkList = mAgendaUtil->entryIds();
	int checkNum = checkList.count();
		
	// Create the required entries.
	int numOfEntries = createMultipleEntries();
	
	entriesRetrieved = mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	numOfEntriesRetrieved = entriesRetrieved.count();
	checkList = mAgendaUtil->entryIds();
	checkNum = checkList.count();

	Q_ASSERT(numOfEntries);
	
	qDebug("Entries have been created");

	// Execute the case.
	mAgendaUtil->deleteEntries(startRange, endRange);
	
	qDebug("Request for deletion made.");

	// Start the timer as the above operation is asynchronous   
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}
	
	qDebug("Waiting for confirmation.");
	entriesRetrieved = mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	numOfEntriesRetrieved = entriesRetrieved.count();
	checkList = mAgendaUtil->entryIds();
	checkNum = checkList.count();
	QVERIFY(0 == numOfEntriesRetrieved);
}

/*!
	Test the api AgendaUtil::fetchEntriesInRange.
 */
void TestAgendaUtil::test_fetchEntriesInRange()
{
	// Clean up all the entries.
	QDateTime startRange(QDate(1900, 01, 1), QTime(0, 0, 0, 0));
	QDateTime endRange(QDate(2100, 12, 30), QTime(23, 59, 59, 0));
	mAgendaUtil->deleteEntries(startRange,endRange);
	
	//qDebug("Request for deletion made.");

	// Start the timer as the above operation is asynchronous   
	if (!mWait) {
		QT_TRAP_THROWING(mWait = new (ELeave) CActiveSchedulerWait;);
		if (!mWait->IsStarted()) {
			mWait->Start();
		}
	}

	//qDebug("Waiting for confirmation.");
	
	QList<AgendaEntry> entriesRetrieved =
			mAgendaUtil->fetchEntriesInRange(startRange, endRange);
	int numOfEntriesRetrieved = entriesRetrieved.count();
	QList<ulong> checkList = mAgendaUtil->entryIds();
	int checkNum = checkList.count();
		
	// Appointment.
	AgendaEntry firstEntry;
	firstEntry.setType(AgendaEntry::TypeAppoinment);
	firstEntry.setSummary("first entry");
	QDateTime firstEntryStart(
			QDate(2020, 05, 01), QTime(9, 0, 0, 0));
	QDateTime firstEntryEnd(
			QDate(2020, 05, 01), QTime(10, 0, 0, 0));
	firstEntry.setStartAndEndTime(firstEntryStart, firstEntryEnd);
	ulong id = mAgendaUtil->addEntry(firstEntry);
	
	// Create the range for which entries are needed
	QDateTime fromRange(
			QDate(2020, 01, 01), QTime(0, 0, 0, 0));
	QDateTime toRange(
			QDate(2023, 12, 31), QTime(23, 59, 0, 0));
	// Run the test case
	QList<AgendaEntry> entries = mAgendaUtil->fetchEntriesInRange(
			fromRange, toRange);

	// Verify
	int num = entries.count();                             
	QVERIFY( 1 == entries.count());
	
	//cleanup();
	
	//User::Exit(0);
}

/*!
	Slot to listen for the completion of deleting the entries asynchronously.
 */
void TestAgendaUtil::handleEntriesDeletion(int error)
{
	// Stop the wait timer.
	if (mWait && mWait->IsStarted()) {
		mWait->AsyncStop();
	}
	QVERIFY(!error);
}

/*!
	Private function to create entries needed for some of the test cases.
 */
int TestAgendaUtil::createMultipleEntries()
{
	// Start creating the entries.
	int numOfEntries = 0;

	// Appointments.
	AgendaEntry firstEntry;
	firstEntry.setType(AgendaEntry::TypeAppoinment);
	firstEntry.setSummary("first entry");
	QDateTime firstEntryStart(
			QDate(2020, 05, 01), QTime(9, 0, 0, 0));
	QDateTime firstEntryEnd(
			QDate(2020, 05, 01), QTime(10, 0, 0, 0));
	firstEntry.setStartAndEndTime(firstEntryStart, firstEntryEnd);
	ulong id = mAgendaUtil->addEntry(firstEntry);
	numOfEntries++;

	// Events.
	AgendaEntry firstEvent;
	firstEvent.setType(AgendaEntry::TypeEvent);
	firstEvent.setSummary("first event");
	QDateTime firstEventStart(
			QDate(2023, 05, 01), QTime(10, 0, 0, 0));
	QDateTime firstEventEnd(
			QDate(2023, 05, 01), QTime(12, 0, 0, 0));
	firstEvent.setStartAndEndTime(firstEventStart, firstEventEnd);
	id = mAgendaUtil->addEntry(firstEvent);
	numOfEntries++;

	// ToDos.
	AgendaEntry firstToDo;
	firstToDo.setType(AgendaEntry::TypeTodo);
	firstToDo.setSummary("first ToDo");
	QDateTime firstToDoStart(
			QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	QDateTime firstToDoEnd(
				QDate(2026, 05, 01), QTime(10, 0, 0, 0));
	firstToDo.setStartAndEndTime(firstToDoStart, firstToDoEnd);
	id = mAgendaUtil->addEntry(firstToDo);
	numOfEntries++;

	return numOfEntries;
}

QTEST_MAIN(TestAgendaUtil)
#include "unittest_agendautil.moc"

// End of file	--Don't remove this.
