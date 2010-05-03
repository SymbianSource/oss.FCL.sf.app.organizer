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
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbdataform.h>

// User Includes
#include "caleneditorreminderfield.h"

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
	 mCalenEditorModel(model)
{
	mReminderItem = new HbDataFormModelItem();
	mReminderItem->setType(HbDataFormModelItem::ComboBoxItem);
	mReminderItem->setData(HbDataFormModelItem::LabelRole,
						   hbTrId("txt_calendar_setlabel_alarm"));
	// Create the remaindar choices
	QStringList remaindarChoices;
	remaindarChoices << hbTrId("txt_calendar_setlabel_alarm_val_off")
			<< hbTrId("txt_calendar_setlabel_alarm_val_before_ln_minutes", 5)
			<< hbTrId("txt_calendar_setlabel_alarm_val_before_ln_minutes", 10)
			<< hbTrId("txt_calendar_setlabel_alarm_val_before_ln_minutes", 15)
			<< hbTrId("txt_calendar_setlabel_alarm_val_before_ln_minutes", 30)
			<< hbTrId("txt_calendar_setlabel_alarm_val_before_ln_hours", 1)
			<< hbTrId("txt_calendar_setlabel_alarm_val_before_ln_hours", 2);

	mReminderItem->setContentWidgetData(QString("items"), remaindarChoices);

	// Build the hash map for the reminder.
	mReminderHash[0] = 0; // OFF.
	mReminderHash[1] = 5;
	mReminderHash[2] = 10;
	mReminderHash[3] = 15;
	mReminderHash[4] = 30;
	mReminderHash[5] = 60;
	mReminderHash[6] = 120;	
	
	// Add it to the model
	mCalenEditorModel->appendDataFormItem( mReminderItem,
										mCalenEditorModel->invisibleRootItem());
}

/*!
	 Destructor
 */
CalenEditorReminderField::~CalenEditorReminderField()
{
	// Nothing Yet
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
	 Removes reminder item frm the model
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
	// Set the default reminder value to 15 minutes 
	if (newEntry) {
		mReminderItem->setContentWidgetData("currentIndex", 3);
		// Save the reminder alarm for the entry
		AgendaAlarm reminder;
		reminder.setTimeOffset(mReminderHash.value(3));
		reminder.setAlarmSoundName(QString(" "));
		// Set the reminder to the entry as well as original entry.
		mCalenEditor->editedEntry()->setAlarm(reminder);
		mCalenEditor->originalEntry()->setAlarm(reminder);
	} else {
		if (mCalenEditor->editedEntry()->alarm().isNull()) {
			// Alarm is set off
			mReminderItem->setContentWidgetData("currentIndex", 0);
		} else {
			// Get the reminder offset value.
			int reminderOffset = mCalenEditor->editedEntry()->alarm().timeOffset();
			// Get the index value for the reminder combo box from the hash 
			// table.
			int index = mReminderHash.key(reminderOffset);
			mReminderItem->setContentWidgetData("currentIndex", index);
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
	// If value for the index in hash table is 0 i.e reminder is "OFF",
	// then dont do anything only set the default constructed reminder to
	// the entry which is Null.
	if (mReminderHash.value(index)) {
		// If not zero then set the reminder offset
		// value to the entry.
		reminder.setTimeOffset(mReminderHash.value(index));
		reminder.setAlarmSoundName(QString(" "));
	} else {
		// Construct the default alarm which is NULL
		reminder = AgendaAlarm();
	}
	// Set the reminder to the entry.
	mCalenEditor->editedEntry()->setAlarm(reminder);
	mCalenEditor->addDiscardAction();
}

/*!
	 Returns the mode index of the reminder item
	 \return Mode index of the reminder item
 */
QModelIndex CalenEditorReminderField::modelIndex()
{
	return mCalenEditorModel->indexFromItem(mReminderItem);
}
// End of file	--Don't remove this.
