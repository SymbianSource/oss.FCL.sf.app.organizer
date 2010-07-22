/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Definition of CalenEditorReminderField class.
*
*/


// System Includes
#include <QDate>

#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbdataform.h>
#include <hbdatetimepicker.h>
#include <hbextendedlocale.h>
#include <hbi18ndef.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbaction.h>

// User Includes
#include "caleneditorreminderfield.h"

#define numberOfMinutesInADay 1440
/*!
	\class CalenEditorReminderField
 */
/*!
	Constructor.

	\param parent QObject pointer
 */

CalenEditorReminderField::CalenEditorReminderField(
										CalenEditorPrivate* calenEditor,
										HbDataForm* form, 
										HbDataFormModel* model,
										QObject *parent)
	:QObject(parent),
	 mCalenEditor(calenEditor), 
	 mEditorForm(form), 
	 mCalenEditorModel(model),
	 mCustomReminderTimeItem(NULL),
	 mReminderTimeAdded(false)
{
	mReminderItem = new HbDataFormModelItem();
	mReminderItem->setType(HbDataFormModelItem::ComboBoxItem);
	mReminderItem->setData(HbDataFormModelItem::LabelRole,
						   hbTrId("txt_calendar_setlabel_reminder"));	
	// Add it to the model
	mCalenEditorModel->appendDataFormItem(mReminderItem,
										mCalenEditorModel->invisibleRootItem());
}

/*!
	 Destructor
 */
CalenEditorReminderField::~CalenEditorReminderField()
{
	if(mReminderHash.count()) {
		mReminderHash.clear();
	}
}

/*!
	 Set the reminder choices.
 */
void CalenEditorReminderField::setReminderChoices()
{
	// Create the reminder choices
	QStringList reminderChoices;
	reminderChoices << hbTrId("txt_calendar_setlabel_reminder_val_off")
				<< hbTrId("txt_calendar_setlabel_reminder_val_at_the_start")
				<< hbTrId("txt_calendar_setlabel_reminder_val_15_minutes_befo")
				<< hbTrId("txt_calendar_setlabel_reminder_val_30_minutes_befo")
				<< hbTrId("txt_calendar_setlabel_reminder_val_1_hour_before");

	mReminderItem->setContentWidgetData(QString("items"), reminderChoices);

	// Build the hash map for the reminder.
	mReminderHash[0] = -1; // OFF.
	mReminderHash[1] = 0;
	mReminderHash[2] = 15;
	mReminderHash[3] = 30;
	mReminderHash[4] = 60;
	mReminderItem->setEnabled(true);
}

/*!
	 Adds reminder item to the model
 */
void CalenEditorReminderField::addItemToModel()
{	
	// Add reminder to the model
	mCalenEditorModel->appendDataFormItem( mReminderItem,
				   mCalenEditorModel->invisibleRootItem());
}

/*!
	 Removes reminder item from the model
 */
void CalenEditorReminderField::removeItemFromModel()
{
	mCalenEditorModel->removeItem(modelIndex());
}

/*!
	 Populates reminder item with available choices to the user
	 \param newEntry bool value to indicate if its a new entry
 */
void CalenEditorReminderField::populateReminderItem(bool newEntry)
{
	AgendaAlarm reminder;
	bool pastEvent =  false;
	
	// Set reference date to start date or repeat until date accordingly to 
	// decide whether its a past event or not.
	QDate referenceDate;
	if (!mCalenEditor->editedEntry()->isRepeating() || 
					(mCalenEditor->isEditRangeThisOnly())) {
		referenceDate = mCalenEditor->editedEntry()->startTime().date();
	} else {
		referenceDate = mCalenEditor->editedEntry()->repeatRule().until().date();
	}
	
	if ((referenceDate < QDate::currentDate()) || 
					(referenceDate == QDate::currentDate()
					&& (mCalenEditor->editedEntry()->startTime().time() < 
					QTime::currentTime()))) {
		pastEvent = true;
	}
	
	if (mCalenEditor->isAllDayEvent()) {
		updateReminderChoicesForAllDay(referenceDate);
	} else {
		setReminderChoices();
	}
	// Set the default reminder value to 15 minutes 
	if (newEntry) {
		if (!pastEvent) {
			mReminderItem->setContentWidgetData("currentIndex", 2);
			// Save the reminder alarm for the entry
			reminder.setTimeOffset(mReminderHash.value(2));
			reminder.setAlarmSoundName(QString(" "));
			// Set the reminder to the entry as well as original entry.
			mCalenEditor->editedEntry()->setAlarm(reminder);
			mCalenEditor->originalEntry()->setAlarm(reminder);
		} else {
			mReminderItem->setContentWidgetData("currentIndex", 0);
			mReminderItem->setEnabled(false);
		}
	} else {
		// If the alarm is not null,
		// Check if all day event or not and then set the choices accordingly.
		if (mCalenEditor->editedEntry()->alarm().isNull()) {
			// Alarm is set off
			mReminderItem->setContentWidgetData("currentIndex", 0);
			if(mReminderTimeAdded) {
				removeReminderTimeField();
			}
		} else if (mCalenEditor->editedEntry()->type()
		        != AgendaEntry::TypeEvent) {
			// Get the reminder offset value.
			int reminderOffset =
			        mCalenEditor->editedEntry()->alarm().timeOffset();
			// Get the index value for the reminder combo box from the hash 
			// table.
			int index = mReminderHash.key(reminderOffset);
			mReminderItem->setContentWidgetData("currentIndex", index);
		} else {
			// Insert reminder time field and display entry's reminder time.
			// If past then disable the field.
			if (!mReminderTimeAdded) {
				insertReminderTimeField();
				if (pastEvent) {
					mCustomReminderTimeItem->setEnabled(false);
				}
			}
			QStringList reminderChoicesForAllDay;
			reminderChoicesForAllDay << hbTrId("txt_calendar_setlabel_reminder_val_off") 
					<< hbTrId("txt_calendar_setlabel_reminder_val_on_event_day")
					<< hbTrId("txt_calendar_setlabel_reminder_val_1_day_before")
					<< hbTrId("txt_calendar_setlabel_reminder_val_2_days_before");
			mReminderItem->setContentWidgetData(QString("items"),
			                                    reminderChoicesForAllDay);
			QTime referenceTime(0, 0, 0);
			// Set the appropriate reminder depending on the value of time offset.
			reminder = mCalenEditor->editedEntry()->alarm();
			int offsetInMins = reminder.timeOffset();
			if (offsetInMins < 0 || offsetInMins == 0) {
				mReminderItem->setContentWidgetData("currentIndex", 1);
				mReminderTimeForAllDay = referenceTime.addSecs(-(offsetInMins
						* 60));
			} else if (offsetInMins < numberOfMinutesInADay) {
				mReminderItem->setContentWidgetData("currentIndex", 2);
				mReminderTimeForAllDay = referenceTime.addSecs(-(offsetInMins
						* 60));
			} else {
				mReminderItem->setContentWidgetData("currentIndex", 3);
				offsetInMins %= (24 * 60);
				mReminderTimeForAllDay = referenceTime.addSecs(-(offsetInMins
						* 60));
			}
			setDisplayTime();
		}
		if (pastEvent) {
			mReminderItem->setEnabled(false);
		}
	}
	mEditorForm->addConnection(mReminderItem,
							SIGNAL(currentIndexChanged(int)), this,
							SLOT(handleReminderIndexChanged(int)));
}

/*!
	Triggerd from tapping on reminder item.
	Handles the reminder time change and updates the same in the event.
	\param index The new index chosen in the reminder list.
 */
void CalenEditorReminderField::handleReminderIndexChanged(int index)
{
	AgendaAlarm reminder;
	if (!mCalenEditor->editedEntry()->alarm().isNull()) {
		reminder = mCalenEditor->editedEntry()->alarm();
	}
	// Check whether all day event or not and store appropriately.
	if (!mCalenEditor->isAllDayEvent()) {
		// If value for the index in hash table is -1 i.e reminder is "OFF",
		// then set the default constructed reminder to
		// the entry which is Null.
		if (mReminderHash.value(index) < 0) {
			// Construct the default alarm which is NULL
			reminder = AgendaAlarm();
		} else {
			// If not zero then set the reminder offset
			// value to the entry.
			reminder.setTimeOffset(mReminderHash.value(index));
			reminder.setAlarmSoundName(QString(" "));
		}
	} else {
		QDateTime reminderDateTimeForAllDay; 
		QDateTime
		        startDateTimeForAllDay(
		                               mCalenEditor->editedEntry()->startTime().date(),
		                               QTime(0, 0, 0));
		if (mReminderItem->contentWidgetData("currentIndex") == 0) {
			reminder = AgendaAlarm();
			mCustomReminderTimeItem->setEnabled(false);
		} else {
			int offset = 0;
			if (mReminderItem->contentWidgetData("currentIndex") == 2) {
				offset = 1;
			} else if (mReminderItem->contentWidgetData("currentIndex") == 3) {
				offset = 2;
			}
			if(!mReminderTimeAdded) {
				insertReminderTimeField()
;			}
			// If on same day as that of the event then check if time has been 
			// changed , if changed retain that else set default time.
			if (offset == 0) {
				if(mReminderTimeForAllDay == QTime(18, 0, 0, 0)) {
					mReminderTimeForAllDay.setHMS(8, 0, 0);
					setDisplayTime();
				}
			}
			reminderDateTimeForAllDay.setDate(
					mCalenEditor->editedEntry()->startTime().date().addDays(
																	-offset));
			reminderDateTimeForAllDay.setTime(mReminderTimeForAllDay);
			int seconds =
					reminderDateTimeForAllDay.secsTo(startDateTimeForAllDay);
			int timeOffest = seconds / 60;
			mCustomReminderTimeItem->setEnabled(true);
			reminder.setTimeOffset(timeOffest);
			reminder.setAlarmSoundName(QString(" "));
		}
	}
	// Set the reminder to the entry.
	mCalenEditor->editedEntry()->setAlarm(reminder);
	if(!mCalenEditor->isNewEntry()) {
		mCalenEditor->addDiscardAction();
	}
}

/*!
	 Returns the mode index of the reminder item
	 \return Mode index of the reminder item
 */
QModelIndex CalenEditorReminderField::modelIndex()
{
	return mCalenEditorModel->indexFromItem(mReminderItem);
}

/*!
	 Set reminder off.
 */
void CalenEditorReminderField::setReminderOff()
{
	// Create the remindar choices
	QStringList reminderChoices;
	reminderChoices << hbTrId("txt_calendar_setlabel_reminder_val_off");
	mReminderItem->setContentWidgetData(QString("items"), reminderChoices);
	mReminderItem->setEnabled(false); 
}

/*!
	 Set the default alarm for a new all day event.
 */
void CalenEditorReminderField::setDefaultAlarmForAllDay()
{	
	// Set default alarm if its a new entry.
	if (mCalenEditor->isNewEntry()) {
		AgendaAlarm reminder;
		QDate defaultDate
				(mCalenEditor->editedEntry()->startTime().date().addDays(-1));
		// Set default time.
		mReminderTimeForAllDay.setHMS(18, 0, 0, 0);
		setDisplayTime();
		QDateTime startDateTimeForAllDay(
				mCalenEditor->editedEntry()->startTime().date(), QTime(0, 0));
		QDateTime defaultReminderDateTimeForAllDay
				(defaultDate, mReminderTimeForAllDay);
		int offsetInSecs = 
				defaultReminderDateTimeForAllDay.secsTo(startDateTimeForAllDay);
		int offsetInMins = offsetInSecs/60;
		reminder.setTimeOffset(offsetInMins);
		reminder.setAlarmSoundName(QString(" "));
		// Set the reminder to the entry as well as original entry.
		mCalenEditor->editedEntry()->setAlarm(reminder);
		mCalenEditor->originalEntry()->setAlarm(reminder);
	}
}

/*!
	 Updates the reminder choices for an all day event.
	 \param referenceDate to indicate past or not.
 */
void CalenEditorReminderField::updateReminderChoicesForAllDay(QDate referenceDate)
{
	if (!mReminderTimeAdded){
		insertReminderTimeField();
	}
	QStringList reminderChoicesForAllDay;
	QDate tomorrow = QDate::currentDate().addDays(1);
	QDate theDayAfterTomorrow = QDate::currentDate().addDays(2);
	
	if (referenceDate < QDate::currentDate() || 
			referenceDate == QDate::currentDate()) {
		// Set reminder off for past event.
		reminderChoicesForAllDay << hbTrId("txt_calendar_setlabel_reminder_val_off");
		mReminderItem->setContentWidgetData(QString("items"), 
													reminderChoicesForAllDay);
		mReminderItem->setEnabled(false); 
		removeReminderTimeField();
	} else if (theDayAfterTomorrow < referenceDate || 
			theDayAfterTomorrow == referenceDate) {
		reminderChoicesForAllDay 
				<< hbTrId("txt_calendar_setlabel_reminder_val_off")
				<< hbTrId("txt_calendar_setlabel_reminder_val_on_event_day")
				<< hbTrId("txt_calendar_setlabel_reminder_val_1_day_before")
				<< hbTrId("txt_calendar_setlabel_reminder_val_2_days_before");
		mReminderItem->setEnabled(true);
		mCustomReminderTimeItem->setEnabled(true);
	} else {
		reminderChoicesForAllDay 
					<< hbTrId("txt_calendar_setlabel_reminder_val_off")
					<< hbTrId("txt_calendar_setlabel_reminder_val_on_event_day")
					<< hbTrId("txt_calendar_setlabel_reminder_val_1_day_before");
		mReminderItem->setEnabled(true);
		mCustomReminderTimeItem->setEnabled(true);
	}
	if (mReminderItem->isEnabled()) {
		mReminderItem->setContentWidgetData(QString("items"), 
											reminderChoicesForAllDay);
	}
}

/*!
	 Insert the reminder time field for an all day event.
 */
void CalenEditorReminderField::insertReminderTimeField()
{
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType> (ReminderTimeOffset);
	
	// If all day item is not added then insert at one level before.
	int index = CalenEditorPrivate::ReminderTimeForAllDayItem;
	if(!mCalenEditor->isAllDayFieldAdded()) {
		index = CalenEditorPrivate::ReminderTimeForAllDayItem - 1;
	} 
	mCustomReminderTimeItem = mCalenEditorModel->insertDataFormItem(
							index,
							itemType,
							QString(hbTrId("txt_calendar_setlabel_reminder_time")),
							mCalenEditorModel->invisibleRootItem());
	if (currentReminderIndex() != 1){
		mReminderTimeForAllDay.setHMS(18,0,0,0);
	} else {
		mReminderTimeForAllDay.setHMS(8,0,0,0);
	}
	
	mEditorForm->addConnection(mCustomReminderTimeItem, SIGNAL(clicked()),
	                           this, SLOT(launchReminderTimePicker()));
	setDisplayTime();
	mReminderTimeAdded = true;
}

/*!
	 Set the reminder time selected by the user.
 */
void CalenEditorReminderField::setDisplayTime()
{
	HbExtendedLocale locale = HbExtendedLocale::system();
	QString timeString = locale.format(
			mReminderTimeForAllDay,
			r_qtn_time_usual_with_zero);
	mCustomReminderTimeItem->setContentWidgetData("text", timeString);
}

/*!
	Remove the reminder time field if its not an all day event.
 */
void CalenEditorReminderField::removeReminderTimeField()
{
	mReminderTimeAdded = false;
	if (mCustomReminderTimeItem) {
		QModelIndex reminderIndex =
				mCalenEditorModel->indexFromItem(mCustomReminderTimeItem);
		mCalenEditorModel->removeItem(
				mCalenEditorModel->index(
						reminderIndex.row(), 0));
		mCustomReminderTimeItem = NULL;
	}
}

/*!
	 Launches time picker to select time.
 */
void CalenEditorReminderField::launchReminderTimePicker()
{
	mTimePicker = new HbDateTimePicker(mReminderTimeForAllDay);
	mTimePicker->setTime(mReminderTimeForAllDay);
	HbDialog *popUp = new HbDialog();
	popUp->setDismissPolicy(HbDialog::NoDismiss);
	popUp->setTimeout(HbDialog::NoTimeout);
	popUp->setContentWidget(mTimePicker);
	popUp->setHeadingWidget( new HbLabel(
			hbTrId("Reminder Time")));
	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"));
	popUp->addAction(okAction);
	connect(okAction, SIGNAL(triggered()), this, SLOT(setReminderTimeForAllDay()));
	connect(okAction, SIGNAL(triggered()), popUp, SLOT(close()));
	popUp->addAction(new HbAction(hbTrId("txt_common_button_cancel"),
	                                      popUp));
	popUp->open();
}

/*!
	 Set the reminder time chosen.
 */
void CalenEditorReminderField::setReminderTimeForAllDay()
{
	mReminderTimeForAllDay = mTimePicker->time();
	if (mReminderTimeForAllDay.isValid()) {
		// Change the time displayed to that selected by the user.
		setDisplayTime();
		handleReminderIndexChanged(currentReminderIndex());
	}	
}

/*!
	 Checks if reminder field is enabled or not.
 */
bool CalenEditorReminderField::isReminderFieldEnabled()
{
	return mReminderItem->isEnabled();
}

/*!
	 Returns the number of items present in the reminder option.
 */
int CalenEditorReminderField::reminderItemsCount()
{
	QVariant strings = mReminderItem->contentWidgetData("items");
	QStringList stringList(strings.toStringList());
	int count = stringList.count();
	return count;
}

/*!
	 Returns the current chosen index.
 */
int CalenEditorReminderField::currentReminderIndex()
{
	QVariant countVariant = mReminderItem->contentWidgetData("currentIndex");
	int index = countVariant.toInt();
	return index;
}

/*!
	 Sets the chosen index as current index.
	 /param index indicates the idex value to be set.
 */
void CalenEditorReminderField::setCurrentIndex(int index)
{
	mReminderItem->setContentWidgetData("currentIndex", index);
}

/*!
	 Disables the reminder time field.
 */
void CalenEditorReminderField::disableReminderTimeField()
{
	if (mReminderTimeAdded) {
		mCustomReminderTimeItem->setEnabled(false);
	}
}

/*!
	 Checks if reminder time field is added or not.
 */
bool CalenEditorReminderField::isReminderTimeForAllDayAdded()
{
	return mReminderTimeAdded;
}

// End of file	--Don't remove this.
