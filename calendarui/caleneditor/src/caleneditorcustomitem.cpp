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

//LI related headers
#include "qlocationpickeritem.h"
#include <xqaiwrequest.h>
#include <xqappmgr.h>

// User includes
#include "caleneditorcustomitem.h"
#include "calendateutils.h"
#include "caleneditorcommon.h"
#include "caleneditordocloader.h"

/*!
	\class CalenEditorCustomItem
	This object creats the custom item for the calendar editor dataform.
 */

/*!
	Constructor.
	\param parent The parent object.
 */
CalenEditorCustomItem::CalenEditorCustomItem(QGraphicsItem *parent)
:HbDataFormViewItem(parent),
 mPushButtonTime(NULL),
 mPushButtonDate(NULL),
 mRepeatUntilWidget(NULL),
 mReminderTimeWidget(NULL),
 mDatePicker(NULL),
 mTimePicker(NULL)
{
	mMinDate = CalenDateUtils::minTime().date();
	mMaxDate = CalenDateUtils::maxTime().date();
	mMinTime.setHMS(0,0,0,0);
	mMaxTime.setHMS(23,59,59,999);
	mLocale = HbExtendedLocale::system();
	mLocationLineEdit = NULL;
	mLocationPushButton = NULL;
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
		QGraphicsLinearLayout *layoutBottom = 
									new QGraphicsLinearLayout(Qt::Horizontal);
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
				
		case CustomWidgetLocation:
		{
			CalenEditorDocLoader editorLocationDocLoader;
			bool loadSuccess = false;

			editorLocationDocLoader.load(CALEN_EDITOR_LOCATION_XML_FILE, 
														&loadSuccess);
			Q_ASSERT_X(loadSuccess, "caleneditor.cpp", 
								"Unable to load caleneditor location view XML");
			HbWidget* widgetLocation = qobject_cast<HbWidget *> (
					editorLocationDocLoader.findWidget(CALEN_EDITOR_LOCATION));

			mLocationLineEdit = qobject_cast<HbLineEdit *>( 
									editorLocationDocLoader.findWidget(
											CALEN_EDITOR_LOCATION_LINEEDIT));
			mLocationLineEdit->setMinRows(1);
			mLocationLineEdit->setMaxRows(4);
			mLocationPushButton = qobject_cast<HbPushButton*>(
									editorLocationDocLoader.findWidget(
											CALEN_EDITOR_LOCATION_PUSHBUTTON));
			mLocationPushButton->setIcon( HbIcon("qtg_mono_location"));
			
			connect(mLocationPushButton, SIGNAL(clicked()), this, 
												SLOT(launchLocationPicker()));
			connect(mLocationLineEdit, SIGNAL(textChanged(const QString)),
						this, SLOT(handleLocationTextChange(const QString)));
						
			connect(mLocationLineEdit, SIGNAL(editingFinished()),
			            this, SLOT(handleEditingFinished()));
			
			return widgetLocation;
		}
		
		case RepeatUntilOffset:
		{
		mRepeatUntilWidget = new HbPushButton(this);
		return mRepeatUntilWidget;
		}
				
		case ReminderTimeOffset:
		{
			mReminderTimeWidget = new HbPushButton(this);
			return mReminderTimeWidget;
		}
		
		default:
			return 0;
	}
}
/*!
	launch the location picker application using QT highway with the required service
*/

void CalenEditorCustomItem::launchLocationPicker()
{
	XQApplicationManager *appManager = new XQApplicationManager();

    XQAiwRequest *request = appManager->create("com.nokia.symbian", "ILocationPick", "pick()", true);
    if( request )
    {
		QVariant retValue;
		if( request->send( retValue ) )
		{
			setSelectedLocation(retValue);
		}
	}
}
/*!
	set the selected location from the picker to the line edit widget 
	and notify the other observers.
*/
void CalenEditorCustomItem::setSelectedLocation( QVariant &aValue )
{
	QLocationPickerItem selectedLocation = aValue.value<QLocationPickerItem>();
	if( selectedLocation.mIsValid )
    {
		QString locationString;
		if( selectedLocation.mStreet.size() )
		{
			locationString.append(selectedLocation.mStreet);
			locationString.append(',');
		}
		if( selectedLocation.mCity.size() )
		{
			locationString.append(selectedLocation.mCity);
			locationString.append(',');
		}
		locationString.append(selectedLocation.mCountry);
		emit locationTextChanged(locationString, selectedLocation.mLatitude, selectedLocation.mLongitude);
		mLocationLineEdit->setText(locationString );
    }
}

/*!
	Populates the date and time in proper formats on the buttons
 */
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

/*!
	Sets the location on the button
 */
void CalenEditorCustomItem::populateLocation(QString location )
{
	mLocationLineEdit->setText( location );
}

/*!
	Sets the date range
 */
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

/*!
	Sets the time range
 */
void CalenEditorCustomItem::setTimeRange(QTime start, QTime end)
{
	mMaxTime = start;
	mMinTime = end;
}

/*!
	Enables the date button
 */
void CalenEditorCustomItem::enableDateButton(bool value)
{
	mPushButtonDate->setEnabled(value);
}

/*!
	Handles the location change
 */
void CalenEditorCustomItem::handleLocationTextChange(QString location)
{
	emit locationTextChanged(location);
}

/*!
	Handles the location editing finished
 */
void CalenEditorCustomItem::handleEditingFinished()
{
    emit locationEditingFinished();
}

/*!
	Launches the date picker
 */
void CalenEditorCustomItem::handleDate()
{
	// Create a popup with datepicker for the user to select date.
	HbDialog *popUp = new HbDialog();
	popUp->setDismissPolicy(HbDialog::NoDismiss);
	popUp->setTimeout(HbDialog::NoTimeout);
	popUp->setAttribute( Qt::WA_DeleteOnClose, true );
	
	// Set the proper heading
	if (mIsFromItem) {
		popUp->setHeadingWidget(
						new HbLabel(hbTrId("txt_calendar_title_start_date")));
	}else {
		popUp->setHeadingWidget(
						new HbLabel(hbTrId("txt_calendar_title_end_date")));
	}
	// Create a date picker
	if(mDatePicker) {
		mDatePicker = NULL;
	}
	mDatePicker = new HbDateTimePicker(mDate, popUp);
	mDatePicker->setMinimumDate(mMinDate);
	mDatePicker->setMaximumDate(mMaxDate);
	mDatePicker->setDate(mDate);
	popUp->setContentWidget(mDatePicker);
	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"), popUp);
	popUp->addAction(okAction);
	connect(okAction, SIGNAL(triggered()), this, SLOT(saveDate()));
	popUp->addAction(new HbAction(hbTrId("txt_common_button_cancel"), popUp));
	popUp->open();
}

/*!
	Launches the time picker
 */
void CalenEditorCustomItem::handleTime()
{
	// Create a popup with time picker for the user to select time.
	HbDialog *popUp = new HbDialog();
	popUp->setDismissPolicy(HbDialog::NoDismiss);
	popUp->setTimeout(HbDialog::NoTimeout);
	popUp->setAttribute( Qt::WA_DeleteOnClose, true );
	
	// Set the proper heading
	if (mIsFromItem) {
		popUp->setHeadingWidget(
					new HbLabel(hbTrId("txt_calendar_title_start_time")));
	}else {
		popUp->setHeadingWidget(
					new HbLabel(hbTrId("txt_calendar_title_end_time")));
	}
	// Create a time picker.
	if(mTimePicker) {
		mTimePicker = NULL;
	}
	mTimePicker = new HbDateTimePicker(mTime, popUp);
	
	if(mLocale.timeStyle() == HbExtendedLocale::Time12) {
		mTimePicker->setDisplayFormat("hh:mm ap");	
	}else {
		mTimePicker->setDisplayFormat("hh:mm");
	}
	mTimePicker->setTime(mTime);
	popUp->setContentWidget(mTimePicker);

	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"), popUp);
	popUp->addAction(okAction);
	connect(okAction, SIGNAL(triggered()), this, SLOT(saveTime()));
	popUp->addAction(new HbAction(hbTrId("txt_common_button_cancel"), popUp));
	popUp->open();
}

/*!
	Save the start date of the event.
 */
void CalenEditorCustomItem::saveDate()
{
	mDate = mDatePicker->date(); 
	if (mDate.isValid()) {
		mPushButtonDate->setText(mLocale.format(mDate, 
		                                           r_qtn_date_usual_with_zero));
		QDateTime dateTime(mDate,mTime);
		emit dateTimeUpdated(dateTime);
		}
}

/*!
	Save the start time of the event.
 */
void CalenEditorCustomItem::saveTime()
{
	mTime = mTimePicker->time();
	if (mTime.isValid()) {
		mPushButtonTime->setText(mLocale.format(mTime, 
									r_qtn_time_usual_with_zero));

		QDateTime dateTime(mDate,mTime);
		emit dateTimeUpdated(dateTime);
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
	
	// Store the time
    mTime = fromDateTime.time();
}

/*!
	Disable Date field.
 */
void CalenEditorCustomItem::disableFromToDateField()
{
	// disable the date field.
	mPushButtonDate->setEnabled(false);
}

/*!
	Enable/Desable ToTime filed and update time value
 */
void CalenEditorCustomItem::enableToTimeFieldAndSetTime(bool enableTimeFiles, QDateTime toDateTime)
{
	// Set ToTime filed Read-Only/Editable 
	mPushButtonTime->setEnabled(enableTimeFiles);
	
	// Set ToTime in Editor
	mPushButtonTime->setText(mLocale.format(
							toDateTime.time(),r_qtn_time_usual_with_zero));
	
	// Store the time
	mTime = toDateTime.time();
}

/*!
   OverRide from DataFrom
 */
bool CalenEditorCustomItem::canSetModelIndex(const QModelIndex &index) const
{
    HbDataFormModelItem::DataItemType itemType = 
        static_cast<HbDataFormModelItem::DataItemType>(
        index.data(HbDataFormModelItem::ItemTypeRole).toInt());

    if(itemType == CustomWidgetFrom || itemType == CustomWidgetTo || itemType == RepeatUntilOffset 
			|| itemType == CustomWidgetLocation || itemType == ReminderTimeOffset ) {
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
    	
    	case ReminderTimeOffset:
    	{
    		mReminderTimeWidget->setText(modelItem->contentWidgetData("text").toString());
    	}
    	break;
    }
}

QDateTime CalenEditorCustomItem::getDateTime()
{
    return QDateTime(mDate, mTime);
}

Q_IMPLEMENT_USER_METATYPE(QLocationPickerItem)

// End of file	--Don't remove this.
