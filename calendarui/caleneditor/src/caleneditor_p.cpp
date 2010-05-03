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
#include "caleneditordatahandler.h"
#include "caleneditorreminderfield.h"
#include "caleneditorrepeatfield.h"

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
									mEditorDocLoader(NULL),
									mEditorView(NULL),
									mDataHandler(NULL),
									mCalenEditorForm(NULL),
									mCalenEditorModel(NULL),
									mSubjectItem(NULL),
									mViewFromItem(NULL),
									mViewToItem(NULL),
                                    mViewLocationItem(NULL),
									mAllDayCheckBoxItem(NULL),		                            
									mReminderField(NULL),
									mRepeatField(NULL),
									mDescriptionItem(NULL),
									mEditRange(ThisAndAll),
									mOriginalEntry(NULL),
									mEditedEntry(NULL),
									mNewEntry(true),
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
	
	// Create the data handler
	mDataHandler = new CalenEditorDataHandler(this,mEditedEntry, mOriginalEntry);
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
	addCustomItemLocation();
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

/*!
	Appends the custom Location widget to the Data form Model.
 */
void CalenEditorPrivate::addCustomItemLocation()
{
	HbDataFormModelItem::DataItemType
    itemType =
            static_cast<HbDataFormModelItem::DataItemType> (CustomWidgetLocation);
    HbDataFormModelItem *customModelItem = new HbDataFormModelItem(itemType, hbTrId("txt_calendar_formlabel_val_location"));
    mCalenEditorModel->appendDataFormItem(customModelItem);
}

/*!
	Appends the repeat item to the Data form Model.
 */
void CalenEditorPrivate::addRepeatItem()
{
	// Create the editor reminder field class to handle reminder related
	// features
	mRepeatField = new CalenEditorRepeatField(this, mCalenEditorForm, 
												  mCalenEditorModel, this);
}
/*!
 Appends the reminder item to the Data form Model.
 */
void CalenEditorPrivate::addReminderItem()
{
	// Create the editor reminder field class to handle reminder related
	// features
	mReminderField = new CalenEditorReminderField(this, mCalenEditorForm, 
	                                              mCalenEditorModel, this);
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
	populateCustomItemLocation();
	mReminderField->populateReminderItem(mNewEntry);

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
		int minutes = 0;
		int hour = currentTime.hour();
		if (isSameDay) {
			minutes = currentTime.minute();
			if (minutes > 0 && minutes < 31) {
				minutes = 30;
			} else {
				minutes = 0;
			}
			currentTime.setHMS(hour, minutes, 0, 0);
			if (minutes == 0) {
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
	Populate location item from the editor model and set it in the widget and listen 
	text change happend in the widget. 
*/
void CalenEditorPrivate::populateCustomItemLocation()
{
	//Get the index of the custom location  widget item of the event item.
	// Check if all day has been added or not 
	// and calculate the index accordingly
	int itemIndex;
	if (mIsAllDayItemAdded) {
		itemIndex = LocationItem;
	}
	else {
		itemIndex = LocationItem - 1;
	}
	QModelIndex index = mCalenEditorModel->index(itemIndex, 0);
	mViewLocationItem = qobject_cast<CalenEditorCustomItem *> 
	                      (mCalenEditorForm->dataFormViewItem(index));        
                
	connect(mViewLocationItem, SIGNAL(locationTextChanged(const QString)),
			this, SLOT(handleLocationChange(const QString)));
                    
	mViewLocationItem->populateLocation(mEditedEntry->location());
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
	
	if (mEditRange == ThisAndAll) {
		// update the repeat choices depending on the meeting duration
		mRepeatField->updateRepeatChoices();
	}
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

	if (mEditRange == ThisAndAll) {
		// update the repeat choices depending on the meeting duration
		mRepeatField->updateRepeatChoices();
	}
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
	
	mRepeatField->populateRepeatItem(index);
}

/*!
	Populates the description item.
 */
void CalenEditorPrivate::populateDescriptionItem()
{
	QModelIndex repeatIndex;
	if (mEditRange == ThisOnly) {
		repeatIndex = mReminderField->modelIndex();
	} else {
		repeatIndex = mRepeatField->modelIndex();
	}

	int descIndex;
	if (mRepeatField && mRepeatField->isRepeatUntilItemAdded()) {
		descIndex = repeatIndex.row() + 2;
	} else {
		descIndex = repeatIndex.row() + 1;
	}
	mDescriptionItem
	        = mCalenEditorModel->insertDataFormItem(
									descIndex, HbDataFormModelItem::TextItem,
									QString(hbTrId(
									"txt_calendar_formlabel_val_description")),
									mCalenEditorModel->invisibleRootItem());
	mDescriptionItem->setContentWidgetData("text", mEditedEntry->description());
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
		repeatIndex = mReminderField->modelIndex();
	} else {
		repeatIndex = mRepeatField->modelIndex();
	}
	int descIndex;
	if (mRepeatField && mRepeatField->isRepeatUntilItemAdded()) {
		descIndex = repeatIndex.row() + 2;
	} else {
		descIndex = repeatIndex.row() + 1;
	}
	mCalenEditorModel->removeItem(mCalenEditorModel->index(descIndex, 0));
	// Remove the description from the entry
	mEditedEntry->setDescription("");

	mDescriptionItem = NULL;
	mDescriptionItemAdded = false;
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
	Q_UNUSED(state)
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

/*!
	Handle "Done". Usually saves, but can also delete or do nothing.
	\return true to inform whether editor should be closed or not,
	false otherwise
 */
CalenEditorPrivate::Action CalenEditorPrivate::handleDone()
{
	if (mEditRange == ThisAndAll) {
		mRepeatField->saveRepeatRule();
	}
	// TODO: Need to check entry status here. EntryStillExistsL
	switch (mDataHandler->shouldSaveOrDeleteOrDoNothing(mLaunchCalendar)) {
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
	error = mDataHandler->checkErrorsForThisAndAll();
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
		mDataHandler->displayErrorMsg(error);
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

/*!
 Returns pointer of entyr being edited
 */
AgendaEntry* CalenEditorPrivate::editedEntry()
{
	return mEditedEntry;
}

/*!
 Returns the pointer of original entry
 */
AgendaEntry* CalenEditorPrivate::originalEntry()
{
	return mOriginalEntry;
}

/*!
 Returns true if new entry being created else false
 */
bool CalenEditorPrivate::isNewEntry()
{
	return mNewEntry;
}

/*!
 Returns the pointer of all day check box item
 */
HbDataFormModelItem* CalenEditorPrivate::allDayCheckBoxItem()
{
	return mAllDayCheckBoxItem;
}
// End of file	--Don't remove this.
