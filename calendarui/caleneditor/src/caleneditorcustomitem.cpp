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
 * Description: Definition file for class CalenEditorCustomItem.
 *
 */

// System includes
#include <QObject>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hbpushbutton.h>
#include <hblineedit.h>
#include <hbdataformmodelitem.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbdatetimepicker.h>
#include <hbi18ndef.h>
#include <HbDataFormModel>
#include <HbAbstractViewItem>
#include <HbAbstractItemView>
#include <QGraphicsLinearLayout>
#include <QDate>
#include <QTime>

// User includes
#include "caleneditorcustomitem.h"
#include "calendateutils.h"
#include "caleneditorcommon.h"

/*!
	\class CalenEditorCustomItem
	This object creats the custom item for the calendar editor dataform.
 */

/*!
	Constructor.
	\param parent The parent object.
 */
CalenEditorCustomItem::CalenEditorCustomItem(QGraphicsItem *parent)
:HbDataFormViewItem(parent)
{
	mMinDate = CalenDateUtils::minTime().date();
	mMaxDate = CalenDateUtils::maxTime().date();
	mMinTime.setHMS(0,0,0,0);
	mMaxTime.setHMS(23,59,59,999);
	mLocale = HbExtendedLocale::system();
}
/*!
	Destructor.
 */
CalenEditorCustomItem::~CalenEditorCustomItem()
{
	// Nothing yet.
}
/*!
	Creates a new CalenEditorCustomItem.
 */
HbAbstractViewItem* CalenEditorCustomItem::createItem()
{
	return new CalenEditorCustomItem(*this);
}
/*!
	Creates the custom widget.
 */
HbWidget* CalenEditorCustomItem::createCustomWidget()
{
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType> (modelIndex() .data(
					HbDataFormModelItem::ItemTypeRole).toInt());

	switch (itemType) {
		case CustomWidgetFrom:
		{
		HbWidget *widgetTop = new HbWidget();
		QGraphicsLinearLayout *layoutTop =
									new QGraphicsLinearLayout(Qt::Horizontal);
		widgetTop->setLayout(layoutTop);

			mPushButtonTime = new HbPushButton();
			connect(mPushButtonTime, SIGNAL(clicked()),
									this, SLOT(handleTime()));
			layoutTop->addItem(mPushButtonTime);

			mPushButtonDate =
			                new HbPushButton();
			connect(mPushButtonDate, SIGNAL(clicked()),
												this, SLOT(handleDate()));
			layoutTop->addItem(mPushButtonDate);
			return widgetTop;
		}


		case CustomWidgetTo:
		{
		HbWidget *widgetBottom = new HbWidget();
		QGraphicsLinearLayout *layoutBottom = new QGraphicsLinearLayout(Qt::Horizontal);
		widgetBottom->setLayout(layoutBottom);

		mPushButtonTime = new HbPushButton(this);
		connect(mPushButtonTime, SIGNAL(clicked()),
											this, SLOT(handleTime()));
		layoutBottom->addItem(mPushButtonTime);
		
		mPushButtonDate = new HbPushButton(this);
		connect(mPushButtonDate, SIGNAL(clicked()),
												this, SLOT(handleDate()));
		layoutBottom->addItem(mPushButtonDate);
		return widgetBottom;
		}
		
		case RepeatUntilOffset:
		{
		mRepeatUntilWidget = new HbPushButton(this);
		return mRepeatUntilWidget;
		}
				

		default:
			return 0;
	}
}

void CalenEditorCustomItem::populateDateTime(QDateTime defaultDateTime, bool isFromItem)
{
	// Store the date and time to be shown
    mDate = defaultDateTime.date();
	mTime = defaultDateTime.time();
	
	mIsFromItem = isFromItem;
	
    mPushButtonDate->setText(mLocale.format(defaultDateTime.date(), 
	                                           r_qtn_date_usual_with_zero));
	mPushButtonTime->setText(mLocale.format(defaultDateTime.time(), 
	                                           r_qtn_time_usual_with_zero));
}

void CalenEditorCustomItem::setDateRange(QDate start, QDate end)
{
	mMaxDate = end;
	mMinDate = start;
	// Check if both are falling on same day
	// If yea, the disable the button
	if (mMaxDate.year() == mMinDate.year() &&
			(mMaxDate.day() == mMinDate.day())) {
		enableDateButton(false);
	}
}

void CalenEditorCustomItem::setTimeRange(QTime start, QTime end)
{
	mMaxTime = start;
	mMinTime = end;
}

void CalenEditorCustomItem::enableDateButton(bool value)
{
	mPushButtonDate->setEnabled(value);
}



void CalenEditorCustomItem::handleDate()
{
	// Create a date picker
	mDatePicker = new HbDateTimePicker(mDate, this);
	mDatePicker->setMinimumDate(mMinDate);
	mDatePicker->setMaximumDate(mMaxDate);

	// Create a popup with datepicker for the user to select date.
	HbDialog popUp;
	popUp.setDismissPolicy(HbDialog::NoDismiss);
	popUp.setTimeout(HbDialog::NoTimeout);
	
	// Set the proper heading
	if (mIsFromItem) {
        popUp.setHeadingWidget(new HbLabel(hbTrId("txt_calendar_title_start_date")));   
    }else {
        popUp.setHeadingWidget(new HbLabel(hbTrId("txt_calendar_title_end_date")));
    }

	popUp.setContentWidget(mDatePicker);
	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"));
	popUp.setPrimaryAction(okAction);
	connect(okAction, SIGNAL(triggered()), this, SLOT(saveDate()));
	connect(okAction, SIGNAL(triggered()), &popUp, SLOT(close()));
	popUp.setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel")));
	popUp.exec();
}

void CalenEditorCustomItem::handleTime()
{
	// Create a time picker.
	mTimePicker = new HbDateTimePicker(mTime, this);
	if(mLocale.timeStyle() == HbExtendedLocale::Time12) {
		mTimePicker->setDisplayFormat("hh:mm ap");	
	}else {
		mTimePicker->setDisplayFormat("hh:mm");
	}
	mTimePicker->setTime(mTime);
	// Create a popup with time picker for the user to select time.
	HbDialog popUp;
	popUp.setDismissPolicy(HbDialog::NoDismiss);
	popUp.setTimeout(HbDialog::NoTimeout);
	
	// Set the proper heading
    if (mIsFromItem) {
        popUp.setHeadingWidget(new HbLabel(hbTrId("txt_calendar_title_start_time")));   
    }else {
        popUp.setHeadingWidget(new HbLabel(hbTrId("txt_calendar_title_end_time")));
    }
    
	popUp.setContentWidget(mTimePicker);
	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"));
	popUp.setPrimaryAction(okAction);
	connect(okAction, SIGNAL(triggered()), this, SLOT(saveTime()));
	connect(okAction, SIGNAL(triggered()), &popUp, SLOT(close()));
	popUp.setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"), &popUp));
	popUp.exec();
}

/*!
	Save the start date of the event.
 */
void CalenEditorCustomItem::saveDate()
{
	mDate = mDatePicker->date(); 
	if (mDate.isValid()) {
		mDateTime.setDate(mDate);
		mDateTime.setTime(mTime);
		mPushButtonDate->setText(mLocale.format(mDate, 
		                                           r_qtn_date_usual_with_zero));
		emit dateTimeUpdated(mDateTime);
		}
}

/*!
	Save the start time of the event.
 */
void CalenEditorCustomItem::saveTime()
{
	mTime = mTimePicker->time();
	if (mTime.isValid()) {
		mDateTime.setTime(mTime);
		mPushButtonTime->setText(mLocale.format(mTime, 
									r_qtn_time_usual_with_zero));

		mDateTime.setDate(mDate);
		emit dateTimeUpdated(mDateTime);
	}
}

/*!
	Enable/Desable FromTime filed and update time value
 */
void CalenEditorCustomItem::enableFromTimeFieldAndSetTime(bool enableTimeFiles, QDateTime fromDateTime)
{
	// Set FromTime filed Read-Only/Editable 
	mPushButtonTime->setEnabled(enableTimeFiles);
	
	// Set FromTime in Editor
	mPushButtonTime->setText(mLocale.format(fromDateTime.time(),r_qtn_time_usual_with_zero));
}

/*!
	Enable/Desable ToTime filed and update time value
 */
void CalenEditorCustomItem::enableToTimeFieldAndSetTime(bool enableTimeFiles, QDateTime toDateTime)
{
	// Set ToTime filed Read-Only/Editable 
	mPushButtonTime->setEnabled(enableTimeFiles);
	
	// Set ToTime in Editor
	mPushButtonTime->setText(mLocale.format(toDateTime.time(),r_qtn_time_usual_with_zero));
}

/*!
   OverRide from DataFrom
 */
bool CalenEditorCustomItem::canSetModelIndex(const QModelIndex &index) const
{
    HbDataFormModelItem::DataItemType itemType = 
        static_cast<HbDataFormModelItem::DataItemType>(
        index.data(HbDataFormModelItem::ItemTypeRole).toInt());

    if(itemType == CustomWidgetFrom || itemType == CustomWidgetTo || itemType == RepeatUntilOffset) {
        return true;
    } else {
        return false;
    }
}

void CalenEditorCustomItem::restore()
{
    HbDataFormModelItem::DataItemType itemType = 
        static_cast<HbDataFormModelItem::DataItemType>(
        modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
    
    HbDataFormModel *model = 
    		static_cast<HbDataFormModel *>(
    		static_cast<HbAbstractViewItem *>(this)->itemView()->model());
    HbDataFormModelItem *modelItem = model->itemFromIndex(modelIndex());
    switch(itemType) {
    	case RepeatUntilOffset:
    	{
    		mRepeatUntilWidget->setText(modelItem->contentWidgetData("text").toString());
    	}
    	break;
    }
}


// End of file	--Don't remove this.
