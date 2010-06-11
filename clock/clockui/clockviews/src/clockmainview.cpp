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
* Definition file for class ClockMainView.
*
*/

// System includes
#include <QGraphicsItem>
#include <HbInstance>
#include <HbLabel>
#include <HbAbstractViewItem>
#include <HbMenu>
#include <HbAction>
#include <HbListView>
#include <HbNotificationDialog>
#include <HbStyleLoader>

// User includes
#include "clockmainview.h"
#include "clockdocloader.h"
#include "clockappcontrollerif.h"
#include "alarmclient.h"
#include "clocksettingsview.h"
#include "clockcommon.h"
#include "clockalarmeditor.h"
#include "settingsutility.h"
#include "timezoneclient.h"
#include "clockalarmlistitemprototype.h"
#include "clockalarmlistmodel.h"
#include "clockwidget.h"

/*!
	\class ClockMainView

	The main view of the clock application.
 */

/*!
	Constructor.

	\param parent The parent of type QGraphicsWidget.
 */
ClockMainView::ClockMainView(QGraphicsItem *parent)
:HbView(parent),
 mAlarmList(0),
 mSelectedItem(-1),
 mIsLongTop(false)
{
	// Nothing yet.
}

/*!
	Destructor.
 */
ClockMainView::~ClockMainView()
{
	if (mDocLoader) {
		delete mDocLoader;
		mDocLoader = 0;
	}
	if (mAlarmListModel) {
		delete mAlarmListModel;
		mAlarmListModel = 0;
	}
	HbStyleLoader::unregisterFilePath(
			":/style/clockalarmlistitemprototype.css");
	HbStyleLoader::unregisterFilePath(
			":/style/clockalarmlistitemprototype.widgetml");
	HbStyleLoader::unregisterFilePath(
			":/style/clockalarmlistitemprototype_color.css");

}

/*!
	Called by the ClockViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The ClockAppController object.
	\param docLoader Pointer to ClockDocLoader object.
 */
void ClockMainView::setupView(
		ClockAppControllerIf &controllerIf, ClockDocLoader *docLoader)
{
	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;

	mTimezoneClient = controllerIf.timezoneClient();
	mSettingsUtility = controllerIf.settingsUtility();
	mAlarmClient = controllerIf.alarmClient();

	// Create the model.
	mAlarmListModel =  new ClockAlarmListModel(*mAppControllerIf, this);
	connect(
			mAlarmListModel, SIGNAL(changeAlarmListDisplay()),
			this, SLOT(handleAlarmListDisplay()));

	QList<AlarmInfo> alarmInfoList;
	// Get the list of pending clock alarms.
	mAlarmClient->getAlarmList(alarmInfoList);
	QList<AlarmInfo> displayInfoList;
	AlarmInfo alarmInfo;
	for (int index = 0; index < alarmInfoList.count(); ++index) {
		alarmInfo = alarmInfoList[index];
		if (Notified == alarmInfo.alarmState) {
			continue;
		}
		displayInfoList.append(alarmInfo);
	}
	int alarmCount = displayInfoList.count();

	HbMainWindow *window = hbInstance->allMainWindows().first();

	bool loadSuccess = false;
	Qt::Orientation currentOrienation = window->orientation();

	// Get the "No alarm set" label.
	mNoAlarmLabel = qobject_cast<HbLabel *> (
			mDocLoader->findWidget(CLOCK_NOALARMLABEL));

	// Get the alarm's list.
	mAlarmList = qobject_cast<HbListView *> (
			mDocLoader->findWidget(CLOCK_ALARMLIST));

	connect(
			mAlarmList, SIGNAL(activated(const QModelIndex &)),
			this, SLOT(handleActivated(const QModelIndex &)));

	connect(
			mAlarmList,
			SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)),
			this,
			SLOT(handleLongPress(HbAbstractViewItem*, const QPointF&)));

	HbStyleLoader::registerFilePath(":/style/clockalarmlistitemprototype.css");
	HbStyleLoader::registerFilePath(
			":/style/clockalarmlistitemprototype.widgetml");
	HbStyleLoader::registerFilePath(
			":/style/clockalarmlistitemprototype_color.css");
	setmodel();

	// Load the correct section based on orientation.
	if (Qt::Vertical == currentOrienation) {
		mDocLoader->load(
				CLOCK_MAIN_VIEW_DOCML,
				CLOCK_MAIN_VIEW_PORTRAIT_SECTION,
				&loadSuccess);
	} else {
		mDocLoader->load(
				CLOCK_MAIN_VIEW_DOCML,
				CLOCK_MAIN_VIEW_LANDSCAPE_SECTION,
				&loadSuccess);
	}
	if (loadSuccess) {
		if (0 == alarmCount) {
			hideAlarmList(true);
		} else {
			hideAlarmList(false);
			mAlarmListModel->populateModel();
		}
	}

	mDayLabel = static_cast<HbLabel *> (
			mDocLoader->findObject("dateLabel"));

	mPlaceLabel = static_cast<HbLabel *> (
			mDocLoader->findObject("placeLabel"));

	

	mClockWidget = static_cast<ClockWidget*> (
			mDocLoader->findObject(CLOCK_WIDGET));

	// Update the date info.
	updateDateLabel();
	// Update the place info.
	updatePlaceLabel(mTimezoneClient->timeUpdateOn());
	// Update clock widget display.
	updateClockWidget();

	// Connect to orientation change and load appropriate section.
	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(checkOrientationAndLoadSection(Qt::Orientation)));
}

/*!
	To defer the connects and initialization. To be done after the view is drawn.
	Should be called in the slot of view ready.
 */
void ClockMainView::setupAfterViewReady()
{
	// Get the toolbar/menu actions.
	mRefreshMainView = static_cast<HbAction *> (
			mDocLoader->findObject("alarmsAction"));

	mRefreshMainView->setCheckable(true);
	mRefreshMainView->setChecked(true);

	connect(
			mRefreshMainView, SIGNAL(changed()),
			this, SLOT(refreshMainView()));

	mDisplayWorldClockView = static_cast<HbAction *> (
			mDocLoader->findObject("worldClockAction"));
	connect(
			mDisplayWorldClockView, SIGNAL(triggered()),
			this, SLOT(displayWorldClockView()));

	mAddNewAlarm = static_cast<HbAction *> (
			mDocLoader->findObject("newAlarmAction"));
	connect(
			mAddNewAlarm, SIGNAL(triggered()),
			this, SLOT(addNewAlarm()));

	mSettingsAction = static_cast<HbAction *> (
			mDocLoader->findObject("settingsAction"));
	connect(
			mSettingsAction, SIGNAL(triggered()),
			this, SLOT(openSettings()));

	// Connect the necessary timezone client call backs.
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateView()));
	connect(
			mTimezoneClient, SIGNAL(autoTimeUpdateChanged(int)),
			this, SLOT(updatePlaceLabel(int)));
	connect(
			mTimezoneClient, SIGNAL(cityUpdated()),
			this, SLOT(updatePlaceLabel()));
	
}

/*!
	Slot which gets called for the status change of the alarm i.e for
	active/inactive.

	\param row contains the row number of the alarm in the alarm list
 */
void ClockMainView::handleAlarmStatusChanged(int row)
{
	AlarmInfo alarmInfo;

	// Get the data for the alarm.
	QList<QVariant> alarmData =
			mAlarmListModel->sourceModel()->index(row, 0).data(
					AlarmDetails).toList();
	int alarmId = alarmData.at(0).value<int>();

	mAlarmClient->getAlarmInfo(alarmId, alarmInfo);

	mSelectedItem = row;

	if (-1 < mSelectedItem) {
		QList<QVariant> alarmData =
				mAlarmListModel->sourceModel()->index(
						mSelectedItem, 0).data(AlarmDetails).toList();
		int alarmId = alarmData.at(0).value<int>();
		QString displayNote;
		// Activate or deactivate the alarm depending on the alarm status.
		// Display the NotificationDialog with appropriate message.
		if (Snoozed == alarmInfo.alarmState && Enabled == alarmInfo.alarmStatus
				&& Once != alarmInfo.repeatType) {
			removeSnoozedAlarm();
			displayNote.append(tr("Snoozed alarm removed"));
		}else if (Enabled == alarmInfo.alarmStatus) {
			mAlarmClient->toggleAlarmStatus(alarmId, Disabled);
			displayNote.append(
			    hbTrId("txt_clock_main_view_dpopinfo_alarm_deactivated"));
		} else {
			mAlarmClient->toggleAlarmStatus(alarmId, Enabled);
			displayNote.append(
			    hbTrId("txt_clock_main_view_dpopinfo_alarm_activated"));
		}

		HbNotificationDialog *dialog = new HbNotificationDialog();
		dialog->setTitle(displayNote);
		dialog->setTimeout(HbPopup::ConfirmationNoteTimeout);
		dialog->show();

		mSelectedItem = -1;
	}
}

/*!
	Slot which gets called when `Alarm' action is triggered from the view
	toolbar. This is responsible for reloading the content of the main view.
 */
void ClockMainView::refreshMainView()
{
	mRefreshMainView->setChecked(true);

}

/*!
	Slot which gets called when `World clock' action is triggered from the view
	toolbar. This is responsible for launching the world clock.
 */
void ClockMainView::displayWorldClockView()
{
	mAppControllerIf->switchToView(WorldClock);

}

/*!
	Slot which gets called when `New alarm' action is triggered from the view
	toolbar. This is responsible for launching the editor to create a new alarm.
 */
void ClockMainView::addNewAlarm()
{
	ClockAlarmEditor *alarmEditor = new ClockAlarmEditor(*mAlarmClient);
	alarmEditor->showAlarmEditor();
}

/*!
	Slot which gets called when `Settings' action is triggered from the view
	menu. This is responsible for launching the settings view.
 */
void ClockMainView::openSettings()
{
	// Create the settings view.
	ClockSettingsView *settingsView = new ClockSettingsView(this);
	settingsView->loadSettingsView();
}

/*!
	This slot handles the 'activated' signal of the alarm list. Opens the
	alarm editor to edit the alarm.
	\param index the modelIndex in the list which has been tapped.
 */
void ClockMainView::handleActivated(const QModelIndex &index)
{
	if(!mIsLongTop) {// Get the data for the alarm.
		int row = index.row();
		QList<QVariant> alarmData =
				mAlarmListModel->sourceModel()->index(row, 0).data(
						AlarmDetails).toList();
		int alarmId = alarmData.at(0).value<int>();

		// Construct the alarm editor.
		ClockAlarmEditor *alarmEditor = new ClockAlarmEditor(
				*mAlarmClient, alarmId);
		alarmEditor->showAlarmEditor();
	}
}

/*!
	This slot handles the 'longpressed' signal of the alarm list. Opens the
	context menu.

	\param item The item in the list which has been long pressed.
	\param coords The position where mouse was pressed.
 */
void ClockMainView::handleLongPress(
		HbAbstractViewItem *item, const QPointF &coords)
{
	mIsLongTop = true;
	AlarmInfo alarmInfo;

	// Save the item row number where the long press was made.
	mSelectedItem = item->modelIndex().row();

	// Get the data for the alarm.
	QList<QVariant> alarmData =
			mAlarmListModel->sourceModel()->
			index(mSelectedItem, 0).data(AlarmDetails).toList();
	int alarmId = alarmData.at(0).value<int>();

	mAlarmClient->getAlarmInfo(alarmId, alarmInfo);

	// On long press we display item specific context menu.
	HbMenu *itemContextMenu = new HbMenu();
	connect(
			itemContextMenu,SIGNAL(aboutToClose()),
			this, SLOT(handleMenuClosed()));

	// Add the delete action to the context menu.
	mDeleteAction = itemContextMenu->addAction(
			hbTrId("txt_clk_main_view_menu_delete_alarm"));

	// Show the menu.
	itemContextMenu->open(this, SLOT(selectedMenuAction(HbAction*)));
	itemContextMenu->setPreferredPos(coords);
}

/*!
	This slot handles the deletion of the alarm from the server.

 */
void ClockMainView::deleteAlarm()
{
	if (-1 < mSelectedItem) {
		// Get the data for the alarm.
		QList<QVariant> alarmData = mAlarmListModel->sourceModel()->
				index(mSelectedItem, 0).data(AlarmDetails).toList();
		int alarmId = alarmData.at(0).value<int>();
		mAlarmClient->deleteAlarm(alarmId);
		mSelectedItem = -1;
	}
}

/*!
	Deltes the snoozed alarm.
 */
void ClockMainView::removeSnoozedAlarm()
{
	if (-1 < mSelectedItem) {
		// Get the data for the alarm.
		QList<QVariant> alarmData = mAlarmListModel->sourceModel()->
		index(mSelectedItem, 0).data(AlarmDetails).toList();
		int alarmId = alarmData.at(0).value<int>();
		mAlarmClient->deleteSnoozedAlarm(alarmId);
		mSelectedItem = -1;
	}
}

void ClockMainView::updateView()
{
	// Update the place label.
	updatePlaceLabel(mTimezoneClient->timeUpdateOn());
	// Update date label.
	updateDateLabel();
	// Update clock widget.
	updateClockWidget();
}

/*!
	Slot which gets called for value change in auto time update in cenrep.

	\param autoTimeUpdate Value of auto time update.
 */
void ClockMainView::updatePlaceLabel(int autoTimeUpdate)
{
	if (-1 == autoTimeUpdate) {
		autoTimeUpdate = mTimezoneClient->timeUpdateOn();
	}
	
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

	// Append space.
	gmtOffset += tr(" ");

	// Append GMT sting.
	gmtOffset += hbTrId("txt_common_common_gmt");

	// Append space.
	gmtOffset += tr(" ");

	// Append DST info.
	if (currentZoneInfo.dstOn) {
		gmtOffset += hbTrId("txt_common_setlabel_dst");
	}

	// Update the labels with the correct info.
	mPlaceLabel->clear();
	if(autoTimeUpdate) {
		mPlaceLabel->setPlainText(
				currentZoneInfo.countryName + tr(" ") + gmtOffset);
	} else {
		mPlaceLabel->setPlainText(
				currentZoneInfo.cityName + tr(", ")
				+ currentZoneInfo.countryName + tr(" ") + gmtOffset);
	}
}

/*!
	Slot to either hide or show the alarmlist.
 */
void ClockMainView::handleAlarmListDisplay()
{
	// Get the list of pending clock alarms from server.
	QList<AlarmInfo> alarmInfoList;
	QList<AlarmInfo> displayInfoList;
	AlarmInfo alarmInfo;
	mAlarmClient->getAlarmList(alarmInfoList);
	for (int index = 0; index < alarmInfoList.count(); ++index) {
		alarmInfo = alarmInfoList[index];
		if (Notified == alarmInfo.alarmState) {
			continue;
		}
		displayInfoList.append(alarmInfo);
	}
	if (mHideAlarmList) {
		hideAlarmList(false);
	} else {
		if (0 == displayInfoList.count() &&
				0 == mAlarmListModel->sourceModel()->rowCount()) {
			hideAlarmList(true);
		}
	}

}

/*!
	Slot to check the orientation & load the appropriate section.

	\param orientation contains the current orientation of the window.
 */
void ClockMainView::checkOrientationAndLoadSection(
		Qt::Orientation orientation)
{
	bool success;
	// If horizontal, load the landscape section.
	if (Qt::Horizontal == orientation) {
		mDocLoader->load(
				CLOCK_MAIN_VIEW_DOCML, CLOCK_MAIN_VIEW_LANDSCAPE_SECTION,
				&success);
	} else {
		mDocLoader->load(
				CLOCK_MAIN_VIEW_DOCML, CLOCK_MAIN_VIEW_PORTRAIT_SECTION,
				&success);
	}

	if(success) {
		QList<AlarmInfo> alarmInfoList;
		// Get the list of all clock alarms.
		mAlarmClient->getAlarmList(alarmInfoList);
		if (0 == alarmInfoList.count()) {
			hideAlarmList(true);
		} else {
			hideAlarmList(false);
		}
	}
}

/*!
	Slot to handle context menu actions.
 */
void ClockMainView::selectedMenuAction(HbAction *action)
{
	if (action == mDeleteAction) {
		deleteAlarm();
	}
}

/*!
	Slot to handle the context menu closed.
 */
void ClockMainView::handleMenuClosed()
{
	mIsLongTop = false;
}
/*!
	Sets the model to the alarm list.
 */
void ClockMainView::setmodel()
{
	// Set the model.
	if (mAlarmList) {
		mAlarmList->setModel(mAlarmListModel->sourceModel());
		ClockAlarmListItemPrototype *listItemPrototype =
				new ClockAlarmListItemPrototype();
		connect(
				listItemPrototype, SIGNAL(alarmStatusHasChanged(int)),
				this, SLOT(handleAlarmStatusChanged(int)));
		mAlarmList->setItemPrototype(listItemPrototype);
		mAlarmList->setLayoutName("layout-alarmlist");
	}

}

/*!
	Hides the alarmlist in the main view.

	\param hide 'true' if alarm list is to be hidden.
 */
void ClockMainView::hideAlarmList(bool hide)
{
	if (hide) {
		mNoAlarmLabel->show();
		mAlarmList->hide();
		mHideAlarmList = true;
	} else {
		mAlarmList->show();
		mNoAlarmLabel->hide();
		mHideAlarmList = false;
	}
}

/*!
	Updates the day and date in the day label.
 */
void ClockMainView::updateDateLabel()
{
	// Get the current datetime.
	QDateTime dateTime = QDateTime::currentDateTime();
	// Get the day name.
	QString dayName = dateTime.toString("ddd");
	// Get the date in correct format.
	QString currentDate = mSettingsUtility->date();
	// Construct the day + date string.
	QString dayDateString;
	dayDateString+= dayName;
	dayDateString += " ";
	dayDateString += currentDate;

	mDayLabel->setPlainText(dayDateString);
}

/*!
	Updates the zone info in the place label.
 */
/*void ClockMainView::updatePlaceLabel()
{
	updatePlaceLabel(mTimezoneClient->timeUpdateOn());
}*/

/*!
	Updates the clock widget display.
 */
void ClockMainView::updateClockWidget()
{
	mClockWidget->updateTime();
}

// End of file	--Don't remove.
