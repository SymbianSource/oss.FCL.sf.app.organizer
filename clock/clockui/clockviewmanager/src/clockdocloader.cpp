/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Definition file for class ClockDocLoader.
 *
*/

// System includes.
#include <QDebug>

// User includes
#include "clockdocloader.h"
#include "clockcommon.h"
#include "clockmainview.h"
#include "skinnableclock.h"
#include "clockworldview.h"

/*!
	\class ClockDocLoader

	Inherits from HbDocumentLoader
	Called whenever the docml file is parsed to create objects
	Over-riding it since we need to create custom views and widgets.

	\param type The object type which is being created. For ex: HbWidget
	\param name The name of the created object

	\return QObject* Pointer to the created object
 */
QObject *ClockDocLoader::createObject(const QString &type, const QString &name)
{
	qDebug() << "clock: ClockDocLoader::createObject -->";

	if (CLOCK_MAIN_VIEW == name) {
		QObject *object = new ClockMainView();
		object->setObjectName(name);
		return object;
	} else if (CLOCK_WIDGET == name) {
		QObject *object = new SkinnableClock();
		object->setObjectName(name);
		return object;
    } else if (WORLD_CLOCK_VIEW == name) {
		QObject *object = new ClockWorldView();
		object->setObjectName(name);
		return object;
    }

	qDebug() << "clock: ClockDocLoader::createObject <--";

	return HbDocumentLoader::createObject(type, name);
}

// End of file	--Don't remove this.
