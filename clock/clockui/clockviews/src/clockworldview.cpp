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
* Definition file for class ClockWorldView.
*
*/

// System includes
#include <QGraphicsItem>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QDebug>
#include <HbInstance>
#include <HbLabel>
#include <HbPushButton>
#include <HbAbstractViewItem>
#include <HbMenu>
#include <HbAction>
#include <HbStyleLoader>
#include <HbListView>

// User includes
#include "clockworldview.h"
#include "clockdocloader.h"
#include "clockappcontrollerif.h"
#include "clockcommon.h"
#include "settingsutility.h"
#include "timezoneclient.h"
#include "clockcityselectionlist.h"
#include "listitemprototype.h"

/*!
	\class ClockWorldView

	The world clock view of the clock application.
 */

/*!
	Constructor.

	\param parent The parent of type QGraphicsWidget.
 */
ClockWorldView::ClockWorldView(QGraphicsItem *parent)
:HbView(parent),
 mSelectedItem(-1)
{
	qDebug("clock: ClockWorldView::ClockWorldView() -->");

	// Timer for updating list data upon time change/update.
	mRefreshTimer = new QTimer();
	connect(
			mRefreshTimer, SIGNAL(timeout()),
			this, SLOT(refreshCityList()));

	// Create the model.
	mCityListModel = new QStandardItemModel();

	qDebug("clock: ClockWorldView::ClockWorldView() <--");
}

/*!
	Destructor.
 */
ClockWorldView::~ClockWorldView()
{
	qDebug("clock: ClockWorldView::~ClockWorldView() -->");

	if (mDocLoader) {
		delete mDocLoader;
	}

	qDebug("clock: ClockWorldView::~ClockWorldView() <--");
}

/*!
	Called by the ClockViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The ClockAppController object.
	\param docLoader Pointer to ClockDocLoader object.
 */
void ClockWorldView::setupView(
		ClockAppControllerIf &controllerIf,
		ClockDocLoader *docLoader)
{
	qDebug("clock: ClockWorldView::setupView() <--");

	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;

	mTimezoneClient = controllerIf.timezoneClient();
	mSettingsUtility = controllerIf.settingsUtility();

	// Establish required connections.
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateClockDisplay()));
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateDayDateInfo()));
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateCurrentLocationInfo()));
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateCurrentZoneInfo()));

	mDisplayWorldClockView = static_cast<HbAction *> (
	            mDocLoader->findObject("worldClockAction"));

	mDisplayWorldClockView->setCheckable(true);
	mDisplayWorldClockView->setChecked(true);

	connect(
	        mDisplayWorldClockView, SIGNAL(changed()),
            this, SLOT(refreshWorldView()));

	// Get the currently added locations to the list.
	mCityInfoList = mTimezoneClient->getSavedLocations();

	// Create the custom prototype.
	ListItemPrototype *customPrototype = new ListItemPrototype();
	mCityListView = qobject_cast<HbListView *> (
			mDocLoader->findWidget("worldClockCityListView"));
	HbStyleLoader::registerFilePath(":/style/");
	mCityListView->setModel(mCityListModel);
	mCityListView->setItemPrototype(customPrototype);
	mCityListView->setLayoutName("custom");

	mPlaceLabel =
			qobject_cast<HbLabel *> (mDocLoader->findWidget("placeLabel"));
//	mPlaceLabel->setTextWrapping(Hb::TextWordWrap);

	mDayDateLabel =
			qobject_cast<HbLabel *> (mDocLoader->findWidget("dateLabel"));

	// Get the toolbar/menu actions.
	mAddCityAction = static_cast<HbAction *> (
			mDocLoader->findObject("addCityAction"));
	mAddCityAction->setIcon(HbIcon(":/clock/add_new_city.svg"));
	connect(
			mAddCityAction, SIGNAL(triggered()),
			this, SLOT(handleAddLocation()));

	mShowAlarmsViewAction = static_cast<HbAction *> (
			mDocLoader->findObject("alarmsAction"));
	connect(
			mShowAlarmsViewAction, SIGNAL(triggered()),
			this, SLOT(showAlarmsView()));

	HbMainWindow *window;
	window = hbInstance->allMainWindows().first();
	updateToolbarTexts(window->orientation());

	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(updateToolbarTexts(Qt::Orientation)));

	if (mCityInfoList.count() > 0) {
		// There is atleast one city. Refresh needed.
		// Calculate after how much time the refresh has to happen.
		QTime currentTime = QDateTime::currentDateTime().time();
		int after = 60 - currentTime.second();
		mRefreshTimer->start(after * 1000);
	}

	// Update the date and day info.
	updateDayDateInfo();

	connect(
			mCityListView,
			SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
			this,
			SLOT(handleItemLongPressed(HbAbstractViewItem *, const QPointF &)));

	// Populate the list.
	int index = 0;
	// Update current location info.
	updateCurrentLocationInfo();

	for (; index < mCityInfoList.count(); index++) {
		addCityToList(mCityInfoList.at(index));
	}

	qDebug("clock: ClockWorldView::setupView() <--");
}

/*!
	Refreshes the data in the city list.
 */
void ClockWorldView::refreshCityList()
{
	int cityInfoCount = mCityInfoList.count();

	if (cityInfoCount) {
		QStandardItem *item = 0;
		for (int infoIndex = 0; infoIndex < cityInfoCount; infoIndex++) {
			item = mCityListModel->item(infoIndex);

			QDateTime dateTime = QDateTime::currentDateTime();
			dateTime = dateTime.toUTC();
			dateTime = dateTime.addSecs(
					mCityInfoList.at(infoIndex).zoneOffset * 60);

			// Display day/night indicators.
			QString dayNightIconPath = "";
			if (isDay(dateTime)) {
				dayNightIconPath = ":/clock/day";
			} else {
				dayNightIconPath = ":/clock/night";
			}
			item->setData(dayNightIconPath, Qt::UserRole + 1000);

			// Update the date info.
			QString dateInfo = dateTime.toString(
					mSettingsUtility->dateFormatString());
			if (dateTime.date() == QDate::currentDate()) {
				dateInfo = tr("Today");
			}
			item->setData(dateInfo, Qt::UserRole + 1001);

			// Set the DST icon.
			QString dstIconPath = "";
			if (mCityInfoList.at(infoIndex).dstOn) {
				dstIconPath = ":/clock/dst_icon";
			}
			item->setData(dstIconPath, Qt::UserRole + 1004);

			// Update the time info.
			QString timeInfo = dateTime.toString(
					mSettingsUtility->timeFormatString());
			item->setData(timeInfo, Qt::UserRole + 1005);
		}

		// Start the timer again for one minute.
		QTimer::singleShot(60 * 1000, this, SLOT(refreshCityList()));
	}
}

/*!
	Updates the clock display.
 */
void ClockWorldView::updateClockDisplay()
{
//	mSkinnableClock->updateDisplay(true);
}

/*!
	Updates the day-date info.
 */
void ClockWorldView::updateDayDateInfo()
{
	// Get the current datetime.
	QDateTime dateTime = QDateTime::currentDateTime();
	QString dayName = dateTime.toString("dddd");

	QString currentDate = mSettingsUtility->date();
	// Construct the day + date string.
	QString dayDateString;
	dayDateString+= dayName;
	dayDateString += " ";
	dayDateString += currentDate;

	mDayDateLabel->clear();
	mDayDateLabel->setPlainText(dayDateString);
}

/*!
	Updates the current location info.
 */
void ClockWorldView::updateCurrentLocationInfo()
{
	// Get the updated home city.
	LocationInfo homeCity = mTimezoneClient->getCurrentZoneInfoL();

	// Construct the GMT +/- X string.
	QString gmtOffset;

	int utcOffset = homeCity.zoneOffset;
	int offsetInHours (utcOffset/60);
	int offsetInMinutes (utcOffset%60);

	// Check wether the offset is +ve or -ve.
	if (0 < utcOffset) {
		// We have a positive offset. Append the '+' character.
		gmtOffset += tr(" +");
	} else if (0 > utcOffset) {
		// We have a negative offset. Append the '-' character.
		gmtOffset += tr(" -");
		offsetInHours = -offsetInHours;
	} else {
		// We dont have an offset. We are at GMT zone.
	}

	// Append the hour component.
	gmtOffset += QString::number(offsetInHours);

	// Get the time separator from settings and append it.
	QStringList timeSeparatorList;
	int index = mSettingsUtility->timeSeparator(timeSeparatorList);
	gmtOffset += timeSeparatorList.at(index);

	// Append the minute component.
	// If minute component is less less than 10, append a '00'
	if (0 <= offsetInMinutes && offsetInMinutes < 10) {
		gmtOffset += tr("00");
	} else {
		gmtOffset += QString::number(offsetInMinutes);
	}

	// Append space.
	gmtOffset += tr(" ");

	// Append GMT sting.
	gmtOffset += hbTrId("txt_common_common_gmt");

	// Append space.
	gmtOffset += tr(" ");

	// Append DST info.
	if (homeCity.dstOn) {
		gmtOffset += hbTrId("txt_common_setlabel_dst");
	}

	// Set the city and country name .
	mPlaceLabel->clear();
	if (mTimezoneClient->timeUpdateOn()) {
		mPlaceLabel->setPlainText(homeCity.countryName + tr(" ") + gmtOffset);
	} else {
		mPlaceLabel->setPlainText(
				homeCity.cityName + tr(", ") + homeCity.countryName
				+ tr(" ") + gmtOffset);
	}
}

void ClockWorldView::handleItemLongPressed(
		HbAbstractViewItem *item, const QPointF &coords)
{
	// Get the ndex of the selected item.
	mSelectedItem = item->modelIndex().row();

	// On long press we display item specific context menu.
	HbMenu *itemContextMenu = new HbMenu();

	// Add the actions to the context menu.
	mSetCurrentLocationAction = itemContextMenu->addAction(
	    hbTrId("txt_clk_menu_set_as_current_location"));
	mRemoveCityAction = itemContextMenu->addAction(
	    hbTrId("txt_clk_menu_delete"));

	connect(
			mSetCurrentLocationAction, SIGNAL(triggered()),
			this, SLOT(handleSetAsCurrentLocationAction()));
	connect(
			mRemoveCityAction, SIGNAL(triggered()),
			this, SLOT(handleDeleteAction()));

	// Show the menu.
	itemContextMenu->exec(coords);
}

/*!
	This slot is called when Add location action is triggered. It is responsible
	for launching the city selection list.
 */
void ClockWorldView::handleAddLocation()
{
	// Construct the city selection list and show the same.
	mCitySelectionList = new ClockCitySelectionList(mTimezoneClient, this);
	connect(
			mCitySelectionList, SIGNAL(citySelected(LocationInfo)),
			this, SLOT(handleCitySelected(LocationInfo)));

	// Show the city list.
	mCitySelectionList->showCityList();
}

/*!
	This slot handles delete action on the city list. It removes the item from
	the view.
 */
void ClockWorldView::handleDeleteAction()
{
	if (-1 != mSelectedItem) {
		int temp = mCityListModel->rowCount();

		QStandardItem *item = mCityListModel->takeItem(mSelectedItem);
		mCityListModel->removeRow(mSelectedItem);
		delete item;

		temp = mCityListModel->rowCount();

		mCityInfoList.removeAt(mSelectedItem);
		mSelectedItem = -1;

		// Update the data file.
		mTimezoneClient->saveLocations(mCityInfoList);

		if (mCityInfoList.count() <= 0) {
			// No need to refresh the list anymore.
			mRefreshTimer->stop();
		}
	}
}

/*!
	This slot sets the selected city as the current location.
 */
void ClockWorldView::handleSetAsCurrentLocationAction()
{
	// Check if time update is set to ON.
	// If yes, reset it to OFF and change the home location.
	if (mTimezoneClient->timeUpdateOn()) {
		mTimezoneClient->setTimeUpdateOn(false);
	} else {
		// The current location and home city should be swapped.
		// Store the info of current home city.
		LocationInfo oldHomeCity = mTimezoneClient->getCurrentZoneInfoL();
		// Get the info of the selected item.
		LocationInfo newHomeCity = mCityInfoList[mSelectedItem];

		QStandardItem *cityItem = mCityListModel->item(mSelectedItem);
		QDateTime dateTime = QDateTime::currentDateTime();

		// Display day/night indicators.
		QString dayNightIconPath = "";
		if (isDay(dateTime)) {
			dayNightIconPath = ":/clock/day";
		} else {
			dayNightIconPath = ":/clock/night";
		}
		cityItem->setData(dayNightIconPath, Qt::UserRole + 1000);

		// Date.
		QString dateString = dateTime.toString(
				mSettingsUtility->dateFormatString());
		if (dateTime.date() == QDate::currentDate()) {
			dateString = tr("Today");
			}
		cityItem->setData(dateString, Qt::UserRole + 1001);

		// The city/country name.
		QString placeInfo =
				oldHomeCity.cityName + tr(", ") + oldHomeCity.countryName;
		cityItem->setData(placeInfo, Qt::UserRole + 1002);

		// Dst icon.
		QString dstIconPath = "";
		if (oldHomeCity.dstOn) {
			dstIconPath = ":/clock/dst_icon";
		}
		cityItem->setData(dstIconPath, Qt::UserRole + 1004);

		// Time.
		QString timeString = dateTime.toString(
				mSettingsUtility->timeFormatString());
		cityItem->setData(timeString, Qt::UserRole + 1005);

		// Update the info list.
		mCityInfoList.removeAt(mSelectedItem);
		mCityInfoList.insert(mSelectedItem, oldHomeCity);
		// Update the home city with the timezone server.
		mTimezoneClient->setAsCurrentLocationL(newHomeCity);

		// Update the current location info.
		updateCurrentLocationInfo();

		// Update the offset difference in each list item.
		updateOffsetDifferences();
	}

	// Update the data file.
	mTimezoneClient->saveLocations(mCityInfoList);
	mSelectedItem = -1;
}

/*!
	Slot to handle the case when a city has been selected in from the city
	selection list.

	\param info of type LocationInfo which contains the city selected.
 */
void ClockWorldView::handleCitySelected(LocationInfo info)
{
	// Info is invalid if the timezoneId is set to -1. We don't do anything in
	// that case.
	if (-1 != info.timezoneId) {
		// Now we check if the city is already added in the list.
		bool proceed = true;
		for (int i = 0; i < mCityInfoList.count(); i++) {
			if (info.timezoneId == mCityInfoList.at(i).timezoneId) {
				proceed = false;
				break;
			}
		}

		// Check if the selected city is not the home city.
		if (info.timezoneId
				== mTimezoneClient->getCurrentZoneInfoL().timezoneId) {
			proceed = false;
		}

		if (proceed) {
			// This is new city totally. So add it.
			// Add the city information to the list
			mCityInfoList.append(info);
			QModelIndex index = addCityToList(info);
			mCityListView->scrollTo(index);
			mTimezoneClient->saveLocations(mCityInfoList);

			// Start the refresh timer if it was stopped previously.
			if (!mRefreshTimer->isActive()) {
				// Calculate after how much time the refresh has to happen.
				QTime currentTime = QDateTime::currentDateTime().time();
				int after = 60 - currentTime.second();
				mRefreshTimer->start(after * 1000);
			}
		}
	}

	// Cleanup.
	mCitySelectionList->deleteLater();
}

/*!
	Navigates to the clock alarms view.
 */

void ClockWorldView::showAlarmsView()
{
	qDebug() << "clock: ClockWorldView::showAlarmsView -->";

	mAppControllerIf->switchToView(MainView);

	qDebug() << "clock: ClockWorldView::showAlarmsView <--";
}

/*!
	Updates the offset difference shown in each item with respect to the home
	city.
 */
void ClockWorldView::updateOffsetDifferences()
{
	// Get the home city information.
	LocationInfo homeCityInfo = mTimezoneClient->getCurrentZoneInfoL();

	for (int iter = 0; iter < mCityListModel->rowCount(); iter++) {
		QModelIndex index = mCityListModel->index(iter, 0);
		LocationInfo cityInfo = mCityInfoList[iter];

		// Find out if the city being added has an offset greater than or less
		// than the homecity offset.
		QString offsetDifference;
		if (cityInfo.zoneOffset < homeCityInfo.zoneOffset) {
			offsetDifference += "-";
		} else if (cityInfo.zoneOffset > homeCityInfo.zoneOffset) {
			offsetDifference += "+";
		}
		// Now get the hours and minutes.
		int difference =
				qAbs(homeCityInfo.zoneOffset - cityInfo.zoneOffset);
		int hours = difference / 60;
		int minutes = difference % 60;
		offsetDifference += QString::number(hours);
		offsetDifference += "hrs";
		if (minutes) {
			offsetDifference += ", ";
			offsetDifference += QString::number(minutes);
			offsetDifference += "mins";
		}
		// TODO : Need to enable these code once we recieve the localisation.
		/*QString displayFormat = tr("%1hrs, %2mins");
		QString offsetString = displayFormat.arg(hours, minutes);
		offsetDifference += offsetString;*/
		mCityListModel->setData(index, offsetDifference, Qt::UserRole + 1003);
	}
}

/*!
    Slot which gets called when `World Clock' action is triggered from the view
    toolbar. This is responsible for reloading the content of the world clock view.
 */
void ClockWorldView::refreshWorldView()
{
    qDebug() << "clock: ClockWorldView::refreshWorldView -->";

	mDisplayWorldClockView->setChecked(true);

    qDebug() << "clock: ClockWorldView::refreshWorldView <--";
}

/*!
	Slot to handle orientation changes
 */
void ClockWorldView::updateToolbarTexts(Qt::Orientation currentOrientation)
{
	if (Qt::Horizontal == currentOrientation) {
		// Display toolbar item's texts
		// TODO to use text ids from ts file.
		mShowAlarmsViewAction->setText(tr("Alarms"));
		mDisplayWorldClockView->setText(tr("World clock"));
		mAddCityAction->setText(tr("Add city"));
	} else if(Qt::Vertical == currentOrientation){
		// Remove toolbar item's texts as only icons are shown.
		// TODO to use text ids from ts file.
		mShowAlarmsViewAction->setText(tr(""));
		mDisplayWorldClockView->setText(tr(""));
		mAddCityAction->setText("");
	}
}

/*!
	Adds the location info to the city list.

	\param locationInfo Details of the city to be added to the list.
 */
QModelIndex ClockWorldView::addCityToList(const LocationInfo& locationInfo)
{
	// Here we construct a model item and add it to the list model.
	QStandardItem *modelItem = new QStandardItem();

	QDateTime dateTime = QDateTime::currentDateTime();
	dateTime = dateTime.toUTC();
	dateTime = dateTime.addSecs(locationInfo.zoneOffset * 60);

	// Display day/night indicators.
	QString dayNightIconPath = "";
	if (isDay(dateTime)) {
		dayNightIconPath = ":/clock/day";
	} else {
		dayNightIconPath = ":/clock/night";
	}
	modelItem->setData(dayNightIconPath, Qt::UserRole + 1000);

	// Show the date. If date is current date then show 'today'.
	QString dateInfo = dateTime.toString(mSettingsUtility->dateFormatString());
	if (dateTime.date() == QDate::currentDate()) {
		dateInfo = tr("Today");
	}
	modelItem->setData(dateInfo, Qt::UserRole + 1001);

	// Show the city and country name.
	QString placeInfo =
			locationInfo.cityName + tr(", ") + locationInfo.countryName;
	modelItem->setData(placeInfo, Qt::UserRole + 1002);

	// Get the homecity information.
	LocationInfo homeCityInfo = mTimezoneClient->getCurrentZoneInfoL();
	// Find out if the city being added has an offset greater than or less than
	// the homecity offset.
	QString offsetDifference;
	if (locationInfo.zoneOffset < homeCityInfo.zoneOffset) {
		offsetDifference += "-";
	} else if (locationInfo.zoneOffset > homeCityInfo.zoneOffset) {
		offsetDifference += "+";
	}
	// Now get the hours and minutes.
	int difference = qAbs(homeCityInfo.zoneOffset - locationInfo.zoneOffset);
	int hours = difference / 60;
	int minutes = difference % 60;

	if ( hours && minutes ) {
		if (hours == 1) {
			QString displayFormat =
					hbTrId("txt_clock_dblist_daily_val_1_hr_2_mins");
			QString offsetString = displayFormat.arg(hours).arg(minutes);
			offsetDifference += offsetString;
		}
		else {
			QString displayFormat =
					hbTrId("txt_clock_dblist_daily_val_1_hrs_2_mins");
			QString offsetString = displayFormat.arg(hours).arg(minutes);
			offsetDifference += offsetString;
		}
	}
	else if ( hours ){
		if(hours == 1 ) {
			QString displayFormat = hbTrId("txt_clock_dblist_val_1_hr");
			QString offsetString = displayFormat.arg(hours);
			offsetDifference += offsetString;
		}
		else {
			QString displayFormat = hbTrId("txt_clock_dblist_val_1_hrs");
			QString offsetString = displayFormat.arg(hours);
			offsetDifference += offsetString;
		}
	}
	else if (minutes){
		QString displayFormat = hbTrId("txt_clock_dblist_val_1_mins");
		QString offsetString = displayFormat.arg(minutes);
		offsetDifference += offsetString;
	}

	modelItem->setData(offsetDifference, Qt::UserRole + 1003);

	// Show dst icon when needed.
	QString dstIconPath = "";
	if (locationInfo.dstOn) {
		dstIconPath = ":/clock/dst_icon";
	}
	modelItem->setData(dstIconPath, Qt::UserRole + 1004);

	// Show the time at that location.
	QString timeInfo = dateTime.toString(mSettingsUtility->timeFormatString());
	modelItem->setData(timeInfo, Qt::UserRole + 1005);

	// Add the item to the model.
	mCityListModel->appendRow(modelItem);

	return(mCityListModel->indexFromItem(modelItem));
}

/*!
	Checks if the given time is day or night.
	6:00 AM and 6:00 PM is considered as day. Otherwise night.

	\param dateTime Time for which check has to be performed.
	\return bool True if it is day otherwise false.
 */
bool ClockWorldView::isDay(QDateTime dateTime)
{
	// It is day between 6:00 AM and 6:00 PM. Otherwise night.
	if (17 < dateTime.time().hour() || 6 > dateTime.time().hour()) {
		return false;
	}
	return true;
}

// End of file-- Don't delete.
