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
 * Description: Header file for class CalenEditorPrivate.
 *
 */

#ifndef CalenEditorPrivate_P_H
#define CalenEditorPrivate_P_H

// System includes
#include <QObject>
#include <hbmainwindow.h>
#include <QHash>
#include <QDateTime>

//user includes
#include <AgendaEntry>

#include "caleneditor.h"
#include "caleneditorcommon.h"

//forward declarations
class QTranslator;
class QFile;
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class HbAction;
class HbLineEdit;
class HbPushButton;
class HbDateTimePicker;
class HbListWidget;
class HbListWidgetItem;
class HbCheckBox;
class HbComboBox;
class HbView;
class HbRadioButtonList;
class MCalenServices;
class AgendaEntry;
class AgendaUtil;
class CalenEditor;
class CalenEditorDocLoader;
class CalenEditorCustomItem;

class CalenEditorPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(CalenEditor)
	
public:	
	enum Error
	        {
	        CalenEditorErrorNone,
	        CalenEditorErrorAlarmTimeLaterThanNote,
	        CalenEditorErrorAlarmTimePast,
	        CalenEditorErrorAlarmDateTooManyDaysBeforeNote,
	        CalenEditorErrorRepeatUntilEarlierThanNote,
	        CalenEditorErrorDurationGreaterThanRepeatInterval,
	        CalenEditorErrorStopTimeEarlierThanStartTime,
	        CalenEditorErrorDbConflictEntryDeleted,//ToDo ::need to handle
	        CalenEditorErrorDbConflictEntryModified,//ToDo ::need to handle
	        CalenEditorErrorOutOfSequence,//ToDo ::need to handle
	        CalenEditorErrorOverlapsExistingInstance,//ToDo ::need to handle
	        CalenEditorErrorInstanceAlreadyExistsOnThisDay//ToDo ::need to handle
	        };
	
	enum Action 
	        {
	        ActionNothing,
	        ActionSave,
	        ActionDelete
	        };
	
	enum EditorItemIndex 
			{
			SubjectItem = 0,
			AllDayItem,
			DateTimeFromItem,
			DateTimeToItem,
			LocationItem,
			ReminderItem,
			RepeatItem,
			RepeatUntilItem
			};
public:
	CalenEditorPrivate(AgendaUtil *agendaUtil,
	                   QObject *parent);
	virtual ~CalenEditorPrivate();

private:
	void edit(const QFile &handle, bool launchCalendar);
	void edit(AgendaEntry entry, bool launchCalendar);
	void edit(ulong id, bool launchCalendar);
	void create(CalenEditor::CreateType type, QDateTime newEntryDateTime, 
	            bool launchCalendar);
	void create(CalenEditor::CreateType type, AgendaEntry entry, 
		            bool launchCalendar);
	void showEditor(AgendaEntry entry);
	void showEditOccurencePopup();
	void addDiscardAction();
	void setUpView();
	void initModel();
	void addSubjectItem();
	void addAllDayCheckBoxItem();
	void addCustomItemFrom();
	void addCustomItemTo();
	void addLocationItem();
	void addReminderItem();
	void addRepeatItem();
	void insertRepeatUntilItem();
	
	void populateModel();
	void populateSubjectItem();
	void populateAllDayItem();
	void populateCustomItemDateTime();
	void populateLocationItem();
	void populateReminderItem();
	void populateRepeatItem();
	void populateDescriptionItem();
	void removeDescriptionItem();
	void closeEditor();
		
	bool isChild() const ;
	Error checkErrorsForThisAndAll();
	bool isEdited() const;
	bool isSummaryEdited() const;
	bool isAllDayEdited() const;
	bool isLocationEdited() const;
	bool isStartDateTimeEdited() const;
	bool isEndDateTimeEdited() const;
	bool isAlarmEdited() const;
	bool isRepeatRuleEdited() const;
	bool isDescriptionEdited() const;
	bool nonTextItemsEdited() const;
	bool areTextItemsEmpty() const;
	bool areTextItemsCleared() const;
	bool isSummaryEmptied() const;
	bool isLocationEmptied() const;
	bool isDescriptionEmptied() const;
	Action shouldSaveOrDeleteOrDoNothing() const;
	bool durationGreaterThanRepeatIntervalError() const;
	Error checkAlarmFieldsForErrors(bool series = false) const;
	bool isAlarmInAcceptablePeriod(Error &error, const QDateTime &alarmTime,
	                               const QDateTime &startTime) const;
	Action handleDone();
	void updateEditedEntry();
	bool saveEntry();
	void deleteEntry(bool close = false);
	void displayErrorMsg(int error);
	void dispalyErrorMsgByRepeatType();
	bool handleAllDayToSave();
	void enableFromTotimeFileds(bool, QDateTime, QDateTime);
	void connectSlots(bool toConnect);
	int showDeleteConfirmationQuery();
	void updateRepeatChoices();
	
private slots:
	void handleSubjectChange(const QString subject);
	void handleAllDayChange(int state);
	void saveFromDateTime(QDateTime& fromDateTime);
	void saveToDateTime(QDateTime& toDateTime);
	void handleLocationChange(const QString location);
	void handleReminderIndexChanged(int index);
	void handleRepeatIndexChanged(int index);
	void handleDescriptionChange(const QString description);
	void saveAndCloseEditor();
	void handleDeleteAction();
	void launchSettingsView();
	void discardChanges();
	void launchRepeatUntilDatePicker();
	void setRepeatUntilDate();
	void handleDescriptionAction();
	void handleEditOccurence(int index);
	void handleCancel();
	void handleCalendarLaunchError(int error);

private:
	enum EditRange {
		ThisOnly,
		ThisAndAll,
		UserCancelled
	};
	
	enum RepeatTypes {
		RepeatOnce,
		RepeatDaily,
		RepeatWeekly,
		RepeatBiWeekly,
		RepeatMonthly,
		RepeatYearly
	};
	CalenEditor *q_ptr;
	AgendaUtil *mAgendaUtil;
	EditRange mEditRange;
	CalenEditorDocLoader *mEditorDocLoader;
	CalenEditorCustomItem *mViewFromItem;
	CalenEditorCustomItem *mViewToItem;
	HbView *mEditorView;
	HbDataForm *mCalenEditorForm;
	HbDataFormModel *mCalenEditorModel;
	HbDataFormModelItem *mSubjectItem;
	HbDataFormModelItem *mAllDayCheckBoxItem;
	HbDataFormModelItem *mLocationItem;
	HbDataFormModelItem *mReminderItem;
	HbDataFormModelItem *mRepeatItem;
	HbDataFormModelItem *mCustomRepeatUntilItem;
	HbDataFormModelItem *mDescriptionItem;
	
	HbComboBox *mRepeatComboBox;
	HbDateTimePicker* mDatePicker;
	QHash<int, int> mReminderHash;
	QDateTime mNewEntryDateTime;
	QDate mRepeatUntilDate;
	AgendaRepeatRule::RuleType mRepeatRuleType;
		
	AgendaEntry *mOriginalEntry;
	AgendaEntry *mEditedEntry;

	HbAction *mSoftKeyAction;
	HbAction *mDescriptionAction;
	HbMainWindow *mMainWindow;
	QTranslator *mTranslator;
	int mMinutes;
	int mHour;
	
	bool mNewEntry;
	bool mRepeatUntilItemAdded;
	bool mDescriptionItemAdded;
	bool mIsBiWeekly;
	bool mIsChild;
	bool mIsAllDayItemAdded;
	bool mOwnsAgendaUtil;
	bool mLaunchCalendar;
	bool mMenuItemAdded;
	
private:
	friend class CalenEditor;
#ifdef TESTCALENEDITOR
	friend class TestCalenEditor;
#endif

};

#endif // CalenEditorPrivate_P_H

// End of file	--Don't remove this.
