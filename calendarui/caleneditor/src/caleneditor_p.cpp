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
 * Description: Definition file for class CalenEditorPrivate.
 *
 */

// System includes
#include <QObject>
#include <QTimer>
#include <QGraphicsLinearLayout>
#include <qtranslator.h>
#include <qapplication.h>
#include <hbdataform.h>
#include <hbmainwindow.h>
#include <hbinstance.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>
#include <hbaction.h>
#include <hbmenu.h>
#include <hbgraphicsscene.h>
#include <hblineedit.h>
#include <hbcheckbox.h>
#include <hbcombobox.h>
#include <hbpushbutton.h>
#include <hbdatetimepicker.h>
#include <hbmessagebox.h>
#include <hblabel.h>
#include <hbview.h>
#include <hbextendedlocale.h>
#include <hbi18ndef.h>
#include <qdatetime.h>
#include <HbMessageBox>
#include <hbgroupbox.h>
#include <hbapplication.h>
#include <hbradiobuttonlist.h>
#include <hbnotificationdialog.h>

// User includes
#include <CalenLauncher>
#include <agendaentry.h>
#include <agendautil.h>

#include "caleneditor_p.h"
#include "caleneditor.h"
#include "caleneditor_p.h"
#include "caleneditorcustomitem.h"
#include "calendateutils.h"
#include "caleneditordocloader.h"
#include "caleneditorcommon.h"
#include "calendateutils.h"

/*!
	\class CalenEditorPrivate
	This object loads the calendar editor view. The view is added to the top
	of the HbMainWindow.
 */
/*!
	\enum CalenEditorPrivate::Error
	This enum defines the possible errors of CalenEditorPrivate
 */
/*!
	\var CalenEditorPrivate::Error CalenEditorPrivate::CalenEditorErrorNone
	No error.
 */
/*!
	\var CalenEditorPrivate::Error CalenEditorPrivate::CalenEditorErrorAlarmTimeLaterThanNote
	Alarm time is later than entry error.
 */
/*!
	\var CalenEditorPrivate::Error CalenEditorPrivate::CalenEditorErrorAlarmTimePast
	The time for the entry alarm has already expired error.
 */
/*!
	\var CalenEditorPrivate::Error
	CalenEditorPrivate::CalenEditorErrorAlarmDateTooManyDaysBeforeNote
	Alarm date is 1000 days or more past error.
 */
/*!
	\var CalenEditorPrivate::Error
	CalenEditorPrivate::CalenEditorErrorRepeatUntilEarlierThanNote
	Repeat until date earlier than start date error.
 */
/*!
	\var CalenEditorPrivate::Error
	CalenEditorPrivate::CalenEditorErrorDurationGreaterThanRepeatInterval
	The duration of the entry is greater than repeat type interval error.
 */
/*!
	\var CalenEditorPrivate::Error
	CalenEditorPrivate::CalenEditorErrorStopTimeEarlierThanStartTime
	End date/time is earlier than start error.
 */
/*!
	\var CalenEditorPrivate::Error CalenEditorPrivate::CalenEditorErrorDbConflictEntryDeleted
	Entry was deleted by other application error.
 */
/*!
	\var CalenEditorPrivate::Error CalenEditorPrivate::CalenEditorErrorDbConflictEntryModified
	Entry was modified by other application error.
 */
/*!
	\var CalenEditorPrivate::Error CalenEditorPrivate::CalenEditorErrorOutOfSequence
	Moving a meeting out of sequence error.
 */
/*!
	\var CalenEditorPrivate::Error CalenEditorPrivate::CalenEditorErrorOverlapsExistingInstance
	Attempting to move a meeting to overlap another instance of the meeting error.
 */
/*!
	\var CalenEditorPrivate::Error
	CalenEditorPrivate::CalenEditorErrorInstanceAlreadyExistsOnThisDay
	Attempting to move an instance of a meeting to a day when an instance 
	already exists error.
 */
/*!
	\enum CalenEditorPrivate::Action
	This enum defines the actions on entry
 */
/*!
	\var CalenEditorPrivate::Action CalenEditorPrivate::ActionNothing
	Nothing to do with entry 
 */
/*!
	\var CalenEditorPrivate::Action CalenEditorPrivate::ActionSave
	Save entry 
 */
/*!
	\var CalenEditorPrivate::Action CalenEditorPrivate::ActionDelete
	Delete entry 
 */
/*!
	\enum CalenEditorPrivate::EditorItemIndex
	This enum defines the indexes of the items in the dataform
 */
/*!
	\var CalenEditorPrivate::EditorItemIndex CalenEditorPrivate::SubjectItem
	Index of the subject dataform item.
 */
/*!
	\var CalenEditorPrivate::EditorItemIndex CalenEditorPrivate::AllDayItem
	Index of the all day checkbox item.
 */
/*!
	\var CalenEditorPrivate::EditorItemIndex CalenEditorPrivate::DateTimeFromItem
	Index of the Date time (from) item.
 */
/*!
	\var CalenEditorPrivate::EditorItemIndex CalenEditorPrivate::DateTimeToItem
	Index of the Date time (to) item.
 */
/*!
	\var CalenEditorPrivate::EditorItemIndex CalenEditorPrivate::LocationItem
	Index of the location item.
 */
/*!
	\var CalenEditorPrivate::EditorItemIndex CalenEditorPrivate::ReminderItem
	Index of the reminder item.
 */
/*!
	\var CalenEditorPrivate::EditorItemIndex CalenEditorPrivate::RepeatItem
	Index of the repeat item.
 */
/*!
	\var CalenEditorPrivate::EditorItemIndex CalenEditorPrivate::RepeatUntilItem
	Index of the repest Until item.
 */

/*!
	\class CalenEditorPrivate
 */

/*!
	Constructor.

	\param agendaUtil pointer to agenda utility
	\param parent HbView pointer.
 */
CalenEditorPrivate::CalenEditorPrivate(AgendaUtil *agendaUtil, 
                                       QObject *parent) :
									QObject(parent), 
									mAgendaUtil(NULL), 
									mEditRange(ThisAndAll),
									mEditorDocLoader(NULL),
									mViewFromItem(NULL),
									mViewToItem(NULL),
									mEditorView(NULL), 
									mCalenEditorForm(NULL), 
									mCalenEditorModel(NULL),
									mReminderItem(NULL), 
									mRepeatItem(NULL),
									mCustomRepeatUntilItem(NULL),
									mDescriptionItem(NULL),
									mSubjectItem(NULL),
									mAllDayCheckBoxItem(NULL),
									mLocationItem(NULL),
									mNewEntry(true), 
									mRepeatUntilItemAdded(false),
									mDescriptionItemAdded(false), 
									mIsChild(false),
									mIsAllDayItemAdded(false), 
									mLaunchCalendar(false),
									mMenuItemAdded(false)
{
	// First get the q-pointer.
	q_ptr = static_cast<CalenEditor *> (parent);
	mMainWindow = NULL;
	
	if (!agendaUtil) {
		mAgendaUtil = new AgendaUtil(this);
		mOwnsAgendaUtil = true;
	} else {
		mAgendaUtil = agendaUtil;
		mOwnsAgendaUtil = false;
	}

	// Load the translation file and install the editor specific translator
	mTranslator = new QTranslator;
	QString lang = QLocale::system().name();
	QString path = "Z:/resource/qt/translations/";
	mTranslator->load("caleneditor_en_GB", ":/translations");
	// TODO: Load the appropriate .qm file based on locale
	//bool loaded = mTranslator->load("caleneditor_" + lang, path);
	HbApplication::instance()->installTranslator(mTranslator);
}

/*!
	Destructor.
 */
CalenEditorPrivate::~CalenEditorPrivate()
{
	if (mOriginalEntry) {
		delete mOriginalEntry;
		mOriginalEntry = NULL;
	}
	if (mEditedEntry) {
		delete mEditedEntry;
		mEditedEntry = NULL;
	}
	if (mCalenEditorModel) {
		delete mCalenEditorModel;
		mCalenEditorModel = NULL;
	}
	// Remove the translator
	HbApplication::instance()->removeTranslator(mTranslator);
	if (mTranslator) {
		delete mTranslator;
		mTranslator = 0;
	}
	
	// delete the mainwindow object if created any
	if (mMainWindow) {
		delete mMainWindow;
		mMainWindow = NULL;
	}
}

/*!
	Shows the caleneditor by parsing a .vcs.

	\param handle reference to QFile handle.
 */
void CalenEditorPrivate::edit(const QFile &/*handle*/, bool launchCalendar)
{
	// TODO: Needs to be implemented using importer.
	mNewEntry = false;
	mLaunchCalendar = launchCalendar;
}

/*!
	Shows the caleneditor, by parsing an AgendaEntry.
	\param entry An object of AgendaEntry.
 */
void CalenEditorPrivate::edit(AgendaEntry entry, bool launchCalendar)
{
	mNewEntry = false;
	mLaunchCalendar = launchCalendar;
	showEditor(entry);
}

/*!
	Shows the caleneditor, by using the entry id provided.
	\param id entry id.
 */
void CalenEditorPrivate::edit(ulong id, bool launchCalendar)
{
	mNewEntry = false;
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	edit(entry, launchCalendar);
}

/*!
	Creates a new entry based on the create type.It launches the
	editor for editing
	\type type of the editor to be shown for creating new entry.
	\QDateTime newEntryDateTime is the event start time.
 */
void CalenEditorPrivate::create(CalenEditor::CreateType type,
                                QDateTime newEntryDateTime, bool launchCalendar)
{
	mNewEntryDateTime = newEntryDateTime;
	mNewEntry = true;
	AgendaEntry entry;
	switch(type){
		case CalenEditor::TypeAppointment: {
			entry.setType(AgendaEntry::TypeAppoinment);
		}
		break;
		case CalenEditor::TypeUnKnown:
		default:
			break;
	}
	mLaunchCalendar = launchCalendar;
	showEditor(entry);
}

/*!
	Creates a new entry based on the create type.It launches the
	editor for editing
	\type type of the editor to be shown for creating new entry.
	\QDateTime newEntryDateTime is the event start time.
 */
void CalenEditorPrivate::create(CalenEditor::CreateType type,
                                AgendaEntry entry, bool launchCalendar)
{
	mNewEntry = true;
	switch(type){
		case CalenEditor::TypeAppointment: {
			entry.setType(AgendaEntry::TypeAppoinment);
		}
		break;
		case CalenEditor::TypeUnKnown:
		default:
			break;
	}
	mNewEntryDateTime = entry.startTime();
	mLaunchCalendar = launchCalendar;
	showEditor(entry);
}

/*!
	Queries user whether to edit whole series or just this single occurence
 */
void CalenEditorPrivate::showEditOccurencePopup()
{
	HbDialog popUp;
	popUp.setDismissPolicy(HbDialog::NoDismiss);
	popUp.setTimeout(HbDialog::NoTimeout);

	QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
	HbWidget *editWidget = new HbWidget();
	editWidget->setLayout(layout);

	HbRadioButtonList *editButtonList = new HbRadioButtonList();

	QStringList list;
	list << hbTrId("txt_calendar_info_this_occurrence_only")
	        << hbTrId("txt_calendar_info_all_occurences");

	editButtonList->setItems(list);

	layout->addItem(editButtonList);

	popUp.setContentWidget(editWidget);
	popUp.setHeadingWidget(new HbLabel(hbTrId("txt_calendar_title_edit")));

	// Create secondary action
	HbAction *cancelAction =
	        new HbAction(hbTrId("txt_calendar_button_softkey1_cancel"));
	popUp.setSecondaryAction(cancelAction);
	connect(editButtonList, SIGNAL(itemSelected(int)), this,
	        SLOT(handleEditOccurence(int)));
	connect(editButtonList, SIGNAL(itemSelected(int)), &popUp, SLOT(close()));

	connect(cancelAction, SIGNAL(triggered()), &popUp, SLOT(close()));
	connect(cancelAction, SIGNAL(triggered()), this, SLOT(handleCancel()));

	// Show the popup
	popUp.exec();
}

/*!
	Slot to handle User selection for series editing or single occurence popup
 */
void CalenEditorPrivate::handleEditOccurence(int index)
{
	switch (index) {
		case 0:
			// User wants to edit only this occurence
			mEditRange = ThisOnly;
			break;
		case 1:
			// User wants to edit all the occurences
			mEditRange = ThisAndAll;
			break;
	}
}

/*!
	Slot to handle cancel action on the popup
 */
void CalenEditorPrivate::handleCancel()
{
	// User has chosen not to edit the event, hence return
	mEditRange = UserCancelled;
}

/*!
	Displays the editor.
 */
void CalenEditorPrivate::showEditor(AgendaEntry entry)
{
	
	if (0 < entry.id()) {
		mNewEntry = false;
	}

	AgendaEntry entryToBeEdited = entry;
	if (!mNewEntry) {
		// Before we do anything, check in the entry is repeating
		// OR its a child item
		mIsChild = !(entry.recurrenceId().isNull());
		bool isRepeating = entry.isRepeating();
		if (mIsChild || isRepeating) {
			// Query user if he wants to edit whole series 
			// or just this occurence
			showEditOccurencePopup();
			// If user has selected to edit all the occurences, 
			// then get the parent of it
			if (mEditRange == ThisAndAll) {
				entryToBeEdited = mAgendaUtil->parentEntry(entry);
			} else if (mEditRange == UserCancelled) {
				// Do cleanup and return
				emit q_ptr->dialogClosed();
				return;
			}
		} else {
			mEditRange = ThisAndAll;
		}
	}

	mOriginalEntry = new AgendaEntry(entryToBeEdited);
	mEditedEntry = new AgendaEntry(entryToBeEdited);

	if (!mEditedEntry->description().isEmpty()) {
		mDescriptionItemAdded = true;
	}
	// Construct the view using docloader
	setUpView();
		
	// Set the title text. 
	mEditorView->setTitle(hbTrId("txt_calendar_title_calendar"));

	mSoftKeyAction = new HbAction(Hb::BackAction);
	mEditorView->setNavigationAction(mSoftKeyAction);
	connect(mSoftKeyAction, SIGNAL(triggered()), this,
	        SLOT(saveAndCloseEditor()));
}

/*!
	Sets up the view.
 */
void CalenEditorPrivate::setUpView()
{
	mEditorDocLoader = new CalenEditorDocLoader();
	bool loadSuccess = false;

	mEditorDocLoader->load(CALEN_EDITOR_XML_FILE, &loadSuccess);
	Q_ASSERT_X(loadSuccess, "caleneditor.cpp",
			"Unable to load caleneditor view XML");

	mEditorView = qobject_cast<HbView *> (mEditorDocLoader->findWidget(
											CALEN_EDITOR_VIEW));

	Q_ASSERT_X(mEditorView, "caleneditor.cpp",
			"Unable to load calenEditor view");

	HbGroupBox *headingWidget = qobject_cast<HbGroupBox *> (
							mEditorDocLoader->findWidget(CALEN_EDITOR_HEADING));

	mCalenEditorForm = qobject_cast<HbDataForm *> (
							mEditorDocLoader->findWidget(CALEN_EDITOR_DATAFORM));

	mDescriptionAction = qobject_cast<HbAction *> (
							mEditorDocLoader->findObject(
										CALEN_EDITOR_ADD_DESCRIPTION_ACTION));

	if (mDescriptionItemAdded) {
		mDescriptionAction->setText(
							hbTrId("txt_calendar_opt_remove_description"));
	}
	connect(mDescriptionAction, SIGNAL(triggered()), this,
							SLOT(handleDescriptionAction()));

	HbAction *deleteEventAction = qobject_cast<HbAction *> (
							mEditorDocLoader->findObject(
										CALEN_EDITOR_DELETE_EVENT_ACTION));
	connect(deleteEventAction, SIGNAL(triggered()), this,
							SLOT(handleDeleteAction()));

	if (!mNewEntry) {
		//TODO: Add the text id based on the entry type Anniversary or meeting
		headingWidget->setHeading(hbTrId("txt_calendar_subhead_event"));
	}

	initModel();
	// Set the custom item.
	CalenEditorCustomItem *customItem =
							new CalenEditorCustomItem(mCalenEditorForm);
	QList<HbAbstractViewItem*> protos = mCalenEditorForm->itemPrototypes();
	protos.append(customItem);
	mCalenEditorForm->setItemPrototypes(protos);

	// Now populate the model items.
	populateModel();

	HbMainWindow* window = hbInstance->allMainWindows().first();
	if (!window) {
		// Might be some non-ui based app called us
		// so create mainwindow now
		mMainWindow = new HbMainWindow();
		mMainWindow->addView(mEditorView);
		mMainWindow->setCurrentView(mEditorView);
	} else {
		window->addView(mEditorView);
		window->setCurrentView(mEditorView);
	}
	
}

/*!
	Adding the discard Menu option for the Editor view.
 */
void CalenEditorPrivate::addDiscardAction()
{
	if (!mMenuItemAdded) {
		// Set this specific menu   
		HbMenu *menu = mEditorView->menu();
		HbAction *discardAction =
					new HbAction(hbTrId("txt_calendar_opt_discard_changes"));
		menu->insertAction(mDescriptionAction, discardAction);
		connect(discardAction, SIGNAL(triggered()), this,
					SLOT(discardChanges()));
		mEditorView->setMenu(menu);
		mMenuItemAdded = true;
	}
}

/*!
	Initialises the Model & sets for the CalenEditorForm.
 */
void CalenEditorPrivate::initModel()
{
	// Remove the model.
	if (mCalenEditorForm->model()) {
		delete mCalenEditorForm->model();
		mCalenEditorForm->setModel(0);
	}
	// Create a model and set it.
	mCalenEditorModel = new HbDataFormModel(mCalenEditorForm);

	// Add the fields to the DataForm.
	addSubjectItem();

	// Add all day check box for new entry creation or while editing
	// parent entry or existing non repeating event
	if (mNewEntry) {
		addAllDayCheckBoxItem();
	} else { 
		if (!mEditedEntry->isRepeating() || (mEditRange == ThisAndAll)) {
			addAllDayCheckBoxItem();
		}
	}
	
	addCustomItemFrom();
	addCustomItemTo();
	addLocationItem();
	addReminderItem();
	// Add the repeat information only if particular occurence is not being 
	if (mEditRange == ThisAndAll) {
		addRepeatItem();
	}

	mCalenEditorForm->setModel(mCalenEditorModel);
}

/*!
	Appends the Subject Item to the Data form Model.
 */
void CalenEditorPrivate::addSubjectItem()
{
	// Creats and appends data form model item to set subject of entry.
	mSubjectItem = mCalenEditorModel->appendDataFormItem(
										HbDataFormModelItem::TextItem,
										hbTrId(
										"txt_calendar_formlabel_val_subject"),
										mCalenEditorModel->invisibleRootItem());
}

/*!
	Appends the allday item to the Data form Model.
 */
void CalenEditorPrivate::addAllDayCheckBoxItem()
{
	mAllDayCheckBoxItem = new HbDataFormModelItem();
	mAllDayCheckBoxItem->setType(HbDataFormModelItem::CheckBoxItem);
	mCalenEditorModel->appendDataFormItem(mAllDayCheckBoxItem);
	mIsAllDayItemAdded = true;
}

/*!
	Appends the start time of the event item to the Data form Model.
 */
void CalenEditorPrivate::addCustomItemFrom()
{
	HbDataFormModelItem::DataItemType itemType =
	        static_cast<HbDataFormModelItem::DataItemType> (CustomWidgetFrom);
	HbDataFormModelItem *customModelItem = new HbDataFormModelItem(itemType,
								hbTrId("txt_calendar_setlabel_start_time"));
	mCalenEditorModel->appendDataFormItem(customModelItem);
}

/*!
	Appends the end time of the event item to the Data form Model.
 */
void CalenEditorPrivate::addCustomItemTo()
{

	HbDataFormModelItem::DataItemType itemType =
	        static_cast<HbDataFormModelItem::DataItemType> (CustomWidgetTo);
	HbDataFormModelItem *customModelItem = new HbDataFormModelItem(itemType,
									hbTrId("txt_calendar_setlabel_end_time"));
	mCalenEditorModel->appendDataFormItem(customModelItem);
}

/*!
	Appends the Location Item to the Data form Model.
 */
void CalenEditorPrivate::addLocationItem()
{
	// Creates and appends a data form model item to set location.
	mLocationItem = mCalenEditorModel->appendDataFormItem(
										HbDataFormModelItem::TextItem, hbTrId(
										"txt_calendar_formlabel_val_location"),
										mCalenEditorModel->invisibleRootItem());
}

/*!
	Appends the repeat item to the Data form Model.
 */
void CalenEditorPrivate::addRepeatItem()
{
	mRepeatRuleType = mEditedEntry->repeatRule().type();
	mRepeatItem = new HbDataFormModelItem();
	mRepeatItem->setType(HbDataFormModelItem::ComboBoxItem);
	mRepeatItem->setData(HbDataFormModelItem::LabelRole,
	                     hbTrId("txt_calendar_setlabel_repeat"));

	// Create the repeat choices
	QStringList repeatChoices;
	repeatChoices << hbTrId("txt_calendar_setlabel_repeat_val_only_once")
	        << hbTrId("txt_calendar_setlabel_repeat_val_daily")
	        << hbTrId("txt_calendar_setlabel_repeat_val_weekly")
	        << hbTrId("txt_calendar_setlabel_repeat_val_fortnightly")
	        << hbTrId("txt_calendar_setlabel_repeat_val_monthly")
	        << hbTrId("txt_calendar_setlabel_repeat_val_yearly");

	mRepeatItem->setContentWidgetData(QString("items"), repeatChoices);
	mCalenEditorModel->appendDataFormItem( mRepeatItem,
								mCalenEditorModel->invisibleRootItem());
}
/*!
 Appends the reminder item to the Data form Model.
 */
void CalenEditorPrivate::addReminderItem()
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

	mCalenEditorModel->appendDataFormItem( mReminderItem,
									mCalenEditorModel->invisibleRootItem());
}

/*!
	Appends the description item to the Data form Model.
 */
void CalenEditorPrivate::handleDescriptionAction()
{
	if (!mDescriptionItemAdded) {
		populateDescriptionItem();
		mDescriptionAction->setText(
							hbTrId("txt_calendar_opt_remove_description"));
	} else {
		removeDescriptionItem();
		mDescriptionAction->setText(
							hbTrId("txt_calendar_opt_add_description"));
	}
}

/*!
	Poplulates the Model.
 */
void CalenEditorPrivate::populateModel()
{
	populateSubjectItem();

	if (mIsAllDayItemAdded) {
		populateAllDayItem();
	}

	populateCustomItemDateTime();
	populateLocationItem();
	populateReminderItem();

	// Dont Populate the repeat field when particular occurence is being edited
	if (mEditRange == ThisAndAll) {
		populateRepeatItem();
	}

	if (mDescriptionItemAdded) {
		populateDescriptionItem();
	}
}

/*!
	Populates the Subject Item in the Data From Model.
 */
void CalenEditorPrivate::populateSubjectItem()
{
	mSubjectItem->setContentWidgetData("text", mEditedEntry->summary());
	mSubjectItem->setContentWidgetData("minRows", 2);
	mSubjectItem->setContentWidgetData("maxRows", 4);
	mCalenEditorForm->addConnection(mSubjectItem,
	                                SIGNAL(textChanged(const QString)), this,
	                                SLOT(handleSubjectChange(const QString)));
}

/*!
	Populates the all day item.
 */
void CalenEditorPrivate::populateAllDayItem()
{
	mAllDayCheckBoxItem->setContentWidgetData("text",
									hbTrId("txt_calendar_list_all_day_event"));
	if (AgendaEntry::TypeEvent == mEditedEntry->type()) {
		//If All day is enabled.
		mAllDayCheckBoxItem->setContentWidgetData("checkState", Qt::Checked);
	}
	mCalenEditorForm->addConnection(mAllDayCheckBoxItem,
	                                SIGNAL(stateChanged(int )), this,
	                                SLOT(handleAllDayChange(int)));

}

/*!
	Populates the start and end time of the event item.
 */
void CalenEditorPrivate::populateCustomItemDateTime()
{
	// Get the index of the start time of the event item.
	// Check if all day has been added or not 
	// and calculate the index accordingly
	int itemIndex;
	if (mIsAllDayItemAdded) {
		itemIndex = DateTimeFromItem;
	} else {
		itemIndex = DateTimeFromItem - 1;
	}
	QModelIndex index = mCalenEditorModel->index(itemIndex, 0);
	mViewFromItem
	        = qobject_cast<CalenEditorCustomItem *> (
										 mCalenEditorForm->itemByIndex(index));
	connect(mViewFromItem, SIGNAL(dateTimeUpdated(QDateTime &)), this,
										SLOT(saveFromDateTime(QDateTime &)));
	// Pass the start time of the event according to new or existing event.
	QDateTime fromDateTime = mNewEntryDateTime;
	QTime currentTime = QTime::currentTime();
	if (!mNewEntry) {
		// Check if we are going to create an exception here
		if (mEditRange == ThisOnly) {
			// Update the end time of the instance properly as we would have got 
			// parent entry from the caller - this is acheived by adding the
			// meeting duration
			AgendaEntry parentEntry = mAgendaUtil->parentEntry(*mEditedEntry);
			int durationInSeconds = parentEntry.durationInSecs();
			QDateTime endTime =
			        mEditedEntry-> startTime().addSecs(durationInSeconds);

			// set this to the original entr as well as edited entry
			mOriginalEntry->setStartAndEndTime(mOriginalEntry->startTime(),
										endTime);
			mEditedEntry->setStartAndEndTime(mEditedEntry->startTime(), endTime);
		}
		fromDateTime = mEditedEntry->startTime();
	} else {
		// Check if it is on same day and set the default time and date accordingly.
		bool isSameDay = CalenDateUtils::isOnToday(fromDateTime);
		mMinutes = 0;
		mHour = currentTime.hour();
		if (isSameDay) {
			mMinutes = currentTime.minute();
			if (mMinutes > 0 && mMinutes < 31) {
				mMinutes = 30;
			} else {
				mMinutes = 0;
			}
			currentTime.setHMS(mHour, mMinutes, 0, 0);
			if (mMinutes == 0) {
				currentTime = currentTime.addSecs(60 * 60);
			}
			fromDateTime.setDate(fromDateTime.date());
			fromDateTime.setTime(currentTime);
		}
	}
	mViewFromItem->populateDateTime(fromDateTime, true);
	mNewEntryDateTime = fromDateTime;

	// Get the index of the end time.
	if (mIsAllDayItemAdded) {
		index = mCalenEditorModel->index(DateTimeToItem, 0);
	} else {
		index = mCalenEditorModel->index(DateTimeToItem - 1, 0);
	}

	mViewToItem
	        = qobject_cast<CalenEditorCustomItem *> (
									mCalenEditorForm->itemByIndex( index));
	connect(mViewToItem, SIGNAL(dateTimeUpdated(QDateTime &)), this,
	        SLOT(saveToDateTime(QDateTime &)));

	// Pass the end time of the event according to new or existing event.
	QDateTime toDateTime;
	if (!mNewEntry) {
		toDateTime = mEditedEntry->endTime();
		// Check if we are going to create an exception here
		if (mEditRange == ThisOnly) {
			// set the date picker range so that it does not collide
			// with the next  and previous occurence
			QDateTime prevInstanceStartTime;
			QDateTime prevInstanceEndTime;
			mAgendaUtil->getPreviousInstanceTimes(*mEditedEntry,
			                                      prevInstanceStartTime,
			                                      prevInstanceEndTime);

			QDateTime nextInstanceStartTime;
			QDateTime nextInstanceEndTime;
			mAgendaUtil->getNextInstanceTimes(*mEditedEntry,
			                                  nextInstanceStartTime,
			                                  nextInstanceEndTime);
			mViewFromItem->setDateRange(
									prevInstanceStartTime.addDays(1).date(),
									nextInstanceStartTime.addDays(-1).date());
			mViewToItem->setDateRange(prevInstanceEndTime.date().addDays(1),
			                        nextInstanceEndTime.date().addDays(-1));
		}
	} else {
		toDateTime = fromDateTime.addSecs(60 * 60);
	}
	mViewToItem->populateDateTime(toDateTime, false);

	// Set the default start time to the event.
	mEditedEntry->setStartAndEndTime(fromDateTime, toDateTime);
	// Set the default start time to the Original entry if its a new event.
	if (mNewEntry) {
		mOriginalEntry->setStartAndEndTime(fromDateTime, toDateTime);
	}
	if (mAllDayCheckBoxItem
	        && (mAllDayCheckBoxItem->contentWidgetData("checkState")
	                == Qt::Checked)) {
		// If the all day option is checked, we need to
		// disable the time fields
		enableFromTotimeFileds(false, mEditedEntry->startTime(),
		                       mEditedEntry->endTime());
	}
}

/*!
	Save the changed start time of the event.
 */
void CalenEditorPrivate::saveFromDateTime(QDateTime& fromDateTime)
{
	QDateTime endTime = mEditedEntry->endTime();
	// Update the end time accordingly on UI - duration will be 60 mins
	// bydefault only while creating new entry and if it crossed the endtime
	if (mNewEntry && fromDateTime > endTime) {
		endTime = fromDateTime.addSecs(3600);
	} else { // for existing entry
		// we need to see if user has moved start time beyond the end time
		// then add the duration of the meeting that was saved earlier to the 
		// new start time to get the new end time
		if (fromDateTime > endTime) {
			int duration = mEditedEntry->durationInSecs();
			endTime = fromDateTime.addSecs(duration);
		}
	}
	// Set the new end time to the form
	if (mViewToItem) {
		mViewToItem->populateDateTime(endTime, false);
	}
	// Set the times to edited entry
	mEditedEntry->setStartAndEndTime(fromDateTime, endTime);
}

/*!
	Save the changed end time of the event.
 */
void CalenEditorPrivate::saveToDateTime(QDateTime& toDateTime)
{
	QDateTime startTime = mEditedEntry->startTime();
	// Update the start time accordingly on UI - duration will be 60 mins
	// bydefault for new entry and if it crosses the starttime
	if (mNewEntry && toDateTime < startTime) {
		startTime = toDateTime.addSecs(-3600);
	} else { // for exisitng entry
		// we need to see if user has moved end time before the start time
		// then substract the duration of the meeting that was saved earlier to 
		// the new end time to get the new start time
		if (toDateTime < startTime) {
			int duration = mEditedEntry->durationInSecs();
			startTime = toDateTime.addSecs(-duration);
		}
	}
	// Set the new start time to the form
	if (mViewFromItem) {
		mViewFromItem->populateDateTime(startTime, true);
	}

	// Set the times to edited entry
	mEditedEntry->setStartAndEndTime(startTime, toDateTime);

	// update the repeat choices depending on the meeting duration
	updateRepeatChoices();
}

/*!
	Populates the Location item.
 */
void CalenEditorPrivate::populateLocationItem()
{
	// Check if all day has been added or not 
	// and calculate the index accordingly
	int index;
	if (mIsAllDayItemAdded) {
		index = LocationItem;
	} else {
		index = LocationItem - 1;
	}

	mLocationItem->setContentWidgetData("text", mEditedEntry->location());
	mSubjectItem->setContentWidgetData("minRows", 2);
	mSubjectItem->setContentWidgetData("maxRows", 4);
	mCalenEditorForm->addConnection(
	                                mLocationItem,
	                                SIGNAL(textChanged(const QString)),
	                                this,
	                                SLOT(handleLocationChange(const QString)));
}

/*!
	Populates the reminder item.
 */
void CalenEditorPrivate::populateReminderItem()
{
	// Check if all day has been added or not 
	// and calculate the index accordingly
	int index;
	if (mIsAllDayItemAdded) {
		index = ReminderItem;
	} else {
		index = ReminderItem - 1;
	}
	// Set the default reminder value to 15 minutes 
	if (mNewEntry) {
		mReminderItem->setContentWidgetData("currentIndex", 3);
		// Save the reminder alarm for the entry
		AgendaAlarm reminder;
		reminder.setTimeOffset(mReminderHash.value(3));
		reminder.setAlarmSoundName(QString(" "));
		// Set the reminder to the entry as well as original entry.
		mEditedEntry->setAlarm(reminder);
		mOriginalEntry->setAlarm(reminder);
	} else {
		if (mEditedEntry->alarm().isNull()) {
			// Alarm is set off
			mReminderItem->setContentWidgetData("currentIndex", 0);
		} else {
			// Get the reminder offset value.
			int reminderOffset = mEditedEntry->alarm().timeOffset();
			// Get the index value for the reminder combo box from the hash 
			// table.
			int index = mReminderHash.key(reminderOffset);
			mReminderItem->setContentWidgetData("currentIndex", index);
		}
	}
	mCalenEditorForm->addConnection(mReminderItem,
	                                SIGNAL(currentIndexChanged(int)), this,
	                                SLOT(handleReminderIndexChanged(int)));
}

/*!
	Populates the repeat item.
 */
void CalenEditorPrivate::populateRepeatItem()
{
	// Check if all day has been added or not 
	// and calculate the index accordingly
	int index;
	if (mIsAllDayItemAdded) {
		index = RepeatItem;
	} else {
		index = RepeatItem - 1;
	}
	HbDataFormViewItem
	        *item =
	                qobject_cast<HbDataFormViewItem *> (
										mCalenEditorForm->itemByIndex(
										mCalenEditorModel->index( index, 0)));
	mRepeatComboBox
	        = qobject_cast<HbComboBox *> (item->dataItemContentWidget());

	// Set the user roles for the combobox items so that we depend on these
	// roles to identify the correct repeat type when repeat choices are 
	// dynamically removed or added
	mRepeatComboBox->setItemData(RepeatOnce, RepeatOnce, Qt::UserRole+100);
	mRepeatComboBox->setItemData(RepeatDaily, RepeatDaily, Qt::UserRole+100);
	mRepeatComboBox->setItemData(RepeatWeekly, RepeatWeekly, Qt::UserRole+100);
	mRepeatComboBox->setItemData(RepeatBiWeekly, 
	                             RepeatBiWeekly, Qt::UserRole+100);
	mRepeatComboBox->setItemData(RepeatMonthly, 
	                             RepeatMonthly, Qt::UserRole+100);
	mRepeatComboBox->setItemData(RepeatYearly, RepeatYearly, Qt::UserRole+100);
	
	if (mEditedEntry->isRepeating()) {
		switch (mEditedEntry->repeatRule().type()) {
			case AgendaRepeatRule::DailyRule: {
				mRepeatComboBox->setCurrentIndex(1);
			}
				break;
			case AgendaRepeatRule::WeeklyRule: {
				if (mEditedEntry->repeatRule().interval() == 1) {
					mRepeatComboBox->setCurrentIndex(2);
				} else {
					mRepeatComboBox->setCurrentIndex(3);
					mIsBiWeekly = true;
				}
			}
				break;
			case AgendaRepeatRule::MonthlyRule: {
				mRepeatComboBox->setCurrentIndex(4);
			}
				break;
			case AgendaRepeatRule::YearlyRule: {
				mRepeatComboBox->setCurrentIndex(5);
			}
				break;
			default:
				break;
		}
		// If entry is repeating type then insert the repeatuntil item.
		insertRepeatUntilItem();
	} else {
		mRepeatComboBox->setCurrentIndex(0);
		// Set the Original entry value also.
		mOriginalEntry->setRepeatRule(
		                              AgendaRepeatRule(
		                              AgendaRepeatRule::InvalidRule));
	}
	connect(mRepeatComboBox, SIGNAL(currentIndexChanged(int)), this,
	        SLOT(handleRepeatIndexChanged(int)));

}

/*!
	Populates the description item.
 */
void CalenEditorPrivate::populateDescriptionItem()
{
	QModelIndex repeatIndex;
	if (mEditRange == ThisOnly) {
		repeatIndex = mCalenEditorModel->indexFromItem(mReminderItem);
	} else {
		repeatIndex = mCalenEditorModel->indexFromItem(mRepeatItem);
	}

	int descIndex;
	if (!mRepeatUntilItemAdded) {
		descIndex = repeatIndex.row() + 1;
	} else {
		descIndex = repeatIndex.row() + 2;
	}
	mDescriptionItem
	        = mCalenEditorModel->insertDataFormItem(
									descIndex, HbDataFormModelItem::TextItem,
									QString(hbTrId(
									"txt_calendar_formlabel_val_description")),
									mCalenEditorModel->invisibleRootItem());
	mDescriptionItem->setContentWidgetData("minRows", 2);
	mDescriptionItem->setContentWidgetData("maxRows", 4);
	mCalenEditorForm->addConnection(
								mDescriptionItem,
								SIGNAL(textChanged(const QString)), this,
								SLOT(handleDescriptionChange(const QString)));
	mDescriptionItemAdded = true;
	// TODO: Scroll the previous form item to the top so that the description 
	// field automatically scrolls to the top as well
	// mCalenEditorForm->scrollTo(repeatIndex, HbAbstractItemView::EnsureVisible);
}

/*!
	Removes the description item.
 */
void CalenEditorPrivate::removeDescriptionItem()
{
	// Disconnect the slot connected to description item
	mCalenEditorForm->removeConnection(
								   mDescriptionItem,
								   SIGNAL(textChanged(const QString)), this,
								   SLOT(handleDescriptionChange(const QString)));
	QModelIndex repeatIndex;
	if (mEditRange == ThisOnly) {
		repeatIndex = mCalenEditorModel->indexFromItem(mReminderItem);
	} else {
		repeatIndex = mCalenEditorModel->indexFromItem(mRepeatItem);
	}
	int descIndex;
	if (!mRepeatUntilItemAdded) {
		descIndex = repeatIndex.row() + 1;
	} else {
		descIndex = repeatIndex.row() + 2;
	}
	mCalenEditorModel->removeItem(mCalenEditorModel->index(descIndex, 0));
	// Remove the description from the entry
	mEditedEntry->setDescription("");

	mDescriptionItem = NULL;
	mDescriptionItemAdded = false;
}

/*!
	Inserts the repeat until item to the dataform model
 */
void CalenEditorPrivate::insertRepeatUntilItem()
{
	HbDataFormModelItem::DataItemType itemType =
	        static_cast<HbDataFormModelItem::DataItemType> (RepeatUntilOffset);

	QModelIndex repeatIndex = mCalenEditorModel->indexFromItem(mRepeatItem);
	mCustomRepeatUntilItem = mCalenEditorModel->insertDataFormItem(
										RepeatUntilItem,
										itemType,
										QString(
										hbTrId(
										"txt_calendar_setlabel_repeat_until")),
										mCalenEditorModel->invisibleRootItem());
	mRepeatUntilItemAdded = true;

	mCalenEditorForm->addConnection(mCustomRepeatUntilItem, SIGNAL(clicked()),
	                                this, SLOT(launchRepeatUntilDatePicker()));
	if (!mNewEntry && mRepeatRuleType != AgendaRepeatRule::InvalidRule) {
		HbExtendedLocale locale = HbExtendedLocale::system();
		QString dateString = locale.format(mEditedEntry->repeatRule().until(),
		                                   r_qtn_date_usual_with_zero);
		mCustomRepeatUntilItem->setContentWidgetData("text", dateString);
	}
	//TODO: Scroll to functionality has to be implemented	
}

/*!
	Close the Editor.
 */
void CalenEditorPrivate::closeEditor()
{
	HbMainWindow* window = hbInstance->allMainWindows().first();
	window->removeView(mEditorView);
	emit q_ptr->dialogClosed();
}

/*!
	Triggered when the subject editor is being edited.
	\param subject Contains the string displayed in the subject item.
 */
void CalenEditorPrivate::handleSubjectChange(const QString subject)
{
	mEditedEntry->setSummary(subject);
	addDiscardAction();
}

/*!
 Triggered when the Allday Checkbox is being edited.
 \param state of checkbox.
 */
void CalenEditorPrivate::handleAllDayChange(int state)
{
	if (mAllDayCheckBoxItem->contentWidgetData("checkState") == Qt::Checked) {
		// AllDayCheckBox is checked
		// Set From/To times buttons Read-Only
		// Display Start/End times as beginning of the day
		QDateTime tempSartTime =
		        CalenDateUtils::beginningOfDay(mEditedEntry->startTime());

		// Set EndTime of AllDay event to 23:59:59
		QDateTime tempEndTime = mEditedEntry->endTime();
		QTime tempEndQTime = tempEndTime.time();
		tempEndQTime.setHMS(23, 59, 59);
		tempEndTime.setTime(tempEndQTime);

		enableFromTotimeFileds(false, tempSartTime, tempEndTime);
	} else {
		// AllDayCheckBox in un-checked
		// Set From/To times buttons editable
		// Update Start/End Times with Edited entry values
		enableFromTotimeFileds(true, mEditedEntry->startTime(),
		                       mEditedEntry->endTime());

	}
	addDiscardAction();
}

/*!
	Triggered when the location editor is being edited.
	\param subject Contains the string displayed in the subject item.
 */
void CalenEditorPrivate::handleLocationChange(const QString location)
{
	mEditedEntry->setLocation(location);
	addDiscardAction();
}

/*!
	Triggerd from tapping on reminder item.
	Handles the reminder time change and updates the same in the event.
	\param index The new index chosen in the reminder list.
 */
void CalenEditorPrivate::handleReminderIndexChanged(int index)
{
	AgendaAlarm reminder;
	if (!mEditedEntry->alarm().isNull()) {
		reminder = mEditedEntry->alarm();
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
	mEditedEntry->setAlarm(reminder);
	addDiscardAction();
}

/*!
	Triggerd from tapping on repeat item.
	Handles the repeat rule change and updates the same in the event.
	\param index The new index chosen in the repeat list.
 */
void CalenEditorPrivate::handleRepeatIndexChanged(int index)
{
	mIsBiWeekly = false;

	HbExtendedLocale locale = HbExtendedLocale::system();
	// Get the user role w ehave set for this index
	QVariant userRole = mRepeatComboBox->itemData(index, Qt::UserRole + 100);
	int value = userRole.toInt();
	switch (value) {
		case 1: {
			if (!mRepeatUntilItemAdded) {
				insertRepeatUntilItem();
			}
			if (mCustomRepeatUntilItem) {
				QDate date = mEditedEntry->startTime().date().addYears(1);
				mCustomRepeatUntilItem->setContentWidgetData( "text", 
												locale.format( date,
												r_qtn_date_usual_with_zero));
			}
			mRepeatRuleType = AgendaRepeatRule::DailyRule;
			mIsBiWeekly = false;
		}
		break;
		case 2: {
			if (!mRepeatUntilItemAdded) {
				insertRepeatUntilItem();
			}
			if (mCustomRepeatUntilItem) {
				QDate date = mEditedEntry->startTime().date().addYears(1);
				mCustomRepeatUntilItem->setContentWidgetData( "text",
												locale.format( date,
												r_qtn_date_usual_with_zero));

			}
			mRepeatRuleType = AgendaRepeatRule::WeeklyRule;
			mIsBiWeekly = false;
		}
		break;
		case 3: {
			if (!mRepeatUntilItemAdded) {
				insertRepeatUntilItem();
			}
			if (mCustomRepeatUntilItem) {
				QDate date = mEditedEntry->startTime().date().addYears(1);
				mCustomRepeatUntilItem->setContentWidgetData( "text",
												locale.format( date,
												r_qtn_date_usual_with_zero));
			}
			mRepeatRuleType = AgendaRepeatRule::WeeklyRule;
			mIsBiWeekly = true;
				}
				break;
		case 4: {
			if (!mRepeatUntilItemAdded) {
				insertRepeatUntilItem();
			}
			if (mCustomRepeatUntilItem) {
				QDate date = mEditedEntry->startTime().date().addYears(5);
				mCustomRepeatUntilItem->setContentWidgetData( "text",
												locale.format( date,
												r_qtn_date_usual_with_zero));
			}
			mRepeatRuleType = AgendaRepeatRule::MonthlyRule;
			mIsBiWeekly = false;
		}
		break;
		case 5: {
			if (!mRepeatUntilItemAdded) {
				insertRepeatUntilItem();
			}
			if (mCustomRepeatUntilItem) {
				QDate date = mEditedEntry->startTime().date().addYears(10);
				mCustomRepeatUntilItem->setContentWidgetData( "text",
												locale.format( date,
												r_qtn_date_usual_with_zero));
			}
			mRepeatRuleType = AgendaRepeatRule::YearlyRule;
			mIsBiWeekly = false;
		}
		break;
		default: {
			mRepeatRuleType = AgendaRepeatRule::InvalidRule;
			if (mRepeatUntilItemAdded) {
				QModelIndex repeatIndex =
				        mCalenEditorModel->indexFromItem(mRepeatItem);
				mCalenEditorModel->removeItem(
				                              mCalenEditorModel->index(
				                              repeatIndex.row()+ 1, 0));
				mRepeatUntilItemAdded = false;
				mCustomRepeatUntilItem = NULL;
			}
			mIsBiWeekly = false;
		}
		break;
	}
	addDiscardAction();
}

/*!
	Launches the date picker by tapping on the repaet until pushbutton
 */
void CalenEditorPrivate::launchRepeatUntilDatePicker()
{
	if (mDatePicker) {
		mDatePicker = NULL;
	}
	if (mRepeatRuleType == AgendaRepeatRule::DailyRule) {
		QDate minDate = mEditedEntry->endTime().date().addDays(1);
		mDatePicker = new HbDateTimePicker(mRepeatUntilDate, mEditorView);
		mDatePicker->setMinimumDate(minDate);
		mDatePicker->setMaximumDate(QDate(31, 12, 2100));
		mDatePicker->setDate(mRepeatUntilDate);
	} else if (mRepeatRuleType == AgendaRepeatRule::WeeklyRule) {
		QDate minDate;
		if (!mIsBiWeekly) {
			minDate = mEditedEntry->endTime().date().addDays(7);
		} else {
			minDate = mEditedEntry->endTime().date().addDays(14);
		}
		mDatePicker = new HbDateTimePicker(mRepeatUntilDate, mEditorView);
		mDatePicker->setMinimumDate(minDate);
		mDatePicker->setMaximumDate(QDate(31, 12, 2100));
		mDatePicker->setDate(mRepeatUntilDate);
	} else if (mRepeatRuleType == AgendaRepeatRule::MonthlyRule) {
		QDate minDate = mEditedEntry->endTime().date().addMonths(1);
		mDatePicker = new HbDateTimePicker(mRepeatUntilDate, mEditorView);
		mDatePicker->setMinimumDate(minDate);
		mDatePicker->setMaximumDate(QDate(31, 12, 2100));
		mDatePicker->setDate(mRepeatUntilDate);
	} else if (mRepeatRuleType == AgendaRepeatRule::YearlyRule) {
		QDate minDate = mEditedEntry->endTime().date().addYears(1);
		mDatePicker = new HbDateTimePicker(mRepeatUntilDate, mEditorView);
		mDatePicker->setMinimumDate(minDate);
		mDatePicker->setMaximumDate(QDate(31, 12, 2100));
		mDatePicker->setDate(mRepeatUntilDate);
	}
	HbDialog popUp;
	popUp.setDismissPolicy(HbDialog::NoDismiss);
	popUp.setTimeout(HbDialog::NoTimeout);
	popUp.setContentWidget(mDatePicker);
	popUp.setHeadingWidget( new HbLabel(
								hbTrId("txt_calendar_title_repeat_until")));
	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"));
	popUp.setPrimaryAction(okAction);
	connect(okAction, SIGNAL(triggered()), this, SLOT(setRepeatUntilDate()));
	connect(okAction, SIGNAL(triggered()), &popUp, SLOT(close()));
	popUp.setSecondaryAction(new HbAction(hbTrId("txt_common_button_cancel"),
								&popUp));
	popUp.exec();
}

/*!
	Sets the repeat until date on the repeat until item
 */
void CalenEditorPrivate::setRepeatUntilDate()
{
	mRepeatUntilDate = mDatePicker->date();
	if (mRepeatUntilDate.isValid()) {
		HbExtendedLocale locale = HbExtendedLocale::system();
		QString dateString = locale.format(mRepeatUntilDate,
									r_qtn_date_usual_with_zero);
		mCustomRepeatUntilItem->setContentWidgetData("text", dateString);
	}
}

/*!
	Triggered when the description editor is being edited.
	\param description Contains the string displayed in the description item.
 */
void CalenEditorPrivate::handleDescriptionChange(const QString description)
{
	mEditedEntry->setDescription(description);
	addDiscardAction();
}

/*!
	Saves the entry & closes the editor.
	If mLaunchCalendar is true, then it tries to launch the calendar
 */
void CalenEditorPrivate::saveAndCloseEditor()
{
	Action action = handleDone();
	closeEditor();

	if (CalenEditorPrivate::ActionSave == action) {
		// check if we need to launch the calendar application
		if (mLaunchCalendar) {
			CalenLauncher* launcher = new CalenLauncher(this);
			QDateTime startTime = mEditedEntry->startTime();
			launcher->launchCalendarApp(CalenLauncher::DayView, startTime);

			// connect to the error signal
			connect(launcher, SIGNAL(calendarLaunchFailed(int)), this,
									SLOT(handleCalendarLaunchError(int)));
		}
	}
}

/*!
	Slot to handle errors while launching the calendar application
 */
void CalenEditorPrivate::handleCalendarLaunchError(int error)
{
	// emit signal to the editor launcher
	emit q_ptr->calendarLaunchFailed(error);
}

/*!
	Deletes the entry.
 */
void CalenEditorPrivate::handleDeleteAction()
{

	if (mNewEntry) {
		if (showDeleteConfirmationQuery()) {
			closeEditor();
		}
		return;
	}
	
	// Delete entry and close editor
	deleteEntry(true);

}

/*!
	Launches the Settings view.
 */
void CalenEditorPrivate::launchSettingsView()
{
	// TODO :: Not yet implemented.
}

/*!
	Discards the changes done to the entry.
 */
void CalenEditorPrivate::discardChanges()
{
	// Close the editor without saving the changes.
	closeEditor();
}

/*!
	Connect or disconnect the slots.
 */
void CalenEditorPrivate::connectSlots(bool toConnect)
{
	if (toConnect) {
		mCalenEditorForm->addConnection( mSubjectItem,
									SIGNAL(textChanged(const QString)), this,
									SLOT(handleSubjectChange(const QString)));
		if (mIsAllDayItemAdded) {
			connect( mCalenEditorModel,
				SIGNAL(dataChanged(const QModelIndex, const QModelIndex)), this,
				SLOT(handleAllDayChange(const QModelIndex, const QModelIndex)));
		}
		mCalenEditorForm->addConnection( mLocationItem,
				SIGNAL(textChanged(const QString)),	this,
				SLOT(handleLocationChange(const QString)));
		mCalenEditorForm->addConnection( mReminderItem,
				SIGNAL(currentIndexChanged(int)), this,
				SLOT(handleReminderIndexChanged(int)));
		if (mEditRange == ThisAndAll) {
			connect(mRepeatComboBox, SIGNAL(currentIndexChanged(int)), this,
				SLOT(handleRepeatIndexChanged(int)));
		}
		if (mDescriptionItemAdded) {
			mCalenEditorForm->addConnection( mDescriptionItem,
				SIGNAL(textChanged(const QString)), this,
				SLOT(handleDescriptionChange(const QString)));
		}
	} else {
		mCalenEditorForm->removeConnection( mSubjectItem,
				SIGNAL(textChanged(const QString)),this,
				SLOT(handleSubjectChange(const QString)));
		if (mIsAllDayItemAdded) { 
			disconnect( mCalenEditorModel,
				SIGNAL(dataChanged(const QModelIndex, const QModelIndex)), this,
				SLOT(handleAllDayChange(const QModelIndex, const QModelIndex)));
		}
		mCalenEditorForm->removeConnection( mLocationItem,
				SIGNAL(textChanged(const QString)), this,
				SLOT(handleLocationChange(const QString)));
		mCalenEditorForm->removeConnection( mReminderItem,
				SIGNAL(currentIndexChanged(int)), this,
				SLOT(handleReminderIndexChanged(int)));
		if (mEditRange == ThisAndAll) {
			disconnect(mRepeatComboBox, SIGNAL(currentIndexChanged(int)), this,
				SLOT(handleRepeatIndexChanged(int)));
		}
		if (mDescriptionItemAdded) {
			mCalenEditorForm->removeConnection( mDescriptionItem,
				SIGNAL(textChanged(const QString)), this,
				SLOT(handleDescriptionChange(const QString)));
		}

	}

}

/*!
  Updates the repeat choices depending on the meeting duration
*/
void CalenEditorPrivate::updateRepeatChoices()
{
	// Only when editing all the occurences, repeat item will be shown
	if (mEditRange == ThisAndAll) {
		// Clear all the choices and add it again. If we dont do it 
		// as user would have changed the end times many times and we would have
		// deleted repeat options depending upon that
		// Get the current choice
		int choice = mRepeatComboBox->currentIndex();
		mRepeatComboBox->clear();
		QStringList repeatChoices;
		repeatChoices << hbTrId("txt_calendar_setlabel_repeat_val_only_once")
		        << hbTrId("txt_calendar_setlabel_repeat_val_daily")
		        << hbTrId("txt_calendar_setlabel_repeat_val_weekly")
		        << hbTrId("txt_calendar_setlabel_repeat_val_fortnightly")
		        << hbTrId("txt_calendar_setlabel_repeat_val_monthly")
		        << hbTrId("txt_calendar_setlabel_repeat_val_yearly");
		mRepeatComboBox->addItems(repeatChoices);
		// Set the user roles for the combobox items so that we depend on these
		// roles to identify the correct repeat type when repeat choices are 
		// dynamically removed or added
		mRepeatComboBox->setItemData(RepeatOnce, RepeatOnce, Qt::UserRole + 100);
		mRepeatComboBox->setItemData(RepeatDaily, RepeatDaily, 
		                             Qt::UserRole + 100);
		mRepeatComboBox->setItemData(RepeatWeekly, RepeatWeekly,
		                             Qt::UserRole + 100);
		mRepeatComboBox->setItemData(RepeatBiWeekly, RepeatBiWeekly,
		                             Qt::UserRole + 100);
		mRepeatComboBox->setItemData(RepeatMonthly, RepeatMonthly,
		                             Qt::UserRole + 100);
		mRepeatComboBox->setItemData(RepeatYearly, RepeatYearly, 
		                             Qt::UserRole + 100);

		// Now check if the duration of the meeting and remove the repeat choices 
		// if necessary
		int duration =
		        mEditedEntry->startTime().daysTo(mEditedEntry->endTime());
		if (mEditedEntry->endTime() >= (mEditedEntry->startTime().addYears(1))) {
			// Remove all options except "RepeatOnce"
			// Should be deletd in the descending order only
			mRepeatComboBox->removeItem(RepeatYearly);
			mRepeatComboBox->removeItem(RepeatMonthly);
			mRepeatComboBox->removeItem(RepeatBiWeekly);
			mRepeatComboBox->removeItem(RepeatWeekly);
			mRepeatComboBox->removeItem(RepeatDaily);
		} else if (mEditedEntry->endTime()
		        >= (mEditedEntry->startTime().addMonths(1))) {
			// Remove all the options except "Repeat Once"
			// and "Repeat Yearly" options
			// Should be deletd in the descending order only
			mRepeatComboBox->removeItem(RepeatMonthly);
			mRepeatComboBox->removeItem(RepeatBiWeekly);
			mRepeatComboBox->removeItem(RepeatWeekly);
			mRepeatComboBox->removeItem(RepeatDaily);
		} else if (duration >= 14) {
			// Remove daily, weekly and biweekly options
			// Should be deletd in the descending order only
			mRepeatComboBox->removeItem(RepeatBiWeekly);
			mRepeatComboBox->removeItem(RepeatWeekly);
			mRepeatComboBox->removeItem(RepeatDaily);
		} else if (duration >= 7) {
			// Remove daily and weekly options
			// Should be deletd in the descending order only
			mRepeatComboBox->removeItem(RepeatWeekly);
			mRepeatComboBox->removeItem(RepeatDaily);
		} else if (duration >= 1) {
			// Remove ont daily option
			mRepeatComboBox->removeItem(RepeatDaily);
		}
	// Set the previous user's choice
	mRepeatComboBox->setCurrentIndex(choice);
	handleRepeatIndexChanged(choice);
	}
}
/*!
 * Show delete confirmation query
 */
int CalenEditorPrivate::showDeleteConfirmationQuery()
{
	int retStatus = 0;

	HbMessageBox popup(HbMessageBox::MessageTypeQuestion);
	popup.setDismissPolicy(HbDialog::NoDismiss);
	popup.setTimeout(HbDialog::NoTimeout);
	popup.setIconVisible(true);

	QString text = 0;

	switch (mOriginalEntry->type()) {
		case AgendaEntry::TypeAppoinment:
		case AgendaEntry::TypeEvent: {
			if (mEditRange == ThisAndAll) {
				text.append(hbTrId("txt_calendar_info_delete_entries"));
			} else {
				text.append(hbTrId("txt_calendar_info_delete_meeting"));
			}
			break;
		}
		case AgendaEntry::TypeAnniversary: {
			text.append(hbTrId("txt_calendar_info_delete_anniversary"));
			break;
		}
	}

	popup.setText(text);

	popup.setPrimaryAction(new HbAction(hbTrId("txt_calendar_button_delete"),
	                                    &popup));
	popup.setSecondaryAction(new HbAction(hbTrId("txt_calendar_button_cancel"),
	                                      &popup));
	HbAction *selected = popup.exec();
	if (selected == popup.primaryAction()) {
		retStatus = 1;
	}

	return retStatus;
}

/*!
	Returns true if the entry is a child ,else false.
	\return true if the entry is a child, false otherwise.
 */
bool CalenEditorPrivate::isChild() const
{
	return mEditedEntry->recurrenceId().toUTC().isNull();
}

/*!
	Check for errors in the form. This function should only be called when the
	user has decided they want to edit "This and all" from a prompt, or when the
	entry is not repeating.
	\return enum Error which shows type of error.
 */
CalenEditorPrivate::Error CalenEditorPrivate::checkErrorsForThisAndAll()
{
	const QDateTime startTime = mEditedEntry->startTime();
	const QDateTime endTime = mEditedEntry->endTime();

	// Repeating entry checks:
	if (mEditedEntry->isRepeating()) {
		// Check that repeat until date is a) later than start date
		//										(for new notes)
		//									b) not before start date
		//										(for existing notes)
		QDate repeatUntilDay = mEditedEntry->repeatRule().until();

		QDate repeatStartDay;

		// if new note or old note isnt repeating
		// edited.repeatUntil date must be greater than edited.start date
		// else
		// if IsRepeatRuleEdited or IsStartDateTimeEdited 
		// (either one above will make a new rule in which edited.startdate
		// is the start date)
		// edited.repeatUntil must be greater than edited.start date
		// else
		// edited.repeatUntil must be greater than start date on disk

		if (mNewEntry || mOriginalEntry->repeatRule().isNull()
				|| isRepeatRuleEdited() || isStartDateTimeEdited()) {
			// We don't have an rrule so we can't get the rrule start date,
			// or user has edited a field that will cause new start date to be
			// used in the new rule.
			// Use the edited entry's start date.
			repeatStartDay = startTime.date();
		} else {
			// original rule is valid and new rule will not be created
			repeatStartDay = mOriginalEntry->repeatRule().repeatRuleStart();
		}

		if (durationGreaterThanRepeatIntervalError()) {
			return 
			CalenEditorPrivate::CalenEditorErrorDurationGreaterThanRepeatInterval;
		}
		return CalenEditorPrivate::CalenEditorErrorNone;
	}
	return CalenEditorPrivate::CalenEditorErrorNone;
}

/*!
	Returns true if the entry has been modified, false otherwise.
	\return true if the entry has been modified, false otherwise.
 */
bool CalenEditorPrivate::isEdited() const
{
	return (isSummaryEdited() ||
			isAllDayEdited() ||
			isLocationEdited() ||
			isStartDateTimeEdited() ||
			isEndDateTimeEdited() ||
			isAlarmEdited() ||
			isRepeatRuleEdited() ||
			isDescriptionEdited());
}

/*!
	Returns true if the summary has been edited, false otherwise.
	\return true if the summary has been edited, false otherwise.
 */
bool CalenEditorPrivate::isSummaryEdited() const
{
	return (mOriginalEntry->summary() != mEditedEntry->summary());
}

/*!
	Returns true if the all day has been edited, false otherwise.
	\return true if the all day has been edited, false otherwise.
 */
bool CalenEditorPrivate::isAllDayEdited() const
{
	if (mAllDayCheckBoxItem) {
		if (mOriginalEntry->type() == AgendaEntry::TypeEvent) {
			if (mAllDayCheckBoxItem->contentWidgetData("checkState")
			        == Qt::Checked) {
				return false;
			} else {
				return true;
			}
		} else if (mOriginalEntry->type() == AgendaEntry::TypeAppoinment) {
			if (mAllDayCheckBoxItem->contentWidgetData("checkState")
			        == Qt::Checked) {
				return true;
			} else {
				return false;
			}
		}
	}
	return false;
}

/*!
	Returns true if the location has been edited, false otherwise.
	\return true if the location has been edited, false otherwise.
 */
bool CalenEditorPrivate::isLocationEdited() const
{
	return (mOriginalEntry->location() != mEditedEntry->location());
}

/*!
	Returns true if the start date/time has been edited, false otherwise.
	\return true if the start date/time has been edited, false otherwise.
 */
bool CalenEditorPrivate::isStartDateTimeEdited() const
{
	return (mOriginalEntry->startTime() != mEditedEntry->startTime());
}

/*!
	Returns true if the end date/time has been edited, false otherwise.
	\return true if the end date/time has been edited, false otherwise.
 */
bool CalenEditorPrivate::isEndDateTimeEdited() const
{
	return (mOriginalEntry->endTime() != mEditedEntry->endTime());
}

/*!
	Returns true if the alarm has been edited, false otherwise.
	\return true if the alarm has been edited, false otherwise.
 */
bool CalenEditorPrivate::isAlarmEdited() const
{
	return (mOriginalEntry->alarm() != mEditedEntry->alarm());
}

/*!
	Returns true if the repeat rule has been edited, false otherwise.
	\return true if the repeat rule has been edited, false otherwise.
 */
bool CalenEditorPrivate::isRepeatRuleEdited() const
{
	if ((mOriginalEntry->repeatRule().type() == AgendaRepeatRule::InvalidRule)
	        && (mEditedEntry->repeatRule().type()
	                == AgendaRepeatRule::InvalidRule)) {
		return false;
	} else {
		return (mOriginalEntry->repeatRule() != mEditedEntry->repeatRule());
	}
}

/*!
	Returns true if the Description field has been edited, false otherwise.
	\return true if the Description field has been edited, false otherwise.
 */
bool CalenEditorPrivate::isDescriptionEdited() const
{
	return (mOriginalEntry->description() != mEditedEntry->description());
}

/*!
	Returns true if any of the non-text items (e.g. time fields) of the entry
	have been edited, false otherwise.
	\return true if any of the non text items edited,false otherwise.
 */
bool CalenEditorPrivate::nonTextItemsEdited() const
{
	return (isAllDayEdited() ||
			isStartDateTimeEdited() ||
			isEndDateTimeEdited() ||
			isAlarmEdited() ||
			isRepeatRuleEdited());
}

/*!
	Returns true if summary && location && description text items are all empty,
	false otherwise.
	\return true if text items are all empty,false otherwise.
 */
bool CalenEditorPrivate::areTextItemsEmpty() const
{
	return (mEditedEntry->summary().isEmpty()
	        && mEditedEntry->location().isEmpty()
	        && mEditedEntry->description().isEmpty());
}

/*!
	Returns true if the user cleared the text in the location and summary items,
	false otherwise.
	\return true if summary & location items are cleared,false otherwise.
 */
bool CalenEditorPrivate::areTextItemsCleared() const
{
	if (mEditedEntry->summary().isEmpty() &&
			mEditedEntry->location().isEmpty() &&
			mEditedEntry->description().isEmpty()) {
		if (isSummaryEmptied() 
				|| isLocationEmptied() 
				|| isDescriptionEmptied()) {
			return true;
		}
	}
	return false;
}

/*!
	Returns true the summary was not empty in original && is empty
	in the edited note,false otherwise
	\return true if summary is cleared in edited note,false otherwise
 */
bool CalenEditorPrivate::isSummaryEmptied() const
{
	return (!mOriginalEntry->summary().isEmpty()
	        && mEditedEntry->summary().isEmpty());
}

/*!
	Returns true the location was not empty in original && is empty
	in the edited note,false otherwise
	\return true if location is cleared in edited note,false otherwise
 */
bool CalenEditorPrivate::isLocationEmptied() const
{
	return (!mOriginalEntry->location().isEmpty()
	        && mEditedEntry->location().isEmpty());
}

/*!
	Returns true the description was not empty in original && is empty
	in the edited note,false otherwise
	\return true if description is cleared in edited note,false otherwise
 */
bool CalenEditorPrivate::isDescriptionEmptied() const
{
	return (!mOriginalEntry->description().isEmpty()
	        && mEditedEntry->description().isEmpty());
}

/*!
	Works out whether the entry should be deleted, saved, 
	or whether no action should be taken.
	\return enum Action
 */
CalenEditorPrivate::Action CalenEditorPrivate::shouldSaveOrDeleteOrDoNothing() 
																		const
{
	// Need to save the entry if third party calls editor to launch the
	// calendar after that. So, that entry will be new entry adn we assume
	// that client launches editor with some prefilled text items
	if (!isEdited() && !mLaunchCalendar) {
		// Not edited at all OR
		// Only added space characters to text fields but not
		// edited the non-text items
		// no need to save the entry
		return CalenEditorPrivate::ActionNothing;
	}
	// new entry is edited
	if (mNewEntry) {
		// Subject && Location && Description are text items.
		// If text items as a whole is not empty, we can save the note
		// If text items as a whole is empty, we can still save the note
		// since we edited "non-text" fields
		if (!nonTextItemsEdited() && areTextItemsEmpty()) {
			return CalenEditorPrivate::ActionNothing;
		} else {
			return CalenEditorPrivate::ActionSave;
		}
	}
	if (areTextItemsCleared() && !nonTextItemsEdited()) {
		// ***** edited entry + text items emptied + non-text items not edited
		// Even if user may have edited non-text fields, 
		// delete the note 
		return CalenEditorPrivate::ActionDelete;
	}
	// Save the note, since the text fields contain something
	return CalenEditorPrivate::ActionSave;
}

/*!
	Returns true if the duration of instances of the meeting is greater than
	the repeat period of the series, false otherwise.
	\return true if duration of meeting is greater than repeat period, false
	otherwise
 */
bool CalenEditorPrivate::durationGreaterThanRepeatIntervalError() const
{
	bool isError = false;
	switch (mEditedEntry->repeatRule().type()) {
		case AgendaRepeatRule::DailyRule: {
			int durationDays =
			        mEditedEntry->startTime().daysTo(mEditedEntry->endTime());
			isError = durationDays >= 1;
		}
		break;
		case AgendaRepeatRule::WeeklyRule: {
			int durationDays =
			        mEditedEntry->startTime().daysTo(mEditedEntry->endTime());
			if (mEditedEntry->repeatRule().interval() == 1) {
				isError = durationDays >= 7;
			} else {
				isError = durationDays >= 14;
			}
		}
		break;
		case AgendaRepeatRule::MonthlyRule: {
			if (mEditedEntry->endTime()
			        >= (mEditedEntry->startTime().addMonths(1))) {
				isError = true;
			}
		}
		break;
		case AgendaRepeatRule::YearlyRule: {
			if (mEditedEntry->endTime()
			        >= (mEditedEntry->startTime().addYears(1))) {
				isError = true;
			}
		}
		break;
		default:
			// Not repeating, no error
			isError = false;
			break;
	}
	return isError;
}

/*!
	Check the alarm fields for errors.
	\return the error if found, or CalenEditorErrorNone if no error found.
 */
CalenEditorPrivate::Error CalenEditorPrivate::checkAlarmFieldsForErrors(
															bool series) const
{
	Error error = CalenEditorErrorNone;
	// If alarm not active, no check
	if (!mEditedEntry->alarm().isNull()) {
		int alarm = mEditedEntry->alarm().timeOffset();
		QDateTime startTime = mEditedEntry->startTime();
		QDateTime alarmTime;
		if (alarm > 0) {
			alarmTime = startTime.addSecs(-alarm * 60);
		} else {
			alarmTime = startTime.addSecs(alarm * 60);
		}
		QDateTime currentTime = CalenDateUtils::now();
		if (isAlarmInAcceptablePeriod(error, alarmTime, startTime)) {
			if (!series && (alarmTime < currentTime)) {
				// dont let non-repeating future entries have alarms in past
				error = CalenEditorErrorAlarmTimePast;
			}
		}
	}
	return error;
}

/*!
	Checks if AlarmTime is 31 days from StartTime, 
	then sets Error to CalenEditorErrorAlarmDateTooManyDaysBeforeNote and
	returns false
	Checks if AlarmTime is later StartTime, 
	then sets Error to CalenEditorErrorAlarmTimeLaterThanNote and returns false
	\return true if error untouched, false otherwise 
 */
bool CalenEditorPrivate::isAlarmInAcceptablePeriod(Error &error,
										const QDateTime &alarmTime,
										const QDateTime &startTime) const
{
	QDateTime upperLimit = startTime;

	QDateTime lowerLimit = startTime.addDays(-31);
	bool acceptable = true;
	if (alarmTime < lowerLimit) {
		acceptable = false;
		error = CalenEditorErrorAlarmDateTooManyDaysBeforeNote;
	} else {
		if (alarmTime > upperLimit) {
			acceptable = false;
			error = CalenEditorErrorAlarmTimeLaterThanNote;
		}
	}
	return acceptable;
}

/*!
	Handle "Done". Usually saves, but can also delete or do nothing.
	\return true to inform whether editor should be closed or not,
	false otherwise
 */
CalenEditorPrivate::Action CalenEditorPrivate::handleDone()
{
	updateEditedEntry();
	// TODO: Need to check entry status here. EntryStillExistsL
	switch (shouldSaveOrDeleteOrDoNothing()) {
		case CalenEditorPrivate::ActionSave:
			if (saveEntry()) {
				return CalenEditorPrivate::ActionSave;
			}
			return CalenEditorPrivate::ActionNothing;
		case CalenEditorPrivate::ActionDelete:
			deleteEntry();
			return CalenEditorPrivate::ActionDelete;
		case CalenEditorPrivate::ActionNothing:
			return CalenEditorPrivate::ActionNothing;
		default:
			break;
	}
	return CalenEditorPrivate::ActionNothing;
}

/*!
	Updates the edited entry
 */
void CalenEditorPrivate::updateEditedEntry()
{
	if (mEditRange == ThisAndAll) {
		// saves repeat type of entry.
		if (mRepeatRuleType != AgendaRepeatRule::InvalidRule) {
			AgendaRepeatRule repeatRule(mRepeatRuleType);

			//TODO : Set the repeat from and to dates
			QVariant dateVariant =
			        mCustomRepeatUntilItem->contentWidgetData("text");
			QString dateString = dateVariant.toString();
			QDate untilDate = QDate::fromString(dateString, "dd/MM/yyyy");
			repeatRule.setRepeatRuleStart(mEditedEntry->startTime().date());
			repeatRule.setInterval(1);
			repeatRule.setUntil(mRepeatUntilDate);

			// need to set the day for weekly & monthly repeat rule.
			if (mRepeatRuleType == AgendaRepeatRule::WeeklyRule) {
				QList<AgendaRepeatRule::Day> days;
				if (mIsBiWeekly) {
					repeatRule.setInterval(2);
					mIsBiWeekly = false;
				}
				int dayOfWeek = mEditedEntry->startTime().date().dayOfWeek();
				days.append(AgendaRepeatRule::Day(dayOfWeek - 1));
				repeatRule.setByDay(days);
			} else if (mRepeatRuleType == AgendaRepeatRule::MonthlyRule) {
				QList<int> monthDays;
				//TODO :
				int dayNoInMonth = mEditedEntry->startTime().date().day();
				monthDays.append(dayNoInMonth);
				repeatRule.setByMonthDay(monthDays);
			} else if (mRepeatRuleType == AgendaRepeatRule::YearlyRule) {
				//TODO : Add yearly rule.Check if required.
			}
			mEditedEntry->setRepeatRule(repeatRule);
		} else {
			mEditedEntry->setRepeatRule( AgendaRepeatRule(
												AgendaRepeatRule::InvalidRule));
		}
		// TODO: Need to update rDates here
	}
}

/*!
	Save the entry
	\return true if entry is saved ,false otherwise
 */
bool CalenEditorPrivate::saveEntry()
{
	// check if we are editing child
	if (mIsChild && (mEditRange == ThisOnly)) {
		// Add the entry
		mAgendaUtil->updateEntry(*mEditedEntry, true);
		// TODO: Add the text id based on meeting or anniversary
		HbNotificationDialog::launchDialog(hbTrId("Event updated"));
		emit q_ptr->entrySaved();
		return true;
	} else if ((mEditRange == ThisOnly)) {
		// If we are editing only this occurence, then 
		// clear the repeating properties of it
		mAgendaUtil->clearRepeatingProperties(*mEditedEntry);
	}

	CalenEditorPrivate::Error error = CalenEditorPrivate::CalenEditorErrorNone;
	error = checkErrorsForThisAndAll();
	if (CalenEditorPrivate::CalenEditorErrorNone == error) {
		if (!handleAllDayToSave()) {
			if (mNewEntry) {
				mAgendaUtil->addEntry(*mEditedEntry);
			} else if (mEditRange == ThisAndAll) {
				mAgendaUtil->storeRepeatingEntry(*mEditedEntry, true);
			} else if (!mIsChild && (mEditRange == ThisOnly)) {
				// Create the new exception
				mAgendaUtil->createException(*mEditedEntry);
			} else {
				// Normal entry updation
				mAgendaUtil->updateEntry(*mEditedEntry, false);
			}
		}
		if (mNewEntry) {
			// TODO: Add the text id based on meeting or anniversary
			HbNotificationDialog::launchDialog(hbTrId("New Event saved"));
		} else {
			// TODO: Add the text id based on meeting or anniversary
			HbNotificationDialog::launchDialog(hbTrId("Event updated"));
		}
		emit q_ptr->entrySaved();
	} else if (error) {
		displayErrorMsg(error);
		return false;
	}
	return true;

}

/*!
	Delete the entry 
	@param closeEditor set true to close editor else default value is false
 */
void CalenEditorPrivate::deleteEntry(bool close)
{

	// if editor is launched from Notes then don't delete entry
	// Just exit from calendar editor
	if (mOriginalEntry->id() > 0) {

		// If user is editing single instanc then delete single instance 
		// else delete entry
		if (showDeleteConfirmationQuery()) {
			if (mEditRange == ThisOnly) {
				// User wants to delete only this occurence
				mAgendaUtil->deleteRepeatedEntry(*mOriginalEntry,
				                                 AgendaUtil::ThisOnly);
			} else {
				// Delete the entry.
				mAgendaUtil->deleteEntry(mOriginalEntry->id());
			}

			if (close) {
				closeEditor();
			}
		}
	}

}

/*!
	Display the given error msg
 */
void CalenEditorPrivate::displayErrorMsg(int error)
{
	QString errorMsg = QString::Null();

	switch (error) {
		case CalenEditorPrivate::CalenEditorErrorAlarmTimeLaterThanNote:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_alarm_later_than_note"));
			break;
		case CalenEditorPrivate::CalenEditorErrorAlarmTimePast:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_the_time_for_the_note_alarm"));
			break;
		case CalenEditorPrivate::CalenEditorErrorAlarmDateTooManyDaysBeforeNote:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_alarm_date_is_too_past"));
			break;
		case CalenEditorPrivate::CalenEditorErrorRepeatUntilEarlierThanNote:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_repeat_until_has_to_be_later"));
			break;
		case 
		CalenEditorPrivate::CalenEditorErrorDurationGreaterThanRepeatInterval:
			dispalyErrorMsgByRepeatType();
			break;
		case CalenEditorPrivate::CalenEditorErrorStopTimeEarlierThanStartTime:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_note_ends_before_than_starts"));
			break;
		default:
			break;
	}
	if (!errorMsg.isNull()) {
		HbMessageBox::information(errorMsg);
	}
}

/*!
	Display conflict error message regarding repeat type
 */
void CalenEditorPrivate::dispalyErrorMsgByRepeatType()
{
	QString errorMsg = QString::Null();

	int durationDays =
	        mEditedEntry->startTime().daysTo(mEditedEntry->endTime());
	int numDaysEntrySpan = durationDays + 1;
	// Add the text proper text ids
	switch (mEditedEntry->repeatRule().type()) {
		case AgendaRepeatRule::DailyRule:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_l1_day_meeting_cant_daily"));
			break;
		case AgendaRepeatRule::WeeklyRule:
			if (mEditedEntry->repeatRule().interval() == 1) {
				errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_l1_day_meeting_cant_weekly"));
			} else {
				errorMsg.append("meeting duration is more than 2 weeks");
			}
			break;
		case AgendaRepeatRule::MonthlyRule:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_l1_day_meeting_cant_monthly"));
			break;
		case AgendaRepeatRule::YearlyRule:
			errorMsg.append( hbTrId(
						"txt_calendar_dpopinfo_l1_day_meeting_cant_yearly"));
			break;
		default:
			break;
	}
	if (!errorMsg.isNull()) {
		HbMessageBox::information(errorMsg.arg(numDaysEntrySpan));
	}
}

/*!
 Change the entry before saving, depending on AllDay checkbox state
 */
bool CalenEditorPrivate::handleAllDayToSave()
{
	if (!mIsAllDayItemAdded) {
		// All day item was not added, return true
		return false;
	}
	// Check the state of AllDay checkBox

	// If Creating new Entry and AllDay Box is checked.
	// Clone the entry to AllDay 

	QDateTime tempSartTime =
	        CalenDateUtils::beginningOfDay(mEditedEntry->startTime());

	// Set EndTime of AllDay event to 23:59:59
	QDateTime tempEndTime = mEditedEntry->endTime();
	QTime tempEndQTime = tempEndTime.time();
	tempEndQTime.setHMS(23, 59, 59);
	tempEndTime.setTime(tempEndQTime);

	if (mNewEntry && (mAllDayCheckBoxItem->contentWidgetData("checkState")
	        == Qt::Checked)) {
		// changes Start/End times of entry to Beginning ot the day
		mEditedEntry->setStartAndEndTime(tempSartTime, tempEndTime);
		mEditedEntry->setType(AgendaEntry::TypeEvent);
		return false;
	} else if ((mAllDayCheckBoxItem->contentWidgetData("checkState")
	        == Qt::Checked) && (mEditedEntry->type() != 
								AgendaEntry::TypeEvent)) {
		// Editing existing Meetingentry, and Alldat Box is Checked
		// changes Start/End times of entry to Beginning ot the day
		mEditedEntry->setStartAndEndTime(tempSartTime, tempEndTime);

		// Clone the entry to AllDayEntry, Delete old entry from Database
		mAgendaUtil->cloneEntry(*mEditedEntry, AgendaEntry::TypeEvent);
		mAgendaUtil->deleteEntry(mEditedEntry->id());

		return true;
	} else if ((mAllDayCheckBoxItem->contentWidgetData("checkState")
	        != Qt::Checked) && (mEditedEntry->type()
	        != AgendaEntry::TypeAppoinment)) {
		// Editing exissting AllDayentry, and Alldat Box is Not-Checked
		// Clone the entry to MeetingEntry, Delete old entry from Database
		mAgendaUtil->cloneEntry(*mEditedEntry, AgendaEntry::TypeAppoinment);
		mAgendaUtil->deleteEntry(mEditedEntry->id());

		return true;
	}
	return false;
}

/*!
 Enable/Disable TimeFields depending on AllDay checkbox state
 */
void CalenEditorPrivate::enableFromTotimeFileds(bool enableFileds,
                                                QDateTime fromTime,
                                                QDateTime toTime)
{
	mViewFromItem->enableFromTimeFieldAndSetTime(enableFileds, fromTime);
	mViewToItem->enableToTimeFieldAndSetTime(enableFileds, toTime);

}
// End of file	--Don't remove this.
