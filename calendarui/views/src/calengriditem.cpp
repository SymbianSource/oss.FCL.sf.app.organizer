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

// System includes
#include <QString>
#include <QGraphicsWidget>
#include <hbstyle.h>
#include <hbwidget.h>
#include <hbtextitem.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <hbiconitem.h>
#include <hbcolorscheme.h>

// User includes
#include "calengriditem.h"

#define PLUGINPATH "calengriditemstyleplugin.dll"

/*!
 \class CalenGridItem

 Custom Calendar Grid Item class
 */

/*!
 Default constructor.
 */
CalenGridItem::CalenGridItem(QGraphicsWidget *parent):
	HbWidget(parent),
	mEventIndicatorItem(NULL),
	mMonthDayInfoItem(NULL),
	mFocusIndicatorItem(NULL),
	mTodayIndicatorItem(NULL)
{
	qDebug("CalenGridItem::CalenGridItem");

	// Here we register the plugin.
	setPluginBaseId(style()->registerPlugin(pluginPath()));

	// Now create each of the primitives.
	init();
	
}

/*!
 Destructor.
 */
CalenGridItem::~CalenGridItem()
{
	//	Here we un-register the plugin .
	style()->unregisterPlugin(pluginPath());
}

/*!
 To update the month day info
 */
void CalenGridItem::updateMonthDayInfo(QString monthDayText)
{
	mMonthDayInfoItem->setText(monthDayText);
	mMonthDayInfoItem->setElideMode(Qt::ElideNone);
}

/*!
 To update the month day info color
 */
void CalenGridItem::updateMonthDayInfoColor(QColor monthTextColor)
{
	mMonthDayInfoItem->setTextColor(monthTextColor);	
}

/*!
 To update the event indicator icon
 */
void CalenGridItem::updateEventIndicator(QString eventIconPath)
{
	mEventIndicatorItem->setIconName(eventIconPath);
}

/*!
 To update the focus indicator icon
 */
void CalenGridItem::updateFocusIndicator(QString focusIconPath)
{
	mFocusIndicatorItem->frameDrawer().setFrameGraphicsName(focusIconPath);
}

/*!
 If underlineEnabled is true then will draw underline else not
 */
void CalenGridItem::drawUnderline(bool underlineEnabled)
{
	if (underlineEnabled) {
		if (!mTodayIndicatorItem) {
			mTodayIndicatorItem = new HbIconItem(this);
			HbStyle::setItemName(mTodayIndicatorItem,
			                     QLatin1String("todayIndicatorItem"));
			// Get the color from the theme
			QColor todayColor = HbColorScheme::color("qtc_cal_month_current_day");
			if (todayColor.isValid()) {
			    mTodayIndicatorItem->setColor(todayColor);
			}
		}
	} else {
		if (mTodayIndicatorItem) {
			delete mTodayIndicatorItem;
			mTodayIndicatorItem = NULL;
		}
	}
}

/*!
 Returns the style plugin path
 */
QString CalenGridItem::pluginPath()
{
	QString pluginPath;
	pluginPath.append(PLUGINPATH);
	return pluginPath;
}

/*!
 Create primitives 
 */
void CalenGridItem::init()
{

	mEventIndicatorItem = new HbIconItem(this);
	HbStyle::setItemName(mEventIndicatorItem, QLatin1String("eventIconItem"));

	mMonthDayInfoItem = new HbTextItem(this);
	HbStyle::setItemName(mMonthDayInfoItem,
	                     QLatin1String("monthDayInfoTextItem"));

	mFocusIndicatorItem = new HbFrameItem(this);
	mFocusIndicatorItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
	mFocusIndicatorItem->setZValue(-1);
	HbStyle::setItemName(mFocusIndicatorItem, QLatin1String("focusIconItem"));
	
}

// End of file  --Don't remove this.
