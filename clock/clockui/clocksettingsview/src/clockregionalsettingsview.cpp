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
* Definition file for class ClockRegionalSettingsView.
*
*/

// System includes
#include <HbView>
#include <HbAction>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbDataFormViewItem>
#include <HbMainWindow>
#include <HbInstance>
#include <HbPushButton>
#include <HbExtendedLocale>
#include <HbListWidget>
#include <HbComboBox>
#include <HBListWidgetItem>
#include <HbApplication>
#include <QTranslator>

// User includes
#include "clockregionalsettingsview.h"
#include "clocksettingsdocloader.h"
#include "clocksettingsdefines.h"
#include "settingsdatatypes.h"
#include "settingscustomitem.h"

/*!
	\class ClockRegionalSettingsView

	The view for regional setting items in clock applications settings.
 */

/*!
	Default constructor.
 */
ClockRegionalSettingsView::ClockRegionalSettingsView(
		SettingsUtility &utility, QObject *parent)
:QObject(parent),
 mView(0),
 mBackAction(0),
 mLoader(0),
 mSettingsUtility(utility)
{
	// Construct the document loader.
	mLoader = new ClockSettingsDocLoader;
	
	// Load the translation file and install the editor specific translator
    mTranslator = new QTranslator;
    //QString lang = QLocale::system().name();
    //QString path = "Z:/resource/qt/translations/";
    mTranslator->load("clocksettingsview",":/translations");
    // TODO: Load the appropriate .qm file based on locale
    //bool loaded = mTranslator->load("caleneditor_" + lang, path);
    HbApplication::instance()->installTranslator(mTranslator);
	// Create the custom prototype.
	mCustomPrototype = new SettingsCustomItem();
}

/*!
	Destructor.
 */
ClockRegionalSettingsView::~ClockRegionalSettingsView()
{
	if (mLoader) {
		delete mLoader;
		mLoader = 0;
	}
// Remove the translator
    HbApplication::instance()->removeTranslator(mTranslator);
    if (mTranslator) {
        delete mTranslator;
        mTranslator = 0;
    }
}

/*!
	Shows the regional settings view.
 */
void ClockRegionalSettingsView::showView()
{
	bool success;

	// Load the application xml.
	mLoader->load(CLOCK_REG_SETTINGS_VIEW_DOCML, &success);
	if (!success) {
		qFatal("Unable to load the docml.");
	}

	// Find the main view.
	mView = static_cast<HbView *> (
			mLoader->findWidget(CLOCK_REG_SETTINGS_VIEW));
	if (!mView) {
		qFatal("Unable to find view");
	}

	// Find the data form.
	mForm = static_cast<HbDataForm *> (
			mLoader->findWidget(CLOCK_REG_SETTINGS_DATA_FORM));
	if (!mForm) {
		qFatal("Unable to find the form");
	}
	connect(
			mForm, SIGNAL(itemShown(QModelIndex)),
			this, SLOT(handleItemDisplayed(QModelIndex)));
	

	QList <HbAbstractViewItem*> prototypes = mForm->itemPrototypes();
	prototypes.append(mCustomPrototype);
	mForm->setItemPrototypes(prototypes);
	mCustomPrototype->setWeekdaysList(weekdayList());

	// Create the form model.
	createModel();

	// Set the view as the current view.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	window->addView(mView);
	window->setCurrentView(mView);

	// Add the back softkey.
	mBackAction = new HbAction(Hb::BackAction);
	mView->setNavigationAction(mBackAction);
	connect(
			mBackAction, SIGNAL(triggered()),
			this, SLOT(handleBackAction()));
}

/*!
	The view is removed from main window and a deleteLater is called on `this'.
 */
void ClockRegionalSettingsView::handleBackAction()
{
	HbExtendedLocale locale = HbExtendedLocale::system();
	HbExtendedLocale::WeekDay startOfWeekIndex =
				HbExtendedLocale::system().startOfWeek();
	// TODO: Save workdays settings.
	QItemSelectionModel *model = 0;
	model = mWorkdaysItem->selectionModel();
	QModelIndexList selectedModelIndex = model->selectedIndexes();
	int count = selectedModelIndex.count();
	QModelIndex index;
	for(int i = 0 ; i < count ; i++){
		index = selectedModelIndex[i];
	}
	
	QString workdayString("0000000");
	for (int i = 0; i < workdayString.count(); ++i) {
		if (model->isSelected(model->model()->index(i, 0))) {
			workdayString.replace(i, 1, '1');
		}
	}
	
	QString workdaysSetting = workdayString;
	int max = workdayString.size() - 1;
	for (int index = max - startOfWeekIndex, i = 0; i <= max; i++) {
		QChar ch = workdayString.at(index);
		workdaysSetting.replace(i, 1, ch);
		if (index == 0) {
			index = max;
		} else {
			index--;
		}
	}
	locale.setWorkDays(workdaysSetting);
	
	HbMainWindow *window = hbInstance->allMainWindows().first();
	// Cleanup.
	window->removeView(mView);
	deleteLater();
}

/*!
	Called when each of the form items are displayed. Using this slot, we
	connect to SIGNALS of the items that have been added to the form.

	\param index QModelIndex of the row that was just displayed.
 */
void ClockRegionalSettingsView::handleItemDisplayed(const QModelIndex &index)
{
	if (!index.isValid()) {
		return;
	}

	HbDataFormViewItem *item =
			static_cast<HbDataFormViewItem*>(mForm->itemByIndex(index));
	HbWidget *widget = item->dataItemContentWidget();

	switch (index.row()) {
		case 0:

			break;
		case 1:

			break;

		case 2:

			break;

		case 3:

			break;

		case 4:
			mWorkdaysItem = static_cast<HbListWidget *> (widget);
		case 5:
			mStartOfWeekItem = static_cast<HbComboBox *> (widget);
			break;
		default:
			break;
	}
}

/*!
	Called when the time format toggle item is clicked. Here we update the
	value in the locale.
 */
void ClockRegionalSettingsView::handleTimeFormatChange()
{
//	mSettingsUtility.setTimeFormat(mTimeFormatItem->text());
	mSettingsUtility.setTimeFormat(
			mTimeFormatItem->contentWidgetData("text").toString());
}

/*!
	Called when the time separator toggle item is clicked. Here we update the
	value in the locale.
 */
void ClockRegionalSettingsView::handleTimeSeparatorChange()
{
//	mSettingsUtility.setTimeSeparator(mTimeSeparatorItem->text());
	mSettingsUtility.setTimeSeparator(
			mTimeSeparatorItem->contentWidgetData("text").toString());
}

/*!
 */
void ClockRegionalSettingsView::handleDateFormatChange(QString text)
{
	mSettingsUtility.setDateFormat(text);
}

/*!
    Called when the date separator item is changed. Here we update the
    value in the locale.
 */
void ClockRegionalSettingsView::handleDateSeparatorChange(QString text)
{
	mSettingsUtility.setDateSeparator(text);
}

/*!
	This slot is called any item in the data form is changed.
 */
void ClockRegionalSettingsView::handleDataChanged(
		const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
	qDebug("clock: ClockRegionalSettingsView::handleDataChanged() -->");

	Q_UNUSED(bottomRight)


	switch (topLeft.row()) {
/*		case 4:
		{
		// Weekdays item.
		HbDataFormModelItem *workdayItem = mFormModel->itemFromIndex(topLeft);
		QList<QVariant> items = workdayItem
				->contentWidgetData("selectedItems").toList();
		int temp = items.count();
		for (int var = 0; var < temp; ++var) {
	        int temp1 = items.at(var).toInt();
        }
		}
		break;*/
		case 5:
		{
		// The Start of week item.
		if (mStartOfWeekItem != 0) {
			int index = mStartOfWeekItem->currentIndex();
			mSettingsUtility.setStartOfWeek(index);
			
			QStringList weekdays = weekdayList();
			QString workdays = mCustomPrototype->workdaysSetting();
			QItemSelectionModel *model = 0;
			model = mWorkdaysItem->selectionModel();
			
			for (int i = 0, index = workdays.size() - 1;
					i < mWorkdaysItem->count(); ++i, index--) {
				QString str = weekdays[i];
				mWorkdaysItem->item(i)->setText(str);
				
				QChar ch = workdays.at(index);
				if ( ch == QChar('0')) {
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
		}
		}
		break;

		default:
		break;
	}
}

/*!
	Here we create the form model.
 */
void ClockRegionalSettingsView::createModel()
{
	if (mForm->model()) {
		delete mForm->model();
		mForm->setModel(0);
	}

	mFormModel = new HbDataFormModel(this);
	
	// Populate the form model.
	populateFormModel();
	
	mForm->setModel(mFormModel);

	connect(
			mFormModel,
			SIGNAL(dataChanged(const QModelIndex, const QModelIndex)),
			this,
			SLOT(handleDataChanged(const QModelIndex, const QModelIndex)));
}

/*!
	Function in which we populate the form model.
 */
void ClockRegionalSettingsView::populateFormModel()
{
	if (!mFormModel) {
		createModel();
	}

	// Get the locale.
	 HbExtendedLocale locale = HbExtendedLocale::system();

	// Time format item.
	 mTimeFormatItem = mFormModel->appendDataFormItem(
			HbDataFormModelItem::ToggleValueItem,
			hbTrId("txt_clock_setlabel_time_format"));
	int index = mSettingsUtility.timeFormat(mTimeFormatStringList);
	if (0 == index) {
		mTimeFormatItem->setContentWidgetData("text", mTimeFormatStringList[0]);
		mTimeFormatItem->setContentWidgetData("additionalText", mTimeFormatStringList[1]);
	} else {
		mTimeFormatItem->setContentWidgetData("text", mTimeFormatStringList[1]);
		mTimeFormatItem->setContentWidgetData("additionalText", mTimeFormatStringList[0]);
	}
	mForm->addConnection(
			mTimeFormatItem, SIGNAL(clicked()),
			this, SLOT(handleTimeFormatChange()));

	// Time separator item.
	mTimeSeparatorItem = mFormModel->appendDataFormItem(
			HbDataFormModelItem::ToggleValueItem,
			hbTrId("txt_clock_setlabel_time_separator"));
	index = mSettingsUtility.timeSeparator(mTimeSeparatorStringList);
	if (0 == index) {
		mTimeSeparatorItem->setContentWidgetData("text", mTimeSeparatorStringList[0]);
		mTimeSeparatorItem->setContentWidgetData(
				"additionalText", mTimeSeparatorStringList[1]);
	} else {
		mTimeSeparatorItem->setContentWidgetData("text", mTimeSeparatorStringList[1]);
		mTimeSeparatorItem->setContentWidgetData(
				"additionalText", mTimeSeparatorStringList[0]);
	}
	mForm->addConnection(
			mTimeSeparatorItem, SIGNAL(clicked()),
			this, SLOT(handleTimeSeparatorChange()));

	// Date format.
	mDateFormatItem = mFormModel->appendDataFormItem(
			HbDataFormModelItem::ComboBoxItem,
			hbTrId("txt_clock_setlabel_date_format"));
	index = mSettingsUtility.dateFormat(mDateFormatStringList);

	mDateFormatItem->setContentWidgetData("items", mDateFormatStringList);
	mDateFormatItem->setContentWidgetData("currentIndex",index);
	mForm->addConnection(
			mDateFormatItem, SIGNAL(currentIndexChanged(QString)),
	 		this, SLOT(handleDateFormatChange(QString)));

	// Date separator.
	mDateSeparatorItem = mFormModel->appendDataFormItem(
			HbDataFormModelItem::ComboBoxItem,
			hbTrId("txt_clock_setlabel_date_separator"));
	index = mSettingsUtility.dateSeparator(mDateSeparatorStringList);

	mDateSeparatorItem->setContentWidgetData("items", mDateSeparatorStringList);
	mDateSeparatorItem->setContentWidgetData("currentIndex",index);
	mForm->addConnection(
			mDateSeparatorItem, SIGNAL(currentIndexChanged(QString)),
			this, SLOT(handleDateSeparatorChange(QString)));

	// Workdays.
	HbDataFormModelItem *item = 0;
	// Create the weekday list based on start of week.
	QStringList weekdaysList;
	weekdaysList
			<< hbTrId("txt_clk_setlabel_val_monday")
			<< hbTrId("txt_clk_setlabel_val_tuesday")
			<< hbTrId("txt_clk_setlabel_val_wednesday")
			<< hbTrId("txt_clk_setlabel_val_thursday")
			<< hbTrId("txt_clk_setlabel_val_friday")
			<< hbTrId("txt_clk_setlabel_val_saturday")
			<< hbTrId("txt_clk_setlabel_val_sunday");
	
	HbDataFormModelItem::DataItemType workdaysItemType =
			static_cast<HbDataFormModelItem::DataItemType>
			(HbDataFormModelItem::CustomItemBase + 50);
	item = new HbDataFormModelItem(workdaysItemType,
			hbTrId("txt_clock_setlabel_workdays"));
	mFormModel->appendDataFormItem(item);

	// Start of week item.
	item = 0;
	item = mFormModel->appendDataFormItem(HbDataFormModelItem::ComboBoxItem,
		hbTrId("txt_clock_setlabel_week_starts_on"));
	HbExtendedLocale::WeekDay startOfWeek = locale.startOfWeek();
	item->setContentWidgetData("items", weekdaysList);
	item->setContentWidgetData("currentIndex", startOfWeek);
}

/*!
	Returns the weekday list ordered based on start of week.
 */

QStringList ClockRegionalSettingsView::weekdayList()
{
	QStringList weekDays;
	QStringList daysList;
	daysList
			<< hbTrId("txt_clk_setlabel_val_monday")
			<< hbTrId("txt_clk_setlabel_val_tuesday")
			<< hbTrId("txt_clk_setlabel_val_wednesday")
			<< hbTrId("txt_clk_setlabel_val_thursday")
			<< hbTrId("txt_clk_setlabel_val_friday")
			<< hbTrId("txt_clk_setlabel_val_saturday")
			<< hbTrId("txt_clk_setlabel_val_sunday");
	
	HbExtendedLocale::WeekDay startOfWeekIndex =
			HbExtendedLocale::system().startOfWeek();
	
	for (int i = 0, index = startOfWeekIndex; i < daysList.count(); ++i) {
		weekDays.append(daysList.at(index));
		if (6 == index) {
			index = 0;
		} else {
			index++;
		}
	}
	
	return weekDays;
}

// End of file	--Don't remove this.
