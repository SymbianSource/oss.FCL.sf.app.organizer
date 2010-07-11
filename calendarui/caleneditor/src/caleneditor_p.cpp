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
#include <QApplication>

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
#include <hbgroupbox.h>
#include <hbradiobuttonlist.h>
#include <hbnotificationdialog.h>
#include <hbtranslator.h>

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
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "caleneditor_pTraces.h"
#endif


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
									mTranslator(new HbTranslator("caleneditor")),
									mNewEntry(true),
									mDescriptionItemAdded(false),
									mIsChild(false),
									mIsAllDayItemAdded(false),
									mLaunchCalendar(false),
									mMenuItemAdded(false)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_CALENEDITORPRIVATE_ENTRY );
	// First get the q-pointer.
	q_ptr = static_cast<CalenEditor *> (parent);
	mMainWindow = NULL;
	mTranslator->loadCommon();
	
	if (!agendaUtil) {
		mAgendaUtil = new AgendaUtil(this);
		mOwnsAgendaUtil = true;
	} else {
		mAgendaUtil = agendaUtil;
		mOwnsAgendaUtil = false;
	}
	OstTraceFunctionExit0( CALENEDITORPRIVATE_CALENEDITORPRIVATE_EXIT );
}

/*!
	Destructor.
 */
CalenEditorPrivate::~CalenEditorPrivate()
{
	OstTraceFunctionEntry0( DUP1_CALENEDITORPRIVATE_CALENEDITORPRIVATE_ENTRY );
	if(mOwnsAgendaUtil) {
		delete mAgendaUtil;
		mAgendaUtil = NULL;
	}
	if (mOriginalEntry) {
		delete mOriginalEntry;
		mOriginalEntry = NULL;
	}
	if (mEditedEntry) {
		delete mEditedEntry;
		mEditedEntry = NULL;
	}
	if(mEditorDocLoader) {
		delete mEditorDocLoader;
		mEditorDocLoader = NULL;
	}
	if(mReminderField) {
		delete mReminderField;
		mReminderField = NULL;
	}
	if(mRepeatField) {
		delete mRepeatField;
		mRepeatField = NULL;
	}
	if(mDataHandler) {
		delete mDataHandler;
		mDataHandler = NULL;
	}
	if (mCalenEditorModel) {
		delete mCalenEditorModel;
		mCalenEditorModel = NULL;
	}
	// Remove the translator
	if (mTranslator) {
		delete mTranslator;
		mTranslator = 0;
	}
	
	// delete the mainwindow object if created any
	if (mMainWindow) {
		delete mMainWindow;
		mMainWindow = NULL;
	}
	OstTraceFunctionExit0( DUP1_CALENEDITORPRIVATE_CALENEDITORPRIVATE_EXIT );
}

/*!
	Shows the caleneditor by parsing a .vcs.

	\param handle reference to QFile handle.
 */
void CalenEditorPrivate::edit(const QFile &/*handle*/, bool launchCalendar)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_EDIT_ENTRY );
	// TODO: Needs to be implemented using importer.
	mNewEntry = false;
	mLaunchCalendar = launchCalendar;
	OstTraceFunctionExit0( CALENEDITORPRIVATE_EDIT_EXIT );
}

/*!
	Shows the caleneditor, by parsing an AgendaEntry.
	\param entry An object of AgendaEntry.
 */
void CalenEditorPrivate::edit(AgendaEntry entry, bool launchCalendar)
{
	OstTraceFunctionEntry0( DUP1_CALENEDITORPRIVATE_EDIT_ENTRY );
	mNewEntry = false;
	mLaunchCalendar = launchCalendar;
	openEditor(entry);
	OstTraceFunctionExit0( DUP1_CALENEDITORPRIVATE_EDIT_EXIT );
}

/*!
	Shows the caleneditor, by using the entry id provided.
	\param id entry id.
 */
void CalenEditorPrivate::edit(ulong id, bool launchCalendar)
{
	OstTraceFunctionEntry0( DUP2_CALENEDITORPRIVATE_EDIT_ENTRY );
	mNewEntry = false;
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if(entry.isNull()) {
		OstTraceFunctionExit0( DUP2_CALENEDITORPRIVATE_EDIT_EXIT );
		return;
	}
	edit(entry, launchCalendar);
	OstTraceFunctionExit0( DUP3_CALENEDITORPRIVATE_EDIT_EXIT );
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
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_CREATE_ENTRY );
	mNewEntryDateTime = newEntryDateTime;
	mNewEntry = true;
	AgendaEntry entry;
	switch(type){
		case CalenEditor::TypeAppointment: {
			entry.setType(AgendaEntry::TypeAppoinment);
		}
		break;
		default:
			// What ever be the type of entry, currently editor supports only to
			// open the entries of TypeAppoinment
			entry.setType(AgendaEntry::TypeAppoinment);
			break;
	}
	mLaunchCalendar = launchCalendar;
	openEditor(entry);
	OstTraceFunctionExit0( CALENEDITORPRIVATE_CREATE_EXIT );
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
	OstTraceFunctionEntry0( DUP1_CALENEDITORPRIVATE_CREATE_ENTRY );
	mNewEntry = true;
	switch(type){
		case CalenEditor::TypeAppointment: {
			entry.setType(AgendaEntry::TypeAppoinment);
		}
		break;
		default:
			// What ever be the type of entry, currently editor supports only to
			// open the entries of TypeAppoinment
			entry.setType(AgendaEntry::TypeAppoinment);
			break;
	}
	mNewEntryDateTime = entry.startTime();
	mLaunchCalendar = launchCalendar;
	openEditor(entry);
	OstTraceFunctionExit0( DUP1_CALENEDITORPRIVATE_CREATE_EXIT );
}

/*!
	Queries user whether to edit whole series or just this single occurence
 */
void CalenEditorPrivate::showEditOccurencePopup()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_SHOWEDITOCCURENCEPOPUP_ENTRY );
	HbDialog *popUp = new HbDialog();
	popUp->setDismissPolicy(HbDialog::NoDismiss);
	popUp->setTimeout(HbDialog::NoTimeout);
	popUp->setAttribute( Qt::WA_DeleteOnClose, true );
	
	QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
	HbWidget *editWidget = new HbWidget();
	editWidget->setLayout(layout);

	HbRadioButtonList *editButtonList = new HbRadioButtonList();

	QStringList list;
	list << hbTrId("txt_calendar_info_this_occurrence_only")
	        << hbTrId("txt_calendar_info_all_occurences");

	editButtonList->setItems(list);

	layout->addItem(editButtonList);

	popUp->setContentWidget(editWidget);
	popUp->setHeadingWidget(new HbLabel(hbTrId("txt_calendar_title_edit")));

	// Create cancel action
	HbAction *cancelAction =
	        new HbAction(hbTrId("txt_calendar_button_softkey1_cancel"));
	popUp->addAction(cancelAction);
	connect(editButtonList, SIGNAL(itemSelected(int)), popUp, SLOT(close()));
	connect(editButtonList, SIGNAL(itemSelected(int)), this,
	        SLOT(handleEditOccurence(int)));
	connect(cancelAction, SIGNAL(triggered()), this, SLOT(handleCancel()));

	// Show the popup
	popUp->open();
	OstTraceFunctionExit0( CALENEDITORPRIVATE_SHOWEDITOCCURENCEPOPUP_EXIT );
}

/*!
	Slot to handle User selection for series editing or single occurence popup
 */
void CalenEditorPrivate::handleEditOccurence(int index)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_HANDLEEDITOCCURENCE_ENTRY );
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
	// If user has selected to edit all the occurences, 
	// then get the parent of it
	AgendaEntry entryToBeEdited = mEntry;
	if (mEditRange == ThisAndAll) {
		entryToBeEdited = mAgendaUtil->parentEntry(mEntry);
	}
	showEditor(entryToBeEdited);
	OstTraceFunctionExit0( CALENEDITORPRIVATE_HANDLEEDITOCCURENCE_EXIT );
}

/*!
	Slot to handle cancel action on the popup
 */
void CalenEditorPrivate::handleCancel()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_HANDLECANCEL_ENTRY );
	// User has chosen not to edit the event, hence return
	mEditRange = UserCancelled;
	// Do cleanup and return
	emit q_ptr->dialogClosed();
	OstTraceFunctionExit0( CALENEDITORPRIVATE_HANDLECANCEL_EXIT );
	return;
}

/*!
	Opens the editor.
 */
void CalenEditorPrivate::openEditor(AgendaEntry entry)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_OPENEDITOR_ENTRY );
	if (0 < entry.id()) {
		mNewEntry = false;
	}
	if (!mNewEntry) {
		// Before we do anything, check in the entry is repeating
		// OR its a child item
		mIsChild = !(entry.recurrenceId().isNull());
		bool isRepeating = entry.isRepeating();
		// For later reference
		mEntry = entry;
		if (mIsChild || isRepeating) {
			// Query user if he wants to edit whole series 
			// or just this occurence
			showEditOccurencePopup();
			OstTraceFunctionExit0( CALENEDITORPRIVATE_OPENEDITOR_EXIT );
			return;
		}else {
			mEditRange = ThisAndAll;
		}
	}
	showEditor(entry);
	OstTraceFunctionExit0( DUP1_CALENEDITORPRIVATE_OPENEDITOR_EXIT );
}

/*!
	Displays the editor.
 */
void CalenEditorPrivate::showEditor(AgendaEntry entry)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_SHOWEDITOR_ENTRY );
	mOriginalEntry = new AgendaEntry(entry);
	mEditedEntry = new AgendaEntry(entry);

	if (!mEditedEntry->description().isEmpty()) {
		mDescriptionItemAdded = true;
	}
	// Construct the view using docloader
	setUpView();

	// Set the title text. 
	mEditorView->setTitle(hbTrId("txt_calendar_title_calendar"));

	mSoftKeyAction = new HbAction(Hb::BackNaviAction);
	mEditorView->setNavigationAction(mSoftKeyAction);
	connect(mSoftKeyAction, SIGNAL(triggered()), this,
	        SLOT(saveAndCloseEditor()));

	// Create the data handler
	mDataHandler = new CalenEditorDataHandler(this,mEditedEntry, mOriginalEntry);
	OstTraceFunctionExit0( CALENEDITORPRIVATE_SHOWEDITOR_EXIT );
}

/*!
	Sets up the view.
 */
void CalenEditorPrivate::setUpView()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_SETUPVIEW_ENTRY );
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
	deleteEventAction->setText(hbTrId("txt_common_menu_delete"));
	connect(deleteEventAction, SIGNAL(triggered()), this,
							SLOT(showDeleteConfirmationQuery()));

	if (!mNewEntry) {
		AgendaEntry::Type entryType = mEditedEntry->type();
		if( entryType == AgendaEntry::TypeAppoinment) {
			headingWidget->setHeading(hbTrId("txt_calendar_subhead_meeting"));
		}else if(entryType == AgendaEntry::TypeEvent) {
			//TODO: Add the text id once available
			headingWidget->setHeading(hbTrId("All-day event"));
		}else if (entryType == AgendaEntry::TypeTodo) {
			headingWidget->setHeading(hbTrId("txt_calendar_subhead_to_do"));
		}
		
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
	
	OstTraceFunctionExit0( CALENEDITORPRIVATE_SETUPVIEW_EXIT );
}

/*!
	Adding the discard Menu option for the Editor view.
 */
void CalenEditorPrivate::addDiscardAction()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ADDDISCARDACTION_ENTRY );
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
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ADDDISCARDACTION_EXIT );
}

/*!
	Initialises the Model & sets for the CalenEditorForm.
 */
void CalenEditorPrivate::initModel()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_INITMODEL_ENTRY );
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
	if (mNewEntry || (mEditRange == ThisAndAll)) {
		addAllDayCheckBoxItem();
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
	OstTraceFunctionExit0( CALENEDITORPRIVATE_INITMODEL_EXIT );
}

/*!
	Appends the Subject Item to the Data form Model.
 */
void CalenEditorPrivate::addSubjectItem()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ADDSUBJECTITEM_ENTRY );
	// Creats and appends data form model item to set subject of entry.
	mSubjectItem = mCalenEditorModel->appendDataFormItem(
										HbDataFormModelItem::TextItem,
										hbTrId(
										"txt_calendar_formlabel_val_subject"),
										mCalenEditorModel->invisibleRootItem());
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ADDSUBJECTITEM_EXIT );
}

/*!
	Appends the allday item to the Data form Model.
 */
void CalenEditorPrivate::addAllDayCheckBoxItem()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ADDALLDAYCHECKBOXITEM_ENTRY );
	mAllDayCheckBoxItem = new HbDataFormModelItem();
	mAllDayCheckBoxItem->setType(HbDataFormModelItem::CheckBoxItem);
	mCalenEditorModel->appendDataFormItem(mAllDayCheckBoxItem);
	mIsAllDayItemAdded = true;
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ADDALLDAYCHECKBOXITEM_EXIT );
}

/*!
	Appends the start time of the event item to the Data form Model.
 */
void CalenEditorPrivate::addCustomItemFrom()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ADDCUSTOMITEMFROM_ENTRY );
	HbDataFormModelItem::DataItemType itemType =
	        static_cast<HbDataFormModelItem::DataItemType> (CustomWidgetFrom);
	HbDataFormModelItem *customModelItem = new HbDataFormModelItem(itemType,
								hbTrId("txt_calendar_setlabel_start_time"));
	mCalenEditorModel->appendDataFormItem(customModelItem);
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ADDCUSTOMITEMFROM_EXIT );
}

/*!
	Appends the end time of the event item to the Data form Model.
 */
void CalenEditorPrivate::addCustomItemTo()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ADDCUSTOMITEMTO_ENTRY );

	HbDataFormModelItem::DataItemType itemType =
	        static_cast<HbDataFormModelItem::DataItemType> (CustomWidgetTo);
	HbDataFormModelItem *customModelItem = new HbDataFormModelItem(itemType,
									hbTrId("txt_calendar_setlabel_end_time"));
	mCalenEditorModel->appendDataFormItem(customModelItem);
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ADDCUSTOMITEMTO_EXIT );
}

/*!
	Appends the Location Item to the Data form Model.
 */

/*!
	Appends the custom Location widget to the Data form Model.
 */
void CalenEditorPrivate::addCustomItemLocation()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ADDCUSTOMITEMLOCATION_ENTRY );
	HbDataFormModelItem::DataItemType
    itemType =
            static_cast<HbDataFormModelItem::DataItemType> (CustomWidgetLocation);
    HbDataFormModelItem *customModelItem = new HbDataFormModelItem(itemType, hbTrId("txt_calendar_formlabel_val_location"));
    mCalenEditorModel->appendDataFormItem(customModelItem);
    OstTraceFunctionExit0( CALENEDITORPRIVATE_ADDCUSTOMITEMLOCATION_EXIT );
}

/*!
	Appends the repeat item to the Data form Model.
 */
void CalenEditorPrivate::addRepeatItem()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ADDREPEATITEM_ENTRY );
	// Create the editor reminder field class to handle reminder related
	// features
	mRepeatField = new CalenEditorRepeatField(this, mCalenEditorForm, 
												  mCalenEditorModel, this);
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ADDREPEATITEM_EXIT );
}
/*!
 Appends the reminder item to the Data form Model.
 */
void CalenEditorPrivate::addReminderItem()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ADDREMINDERITEM_ENTRY );
	// Create the editor reminder field class to handle reminder related
	// features
	mReminderField = new CalenEditorReminderField(this, mCalenEditorForm, 
	                                              mCalenEditorModel, this);
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ADDREMINDERITEM_EXIT );
}

/*!
	Appends the description item to the Data form Model.
 */
void CalenEditorPrivate::handleDescriptionAction()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_HANDLEDESCRIPTIONACTION_ENTRY );
	if (!mDescriptionItemAdded) {
		populateDescriptionItem();
		mDescriptionAction->setText(
							hbTrId("txt_calendar_opt_remove_description"));
	} else {
		removeDescriptionItem();
		mDescriptionAction->setText(
							hbTrId("txt_calendar_opt_add_description"));
	}
	OstTraceFunctionExit0( CALENEDITORPRIVATE_HANDLEDESCRIPTIONACTION_EXIT );
}

/*!
	Poplulates the Model.
 */
void CalenEditorPrivate::populateModel()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_POPULATEMODEL_ENTRY );
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
	OstTraceFunctionExit0( CALENEDITORPRIVATE_POPULATEMODEL_EXIT );
}

/*!
	Populates the Subject Item in the Data From Model.
 */
void CalenEditorPrivate::populateSubjectItem()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_POPULATESUBJECTITEM_ENTRY );
	mSubjectItem->setContentWidgetData("text", mEditedEntry->summary());
	mSubjectItem->setContentWidgetData("minRows", 2);
	mSubjectItem->setContentWidgetData("maxRows", 4);
	mSubjectItem->setContentWidgetData("objectName", "subjectItem");
	mCalenEditorForm->addConnection(mSubjectItem,
	                                SIGNAL(textChanged(const QString)), this,
	                                SLOT(handleSubjectChange(const QString)));
	OstTraceFunctionExit0( CALENEDITORPRIVATE_POPULATESUBJECTITEM_EXIT );
}

/*!
	Populates the all day item.
 */
void CalenEditorPrivate::populateAllDayItem()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_POPULATEALLDAYITEM_ENTRY );
	mAllDayCheckBoxItem->setContentWidgetData("text",
									hbTrId("txt_calendar_list_all_day_event"));
	if (AgendaEntry::TypeEvent == mEditedEntry->type()) {
		//If All day is enabled.
		mAllDayCheckBoxItem->setContentWidgetData("checkState", Qt::Checked);
	}
	mAllDayCheckBoxItem->setContentWidgetData("objectName", "allDayItem");
	mCalenEditorForm->addConnection(mAllDayCheckBoxItem,
	                                SIGNAL(stateChanged(int )), this,
	                                SLOT(handleAllDayChange(int)));

	OstTraceFunctionExit0( CALENEDITORPRIVATE_POPULATEALLDAYITEM_EXIT );
}

/*!
	Populates the start and end time of the event item.
 */
void CalenEditorPrivate::populateCustomItemDateTime()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_POPULATECUSTOMITEMDATETIME_ENTRY );
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

			// set this to the original entry as well as edited entry
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
			
			// If no instances earlier then set it to 01/01/1900.
			if (prevInstanceStartTime.isNull()) {
				prevInstanceEndTime.setDate(QDate(1900, 01, 01));
				prevInstanceEndTime.setTime(QTime(0, 0, 0));
			} else {
				prevInstanceEndTime = prevInstanceEndTime.addDays(1);
			}
			
			// If no instances later then set it to 30/01/2100.
			if (nextInstanceEndTime.isNull()) {
				nextInstanceStartTime.setDate(QDate(2100, 12, 30));
				nextInstanceStartTime.setTime(QTime(0, 0, 0));
			} else {
				nextInstanceStartTime = nextInstanceStartTime.addDays(-1);
			}
			mViewFromItem->setDateRange(
									prevInstanceEndTime.date(),
									nextInstanceStartTime.date());
			mViewToItem->setDateRange(prevInstanceEndTime.date(),
									nextInstanceStartTime.date());
			
			// If repeating daily then disable the date fields as 
			// date cannot be changed
 			if ((prevInstanceEndTime.date().daysTo(
				mEditedEntry->startTime().date()) == 1) && 
				(mEditedEntry->endTime().date().daysTo(
				nextInstanceStartTime.date()) == 1)) {
				mViewFromItem->disableFromToDateField();
			}
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
	if ((mAllDayCheckBoxItem && 
		(mAllDayCheckBoxItem->contentWidgetData("checkState") == Qt::Checked))
		|| (!mNewEntry && mEditedEntry->type() == AgendaEntry::TypeEvent)) {
        
        // For all-day, we need to substratc 1 minute to get the actual end time
        // as we store all-day as 12.00AM to 12.00 AM next day
        QDateTime actualEndTime = mEditedEntry->endTime().addSecs(-60);
        mViewToItem->populateDateTime(actualEndTime, false);
        
		// If the all day option is checked, we need to
		// disable the time fields
		enableFromTotimeFileds(false, mEditedEntry->startTime(),
                                actualEndTime);
	}
	OstTraceFunctionExit0( CALENEDITORPRIVATE_POPULATECUSTOMITEMDATETIME_EXIT );
}
/*!
	Populate location item from the editor model and set it in the widget and listen 
	text change happend in the widget. 
*/
void CalenEditorPrivate::populateCustomItemLocation()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_POPULATECUSTOMITEMLOCATION_ENTRY );
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
									(mCalenEditorForm->itemByIndex(index));
	mViewLocationItem->populateLocation(mEditedEntry->location());
	
	// Connect the slots only after setting the location text
	connect(mViewLocationItem, SIGNAL(locationTextChanged(const QString)),
			this, SLOT(handleLocationChange(const QString)));
	
	connect(mViewLocationItem, 
		SIGNAL(locationTextChanged(const QString, const double, const double)),
		this, 
		SLOT(handleLocationChange(const QString, const double, const double)));

	connect(mViewLocationItem, SIGNAL(locationEditingFinished()),
				this, SLOT(handleLocationEditingFinished()));
	
	OstTraceFunctionExit0( CALENEDITORPRIVATE_POPULATECUSTOMITEMLOCATION_EXIT );
}
/*!
	Save the changed start time of the event.
 */
void CalenEditorPrivate::saveFromDateTime(QDateTime& fromDateTime)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_SAVEFROMDATETIME_ENTRY );
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

	updateReminderChoices();
	OstTraceFunctionExit0( CALENEDITORPRIVATE_SAVEFROMDATETIME_EXIT );
}

void CalenEditorPrivate::updateReminderChoices()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_UPDATEREMINDERCHOICES_ENTRY );
	QDate referenceDate;
	
	// Start date or repeat until date will be the reference to decide 
	// whether the event is in past or future.
	if (mRepeatField && mRepeatField->isRepeatUntilItemAdded()) {
		referenceDate = mRepeatField->repeatUntilDate();
	} else {
		referenceDate = mEditedEntry->startTime().date();
	}
	
	// Check if all day event or not.
	if (isAllDayEvent()) {
		int currentIndex = mReminderField->currentReminderIndex();
		mReminderField->updateReminderChoicesForAllDay(referenceDate);
		int numberOfReminderChoices = mReminderField->reminderItemsCount();
		if (currentIndex >= numberOfReminderChoices) {
			currentIndex = 0;
			mReminderField->disableReminderTimeField();
		} 
		mReminderField->setCurrentIndex(currentIndex);
		if(currentIndex == 0 && mReminderField->isReminderTimeForAllDayAdded()) {
			mReminderField->removeReminderTimeField();
		}
	} else { 
		if ((referenceDate < QDate::currentDate())
						|| (referenceDate == QDate::currentDate()
				&& (mEditedEntry->startTime().time() < QTime::currentTime())))
				 {
					mReminderField->setReminderOff();
		} else {
			// Enabled implies future. If changing from future to future do not 
			// do anything.
			if (!mReminderField->isReminderFieldEnabled()) {
				mReminderField->setReminderChoices();
			}
		}
	}
	OstTraceFunctionExit0( CALENEDITORPRIVATE_UPDATEREMINDERCHOICES_EXIT );
}

/*!
	Save the changed end time of the event.
 */
void CalenEditorPrivate::saveToDateTime(QDateTime& toDateTime)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_SAVETODATETIME_ENTRY );
	QDateTime startTime = mEditedEntry->startTime();
	bool fromDateChanged = false;
	// Update the start time accordingly on UI - duration will be 60 mins
	// bydefault for new entry and if it crosses the starttime
	if (mNewEntry && toDateTime < startTime) {
		startTime = toDateTime.addSecs(-3600);
		fromDateChanged = true;
	} else { 
		// for exisitng entry
		// we need to see if user has moved end time before the start time
		// then substract the duration of the meeting that was saved earlier to 
		// the new end time to get the new start time
		if (toDateTime < startTime) {
			int duration = mEditedEntry->durationInSecs();
			startTime = toDateTime.addSecs(-duration);
			fromDateChanged = true;
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
	
	// Update reminder choices if start time got changed.
	if (fromDateChanged) {
		updateReminderChoices();
	}
	OstTraceFunctionExit0( CALENEDITORPRIVATE_SAVETODATETIME_EXIT );
}

/*!
	Populates the repeat item.
 */
void CalenEditorPrivate::populateRepeatItem()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_POPULATEREPEATITEM_ENTRY );
	// Check if all day has been added or not 
	// and calculate the index accordingly
	// all day added implies reminder time field is also added
	int index;
	if (mIsAllDayItemAdded && !isReminderTimeForAllDayAdded()) {
		index = RepeatItem - 1;
	} else if (!mNewEntry && isReminderTimeForAllDayAdded()) {
		index = RepeatItem;
	} else {
		index = RepeatItem - 2;
	}
	
	mRepeatField->populateRepeatItem(index);
	OstTraceFunctionExit0( CALENEDITORPRIVATE_POPULATEREPEATITEM_EXIT );
}

/*!
	Populates the description item.
 */
void CalenEditorPrivate::populateDescriptionItem()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_POPULATEDESCRIPTIONITEM_ENTRY );
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
	mDescriptionItem->setContentWidgetData("objectName", "descriptionItem");
	mCalenEditorForm->addConnection(
								mDescriptionItem,
								SIGNAL(textChanged(const QString)), this,
								SLOT(handleDescriptionChange(const QString)));
	mDescriptionItemAdded = true;
    //Scroll to the description item added.
	mCalenEditorForm->scrollTo(mCalenEditorModel->index(descIndex, 0), HbAbstractItemView::EnsureVisible);
	OstTraceFunctionExit0( CALENEDITORPRIVATE_POPULATEDESCRIPTIONITEM_EXIT );
}

/*!
	Removes the description item.
 */
void CalenEditorPrivate::removeDescriptionItem()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_REMOVEDESCRIPTIONITEM_ENTRY );
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
	OstTraceFunctionExit0( CALENEDITORPRIVATE_REMOVEDESCRIPTIONITEM_EXIT );
}


/*!
	Close the Editor.
 */
void CalenEditorPrivate::closeEditor()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_CLOSEEDITOR_ENTRY );
	HbMainWindow* window = hbInstance->allMainWindows().first();
	window->removeView(mEditorView);
	emit q_ptr->dialogClosed();
	OstTraceFunctionExit0( CALENEDITORPRIVATE_CLOSEEDITOR_EXIT );
}

/*!
	Triggered when the subject editor is being edited.
	\param subject Contains the string displayed in the subject item.
 */
void CalenEditorPrivate::handleSubjectChange(const QString subject)
{
	mEditedEntry->setSummary(subject);
	if(!mNewEntry ){
		addDiscardAction();
	}
}

/*!
 Triggered when the Allday Checkbox is being edited.
 \param state of checkbox.
 */
void CalenEditorPrivate::handleAllDayChange(int state)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_HANDLEALLDAYCHANGE_ENTRY );
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
		QDate referenceDate;
		if (mRepeatField->isRepeatUntilItemAdded()) {
			referenceDate = mRepeatField->repeatUntilDate();
		} else {
			referenceDate = mEditedEntry->startTime().date();
		}
		mReminderField->updateReminderChoicesForAllDay(referenceDate);
		// If the reminder field is enabled and it is not off 
		// it implies default alarm day and time is being displayed.
		if (mReminderField->isReminderFieldEnabled() && 
					mReminderField->currentReminderIndex() != 0) {
			// Set the default alarm for all day.
			mReminderField->setDefaultAlarmForAllDay();
		} else {
			// Remove reminder time field.
			mReminderField->removeReminderTimeField();
		}
	} else {
		// AllDayCheckBox in un-checked
		// Set From/To times buttons editable
		// Update Start/End Times with Edited entry values
		enableFromTotimeFileds(true, mEditedEntry->startTime(),
		                       mEditedEntry->endTime());
		// If original entry was an All-day, then we need to save the date that
		// is shown on the "To" date push button
		if (mOriginalEntry->type() == AgendaEntry::TypeEvent) {
            mEditedEntry->setStartAndEndTime(mViewFromItem->getDateTime(),
                                    mViewToItem->getDateTime());
		}
		int index;
		if (mIsAllDayItemAdded) {
			index = ReminderTimeForAllDayItem;
		} else {
			index = ReminderTimeForAllDayItem - 1;
		}
		mReminderField->removeReminderTimeField();
		mReminderField->setReminderChoices();
		updateReminderChoices();
	}

	if(!mNewEntry){
		addDiscardAction();
	}
	OstTraceFunctionExit0( CALENEDITORPRIVATE_HANDLEALLDAYCHANGE_EXIT );
}

/*!
	Triggered when the location editor is being edited.
	\param subject Contains the string displayed in the subject item.
 */
void CalenEditorPrivate::handleLocationChange(const QString location)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_HANDLELOCATIONCHANGE_ENTRY );
	mEditedEntry->setLocation(location);
	if(!mNewEntry){
		addDiscardAction();
	}
	OstTraceFunctionExit0( CALENEDITORPRIVATE_HANDLELOCATIONCHANGE_EXIT );
}


/*!
	Triggered when the location editor is being edited.
	\param subject Contains the string displayed in the subject item.
 */
void CalenEditorPrivate::handleLocationChange(const QString location,
    const double /*geoLatitude*/, const double /*geoLongitude*/)
{
	OstTraceFunctionEntry0( DUP1_CALENEDITORPRIVATE_HANDLELOCATIONCHANGE_ENTRY );
	mEditedEntry->setLocation(location);
	mEditedEntry->clearGeoValue();
	addDiscardAction();
	OstTraceFunctionExit0( DUP1_CALENEDITORPRIVATE_HANDLELOCATIONCHANGE_EXIT );
}

/*!
 * Handles the completion of location editing
 */
void CalenEditorPrivate::handleLocationEditingFinished()
{
    OstTraceFunctionEntry0( CALENEDITORPRIVATE_HANDLELOCATIONEDITINGFINISHED_ENTRY );
    if ( !mOriginalEntry->location().isEmpty() )
    {
       AgendaGeoValue entryGeoValue =mAgendaUtil->fetchById(mEditedEntry->id()).geoValue();
       if ( !entryGeoValue.isNull() && (mEditedEntry->location()!=mOriginalEntry->location()) )
       {   
          
           mEditedEntry->setGeoValue(entryGeoValue);
           HbMessageBox* confirmationQuery = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
                   
           confirmationQuery->setDismissPolicy(HbDialog::NoDismiss);
           confirmationQuery->setTimeout(HbDialog::NoTimeout);
           confirmationQuery->setIconVisible(true);  
           
           QString displayText;
           displayText = displayText.append("Location changed. Keep existing location on Map?");
           
           confirmationQuery->setText(displayText);
           
           // Remove the default actions.
           QList<QAction *> defaultActions = confirmationQuery->actions();
           for (int index=0;index<defaultActions.count();index++) 
           {
               confirmationQuery->removeAction(defaultActions[index]);
           }
           
           defaultActions.clear();
           
           confirmationQuery->addAction(new HbAction("Yes"));
           confirmationQuery->addAction(new HbAction("No"));
           confirmationQuery->open(this, SLOT(selectEditingFinishedAction(HbAction*)));
       }
    }       
    OstTraceFunctionExit0( CALENEDITORPRIVATE_HANDLELOCATIONEDITINGFINISHED_EXIT );
}

/*!
 * Handles the editing finished action.
 */
void CalenEditorPrivate::selectEditingFinishedAction(HbAction* action)
{
    OstTraceFunctionEntry0( CALENEDITORPRIVATE_SELECTEDITINGFINISHEDACTION_ENTRY );
    HbMessageBox* dlg = static_cast<HbMessageBox*>(sender());    

    if (action == dlg->actions().at(1))
    {           
        mEditedEntry->clearGeoValue();
    } 
    OstTraceFunctionExit0( CALENEDITORPRIVATE_SELECTEDITINGFINISHEDACTION_EXIT );
}

/*!
	Triggered when the description editor is being edited.
	\param description Contains the string displayed in the description item.
 */
void CalenEditorPrivate::handleDescriptionChange(const QString description)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_HANDLEDESCRIPTIONCHANGE_ENTRY );
	mEditedEntry->setDescription(description);
	if(!mNewEntry){
		addDiscardAction();
	}
	OstTraceFunctionExit0( CALENEDITORPRIVATE_HANDLEDESCRIPTIONCHANGE_EXIT );
}

/*!
	Saves the entry & closes the editor.
	If mLaunchCalendar is true, then it tries to launch the calendar
 */
void CalenEditorPrivate::saveAndCloseEditor()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_SAVEANDCLOSEEDITOR_ENTRY );
	Action action = handleDone();
	if (CalenEditorPrivate::ActionDelete != action) {
		closeEditor();
	}

	if (CalenEditorPrivate::ActionSave == action) {
		// check if we need to launch the calendar application
		if (mLaunchCalendar) {
			CalenLauncher* launcher = new CalenLauncher(this);
			QDateTime startTime = mEditedEntry->startTime();
			launcher->launchCalendarApp(CalenLauncher::AgendaView, startTime);

			// connect to the error signal
			connect(launcher, SIGNAL(calendarLaunchFailed(int)), this,
									SLOT(handleCalendarLaunchError(int)));
		}
	}
	OstTraceFunctionExit0( CALENEDITORPRIVATE_SAVEANDCLOSEEDITOR_EXIT );
}

/*!
	Slot to handle errors while launching the calendar application
 */
void CalenEditorPrivate::handleCalendarLaunchError(int error)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_HANDLECALENDARLAUNCHERROR_ENTRY );
	// emit signal to the editor launcher
	emit q_ptr->calendarLaunchFailed(error);
	OstTraceFunctionExit0( CALENEDITORPRIVATE_HANDLECALENDARLAUNCHERROR_EXIT );
}

/*!
	Deletes the entry.
 */
void CalenEditorPrivate::handleDeleteAction()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_HANDLEDELETEACTION_ENTRY );
	// If its a new entry just close the editor
	if (mNewEntry) {
		closeEditor();
		OstTraceFunctionExit0( CALENEDITORPRIVATE_HANDLEDELETEACTION_EXIT );
		return;
	}else {
		// Delete entry and close editor
		deleteEntry(true);
	}
	OstTraceFunctionExit0( DUP1_CALENEDITORPRIVATE_HANDLEDELETEACTION_EXIT );
}

/*!
	Launches the Settings view.
 */
void CalenEditorPrivate::launchSettingsView()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_LAUNCHSETTINGSVIEW_ENTRY );
	// TODO :: Not yet implemented.
	OstTraceFunctionExit0( CALENEDITORPRIVATE_LAUNCHSETTINGSVIEW_EXIT );
}

/*!
	Discards the changes done to the entry.
 */
void CalenEditorPrivate::discardChanges()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_DISCARDCHANGES_ENTRY );
	// Close the editor without saving the changes.
	closeEditor();
	OstTraceFunctionExit0( CALENEDITORPRIVATE_DISCARDCHANGES_EXIT );
}

/*!
 * Show delete confirmation query
 */
void CalenEditorPrivate::showDeleteConfirmationQuery(bool closeEditor)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_SHOWDELETECONFIRMATIONQUERY_ENTRY );
	HbMessageBox *popup = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
	popup->setDismissPolicy(HbDialog::NoDismiss);
	popup->setTimeout(HbDialog::NoTimeout);
	popup->setAttribute( Qt::WA_DeleteOnClose, true );

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

	popup->setText(text);
	
    QList<QAction*> list = popup->actions();
    for(int i=0; i < list.count(); i++)
        {
        popup->removeAction(list[i]);
        }
	HbAction *deleteAction = new HbAction(hbTrId("txt_calendar_button_dialog_delete"),
										popup);
	popup->addAction(deleteAction);
	connect(deleteAction, SIGNAL(triggered()), this, 
										SLOT(handleDeleteAction()));
	HbAction *cancelAction = new HbAction(hbTrId("txt_common_button_cancel"), 
											popup);
	// Editor should not be closed for all the cases when cancel is pressed
	if(closeEditor) {
		connect(cancelAction, SIGNAL(triggered()), this, 
											SLOT(closeEditor()));
	}
	popup->addAction(cancelAction);
	popup->open();
	OstTraceFunctionExit0( CALENEDITORPRIVATE_SHOWDELETECONFIRMATIONQUERY_EXIT );
}

/*!
	Returns true if the entry is a child ,else false.
	\return true if the entry is a child, false otherwise.
 */
bool CalenEditorPrivate::isChild() const
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ISCHILD_ENTRY );
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
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_HANDLEDONE_ENTRY );
	if (mEditRange == ThisAndAll) {
		mRepeatField->saveRepeatRule();
	}
	// TODO: Need to check entry status here. EntryStillExistsL
	switch (mDataHandler->shouldSaveOrDeleteOrDoNothing(mLaunchCalendar)) {
		case CalenEditorPrivate::ActionSave:
			if (saveEntry()) {
				OstTraceFunctionExit0( CALENEDITORPRIVATE_HANDLEDONE_EXIT );
				return CalenEditorPrivate::ActionSave;
			}
			OstTraceFunctionExit0( DUP1_CALENEDITORPRIVATE_HANDLEDONE_EXIT );
			return CalenEditorPrivate::ActionNothing;
		case CalenEditorPrivate::ActionDelete:
			showDeleteConfirmationQuery(true);
			OstTraceFunctionExit0( DUP2_CALENEDITORPRIVATE_HANDLEDONE_EXIT );
			return CalenEditorPrivate::ActionDelete;
		case CalenEditorPrivate::ActionNothing:
			return CalenEditorPrivate::ActionNothing;
		default:
			break;
	}
	OstTraceFunctionExit0( DUP3_CALENEDITORPRIVATE_HANDLEDONE_EXIT );
	return CalenEditorPrivate::ActionNothing;
}

/*!
	Save the entry
	\return true if entry is saved ,false otherwise
 */
bool CalenEditorPrivate::saveEntry()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_SAVEENTRY_ENTRY );
	// check if we are editing child
	if (mIsChild && (mEditRange == ThisOnly)) {
		// Add the entry
		mAgendaUtil->updateEntry(*mEditedEntry, true);
		// TODO: Add the text id once available
		if(mEditedEntry->type() == AgendaEntry::TypeAppoinment) {
			HbNotificationDialog::launchDialog(hbTrId("Meeting updated"));
		}else if(mEditedEntry->type() == AgendaEntry::TypeEvent) {
			HbNotificationDialog::launchDialog(hbTrId("All day event updated"));
		}
		emit q_ptr->entrySaved();
		OstTraceFunctionExit0( CALENEDITORPRIVATE_SAVEENTRY_EXIT );
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
			} else if (mEditRange == ThisAndAll && mOriginalEntry->isRepeating()) {
				mAgendaUtil->storeRepeatingEntry(*mEditedEntry, true);
			} else if (!mIsChild && (mEditRange == ThisOnly)) {
				// Create the new exception
				mAgendaUtil->createException(*mEditedEntry, 
												mOriginalEntry->startTime());
			} else {
				// Normal entry updation
				mAgendaUtil->updateEntry(*mEditedEntry, false);
			}
		}
		if (mNewEntry) {
			// TODO: Add the text id once available
			if(mEditedEntry->type() == AgendaEntry::TypeAppoinment) {
				HbNotificationDialog::launchDialog(hbTrId("New meeting saved"));
			} else if(mEditedEntry->type() == AgendaEntry::TypeEvent) {
				HbNotificationDialog::launchDialog(hbTrId("New all-day saved"));
			}
		} else {
			// TODO: Add the text id once available
			if(mEditedEntry->type() == AgendaEntry::TypeAppoinment) {
				HbNotificationDialog::launchDialog(hbTrId("Meeting updated"));
			} else if(mEditedEntry->type() == AgendaEntry::TypeEvent) {
				HbNotificationDialog::launchDialog(hbTrId("All day event updated"));
			}
		}
		emit q_ptr->entrySaved();
	} else if (error) {
		mDataHandler->displayErrorMsg(error);
		OstTraceFunctionExit0( DUP1_CALENEDITORPRIVATE_SAVEENTRY_EXIT );
		return false;
	}
	OstTraceFunctionExit0( DUP2_CALENEDITORPRIVATE_SAVEENTRY_EXIT );
	return true;

}

/*!
	Delete the entry 
	@param closeEditor set true to close editor else default value is false
 */
void CalenEditorPrivate::deleteEntry(bool close)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_DELETEENTRY_ENTRY );
	// if editor is launched from Notes then don't delete entry
	// Just exit from calendar editor
	if (mOriginalEntry->id() > 0) {
		// If user is editing single instanc then delete single instance 
		// else delete entry
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
	OstTraceFunctionExit0( CALENEDITORPRIVATE_DELETEENTRY_EXIT );
}

/*!
 Change the entry before saving, depending on AllDay checkbox state
 */
bool CalenEditorPrivate::handleAllDayToSave()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_HANDLEALLDAYTOSAVE_ENTRY );
	if (!mIsAllDayItemAdded) {
		// All day item was not added, return true
		OstTraceFunctionExit0( CALENEDITORPRIVATE_HANDLEALLDAYTOSAVE_EXIT );
		return false;
	}
	// Check the state of AllDay checkBox

	// If Creating new Entry and AllDay Box is checked.
	// Clone the entry to AllDay 

	QDateTime tempSartTime =
	        CalenDateUtils::beginningOfDay(mEditedEntry->startTime());

	// Set EndTime of AllDay event to 00:00:00 of next day
	QDateTime tempEndTime = mEditedEntry->endTime().addDays(1);
	QTime tempEndQTime = tempEndTime.time();
	tempEndQTime.setHMS(0, 0, 0);
	tempEndTime.setTime(tempEndQTime);

	if (mNewEntry && (mAllDayCheckBoxItem->contentWidgetData("checkState")
	        == Qt::Checked)) {
		// changes Start/End times of entry to Beginning ot the day
		mEditedEntry->setStartAndEndTime(tempSartTime, tempEndTime);
		mEditedEntry->setType(AgendaEntry::TypeEvent);
		OstTraceFunctionExit0( DUP1_CALENEDITORPRIVATE_HANDLEALLDAYTOSAVE_EXIT );
		return false;
	} else if ((mAllDayCheckBoxItem->contentWidgetData("checkState")
	        == Qt::Checked) && (mEditedEntry->type() != 
								AgendaEntry::TypeEvent)) {
		// Editing existing Meetingentry, and Alldat Box is Checked
		// changes Start/End times of entry to Beginning ot the day
		mEditedEntry->setStartAndEndTime(tempSartTime, tempEndTime);

		// Clone the entry to AllDayEntry, Delete old entry from Database
		mAgendaUtil->cloneEntry(*mEditedEntry, AgendaEntry::TypeEvent);
		// For later reference for the notification popup
		mEditedEntry->setType(AgendaEntry::TypeEvent);
		OstTraceFunctionExit0( DUP2_CALENEDITORPRIVATE_HANDLEALLDAYTOSAVE_EXIT );
		return true;
	} else if (mAllDayCheckBoxItem->contentWidgetData("checkState")
	        != Qt::Checked) {
             if (mEditedEntry->type() != AgendaEntry::TypeAppoinment) {
	            // Editing existing AllDayentry, and Alldat Box is Not-Checked
	            // Clone the entry to MeetingEntry, Delete old entry from Database
	            mAgendaUtil->cloneEntry(*mEditedEntry, AgendaEntry::TypeAppoinment);
	            // For later reference for the notification popup
	            mEditedEntry->setType(AgendaEntry::TypeAppoinment);
	            OstTraceFunctionExit0( DUP3_CALENEDITORPRIVATE_HANDLEALLDAYTOSAVE_EXIT );
	            return true;
	        }
            // Check if the duration of the meeting is matching the all-day criteria
            // if yes, then we need to store it as all-day instead of normal meeting
             else if (mEditedEntry->startTime() == CalenDateUtils::beginningOfDay(mEditedEntry->startTime())) {
            // Get the end time and see if it is at the beginning of the end date day
            if (mEditedEntry->endTime() == CalenDateUtils::beginningOfDay(mEditedEntry->endTime())) {
                // Store it as all-day
                mEditedEntry->setType(AgendaEntry::TypeEvent);
                // Check if this was an all-day earlier and now user has changed it like that
                // or it is a new entry
                if (mOriginalEntry->type() == AgendaEntry::TypeEvent ||
                        mNewEntry) {
                    OstTraceFunctionExit0( DUP4_CALENEDITORPRIVATE_HANDLEALLDAYTOSAVE_EXIT );
                    return false;
                } else {
                    // It was a meeting
                    // Clone the entry to AllDayEntry, Delete old entry from Database
                    mEditedEntry->setStartAndEndTime(tempSartTime, tempEndTime);
                    mAgendaUtil->cloneEntry(*mEditedEntry, AgendaEntry::TypeEvent);
                    // For later reference for the notification popup
                    mEditedEntry->setType(AgendaEntry::TypeEvent);
                    OstTraceFunctionExit0( DUP5_CALENEDITORPRIVATE_HANDLEALLDAYTOSAVE_EXIT );
                    return true;
                }
            }
        } 
	} 
	OstTraceFunctionExit0( DUP6_CALENEDITORPRIVATE_HANDLEALLDAYTOSAVE_EXIT );
	return false;
}

/*!
 Enable/Disable TimeFields depending on AllDay checkbox state
 */
void CalenEditorPrivate::enableFromTotimeFileds(bool enableFileds,
                                                QDateTime fromTime,
                                                QDateTime toTime)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ENABLEFROMTOTIMEFILEDS_ENTRY );
	mViewFromItem->enableFromTimeFieldAndSetTime(enableFileds, fromTime);
	mViewToItem->enableToTimeFieldAndSetTime(enableFileds, toTime);

	OstTraceFunctionExit0( CALENEDITORPRIVATE_ENABLEFROMTOTIMEFILEDS_EXIT );
}

/*!
 Returns pointer of entyr being edited
 */
AgendaEntry* CalenEditorPrivate::editedEntry()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_EDITEDENTRY_ENTRY );
	OstTraceFunctionExit0( CALENEDITORPRIVATE_EDITEDENTRY_EXIT );
	return mEditedEntry;
}

/*!
 Returns the pointer of original entry
 */
AgendaEntry* CalenEditorPrivate::originalEntry()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ORIGINALENTRY_ENTRY );
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ORIGINALENTRY_EXIT );
	return mOriginalEntry;
}

/*!
 Returns true if new entry being created else false
 */
bool CalenEditorPrivate::isNewEntry()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ISNEWENTRY_ENTRY );
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ISNEWENTRY_EXIT );
	return mNewEntry;
}

/*!
 Returns the pointer of all day check box item
 */
HbDataFormModelItem* CalenEditorPrivate::allDayCheckBoxItem()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ALLDAYCHECKBOXITEM_ENTRY );
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ALLDAYCHECKBOXITEM_EXIT );
	return mAllDayCheckBoxItem;
}

/*!
	Checks if all day item is added and if selected implies reminder time is added.
	When editing single occurence irrespective of type of the event 
	the all day item is not shown.
	Used to calculate the index of other items which are dependant on this.
 */
bool CalenEditorPrivate::isReminderTimeForAllDayAdded()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ISREMINDERTIMEFORALLDAYADDED_ENTRY );
	return mReminderField->isReminderTimeForAllDayAdded();
}

/*!
	Checks if it is an all day event or not.
 */
bool CalenEditorPrivate::isAllDayEvent()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ISALLDAYEVENT_ENTRY );
	if (mAllDayCheckBoxItem) {
		return (mAllDayCheckBoxItem->contentWidgetData("checkState") 
													== Qt::Checked)? true:false;
	} else if (!mNewEntry && mEditedEntry->type() == AgendaEntry::TypeEvent) {
		// If editing single occurence then all day item not shown but still it 
		// is an all day event..
		OstTraceFunctionExit0( CALENEDITORPRIVATE_ISALLDAYEVENT_EXIT );
		return true;;
	} else {
		OstTraceFunctionExit0( DUP1_CALENEDITORPRIVATE_ISALLDAYEVENT_EXIT );
		return false;
	}
}

/*!
	Returns the current index of the reminder field.
 */
int CalenEditorPrivate::currentIndexOfReminderField()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_CURRENTINDEXOFREMINDERFIELD_ENTRY );
	return mReminderField->currentReminderIndex();
}

/*!
	Sets the index. 
	/param index indicates the index value to be set.
 */
void CalenEditorPrivate::setCurrentIndexOfReminderField(int index)
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_SETCURRENTINDEXOFREMINDERFIELD_ENTRY );
	mReminderField->setCurrentIndex(index);
	OstTraceFunctionExit0( CALENEDITORPRIVATE_SETCURRENTINDEXOFREMINDERFIELD_EXIT );
}

/*!
	Sets the reminder choices for a non all day event.
 */
void CalenEditorPrivate::setReminderChoices()
{
    OstTraceFunctionEntry0( CALENEDITORPRIVATE_SETREMINDERCHOICES_ENTRY );
    mReminderField->setReminderChoices();
    OstTraceFunctionExit0( CALENEDITORPRIVATE_SETREMINDERCHOICES_EXIT );
}

/*!
	Checks if editing all occurences or a single occurence.
 */
bool CalenEditorPrivate::isEditRangeThisOnly()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ISEDITRANGETHISONLY_ENTRY );
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ISEDITRANGETHISONLY_EXIT );
	return (mEditRange == ThisOnly);
}

/*!
	Checks if all day field is added.
 */
bool CalenEditorPrivate::isAllDayFieldAdded()
{
	OstTraceFunctionEntry0( CALENEDITORPRIVATE_ISALLDAYFIELDADDED_ENTRY );
	OstTraceFunctionExit0( CALENEDITORPRIVATE_ISALLDAYFIELDADDED_EXIT );
	return mIsAllDayItemAdded;
}

// End of file	--Don't remove this.
