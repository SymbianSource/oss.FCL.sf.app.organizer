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
* Implementation file for class ClockSettingsDocLoader.
*
*/

// System includes.
#include <QDebug>

// User includes
#include "clocksettingsdocloader.h"
#include "clocksettingsdefines.h"
#include "skinnableclock.h"
#include "clocksettingsview.h"

/*!
	\class ClockSettingsDocLoader

	Inherits from HbDocumentLoader
	Called whenever the docml file is parsed to create objects
	Over-riding it since we need to create custom views and widgets.

	\param type The object type which is being created. For ex: HbWidget
	\param name The name of the created object

	\return QObject* Pointer to the created object
 */
QObject *ClockSettingsDocLoader::createObject(const QString &type, const QString &name)
{
	qDebug() << "clock: ClockSettingsDocLoader::createObject -->";

	if (CLOCK_WIDGET == name) {
		QObject *object = new SkinnableClock();
		object->setObjectName(name);
		return object;
	}

	qDebug() << "clock: ClockSettingsDocLoader::createObject <--";

	return HbDocumentLoader::createObject(type, name);
}

// End of file	--Don't remove this.
