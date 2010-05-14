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
 * Description: FTU wizard plugin view implementaion
 *
 */

// System includes
#include <QObject>
#include <QDate>
#include <QTime>
#include <qabstractitemmodel.h>
#include <hbdataform.h>
#include <hbview.h>
#include <hbdataformmodel.h>
#include <hbpushbutton.h>
#include <hbdataformviewitem.h>
#include <hbdatetimepicker.h>
#include <hbaction.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>

// User includes
#include "settingsutility.h"
#include "timezoneclient.h"
#include "ftudatetimeview.h"
#include "ftudatetimecustomitem.h"
#include "ftudatetimeprivatecrkeys.h"

/*!
    \class FtuDateTimeView
    This is the view class for the FTU plugin
 */
/*!
    \enum FtuDateTimeView::dateTimePicker
    This enum defines different pickers to be launched(date picker/time picker)
 */
/*!
    \var FtuDateTimeView::datePicker
    Date picker.
 */
/*!
    \var FtuDateTimeView::timePicker
    Time picker.
 */

/*!
    Constructor.
 */
FtuDateTimeView::FtuDateTimeView() :
HbView(),
mDatePicker(NULL),
mTimePicker(NULL)
{
	//TODO: Localisation has to be done
	// set title for wizard
	setTitle("Date and Time");
	mDateTimePlaceForm = new HbDataForm();

	QList <HbAbstractViewItem*> prototypes = mDateTimePlaceForm->itemPrototypes();
	FtuDateTimeCustomItem *customItem =
			new FtuDateTimeCustomItem(mDateTimePlaceForm);
	prototypes.append(customItem);
	mDateTimePlaceForm->setItemPrototypes(prototypes);

	// Construct the settings utility.
	mSettingsUtility = new SettingsUtility();
	mTimeZoneClient = TimezoneClient::getInstance();
	mTimeAutoUpdate = mTimeZoneClient->timeUpdateOn();
}

/*!
    Destructor.
 */
FtuDateTimeView::~FtuDateTimeView()
{
	delete mDateTimePlaceForm;
	delete mDateTimePlaceModel;
	delete mSettingsUtility;

	if (!mTimeZoneClient->isNull()) {
		mTimeZoneClient->deleteInstance();
	}
}
/*!
    Creates the main view.
 */
void FtuDateTimeView::constructView()
{
	createMainLayout();
	setWidget(mDateTimePlaceForm);
}

/*!
    Implemantation to create the main layout using the dataform.
 */
void FtuDateTimeView::createMainLayout()
{
	if (mDateTimePlaceForm->model()) {
		delete mDateTimePlaceForm->model();
		mDateTimePlaceForm->setModel(0);
	}

	mDateTimePlaceModel = new HbDataFormModel();

	// Populates the datetime and place groups
	populateDateTimeGroup();
	populatePlaceGroup();

	setItemDisplayed();
	mDateTimePlaceForm->setModel(mDateTimePlaceModel);
}

/*!
    Populates the Date and Time Group.
 */
void FtuDateTimeView::populateDateTimeGroup()
{
	HbDataFormModelItem *dateTimeGroup =
			mDateTimePlaceModel->appendDataFormGroup
			(QString(tr("Date and Time")),
			 mDateTimePlaceModel->invisibleRootItem());

	//Custom Date item
	mDateItem = mDateTimePlaceModel->appendDataFormItem(
								HbDataFormModelItem::CustomItemBase,
								QString(tr("Date")), dateTimeGroup);
	// Custom Time item
	mTimeItem = mDateTimePlaceModel->appendDataFormItem(
								HbDataFormModelItem::CustomItemBase,
								QString(tr("Time")), dateTimeGroup);
	// Auto TimeUpdate item
	mAutoTimeUpdateItem = mDateTimePlaceModel->appendDataFormItem(
								HbDataFormModelItem::CustomItemBase,
								QString(tr("Time Autoupdate")), dateTimeGroup);

	// Connect the items to the proper slots
	mDateTimePlaceForm->addConnection(mDateItem, SIGNAL(clicked()), this,
								SLOT(populateDatePicker()));
	mDateTimePlaceForm->addConnection(mTimeItem, SIGNAL(clicked()), this,
								SLOT(populateTimePicker()));
	mDateTimePlaceForm->addConnection(mAutoTimeUpdateItem, SIGNAL(clicked()),
								this, SLOT(setAutoTimeupDate()));
}

/*!
    Populates the Place Group.
 */
void FtuDateTimeView::populatePlaceGroup()
{
	mPlaceGroup = mDateTimePlaceModel->appendDataFormGroup(QString(tr("Place")),
									mDateTimePlaceModel->invisibleRootItem());
	// Custom country item
	mCountryItem = mDateTimePlaceModel->appendDataFormItem(
									HbDataFormModelItem::CustomItemBase,
									QString(tr("Country")), mPlaceGroup);
	// Custom city item
	mCityItem = mDateTimePlaceModel->appendDataFormItem(
									HbDataFormModelItem::CustomItemBase,
									QString(tr("City")), mPlaceGroup);

	// Connect the items to the proper slots
	mDateTimePlaceForm->addConnection(mCountryItem, SIGNAL(clicked()), this,
									SLOT(populateCitySelectionList()));
	mDateTimePlaceForm->addConnection(mCityItem, SIGNAL(clicked()), this,
									SLOT(populateCitySelectionList()));


}

/*!
    Sets the item index.
 */
void FtuDateTimeView::setItemDisplayed()
{
	// Display the items with proper data
	mDateItem->setContentWidgetData("text", mSettingsUtility->date());
	mTimeItem->setContentWidgetData("text", mSettingsUtility->time());

	if (mTimeAutoUpdate) {
		mAutoTimeUpdateItem->setContentWidgetData("text", tr("ON"));
	} else {
		mAutoTimeUpdateItem->setContentWidgetData("text", tr("OFF"));
	}
	mCountryItem->setContentWidgetData("text",
							mTimeZoneClient->getCurrentZoneInfoL().countryName);
	mCityItem->setContentWidgetData("text",
							mTimeZoneClient->getCurrentZoneInfoL().cityName);

	// Set the date,time,country and city fields disable
	// if auto time update is ON
	if (mTimeAutoUpdate) {
		mDateItem->setEnabled(false);
		mTimeItem->setEnabled(false);
		mCountryItem->setEnabled(false);
		mCityItem->setEnabled(false);
	}
}

/*!
    Populates the Date Picker.
 */
void FtuDateTimeView::populateDatePicker()
{
	if (mDatePickerDialog) {
		delete mDatePickerDialog;
	}
	mDatePickerDialog = new HbDialog();

	mDatePickerDialog->setDismissPolicy(HbDialog::NoDismiss);
	mDatePickerDialog->setTimeout(HbDialog::NoDismiss);

	if(mDatePicker) {
		mDatePicker = NULL;
	}
	mDatePicker = new HbDateTimePicker(QDate::currentDate(),
	                                   this);
	mDatePicker->setMinimumDate(QDate::fromString("01/01/1900", "dd/MM/yyyy"));
	mDatePicker->setMaximumDate(QDate::fromString("31/12/2100", "dd/MM/yyyy"));

	mDatePickerDialog->setContentWidget(mDatePicker);

	// Add ok and cancel actions.
	mOkAction = new HbAction(
			hbTrId("txt_common_button_ok"), mDatePickerDialog);
	mCancelAction =  new HbAction(
			hbTrId("txt_common_button_cancel"), mDatePickerDialog);

	mDatePickerDialog->addAction(mOkAction);
	mDatePickerDialog->addAction(mCancelAction);

	mDatePickerDialog->open(this, SLOT(selectedAction(HbAction *)));
}

/*!
    Populates the Time Picker.
 */
void FtuDateTimeView::populateTimePicker()
{
	if (mTimePickerDialog) {
		delete mTimePickerDialog;
	}
	mTimePickerDialog = new HbDialog();
	mTimePickerDialog->setDismissPolicy(HbDialog::NoDismiss);
	mTimePickerDialog->setTimeout(HbDialog::NoDismiss);

	if(mTimePicker) {
		mTimePicker = NULL;
	}
	mTimePicker = new HbDateTimePicker(
			QTime().currentTime(), this);
	mTimePicker->setDisplayFormat(mSettingsUtility->timeFormatString());

	mTimePickerDialog->setContentWidget(mTimePicker);

	// Add ok and cancel actions.
	mOkAction = new HbAction(
			hbTrId("txt_common_button_ok"), mTimePickerDialog);

	mCancelAction = new HbAction(
			hbTrId("txt_common_button_cancel"), mTimePickerDialog);

	mTimePickerDialog->addAction(mOkAction);
	mTimePickerDialog->addAction(mCancelAction);

	mTimePickerDialog->open(this, SLOT(selectedAction(HbAction*)));
}

/*!
    Sets the Auto Time update.
 */
void FtuDateTimeView::setAutoTimeupDate()
{
	if (mTimeAutoUpdate) {
		mAutoTimeUpdateItem->setContentWidgetData("text", "OFF");
		// Set the fields enabled if auto time update is OFF
		mDateItem->setEnabled(true);
		mTimeItem->setEnabled(true);
		mCountryItem->setEnabled(true);
		mCityItem->setEnabled(true);
		// SetAutomaticTimeUpdate OFF, UnLoad the Plugins
		setAutomaticTimeUpdateOff(false);
	} else {
		mAutoTimeUpdateItem->setContentWidgetData("text", "ON");
		// Set the fields disabled if auto time update is ON
		mDateItem->setEnabled(false);
		mTimeItem->setEnabled(false);
		mCountryItem->setEnabled(false);
		mCityItem->setEnabled(false);
		// SetAutomaticTimeUpdate ON, Load the Plugins
		setAutomaticTimeUpdateOff(true);
	}
	wizardEditedDate(QDate::currentDate());
}

/*!
    Populates the City Selection List.
 */
void FtuDateTimeView::populateCitySelectionList()
{
	if(mCitySelectionList) {
		mCitySelectionList = NULL;
	}
	mCitySelectionList = new ClockCitySelectionList(mTimeZoneClient, this);
	connect(mCitySelectionList,SIGNAL(citySelected(LocationInfo)),
	        SLOT(HandleLocationChange(LocationInfo)));
	mCitySelectionList->showCityList();
}

/*!
	Slot to handle the case when a city has been selected from the city.
	\param info of type LocationInfo which contains the city selected.
 */
void FtuDateTimeView::HandleLocationChange(LocationInfo location)
{
	// Check if the location is valid. If its not valid the timezoneId will be -1
	if(location.timezoneId != -1) {
		// Set the location
		mTimeZoneClient->setAsCurrentLocationL(location);
		mCountryItem->setContentWidgetData("text", location.countryName);
		mCityItem->setContentWidgetData("text", location.cityName);
	}
	// Cleanup
	mCitySelectionList->deleteLater();
}

/*!
	Slot to handle the selected action.
 */
void FtuDateTimeView::selectedAction(HbAction *action)
{
	// Update time/date based on the picker selected.
	if (action == mOkAction) {
		if (mTimePickerDialog) {
			updateTime();
		} else if(mDatePickerDialog) {
			updateDate();
		}
	}else {
		if(mTimePickerDialog) {
			mTimePickerDialog->deleteLater();
		} else if(mDatePickerDialog) {
			mDatePickerDialog->deleteLater();
		}
	}
}

/*!
    Sets the device date.
 */
void FtuDateTimeView::updateDate()
{
	QDate date = mDatePicker->date();
	// Set device Date
	if (date.isValid()) {
		mDateItem->setContentWidgetData("text",
						date.toString(mSettingsUtility->dateFormatString()));
		mTimeZoneClient->setDateTime(QDateTime(date, QTime::currentTime()));
		wizardEditedDate(date);
	}
}

/*!
    Sets the device time.
 */
void FtuDateTimeView::updateTime()
{
	QTime time = mTimePicker->time();
	// Set device Time
	if (time.isValid()) {
		mTimeItem->setContentWidgetData("text",
						time.toString(mSettingsUtility->timeFormatString()));
		mTimeZoneClient->setDateTime(QDateTime(QDate::currentDate(), time));
		wizardEditedDate(QDate::currentDate());
	}

}

/*!
    Gets the wizard completed status.
 */
QDate FtuDateTimeView::getWizardCompletedDate()
{
	XQSettingsManager *settingsManager = new XQSettingsManager();
	XQSettingsKey *ftuPluginDateCenrepKey =
			new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
			                  KCRUidClockApp, KFtuPluginDate);
	// Read the initial values from the cenrep
	QString dateString = settingsManager->readItemValue(*ftuPluginDateCenrepKey,
	                                                    XQSettingsManager::TypeString).toString();
	QDate completedDate = QDate::fromString(dateString,
	                                        mSettingsUtility->dateFormatString());

	// Cleanup.
	delete ftuPluginDateCenrepKey;
	delete settingsManager;

	return completedDate;
}

/*!
    Gets the wizard Edited date.
 */
void FtuDateTimeView::wizardEditedDate(const QDate &date)
{
	XQSettingsManager *settingsManager = new XQSettingsManager();
	XQSettingsKey *ftuPluginDateCenrepKey =
			new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
			                  KCRUidClockApp, KFtuPluginDate);
	QString dateString = date.toString(mSettingsUtility->dateFormatString());
	settingsManager->writeItemValue(*ftuPluginDateCenrepKey,dateString);

	// Cleanup.
	delete ftuPluginDateCenrepKey;
	delete settingsManager;
}

/*!
    To set AutomaticTimeUpdate
 */
void FtuDateTimeView::setAutomaticTimeUpdateOff(bool value)
{
	mTimeZoneClient->setTimeUpdateOn(value);
	mTimeAutoUpdate = value;
}
// End of file  --Don't remove this.
