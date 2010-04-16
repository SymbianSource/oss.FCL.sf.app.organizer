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
 *
 */

#include <QModelIndex>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>
#include <hbabstractbutton.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>

#include "calensettings.h"
#include <calendarinternalcrkeys.h>  // includes CalendarInternalCRKeys.h

CalenSettings::CalenSettings(HbDataForm *form, QObject *parent)
:QObject(parent),
 mSettingsForm(form),
 mSettingsModel(0)
{   
    mSettingsManager = new XQSettingsManager(this);
	mAlarmSnoozeCenrepKey = new XQSettingsKey(
										XQSettingsKey::TargetCentralRepository, 
										KCRUidCalendar, KCalendarSnoozeTime);
	mWeekNumberCenrepKey = new XQSettingsKey(
										XQSettingsKey::TargetCentralRepository, 
										KCRUidCalendar, KCalendarWeekViewTitle);
	
	// Read the initial values from the cenrep
	QVariant value = mSettingsManager->readItemValue(*mAlarmSnoozeCenrepKey);
	mAlarmSnoozeTime = value.toUInt();
	
	value = mSettingsManager->readItemValue(*mWeekNumberCenrepKey);
	mShowWeekNumber = value.toUInt();
}

CalenSettings::~CalenSettings()
{
	// Nothing yet.
}

void CalenSettings::createModel()
{
	// Remove the model.
	if (mSettingsForm->model()) {
		delete mSettingsForm->model();
		mSettingsForm->setModel(0);
		mSettingsModel = 0;
	}

	// Create a model and set it.
	mSettingsModel = new HbDataFormModel();
	
	// Create the alarm snooze choices
	QStringList alarmSnoozeChoices;
	alarmSnoozeChoices << hbTrId(
						"txt_calendar_setlabel_alarm_snooze_time_val_5_minu")
				<< hbTrId("txt_calendar_setlabel_alarm_snooze_time_val_10_min")
				<< hbTrId("txt_calendar_setlabel_alarm_snooze_time_val_15_min")
				<< hbTrId("txt_calendar_setlabel_alarm_snooze_time_val_30_min");
	
	// Append Alarm Snooze Time Setting item
	mAlarmSnoozeTimeItem = new HbDataFormModelItem();
	mAlarmSnoozeTimeItem->setType(HbDataFormModelItem::ComboBoxItem);
	mAlarmSnoozeTimeItem->setData(HbDataFormModelItem::LabelRole, 
					QString(hbTrId("txt_calendar_setlabel_alarm_snooze_time")));

	mAlarmSnoozeTimeItem->setContentWidgetData(QString("items"), 
															alarmSnoozeChoices);
	mAlarmSnoozeTimeItem->setContentWidgetData(QString("currentIndex"), 0);
	mSettingsForm->addConnection(mAlarmSnoozeTimeItem, 
								SIGNAL(currentIndexChanged(int)),
								this, SLOT(handleAlarmSnoozeTimeChange(int)));
	mSettingsModel->appendDataFormItem(mAlarmSnoozeTimeItem);
	
	// Append Show Week Number settings item
	mShowWeekNumberItem = new HbDataFormModelItem();
	mShowWeekNumberItem->setType(HbDataFormModelItem::ToggleValueItem);
	QStringList values;
	values << hbTrId("txt_calendar_button_no")
            << hbTrId("txt_calendar_button_yes");
	mShowWeekNumberItem->setData(HbDataFormModelItem::LabelRole, 
								QString(hbTrId("txt_calendar_setlabel_show_week_numbers")));     
	
	
	// For HbPushButton type properties -- to be used for toggle value item
	mShowWeekNumberItem->setContentWidgetData(QString("text"), 
										QString(hbTrId("txt_calendar_button_no")));
	mShowWeekNumberItem->setContentWidgetData(QString("additionalText"), 
										QString(hbTrId("txt_calendar_button_yes")));
	mSettingsForm->addConnection(mShowWeekNumberItem, SIGNAL(clicked()), 
										this, SLOT(handleWeekNumberChange()));
	mSettingsModel->appendDataFormItem(mShowWeekNumberItem);
}

void CalenSettings::handleAlarmSnoozeTimeChange(int index)
{
switch(index)
    {
    case 0:
        {
        mSettingsManager->writeItemValue(*mAlarmSnoozeCenrepKey, 5);
        }
        break;
    case 1:
        {
        mSettingsManager->writeItemValue(*mAlarmSnoozeCenrepKey, 10);
        }
        break;
    case 2:
        {
        mSettingsManager->writeItemValue(*mAlarmSnoozeCenrepKey, 15);
        }
        break;
    case 3:
        {
        mSettingsManager->writeItemValue(*mAlarmSnoozeCenrepKey, 30);
        }
        break;
    default:
        break;
    }
QVariant value = mSettingsManager->readItemValue(*mAlarmSnoozeCenrepKey);
    uint time = value.toUInt();
}

void CalenSettings::handleWeekNumberChange()
{
	QVariant data = mShowWeekNumberItem->contentWidgetData(QString("text"));
	QString value = data.toString();
	if(!value.compare(hbTrId("txt_calendar_button_no")))
	{    
		// User has made it OFF.
		mShowWeekNumber = 0;
		mSettingsManager->writeItemValue(*mWeekNumberCenrepKey, 0);
	}
	else if(!value.compare(hbTrId("txt_calendar_button_yes")))
	{
		// User has made it ON.
		mShowWeekNumber = 1;
		mSettingsManager->writeItemValue(*mWeekNumberCenrepKey, 1);
	}
}

void CalenSettings::populateSettingList()
{   
    // Read the values from cenrep
    // Read the initial values from the cenrep
    QVariant value = mSettingsManager->readItemValue(*mAlarmSnoozeCenrepKey);
    mAlarmSnoozeTime = value.toUInt();
    
    value = mSettingsManager->readItemValue(*mWeekNumberCenrepKey);
    mShowWeekNumber = value.toUInt();
    
    // set the value to the form
    int choiceIndex = 0;
    switch(mAlarmSnoozeTime)
        {
        case 5:
            choiceIndex = 0;
            break;
        case 10:
            choiceIndex = 1;
            break;
        case 15:
            choiceIndex = 2;
            break;
        case 30:
            choiceIndex = 3;
            break;
        default:
            choiceIndex = 0;
            break;
        }
    mAlarmSnoozeTimeItem->setContentWidgetData(QString("currentIndex"), 
												choiceIndex);
    if(mShowWeekNumber)
        {
        mShowWeekNumberItem->setContentWidgetData(QString("text"), 
												QString(hbTrId("txt_calendar_button_yes")));
        mShowWeekNumberItem->setContentWidgetData(QString("additionalText"), 
												QString(hbTrId("txt_calendar_button_no")));
        }
    else
        {
        mShowWeekNumberItem->setContentWidgetData(QString("text"), 
												QString(hbTrId("txt_calendar_button_no")));
        mShowWeekNumberItem->setContentWidgetData(QString("additionalText"), 
												QString(hbTrId("txt_calendar_button_yes")));
        }    
    mSettingsForm->setModel(mSettingsModel);
}
// End of file
