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
* Header file for class Clock AlarmEditor.
*
*/

#ifndef CLOCKALARMEDITOR_H
#define CLOCKALARMEDITOR_H

// System includes
#include <QObject>
#include <QPointer>
#include <HbView>

// User includes
#include "clockalarmeditordefines.h"
#include "clockdatatypes.h"

// Forward declarations
class QGraphicsWidget;
class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class HbAction;
class HbDateTimePicker;
class HbDialog;
class XQSettingsManager;
class XQSettingsKey;
class AlarmClient;
class TimezoneClient;
class ClockAlarmCustomItem;
class QTranslator;

class ClockAlarmEditor : public QObject
{
	Q_OBJECT

public:
	CLOCKALARMEDITOR_EXPORT ClockAlarmEditor(
			AlarmClient& alarmClient,
			int alarmId = 0,
			QObject *parent = 0);
	CLOCKALARMEDITOR_EXPORT virtual ~ClockAlarmEditor();

public:
	CLOCKALARMEDITOR_EXPORT void showAlarmEditor();

public slots:
	void handleDoneAction();
	void handleDeleteAction();
	void handleDiscardAction();
	void handleTimeChange(const QString &text);
	void handleOccurenceChanged(int index);
	void handleOkAction();
	void launchTimePicker();
	void handleAlarmSoundChanged(int checkedState);
	void selectedAction(HbAction *action);

signals:
	void alarmSet();

private:
	void initModel();
	void populateModelItems();
	void initAlarmInfo();
	void createToolbar();
	void setAlarm(
			QString timeInfo, QString descInfo, int repeatType,
			int alarmDayIndex);
	QTime getPreviousAlarmTime();
	void setPreviosAlarmTime(QTime time);
	void closeAlarmEditor();
	void displayDSTRollOverNote(AlarmInfo& alarmInfo);
	void displayRemainingTimeNote(AlarmInfo& alarmInfo);
	void getDayText(int alarmDay,QString& dayText);
	int getRemainingSeconds(QDateTime& alarmDateTime);
	void launchDialog(QString title, QString text = 0);
    void sortAlarmDaysList(QStringList& alarmDays);

private:
	int mAlarmId;
	int mStartOfWeek;
	bool mAlarmDayItemInserted;
	QString mTimeFormat;

	HbDataForm *mAlarmEditorForm;
	HbDataFormModel *mAlarmEditorModel;
	HbDataFormModelItem *mAlarmTimeItem;
	HbDataFormModelItem *mAlarmOccurenceItem;
	HbDataFormModelItem *mAlarmDayItem;
	HbDataFormModelItem *mAlarmSoundItem;
	HbDataFormModelItem *mAlarmDescription;

	HbView *mAlarmEditorView;
	HbAction *mDiscardAction;
	HbAction *mDeleteAction;
	HbAction *mDoneAction;
	HbAction *mOkAction;
	HbAction *mCancelAction;

	QPointer<HbDialog> mTimePickerDialog;

	XQSettingsManager *mSettingsManager;
	XQSettingsKey *mPreviosAlarmTime;

	AlarmClient &mAlarmClient;
	AlarmInfo mAlarmInfo;
	TimezoneClient *mTimezoneClient;
	QTranslator *mTranslator;
};

#endif // CLOCKALARMEDITOR_H

// End of file	--Don't remove this.
