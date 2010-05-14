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
 * Description: Definition file for class Clock AlarmEditor.
 *
 */

// System includes
#include <QDateTime>
#include <HbDataForm>
#include <HbDataFormModel>
#include <HbDataFormModelItem>
#include <HbDataFormViewItem>
#include <HbInstance>
#include <HbAction>
#include <HbMenu>
#include <HbLabel>
#include <HbNotificationDialog>
#include <HbDialog>
#include <HbDateTimePicker>
#include <HbCheckBox>
#include <HbExtendedLocale>
#include <HbGroupBox>
#include <HbDocumentLoader>
#include <HbApplication>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <QTranslator>

// User includes
#include "clockcommon.h"
#include "clockalarmeditor.h"
#include "alarmclient.h"
#include "settingsutility.h"
#include "timezoneclient.h"
#include "clockalarmcustomitem.h"

// Constants
const int KOneHourInMinute(60);
const int KOneMinuteInSecons(60);
const int KOneMinute(1);
const int KOneHour(1);
const int KSecondsInOneDay(24 * 60 * 60);
const int KDaysInWeek(7);

/*!
    \class ClockAlarmEditor

    This object loads the clock alarm editor view. The view is added to the top
    of the HbMainWindow.
 */

/*!
    Constructor.

    \param parentViewIndex Previous view Index.
    \param alarmId Id of the alarm, if new alarm(0) or else id of alarm to be
    edit.
    \param parent The parent object.
 */
ClockAlarmEditor::ClockAlarmEditor(
		AlarmClient &alarmClient, int alarmId,
		QObject *parent)
:QObject(parent),
 mAlarmId(alarmId),
 mStartOfWeek(0),
 mAlarmDayItemInserted(false),
 mAlarmEditorForm(0),
 mAlarmEditorModel(0),
 mAlarmTimeItem(0),
 mAlarmOccurenceItem(0),
 mAlarmDayItem(0),
 mAlarmSoundItem(0),
 mAlarmDescription(0),
 mAlarmClient(alarmClient)
{
 	// Load the translation file and install the alarmeditor specific translator
	mTranslator = new QTranslator;
	//QString lang = QLocale::system().name();
	//QString path = "Z:/resource/qt/translations/";
	mTranslator->load("clockalarmeditor",":/translations");
	// TODO: Load the appropriate .qm file based on locale
	//bool loaded = mTranslator->load("caleneditor_" + lang, path);
	HbApplication::instance()->installTranslator(mTranslator);

	// create the timezone client object
	mTimezoneClient = TimezoneClient::getInstance();
	// Create the settings manager.
	mSettingsManager = new XQSettingsManager(this);

	mPreviosAlarmTime = new XQSettingsKey(
			XQSettingsKey::TargetCentralRepository,
			KCRUidNitz,
			KPreviousAlarmTime);

	// Create settings utility object. To get the date & time format.
	// TODO: do i need to delete this object ??
	SettingsUtility *settingsUtil = new SettingsUtility(this);
	mTimeFormat = settingsUtil->timeFormatString();

	// Get start of week from the locale.
    HbExtendedLocale locale = HbExtendedLocale::system();
    mStartOfWeek = locale.startOfWeek();

	// Create the HbDocumentLoader object.
	HbDocumentLoader *loader = new HbDocumentLoader();
	bool success;
	loader->load(":/xml/clockalarmeditor.docml", &success);

	mAlarmEditorView = qobject_cast<HbView *>(
			loader->findWidget("alarmEditorView"));

	// Get the subtitle groupBox.
	HbGroupBox *subtitleGroupBox = qobject_cast<HbGroupBox *>(
			loader->findWidget("subtitleGroupBox"));

	// Set the heading of the subtitle groupBox.
	if (mAlarmId) {
		subtitleGroupBox->setHeading(hbTrId("txt_clock_subhead_alarm"));
	} else {
		subtitleGroupBox->setHeading(hbTrId("txt_clock_subhead_new_alarm"));
	}

	// Get the Alarm Editor DataForm.
	mAlarmEditorForm = qobject_cast<HbDataForm *> (
			loader->findWidget("alarmEditorForm"));


	// Create the alarm info structure with desired values.
	initAlarmInfo();

	// Creates & initializes the DataFormModel for the AlarmEditor DataForm.
	initModel();

	QList <HbAbstractViewItem*> prototypes = mAlarmEditorForm->itemPrototypes();
	ClockAlarmCustomItem *customItem =
			new ClockAlarmCustomItem(mAlarmEditorForm);
	prototypes.append(customItem);
	mAlarmEditorForm->setItemPrototypes(prototypes);

	// Get the menu items for the alarm editor.
	mDeleteAction = qobject_cast<HbAction *> (
			loader->findObject("deleteAction"));
	mDiscardAction = qobject_cast<HbAction *>(
			loader->findObject("discardChanges"));

	// Connect the signals for the menu item.
	connect(
			mDeleteAction, SIGNAL(triggered()),
			this, SLOT(handleDeleteAction()));
	connect(
			mDiscardAction, SIGNAL(triggered()),
			this, SLOT(handleDiscardAction()));

	// Add the done soft key action.
	mDoneAction = new HbAction(Hb::DoneNaviAction);
	connect(
			mDoneAction, SIGNAL(triggered()),
			this, SLOT(handleDoneAction()));
	mAlarmEditorView->setNavigationAction(mDoneAction);

}

/*!
    Destructor.
 */
ClockAlarmEditor::~ClockAlarmEditor()
{
	if(!mTimezoneClient->isNull()) {
		mTimezoneClient->deleteInstance();
	}
	// Remove the translator
	HbApplication::instance()->removeTranslator(mTranslator);
	if (mTranslator) {
		delete mTranslator;
		mTranslator = 0;
	}

}

/*!
    Show the alarm editor.
 */
void ClockAlarmEditor::showAlarmEditor()
{
	// Store the current view and set alarm editor as current view.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	window->addView(mAlarmEditorView);
	window->setCurrentView(mAlarmEditorView);
}

/*!
    Handles done action on the editor. Saves the alarm
 */
void ClockAlarmEditor::handleDoneAction()
{
	int alarmDayIndex = -1;
	if (mAlarmDayItemInserted) {
		alarmDayIndex = mAlarmDayItem->contentWidgetData("currentIndex").toInt();
		alarmDayIndex += mStartOfWeek;
		if(alarmDayIndex >= KDaysInWeek){
		   alarmDayIndex -= KDaysInWeek;
		}
	}

	setAlarm(
			mAlarmTimeItem->contentWidgetData("text").toString(),
			mAlarmDescription->contentWidgetData("text").toString(),
			mAlarmOccurenceItem->contentWidgetData("currentIndex").toInt(),
			alarmDayIndex);

	mAlarmDayItemInserted = false;

	emit alarmSet();

	closeAlarmEditor();
}

/*!
	Handles delete action on the editor. Deletes the alarm.
 */
void ClockAlarmEditor::handleDeleteAction()
{
	closeAlarmEditor();
}

/*!
    Handles discard changes action on the editor.
 */
void ClockAlarmEditor::handleDiscardAction()
{
	// Reset alarm time.
	mAlarmTimeItem->setContentWidgetData(
			"text",mAlarmInfo.nextDueTime.toString(mTimeFormat));
	// Reset repeat info.
	mAlarmOccurenceItem->setContentWidgetData("currentIndex", mAlarmInfo.repeatType);

	// Reset alarm day item.
	if (mAlarmDayItemInserted) {
    	int currentIndex = mAlarmInfo.alarmDateTime.dayOfWeek() - 1;
        currentIndex -= mStartOfWeek;
        if(0 > currentIndex){
           currentIndex += KDaysInWeek;
        }
		mAlarmDayItem->setContentWidgetData(
				"currentIndex",currentIndex);
	}

	// Reset for alarm sound.
	if (AlarmVolumeOn == mAlarmInfo.volumeStatus) {
		mAlarmSoundItem->setContentWidgetData("checkState", Qt::Checked);
	} else {
		mAlarmSoundItem->setContentWidgetData("checkState", Qt::Unchecked);
	}

	mAlarmDescription->setContentWidgetData("text", mAlarmInfo.alarmDesc);
}

/*!
    Handles change in alarmtime item of the data form.
 */
void ClockAlarmEditor::handleTimeChange(const QString &text)
{
	// If it is a new alarm.
	if (!mAlarmId) {
		// Decide the day to focus.
		int alarmDayIndex = (QDate::currentDate()).dayOfWeek() - 1;

		QTime alarmTime = QTime::fromString(
				text,
				mTimeFormat);
		if (!alarmTime.isValid()) {
			return;
		}

		QTime homeTime = QTime::currentTime();
		if (-1 == alarmDayIndex) {
			return;
		}
		if (homeTime > alarmTime) {
			alarmDayIndex++;
		}
		if (7 == alarmDayIndex) {
			alarmDayIndex = 0;
		}
		if (mAlarmDayItemInserted) {
  		    alarmDayIndex -= mStartOfWeek;

            if(0 > alarmDayIndex){
               alarmDayIndex += KDaysInWeek;
            }
			mAlarmDayItem->setContentWidgetData(
					"currentIndex",
					alarmDayIndex);
		}
	}
}

/*!
    Handles change in alarm occurence item of the data form.
 */
void ClockAlarmEditor:: handleOccurenceChanged(int index)
{
	Q_UNUSED(index)

	int repeatIndex =
			mAlarmOccurenceItem->contentWidgetData("currentIndex").toInt();

	if ((Weekly == repeatIndex || Once == repeatIndex)) {
		// Decide which day to focus.
		int alarmDayIndex = (QDate::currentDate()).dayOfWeek() - 1;
		QTime alarmTime = QTime::fromString(
				mAlarmTimeItem->contentWidgetData("text").toString(),
				mTimeFormat);
		QTime homeTime = QTime::currentTime();
		if (-1 == alarmDayIndex) {
			return;
		}
		if (homeTime > alarmTime) {
			alarmDayIndex++;
		}
		if (7 == alarmDayIndex) {
			alarmDayIndex = 0;
		}
		if (!mAlarmDayItemInserted) {
			// Show the alarm days field to select the day.
			mAlarmDayItem = new HbDataFormModelItem(
					HbDataFormModelItem::ComboBoxItem,
					QString(hbTrId("txt_clock_formlabel_day")),
					mAlarmEditorModel->invisibleRootItem());

			// Add the alarm days beginning from the start of week.
			QStringList alarmDays;
			sortAlarmDaysList(alarmDays);
			alarmDayIndex -= mStartOfWeek;

			if(0 > alarmDayIndex){
				alarmDayIndex += KDaysInWeek;
			}
			mAlarmDayItem->setContentWidgetData("items", alarmDays);
			mAlarmDayItem->setContentWidgetData(
					"currentIndex",
					alarmDayIndex);

			int insertIndex = mAlarmEditorModel
					->indexFromItem(mAlarmOccurenceItem).row() + 1;
			mAlarmEditorModel->insertDataFormItem(insertIndex, mAlarmDayItem);

			mAlarmDayItemInserted = true;
		}
	} else {
		// Check if the Alarm days item is added.
		// Remove if already added.
		if (mAlarmDayItemInserted) {
			mAlarmEditorModel->removeItem(mAlarmDayItem);
			mAlarmDayItemInserted = false;
		}
	}
}

/*!
    Called when `OK' is pressed on the Dialog.
 */
void ClockAlarmEditor::handleOkAction()
{
	// Get the time from the time picker.
	QTime newAlarmTime = static_cast<HbDateTimePicker *> (
			mTimePickerDialog->contentWidget())->time();

	// Update the selected time value.
	mAlarmTimeItem->setContentWidgetData("text",newAlarmTime.toString(mTimeFormat));

	if (mAlarmInfo.nextDueTime != newAlarmTime ) {
		handleTimeChange(newAlarmTime.toString(mTimeFormat));
	}
}

/*!
    Launches datetimepicker when Alarm time button is clicked.
 */
void ClockAlarmEditor::launchTimePicker()
{
	if (mTimePickerDialog) {
		delete mTimePickerDialog;
	}

	// Create the dialog.
	mTimePickerDialog = new HbDialog;
	mTimePickerDialog->setTimeout(HbDialog::NoTimeout);
	mTimePickerDialog->setDismissPolicy(HbDialog::NoDismiss);

	// Set the heading for the dialog.
	HbLabel * timeLabel =
		new HbLabel(hbTrId("txt_tumbler_title_alarm_time"),
			mTimePickerDialog);
	mTimePickerDialog->setHeadingWidget(timeLabel);

	SettingsUtility *settingsUtil = new SettingsUtility(this);
	QStringList timeSeparator;
	int index = settingsUtil->timeFormat(timeSeparator);
	QString tumblerDisplayFormat =
			mTimeFormat.replace(timeSeparator.at(index), QString("."));

	// Create the tumbler.
	HbDateTimePicker *timePicker = new HbDateTimePicker(mTimePickerDialog);
	timePicker->setDisplayFormat(tumblerDisplayFormat);

	mTimePickerDialog->setContentWidget(timePicker);

	QString timeString = mAlarmTimeItem->contentWidgetData("text").toString();
	QTime time = QTime::fromString(timeString, mTimeFormat);
	timePicker->setTime(time);

	mOkAction =
		new HbAction(QString(hbTrId("txt_common_button_ok")),
			mTimePickerDialog);

	mCancelAction =
		new HbAction(QString(hbTrId("txt_common_button_cancel")),
			mTimePickerDialog);

	mTimePickerDialog->addAction(mOkAction);
	mTimePickerDialog->addAction(mCancelAction);

	mTimePickerDialog->open(this, SLOT(selectedAction(HbAction*)));
}

/*!
	Handles the alarm sound change.
 */
void ClockAlarmEditor::handleAlarmSoundChanged(int checkedState)
{
	Q_UNUSED(checkedState)
	if (AlarmVolumeOff == mAlarmInfo.volumeStatus) {
		mAlarmInfo.volumeStatus = AlarmVolumeOn;
	} else {
		mAlarmInfo.volumeStatus = AlarmVolumeOff;
	}
}

/*!
	Slot to handle the selected action
 */
void ClockAlarmEditor::selectedAction(HbAction *action)
{
	if (action == mOkAction) {
		handleOkAction();
	}
}

/*!
	Initialises the Model & sets for the AlarmEditorForm.
 */
void ClockAlarmEditor::initModel()
{
	// Remove the model.
	if (mAlarmEditorForm->model()) {
		delete mAlarmEditorForm->model();
		mAlarmEditorForm->setModel(0);
	}

	// Create a model and set it.
	mAlarmEditorModel = new HbDataFormModel(mAlarmEditorForm);
	// Populates the model items.
	populateModelItems();
	// Set the model.
	mAlarmEditorForm->setModel(mAlarmEditorModel);
}

/*!
	Populates the items for the alarm editor.
 */
void ClockAlarmEditor::populateModelItems()
{
	// Time.
	// Creats a data form model item to set time.
	mAlarmTimeItem = mAlarmEditorModel->appendDataFormItem(
			HbDataFormModelItem::CustomItemBase,
			QString(hbTrId("txt_clk_setlabel_time")),
			mAlarmEditorModel->invisibleRootItem());
	mAlarmTimeItem->setContentWidgetData(
			"text", mAlarmInfo.nextDueTime.toString(mTimeFormat));
	mAlarmEditorForm->addConnection(
			mAlarmTimeItem, SIGNAL(pressed()),
			this, SLOT(launchTimePicker()));

	// Occurence.
	mAlarmOccurenceItem = mAlarmEditorModel->appendDataFormItem(
			HbDataFormModelItem::ComboBoxItem,
			QString(hbTrId("txt_clk_setlabel_occurence")),
			mAlarmEditorModel->invisibleRootItem());
	QStringList repeatTypes;
	repeatTypes << hbTrId("txt_clk_setlabel_val_once")
			<< hbTrId("txt_clk_setlabel_val_repeat_daily")
			<< hbTrId("txt_clk_setlabel_val_repeat_weekly")
			<< hbTrId("txt_clk_setlabel_val_repeat_on_workdays");
	mAlarmOccurenceItem->setContentWidgetData("items", repeatTypes);
	mAlarmEditorForm->addConnection(
			mAlarmOccurenceItem, SIGNAL(currentIndexChanged(int)),
			this, SLOT(handleOccurenceChanged(int)));

	// Get the repeat type of alarm and set the value accordingly.
	int repeatItemIndex = mAlarmInfo.repeatType;
	mAlarmOccurenceItem->setContentWidgetData("currentIndex", repeatItemIndex);

	// Day.
	// This item is inserted only if the repeat type is
	// "once" or "repeat weekly".
	if (Once == repeatItemIndex || Weekly == repeatItemIndex) {
		if (!mAlarmDayItemInserted) {
			// Show the alarm days item.
			mAlarmDayItem = mAlarmEditorModel->appendDataFormItem(
					HbDataFormModelItem::ComboBoxItem,
					QString(hbTrId("txt_clk_setlabel_day")),
					mAlarmEditorModel->invisibleRootItem());

			// Add the alarm days beginning from the start of week.
			QStringList alarmDays;
			sortAlarmDaysList(alarmDays);

			int currentIndex = mAlarmInfo.alarmDateTime.dayOfWeek() - 1;
			currentIndex -= mStartOfWeek;
			if(0 > currentIndex){
			   currentIndex += KDaysInWeek;
			}
			mAlarmDayItem->setContentWidgetData("items", alarmDays);
			mAlarmDayItem->setContentWidgetData(
					"currentIndex",currentIndex);

			mAlarmDayItemInserted = true;
		}/* else { TODO: check and remove this else block.
			HbDataFormModelItem *dayItem = formItem(2);
			dayItem->setData(mAlarmInfo.alarmDateTime.dayOfWeek() - 1,
			                 "selected");
			mDayInserted = false;
		}*/
	}


	// Alarm sound.
	mAlarmSoundItem = mAlarmEditorModel->appendDataFormItem(
			HbDataFormModelItem::CheckBoxItem,
			QString(hbTrId("")),
			mAlarmEditorModel->invisibleRootItem());

	mAlarmSoundItem->setContentWidgetData(
			"text", QString(hbTrId("txt_clk_setlabel_alarm_sound")));

	if (AlarmVolumeOn == mAlarmInfo.volumeStatus) {
		mAlarmSoundItem->setContentWidgetData("checkState",Qt::Checked);
	}

	mAlarmEditorForm->addConnection(
			mAlarmSoundItem, SIGNAL(stateChanged(int)),
			this,SLOT(handleAlarmSoundChanged(int)));

	// Description.
	mAlarmDescription = mAlarmEditorModel->appendDataFormItem(
			HbDataFormModelItem::TextItem,
			QString(hbTrId("txt_clk_formlabel_description")),
			mAlarmEditorModel->invisibleRootItem());
	mAlarmDescription->setContentWidgetData("text", mAlarmInfo.alarmDesc);

}

/*!
	Create the alarm info structure with desired values.
	If new alarm: fill with default values
	else: fill with the values of the alram being editted.
 */
void ClockAlarmEditor::initAlarmInfo()
{
	// Fill default values for alarmInfo.
	mAlarmInfo.nextDueTime = getPreviousAlarmTime();
	mAlarmInfo.alarmDesc =
			hbTrId("txt_clk_formlabel_val_alarm");
	mAlarmInfo.repeatType = Once;
	mAlarmInfo.alarmDateTime = QDate::currentDate();

	// Decide the default day.
	QTime alarmTime = QTime::fromString(
			mAlarmInfo.nextDueTime.toString(mTimeFormat), mTimeFormat);
	QTime homeTime = QTime::fromString(
			QTime::currentTime().toString(mTimeFormat), mTimeFormat);
	if (homeTime > alarmTime) {
		mAlarmInfo.alarmDateTime = mAlarmInfo.alarmDateTime.addDays(1);
	}

	if (!mAlarmId) {
		// Set the alarm volume On by default for new alarms..
		mAlarmInfo.volumeStatus = AlarmVolumeOn;
	}

	// If editor state is a reset alarm, then its an already existing alarm
	// get the alarm details and fill it in mAlarmInfo.
	if (mAlarmId) {
		mAlarmClient.getAlarmInfo(mAlarmId, mAlarmInfo);
		mAlarmClient.deleteAlarm(mAlarmId);
	}
}

/*!
	Sets the alarm to the alarm server.
 */
void ClockAlarmEditor::setAlarm(
		QString timeInfo, QString descInfo, int repeatType, int alarmDayIndex)
{
	QTime alarmTime;
	AlarmInfo alarmInfo;

	// First the description.
	alarmInfo.alarmDesc = descInfo;
	// The time.
	alarmTime = QTime::fromString(timeInfo, mTimeFormat);

	// Construct the alarm info.
	alarmInfo.origAlarmTime = alarmTime;

	// Fill the repeat type.
	alarmInfo.repeatType = (AlarmRepeatType) repeatType;

	// Fill the alarm day selected if it is a weekly alarm,
	// otherwise set to -1.
	if (Weekly == repeatType || Once == repeatType) {
		alarmInfo.alarmDay = alarmDayIndex;
	} else {
		alarmInfo.alarmDay = -1;
	}

	// Fill the alarm volume status
	alarmInfo.volumeStatus = mAlarmInfo.volumeStatus;

	// Request the listener to set the alarm.
	mAlarmClient.setAlarm(alarmInfo);

	// Check if DST rule gets applied in 24hrs.
	// If so we don't display the remaining time.
	bool displayDSTRolloverNote(false);
	// Check if DST rule gets applied in 24hrs.
	displayDSTRolloverNote = mTimezoneClient->checkForDstChange(alarmInfo);

	if (displayDSTRolloverNote) {
		// display DST rollover note
		displayDSTRollOverNote(alarmInfo);
	}
	else {
		// display remaining time note
		displayRemainingTimeNote(alarmInfo);
	}

	// Save previous alarm time.
	setPreviosAlarmTime(alarmTime);
}

/*!
	Gets the previous alarm time.
	Fetches the time from cenrep. Default value is 09:00.
	If there is an error in reading the cenrep the default time
	is returned.
 */
QTime ClockAlarmEditor::getPreviousAlarmTime()
{
	QTime previousTime(9,0,0);

	// Read the initial values from the cenrep.
	QVariant value = mSettingsManager->readItemValue(*mPreviosAlarmTime);

	bool ok(false);
	int alarmTimeMinutes = value.toInt(&ok);

	if (ok) {
		previousTime.setHMS(
				alarmTimeMinutes / 60, alarmTimeMinutes % 60, 0);

		return previousTime;
	}

	return previousTime;
}

/*!
	Saves the previous alarm time in cenrep.
 */
void ClockAlarmEditor::setPreviosAlarmTime(QTime time)
{
	int alarmTimeMinutes = (time.hour() * 60) + (time.minute());

	// Write the value to cenrep.
	mSettingsManager->writeItemValue(*mPreviosAlarmTime, alarmTimeMinutes);
}

void ClockAlarmEditor::closeAlarmEditor()
{
	// Remove the alarm editor.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	window->removeView(mAlarmEditorView);
	deleteLater();
}

/*!
	Displays DST rollover note

	\param alarmInfo reference to alarm info
 */
void ClockAlarmEditor::displayDSTRollOverNote(AlarmInfo& alarmInfo)
{

	QString alarmTime = alarmInfo.nextDueTime.toString(mTimeFormat);
	QString dstRollOverText =
	hbTrId("txt_clock_dpopinfo_alarm_will_occur_at_1_after_au");
	QString displayText = dstRollOverText.arg(alarmTime);
	// show the note
	launchDialog(displayText);
}

/*!
	Displays remaining time note after saving the alarm

	\param alarmInfo reference to alarm info
 */
void ClockAlarmEditor::displayRemainingTimeNote(AlarmInfo& alarmInfo)
{
	QDateTime currentDateTime= QDateTime::currentDateTime();

	QTime alarmTime = alarmInfo.origAlarmTime;
	QDate alarmDate = alarmInfo.alarmDateTime;
	QDateTime alarmDateTime(alarmDate,alarmTime);

	QString alarmTimeText = alarmTime.toString(mTimeFormat);
	bool oneDayDifference( false );

	if (currentDateTime<alarmDateTime) {
		QDateTime newCurrentDateTime = currentDateTime.addDays(1);
		oneDayDifference = (newCurrentDateTime < alarmDateTime)? true : false;
	}

	int alarmDay(alarmInfo.alarmDay);
	QString dayText;
	getDayText(alarmDay+1,dayText);

	QString displayText;
	QString displayFormattedText;
	bool showDisplayText(false);
	// Choose the appropriate Repeat type.
	switch ( alarmInfo.repeatType ) {
		case Once:{
			if( oneDayDifference ){
				displayFormattedText =
				hbTrId("txt_clock_note_alarm_occurs_once_only_on_next_1_a");
				displayText = displayFormattedText.arg(dayText,alarmTimeText);
				showDisplayText = true;
			}
		}
		break;
		case Daily:	{
			displayFormattedText =
				hbTrId("txt_clock_dpopinfo_alarm_occurs_every_day_at_1");
			displayText = displayFormattedText.arg(alarmTimeText);
		}
		break;
		case Weekly:{
			displayFormattedText =
				hbTrId("txt_clock_note_alarm_occurs_every_week_on_1_at_2");
			displayText = displayFormattedText.arg(dayText,alarmTimeText);
		}
		break;
		case Workday:{
			displayFormattedText =
				hbTrId("txt_clock_note_alarm_occurs_workdays_at_1");
			displayText = displayFormattedText.arg(alarmTimeText);
		}
		break;
		default:
			break;
	}
	if (Enabled == alarmInfo.alarmStatus && !(displayText.isEmpty())) {
		showDisplayText = true;
	}

	QString remainingTimeText;
	// Dislpay the second note.
	if (!oneDayDifference) {
		QDateTime alarmDateTime(alarmInfo.alarmDateTime,alarmInfo.nextDueTime);
		int remainingSeconds;
		if (alarmDateTime<currentDateTime) {
			QDateTime newAlmDateTime = alarmDateTime.addDays(1);
//			remainingSeconds = currentDateTime.secsTo(newAlmDateTime);
			// TODO getRemainingSeconds is used since secsTo api is not giving
			// proper seconds.Once the issue is fixed,this method can be removed
			remainingSeconds = getRemainingSeconds(newAlmDateTime);
		}
		else {
//			remainingSeconds = currentDateTime.secsTo(alarmDateTime);
			// TODO getRemainingSeconds is used since secsTo api is not giving
			// proper seconds.Once the issue is fixed,this method can be removed
			remainingSeconds = getRemainingSeconds(alarmDateTime);
		}

		int remainingMinutes = qAbs(remainingSeconds)/KOneMinuteInSecons;
		int remainingHours = remainingMinutes/KOneHourInMinute;
		remainingMinutes -= remainingHours * KOneHourInMinute;

		QString hourText;
		QString minutesText;
		QString formattedText;
		// Alarm is with in 1 day. Choose the appropriate strings
		// to be displayed
		switch( remainingHours ) {
			case KOneHour:{
				if ( KOneMinute == remainingMinutes ) {
					formattedText =
					hbTrId("txt_clock_dpopinfo_time_to_alarm_1_hr_and_2_min");
					remainingTimeText = formattedText.arg(
							hourText.setNum(remainingHours),
							minutesText.setNum(remainingMinutes));
				}
				else {
					formattedText =
						hbTrId("txt_clock_dpopinfo_time_to_alarm_1_hr_and_2_mins");
					remainingTimeText = formattedText.arg(
							hourText.setNum(remainingHours),
							minutesText.setNum(remainingMinutes));
				}
			}
			break;
			default:{
				if ( KOneMinute == remainingMinutes ) {
					formattedText =
						hbTrId("txt_clock_dpopinfo_time_to_alarm_1_hrs_and_2_min");
					remainingTimeText = formattedText.arg(
							hourText.setNum(remainingHours),
							minutesText.setNum(remainingMinutes));
				}
				else {
					formattedText =
						hbTrId("txt_clock_dpopinfo_time_to_alarm_1_hrs_and_2_mins");
					remainingTimeText = formattedText.arg(
							hourText.setNum(remainingHours),
							minutesText.setNum(remainingMinutes));
				}
			}
			break;
		}

		if ( Enabled == alarmInfo.alarmStatus
				&& 	!(remainingTimeText.isEmpty())) {

			if (!(displayText.isEmpty()) ) {
				launchDialog(displayText, remainingTimeText);
				showDisplayText = false;
			}
			else {
				launchDialog(remainingTimeText);
			}
		}
	}
	if (showDisplayText) {
		launchDialog(displayText);
	}
}

/*!
	 gets day text for the week day

	 \param alarmDay for which the day text is required
	 \param dayText week day's text in QString
 */
void ClockAlarmEditor::getDayText(int alarmDay,QString& dayText)
{
	switch(alarmDay) {
		case 1:dayText.append(hbTrId("txt_clk_setlabel_val_monday"));
		break;
		case 2:dayText.append(hbTrId("txt_clk_setlabel_val_tuesday"));
		break;
		case 3:dayText.append(hbTrId("txt_clk_setlabel_val_wednesday"));
		break;
		case 4:dayText.append(hbTrId("txt_clk_setlabel_val_thursday"));
		break;
		case 5:dayText.append(hbTrId("txt_clk_setlabel_val_friday"));
		break;
		case 6:dayText.append(hbTrId("txt_clk_setlabel_val_saturday"));
		break;
		case 7:dayText.append(hbTrId("txt_clk_setlabel_val_sunday"));
		break;
		default:
			break;
	}
}
/*!
	gets remaining seconds to the alarm time from current date time

	\param alarmDateTime reference to the alarm date time
 */
int ClockAlarmEditor::getRemainingSeconds(QDateTime& alarmDateTime)
{
	QDateTime currentDateTime = QDateTime::currentDateTime();

	int currentDateTimeSec =
			(currentDateTime.time().hour() * KOneHourInMinute * KOneMinuteInSecons)
			+ (currentDateTime.time().minute() * KOneMinuteInSecons)
			+ currentDateTime.time().second();

	int alarmDateTimeSec =
			(alarmDateTime.time().hour() * KOneHourInMinute * KOneMinuteInSecons)
			+ alarmDateTime.time().minute() * KOneMinuteInSecons
			+ alarmDateTime.time().second();

	int dayInterval =
			alarmDateTime.date().day() - currentDateTime.date().day();

	if (dayInterval) {
		alarmDateTimeSec += (dayInterval * KSecondsInOneDay);
	}

	int remainingSeconds = alarmDateTimeSec - currentDateTimeSec;
	return remainingSeconds;
}

/*!
	Launches the soft notification.
 */
void ClockAlarmEditor::launchDialog(QString title, QString text)
{
	HbNotificationDialog *notificationDialog = new HbNotificationDialog();
	notificationDialog->setTitle(title);

	if (!text.isNull()) {
		notificationDialog->setText(text);
	}

	notificationDialog->setTimeout(HbPopup::ConfirmationNoteTimeout);
	notificationDialog->show();
}

/*!
    Sorts and appends alarm days on the basis of start of week.
 */
void ClockAlarmEditor::sortAlarmDaysList(QStringList& alarmDaysList)

{
    for(int index=(mStartOfWeek + 1),j=KDaysInWeek;j!=0 ;index++,j--){
        if(index > KDaysInWeek){
           index = index - KDaysInWeek;
        }
        QString alarmDayText;
        getDayText(index, alarmDayText );
        alarmDaysList.append(alarmDayText);
    }
}
// End of file	--Don't remove this.
