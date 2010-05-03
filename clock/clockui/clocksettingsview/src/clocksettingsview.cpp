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
* Implementation file for class ClockSettingsView.
*
*/

// System includes
#include <QDebug>
#include <HbInstance>
#include <HbDataForm>
#include <HbAction>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbLabel>
#include <HbPushButton>
#include <HbCheckBox>
#include <HbApplication>
#include <QTranslator>

// User includes
#include "clocksettingsview.h"
#include "clocksettingsdefines.h"
#include "clocksettingsdocloader.h"
#include "settingsutility.h"
#include "timezoneclient.h"
#include "skinnableclock.h"
#include "settingsdatatypes.h"
#include "settingscustomitem.h"

/*!
	\class ClockSettingsView

	This implements the clock settings which allows user to set the
	date, time, location and other related parameters.
 */

/*!
	Constructor.

	\param parent The parent of type QGraphicsWidget.
 */
ClockSettingsView::ClockSettingsView(QObject *parent)
:QObject(parent)
{
	qDebug("clock: ClockSettingsView::ClockSettingsView() -->");
	
	// Load the translation file and install the editor specific translator
    mTranslator = new QTranslator;
    //QString lang = QLocale::system().name();
    //QString path = "Z:/resource/qt/translations/";
    mTranslator->load("clocksettingsview",":/translations");
    // TODO: Load the appropriate .qm file based on locale
    //bool loaded = mTranslator->load("caleneditor_" + lang, path);
    HbApplication::instance()->installTranslator(mTranslator);

	// Construct the settings utility.
	mSettingsUtility = new SettingsUtility();

	// Construct the timezone client.
	mTimezoneClient = new TimezoneClient();
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updatePlaceInfo()));
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateDateLabel()));
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateClockWidget()));
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateDateItem()));
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateTimeItem()));

	// Start a timer. For updating the remaining alarm time.
	mTickTimer = new QTimer(this);
	connect(
			mTickTimer, SIGNAL(timeout()),
			this, SLOT(updateTimeItem()));

	qDebug("clock: ClockSettingsView::ClockSettingsView() <--");
}

/*!
	Destructor.
 */
ClockSettingsView::~ClockSettingsView()
{
	qDebug("clock: ClockSettingsView::~ClockSettingsView() -->");

	if (mDocLoader) {
		delete mDocLoader;
	}
	
	// Remove the translator
    HbApplication::instance()->removeTranslator(mTranslator);
    if (mTranslator) {
        delete mTranslator;
        mTranslator = 0;
    }

	qDebug("clock: ClockSettingsView::~ClockSettingsView() <--");
}

/*!
	Loads the settings view from the docml.
 */
void ClockSettingsView::loadSettingsView()
{
	qDebug() << "clock: ClockViewManager::loadMainView -->";

	bool loadSuccess;

	// Construct the document loader instance
	mDocLoader = new ClockSettingsDocLoader();

	// Load the application xml.
	mDocLoader->load(CLOCK_SETTINGS_VIEW_DOCML, &loadSuccess);
	
	// Load the correct section based on orientation.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	Qt::Orientation currentOrientation = window->orientation();
	
	if (Qt::Vertical == currentOrientation) {
		// Load portrait section.
		mDocLoader->load(
				CLOCK_SETTINGS_VIEW_DOCML,
				CLOCK_SETTINGS_VIEW_PORTRAIT_SECTION,
				&loadSuccess);
	} else {
		// Load landscape section.
		mDocLoader->load(
				CLOCK_SETTINGS_VIEW_DOCML,
				CLOCK_SETTINGS_VIEW_LANDSCAPE_SECTION,
				&loadSuccess);
	}

	// Connect the required signals.
	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(handleOrientationChanged(Qt::Orientation)));

	// Find the main view.
	mSettingsView = static_cast<HbView *> (
			mDocLoader->findWidget(CLOCK_SETTINGS_VIEW));

	// Setup the view.
	setupView();

	qDebug() << "clock: ClockViewManager::loadMainView <--";
}

/*!
	Slot to handle the back action of the view.
 */
void ClockSettingsView::handleBackAction()
{
	HbMainWindow *window = hbInstance->allMainWindows().first();
	window->removeView(mSettingsView);
	deleteLater();
}

/*!
	Updates the day and date in the day label.
 */
void ClockSettingsView::updateDateLabel()
{
	qDebug() << "clock: ClockSettingsView::updateDateLabel -->";

	// Get the current datetime.
	QDateTime dateTime = QDateTime::currentDateTime();
	// Get the day name.
	QString dayName = dateTime.toString("dddd");
	// Get the date in correct format.
	QString currentDate = mSettingsUtility->date();
	// Construct the day + date string.
	QString dayDateString;
	dayDateString+= dayName;
	dayDateString += " ";
	dayDateString += currentDate;

	mDayDateLabel->clear();
	mDayDateLabel->setPlainText(dayDateString);

	qDebug() << "clock: ClockSettingsView::updateDateLabel <--";
}

/*!
	Updates the zone info in the place label.
 */
void ClockSettingsView::updatePlaceInfo()
{
	qDebug() << "clock: ClockSettingsView::updateClockZoneInfo -->";

	// Get the current zone info.
	LocationInfo currentZoneInfo = mTimezoneClient->getCurrentZoneInfoL();

	// Construct the GMT +/- X string.
	QString gmtOffset;

	int utcOffset = currentZoneInfo.zoneOffset;
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

	gmtOffset += tr(" GMT ");

	// Append DST info.
	if (currentZoneInfo.dstOn) {
		gmtOffset += tr(" DST");
	}

	// Update the labels with the correct info.
	mPlaceLabel->clear();
	if (mTimezoneClient->timeUpdateOn()) {
		mPlaceLabel->setPlainText(
				currentZoneInfo.countryName + tr(" ") + gmtOffset);
		mPlaceDataFormItem->setContentWidgetData(
				"text", currentZoneInfo.countryName);
	} else {
		QString placeInfo = currentZoneInfo.cityName
				+ tr(", ") + currentZoneInfo.countryName;
		mPlaceLabel->setPlainText(placeInfo + tr(" ") + gmtOffset);
		mPlaceDataFormItem->setContentWidgetData("text", placeInfo);
	}

	qDebug() << "clock: ClockSettingsView::updateDayDateInfo <--";
}

/*!
	Updates the clock widget display.
 */
void ClockSettingsView::updateClockWidget()
{
	qDebug() << "clock: ClockSettingsView::updateClockWidget -->";

	mClockWidget->updateDisplay(true);

	qDebug() << "clock: ClockSettingsView::updateClockWidget <--";
}

/*!
	Slot to update the display of the date settings item field.
 */
void ClockSettingsView::updateDateItem()
{
	mDateDataFormItem->setContentWidgetData("text", mSettingsUtility->date());
}

/*!
	Slot to update the display of the time settings item field.
 */
void ClockSettingsView::updateTimeItem()
{
	if (!mTickTimer->isActive()) {
		mTickTimer->stop();
    }
	mTimeDataFormItem->setContentWidgetData("text", mSettingsUtility->time());

	// Start the timer again.
	mTickTimer->start(60000 - 1000 * QTime::currentTime().second());
}

/*!
	This slot is called whenever orientattion of the view changes.
 */
void ClockSettingsView::handleOrientationChanged(Qt::Orientation orientation)
{
	qDebug() << "clock: ClockSettingsView::handleOrientationChanged -->"; 

	bool success; 
	// If horizontal, load the landscape section. 
	if (Qt::Horizontal == orientation) { 
		mDocLoader->load( 
				CLOCK_SETTINGS_VIEW_DOCML,
				CLOCK_SETTINGS_VIEW_LANDSCAPE_SECTION, 
				&success); 
	} else if (Qt::Vertical == orientation) { 
		mDocLoader->load( 
				CLOCK_SETTINGS_VIEW_DOCML,
				CLOCK_SETTINGS_VIEW_PORTRAIT_SECTION,
				&success); 
	} 

	qDebug() << "clock: ClockSettingsView::handleOrientationChanged <--"; 

}

void ClockSettingsView::handleNetworkTimeStateChange(int state)
{
	bool cenrepValue = mTimezoneClient->timeUpdateOn();
	if ((Qt::Checked == state && !cenrepValue)
			|| (Qt::Unchecked == state && cenrepValue)) {
		if (Qt::Checked == state) {
			// Disable the time, date and place item.
			if (mTimeDataFormItem) {
				mTimeDataFormItem->setEnabled(false);
			}
			if (mDateDataFormItem) {
				mDateDataFormItem->setEnabled(false);
			}
			if (mPlaceDataFormItem) {
				mPlaceDataFormItem->setEnabled(false);
			}
			// Update the cenrep value.
			mTimezoneClient->setTimeUpdateOn(true);

		} else if (Qt::Unchecked == state) {
			// Enable the time, date and place item.
			if (mTimeDataFormItem) {
				mTimeDataFormItem->setEnabled(true);
			}
			if (mDateDataFormItem) {
				mDateDataFormItem->setEnabled(true);
			}
			if (mPlaceDataFormItem) {
				mPlaceDataFormItem->setEnabled(true);
			}

			// Update the cenrep value.
			mTimezoneClient->setTimeUpdateOn(false);
		}

		updatePlaceInfo();
	}
}

/*!
	Called after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param docLoader Pointer to ClockSettingsDocLoader object.
 */
void ClockSettingsView::setupView()
{
	qDebug("clock: ClockSettingsView::setupView() -->");

	HbMainWindow *window = hbInstance->allMainWindows().first();
	window->addView(mSettingsView);
	window->setCurrentView(mSettingsView);

	// Add the back softkey.
	mBackAction = new HbAction(Hb::BackAction);
	mSettingsView->setNavigationAction(mBackAction);
	connect(
			mBackAction, SIGNAL(triggered()),
			this, SLOT(handleBackAction()));

	// Get the data form.
	mSettingsForm = static_cast<HbDataForm *> (
			mDocLoader->findWidget(CLOCK_SETTINGS_DATA_FORM));

	// Get the day-date label.
	mDayDateLabel = static_cast<HbLabel *> (
			mDocLoader->findWidget(CLOCK_SETTINGS_DATE_LABEL));

	// Get the place label.
	mPlaceLabel = static_cast<HbLabel *> (
			mDocLoader->findWidget(CLOCK_SETTINGS_PLACE_LABEL));

	// Get the clock widget.
	mClockWidget = static_cast<SkinnableClock *> (
			mDocLoader->findObject(CLOCK_WIDGET));

	// Create the custom prototype.
	QList <HbAbstractViewItem*> prototypes = mSettingsForm->itemPrototypes();
	SettingsCustomItem *customPrototype = new SettingsCustomItem();
	prototypes.append(customPrototype);
	mSettingsForm->setItemPrototypes(prototypes);

	// Create the model.
	createModel();
	// Update the relevant info.
	updateDateLabel();
	updateClockWidget();
	updatePlaceInfo();

	mTickTimer->start(60000 - 1000 * QTime::currentTime().second());

	qDebug("clock: ClockSettingsView::setupView() <--");
}

/*!
	Creates the model for the settings form.
 */
void ClockSettingsView::createModel()
{
	// Remove the model.
	if (mSettingsForm->model()) {
		delete mSettingsForm->model();
		mSettingsForm->setModel(0);
	}

	// Create a model and set it.
	mSettingsModel = new HbDataFormModel();
	// Add the items to the view.
	populateModel();
	mSettingsForm->setModel(mSettingsModel);
}

/*!
	Poplulates the item in the model.
 */
void ClockSettingsView::populateModel()
{
	if (!mSettingsModel) {
		createModel();
	}

	// Add the network time update item.
	mNetworkTimeItem = mSettingsModel->appendDataFormItem(
			HbDataFormModelItem::CheckBoxItem, "");
	bool networkTime = mTimezoneClient->timeUpdateOn();
	Qt::CheckState state = Qt::Unchecked;
	if (networkTime) {
		state = Qt::Checked;
	}
	mNetworkTimeItem->setContentWidgetData(
			"checkState", state);
	mNetworkTimeItem->setContentWidgetData(
			"text", QString(hbTrId("txt_clk_setlabel_use_network_date_time")));
	mSettingsForm->addConnection(
			mNetworkTimeItem, SIGNAL(stateChanged(int)),
			this, SLOT(handleNetworkTimeStateChange(int)));

	// Add the time item.
	// Custom data type for adding a time button to the data form.
	HbDataFormModelItem::DataItemType timeItemType =
			static_cast<HbDataFormModelItem::DataItemType>
			(HbDataFormModelItem::CustomItemBase + TimeItem);
	mTimeDataFormItem = mSettingsModel->appendDataFormItem(
			timeItemType, QString(hbTrId("txt_clock_setlabel_time")));
	mTimeDataFormItem->setContentWidgetData("text", mSettingsUtility->time());

	// Add the date item.
	// Custom data type for adding a time button to the data form.
	HbDataFormModelItem::DataItemType dateItemType =
			static_cast<HbDataFormModelItem::DataItemType>
			(HbDataFormModelItem::CustomItemBase + DateItem);
	mDateDataFormItem = mSettingsModel->appendDataFormItem(
			dateItemType, QString(hbTrId("txt_clock_setlabel_date")));
	mDateDataFormItem->setContentWidgetData("text", mSettingsUtility->date());

	// Add the place item.
	HbDataFormModelItem::DataItemType placeItemType =
			static_cast<HbDataFormModelItem::DataItemType>
			(HbDataFormModelItem::CustomItemBase + PlaceItem);
	mPlaceDataFormItem = mSettingsModel->appendDataFormItem(
			placeItemType, QString(hbTrId("txt_clock_formlabel_place")));
	
	LocationInfo currentZoneInfo = mTimezoneClient->getCurrentZoneInfoL();
	QString placeItemText("");
	if (Qt::Unchecked == state) {
		placeItemText.append(currentZoneInfo.cityName);
		placeItemText+= tr(", ");
	}
	placeItemText.append(currentZoneInfo.countryName);
	mPlaceDataFormItem->setContentWidgetData("text", placeItemText);
	
	if (networkTime) {
		mTimeDataFormItem->setEnabled(false);
		mDateDataFormItem->setEnabled(false);
		mPlaceDataFormItem->setEnabled(false);
	} else {
		mTimeDataFormItem->setEnabled(true);
		mDateDataFormItem->setEnabled(true);
		mPlaceDataFormItem->setEnabled(true);
	}

	// Add the regional settings item.
	HbDataFormModelItem::DataItemType regionalSettingsItem =
			static_cast<HbDataFormModelItem::DataItemType>
			(HbDataFormModelItem::CustomItemBase + RegionalSettingsItem);
	mSettingsModel->appendDataFormItem(regionalSettingsItem);

}

// End of file	--Don't remove this.
