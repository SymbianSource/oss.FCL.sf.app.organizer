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


#include <hbdataform.h>
#include "calendarsettingsview.h"
#include "calensettings.h"

CalendarSettingsView::CalendarSettingsView(QGraphicsItem *parent)
:CpBaseSettingView(0, parent)
{
	HbDataForm *form = settingForm();
	
	if (form) {
		form->setHeading(hbTrId("txt_calendar_subhead_calendar_settings"));
		
		CalenSettings *settings = new CalenSettings(form);
		settings->createModel();
		settings->populateSettingList();
    }
}

CalendarSettingsView::~CalendarSettingsView()
{
}
