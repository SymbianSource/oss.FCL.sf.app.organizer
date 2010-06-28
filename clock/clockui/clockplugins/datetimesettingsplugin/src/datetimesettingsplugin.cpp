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
* Definition file for class DateTimeSettingsPlugin.
*
*/
// System includes
#include <QDateTime>
#include <cpsettingformentryitemdataimpl.h>
#include <HbExtendedLocale>

// User includes
#include "datetimesettingsplugin.h"
#include "datetimesettingsview.h"

/*!
	Constructor.
 */
DateTimeSettingsPlugin::DateTimeSettingsPlugin()
{
}

/*!
	Destructor.
 */
DateTimeSettingsPlugin::~DateTimeSettingsPlugin()
{
}

/*!
	Returns the uid of the plugin.
 */
int DateTimeSettingsPlugin::uid() const
{
	// UID of the plugin.
	return 0x102818E9;
}

/*!
	Creates the object of DateTimeSettingsView class.
 */
QList<CpSettingFormItemData*> DateTimeSettingsPlugin:: 
						      createSettingFormItemData(
							  CpItemDataHelper &itemDataHelper) const
{
	HbIcon icon ;
	HbExtendedLocale locale = HbExtendedLocale::system();
	QString timeInfo = locale.format(
			QTime::currentTime(), r_qtn_time_usual_with_zero);
	QString dateinfo = locale.format(
			QDate::currentDate(), r_qtn_date_usual_with_zero);
	QString displayString;
	displayString.append(timeInfo);
	displayString.append(" ");
	displayString.append(dateinfo);
	
	QList<CpSettingFormItemData*> entryItemList;
	CpSettingFormItemData *entryItem = new DateTimeSettingsView(
			itemDataHelper,hbTrId("txt_cp_main_view_list_time_date"),
			displayString,icon);
	entryItemList.append(entryItem);
	return entryItemList;
}

Q_EXPORT_PLUGIN2(DateTimeSettingsPlugin, DateTimeSettingsPlugin)

// End of file	--Don't remove this
