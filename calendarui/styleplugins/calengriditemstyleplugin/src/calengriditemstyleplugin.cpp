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
 * Description: Definition file for class CalenGridItemStylePlugin.
 *
 */

// System includes
#include <QtPlugin>
#include <QGraphicsItem>
#include <hbiconitem.h>
#include <hbtextitem.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>

// User includes
#include "calengriditemstyleplugin.h"

// Constants
#define PRIMITIVE_COUNT 1

/*!
 \class CalenGridItemStylePlugin
 
 Style plugin of Custom Grid Item of Calendar
 */

/*!
 From HbStyleInterface.
 Returns the number of primitives in the widget.

 \return int The number of primitives.
 \sa HbStyleInterface.
 */
int CalenGridItemStylePlugin::primitiveCount() const
{
	qDebug("CalenGridItemStylePlugin::primitiveCount");

	return PRIMITIVE_COUNT;
}

/*!
 From HbStyleInterface.

 \sa HbStyleInterface.
 */
QGraphicsItem *CalenGridItemStylePlugin::createPrimitive(
                                                        HbStyle::Primitive primitive,
                                                        QGraphicsItem *parent) const
{
	qDebug("CalenGridItemStylePlugin::createPrimitive");
    Q_UNUSED(primitive)
    Q_UNUSED(parent)
    return NULL;
}

/*!
 From HbStyleInterface.

 \sa HbStyleInterface.
 */
void CalenGridItemStylePlugin::updatePrimitive(QGraphicsItem *item,
                                               HbStyle::Primitive primitive,
                                               const QStyleOption *option) const
{
	qDebug("CalenGridItemStylePlugin::updatePrimitive");
	
	  Q_UNUSED(item)
	  Q_UNUSED(primitive)
	  Q_UNUSED(option)

}

/*!
 From HbStyleInterface.

 \sa HbStyleInterface.
 */
QString CalenGridItemStylePlugin::layoutPath() const
{
	qDebug("CalenGridItemStylePlugin::layoutPath");

	QString layoutPath(":/");

	return layoutPath;
}

/*!
 Plugin registration.
 */
Q_EXPORT_PLUGIN2(calengriditemstyleplugin, CalenGridItemStylePlugin)


// End of file  --Don't remove this.
