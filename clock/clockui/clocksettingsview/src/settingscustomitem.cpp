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
* Definition file for class ClockSettingsView.
*
*/

// System includes
#include <QGraphicsLinearLayout>
#include <QItemSelectionModel>
#include <HbDataFormModelItem>
#include <HbDataFormModel>
#include <HbPushButton>
#include <HbLabel>
#include <HbDateTimePicker>
#include <HbAction>
#include <HbListWidget>
#include <HbExtendedLocale>
#include <HbListWidgetItem>

// User includes
#include "settingscustomitem.h"
#include "settingsdatatypes.h"
#include "settingsutility.h"
#include "clockcityselectionlist.h"
#include "timezoneclient.h"
#include "clockregionalsettingsview.h"

/*!
	\class SettingsCustomItem

	This class is responsible for creating custom items for clock settings.
 */

/*!
	Default Constructor.

	\param parent The parent.
 */

SettingsCustomItem::SettingsCustomItem(QGraphicsItem *parent)
:HbDataFormViewItem(parent)
{
	// Construct the settignsutility.
	mSettingsUtility = new SettingsUtility();
	// Construct the timezone client.
	mTimezoneClient = TimezoneClient::getInstance();
}

/*!
	Destructor.
 */
SettingsCustomItem::~SettingsCustomItem()
{
	if (!mTimezoneClient->isNull()) {
		mTimezoneClient->deleteInstance();
	}
}

/*!
	\reimp
	Creates SettingsCustomItem.
 */
HbAbstractViewItem* SettingsCustomItem::createItem()
{
	return new SettingsCustomItem(*this);
}

/*!

	\sa
 */
bool SettingsCustomItem::canSetModelIndex(const QModelIndex &index) const
{
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType>(
			index.data(HbDataFormModelItem::ItemTypeRole).toInt());

	if(itemType >= HbDataFormModelItem::CustomItemBase &&
			itemType <= (HbDataFormModelItem::CustomItemBase + 50)) {
		return true;
	} else {
		return false;
	}

}

void SettingsCustomItem::restore()
{
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType>(
			modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());

	HbDataFormModel* model =
			static_cast<HbDataFormModel*>(
			static_cast<HbAbstractViewItem*>(this)->itemView()->model());
	HbDataFormModelItem* modlItem = model->itemFromIndex(modelIndex());

	switch (itemType) {
		case (TimeItemOffset + HbDataFormModelItem::CustomItemBase):
		mTimeWidget->setProperty("text", modlItem->contentWidgetData("text"));
		break;

		case (DateItemOffset + HbDataFormModelItem::CustomItemBase):
		mDateWidget->setProperty("text", modlItem->contentWidgetData("text"));
		break;

		case (PlaceItemOffset + HbDataFormModelItem::CustomItemBase):
		mPlaceWidget->setProperty("text", modlItem->contentWidgetData("text"));
		break;
		case (50 + HbDataFormModelItem::CustomItemBase):

		break;
		default:
		break;
	}
}

void SettingsCustomItem::setWeekdaysList(QStringList list)
{
	mWeekdaysList = list;
}

/*!
	\reimp
	Creates custom data form item.
 */
HbWidget* SettingsCustomItem::createCustomWidget()
{
	HbDataFormModelItem::DataItemType itemType =
		static_cast<HbDataFormModelItem::DataItemType>(modelIndex()
		.data(HbDataFormModelItem::ItemTypeRole).toInt());

	switch (itemType) {
		case (TimeItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			mTimeWidget = new HbPushButton(this);
			connect(
					mTimeWidget, SIGNAL(clicked()),
					this, SLOT(launchTimePicker()));
			return mTimeWidget;
		}

		case (DateItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			mDateWidget = new HbPushButton(this);
			connect(
					mDateWidget, SIGNAL(clicked()),
					this, SLOT(launchDatePicker()));
			return mDateWidget;
		}

		case (PlaceItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			mPlaceWidget = new HbPushButton(this);
			connect(
					mPlaceWidget, SIGNAL(clicked()),
					this, SLOT(launchCitySelectionList()));
			return mPlaceWidget;
		}

		case (RegionalItemOffset + HbDataFormModelItem::CustomItemBase):
		{
			HbPushButton *regSettingsItem = new HbPushButton(this);
			regSettingsItem->setText(
				QString(hbTrId("txt_clock_button_regional_date_time_settings")));
			connect(
					regSettingsItem, SIGNAL(clicked()),
					this, SLOT(launchRegSettingsView()));
			return regSettingsItem;
		}

		case (50 + HbDataFormModelItem::CustomItemBase):
		{
			HbListWidget *workdaysItem = new HbListWidget();
			workdaysItem->setSelectionMode(HbAbstractItemView::MultiSelection);
			QItemSelectionModel *model = 0;
			model = workdaysItem->selectionModel();
			for (int index = 0; index < mWeekdaysList.count(); ++index) {
				workdaysItem->addItem(mWeekdaysList.at(index));
			}

			QString workdaysString = workdaysSetting();
			for (int i = 0, index = workdaysString.size() - 1;
					index >= 0; ++i, index--) {
				if (workdaysString.at(index) == QChar('0')) {
					// Not a workday.
					model->select(
							model->model()->index(i,0),
							QItemSelectionModel::Deselect);
				} else {
					// Workday.
					model->select(
							model->model()->index(i,0),
							QItemSelectionModel::Select);
				}
			}

			return workdaysItem;
		}

		default:
			return 0;
	}
}

/*!
	Launches the time picker dialog.
 */
void SettingsCustomItem::launchTimePicker()
{
	if (mTimePickerDialog) {
		delete mTimePickerDialog;
	}

	// Create the dialog.
	mTimePickerDialog = new HbDialog;
	mTimePickerDialog->setTimeout(HbDialog::NoTimeout);
	mTimePickerDialog->setDismissPolicy(HbDialog::NoDismiss);
	mTimePickerDialog->setAttribute(Qt::WA_DeleteOnClose, true );

	// Set the heading for the dialog.
	HbLabel * timeLabel =
		new HbLabel(hbTrId("txt_clock_setlabel_time"), mTimePickerDialog);
	mTimePickerDialog->setHeadingWidget(timeLabel);

	QStringList timeSeparator;
	int index = mSettingsUtility->timeFormat(timeSeparator);
	QString tumblerDisplayFormat = mSettingsUtility->timeFormatString()
			.replace(timeSeparator.at(index), QString("."));

	// Create the tumbler.
	HbDateTimePicker *timePicker = new HbDateTimePicker(mTimePickerDialog);
	timePicker->setDisplayFormat(tumblerDisplayFormat);
	// Set the tumbler as the content widget.
	mTimePickerDialog->setContentWidget(timePicker);
	timePicker->setTime(QTime::currentTime());

	mOkAction = new HbAction(
			QString(hbTrId("txt_common_button_ok")), mTimePickerDialog);
	mCancelAction = new HbAction(
			QString(hbTrId("txt_common_button_cancel")), mTimePickerDialog);

	mTimePickerDialog->addAction(mOkAction);
	mTimePickerDialog->addAction(mCancelAction);

	mTimePickerDialog->open(this, SLOT(selectedAction(HbAction*)));
}

/*!
	Handles the ok action of time/date picker dialog.
 */
void SettingsCustomItem::handleOkAction()
{
	if (mTimePickerDialog) {
		// Get the time from the time picker.
		QTime newTime = static_cast<HbDateTimePicker *> (
				mTimePickerDialog->contentWidget())->time();

		QString timeFormatString = mSettingsUtility->timeFormatString();

		// If time returned by the picker is valid, then only update.
		if (newTime.isValid()) {
			// Update the display text on form item.
			mTimeWidget->setText(newTime.toString(timeFormatString));
			// Update the system time.
			mTimezoneClient->setDateTime(
					QDateTime(QDate::currentDate(), newTime));
		}
	} else if(mDatePickerDialog) {
		// Get the time from the time picker.
		QDate newDate = static_cast<HbDateTimePicker *> (
				mDatePickerDialog->contentWidget())->date();

		QString dateFormatString = mSettingsUtility->dateFormatString();

		// If date returned by the picker is valid, then only update.
		if (newDate.isValid()) {
			// Update the display text on form item.
			mDateWidget->setText(newDate.toString(dateFormatString));
			// Update the system date.
			mTimezoneClient->setDateTime(
					QDateTime(newDate, QTime::currentTime()));
		}
	}
}

/*!
	Launches the date picker dialog.
 */
void SettingsCustomItem::launchDatePicker()
{
	if (mDatePickerDialog) {
		delete mDatePickerDialog;
	}

	// Create the dialog.
	mDatePickerDialog = new HbDialog;
	mDatePickerDialog->setTimeout(HbDialog::NoTimeout);
	mDatePickerDialog->setDismissPolicy(HbDialog::NoDismiss);
	mDatePickerDialog->setAttribute(Qt::WA_DeleteOnClose, true );

	// Set the heading for the dialog.
	HbLabel * timeLabel =
		new HbLabel(hbTrId("txt_clock_setlabel_date"), mDatePickerDialog);
	mDatePickerDialog->setHeadingWidget(timeLabel);

	// Create the tumbler.
	HbDateTimePicker *datePicker = new HbDateTimePicker(mDatePickerDialog);
	datePicker->setDisplayFormat(mSettingsUtility->dateFormatString());
	datePicker->setMinimumDate(QDate(2010, 01, 01));
	// Set the tumbler as the content widget.
	mDatePickerDialog->setContentWidget(datePicker);
	datePicker->setDate(QDate::currentDate());

	mOkAction = new HbAction(
			QString(hbTrId("txt_common_button_ok")), mDatePickerDialog);

	mCancelAction = new HbAction(
			QString(hbTrId("txt_common_button_cancel")),mDatePickerDialog);

	mDatePickerDialog->addAction(mOkAction);
	mDatePickerDialog->addAction(mCancelAction);

	mDatePickerDialog->open(this, SLOT(selectedAction(HbAction*)));
}

/*!
	Launches cityselectionlist for timezone selection.
 */
void SettingsCustomItem::launchCitySelectionList()
{
	mCitySelectionList = new ClockCitySelectionList(this);
	connect(
			mCitySelectionList, SIGNAL(citySelected(LocationInfo)),
			this, SLOT(updatePlaceItem(LocationInfo)));

	mCitySelectionList->showCityList();
}

/*!
	Launches cityselectionlist for timezone selection.

	\param info Will contain the location info selected by the user.
 */
void SettingsCustomItem::updatePlaceItem(LocationInfo info)
{
	if (-1 != info.timezoneId) {
		mTimezoneClient->setAsCurrentLocationL(info);
	}
}

/*!
	Launches the regional settings view.
 */
void SettingsCustomItem::launchRegSettingsView()
{
	ClockRegionalSettingsView *view =
			new ClockRegionalSettingsView();
	connect(mTimezoneClient, SIGNAL(timechanged()),
          view, SLOT(updateWeekStartOn()));
	view->showView();
	
}

/*!
	Slot to handle the selected action.
 */
void SettingsCustomItem::selectedAction(HbAction *action)
{
	if (action==mOkAction) {
		handleOkAction();
	}
}

/*!
	Returns the workdays setting string based on start of week setting.
 */
QString SettingsCustomItem::workdaysSetting()
{
	HbExtendedLocale locale = HbExtendedLocale::system();
	// 0(Sun)0(sat)1(Fri)1(Thu)1(Wed)1(Tue)1(Mon)
	QString workdaysString = locale.workDays();
	QString workdays = workdaysString;
	HbExtendedLocale::WeekDay startOfWeekIndex =
				HbExtendedLocale::system().startOfWeek();
	int max = workdaysString.size() - 1;
	for (int index = max - startOfWeekIndex, i = max; i >= 0; i--) {
		QChar ch = workdaysString.at(index);
		workdays.replace(i, 1, ch);
	    if (index == 0) {
	        index = max;
        } else {
        	index--;
        }
    }
//	workdays.append('\0');

	return workdays;
}

// End of file	--Don't remove this.
