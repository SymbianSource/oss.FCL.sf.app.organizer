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
* Definition file for class ClockCitySelectionListPrivate.
*
*/

// System includes
#include <QStandardItemModel>
#include <QModelIndex>
#include <HbDocumentLoader>
#include <HbView>
#include <HbListView>
#include <HbListViewItem>
#include <HbMainWindow>
#include <HbInstance>
#include <HbStyleLoader>
#include <HbAction>
#include <HbSearchPanel>
#include <HbLineEdit>
#include <HbDialog>
#include <HbLabel>
#include <HbComboBox>
#include <HbExtendedLocale>
#include <HbAbstractItemView>

// User includes
#include "clockcityselectionlist_p.h"
#include "clockcityselectionlist.h"
#include "clockcitylistproxymodel.h"
#include "clockcityselectionlistcommon.h"
#include "clockcityselectionlistprototype.h"
#include "timezoneclient.h"
#include "clockdatatypes.h"

/*!
	\class ClockCitySelectionListPrivate

	This is the private implementation class for ClockCitySelectionList class.
 */

/*!
	Default constructor.

	\param client The pointer to an object of TimezoneClient.
	\param parent of type QObject.
 */
ClockCitySelectionListPrivate::ClockCitySelectionListPrivate(
		TimezoneClient *client, QObject *parent)
:QObject(parent),
 mLoader(0),
 mOwnsClient(false)
{
	// First get the q-pointer.
	q_ptr = static_cast<ClockCitySelectionList *> (parent);

	mClient = client;
	if (!mClient) {
		mClient = new TimezoneClient(this);
		mOwnsClient = true;
	}
}

/*!
	Destructor.
 */
ClockCitySelectionListPrivate::~ClockCitySelectionListPrivate()
{
	if (mOwnsClient) {
		delete mClient;
	}
	if (mLoader) {
		mLoader->reset();
		delete mLoader;
		mLoader = 0;
	}
	if (mListModel) {
		delete mListModel;
	}
	if (mTimeZoneOffsetList.count()) {
		mTimeZoneOffsetList.clear();
	}
	if(mCountryList.count()) {
		mCountryList.clear();
	}

}

/*!
	Populates the list model by getting the data from timezone client.
 */
void ClockCitySelectionListPrivate::populateListModel()
{
	// First get the data from the timezone client.
	QList<LocationInfo> infoList = mClient->getLocations();

	// Sanity check.
	if (!mListModel) {
		mListModel = new QStandardItemModel(0, 1, this);
	}
	// Clear the model before starting population.
	mListModel->removeRows(0,infoList.count()-1);
	mListModel->insertRows(0, infoList.count());
	mListModel->setColumnCount(1);

	for (int iter = 0; iter < infoList.count(); iter++) {
		QModelIndex index = mListModel->index(iter, 0);

		// Get the location info and add it to the model.
		LocationInfo info = infoList.at(iter);
		QString displayString;
		displayString += info.cityName;
		displayString += ", ";
		displayString += info.countryName;
		mListModel->setData(index, displayString, Qt::UserRole + 100);

		// Now save the timezone and city group ids.
		mListModel->setData(
				index, QVariant(info.timezoneId), Qt::UserRole + 101);
		mListModel->setData(
				index, QVariant(info.cityGroupId), Qt::UserRole + 102);
		mListModel->setData(
				index, info.cityName, Qt::UserRole + 103);
		mListModel->setData(
				index, info.countryName, Qt::UserRole + 104);
	}
}

/*!
	Slot to handle the case when a list item is activated. Here we return the
	city selected by the user. We emit a signal notifying this event and close
	the editor.
 */
void ClockCitySelectionListPrivate::handleItemActivated(
		const QModelIndex &index)
{
	LocationInfo selectedInfo;
	selectedInfo.timezoneId = index.data(Qt::UserRole + 101).value<int>();
	selectedInfo.cityGroupId = index.data(Qt::UserRole + 102).value<int>();
	selectedInfo.cityName = index.data(Qt::UserRole + 103).value<QString>();
	selectedInfo.countryName = index.data(Qt::UserRole + 104).value<QString>();
	selectedInfo.zoneOffset =
			mClient->getStandardOffset(selectedInfo.timezoneId);
	selectedInfo.dstOn = mClient->isDSTOnL(selectedInfo.timezoneId);

	// We have the information. Now emit a signal to notify clients if any.
	emit q_ptr->citySelected(selectedInfo);

	// Close the list.
	closeCityList();
}

/*!
	Slot to handle back action is pressed.
 */
void ClockCitySelectionListPrivate::handleBackAction()
{
	LocationInfo info;
	info.timezoneId = -1;

	// Test code...
	int rowCount = mListModel->rowCount();
	QStandardItem *item = mListModel->item(0);
	QString text = item->data(Qt::DisplayRole).value<QString>();
	int tz = item->data(Qt::UserRole + 100).value<int>();
	int cg = item->data(Qt::UserRole + 101).value<int>();
	QString ci = item->data(Qt::UserRole + 102).value<QString>();
	QString co = item->data(Qt::UserRole + 103).value<QString>();

	// Emit the signal to notify the closure.
	emit q_ptr->citySelected(info);

	// Close the list.
	closeCityList();
}

/*!
	closes the city selection list.
 */
void ClockCitySelectionListPrivate::closeCityList()
{
	// We are done, lets remove the view off the main window.
	HbMainWindow *window = hbInstance->allMainWindows().at(0);
	window->removeView(mView);

	if (mLoader) {
		mLoader->reset();
		delete mLoader;
		mLoader = 0;
	}

	if (mListModel) {
		delete mListModel;
	}
}

/*!
	Updates the search criteria with the proxy model.

	\param criteria The string entered by the user.
 */
void ClockCitySelectionListPrivate::updateSearchCriteria(
		const QString &criteria)
{
     int originalMask  = mListView->enabledAnimations();
     mListView->setEnabledAnimations(HbAbstractItemView::TouchDown);

	// Create the reg ex so that search is performed for match immediately
	// after word boundary.
	QString searchPattern("\\b");
	searchPattern.append(criteria);

	QRegExp searchExp;
	searchExp.setPattern(searchPattern);

	mProxyModel->setFilterRegExp(searchExp);
	mProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
}

/*!
	Handles add own city menu action
 */
void ClockCitySelectionListPrivate::handleAddOwnCityAction()
{
	if (mAddOwnCityDialog) {
		delete mAddOwnCityDialog;
	}

	// Create the dialog.
	mAddOwnCityDialog = new HbDialog;
	mAddOwnCityDialog->setTimeout(HbDialog::NoTimeout);
	mAddOwnCityDialog->setDismissPolicy(HbDialog::NoDismiss);

	// Set the heading text
	HbLabel *titlelabel = new HbLabel(hbTrId("txt_clk_opt_add_own_city"));
	mAddOwnCityDialog->setHeadingWidget(titlelabel);

	QGraphicsGridLayout *layout = new QGraphicsGridLayout();

	// Set city name label and line edit
	HbLabel *cityNameLabel = new HbLabel(hbTrId("txt_clock_formlabel_city_name"));
	layout->addItem(cityNameLabel,0,0);

	addCityNameField();
	layout->addItem(mCityNameEdit, 0, 1);

	// Set time zone label and combobox
	HbLabel *timeZoneLabel = new HbLabel(hbTrId("txt_clock_formlabel_timezone"));
	layout->addItem(timeZoneLabel, 1,0);

	addTimeZoneField();
	layout->addItem(mTimeZoneComboBox, 1,1);

	// Set the country label and combobox
	HbLabel *countryLabel = new HbLabel(hbTrId("txt_clock_formlabel_country"));
	layout->addItem(countryLabel, 2,0);

	addCountryListField();
	layout->addItem(mCountryComboBox, 2,1);

	QGraphicsWidget *widget = new QGraphicsWidget();
	widget->setLayout(layout);

	// Add actions to the dialog
	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"));
	mAddOwnCityDialog->setPrimaryAction(okAction);
	connect(
			okAction, SIGNAL(triggered()),
			this, SLOT(handleOkAction()));

	HbAction *cancelAction = new HbAction(hbTrId("txt_common_button_cancel"));
	mAddOwnCityDialog->setSecondaryAction(cancelAction);
	connect(
			cancelAction, SIGNAL(triggered()),
			this, SLOT(handleCancelAction()));

	mAddOwnCityDialog->setContentWidget(widget);
	mAddOwnCityDialog->exec();
}

/*!
	Hanldes Ok action of add own city dialog
 */
void ClockCitySelectionListPrivate::handleOkAction()
{
	QString cityName = mCityNameEdit->text();

	// Add the city if city name is not empty
	if (!cityName.isEmpty()) {
		int selectedTZOffset =
				mTimeZoneOffsetList.at(mTimeZoneComboBox->currentIndex());

		QString selectedCountry = mCountryComboBox->currentText();
		int selectedCityGroupId(-1);
		int selectedTimeZoneId(-1);
		for (int index=0;index<mCountryList.count();index++) {
			LocationInfo selectedInfo = mCountryList.at(index);
			if(!(selectedCountry.compare(selectedInfo.countryName))) {
				selectedCityGroupId = selectedInfo.cityGroupId;
				selectedTimeZoneId = selectedInfo.timezoneId;
			}
		}

		// Add the new city to the city list.
		LocationInfo addedCity =
				mClient->addCity(selectedTimeZoneId,cityName,selectedCityGroupId);

		if (!addedCity.cityName.compare(cityName)) {
			// Refresh the city list.
			populateListModel();
		}
	}

	// Close the popup
	handleCancelAction();
}

/*!
	Handles Cancel action of add own city dialog
 */
void ClockCitySelectionListPrivate::handleCancelAction()
{
	// Close the dialog.
	if (mAddOwnCityDialog) {
		mAddOwnCityDialog->close();
		mAddOwnCityDialog->deleteLater();
	}
}

/*!
	Handles the selection of time zone offset.
	Populates the country list.
 */
void ClockCitySelectionListPrivate::handleTimeZoneSelection(int index)
{
	// Get the selected time zone offset.
	int selectedTimeZoneOffset = mTimeZoneOffsetList.at(index);

	if(mCountryList.count()) {
		mCountryList.clear();
	}

	// Get country list for the offset selected
	mCountryList = mClient->getCountriesForUTCOffset(selectedTimeZoneOffset);
	int countryListCount = mCountryList.count();
	QStringList countries;
	for(int index=0;index<countryListCount;index++) {
		countries.append(mCountryList.at(index).countryName);

	qSort(countries);
	mCountryComboBox->setItems(countries);
	}
}

/*!
	Displays the city selection list.
 */
void ClockCitySelectionListPrivate::showCityList()
{
	// Construct the document loader.
	bool success = false;
	mLoader = new HbDocumentLoader;
	mLoader->load(":/xml/clockcityselectionlist.docml", &success);
	if (!success) {
		qFatal("Unable to load the document.");
	}

	// Get the selection view.
	mView = static_cast<HbView *> (mLoader->findWidget(CITYSELECTIONLISTVIEW));
	if (!mView) {
		qFatal("Unable to get the selection view.");
	}

	// Get the list view.
	mListView = static_cast<HbListView *> (mLoader->findWidget(CITYLISTVIEW));
	if (!mListView) {
		qFatal("Unable to get the list view.");
	}

	// Get the searchPanel.
	mSearchBox = static_cast<HbSearchPanel *> (mLoader->findWidget("lineEdit"));
	if (!mSearchBox) {
		qFatal("Unable to get the searchpanel.");
	}
	mSearchBox->setProgressive(true);
	mSearchBox->setSearchOptionsEnabled(false);

	connect(
			mSearchBox, SIGNAL(criteriaChanged(QString)),
			this, SLOT(updateSearchCriteria(QString)));
	
	// Construct the source model.
	if (!mListModel) {
		mListModel = new QStandardItemModel(0, 1, this);
	}
	// Construct the proxy model.
	mProxyModel = new ClockCityListProxyModel(this);
	mProxyModel->setDynamicSortFilter(true);
	mProxyModel->setSourceModel(mListModel);
	mProxyModel->setFilterRole(Qt::UserRole + 100);

	// Construct the custom list item prototype.
	ClockCitySelectionListPrototype *prototype =
			new ClockCitySelectionListPrototype;

	// Loader the custom list view layout.
	HbStyleLoader::registerFilePath(":/style/");
	mListView->setLayoutName("cityselectionlist-default");
	mListView->setItemPrototype(prototype);

	// Construct the model for the list.
	QTimer::singleShot(1, this, SLOT(populateListModel()));

	// Set the model to the list.
	mListView->setModel(mProxyModel);

	// Listen for list item activated signal.
	connect(
			mListView, SIGNAL(activated(QModelIndex)),
			this, SLOT(handleItemActivated(QModelIndex)));

	// Read the actions for menu
	mAddOwnCityAction = static_cast<HbAction *>(
			mLoader->findObject(ADDOWNCITYACTION));

	// Listen for menu triggered signal
	connect(
			mAddOwnCityAction, SIGNAL(triggered()),
			this, SLOT(handleAddOwnCityAction()));

	// Add the view to the main window and show it.
	HbMainWindow *window = hbInstance->allMainWindows().at(0);
	mBackAction = new HbAction(Hb::BackAction, this);
	mView->setNavigationAction(mBackAction);
	connect(
			mBackAction, SIGNAL(triggered()),
			this, SLOT(handleBackAction()));

	window->addView(mView);
	window->setCurrentView(mView);

}

/*!
	Get all the time zone offset texts.
 */
QStringList ClockCitySelectionListPrivate::getOffsetTexts()
{
	int offsetCount(mTimeZoneOffsetList.count());

	// Get all the time zone offsets
	if (!offsetCount) {
		mTimeZoneOffsetList = mClient->getAllTimeZoneOffsets();
		offsetCount = mTimeZoneOffsetList.count();
	}

	QStringList offsetTextList;
	int offsetvalue;
	QString offsetText;
	int hours;
	int minutes;
	HbExtendedLocale locale =  HbExtendedLocale::system();
	for(int index=0;index<offsetCount;index++) {
		offsetvalue = mTimeZoneOffsetList.at(index);
		hours = offsetvalue/60;
		minutes = offsetvalue%60;

		if (hours>0) {
			offsetText += "+";
		} else if (hours<0) {
			offsetText += "-";
		}

		offsetText += locale.format(
				QTime(qAbs(hours),qAbs(minutes),0,0),
				r_qtn_time_durat_short_with_zero);

	offsetTextList.append(offsetText);
	offsetText.clear();
	}
	return offsetTextList;
}

/*!
	Add city name edit field to add own city popup.
 */
void ClockCitySelectionListPrivate::addCityNameField()
{
	mCityNameEdit = new HbLineEdit();
	mCityNameEdit->setFocus(Qt::MouseFocusReason);
}

/*!
	Add timezone field to add own city popup.
 */
void ClockCitySelectionListPrivate::addTimeZoneField()
{
	mTimeZoneComboBox = new HbComboBox();

	QStringList texts = getOffsetTexts();
	mTimeZoneComboBox->addItems(texts);

	// Get the home city information.
	LocationInfo homeCityInfo = mClient->getCurrentZoneInfoL();
	mCurrentTZOffsetIndex = mTimeZoneOffsetList.indexOf(
			homeCityInfo.zoneOffset);
	mTimeZoneComboBox->setCurrentIndex(mCurrentTZOffsetIndex);

	connect(
			mTimeZoneComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(handleTimeZoneSelection(int)));
}

/*!
	Add country list field to add own city popup.
 */
void ClockCitySelectionListPrivate::addCountryListField()
{
	mCountryComboBox = new HbComboBox();

	// Populate the country list based on the current TZ offset selected.
	handleTimeZoneSelection(mCurrentTZOffsetIndex);
}

// End of file	--Don't remove this.
