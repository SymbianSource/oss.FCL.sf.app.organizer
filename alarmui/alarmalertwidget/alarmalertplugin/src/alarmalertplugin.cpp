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
// alarmalertplugin.h

// System includes
#include <QString>
#include <QVariantMap>
#include <hbdevicedialog.h>

// User includes
#include "alarmalertplugin.h"
#include "alarmalertwidget_p.h"
#include "alarmalert.h"
#include "alarmalertdocloader.h"

// Plugin export
Q_EXPORT_PLUGIN2(alarmalertplugin, AlarmAlertPlugin);

// ---------------------------------------------------------
// AlarmAlertPlugin::AlarmAlertPlugin
// rest of the details are commented in the header
// ---------------------------------------------------------
//
AlarmAlertPlugin::AlarmAlertPlugin()
{
	// No implementation yet
}

// ---------------------------------------------------------
// AlarmAlertPlugin::~AlarmAlertPlugin
// rest of the details are commented in the header
// ---------------------------------------------------------
//
AlarmAlertPlugin::~AlarmAlertPlugin()
{
    // No implementation yet
}

// ---------------------------------------------------------
// AlarmAlertPlugin::accessAllowed
// rest of the details are commented in the header
// ---------------------------------------------------------
//
bool AlarmAlertPlugin::accessAllowed(const QString &deviceDialogType,
                                             const QVariantMap &parameters,
                                             const QVariantMap &securityInfo) const
{
    Q_UNUSED(securityInfo);
    Q_UNUSED(parameters);
    Q_UNUSED(deviceDialogType);
    
    // No implementation yet
    return true;
}

// ---------------------------------------------------------
// AlarmAlertPlugin::deviceDialogInfo
// rest of the details are commented in the header
// ---------------------------------------------------------
//
bool AlarmAlertPlugin::deviceDialogInfo(const QString &deviceDialogType,
                                                const QVariantMap &parameters,
                                                DeviceDialogInfo *info) const
{
    Q_UNUSED(deviceDialogType);
    Q_UNUSED(parameters);

    info->group = CriticalGroup;
    info->priority = DefaultPriority;
    info->flags = NoDeviceDialogFlags;
    return true;
}

// ---------------------------------------------------------
// AlarmAlertPlugin::deviceDialogTypes
// rest of the details are commented in the header
// ---------------------------------------------------------
//
QStringList AlarmAlertPlugin::deviceDialogTypes() const
{
    // Return the device dialog type this plugin supports 
    QStringList dialogType;
    dialogType.append(ALARM_ALERT_PLUGIN);
    return dialogType;
}

// ---------------------------------------------------------
// AlarmAlertPlugin::pluginFlags
// rest of the details are commented in the header
// ---------------------------------------------------------
//
HbDeviceDialogPlugin::PluginFlags AlarmAlertPlugin::pluginFlags() const
{
    return NoPluginFlags;
}

// ---------------------------------------------------------
// AlarmAlertPlugin::error
// rest of the details are commented in the header
// ---------------------------------------------------------
//
int AlarmAlertPlugin::error() const
{
    // TODO: Need to handle any error that might occur
    return 0;
}

// ---------------------------------------------------------
// AlarmAlertPlugin::createDeviceDialog
// rest of the details are commented in the header
// ---------------------------------------------------------
//
HbDeviceDialogInterface *AlarmAlertPlugin::createDeviceDialog(const QString &deviceDialogType,
                                                              const QVariantMap &parameters)
{
	Q_UNUSED(deviceDialogType);
    
	// Create and return the alarm alert dialog
    HbDeviceDialogInterface *ret(0);
    AlarmAlertDocLoader *alertDocLoader = new AlarmAlertDocLoader(parameters);
    // Get the type of alarm to load the proper docml
    QVariantMap::const_iterator iter = parameters.constBegin();
    int count = parameters.size();
    AlarmType alertType = OtherAlarm;
	while (iter != parameters.constEnd()) {
		QString key(iter.key());
		if (alarmType == key) {
			alertType = static_cast <AlarmType> (iter.value().toInt());
		}
		iter++;
	}
    bool loadSuccess = false;
    if (alertType == ClockAlarm) {
    	alertDocLoader->load(alarmNormalUIClockDocml, &loadSuccess);
    }else if (alertType == TodoAlarm) {
    	alertDocLoader->load(alarmNormalUITodoDocml, &loadSuccess);
    }else if(alertType == CalendarAlarm) {
    	alertDocLoader->load(alarmNormalUICalendarDocml, &loadSuccess);
    }
    if(!loadSuccess) {
    	qFatal("Unable to load the docml");
    }
    AlarmAlertDialogPrivate *alertDialog = qobject_cast<AlarmAlertDialogPrivate*> (alertDocLoader->findWidget("dialog"));
    alertDialog->setupNormalUI(alertDocLoader);
    ret = alertDialog;
    return ret;
}
