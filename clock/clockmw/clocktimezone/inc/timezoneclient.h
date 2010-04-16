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
* Description:
*
*/

// timezoneclient.h
#ifndef __TIMEZONECLIENT_H__
#define __TIMEZONECLIENT_H__

// System includes
#include <qglobal.h>
#include <QtGui>
#include <QList>
#include <QDateTime>
#include <QMap>

#include <tz.h>
#include <bacntf.h>

// User includes
#include "clockdatatypes.h"

#ifdef  TIMEZONECLIENT_LIBRARY
#define TIMEZONECLIENT_EXPORT Q_DECL_EXPORT
#else
#define TIMEZONECLIENT_EXPORT Q_DECL_IMPORT
#endif

class CEnvironmentChangeNotifier;
class QStandardItemModel;
class CTzLocalizer;

const int KInitialEvent = (
		EChangesLocale |
		EChangesMidnightCrossover |
		EChangesThreadDeath |
		EChangesPowerStatus |
		EChangesSystemTime |
		EChangesFreeMemory |
		EChangesOutOfMemory);

class TIMEZONECLIENT_EXPORT TimezoneClient : public QObject, public CBase
{
	Q_OBJECT

public:
	TimezoneClient(QObject* parent = 0);
	~TimezoneClient();

public:
	QList<LocationInfo> getLocations();
	bool getUtcDstOffsetL(int &dstOffset,
						  const CTzId &tzId);
	LocationInfo getCurrentZoneInfoL();
	void setAsCurrentLocationL(LocationInfo &location);
	bool isDSTOnL(int timezoneId);
	int getStandardOffset(int timezoneId);
	void getDstRulesL(
			QDateTime &startTime, QDateTime &endTime,int timezoneId);
	QList<LocationInfo> getSavedLocations();
	void saveLocations(const QList<LocationInfo> &locationList);
	void getCountries(QMap<QString, int>& countries);
	void getCitiesForCountry(
			int id, QMap<QString, int>& cities);
	void getLocationInfo(
			int groupId, int cityIndex, LocationInfo& cityInfo);
	bool dstOn(int tzId);
	int getCityGroupIdByName(const QString& name);
	int getCityOffsetByNameAndId(const QString& name, int tzId);
	void setDateTime(QDateTime dateTime);
	void setTimeUpdateOn(bool timeUpdate = true);
	bool timeUpdateOn();
	QStandardItemModel *locationSelectorModel();
	void createWorldClockModel();
	bool checkForDstChange(AlarmInfo& alarmInfo);
	QList<int> getAllTimeZoneIds();
	QList<int> getAllTimeZoneOffsets();
	QList<LocationInfo> getCountriesForUTCOffset(int utcOffset);
	LocationInfo addCity(int timeZoneId, QString &cityName, int cityGroupId);

public:
	static int environmentCallback(TAny* obj);

signals:
	void timechanged();
	void listUpdated();

private:
	int getDstZoneOffset(int tzId);
	TMonth intToMonth(int month);

private slots:
    void populateCities();

public:
	CEnvironmentChangeNotifier *mNotifier;

private:
	CTzLocalizer *mTzLocalizer;
	QMap<QString, int> mAllCountries;
	QList<LocationInfo> mAllLocations;
	QStandardItemModel *mWorldClockModel;

	QList<int> mTimeZoneIds;

	bool mTimeUpdateOn;
	int mFetchCount;
	int mCountryCount;
};

#endif      // __TIMEZONECLIENT_H__

// End of file
