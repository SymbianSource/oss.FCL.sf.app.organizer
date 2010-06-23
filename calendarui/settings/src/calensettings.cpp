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
#include <HbExtendedLocale.h>

#include "calensettings.h"
#include "calenservices.h"
#include "calendarprivatecrkeys.h"

CalenSettings::CalenSettings(MCalenServices& services, HbDataForm *form, QObject *parent)
:QObject(parent),
 mServices(services),
 mSettingsForm(form),
 mSettingsModel(0),
 mShowRegionalInfoItem(NULL),
 mShowWeekStartOnInfoItem(NULL),
 mWeekNumberItemAdded(false),
 mRegionalpluginActive(false)
{   
    mSettingsManager = new XQSettingsManager(this);
	mAlarmSnoozeCenrepKey = new XQSettingsKey(
			XQSettingsKey::TargetCentralRepository,
			KCRUidCalendar, KCalendarSnoozeTime);
	mWeekNumberCenrepKey = new XQSettingsKey(
			XQSettingsKey::TargetCentralRepository,
			KCRUidCalendar, KCalendarShowWeekNum);
	mShowRegionalInfoKey = new XQSettingsKey(
			XQSettingsKey::TargetCentralRepository,
			KCRUidCalendar, KCalendarShowRegionalInfo);
	
	// Read the initial values from the cenrep
	QVariant value = mSettingsManager->readItemValue(*mAlarmSnoozeCenrepKey);
	mAlarmSnoozeTime = value.toUInt();
	
	value = mSettingsManager->readItemValue(*mWeekNumberCenrepKey);
	mShowWeekNumber = value.toUInt();
	
	value = mSettingsManager->readItemValue(*mShowRegionalInfoKey);
	mShowRegionalInfo = value.toUInt();
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
	
	// Create the weekday list based on start of week.
	QStringList weekdaysList;
	weekdaysList
        << QString("Monday")//hbTrId("txt_clk_setlabel_val_monday")//@TODO set to proper localization strings once available
        << QString("Tuesday")//hbTrId("txt_clk_setlabel_val_tuesday")
        << QString("Wednesday")//hbTrId("txt_clk_setlabel_val_wednesday")
        << QString("Thursday")//hbTrId("txt_clk_setlabel_val_thursday")
        << QString("Friday")//hbTrId("txt_clk_setlabel_val_friday")
        << QString("Saturday")//hbTrId("txt_clk_setlabel_val_saturday")
	   << QString("Sunday");//hbTrId("txt_clk_setlabel_val_sunday");
	// Start of week item.
	mShowWeekStartOnInfoItem = mSettingsModel->appendDataFormItem(HbDataFormModelItem::ComboBoxItem,QString("Week Starts On"));
                                                                 // hbTrId("txt_clock_setlabel_week_starts_on"));//@TODO set to proper localization strings once available
	// Get start of week from the locale.
    HbExtendedLocale locale = HbExtendedLocale::system();
    mStartOfWeek = locale.startOfWeek();
	mShowWeekStartOnInfoItem->setContentWidgetData("items", weekdaysList);
	mShowWeekStartOnInfoItem->setContentWidgetData("currentIndex", mStartOfWeek);
	mSettingsForm->addConnection(mShowWeekStartOnInfoItem, SIGNAL(currentIndexChanged(int)),
	        this, SLOT(setStartDayOfWeek(int)));
	//add show week number item in the model

        addWeekNumberItem();
	mSettingsModel->appendDataFormItem(mShowWeekNumberItem);
	  
	if(isPluginEnabled()) {
		addRegionalInfoItem();
	}
}
	  
void CalenSettings::addRegionalInfoItem()
{
	mRegionalpluginActive = true;
	// Regional info is the second item in the model
	int index = mSettingsModel->indexFromItem(mAlarmSnoozeTimeItem).row() + 1;

	mShowRegionalInfoItem = mSettingsModel->insertDataFormItem(
				index ,
				HbDataFormModelItem::ToggleValueItem,
				QString(hbTrId("txt_calendar_setlabel_show_lunar_calendar")),
				mSettingsModel->invisibleRootItem()); 

	// For HbPushButton type properties -- to be used for toggle value item
	mShowRegionalInfoItem->setContentWidgetData(QString("text"), 
									QString(hbTrId("txt_calendar_button_no")));
	mShowRegionalInfoItem->setContentWidgetData(QString("additionalText"), 
									QString(hbTrId("txt_calendar_button_yes")));
	mSettingsForm->addConnection(mShowRegionalInfoItem, SIGNAL(clicked()),
									this, SLOT(handleRegionalInfoChange()));
	
}

void CalenSettings::removeRegionalInfoItem()
{
	mSettingsForm->removeConnection(mShowRegionalInfoItem, SIGNAL(clicked()),
									this, SLOT(handleRegionalInfoChange()));
    mSettingsModel->removeItem(mShowRegionalInfoItem);
    mRegionalpluginActive = false;
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

void CalenSettings::handleRegionalInfoChange()
{
    QVariant data = mShowRegionalInfoItem->contentWidgetData(QString("text"));
    QString value = data.toString();
    if(!value.compare(hbTrId("txt_calendar_button_no"))) {    
        // User has made it OFF
        mShowRegionalInfo = 0;
        mSettingsManager->writeItemValue(*mShowRegionalInfoKey, 0);
    } else if(!value.compare(hbTrId("txt_calendar_button_yes"))) {
        // User has made it ON.
        mShowRegionalInfo = 1;
        mSettingsManager->writeItemValue(*mShowRegionalInfoKey, 1);
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
    
	// Get start of week from the locale.
    HbExtendedLocale locale = HbExtendedLocale::system();
    mStartOfWeek = locale.startOfWeek();
    
    
    mShowWeekStartOnInfoItem->setContentWidgetData("currentIndex", mStartOfWeek);
    //update the show week number option
    updateShowWeekItem();
    // Check the the regional info plugin is loaded now or not
    if(isPluginEnabled()) {
    	// If the plugin was already there just populate the item
    	// else add the regional info item and then populate
    	if(mRegionalpluginActive) {
    		populateRegionalInfo();
    	}else {
    		addRegionalInfoItem();
    		populateRegionalInfo();
    	}
    }else {
    	// If the plugin is removed now and if it was loaded before
    	// remove the item
    	if(mRegionalpluginActive) {
    		removeRegionalInfoItem();
    	}
    }
    mSettingsForm->setModel(mSettingsModel);
}

void CalenSettings::populateRegionalInfo()
{
    QVariant value  = mSettingsManager->readItemValue(*mShowRegionalInfoKey);
    mShowRegionalInfo = value.toUInt();

    if (mShowRegionalInfo) {
        mShowRegionalInfoItem->setContentWidgetData(
                QString("text"), QString(hbTrId("txt_calendar_button_yes")));
        mShowRegionalInfoItem->setContentWidgetData(
                QString("additionalText"), QString(
                        hbTrId("txt_calendar_button_no")));
    } else {
        mShowRegionalInfoItem->setContentWidgetData(
                QString("text"), QString(hbTrId("txt_calendar_button_no")));
        mShowRegionalInfoItem->setContentWidgetData(
                QString("additionalText"), QString(
                        hbTrId("txt_calendar_button_yes")));
    }

}

void CalenSettings::addWeekNumberItem()
{
    mWeekNumberItemAdded = true;
    // Append Show Week Number settings item
    mShowWeekNumberItem = new HbDataFormModelItem();
    mShowWeekNumberItem->setType(HbDataFormModelItem::ToggleValueItem);
    QStringList values;
    values << hbTrId("txt_calendar_button_no")
	                		                        << hbTrId("txt_calendar_button_yes");
    mShowWeekNumberItem->setData(HbDataFormModelItem::LabelRole, 
            QString(hbTrId("txt_calendar_setlabel_show_week_numbers")));     


    // For HbPushButton type properties -- to be used for toggle value item
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

    mSettingsForm->addConnection(mShowWeekNumberItem, SIGNAL(clicked()), 
            this, SLOT(handleWeekNumberChange()));	
}

/*!
 Returns true if plugin is loaded
 */
bool CalenSettings::isPluginEnabled()
{
    QString *pluginInfo = mServices.InfobarTextL();
    if (!pluginInfo) {
        return false;
    } else {
        return true;
    }
}
// ---------------------------------------------------------
// CalenSettings::setStartDayOfWeek
// sets the start day of the week to the locale
// calls when user select week day
// ---------------------------------------------------------
//
void CalenSettings::setStartDayOfWeek(const int index)
    {
    TLocale locale;
    TDay day = (TDay)index;
    //set the start day of the week to locale
    locale.SetStartOfWeek(day);
    locale.Set();
    updateShowWeekItem();
    }

// ---------------------------------------------------------
// CalenSettings::updateShowWeekItem
// update the  show  week number option 
// ---------------------------------------------------------
//
void CalenSettings::updateShowWeekItem()
    {
    // Get start of week from the locale.
    HbExtendedLocale locale = HbExtendedLocale::system();
    mStartOfWeek = locale.startOfWeek();
    mShowWeekStartOnInfoItem->setContentWidgetData("currentIndex", mStartOfWeek);
    //if start day is not Monday
    //set the show week number option dimmed ,
    //else set to proper status
    if(mStartOfWeek != HbExtendedLocale::Monday)
        {
        mShowWeekNumberItem->setContentWidgetData(QString("text"), 
                QString(hbTrId("txt_calendar_button_no")));
        mShowWeekNumberItem->setContentWidgetData(QString("additionalText"), 
                QString(hbTrId("txt_calendar_button_yes")));
        mShowWeekNumberItem->setEnabled(false);
        }
    else
        {
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
        mShowWeekNumberItem->setEnabled(true);
        }
    }
// End of file
