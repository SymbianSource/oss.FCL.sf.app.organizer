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
* Definition file for NotesTodoEditor class.
*
*/

// System includes
#include <QDateTime>
#include <HbApplication>
#include <HbLineEdit>
#include <HbMainWindow>
#include <HbNotificationDialog>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbDataFormViewItem>
#include <HbInstance>
#include <HbView>
#include <HbMenu>
#include <HbAction>
#include <HbComboBox>
#include <HbExtendedLocale>
#include <HbDateTimePicker>
#include <HbLabel>
#include <HbPushButton>
#include <HbCheckbox>
#include <HbDialog>
#include <HbGroupBox>
#include <HbMessageBox>

// User includes
#include "notestodoeditor.h"
#include "notestodoeditorcustomitem.h"
#include "noteseditor_p.h"
#include "noteseditorcommon.h"
#include "noteseditordocloader.h"
#include "agendaentry.h"

/*!
	\class NotesTodoEditor

	This class is responsible for editing to-dos. This is friend of
	NotesEditorPrivate and it handles all the cases related to to-dos.
 */

/*!
	Default Constructor.

	\param owner Of type NotesEditorPrivate.
	\param parent The parent.
 */
NotesTodoEditor::NotesTodoEditor(
		NotesEditorPrivate* owner, QObject *parent)
:QObject(parent),
 mOwner(owner),
 mReminderEnabler(0),
 mReminderItem(0),
 mDescriptionItemIndex(5),
 mDiscardChangesActive(false)
{
	mDocLoader = new NotesEditorDocLoader;
	Q_ASSERT(mDocLoader);

	bool success;
	// Load the to-do editor docml file.
	mDocLoader->load(TODO_EDITOR_DOCML, &success);
	Q_ASSERT(success);

	// Now load the view and the contents.
	mEditor = static_cast<HbView *> (
			mDocLoader->findWidget(TODO_EDITOR_VIEW));
	Q_ASSERT(mEditor);

	mSubHeading = static_cast<HbGroupBox *> (
			mDocLoader->findWidget(TODO_EDITOR_SUB_HEADING));
	Q_ASSERT(mSubHeading);

	mDataForm = static_cast<HbDataForm *> (
			mDocLoader->findWidget(TODO_EDITOR_DATA_FORM));

	// Initialize the data form model.
	initFormModel();

	// Add menu actions.
	addMenu();

	// Add the custiom widget to the form.
	NotesTodoEditorCustomItem *customItem =
			new NotesTodoEditorCustomItem(mOwner, mDataForm);
	QList <HbAbstractViewItem*> prototypes = mDataForm->itemPrototypes();
	prototypes.append(customItem);
	mDataForm->setItemPrototypes(prototypes);
}

/*!
	Destructor.
 */
NotesTodoEditor::~NotesTodoEditor()
{
	if (mFormModel) {
		delete mFormModel;
	}

	mDocLoader->reset();
	delete mDocLoader;
}

/*!
	Updates the summary text, as entered by the user.
 */
void NotesTodoEditor::updateSummaryText(const QString &text)
{
	mOwner->mModifiedNote.setSummary(text);
	addDiscardChangesAction();
}

/*!
	 updates the priority of the to-do entry

	 \param index The index of the priority item selected by the user
 */
void NotesTodoEditor::updatePriority(int index)
{
	mOwner->mModifiedNote.setPriority(index + 1);
	addDiscardChangesAction();
}

/*!
	Updates the description of the to-do entry.

	\param text The description entered by the user.
 */
void NotesTodoEditor::updateDescription(const QString &text)
{
	mOwner->mModifiedNote.setDescription(text);
	addDiscardChangesAction();
}

/*!
	Check for the reminder field entered by the user.
	If reminder is not enabled it simply returns with true value.

	\return bool Sends the value after validation of alarm vlaue.
 */
bool NotesTodoEditor::validAlarmSet()
{
	bool alarmSet(true);

	// Sanity Check.
	if (mReminderItem) {
		// Get the reminder dateandtime set by user from date and time picker.
		// Get the reminder date.
		QString entryAlarmTimeText =
				mReminderItem->contentWidgetData("alarmTime").toString();
		QTime entryAlarmTime;
		entryAlarmTime = entryAlarmTime.fromString(
				entryAlarmTimeText, mOwner->timeFormatString());
		// Get the reminder time.
		QString entryAlarmDateText =
				mReminderItem->contentWidgetData("alarmDate").toString();
		QDate entryAlarmDate;
		entryAlarmDate = entryAlarmDate.fromString(
				entryAlarmDateText, mOwner->dateFormatString());
		QDateTime entryAlarmDateTime;
		if (entryAlarmTime.isValid()) {
			entryAlarmDateTime.setDate(entryAlarmDate);
			entryAlarmDateTime.setTime(entryAlarmTime);
		}else {
			HbNotificationDialog::launchDialog(tr("Alarm time is not valid"));
			alarmSet = false;
			return alarmSet;
		}

		// Get the to-do entry due date and time.
		// Get the due date.
		QString entryDueDateText =
				mDueDateItem->contentWidgetData("text").toString();
		QDate entryDueDate;
		entryDueDate = entryDueDate.fromString(
				entryDueDateText, mOwner->dateFormatString());
		// Time is considered as 12 am for todo entries.
		QTime entryDueTime (0, 0, 0);
		QDateTime entryDueDateTime;
		if (entryDueTime.isValid()) {
			entryDueDateTime.setDate(entryDueDate);
			entryDueDateTime.setTime(entryDueTime);
		}else {
			HbNotificationDialog::launchDialog(tr("Alarm date is not valid"));
			alarmSet = false;
			return alarmSet;
		}

		// Check whether alarm time doesn't exceed due date
		// nor should be less than current time.
		if (entryAlarmDateTime > entryDueDateTime.addDays(1)) {
			HbNotificationDialog::launchDialog(
				tr("Alarm is set for later than entry starts"));
			alarmSet = false;
		}else if (entryAlarmDateTime.date() < QDate::currentDate()) {
			// if alarm date is set before the current date
			HbNotificationDialog::launchDialog(tr("Alarm already passed"));
			alarmSet = false;
		} else if( (entryAlarmDateTime.date() == QDate::currentDate()) &&
				entryAlarmDateTime.time() < QTime::currentTime() ) {
			// if alarm date is current date and alarm time is set before the
			// current time
			HbNotificationDialog::launchDialog(tr("Alarm already passed"));
			alarmSet = false;
		}else {
			uint entryDueTimeinSec = entryDueDateTime.toTime_t();
			uint entryAlarmTimeinSec = entryAlarmDateTime.toTime_t();
			int32_t diffinSec = entryDueTimeinSec - entryAlarmTimeinSec;
			AgendaAlarm newAlarm;
			newAlarm.setTimeOffset(diffinSec / 60);
			mOwner->mModifiedNote.setAlarm(newAlarm);
			alarmSet = true;
		}
	} else {
		AgendaAlarm dummyAlarm;
		mOwner->mModifiedNote.setAlarm(dummyAlarm);
		return alarmSet;
	}
	return alarmSet;
}

/*!
	Adds discard changes option item to the menu
 */
void NotesTodoEditor::addDiscardChangesAction()
{
	if(!mDiscardChangesActive) {
		mDiscardAction = new HbAction(hbTrId("txt_notes_opt_discard_changes"));

		mEditor->menu()->addAction(mDiscardAction);

		connect(
				mDiscardAction, SIGNAL(triggered()),
				this, SLOT(handleDiscardChangesAction()));

		mDiscardChangesActive = true;
	}
}

/*!
	Displays the to-do editor and populates the data with the given AgendaEntry.

	\param entry The entry from which data has to be read.
 */
void NotesTodoEditor::execute(AgendaEntry entry)
{
	Q_UNUSED(entry)

	HbMenu *viewMenu = mEditor->menu();
	if (!mDescriptionAction) {
		mDescriptionAction = new HbAction;
	}

	// Update the description menu item
	if (entry.description().length()) {
		// Insert description item
		insertDescriptionItem();

		mDescriptionAction->setText(hbTrId("txt_notes_opt_remove_description"));
		viewMenu->addAction(mDescriptionAction);

		connect(
				mDescriptionAction, SIGNAL(triggered()),
				this, SLOT(handleRemoveDescriptionAction()));
	} else {
		mDescriptionAction->setText(hbTrId("txt_notes_opt_add_description"));
		viewMenu->addAction(mDescriptionAction);

		connect(
				mDescriptionAction, SIGNAL(triggered()),
				this, SLOT(handleAddDescriptionAction()));
	}

	// Update the sub heading based on the new /existing entry
	if (mOwner->mNewEntry) {
		mSubHeading->setHeading(hbTrId("txt_notes_subhead_new_todo"));
	} else {
		mSubHeading->setHeading(hbTrId("txt_notes_subhead_todo"));
	}

	// Store the current view and set our view as the current view.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	HbAction *action = new HbAction(Hb::BackAction);
	mEditor->setNavigationAction(action);
	connect(
			action, SIGNAL(triggered()),
			this, SLOT(saveTodo()));
	window->addView(mEditor);
	window->setCurrentView(mEditor);
}

/*!
	Displays the to-do editor for creating new to-do entry

	\param entry The entry from which data has to be read.
 */
void NotesTodoEditor::create(AgendaEntry entry)
{
	Q_UNUSED(entry)

}
/*!
	Initializes the data form model.
 */
void NotesTodoEditor::initFormModel()
{
	// Delete the model if already there.
	if (mFormModel) {
		delete mFormModel;
		mDataForm->setModel(0);
	}

	// Construct the form model.
	mFormModel = new HbDataFormModel(this);

	// Add items to the model.
	insertSubjectItem();
	insertDueDateItem();
	insertReminderToggle();
	// Need to display reminderitem while editing a ToDo entry having alarm.
	if (!mOwner->mModifiedNote.alarm().isNull()) {
		handleReminderItem(Qt::Checked);
	}
	insertPriorityItem();

	// Set the model to the form.
	mDataForm->setModel(mFormModel);
}

/*!
	Inserts the subject item into the model.
 */
void NotesTodoEditor::insertSubjectItem()
{
	mSubjectItem = mFormModel->appendDataFormItem(
			HbDataFormModelItem::TextItem,
			QString(""), mFormModel->invisibleRootItem());

	mSubjectItem->setContentWidgetData(
			QString("text"), mOwner->mModifiedNote.summary());

	mDataForm->addConnection(
			mSubjectItem , SIGNAL(textChanged(const QString &)),
			this, SLOT(updateSummaryText(const QString &)));
}

/*!
	Inserts the due date item into the model.
 */
void NotesTodoEditor::insertDueDateItem()
{
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType>
			(HbDataFormModelItem::CustomItemBase + DueDateItemOffset);

	mDueDateItem = mFormModel->appendDataFormItem(
			itemType, hbTrId("txt_notes_formlabel_due_date"),
			mFormModel->invisibleRootItem());

	QString dueDateText;

	if (mOwner->mModifiedNote.endTime().isNull()) {
		// Set the current date.
		dueDateText = QDate::currentDate().toString(
				mOwner->dateFormatString());
	} else {
		// Read the due date from agenda entry
		dueDateText =
			mOwner->mModifiedNote.endTime().date().toString(
					mOwner->dateFormatString());
	}
	mDueDateItem->setContentWidgetData("text",dueDateText);
}
/*!
	 Inserts the reminder enabler item into the model.
 */
void NotesTodoEditor::insertReminderToggle()
{
	mReminderEnabler = mFormModel->appendDataFormItem(
			HbDataFormModelItem::CheckBoxItem,
			tr(""), mFormModel->invisibleRootItem());

	mReminderEnabler->setContentWidgetData(
			QString("text"), QString(hbTrId("txt_notes_formlabel_alarm")));

	if (!mOwner->mModifiedNote.alarm().isNull()) {
		mReminderEnabler->setContentWidgetData("checkState",Qt::Checked);
	} else {
		mReminderEnabler->setContentWidgetData("checkState",Qt::Unchecked);
	}

	mDataForm->addConnection(
			mReminderEnabler, SIGNAL(stateChanged(int)),
			this, SLOT(handleReminderItem(int)));
}

/*!
	Handle insertion/deletion of reminder item in todo editor.
 */
void NotesTodoEditor::handleReminderItem(int checked)
{
	if (Qt::Checked == checked) {
		HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType>
		(HbDataFormModelItem::CustomItemBase + AlarmDateItemOffset);

		mReminderItem = mFormModel->insertDataFormItem(
				3,itemType,hbTrId("txt_notes_formlabel_alarm_date_and_time"));

		// Set alarm time to the buttton.
		QString alarmTimeText;
		if (mOwner->mModifiedNote.alarm().isNull()) {
			QTime alarmTime(8, 0, 0, 0);
			// Below statement can be used to format the string but there is no
			// reverse way meaning from "text to time" found to do this..

			alarmTimeText =
					alarmTime.toString(
							mOwner->timeFormatString());
		} else {
			int32_t timeOffsetInSec =
					mOwner->mModifiedNote.alarm().timeOffset() * 60;
			uint reminderTimeinSec =
					mOwner->mModifiedNote.endTime().toTime_t()- timeOffsetInSec;
			QDateTime reminderTime;
			reminderTime.setTime_t(reminderTimeinSec);

			alarmTimeText =
					reminderTime.time().toString(
							mOwner->timeFormatString());
		}
		mReminderItem->setContentWidgetData(
				"alarmTime",alarmTimeText);

		// Set alarm date to the buttton.
		QString alarmDateText;
		// Value is set based on convertion of a note to ToDo editor
		// or editing a ToDo editor(having alarm set) for time picker.
		if (mOwner->mModifiedNote.alarm().isNull()) {
			alarmDateText =
					mOwner->mModifiedNote.endTime().date().toString(
							mOwner->dateFormatString());
		} else {
			int64_t offsetInSec =
					mOwner->mModifiedNote.alarm().timeOffset()* 60;
			uint reminderTimeinSec =
					mOwner->mModifiedNote.endTime().toTime_t()- offsetInSec;
			QDateTime reminderTime;
			reminderTime.setTime_t(reminderTimeinSec);

			alarmDateText = reminderTime.date().toString(
					mOwner->dateFormatString());
		}

		mReminderItem->setContentWidgetData("alarmDate", alarmDateText);

		// Add discardChanges Action if reminder field is modified
		addDiscardChangesAction();

	} else if(Qt::Unchecked == checked) {
		if (mReminderItem) {
			mFormModel->removeItem(mReminderItem);

			// Set dummy alarm.
			AgendaAlarm noAlarm;
			mOwner->mModifiedNote.setAlarm(noAlarm);

			mReminderItem = 0;
		}
	}
}

/*!
	Inserts the priority item in to the model
 */
void NotesTodoEditor::insertPriorityItem()
{
	mPriorityItem = mFormModel->appendDataFormItem(
			HbDataFormModelItem::ComboBoxItem,
			hbTrId("txt_notes_setlabel_priority"),
			mFormModel->invisibleRootItem());

	QStringList priorityList;
	priorityList << hbTrId("txt_notes_setlabel_priority_val_high")
				 << hbTrId("txt_notes_setlabel_priority_val_normal")
				 << hbTrId("txt_notes_setlabel_priority_val_low");
	mPriorityItem->setContentWidgetData(QString("items"), priorityList);

	int priority = mOwner->mModifiedNote.priority();
	// If priority is not set,set to Normal.
	if (PriorityUnKnown == priority ) {
		priority = NormalPriority;
	}
	mPriorityItem->setContentWidgetData(
			QString("currentIndex"), priority-1);

	mDataForm->addConnection(
			mPriorityItem, SIGNAL(currentIndexChanged(int)),
			this, SLOT(updatePriority(int)));
}

/*!
	Inserts the description item to the model
 */
void NotesTodoEditor::insertDescriptionItem()
{
	mDescriptionItem = mFormModel->appendDataFormItem(
			HbDataFormModelItem::TextItem,
			hbTrId("txt_notes_formlabel_val_description"),
			mFormModel->invisibleRootItem());

	mDescriptionItemIndex =
			mFormModel->indexFromItem(mDescriptionItem).row();

	mDescriptionItem->setContentWidgetData(
			QString("text"), mOwner->mModifiedNote.description());

	mDataForm->addConnection(
			mDescriptionItem, SIGNAL(textChanged(const QString)),
			this, SLOT(updateDescription(const QString)));
}

/*!
	Inserts actions to the view menu.
 */
void NotesTodoEditor::addMenu()
{
	mDeleteAction = mEditor->menu()->addAction(hbTrId("txt_common_opt_delete"));
	connect(
			mDeleteAction, SIGNAL(triggered()),
			this, SLOT(handleDeleteAction()));
}

/*!
	Saves a to-do entry
 */
void NotesTodoEditor::saveTodo()
{
	// Here we check if the user has set the appropriate alarm time.
	// And save only if its fine to do so. Else we note a discrete notification
	// asking the user to check for the alarm time.
	bool status = false;
	if (validAlarmSet()) {
		status = mOwner->saveTodo();
		HbAction *action = qobject_cast<HbAction *> (QObject::sender());

		// Remove the to-do editor view
		HbMainWindow *window = hbInstance->allMainWindows().first();
		window->removeView(mEditor);

		// Cleanup.
		disconnect(
				action, SIGNAL(triggered()),
				this, SLOT(saveTodo()));

		mOwner->editingCompleted(status);
	}
}

/*!
	Closes the to-do editor and does nothing else. It removes the view off the
	main window's list of views.
 */
void NotesTodoEditor::close()
{
	HbMainWindow *window = hbInstance->allMainWindows().first();
	// Now close the editor.
	window->removeView(mEditor);

}

/*!
	Handles the case when "Add description" action is triggered from the
	menu.
 */
void NotesTodoEditor::handleAddDescriptionAction()
{
	// Now we add the description item here.
	insertDescriptionItem();

	// Update the text in the menu action.
	mDescriptionAction->setText(hbTrId("txt_notes_opt_remove_description"));

	disconnect(
			mDescriptionAction, SIGNAL(triggered()),
			this, SLOT(handleAddDescriptionAction()));

	connect(
			mDescriptionAction, SIGNAL(triggered()),
			this, SLOT(handleRemoveDescriptionAction()));
}

/*!
	Handles the case when "Remove description" action is triggered
	from the menu.
 */
void NotesTodoEditor::handleRemoveDescriptionAction()
{
	// Here we firstset the description text in the entry to empty.
	mOwner->mModifiedNote.setDescription("");

	// Now we remove the Description data form item.
	mFormModel->removeItem(mFormModel->item(mDescriptionItemIndex));

	// Update the text in the menu action.
	mDescriptionAction->setText(hbTrId("txt_notes_opt_add_description"));

	disconnect(
			mDescriptionAction, SIGNAL(triggered()),
			this, SLOT(handleRemoveDescriptionAction()));

	connect(
			mDescriptionAction, SIGNAL(triggered()),
			this, SLOT(handleAddDescriptionAction()));

}

/*!
	Handles "Delete" action when triggered from menu
 */
void NotesTodoEditor::handleDeleteAction()
{
	if (showDeleteConfirmationQuery()) {
	// Delete the to-do entry.
	mOwner->deleteNote();

	// Close the to-do editor.
	close();
	}

}

/*!
	Handles "Discard changes" action when triggered from menu
 */
void NotesTodoEditor::handleDiscardChangesAction()
{
	// Close the to-do editor
	close();

}

/* !
	Show the delete confirmation query.
 */
bool NotesTodoEditor::showDeleteConfirmationQuery()
{
	bool retValue(false);

	HbMessageBox confirmationQuery(HbMessageBox::MessageTypeQuestion);
	confirmationQuery.setDismissPolicy(HbDialog::NoDismiss);
	confirmationQuery.setTimeout(HbDialog::NoTimeout);
	confirmationQuery.setIconVisible(true);

	QString displayText;
	displayText = displayText.append(hbTrId("txt_notes_info_delete_todo_note"));

	confirmationQuery.setText(displayText);

	confirmationQuery.setPrimaryAction(new HbAction(
	    hbTrId("txt_notes_button_dialog_delete"), &confirmationQuery));
	confirmationQuery.setSecondaryAction(new HbAction(
	    hbTrId("txt_common_button_cancel"), &confirmationQuery));
	HbAction *selected = confirmationQuery.exec();
	if (selected == confirmationQuery.primaryAction()) {
		retValue = true;
	}

	return retValue;
}
// End of file	--Don't remove this.
