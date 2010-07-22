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
#include <HbInstance>
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
#include "clockhomecityitem.h"

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
	// Timer for updating list data upon time change/update.
	mRefreshTimer = new QTimer();
	connect(
			mRefreshTimer, SIGNAL(timeout()),
			this, SLOT(refreshCityList()));

	// Create the model.
	mCityListModel = new QStandardItemModel();
}

/*!
	Destructor.
 */
ClockWorldView::~ClockWorldView()
{
	if (mDocLoader) {
		delete mDocLoader;
		mDocLoader = 0;
	}
	if(mCityListModel){
		delete mCityListModel;
	}
	
	HbStyleLoader::unregisterFilePath(":/style/hblistviewitem.css");
	HbStyleLoader::unregisterFilePath(":/style/hblistviewitem.widgetml");
	HbStyleLoader::unregisterFilePath(":/style/hblistviewitem_color.css");
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
	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;

	mTimezoneClient = controllerIf.timezoneClient();
	mSettingsUtility = controllerIf.settingsUtility();

	// Establish required connections.
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateAllLocationInfo()));
	connect(
			mTimezoneClient, SIGNAL(autoTimeUpdateChanged(int)),
			this, SLOT(updateCurrentLocationInfo(int)));
	connect(
			mTimezoneClient, SIGNAL(cityUpdated()),
			this, SLOT(updateAllLocationInfo()));

	mDisplayWorldClockView = static_cast<HbAction *> (
			mDocLoader->findObject("worldClockAction"));

	mDisplayWorldClockView->setCheckable(true);
	mDisplayWorldClockView->setChecked(true);

	connect(
			mDisplayWorldClockView, SIGNAL(changed()),
			this, SLOT(refreshWorldView()));

	// Get the currently added locations to the list.
	mCityInfoList = mTimezoneClient->getSavedLocations();

	// Get the homecity widget.
	mHomeCityWidget = qobject_cast<ClockHomeCityItem *>(
			mDocLoader->findWidget(CLOCK_WORLD_HOMECITY));
	
	// Get the city list item.
	mCityListView = qobject_cast<HbListView *> (
			mDocLoader->findWidget(CLOCK_WORLD_CITYLIST));
	HbStyleLoader::registerFilePath(":/style/hblistviewitem.css");
	HbStyleLoader::registerFilePath(":/style/hblistviewitem.widgetml");
	HbStyleLoader::registerFilePath(":/style/hblistviewitem_color.css");
	mCityListView->setLayoutName("citylist-portrait");
	mCityListView->setModel(mCityListModel);
	
	// Get the toolbar/menu actions.
	mAddCityAction = static_cast<HbAction *> (
			mDocLoader->findObject("addCityAction"));
	connect(
			mAddCityAction, SIGNAL(triggered()),
			this, SLOT(handleAddLocation()));

	mShowAlarmsViewAction = static_cast<HbAction *> (
			mDocLoader->findObject("alarmsAction"));
	connect(
			mShowAlarmsViewAction, SIGNAL(triggered()),
			this, SLOT(showAlarmsView()));

	HbMainWindow *window = hbInstance->allMainWindows().first();

	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(loadSection(Qt::Orientation)));

	if (mCityInfoList.count() > 0) {
		// There is atleast one city. Refresh needed.
		// Calculate after how much time the refresh has to happen.
		QTime currentTime = QDateTime::currentDateTime().time();
		int after = 60 - currentTime.second();
		mRefreshTimer->start(after * 1000);
	}

	connect(
			mCityListView,
			SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
			this,
			SLOT(handleItemLongPressed(HbAbstractViewItem *, const QPointF &)));

	// Update current location info.
	updateCurrentLocationInfo(mTimezoneClient->timeUpdateOn());

	// Populate the list.
	for (int index = 0; index < mCityInfoList.count(); index++) {
		addCityToList(mCityInfoList.at(index));
	}
	
	// Adding the "Add city" in options menu.
	mAddCityMenuAction = new HbAction(
			QString(hbTrId("txt_clock_opt_add_city")),this);
	menu()->addAction(mAddCityMenuAction);
	mAddCityMenuAction->setVisible(true);
	connect(
			mAddCityMenuAction, SIGNAL(triggered()),
			this, SLOT(handleAddLocation()));

	// Check for disabling of addCityAction in view toolbar.
	if ((KMaximumCityListCount-1) == mCityInfoList.count()) {
		mAddCityAction->setEnabled(false);
		mAddCityMenuAction->setVisible(false);
	}
}

/*!
	Refreshes the data in the city list.
 */
void ClockWorldView::refreshCityList()
{
	updateCurrentLocationInfo(mTimezoneClient->timeUpdateOn());
	int cityInfoCount = mCityInfoList.count();

	if (cityInfoCount) {
		QStandardItem *item = 0;
		for (int infoIndex = 0; infoIndex < cityInfoCount; infoIndex++) {
			item = mCityListModel->item(infoIndex);
			item->setData(
					getCityListDisplayString(mCityInfoList.at(infoIndex)),
					Qt::DisplayRole);
			item->setData(
					getCityListDecorationString(mCityInfoList.at(infoIndex)),
					Qt::DecorationRole);
		}

		// Start the timer again for one minute.
		QTimer::singleShot(60 * 1000, this, SLOT(refreshCityList()));
	}
}

/*!
	Updates the current location info.
 */
void ClockWorldView::updateCurrentLocationInfo(int networkTime)
{
	HbMainWindow *window = hbInstance->allMainWindows().first();
	Qt::Orientation currentOrienation = window->orientation();
	loadSection(currentOrienation);

	if (!networkTime) {
		// Get the updated home city.
		LocationInfo homeCity = mTimezoneClient->getCurrentZoneInfoL();

		QMap<QString, QVariant> itemList;
		QString value;
		QDateTime dateTime = QDateTime::currentDateTime();

		// Show the date. If date is current date then show 'today'.
		QString dateInfo = dateTime.toString(
				mSettingsUtility->dateFormatString());
		itemList.insert(value.setNum(ClockHomeCityItem::Date), dateInfo);

		// Show the city and country name.
		QString placeInfo =
				homeCity.cityName + tr(", ") + homeCity.countryName;
		itemList.insert(value.setNum(ClockHomeCityItem::City), placeInfo);

		// Show the time at that location.
		QString timeInfo = dateTime.toString(
				mSettingsUtility->timeFormatString());
		itemList.insert(value.setNum(ClockHomeCityItem::Time), timeInfo);

		//Use the home city specific icons.
		// Display day/night indicators.
		QString dayNightIconPath = "";
		if (isDay(dateTime)) {
			dayNightIconPath = "qtg_large_clock_home";
		} else {
			dayNightIconPath = "qtg_large_clock_night_home";
		}
		itemList.insert(value.setNum(
				ClockHomeCityItem::DayNightIndicator), dayNightIconPath);

		// Show dst icon when needed.
		if (homeCity.dstOn) {
			QString dstIconPath = "qtg_mono_day_light_saving_time";
			itemList.insert(value.setNum(ClockHomeCityItem::Dst), dstIconPath);
		}
		mHomeCityWidget->setHomeCityItemData(itemList);
	}
}

/*!
	Handles the long press on each item in the citylist.
	Displays a list item specific context menu.

	\param item The HbAbstractViewItem that was long pressed.
	\param coords The position where mouse was pressed.
 */
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

	// Show the menu.
	itemContextMenu->open(this, SLOT(selectedMenuAction(HbAction*)));
	itemContextMenu->setPreferredPos(coords);
	itemContextMenu->setAttribute( Qt::WA_DeleteOnClose,true);
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
		QStandardItem *item = mCityListModel->takeItem(mSelectedItem);
		mCityListModel->removeRow(mSelectedItem);
		delete item;

		mCityInfoList.removeAt(mSelectedItem);
		mSelectedItem = -1;

		// Update the data file.
		mTimezoneClient->saveLocations(mCityInfoList);

		if (mCityInfoList.count() <= 0) {
			// No need to refresh the list anymore.
			mRefreshTimer->stop();
		} else if (mCityInfoList.count() < (KMaximumCityListCount-1) &&
				!mAddCityAction->isEnabled()) {
			mAddCityAction->setEnabled(true);
			mAddCityMenuAction->setVisible(true);
		}
	}
}

/*!
	This slot sets the selected city as the current location.
 */
void ClockWorldView::handleSetAsCurrentLocationAction()
{
	// Get the info of the selected item.
	LocationInfo newHomeCity = mCityInfoList[mSelectedItem];

	// Check if time update is set to ON.
	// If yes, reset it to OFF and change the home location.
	if (mTimezoneClient->timeUpdateOn()) {
		mTimezoneClient->setTimeUpdateOn(false);

		QStandardItem *item = mCityListModel->takeItem(mSelectedItem);
		mCityListModel->removeRow(mSelectedItem);
		delete item;

		mCityInfoList.removeAt(mSelectedItem);
	} else {
		// The current location and home city should be swapped.
		// Store the info of current home city.
		LocationInfo oldHomeCity = mTimezoneClient->getCurrentZoneInfoL();

		// Update the info list.
		mCityInfoList.removeAt(mSelectedItem);
		mCityInfoList.insert(mSelectedItem, oldHomeCity);

	}
	// Update the home city with the timezone server.
	mTimezoneClient->setAsCurrentLocationL(newHomeCity);

	// Update the current location info.
	updateCurrentLocationInfo(mTimezoneClient->timeUpdateOn());

	// Update the offset difference in each list item.
	refreshCityList();

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
			if (info.timezoneId == mCityInfoList.at(i).timezoneId
					&& (info.cityName == mCityInfoList.at(i).cityName)
					&& (info.countryName == mCityInfoList.at(i).countryName) ) {
				proceed = false;
				break;
			}
		}

		LocationInfo currentCity = mTimezoneClient->getCurrentZoneInfoL();
		// Check if the selected city is not the home city.
		if (
				(info.timezoneId == currentCity.timezoneId)
				&& (info.cityName == currentCity.cityName)
				&& (info.countryName == currentCity.countryName)) {
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
	if ((KMaximumCityListCount-1) == mCityInfoList.count() &&
			mAddCityAction->isEnabled()) {
		mAddCityAction->setEnabled(false);
		mAddCityMenuAction->setVisible(false);
	}
	// Cleanup.
	mCitySelectionList->deleteLater();
}

/*!
	Navigates to the clock alarms view.
 */
void ClockWorldView::showAlarmsView()
{
	mAppControllerIf->switchToView(MainView);
}

/*!
	Slot which gets called when `World Clock' action is triggered from the view
	toolbar. This is responsible for reloading the content of worldclock view.
 */
void ClockWorldView::refreshWorldView()
{
	mDisplayWorldClockView->setChecked(true);
}

/*!
	Loads the appropriate section based on window orientaion.
 */
void ClockWorldView::loadSection(Qt::Orientation orientation)
{
	bool networkTime = mTimezoneClient->timeUpdateOn();
	bool loadSuccess;
	if (Qt::Horizontal == orientation) {
		if (networkTime) {
			// Do not show home city.
			mDocLoader->load(
					CLOCK_WORLD_VIEW_DOCML,
					CLOCK_WORLD_VIEW_LANDSCAPE_NOHOMECITY_SECTION,
					&loadSuccess);
			mHomeCityWidget->hide();
		} else {
			mDocLoader->load(
					CLOCK_WORLD_VIEW_DOCML,
					CLOCK_WORLD_VIEW_LANDSCAPE_HOMECITY_SECTION,
					&loadSuccess);
			mHomeCityWidget->show();
		}
	} else if (Qt::Vertical == orientation) {
		if (networkTime) {
			// Do not show home city.
			mDocLoader->load(
					CLOCK_WORLD_VIEW_DOCML,
					CLOCK_WORLD_VIEW_PORTRAIT_NOHOMECITY_SECTION,
					&loadSuccess);
			mHomeCityWidget->hide();
		} else {
			// Show home city.
			mDocLoader->load(
					CLOCK_WORLD_VIEW_DOCML,
					CLOCK_WORLD_VIEW_PORTRAIT_HOMECITY_SECTION,
					&loadSuccess);
			mHomeCityWidget->show();
		}
	}
	mCityListView->update();
}

/*!
	Slot which gets called for timeChanged signal of timezone client.
	Refreshes both homecity & city list.
 */
void ClockWorldView::updateAllLocationInfo()
{
	updateCurrentLocationInfo(mTimezoneClient->timeUpdateOn());
	updateCityList();
	refreshCityList();
}

/*!
	Slot to handle the selected context menu actions
 */
void ClockWorldView::selectedMenuAction(HbAction *action)
{
	if (action == mSetCurrentLocationAction) {
		handleSetAsCurrentLocationAction();
	} else if (action == mRemoveCityAction) {
		handleDeleteAction();
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

	// Add the item to the model.
	mCityListModel->appendRow(modelItem);

	QModelIndex index = mCityListModel->indexFromItem(modelItem);
	mCityListModel->setData(
			index, getCityListDisplayString(locationInfo), Qt::DisplayRole);
	mCityListModel->setData(
			index, getCityListDecorationString(locationInfo),
			Qt::DecorationRole);

	return index;
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

/*!
	Returns the QVariantList for citylist to be set for DisplayRole.
	
	/param locationInfo Details of the city to be added to the list.
 */
QVariantList ClockWorldView::getCityListDisplayString(
		const LocationInfo& locationInfo)
{
	QVariantList displayString;
	QDateTime dateTime = QDateTime::currentDateTime();
	dateTime = dateTime.toUTC();
	dateTime = dateTime.addSecs(locationInfo.zoneOffset * 60);

	// Show the date. If date is current date then show 'today'.
	QString dateInfo = dateTime.toString(mSettingsUtility->dateFormatString());
	if (dateTime.date() == QDate::currentDate()) {
		dateInfo = hbTrId("txt_clock_main_view_dblist_daily_val_today");
	}
	displayString.append(dateInfo);

	// Show the city and country name.
	QString placeInfo =
			locationInfo.cityName + tr(", ") + locationInfo.countryName;
	displayString.append(placeInfo);

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
	QString displayFormat("");
	QString offsetString("");

	if ( hours && minutes ) {
		if (hours == 1) {
			displayFormat =
					hbTrId("txt_clock_dblist_daily_val_1_hr_2_mins");
			offsetString = displayFormat.arg(hours).arg(minutes);
			offsetDifference += offsetString;
		}
		else {
			displayFormat =
					hbTrId("txt_clock_dblist_daily_val_1_hrs_2_mins");
			offsetString = displayFormat.arg(hours).arg(minutes);
			offsetDifference += offsetString;
		}
	}
	else if ( hours ){
		if(hours == 1 ) {
			displayFormat = hbTrId("txt_clock_dblist_val_1_hr");
			offsetString = displayFormat.arg(hours);
			offsetDifference += offsetString;
		}
		else {
			displayFormat = hbTrId("txt_clock_dblist_val_1_hrs");
			offsetString = displayFormat.arg(hours);
			offsetDifference += offsetString;
		}
	}
	else if (minutes){
		displayFormat = hbTrId("txt_clock_dblist_val_1_mins");
		offsetString = displayFormat.arg(minutes);
		offsetDifference += offsetString;
	} else {
		displayFormat = hbTrId("txt_clock_dblist_val_1_hrs");
		offsetString = displayFormat.arg(0);
		offsetDifference += offsetString;
	}
	displayString.append(offsetDifference);

	// Show the time at that location.
	QString timeInfo = dateTime.toString(mSettingsUtility->timeFormatString());
	displayString.append(timeInfo);

	return displayString;
}

/*!
	Returns the QVariantList for citylist to be set for DecorationRole.

	/param locationInfo Details of the city to be added to the list.
 */
QVariantList ClockWorldView::getCityListDecorationString(
		const LocationInfo& locationInfo)
{
	QVariantList decorationString;
	QDateTime dateTime = QDateTime::currentDateTime();
	dateTime = dateTime.toUTC();
	dateTime = dateTime.addSecs(locationInfo.zoneOffset * 60);

	// Display day/night indicators.
	// TODO: change the icon name for night when available.
	QString dayNightIconPath = "";
	if (isDay(dateTime)) {
		dayNightIconPath = "qtg_large_clock";
	} else {
		dayNightIconPath = "qtg_large_clock_night";
	}
	decorationString.append(HbIcon(dayNightIconPath));

	// Show dst icon when needed.
	if (locationInfo.dstOn) {
		QString dstIconPath = "qtg_mono_day_light_saving_time";
		decorationString.append(HbIcon(dstIconPath));
	}
	return decorationString;
	
}

/*!
	Updates the city list according to the home city.
 */
void ClockWorldView::updateCityList()
{
	int cityInfoCount = mCityInfoList.count();

	if (cityInfoCount) {
		bool deletion = false;
		int index;
		LocationInfo currentCity = mTimezoneClient->getCurrentZoneInfoL();
		for (index = 0; index < cityInfoCount; index++) {
			if (currentCity.timezoneId == mCityInfoList.at(index).timezoneId
					&& (currentCity.cityName ==
							mCityInfoList.at(index).cityName)
					&& (currentCity.countryName ==
							mCityInfoList.at(index).countryName) ) {
				deletion = true;
				break;
			}
		}
		if (deletion) {
			QStandardItem *item = mCityListModel->takeItem(index);
			mCityListModel->removeRow(index);
			delete item;

			mCityInfoList.removeAt(index);
			index = -1;

			// Update the data file.
			mTimezoneClient->saveLocations(mCityInfoList);

			if (mCityInfoList.count() <= 0) {
				// No need to refresh the list anymore.
				mRefreshTimer->stop();
			} else if (mCityInfoList.count() < (KMaximumCityListCount-1) &&
					!mAddCityAction->isEnabled()) {
				mAddCityAction->setEnabled(true);
				mAddCityMenuAction->setVisible(true);
			}
			
		}
	}
}

// End of file-- Don't delete.
