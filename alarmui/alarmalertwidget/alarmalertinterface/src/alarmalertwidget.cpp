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
*
*/
// alarmalertwidget.cpp

// System includes
#include <QDateTime>
#include <hbdevicedialog.h>
#include <QDebug>

// User includes
#include "alarmalertwidget.h"
#include "alarmalert.h"
#include "alarmalertobserver.h"

// ---------------------------------------------------------
// AlarmAlertWidget::AlarmAlertWidget
// rest of the details are commented in the header
// ---------------------------------------------------------
//
AlarmAlertWidget::AlarmAlertWidget(AlarmAlertObserver *observer) :
    mObserver(observer)
{
    // Create a session with the device dialog server
    mDeviceDialog = new HbDeviceDialog(HbDeviceDialog::NoFlags, this);
    
    // Connect to signals to receive events sent by the dialog
    connect(mDeviceDialog, SIGNAL(dataReceived(QVariantMap)), this, SLOT(triggerAction(QVariantMap)));
}

// ---------------------------------------------------------
// AlarmAlertWidget::~AlarmAlertWidget
// rest of the details are commented in the header
// ---------------------------------------------------------
//
AlarmAlertWidget::~AlarmAlertWidget()
{
    // Cleanup
    if (mDeviceDialog) {
       delete mDeviceDialog; 
    }
}

// ---------------------------------------------------------
// AlarmAlertWidget::ShowAlarmDialog
// rest of the details are commented in the header
// ---------------------------------------------------------
//
bool AlarmAlertWidget::showAlarmDialog(SAlarmInfo *alarmInfo)
{
    // Get the subject of the alarm (description for clock alarms
    // and subject for calendar alarms)
	mSubject = QString::fromUtf16(alarmInfo->iSubject->Ptr(),
	                                     alarmInfo->iSubject->Length());
	
	// Get the location (for calendar alarms)
	mLocation = QString::fromUtf16(alarmInfo->iLocation->Ptr(),
	                                      alarmInfo->iLocation->Length());
	
	// Get the alarm date and time
	// TODO: Check QDateTime to TTime compatibility
	mAlarmTime.setTime(QTime(alarmInfo->iTime.DateTime().Hour(),
	                        alarmInfo->iTime.DateTime().Minute(),
	                        alarmInfo->iTime.DateTime().Second()));
	mAlarmTime.setDate(QDate(alarmInfo->iDate.DateTime().Year(),
                            alarmInfo->iDate.DateTime().Month(),
                            alarmInfo->iDate.DateTime().Day()));
	
	mCanSnooze = alarmInfo->iCanSnooze;
	mIsSilent = alarmInfo->iIsSilent;
	mAlarmAlertType = alarmInfo->iAlarmAlertType;
	mIsTimedAlarm = alarmInfo->iIsTimed;
	
    // Package the different parameters to send
	QVariantMap params;
	params.insert(alarmSubject, mSubject);
	params.insert(alarmLocation, mLocation);
	params.insert(alarmDateTime, mAlarmTime);
	params.insert(alarmRingingType, mIsSilent);
	params.insert(alarmCanSnooze, mCanSnooze);
	params.insert(alarmType, mAlarmAlertType);
	params.insert(alarmIsTimed, mIsTimedAlarm);

	return mDeviceDialog->show(ALARM_ALERT_PLUGIN, params);
}

// ---------------------------------------------------------
// AlarmAlertWidget::DismissAlarmDialog
// rest of the details are commented in the header
// ---------------------------------------------------------
//
bool AlarmAlertWidget::dismissAlarmDialog()
{
    // Dismiss the dialog
    return mDeviceDialog->cancel();
}

// ---------------------------------------------------------
// AlarmAlertWidget::UpdateAlarmDialog
// rest of the details are commented in the header
// ---------------------------------------------------------
//
bool AlarmAlertWidget::updateAlarmDialog(SAlarmInfo* alarmInfo)
{
    Q_UNUSED(alarmInfo);
    // Update the dialog with any new information
    // TODO: Pass the updated information sent by the observer
    QVariantMap params;
    return mDeviceDialog->update(params);
}

// ---------------------------------------------------------
// AlarmAlertWidget::triggerAction
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertWidget::triggerAction(QVariantMap params)
{
    // Check what is the command being sent by the dialog
    // Based on that, update the command to be sent
    QVariantMap::const_iterator iter = params.find(alarmCommand);
    if (iter != params.constEnd()) {
        AlarmCommand command(AlarmCmdLast);
        if (iter.value().toInt() == Stop) {
            command = AlarmStop;
        }
        else if (iter.value().toInt() == Snooze) {
            command = AlarmSnooze;
        }
        else if (iter.value().toInt() == Silence) {
            command = AlarmSilence;
        } else if (iter.value().toInt() == Shown) {
			command = AlarmShown;
			mObserver->alertDisplayed(command);
			return;
		} else {
		    command = AlarmCmdLast;
		}
        if (command != AlarmCmdLast) {
            // Notify the observer with the command
            mObserver->alertCompleted(command);        	
        }
        	
    }
}
// End of file  --Don't remove this.
