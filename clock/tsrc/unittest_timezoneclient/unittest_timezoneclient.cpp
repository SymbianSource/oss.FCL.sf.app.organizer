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
 * Description:  TimeZoneClient unit test class.
 *
 */

// System includes
#include <QtTest/QtTest>

// User includes
#include "timezoneclient.h"
#include "clockdatatypes.h"
/*!
	\class TestTimeZoneClient
	
	This object has functions to test the public apis in the class
	TimeZoneClient.
 */
class TestTimezoneClient: public QObject
{
	Q_OBJECT

private slots:
	// Test case setup.
	void init();
	void cleanup();
	
	// Test cases.
	void testGetLocations();
	void testGetCurrentZoneInfoL();
	void testSetAsCurrentLocation();
	void testSetAsCurrentLocationWithDst();
	void testIsDSTOnL();
	void testGetStandardOffset();
	void testTimeUpdateOn();
	void testSetTimeUpdateOn();
	void testSetDateTime();
	void testGetAllTimeZoneOffsets();
	void testGetCountriesForUTCOffset();
	void testAddCity();
	void testCheckForDstChange();
	void testGetSavedLocations();
	void testSaveLocations();
	//void testGetLocationInfo();
	//void testGetCityGroupIdByName();
	//void testGetCityOffsetByNameAndId();
private:
	void doCleanup();
private:
	TimezoneClient *mTimezoneClient;
	int mError;
};

/*!
	This function will be called before each testfunction is executed.
 */
void TestTimezoneClient::init()
{
	TRAPD(mError, mTimezoneClient = TimezoneClient::getInstance());
	Q_UNUSED(mError)
	
	Q_ASSERT(!mError);
	Q_ASSERT(mTimezoneClient);
}

/*!
	This function will be called after every testfunction.
 */
void TestTimezoneClient::cleanup()
{
	if (!mTimezoneClient->isNull()) {
		mTimezoneClient->deleteInstance();
	}
}

/*!
	Tests the api TimeZoneClient::getLocations
 */
void TestTimezoneClient::testGetLocations()
{
	QList<LocationInfo> locationList = mTimezoneClient->getLocations();
	QVERIFY(0 < locationList.count());
}

/*!
	Tests the api TestTimezoneClient::getCurrentZoneInfoL
 */
void TestTimezoneClient::testGetCurrentZoneInfoL()
{
	//LocationInfo testLocationInfo = {"Canada", "Toronto", "", -300, 1528, true};
	
	LocationInfo testLocationInfo = {"India", "Chennai", "", 330, 1944, 45, false};
	mTimezoneClient->setAsCurrentLocationL(testLocationInfo);
	LocationInfo retrievedLocationInfo;
	retrievedLocationInfo = mTimezoneClient->getCurrentZoneInfoL();

	// Check the location info set with retrieved info.
	QString temp = retrievedLocationInfo.countryName;
	temp = retrievedLocationInfo.cityName;
	QVERIFY(testLocationInfo.countryName == retrievedLocationInfo.countryName);
	QVERIFY(testLocationInfo.cityName == retrievedLocationInfo.cityName);
	QVERIFY(testLocationInfo.timezoneId == retrievedLocationInfo.timezoneId);
}

/*!
	Tests the api TestTimezoneClient::setAsCurrentLocation.
 */
void TestTimezoneClient::testSetAsCurrentLocation()
{
	LocationInfo testLocationInfo = {"India", "Kolkata", "", 330, 1944, false};
	mTimezoneClient->setAsCurrentLocationL(testLocationInfo);
	LocationInfo retrievedLocationInfo;
	retrievedLocationInfo = mTimezoneClient->getCurrentZoneInfoL();

	// Check the location info set with retrieved info.
	QVERIFY(testLocationInfo.countryName == retrievedLocationInfo.countryName);
	QVERIFY(testLocationInfo.cityName == retrievedLocationInfo.cityName);
	QVERIFY(testLocationInfo.listImageName 
	        == retrievedLocationInfo.listImageName);
	QVERIFY(testLocationInfo.zoneOffset == retrievedLocationInfo.zoneOffset);
	QVERIFY(testLocationInfo.timezoneId == retrievedLocationInfo.timezoneId);
	QVERIFY(testLocationInfo.dstOn == retrievedLocationInfo.dstOn);
}

/*!
	Tests the api TimeZoneClient::dstOn.
 */
void TestTimezoneClient::testSetAsCurrentLocationWithDst()
{
	//QDateTime currentDateTime(QDate(2009, 05, 15), QTime(12, 30));
	//QDateTime currentDateTime(QDate(2010, 03, 28), QTime(4, 0));
	QDateTime currentDateTime(QDate(2010, 03, 31), QTime(8, 30));
	mTimezoneClient->setDateTime(currentDateTime);
	LocationInfo testLocationInfo = {"Finland", "Helsinki", "", 180, 2760, 33,
			                         true};
	mTimezoneClient->setAsCurrentLocationL(testLocationInfo);
	LocationInfo retrievedLocationInfo;
	retrievedLocationInfo = mTimezoneClient->getCurrentZoneInfoL();

	// Check the location info set with retrieved info.
	QVERIFY(testLocationInfo.countryName == retrievedLocationInfo.countryName);
	QVERIFY(testLocationInfo.cityName == retrievedLocationInfo.cityName);
	QVERIFY(testLocationInfo.listImageName 
	        == retrievedLocationInfo.listImageName);
	QVERIFY(testLocationInfo.zoneOffset == retrievedLocationInfo.zoneOffset);
	QVERIFY(testLocationInfo.timezoneId == retrievedLocationInfo.timezoneId);
	QVERIFY(testLocationInfo.dstOn == retrievedLocationInfo.dstOn);
}

/*!
	Tests the api TestTimezoneClient::isDSTOnL
 */
void TestTimezoneClient::testIsDSTOnL()
{
	LocationInfo testLocationInfo = 
							{"Finland", "Helsinki", "", 180, 2760, 33, true};
	mTimezoneClient->setAsCurrentLocationL(testLocationInfo);
	QDateTime dstOnDate(QDate(2010, 03, 31), QTime(8, 30));
	mTimezoneClient->setDateTime(dstOnDate);
	QVERIFY(true == mTimezoneClient->isDSTOnL(2760));
	
	QDateTime dstOffDate(QDate(2010, 03, 27), QTime(8, 30));
	mTimezoneClient->setDateTime(dstOffDate);
	QVERIFY(false == mTimezoneClient->isDSTOnL(2760));
	
	LocationInfo testLocationInfo1 = {"India", "Kolkata", "", 330, 1944, false};
	mTimezoneClient->setAsCurrentLocationL(testLocationInfo1);
	QVERIFY(false == mTimezoneClient->isDSTOnL(1944));
}

/*!
	Tests the api TimeZoneClient::getStandardOffset
 */
void TestTimezoneClient::testGetStandardOffset()
{
	// For India
	int expectedOffset = 330;
	int offset = mTimezoneClient->getStandardOffset(1944);
	QVERIFY(expectedOffset == offset);
	
	// For London
	expectedOffset = 345;
	offset = mTimezoneClient->getStandardOffset(1936);
	QVERIFY(expectedOffset == offset);
}

/*!
	Tests the api TimeZoneClient::timeUpdateOn
 */
void TestTimezoneClient::testTimeUpdateOn()
{
	mTimezoneClient->setTimeUpdateOn(true);
	QVERIFY(true == mTimezoneClient->timeUpdateOn());
	mTimezoneClient->setTimeUpdateOn(false);
	QVERIFY(false == mTimezoneClient->timeUpdateOn());
}

/*!
	Tests the api TimeZoneClient::timeUpdateOn
 */
void TestTimezoneClient::testSetTimeUpdateOn()
{
	bool timeUpdate = mTimezoneClient->timeUpdateOn();
	mTimezoneClient->setTimeUpdateOn(!timeUpdate);
	
	QVERIFY(timeUpdate != mTimezoneClient->timeUpdateOn());
}

/*!
	Tests the api TimeZoneClient::setDateTime
 */
void TestTimezoneClient::testSetDateTime()
{
	QDateTime currentDateTime = QDateTime::currentDateTime();

	QDateTime expectedDateTime1 = currentDateTime.addDays(10).addSecs(3600);
	mTimezoneClient->setDateTime(expectedDateTime1);
	QVERIFY(currentDateTime.date() != QDate::currentDate());
	QVERIFY(currentDateTime.time().toString("hh:mm ap") != QTime::currentTime().toString("hh:mm ap"));
	QVERIFY(expectedDateTime1.date() == QDate::currentDate());
	QVERIFY(expectedDateTime1.time().toString("hh:mm ap") == QTime::currentTime().toString("hh:mm ap"));

	QDateTime expectedDateTime2 = currentDateTime.addDays(100).addSecs(7200);

	mTimezoneClient->setDateTime(expectedDateTime2);
	QVERIFY(expectedDateTime2.date() == QDate::currentDate());
	QVERIFY(expectedDateTime2.time().toString("hh:mm ap") == QTime::currentTime().toString("hh:mm ap"));
}

/*!
	Tests the api TimeZoneClient::getAllTimeZoneOffsets
 */
void TestTimezoneClient::testGetAllTimeZoneOffsets()
{
	int indiaOffset = 330;
	QList<int> offsetList = mTimezoneClient->getAllTimeZoneOffsets();
	QVERIFY(offsetList.count());
	QVERIFY(offsetList.contains(indiaOffset));
}

/*!
	Tests the api TimeZoneClient::getCountriesForUTCOffset
 */
void TestTimezoneClient::testGetCountriesForUTCOffset()
{
	int offset = 330;
	QList<QString> expectedList;
	expectedList.append("India");
	expectedList.append("Sri Lanka");
	QList<LocationInfo> countryList = 
			mTimezoneClient->getCountriesForUTCOffset(offset);
	int count = countryList.count();
	QVERIFY(expectedList.count() == count);
	
	QList<QString> nameList;
	for(int i = 0; i < count; i++) {
		nameList.append(countryList[i].countryName);
	}
	for(int i = 0; i < count; i++) {
		QVERIFY(nameList.contains(expectedList[i]));
	}
	
	// TODO: // Add negative
	offset = -12;
	countryList.clear();
	countryList = mTimezoneClient->getCountriesForUTCOffset(offset);
	QVERIFY(!countryList.count());
}

/*!
	Tests the api TimeZoneClient::addCity
 */
void TestTimezoneClient::testAddCity()
{
	QString cityName("Cochin");
	LocationInfo newLocInfo;
	TRAPD(err, newLocInfo = mTimezoneClient->addCity(1944, cityName, 45));
	if(err == KErrAlreadyExists) {
		// City is already existing.
	}
	QList<LocationInfo> &list = mTimezoneClient->getLocations();
	QList<QString> nameList;
	for(int i = 0; i < list.count(); i++) {
		nameList.append(list[i].cityName);
	}
	QVERIFY(nameList.contains(cityName));
}

/*!
	Tests the api TimeZoneClient::checkForDstChange
 */
void TestTimezoneClient::testCheckForDstChange()
{
	LocationInfo testLocationInfo = 
								{"Finland", "Helsinki", "", 180, 2760, 33, true};
	mTimezoneClient->setAsCurrentLocationL(testLocationInfo);
	// Finland, Helsinki
	// DST started on Sunday, 28 March 2010, 03:00 local standard time
	// DST ends on Sunday, 31 October 2010, 04:00 local daylight time
	QDateTime dstOnDate(QDate(2010, 10, 31), QTime(3, 45));
	mTimezoneClient->setDateTime(dstOnDate);
	mTimezoneClient->setAsCurrentLocationL(testLocationInfo);

	AlarmInfo alarmDetails;
	alarmDetails.origAlarmTime = QTime::currentTime().addSecs(3600);
	alarmDetails.alarmDateTime = QDate::currentDate();
	alarmDetails.repeatType = AlarmRepeatType(0);
	alarmDetails.alarmDay = 1;
	bool dstChange = mTimezoneClient->checkForDstChange(alarmDetails);
	QVERIFY(dstChange == true);
	
	QDateTime dstOffDate(QDate(2010, 10, 31), QTime(04, 01));
	mTimezoneClient->setDateTime(dstOffDate);
	AlarmInfo alarmDetails1;
	alarmDetails1.origAlarmTime = QTime::currentTime().addSecs(3600);
	alarmDetails1.alarmDateTime = QDate::currentDate();
	alarmDetails1.repeatType = AlarmRepeatType(0);
	alarmDetails1.alarmDay = 1;
	dstChange = mTimezoneClient->checkForDstChange(alarmDetails1);
	QVERIFY(dstChange == false);
	
	LocationInfo testLocationInfo1 = 
									{"India", "Chennai", "", 330, 1944, 45, false};
	mTimezoneClient->setAsCurrentLocationL(testLocationInfo1);
	mTimezoneClient->setDateTime(dstOnDate);
	mTimezoneClient->setAsCurrentLocationL(testLocationInfo1);
	dstChange = mTimezoneClient->checkForDstChange(alarmDetails1);
	QVERIFY(dstChange == false);
}

/*!
	Tests the api TimeZoneClient::getSavedLocations
 */
void TestTimezoneClient::testGetSavedLocations()
{
	doCleanup();
	mTimezoneClient->setDateTime(QDateTime(QDate(2010, 8, 15), QTime(9,30)));
	
	QList<LocationInfo> locationList = mTimezoneClient->getSavedLocations();
	int firstcount = locationList.count();
	LocationInfo testLocationInfo1 = {"Japan", "Tokyo", "", 540, 2192, 53, 
			false};
	QList<LocationInfo> testList;
	testList << testLocationInfo1;
	mTimezoneClient->saveLocations(testList);
	
	locationList.clear();
	locationList = mTimezoneClient->getSavedLocations();
	int cnt = locationList.count();
	QVERIFY((firstcount + 1) == locationList.count());
	
	QList<QString> nameList;
	int index = 0;
	for(int i = 0; i < locationList.count(); i++) {
		if(testLocationInfo1.countryName == locationList[i].countryName) {
			index = i;
		}
	}
	QVERIFY(testLocationInfo1.countryName == locationList[index].countryName);
	QVERIFY(testLocationInfo1.cityName == locationList[index].cityName);
	QVERIFY(testLocationInfo1.listImageName 
	        == locationList[index].listImageName);
	QVERIFY(testLocationInfo1.zoneOffset == locationList[index].zoneOffset);
	QVERIFY(testLocationInfo1.timezoneId == locationList[index].timezoneId);
	QVERIFY(testLocationInfo1.dstOn == locationList[index].dstOn);
	
	LocationInfo testLocationInfo2 = {"Singapore", "Singapore", "", 480, 2144, 87, false};
	LocationInfo testLocationInfo3 = {"Finland", "Helsinki", "", 180, 2760, 33, true};
	testList << testLocationInfo2
			<< testLocationInfo3;
	mTimezoneClient->saveLocations(testList);
	locationList.clear();
	locationList = mTimezoneClient->getSavedLocations();
	QVERIFY((firstcount + 3) == locationList.count());
	doCleanup();
	
}

/*!
	Tests the api TimeZoneClient::saveLocations.
 */
void TestTimezoneClient::testSaveLocations()
{
	doCleanup();
	LocationInfo testLocationInfo = {"India", "Kolkata", "", 330, 1944, 45, false};
	QList<LocationInfo> testList;
	testList << testLocationInfo;
	mTimezoneClient->saveLocations(testList);
	QList<LocationInfo> resultantList = mTimezoneClient->getSavedLocations();
	
	// Check the location info saved with retrieved info.
	QVERIFY(resultantList.at(0).countryName == testList.at(0).countryName);
	QVERIFY(resultantList.at(0).cityName == testList.at(0).cityName);
	QVERIFY(resultantList.at(0).listImageName == testList.at(0).listImageName);
	QVERIFY(resultantList.at(0).zoneOffset == testList.at(0).zoneOffset);
	QVERIFY(resultantList.at(0).timezoneId == testList.at(0).timezoneId);
	QVERIFY(resultantList.at(0).dstOn == testList.at(0).dstOn);
	doCleanup();
}
void TestTimezoneClient::doCleanup()
{
	// Cleanup
	QString fileName("c:\\data\\clock\\cityInfo.dat");
	QFile cityInfoFile(fileName);
	cityInfoFile.remove();
}
/*
!
	Tests the api TimeZoneClient::getLocationInfo.
 
void TestTimezoneClient::testGetLocationInfo()
{
	LocationInfo gotInfo;
	LocationInfo testInfo = {"India", "Mumbai", "", 330, 1624, false};
	mTimezoneClient->getLocationInfo(45, 2, gotInfo);

	// Check the location info got by specifying groupId and cityIndex 
	// with testInfo.
	QVERIFY(testInfo.countryName == gotInfo.countryName);
	QVERIFY(testInfo.cityName == gotInfo.cityName);
	QVERIFY(testInfo.listImageName == gotInfo.listImageName);
	QVERIFY(testInfo.zoneOffset == gotInfo.zoneOffset);
	QVERIFY(testInfo.timezoneId == gotInfo.timezoneId);
	QVERIFY(testInfo.dstOn == gotInfo.dstOn);
}

!
	Test the api TimeZoneClient::testGetCityGroupIdByName.
 
void TestTimezoneClient::testGetCityGroupIdByName()
{
	int retrievedGroupId = mTimezoneClient->getCityGroupIdByName("India");
	mTimezoneClient->get
	// Check the retrieved city group id with actual id.
	QVERIFY(45 == retrievedGroupId);	
}

!
	Test the api TestTimezoneClient::getCityOffsetByNameAndId.
 
void TestTimezoneClient::testGetCityOffsetByNameAndId()
{
	int retrievedCityOffset = 
		mTimezoneClient->getCityOffsetByNameAndId("Mumbai",1624);
	
	// Check the retrieved city offset with actual offset.
	QVERIFY(1 == retrievedCityOffset);
}
*/

QTEST_MAIN(TestTimezoneClient)
#include "unittest_timezoneclient.moc"

// End of file	--Don't remove this.
