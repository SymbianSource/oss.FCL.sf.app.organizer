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
#include <HbInstance>
#include <HbDataForm>
#include <HbAction>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbLabel>
#include <HbPushButton>
#include <HbCheckBox>
#include <HbTranslator>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <clockdomaincrkeys.h>

// User includes
#include "clocksettingsview.h"
#include "clocksettingsdefines.h"
#include "clocksettingsdocloader.h"
#include "settingsutility.h"
#include "timezoneclient.h"
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
	
	// Load the translation file and install the editor specific translator
    mTranslator = new HbTranslator("clocksettingsview");
    mTranslator->loadCommon();

	// Construct the settings utility.
	mSettingsUtility = new SettingsUtility();

	// Construct the timezone client.
	mTimezoneClient = TimezoneClient::getInstance();
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updatePlaceItem()));
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateDateItem()));
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateTimeItem()));
	connect(
			mTimezoneClient, SIGNAL(autoTimeUpdateChanged(int)),
			this, SLOT(handleAutoTimeUpdateChange(int)));
	connect(
			mTimezoneClient, SIGNAL(cityUpdated()),
			this, SLOT(updatePlaceItem()));

	// Start a timer. For updating the remaining alarm time.
	mTickTimer = new QTimer(this);
	connect(
			mTickTimer, SIGNAL(timeout()),
			this, SLOT(updateTimeItem()));

	// Create the settings manager.
	mSettingsManager = new XQSettingsManager(this);

	// Create the key for alarm snooze time.
	mAlarmSnoozeTimeKey = new XQSettingsKey(
			XQSettingsKey::TargetCentralRepository,
			KCRUidClockApp,
			KClockAppSnoozeTime);
	
	// Start the monitoring for the alarm snooze time key.
	mSettingsManager->startMonitoring(*mAlarmSnoozeTimeKey);

	// Listen to the key value changes.
	connect(
			mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
			this, SLOT(eventMonitor(XQSettingsKey, QVariant)));
}

/*!
	Destructor.
 */
ClockSettingsView::~ClockSettingsView()
{
	if (mDocLoader) {
		delete mDocLoader;
	}
	
	// Remove the translator
    if (mTranslator) {
        delete mTranslator;
        mTranslator = 0;
    }
    if(mSettingsUtility){
    	delete mSettingsUtility;
    }
	
	if(mSettingsModel){
		delete mSettingsModel;
	}
		

}

/*!
	Loads the settings view from the docml.
 */
void ClockSettingsView::loadSettingsView()
{
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
	Updates the zone info in the place item field.
 */
void ClockSettingsView::updatePlaceItem()
{
	// Get the current zone info.
	LocationInfo currentZoneInfo = mTimezoneClient->getCurrentZoneInfoL();

	if (mTimezoneClient->timeUpdateOn()) {
		mPlaceDataFormItem->setContentWidgetData(
				"text", currentZoneInfo.countryName);
	} else {
		QString placeInfo = currentZoneInfo.cityName
				+ tr(", ") + currentZoneInfo.countryName;
		mPlaceDataFormItem->setContentWidgetData("text", placeInfo);
	}
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
}

void ClockSettingsView::handleNetworkTimeStateChange(int state)
{
	bool cenrepValue = mTimezoneClient->timeUpdateOn();
	if ((Qt::Checked == state && !cenrepValue)
			|| (Qt::Unchecked == state && cenrepValue)) {
		if (Qt::Checked == state) {
			// Update the cenrep value.
			mTimezoneClient->setTimeUpdateOn(true);

		} else if (Qt::Unchecked == state) {
			// Update the cenrep value.
			mTimezoneClient->setTimeUpdateOn(false);
		}
	}
}

/*!
	Called after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param docLoader Pointer to ClockSettingsDocLoader object.
 */
void ClockSettingsView::setupView()
{
	HbMainWindow *window = hbInstance->allMainWindows().first();
	window->addView(mSettingsView);
	window->setCurrentView(mSettingsView);

	// Add the back softkey.
	mBackAction = new HbAction(Hb::BackNaviAction);
	mSettingsView->setNavigationAction(mBackAction);
	connect(
			mBackAction, SIGNAL(triggered()),
			this, SLOT(handleBackAction()));

	// Get the data form.
	mSettingsForm = static_cast<HbDataForm *> (
			mDocLoader->findWidget(CLOCK_SETTINGS_DATA_FORM));

	// Create the custom prototype.
	QList <HbAbstractViewItem*> prototypes = mSettingsForm->itemPrototypes();
	SettingsCustomItem *customPrototype = new SettingsCustomItem(mSettingsForm);
	prototypes.append(customPrototype);
	mSettingsForm->setItemPrototypes(prototypes);

	// Create the model.
	createModel();
	updatePlaceItem();

	mTickTimer->start(60000 - 1000 * QTime::currentTime().second());
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
	mNetworkTimeItem->setContentWidgetData("objectName", "networkTime");
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
	mTimeDataFormItem->setContentWidgetData("objectName", "timeItem");

	// Add the date item.
	// Custom data type for adding a time button to the data form.
	HbDataFormModelItem::DataItemType dateItemType =
			static_cast<HbDataFormModelItem::DataItemType>
			(HbDataFormModelItem::CustomItemBase + DateItem);
	mDateDataFormItem = mSettingsModel->appendDataFormItem(
			dateItemType, QString(hbTrId("txt_clock_setlabel_date")));
	mDateDataFormItem->setContentWidgetData("text", mSettingsUtility->date());
	mDateDataFormItem->setContentWidgetData("objectName", "dateItem");

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
	mPlaceDataFormItem->setContentWidgetData("objectName", "placeItem");
	
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
	mRegionalDataFormItem = mSettingsModel->appendDataFormItem(
			regionalSettingsItem);
	mRegionalDataFormItem->setContentWidgetData(
			"text", hbTrId("txt_clock_button_regional_date_time_settings"));
	mRegionalDataFormItem->setContentWidgetData(
					"objectName", "regionalSettings");
    
	// Add the clock type item.
    HbDataFormModelItem::DataItemType clockTypeSettingsItem =
            static_cast<HbDataFormModelItem::DataItemType>
            (HbDataFormModelItem::ToggleValueItem);
    mClockTypeItem = mSettingsModel->appendDataFormItem(
	clockTypeSettingsItem,hbTrId("txt_clock_setlabel_clock_type"));
    QStringList clockTypeList;
    int clockType = mSettingsUtility->clockType(clockTypeList);
	int zeroIndex(0);
    if( zeroIndex == clockType ){
	    mClockTypeItem->setContentWidgetData("text", clockTypeList[0]);
	    mClockTypeItem->setContentWidgetData("additionalText", clockTypeList[1]);
    } else {
	    mClockTypeItem->setContentWidgetData("text", clockTypeList[1]);
	    mClockTypeItem->setContentWidgetData("additionalText", clockTypeList[0]);    
    }
    mClockTypeItem->setContentWidgetData("objectName", "clockType");
    mSettingsForm->addConnection(
            mClockTypeItem, SIGNAL(clicked()),
            this, SLOT(handleClockTypeChanged()));
    
    // Add the alarm snooze time item.
	mAlarmSnoozeItem = mSettingsModel->appendDataFormItem(
			HbDataFormModelItem::ComboBoxItem,
			hbTrId("txt_clock_setlabel_alarm_snooze_time"));
	QStringList alramSnoozeTimes;
	alramSnoozeTimes << hbTrId("txt_clock_setlabel_ln_mins", 5)
			<< hbTrId("txt_clock_setlabel_ln_mins", 10)
			<< hbTrId("txt_clock_setlabel_ln_mins", 15)
			<< hbTrId("txt_clock_setlabel_ln_mins", 30);
	
	// Build the hash map for the reminder.
	mAlarmSnoozeTimeHash[0] = 5;
	mAlarmSnoozeTimeHash[1] = 10;
	mAlarmSnoozeTimeHash[2] = 15;
	mAlarmSnoozeTimeHash[3] = 30;
	
	mAlarmSnoozeItem->setContentWidgetData("items", alramSnoozeTimes);
	QVariant value = mSettingsManager->readItemValue(*mAlarmSnoozeTimeKey);
	bool success;
	int index;
	int alarmSnoozeTime = value.toInt(&success);
	if (success) {
		index = mAlarmSnoozeTimeHash.key(alarmSnoozeTime);
	} else {
		index = mAlarmSnoozeTimeHash.key(15);
	}
	mAlarmSnoozeItem->setContentWidgetData("currentIndex", index);
	mAlarmSnoozeItem->setContentWidgetData("objectName", "alarmSnooze");
	mSettingsForm->addConnection(
			mAlarmSnoozeItem, SIGNAL(currentIndexChanged(int)),
			this, SLOT(handleAlarmSnoozeTimeChanged(int)));

}

/*!
	Slot which handles the auto time update value changes in cenrep.
	
	/param value New value of the auto time update.
 */
void ClockSettingsView::handleAutoTimeUpdateChange(int value)
{
	int state = (mNetworkTimeItem->contentWidgetData("checkState")).toInt();
	
	if (value) {
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
		if (Qt::Unchecked == state) {
			mNetworkTimeItem->setContentWidgetData(
						"checkState", Qt::Checked);
		}
	} else {
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
		if (Qt::Checked == state) {
			mNetworkTimeItem->setContentWidgetData(
						"checkState", Qt::Unchecked);
		}
	}
	updatePlaceItem();
}

/*!
	Slot which handles the alarm snooze time changes.
	
	/param value New index of the alarm snooze time.
 */
void ClockSettingsView::handleAlarmSnoozeTimeChanged(int index)
{
	if (mAlarmSnoozeTimeHash.value(index)) {
		mSettingsManager->writeItemValue(
				*mAlarmSnoozeTimeKey, mAlarmSnoozeTimeHash.value(index));
	}
}

/*!
    Slot which handles the clock type change..
 */
void ClockSettingsView::handleClockTypeChanged()
{
    mSettingsUtility->setClockType(
                mClockTypeItem->contentWidgetData("text").toString());
}

/*!
	Slot which is called when the value changes in cenrep.

	\param key The key which got changed in cenrep.
	\param value The new value of that key.
 */
void ClockSettingsView::eventMonitor(
		const XQSettingsKey& key, const QVariant& value)
{
	if (key.uid() == KCRUidClockApp && key.key() == KClockAppSnoozeTime) {
		if (mSettingsManager->error() == XQSettingsManager::NoError) {

			bool success;
			int alarmSnoozeTime = value.toInt(&success);
			
			if (success) {
				mAlarmSnoozeItem->setContentWidgetData(
						"currentIndex", mAlarmSnoozeTimeHash.key(
								alarmSnoozeTime));
			}
		}
	}
}

// End of file	--Don't remove this.
