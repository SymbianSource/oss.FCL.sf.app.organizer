/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Definition file for the TimeZoneClient class.
*
*/

// System includes
#include <QList>
#include <QStandardItemModel>
#include <QtAlgorithms>
#include <tzlocalizationdatatypes.h>
#include <tzlocalizer.h>
#include <tz.h>
#include <vtzrules.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>

// User includes
#include "timezoneclient.h"
#include "clockcommon.h"
#include "clockserverclt.h"

const int KDaysInWeek(7);
const int KZerothRule(0);
const int KOneHour(1);
const int KNoDifference(0);

/*!
	\class TimezoneClient

	This class is the QObject based client interface for services offered by
	tzserver and timezonelocalization.
 */

TimezoneClient* TimezoneClient::mTimezoneClient = 0;
int TimezoneClient::mReferenceCount = 0;
/*!
	Call this funtion to instantiate the TimezoneClient class.
 */
TimezoneClient* TimezoneClient::getInstance()
{
	if (!mTimezoneClient) {
		mTimezoneClient = new TimezoneClient();
	}

	mReferenceCount++;
	return mTimezoneClient;
}

/*!
	Call this function to clean up the instance of TimezoneClient class.
 */
void TimezoneClient::deleteInstance()
{
	mReferenceCount--;

	if (0 == mReferenceCount) {
		delete mTimezoneClient;
		mTimezoneClient = 0;
	}
}

/*!
	Call this function to check if the object is NULL.
 */
bool TimezoneClient::isNull()
{
	bool deleted = false;
	if (0 == mReferenceCount) {
		deleted = true;
	}
	return deleted;
}

/*!
	The constructor.
 */
TimezoneClient::TimezoneClient()
{
	TCallBack callback(environmentCallback, this);

	mNotifier = CEnvironmentChangeNotifier::NewL(
			CActive::EPriorityStandard, callback);
	mNotifier->Start();

	mTzLocalizer = CTzLocalizer::NewL();

	// Create the settings manager.
	mSettingsManager = new XQSettingsManager(this);

	// Create the key for auto time update.
	mAutoTimeUpdateKey = new XQSettingsKey(
			XQSettingsKey::TargetCentralRepository,
			KCRUidNitz,
			KActiveProtocol);

	// Start the monitoring for the auto time update key.
	mSettingsManager->startMonitoring(*mAutoTimeUpdateKey);

	// Get the value of auto time update from cenrep.
	mAutoTimeUpdateValue = timeUpdateOn();

	// Listen to the key value changes.
	connect(
			mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
			this, SLOT(eventMonitor(XQSettingsKey, QVariant)));
}

/*!
	Destructor.
 */
TimezoneClient::~TimezoneClient()
{
	if (mNotifier) {
		mNotifier->Cancel();
		delete mNotifier;
		mNotifier = 0;
	}
	if (mWorldClockModel) {
		mWorldClockModel->clear();
	}
	if (mTzLocalizer) {
		delete mTzLocalizer;
		mTzLocalizer = NULL;
	}
	if (mTimeZoneIds.count()) {
		mTimeZoneIds.clear();
	}
	// Clear the locations if exist.
	if (mAllLocations.count()) {
		mAllLocations.clear();
	}

}

/*!
	This functions returns a QList of location info which represent all the
	cities preset in the device returned by the tz server.

	\return QList<LocationInfo> A list of location info objects which contain a
			valid cityName, countryName, tz id and city group id. None of the other
			data is filled.
 */
QList<LocationInfo>& TimezoneClient::getLocations()
{
	if (mAllLocations.count()) {
		mAllLocations.clear();
	}

	// Get the cities, in alphabetical-ascending sorted order.
	CTzLocalizedCityArray* cityArray =
			mTzLocalizer->GetCitiesL(CTzLocalizer::ETzAlphaNameAscending);
	ASSERT(cityArray);
	CleanupStack::PushL(cityArray);
	int cityCount = cityArray->Count();

	CTzLocalizedCityGroup* country = 0;
	int prevGroupId = -1;
	TPtrC countryName;

	// Now get the country\city-group of each of the city.
	// Print the timezone id and city group id as well.
	for (int iter = 0; iter < cityCount; iter++) {
		CTzLocalizedCity* localizedCity = &(cityArray->At(iter));
		TPtrC cityName(localizedCity->Name());
		int tzId(localizedCity->TimeZoneId());

		if(-1==mTimeZoneIds.indexOf(tzId)) {
			mTimeZoneIds.append(tzId);
		}

		int cityGroupId(localizedCity->GroupId());

		if (cityGroupId != prevGroupId) {
			prevGroupId = cityGroupId;

			if (country) {
				delete country;
			}
			country = mTzLocalizer->GetCityGroupL(cityGroupId);
			countryName.Set(country->Name());
		}

		// Now insert that data into the info list.
		LocationInfo cityInfo;
		cityInfo.cityGroupId = cityGroupId;
		cityInfo.timezoneId = tzId;
		cityInfo.cityName = QString::fromUtf16(
				cityName.Ptr(), cityName.Length());
		cityInfo.countryName = QString::fromUtf16(
				countryName.Ptr(), countryName.Length());
		mAllLocations.append(cityInfo);
	}

	// Cleanup.
	CleanupStack::PopAndDestroy(cityArray);
	return mAllLocations;
}

bool TimezoneClient::getUtcDstOffsetL(int& dstOffset, const CTzId& timezoneId)
{
	RTz tzHandle;
	User::LeaveIfError(tzHandle.Connect());
	CleanupClosePushL(tzHandle);

	// Local time.
	TTime homeTime;
	homeTime.HomeTime();

	TDateTime dateTime;
	dateTime = homeTime.DateTime();

	// Get the CTzRules for the current year and for the given time zone id.
	CTzRules* timezoneRules = tzHandle.GetTimeZoneRulesL(
			timezoneId, dateTime.Year(), dateTime.Year(), ETzWallTimeReference);
	CleanupStack::PushL(timezoneRules);

	// Get the Actualised rules for the same year.
	// These are the DST rules from which we get the iNewOffset.
	CVTzActualisedRules *actualizedRules =
			CVTzActualisedRules::NewL(
					homeTime.DateTime().Year(), homeTime.DateTime().Year());
	CleanupStack::PushL(actualizedRules);
	timezoneRules->GetActualisedRulesL(*actualizedRules);

	// This way of fetching initial offset is being used rather than
	// tzRules->InitialStdTimeOffset() because in some cases,
	// the offset returned is incorrect. (For ex: Argentina and Canada/Iqaluit)
	RArray<TInt> timeZones;
	RArray<TInt> zoneOffsets;

	// Append the current timezone ID and request for the standard offset
	timeZones.Append(timezoneId.TimeZoneNumericID());
	tzHandle.GetOffsetsForTimeZoneIdsL(timeZones, zoneOffsets);

	// The initial offset or the standard offset (w/o DST)
	int initialTimeZoneOffset = zoneOffsets[ 0 ];

	// Close the resource handles
	timeZones.Close();
	zoneOffsets.Close();

	// The number of actualised rules
	int ruleCount = actualizedRules->Count();

	for (int ruleIndex(0); ruleIndex < ruleCount; ruleIndex++) {
		const TVTzActualisedRule& tVTzactRule = (*actualizedRules)[ruleIndex];

		// If the standard offset and the new offset do not match then we have
		// a dst offset. Technically if a timezone has DST then it can have a
		// max of two offsets. One is the standard which doesn't show the DST
		// usage, and the other is the DST offset which is
		// standard offset + the amount of DST.
		if (initialTimeZoneOffset != tVTzactRule.iNewOffset) {
			dstOffset = tVTzactRule.iNewOffset;
			CleanupStack::PopAndDestroy(actualizedRules);
			CleanupStack::PopAndDestroy(timezoneRules);
			CleanupStack::PopAndDestroy(&tzHandle);
			return true;
		} else {
			dstOffset = initialTimeZoneOffset;
		}
	}
	CleanupStack::PopAndDestroy(actualizedRules);
	CleanupStack::PopAndDestroy(timezoneRules);
	CleanupStack::PopAndDestroy(&tzHandle);
	return false;
}

LocationInfo TimezoneClient::getCurrentZoneInfoL()
{
	// Current zone info.
	LocationInfo currentLocation;
	int timezoneId(0);
	int utcOffset(0);

	// Connect to the timezone server.
	RTz tzHandle;
	User::LeaveIfError(tzHandle.Connect());
	CleanupClosePushL(tzHandle);

	// Get the current timezone ID.
	CTzId* tzId;
	tzId = tzHandle.GetTimeZoneIdL();
	CleanupStack::PushL(tzId);

	// TODO: Get the standard offset for current timezone.
	// This way of fetching initial offset is being used rather than
	// tzRules->InitialStdTimeOffset()because in some cases, the offset returned
	// is incorrect. (For ex: Argentina and Canada/Iqaluit)
	RArray<TInt> timeZones;
	RArray<TInt> zoneOffsets;

	// Append the current timezone ID and request for the standard offset
	timeZones.Append(tzId->TimeZoneNumericID());
	tzHandle.GetOffsetsForTimeZoneIdsL(timeZones, zoneOffsets);

	// The initial offset or the standard offset (w/o DST)
	int initialTimeZoneOffset = zoneOffsets[0];

	// Close the resource handles
	timeZones.Close();
	zoneOffsets.Close();

	// Get all the localized timezones for the current timezone ID.
	CTzLocalizedTimeZone* localizedTimeZone(NULL);

	localizedTimeZone =
			mTzLocalizer->GetLocalizedTimeZoneL(tzId->TimeZoneNumericID());

	if (localizedTimeZone) {
		CleanupStack::PushL(localizedTimeZone);

		// Get the frequently used localized city.
		CTzLocalizedCity* localizedCity(0);
		localizedCity = mTzLocalizer->GetFrequentlyUsedZoneCityL(
				CTzLocalizedTimeZone::ECurrentZone);
		CleanupStack::PushL(localizedCity);

		// Get all the city groups.
		CTzLocalizedCityGroupArray* cityGroupArray =
				mTzLocalizer->GetAllCityGroupsL(
						CTzLocalizer::ETzAlphaNameAscending);
		CleanupStack::PushL(cityGroupArray);

		// Get the index of the country corresponding to the city group ID.
		int countryIndex(1);

		for (int index = 0; index < cityGroupArray->Count(); index++) {
			if (localizedCity->GroupId() ==
					cityGroupArray->At(index).Id()) {
				countryIndex = index;
			}
		}

		// Get all the cities within the currently set country.
		CTzLocalizedCityArray* cityList = mTzLocalizer->GetCitiesInGroupL(
				(cityGroupArray->At(countryIndex )).Id(),
				CTzLocalizer::ETzAlphaNameAscending);
		CleanupStack::PushL(cityList);

		// Check if automatic time update is enabled.
		bool timeUpdateOn( false );

		// Connect to the clock server.
		// TODO: connect to the clock server and get auto update status in
		// var:  timeUpdateOn
		// Check if the country contains only one city or if automatic
		// time update is on.
		if (1 == cityList->Count() || timeUpdateOn) {
			// If yes, then display only the country name.
			// TODO
		} else {
			// Display city name.
			// TODO:
		}

		// Get the country name.
		TPtrC countryName(cityGroupArray->At(countryIndex).Name());
		currentLocation.countryName = QString::fromUtf16(
				countryName.Ptr(), countryName.Length());

		// Get the city name.
		TPtrC cityName(localizedCity->Name());
		currentLocation.cityName = QString::fromUtf16(
				cityName.Ptr(), cityName.Length());

		// Get the UTC offset.
		timezoneId = localizedCity->TimeZoneId();

		if (timezoneId) {
			// Check if the DST is on for the current timezone.
			if (isDSTOnL(timezoneId)) {
				// Get the offset with DST enabled.
				getUtcDstOffsetL(utcOffset, *tzId);

				currentLocation.dstOn = true;
				currentLocation.timezoneId = timezoneId;
				currentLocation.zoneOffset = utcOffset;
			} else {
				// Use the standard offset.
				currentLocation.dstOn = false;
				currentLocation.timezoneId = timezoneId;
				currentLocation.zoneOffset = initialTimeZoneOffset;
			}
		}

		// Cleanup.
		CleanupStack::PopAndDestroy( cityList );
		CleanupStack::PopAndDestroy( cityGroupArray );
		CleanupStack::PopAndDestroy( localizedCity );
		CleanupStack::PopAndDestroy( localizedTimeZone );
	}

	// Cleanup.
	CleanupStack::PopAndDestroy( tzId );
	CleanupStack::PopAndDestroy( &tzHandle );
	return currentLocation;
}

void TimezoneClient::setAsCurrentLocationL(LocationInfo &location)
{
/*	Debug::writeDebugMsg(
			"In time zone client setAsCurrentLocationL " + location.cityName +
			" " +
			location.countryName +
			" " +
			QString::number(location.zoneOffset));*/

	LocationInfo prevLocationInfo ;
	prevLocationInfo = getCurrentZoneInfoL();

	mTzLocalizer->SetTimeZoneL( location.timezoneId );

	TPtrC ptrCityName(
			reinterpret_cast<const TText*>(location.cityName.constData()));
	CTzLocalizedCity* localizedCity =
			mTzLocalizer->FindCityByNameL(ptrCityName, location.timezoneId);
	CleanupStack::PushL( localizedCity );
	mTzLocalizer->SetFrequentlyUsedZoneL(
			*localizedCity, CTzLocalizedTimeZone::ECurrentZone);

	// Cleanup.
	CleanupStack::PopAndDestroy( localizedCity );

	if(prevLocationInfo.timezoneId == location.timezoneId) {
		emit cityUpdated();	
	}
}

bool TimezoneClient::isDSTOnL(int timezoneId)
{
	bool returnVal( false );
	CTzId* tzId = CTzId::NewL( timezoneId );
	CleanupStack::PushL( tzId );

	RTz tzHandle;
	User::LeaveIfError( tzHandle.Connect() );
	CleanupClosePushL( tzHandle );

	returnVal = tzHandle.IsDaylightSavingOnL( *tzId );

	tzHandle.Close();
	CleanupStack::PopAndDestroy( &tzHandle );
	CleanupStack::PopAndDestroy( tzId );

	return returnVal;
}

int TimezoneClient::getStandardOffset(int timezoneId)
{
	RTz tzHandle;
	User::LeaveIfError(tzHandle.Connect());
	CleanupClosePushL(tzHandle);

	RArray<int> idArray;
	RArray<int> offsetArray;
	idArray.Append(timezoneId);

	tzHandle.GetOffsetsForTimeZoneIdsL(idArray, offsetArray);
	int stdOffset = offsetArray[0];

	offsetArray.Close();
	idArray.Close();

	// Cleanup.
	CleanupStack::PopAndDestroy(&tzHandle);

	return stdOffset;
}

void TimezoneClient::getDstRulesL(
		QDateTime &startTime, QDateTime &endTime, int timezoneId)
{
	RTz tzHandle;
	User::LeaveIfError(tzHandle.Connect());
	CleanupClosePushL(tzHandle);

	// Local time.
	TTime homeTime;
	homeTime.HomeTime();

	TDateTime dateTime;
	dateTime = homeTime.DateTime();

	CTzId *tzId = CTzId::NewL(timezoneId);
	CleanupStack::PushL(tzId);
	// Get the CTzRules for the current year and for the given time zone id.
	CTzRules* timezoneRules = tzHandle.GetTimeZoneRulesL(
			*tzId, dateTime.Year(), dateTime.Year(), ETzWallTimeReference);
	CleanupStack::PushL(timezoneRules);

	// Get the Actualised rules for the same year.
	// These are the DST rules from which we get the iNewOffset.
	CVTzActualisedRules *actualizedRules = CVTzActualisedRules::NewL(
			homeTime.DateTime().Year(), homeTime.DateTime().Year() );
	CleanupStack::PushL(actualizedRules);
	timezoneRules->GetActualisedRulesL(*actualizedRules);

	// This way of fetching initial offset is being used rather than
	// tzRules->InitialStdTimeOffset() because in some cases,
	// the offset returned is incorrect. (For ex: Argentina and Canada/Iqaluit)
	RArray<TInt> timeZones;
	RArray<TInt> zoneOffsets;

	// Append the current timezone ID and request for the standard offset
	timeZones.Append(timezoneId);
	tzHandle.GetOffsetsForTimeZoneIdsL(timeZones, zoneOffsets);

	// The initial offset or the standard offset (w/o DST)
	int initialTimeZoneOffset = zoneOffsets[ 0 ];

	// Close the resource handles
	timeZones.Close();
	zoneOffsets.Close();

	// The number of actualised rules
	int ruleCount = actualizedRules->Count();

	for(int ruleIndex(1); ruleIndex < ruleCount; ruleIndex++) {
		const TVTzActualisedRule& tVTzactRule = (*actualizedRules)[ruleIndex];

		TTime ruleTime(tVTzactRule.iTimeOfChange);

		if (initialTimeZoneOffset == tVTzactRule.iNewOffset) {
			TTime tempRuleTime(ruleTime);
			if (tVTzactRule.iTimeReference == ETzUtcTimeReference) {
				tempRuleTime += TTimeIntervalMinutes(tVTzactRule.iNewOffset);
			}

			QTime tempTime;
			QDate tempDate;
			tempTime.setHMS(tempRuleTime.DateTime().Hour(),
			                tempRuleTime.DateTime().Minute(),
			                tempRuleTime.DateTime().Second());
			tempDate.setYMD(tempRuleTime.DateTime().Year(),
			                tempRuleTime.DateTime().Month()+1,
			                tempRuleTime.DateTime().Day()+1);
			endTime.setDate(tempDate);
			endTime.setTime(tempTime);
		} else {
			TTime tempRuleTime(ruleTime);
			if (tVTzactRule.iTimeReference == ETzUtcTimeReference) {
				tempRuleTime += TTimeIntervalMinutes(tVTzactRule.iNewOffset);
			}
			QTime tempTime;
			QDate tempDate;
			tempTime.setHMS(tempRuleTime.DateTime().Hour(),
			                tempRuleTime.DateTime().Minute(),
			                tempRuleTime.DateTime().Second());
			tempDate.setYMD(tempRuleTime.DateTime().Year(),
			                tempRuleTime.DateTime().Month() + 1,
			                tempRuleTime.DateTime().Day() + 1);
			startTime.setDate(tempDate);
			startTime.setTime(tempTime);
		}
	}
	CleanupStack::PopAndDestroy(actualizedRules);
	CleanupStack::PopAndDestroy(timezoneRules);
	CleanupStack::PopAndDestroy(tzId);
	CleanupStack::PopAndDestroy(&tzHandle);
}

QList<LocationInfo> TimezoneClient::getSavedLocations()
{
	QList<LocationInfo> locationList;

	QString fileName(CITY_INFO_DB_PATH);
	fileName.append(CITY_INFO_DB);

	QFile cityInfoFile(fileName);
	if (cityInfoFile.open(QIODevice::ReadOnly)) {
		QDataStream writeStream(&cityInfoFile);
		writeStream >> locationList;
		cityInfoFile.close();
	}
	return locationList;
}

void TimezoneClient::saveLocations(const QList<LocationInfo> &locationList)
{
	QDir cityDbDir;
	cityDbDir.mkpath(CITY_INFO_DB_PATH);

	QString fileName(CITY_INFO_DB_PATH);
	fileName.append(CITY_INFO_DB);

	QFile cityInfoFile(fileName);
	if (!cityInfoFile.open(QIODevice::WriteOnly)) {
		// Error opening or creating file.
		return;
	}
	QDataStream writeStream(&cityInfoFile);
	writeStream << locationList;
	cityInfoFile.close();

	emit listUpdated();
}

void TimezoneClient::getCountries(QMap<QString, int>& countries)
{
	// Get all the city groups(countries).
	QTime t;
	t.start();
	CTzLocalizedCityGroupArray* cityGroupArray =
			mTzLocalizer->GetAllCityGroupsL(CTzLocalizer::ETzAlphaNameAscending);
	CleanupStack::PushL(cityGroupArray);

	t.restart();
	// Iterate through each of the city groups.
	for (int i = 0; i < cityGroupArray->Count(); i++) {
		CTzLocalizedCityGroup& cityGroup(cityGroupArray->At(i));
		TPtrC countryName(cityGroup.Name());

		// Get the QString of country name
		QString qCountryName = QString::fromUtf16(
				countryName.Ptr(),countryName.Length());
	    countries[qCountryName] = cityGroup.Id();
	}

	// Cleanup.
	CleanupStack::PopAndDestroy(cityGroupArray);
}

void TimezoneClient::getCitiesForCountry(int id, QMap<QString, int>& cities)
{
	// Get the city group for the given id.
	CTzLocalizedCityArray* cityArray = mTzLocalizer->GetCitiesInGroupL(id,
			CTzLocalizer::ETzAlphaNameAscending);
	CleanupStack::PushL(cityArray);
	CTzLocalizedCityArray* unsortedArray = mTzLocalizer->GetCitiesInGroupL(id,
			CTzLocalizer::ETzUnsorted);
	CleanupStack::PushL(unsortedArray);

	for (int i = 0; i < cityArray->Count(); i++) {
		CTzLocalizedCity *city = &(cityArray->At(i));
		TPtrC cityName = city->Name();

		for (int j = 0; j < unsortedArray->Count(); j++) {
			if (KErrNone ==
					(unsortedArray->At(j)).Name().Compare(city->Name())) {
				QString qCityname = QString::fromUtf16(
						cityName.Ptr(),cityName.Length());
				cities[qCityname] = j;

				break;
			}
		}
	}

	// Cleanup.
	CleanupStack::PopAndDestroy(unsortedArray);
	CleanupStack::PopAndDestroy(cityArray);
}

void TimezoneClient::getLocationInfo(
		int groupId, int cityIndex, LocationInfo& cityInfo)
{
	TRAPD(
			error,

			// Get the localized city group.
			CTzLocalizedCityGroup* cityGroup = mTzLocalizer->GetCityGroupL(groupId);
			CleanupStack::PushL(cityGroup);

			// Get the localized city array for the given city group.
			CTzLocalizedCityArray* cityArray = mTzLocalizer->GetCitiesInGroupL(
					groupId, CTzLocalizer::ETzUnsorted);
			CleanupStack::PushL(cityArray);

			// Get the required city.
			CTzLocalizedCity* neededCity = &(cityArray->At(cityIndex));

			// Fill the location information.
			cityInfo.countryName = QString::fromUtf16(
					cityGroup->Name().Ptr(), cityGroup->Name().Length());
			cityInfo.cityName = QString::fromUtf16(
					neededCity->Name().Ptr(), neededCity->Name().Length());
			cityInfo.timezoneId = neededCity->TimeZoneId();
			cityInfo.dstOn = dstOn(cityInfo.timezoneId);
			cityInfo.zoneOffset = getDstZoneOffset(cityInfo.timezoneId);

			// Cleanup.
			CleanupStack::PopAndDestroy(cityArray);
			CleanupStack::PopAndDestroy(cityGroup);
	)
	Q_UNUSED(error)
}

QDataStream &operator<<(
		QDataStream &writeStream, const LocationInfo& locationInfo)
{
	writeStream << locationInfo.cityName
			<< locationInfo.countryName
			<< locationInfo.listImageName
			<< locationInfo.dstOn
			<< locationInfo.timezoneId
			<< locationInfo.zoneOffset;
	return writeStream;
}

QDataStream &operator>>(
		QDataStream &readStream, LocationInfo &locationInfo)
{
	readStream >> locationInfo.cityName
	>> locationInfo.countryName
	>> locationInfo.listImageName
	>> locationInfo.dstOn
	>> locationInfo.timezoneId
	>> locationInfo.zoneOffset;
	return readStream;
}

int TimezoneClient::environmentCallback(TAny* obj)
{
	TimezoneClient* self = static_cast<TimezoneClient *> (obj);

	int changes = KInitialEvent;
	if (self->mNotifier) {
		changes = self->mNotifier->Change();
	}

	if (KInitialEvent <= changes) {
		// We're not concerned about handling environment changes in that range.
		return 0;
	}

	if (changes & (EChangesMidnightCrossover |
			EChangesLocale |
			EChangesSystemTime)) {
		emit self->timechanged();
	} else {
		// Nothing to do.
	}
	return 0;
}

int TimezoneClient::getDstZoneOffset(int tzId)
{
	// Connect to the timezone server.
	RTz client;
	User::LeaveIfError(client.Connect());
	CleanupClosePushL(client);

	RArray<int> zoneIds;
	RArray<int> zoneOffsets;
	zoneIds.Append(tzId);

	// Get the offsets.
	client.GetOffsetsForTimeZoneIdsL(zoneIds, zoneOffsets);

	// Cleanup.
	CleanupStack::PopAndDestroy(&client);

	return zoneOffsets[0];
}

bool TimezoneClient::dstOn(int tzId)
{
	// Connect to the timezone server.
	RTz client;
	User::LeaveIfError(client.Connect());
	CleanupClosePushL(client);

	CTzId *zoneId = CTzId::NewL(tzId);
	CleanupStack::PushL(zoneId);

	bool returnVal = client.IsDaylightSavingOnL(*zoneId);

	// Cleanup.
	CleanupStack::PopAndDestroy(zoneId);
	CleanupStack::PopAndDestroy(&client);

	return returnVal;
}

int TimezoneClient::getCityGroupIdByName(const QString& name)
{
	TPtrC namePtr;
	namePtr.Set(name.utf16(), name.length());

	// Get the citygroup matching the name.
	CTzLocalizedCityGroup *cityGroup = mTzLocalizer->FindCityGroupByNameL(namePtr);
	CleanupStack::PushL(cityGroup);

	// Get the id.
	int id = cityGroup->Id();

	// Cleanup.
	CleanupStack::PopAndDestroy(cityGroup);

	return id;
}

int TimezoneClient::getCityOffsetByNameAndId(const QString& name, int tzId)
{
	TPtrC namePtr;
	namePtr.Set(name.utf16(), name.length());

	// Get the citygroup matching the name.
	CTzLocalizedCityArray *cityArray = mTzLocalizer->GetCitiesL(tzId);
	CleanupStack::PushL(cityArray);

	int id;
	for (id = 0; id < cityArray->Count(); id++) {
		if (KErrNone == cityArray->At(id).Name().Compare(namePtr)) {
			break;
		}
	}
	if (id == cityArray->Count()) {
		id = -1;
	}

	// Cleanup.
	CleanupStack::PopAndDestroy(cityArray);

	return id;
}

void TimezoneClient::setDateTime(QDateTime dateTime)
{
	TMonth month = intToMonth(dateTime.date().month());
	TTime current(TDateTime(
			dateTime.date().year(), month, dateTime.date().day() - 1,
			dateTime.time().hour(), dateTime.time().minute(),
			dateTime.time().second(), dateTime.time().msec() * 1000));
	RTz tz;
	User::LeaveIfError(tz.Connect());
	CleanupClosePushL(tz);
	TInt ret(tz.SetHomeTime(current));
	CleanupStack::PopAndDestroy(&tz);
}

void TimezoneClient::setTimeUpdateOn(bool timeUpdate)
{
	RClkSrvInterface clkSrvInterface;
	User::LeaveIfError(clkSrvInterface.Connect());
	if (timeUpdate) {
		clkSrvInterface.ActivateAllProtocols();
	}
	else {
		clkSrvInterface.DeActivateAllProtocols();
	}
	clkSrvInterface.Close();
}

bool TimezoneClient::timeUpdateOn()
{
	TBool autoTimeUpdateOn;
	RClkSrvInterface clkSrvInterface;
	User::LeaveIfError(clkSrvInterface.Connect());
	clkSrvInterface.IsAutoTimeUpdateOn(autoTimeUpdateOn);
	clkSrvInterface.Close();
	return autoTimeUpdateOn;
}

QStandardItemModel *TimezoneClient::locationSelectorModel()
{
	if (!mWorldClockModel) {
		createWorldClockModel();
	}
	return mWorldClockModel;
}

TMonth TimezoneClient::intToMonth(int month)
{
	switch (month) {
		case 1:
			return EJanuary;
		case 2:
			return EFebruary;
		case 3:
			return EMarch;
		case 4:
			return EApril;
		case 5:
			return EMay;
		case 6:
			return EJune;
		case 7:
			return EJuly;
		case 8:
			return EAugust;
		case 9:
			return ESeptember;
		case 10:
			return EOctober;
		case 11:
			return ENovember;
		case 12:
			return EDecember;
		default:
			// Nothing to do.
			break;
	}
	return (TMonth) -1;
}

void TimezoneClient::createWorldClockModel()
{
	// Construct the model if its not yet done
	if (!mWorldClockModel) {
		// Create the model
		mWorldClockModel = new QStandardItemModel(this);

		getCountries(mAllCountries);
		mCountryCount = mAllCountries.count();

		// Construct the model in asynchronously
		QTimer::singleShot(2000, this, SLOT(populateCities()));
	}
}

void TimezoneClient::populateCities()
{
	// First iterate over all the counties
	QMapIterator<QString, int> countryIter(mAllCountries);
	while (countryIter.hasNext()) {
		countryIter.next();
		// Create an item for each country and append it to the model
		QStandardItem *country = new QStandardItem(countryIter.key());
		// Add the city group id
		// TODO: Define and use proper role
		country->setData(countryIter.value(), Qt::UserRole + 200);
		mWorldClockModel->appendRow(country);

		// Iterate through the city list of each country
		QMap<QString, int> cityList;
		getCitiesForCountry(countryIter.value(), cityList);
		QMapIterator<QString, int> cityIter(cityList);
		while (cityIter.hasNext()) {
			cityIter.next();
			// Create an item for the city and append it to that country
			QStandardItem *city = new QStandardItem(cityIter.key());
			// TODO: Define and use proper role
			city->setData(cityIter.value(), Qt::UserRole + 200);
			country->appendRow(city);
		}
	}
}

/*!
	 Checks if DST changes will be applied in the next 24 hours.

	 \param alarmInfo reference to alarm info
	 \return true if there is DST change otherwise false
 */
bool TimezoneClient::checkForDstChange(AlarmInfo& alarmInfo)
{
	// User to be notified whether DST rollover happens in a day or
	// has happen within a day if he tries to change the time.
	bool returnValue( EFalse );

	// Establish connection with RTz to get the timezone ID
	RTz tzHandle;
	User::LeaveIfError( tzHandle.Connect() );
	CleanupClosePushL( tzHandle );

	// The timezone ID (current)
	CTzId* currentTZId = tzHandle.GetTimeZoneIdL();
	CleanupStack::PushL( currentTZId );

	// The current time in UTC
	TTime currentTime;
	currentTime.UniversalTime();

	// hometime (local time)
	TTime homeTime;
	homeTime.HomeTime();

	//(Year, Month, Day, Hour, Minute, Second, Micrsecond)
	TDateTime dateTime(homeTime.DateTime().Year(), EJanuary, 1, FALSE, FALSE,
			FALSE, FALSE);

	TTime tempTime( dateTime );

	// Get the current rules for the timezone
	CTzRules* currentRules = tzHandle.GetTimeZoneRulesL(*currentTZId,
							tempTime, currentTime, ETzUtcTimeReference);
	CleanupStack::PushL( currentRules );

	// CVTzActualisedRules encapsulates the rules for a specific year.
	// Every year has a dummy rule and further DST rules if DST is applicable
	// (if Ohlson provides them)
	CVTzActualisedRules *vActualisedRules = CVTzActualisedRules::NewL(
			homeTime.DateTime().Year(),
			homeTime.DateTime().Year());
	CleanupStack::PushL( vActualisedRules );

	// The dummy rule is always the begining of the year.
	// For example there is only 1 rule for
	// India/NewDelhi but USA/Atlanta has 3 rules.
	currentRules->GetActualisedRulesL( *vActualisedRules );

	const int ruleCount( vActualisedRules->Count() );
	int ruleMatchIndex( KNoDifference );

	TTimeIntervalSeconds secondsDifference;
	TTime ruleMatchTime;

	// Fetch lowest time offset for the year residing at aTime.
	// This is used to determine if DST is on.
	for ( int ruleIndex( FALSE ); ruleIndex < ruleCount; ++ruleIndex ) {
		const TVTzActualisedRule& actualisedRule =
				( *vActualisedRules )[ ruleIndex ];

		// Only check for the same year as requested (aTime)
		if (actualisedRule.iTimeOfChange.DateTime().Year()
				== homeTime.DateTime().Year()) {
			TMonth month = intToMonth(alarmInfo.alarmDateTime.month());
			TTime alarmTime(TDateTime(
					alarmInfo.alarmDateTime.year(),
					month,
					alarmInfo.alarmDateTime.day() - 1,
					alarmInfo.origAlarmTime.hour(),
					alarmInfo.origAlarmTime.minute(),
					alarmInfo.origAlarmTime.second(),
					alarmInfo.origAlarmTime.msec()*1000));
			int tempSecDiff = actualisedRule.iTimeOfChange.DateTime().Second() -
					alarmInfo.origAlarmTime.second() ;

			alarmTime.SecondsFrom(actualisedRule.iTimeOfChange,
							secondsDifference);
			// Considering the time reference is important as America
			// (North & South) uses the Wall time (local time) reference
			// where as whole of Europe refers to time in terms of UTC time.
			// Correspondingly, the choise of local time or utc time
			// has to be made.
			TTime ruleTime;

			if ( ETzUtcTimeReference == actualisedRule.iTimeReference ) {
				ruleTime = currentTime;
			}
			else if ( ETzWallTimeReference == actualisedRule.iTimeReference ) {
				ruleTime = homeTime;
			}
			else if( ETzStdTimeReference == actualisedRule.iTimeReference ) {
				// TODO: Testing so far hasn't encountered a rule in this time reference.
				// If in case an error is found, corresponding code can be added here.
				// No support from symbian for this.
			}

			TDateTime sevenDays(FALSE, EJanuary, KDaysInWeek, FALSE, FALSE,
							FALSE, FALSE);
			TTime tempTime( sevenDays );
			TTime newTime( ruleTime.Int64() + tempTime.Int64() );

			TTimeIntervalDays temp;
			temp = newTime.DaysFrom( ruleTime );

			if ( ( secondsDifference.Int() >= KNoDifference ) &&
					( newTime > alarmTime) &&
					( actualisedRule.iTimeOfChange < alarmTime ) &&
					( ruleTime < actualisedRule.iTimeOfChange ) ) {
				// If there is a match, save the index and break.
				// We've got the rule and there's no need to continue with other rules.
				ruleMatchIndex = ruleIndex;
				ruleMatchTime = actualisedRule.iTimeOfChange;
				break;
			}
		}
	}

	if ( ruleMatchIndex > KZerothRule ) {
		// There's a match, display the information note about DST change.
		TTime displayTime;
		TTimeIntervalHours oneHour( KOneHour );
		returnValue = ETrue;
	}

	tzHandle.Close();
	CleanupStack::PopAndDestroy( vActualisedRules);
	CleanupStack::PopAndDestroy( currentRules );
	CleanupStack::PopAndDestroy( currentTZId );
	CleanupStack::PopAndDestroy( &tzHandle );

	return returnValue;
}

/*!
	Get all time zone ids

	\return list of time zone ids
 */
QList<int> TimezoneClient::getAllTimeZoneIds()
{
	if (mTimeZoneIds.count()) {
		return mTimeZoneIds;
	} else {
		// This list will contain the info of the cities fetched from tz server.
		QList<LocationInfo> infoList;

		// Get the cities, in alphabetical-ascending sorted order.
		CTzLocalizedCityArray* cityArray =
				mTzLocalizer->GetCitiesL(CTzLocalizer::ETzAlphaNameAscending);
		ASSERT(cityArray);
		CleanupStack::PushL(cityArray);
		int cityCount = cityArray->Count();

		// Now get the country\city-group of each of the city.
		// Print the timezone id and city group id as well.
		for (int iter = 0; iter < cityCount; iter++) {
			CTzLocalizedCity* localizedCity = &(cityArray->At(iter));
			int tzId(localizedCity->TimeZoneId());
			if(-1==mTimeZoneIds.indexOf(tzId)) {
				mTimeZoneIds.append(tzId);
			}
		}

		CleanupStack::PopAndDestroy(cityArray);
		return mTimeZoneIds;
	}
}

/*!
	Get all time zone ids

	\return list of time zone ids
 */
QList<int> TimezoneClient::getAllTimeZoneOffsets()
{
	QList<int> timeZoneOffsetList;
	QList<int> timeZoneIdList;
	if (mTimeZoneIds.count()) {
		timeZoneIdList = mTimeZoneIds;
	} else {
		timeZoneIdList = getAllTimeZoneIds();
	}

	RTz tzHandle;
	User::LeaveIfError( tzHandle.Connect() );
	CleanupClosePushL( tzHandle );

	RArray<int> idArray;
	RArray<int> offsetArray;
	int offset;
	int tzIdsCount = timeZoneIdList.count();
	for (int index=0;index<tzIdsCount;index++) {
		idArray.Append(timeZoneIdList.at(index));
		// Get the offsets for the time zone ids
		tzHandle.GetOffsetsForTimeZoneIdsL(idArray, offsetArray);
		offset = offsetArray[0];

		// Add the offset if not exists in the list
		if(-1==(timeZoneOffsetList.indexOf(offset))) {
			timeZoneOffsetList.append(offset);
		}

		offsetArray.Close();
		idArray.Close();
	}

	tzHandle.Close();
	CleanupStack::PopAndDestroy( &tzHandle );
	// Sort the offset list
	qSort(timeZoneOffsetList.begin(),timeZoneOffsetList.end());
	return timeZoneOffsetList;
}


/*!
	Get countries list for a UTC offset

	\param list of countries infomation having the same UTC offsets
 */
QList<LocationInfo> TimezoneClient::getCountriesForUTCOffset(int utcOffset)
{
	// This list will contain the info of the countries.
	QList<LocationInfo> countryList;
	QList<int> cityGroupIdList;

	// Get all cities for the UTC offset.
	CTzLocalizedCityArray* cityList =
			mTzLocalizer->GetCitiesWithUTCOffsetL(utcOffset);
	CleanupStack::PushL(cityList);

	int cityCount = cityList->Count();
	CTzLocalizedCityGroup* country;
	for(int index=0;index<cityCount;index++) {
		CTzLocalizedCity* city = &(cityList->At(index));
		int cityGroupId(city->GroupId());
		int timeZoneId(city->TimeZoneId());

		if(-1==cityGroupIdList.indexOf(cityGroupId)) {
			cityGroupIdList.append(cityGroupId);

			country = mTzLocalizer->GetCityGroupL(cityGroupId);

			// Now insert that data into the country List.
			LocationInfo cityInfo;
			cityInfo.cityGroupId = cityGroupId;
			cityInfo.countryName = QString::fromUtf16(
					country->Name().Ptr(), country->Name().Length());
			cityInfo.timezoneId = timeZoneId;

			countryList.append(cityInfo);
			delete country;
		}
	}
	cityGroupIdList.clear();
	CleanupStack::PopAndDestroy(cityList);
	return countryList;
}

/*!
	Add a new city to the city list using the
	name, time zone id and city group id

	\param timeZoneId time zone id of the new city
	\cityName Name of the new city
	\cityGroupId Group Id of the country
 */
LocationInfo TimezoneClient::addCity(
		int timeZoneId,QString &cityName,int cityGroupId)
{
	TPtrC namePtr;
	namePtr.Set(cityName.utf16(), cityName.length());

	// Add a new city using the localizer
	CTzLocalizedCity* newCity =
			mTzLocalizer->AddCityL(timeZoneId, namePtr, cityGroupId);
	CleanupStack::PushL(newCity);

	LocationInfo info;
	info.cityGroupId = newCity->GroupId();
	info.timezoneId = newCity->TimeZoneId();
	TPtrC newCityName(newCity->Name());
	info.cityName = QString::fromUtf16(
			newCityName.Ptr(), newCityName.Length());

	CleanupStack::PopAndDestroy(newCity);

	return info;
}

/*!
	Slot which is called when the value changes in cevrep.

	\param key The key which got changed in cenrep.
	\param value The new value of that key.
 */
void TimezoneClient::eventMonitor(const XQSettingsKey& key, const QVariant& value)
{
	if (key.uid() == KCRUidNitz && key.key() == KActiveProtocol) {
		if (mSettingsManager->error() == XQSettingsManager::NoError) {

			// New value of auto time update.
			int autoTimeUpdate = value.toInt();

			// Check if the auto time update value has actually changed.
			bool keyValueChanged = (autoTimeUpdate != mAutoTimeUpdateValue);

			if(keyValueChanged) {
				mAutoTimeUpdateValue = autoTimeUpdate;
				emit autoTimeUpdateChanged(mAutoTimeUpdateValue);
			}
		}
	}
}
// End of file	--Don't remove this.
