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
* Definition file for class ClockSettingsView.
*
*/


#include <hbdataform.h>
#include "datetimesettingsview.h"
#include "clocksettingsview.h"

DateTimeSettingsView::DateTimeSettingsView(QGraphicsItem *parent)
:CpBaseSettingView(0, parent)
{
	HbDataForm *form = settingForm();

	if (form) {
		form->setHeading(tr("Date & Time Settings"));

		ClockSettingsView *settingsView = new ClockSettingsView(this);
		settingsView->loadSettingsView();
    }
}

DateTimeSettingsView::~DateTimeSettingsView()
{
}
