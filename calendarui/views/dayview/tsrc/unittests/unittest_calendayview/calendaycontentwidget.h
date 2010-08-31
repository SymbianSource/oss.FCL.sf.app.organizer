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
 * Description:  CalenDayContentWidget class definition.
 *
 */

#ifndef CALENDAYCONTENTWIDGET_H_
#define CALENDAYCONTENTWIDGET_H_

// System includes
#include <hbwidget.h>

// User includes
#include "calendaycommonheaders.h"

// Forward declarations
class QGraphicsLinearLayout;
class CalenDayModelManager;

class CalenDayContentWidget : public HbWidget
	{
Q_OBJECT

public:
	/*!
	 \enum CalenWidgetPosition
	 \brief Enumeration identifies widget position if queue.
	 */
	enum CalenWidgetPosition
		{
		ECalenLastWidget = 0, ECalenFirstWidget
		};

public:
	CalenDayContentWidget(CalenDayModelManager &modelManager,
			QGraphicsItem *parent = 0)
		{
		Q_UNUSED(parent);
		Q_UNUSED(modelManager);
		}
	~CalenDayContentWidget()
		{
		}

	void add(HbWidget* item, CalenWidgetPosition where = ECalenLastWidget)
		{
		Q_UNUSED(item);
		Q_UNUSED(where);
		}
	HbWidget* take(CalenWidgetPosition which)
		{
		Q_UNUSED(which);
		return 0;
		}
	void remove(CalenWidgetPosition which)
		{
		Q_UNUSED(which);
		}

	signals:
	void widgetsRelayoutFinished(CalenScrollDirection scrollTo);
	void scrollPositionChanged(const QPointF &newPos);

public slots:
	void relayoutWidgets(CalenScrollDirection scrollTo)
		{
		Q_UNUSED(scrollTo);
		}
	void widgetScrolled(const QPointF &newPos)
		{
		Q_UNUSED(newPos);
		}
	};

#endif /* CALENDAYCONTENTWIDGET_H_ */
