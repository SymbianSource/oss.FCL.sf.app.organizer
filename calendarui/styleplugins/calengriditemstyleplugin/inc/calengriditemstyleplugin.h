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
 * Description: Header file for class CalenGridItemStylePlugin.
 *
 */

#ifndef CALENGRIDITEMSTYLEPLUGIN_H
#define CALENGRIDITEMSTYLEPLUGIN_H

// System includes
#include <QObject>
#include <hbstyleinterface.h>

// Forward declarations
class QGraphicsItem;
class QStyleOption;

class CalenGridItemStylePlugin : public QObject, public HbStyleInterface
{
    Q_OBJECT Q_INTERFACES(HbStyleInterface)

public:
	int primitiveCount() const;
	QGraphicsItem *createPrimitive(HbStyle::Primitive primitive,
	                              QGraphicsItem *parent = 0) const;
	void updatePrimitive(QGraphicsItem *item, HbStyle::Primitive primitive,
	                     const QStyleOption *option) const;
	QString layoutPath() const;
};

#endif // CALENGRIDITEMSTYLEPLUGIN_H

// End of file  --Don't remove this.
