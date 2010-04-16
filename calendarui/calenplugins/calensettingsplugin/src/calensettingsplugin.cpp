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

#include <cpsettingformentryitemdataimpl.h>

#include "calensettingsplugin.h"
#include "calendarsettingsview.h"
#include <cpitemdatahelper.h>


CalenSettingsPlugin::CalenSettingsPlugin()
{
}

CalenSettingsPlugin::~CalenSettingsPlugin()
{
}

int CalenSettingsPlugin::uid() const
{
	// TODO: get a uid and replace it.
    return 0x20029F7F;
}

CpSettingFormItemData *CalenSettingsPlugin::createSettingFormItemData(CpItemDataHelper& itemDataHelper) const
{
    //CpItemDataHelper* pHelper = new CpItemDataHelper();
	
	// For now dummyicon
    HbIcon* icon = new HbIcon();
    return new CpSettingFormEntryItemDataImpl<CalendarSettingsView>(itemDataHelper, QString("Calendar application"),
                                          QString("Calendar settings plugin"), *icon);
}

Q_EXPORT_PLUGIN2(CalenSettingsPlugin, CalenSettingsPlugin)
