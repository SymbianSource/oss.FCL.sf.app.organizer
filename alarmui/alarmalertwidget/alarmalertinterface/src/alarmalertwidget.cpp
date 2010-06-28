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
#include <hbsymbianvariant.h>

// User includes
#include "alarmalertwidget.h"
#include "alarmalert.h"
#include "alarmalertobserver.h"

// Constants.
const TInt KMaxlength=30;

// ---------------------------------------------------------
// AlarmAlertWidget::AlarmAlertWidget
// rest of the details are commented in the header
// ---------------------------------------------------------
//
AlarmAlertWidget::AlarmAlertWidget(AlarmAlertObserver *observer) :
    mObserver(observer)
{
}

// ---------------------------------------------------------
// AlarmAlertWidget::~AlarmAlertWidget
// rest of the details are commented in the header
// ---------------------------------------------------------
//
AlarmAlertWidget::~AlarmAlertWidget()
{
	// Cleanup
	
	if (mAlarmSubject) {
		delete mAlarmSubject;
	}

	if (mLocation) {
		delete mLocation;
	}

	if (mAlarmTime) {
		delete mAlarmTime;
	}
	
	if (mAlarmDate) {
        delete mAlarmDate;
	}

	if (mAlarmAlertType) {
		delete mAlarmAlertType;
	}

	if (mCanSnooze) {
		delete mCanSnooze;
	}

	if (mIsSilent) {
		delete mIsSilent;
	}

	if (mIsTimedAlarm) {
		delete mIsTimedAlarm;
	}

	if (mVariantMap) {
		delete mVariantMap;
	}

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
	// Get the alarm date and time
    TBuf<KMaxlength> timeString;
    TBuf<KMaxlength> dateString;
    
    TBuf<KMaxlength> timeFormat(_L("%-B%:0%J%:1%T%:3%+B"));
    TBuf<KMaxlength> dateFormat;
    TLocale locale;
    switch (locale.DateFormat()) {
        case EDateEuropean:
            dateFormat.Copy(_L("%F%/0%D%/1%M%/2%Y%/3"));
            break;
        case EDateAmerican:
            dateFormat.Copy(_L("%F%/0%M%/1%D%/2%Y%/3"));
            break;
        case EDateJapanese:
            dateFormat.Copy(_L("%F%/0%Y%/1%M%/2%D%/3"));
            break;
        default:
            // Nothing yet.
            break;
        }

	
	alarmInfo->iTime.FormatL(timeString, timeFormat);
	alarmInfo->iTime.FormatL(dateString, dateFormat); 

	// Create a session with the device dialog server
    mDeviceDialog = CHbDeviceDialogSymbian::NewL();
	

    // Package the different parameters to send
	mVariantMap = CHbSymbianVariantMap::NewL();

	// Get the subject of the alarm (description for clock alarms
	// and subject for calendar alarms)
	mAlarmSubject = CHbSymbianVariant::NewL(
			alarmInfo->iSubject, CHbSymbianVariant::EDes );
	mVariantMap->Add(alarmSubjectSymbian, mAlarmSubject);

	// Get the location (for calendar alarms)
	mLocation = CHbSymbianVariant::NewL(
			alarmInfo->iLocation, CHbSymbianVariant::EDes);
	mVariantMap->Add(alarmLocationSymbian, mLocation);

	mAlarmTime = CHbSymbianVariant::NewL(
	        &timeString, CHbSymbianVariant::EDes);
	mVariantMap->Add(alarmTimeSymbian, mAlarmTime);
	
	mAlarmDate = CHbSymbianVariant::NewL(
	        &dateString, CHbSymbianVariant::EDes);
	mVariantMap->Add(alarmDateSymbian, mAlarmDate);
	    
	mIsSilent = CHbSymbianVariant::NewL(
			&alarmInfo->iIsSilent, CHbSymbianVariant::EBool);
	mVariantMap->Add(alarmRingingTypeSymbian, mIsSilent);

	mCanSnooze = CHbSymbianVariant::NewL(
			&alarmInfo->iCanSnooze, CHbSymbianVariant::EBool);
	mVariantMap->Add(alarmCanSnoozeSymbain, mCanSnooze);

	mAlarmAlertType = CHbSymbianVariant::NewL(
			&alarmInfo->iAlarmAlertType, CHbSymbianVariant::EInt);
	mVariantMap->Add(alarmTypeSymbian, mAlarmAlertType);

	mIsTimedAlarm = CHbSymbianVariant::NewL(
			&alarmInfo->iIsTimed, CHbSymbianVariant::EBool);
	mVariantMap->Add(alarmIsTimedSymbian, mIsTimedAlarm);

	// Package the different parameters to send
	return mDeviceDialog->Show(ALARM_ALERT_PLUGIN_SYMBIAN, *mVariantMap, this);
	
}

// ---------------------------------------------------------
// AlarmAlertWidget::DismissAlarmDialog
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertWidget::dismissAlarmDialog()
{
    // Dismiss the dialog
    mDeviceDialog->Cancel();
}

// ---------------------------------------------------------
// AlarmAlertWidget::UpdateAlarmDialog
// rest of the details are commented in the header
// ---------------------------------------------------------
//
bool AlarmAlertWidget::updateAlarmDialog(SAlarmInfo* /*alarmInfo*/)
{
    // Update the dialog with any new information
    // TODO: Pass the updated information sent by the observer
    /*CHbSymbianVariantMap params;
    mDeviceDialog->Update(params);*/
    return false;
}

// ---------------------------------------------------------
// AlarmAlertWidget::DataReceived
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertWidget::DataReceived(CHbSymbianVariantMap& aData)
{
    if (!aData.Keys().MdcaCount()) {
        return;
    }
    
    triggerAction(aData.Get(alarmCommandSymbian));
}

// ---------------------------------------------------------
// AlarmAlertWidget::DeviceDialogClosed
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertWidget::DeviceDialogClosed(TInt /*aCompletionCode*/)
{
  
}

// ---------------------------------------------------------
// AlarmAlertWidget::triggerAction
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void AlarmAlertWidget::triggerAction(const CHbSymbianVariant* source)
{
       AlarmCommand command(AlarmCmdLast);
       
       if (*source->Value<TInt>() == Stop) {    
       command = AlarmStop;    
       } else if (*source->Value<TInt>() == Snooze) {
       command = AlarmSnooze;
       } else if (*source->Value<TInt>() == Silence) {
       command = AlarmSilence;
       } else if (*source->Value<TInt>() == Shown) {
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

// End of file  --Don't remove this.
