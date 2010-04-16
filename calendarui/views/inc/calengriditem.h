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
 * Description: Definition file for class CalenGridItem.
 *
 */
#ifndef CALENGRIDITEM_H
#define CALENGRIDITEM_H

// System includes
#include <hbwidget.h>

// Forward declarations
class HbAbstractItem;
class QString;
class HbIconItem;
class HbTextItem;
class HbFrameItem;
class CalenTodayIndicator;
class QGraphicsWidget;

class CalenGridItem : public HbWidget
{
	Q_OBJECT

public:
	CalenGridItem(QGraphicsWidget *parent = 0);
	virtual ~CalenGridItem();

public:
	void updateMonthDayInfo(QString monthDayText);
	void updateMonthDayInfoColor(QColor monthTextColor);
	void updateEventIndicator(QString eventIconPath);
	void updateFocusIndicator(QString focusIconPath);
	void drawUnderline(bool underlineEnabled);
	
private:

	QString pluginPath();
	void init();

private:
	HbIconItem *mEventIndicatorItem;
	HbTextItem *mMonthDayInfoItem;
	HbFrameItem *mFocusIndicatorItem;
	HbIconItem* mTodayIndicatorItem;
};

#endif // CALENGRIDITEM_H

// End of file  --Don't remove this.
