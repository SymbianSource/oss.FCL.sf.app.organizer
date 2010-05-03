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
 * Description: Definition file for class CalenGridItemPrototype.
 *
 */

// System includes
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <hblabel.h>
#include <hbtextitem.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <hbiconitem.h>
#include <hbframebackground.h>
#include <hbcolorscheme.h>

// User includes
#include "calengriditemprototype.h"
#include "calencommon.h"

#define GRIDLINE_WIDTH 0.075 //units
/*!
 \class CalenGridItemPrototype

 Prototype class for HbGridView.
 */

/*!
 Constructor.
 */
CalenGridItemPrototype::CalenGridItemPrototype(QGraphicsWidget *parent) :
	HbGridViewItem(parent),
	mLayout(0),
	mCurrentDateColor(Qt::black),
	mGridBorderColor(Qt::gray),
	mEventIndicatorItem(0),
	mMonthDayInfoItem(0),
	mFocusIndicatorItem(0),
	mTodayIndicatorItem(0),
	mTodayUnderLineColor(Qt::gray)
	{
	mTodayUnderLineColor = HbColorScheme::color("qtc_cal_month_current_day");
	}

/*!
 From HbAbstractViewItem.

 \return CalenGridItemPrototype* Pointer to the constructed object.
 \sa HbAbstractViewItem, HbGridViewItem
 */
HbAbstractViewItem *CalenGridItemPrototype::createItem()
{
	return new CalenGridItemPrototype(*this);
}

/*!
 From HbAbstractViewItem.

 \sa HbAbstractViewItem, HbGridViewItem
 */
void CalenGridItemPrototype::updateChildItems()
{
	
	// base class implementation
	HbGridViewItem::updateChildItems();
		
	// Here update content of each item.
	QVariant monthDayRole;
	QVariant monthFocusRole;
	bool underlineEnabled = false;
	QVariant monthEventRole;
	QVariant monthTextColorRole;
	QVariant itemData = modelIndex().data(Qt::UserRole + 1);
	if (itemData.isValid()) {
		if (itemData.canConvert<QVariantList>()) {
			// Get the item list
			QVariantList itemList = itemData.toList();
			// Get the day text
			monthDayRole = itemList.at(CalendarNamespace::CalendarMonthDayRole);
			if (monthDayRole.canConvert<QString>()) {
				QString monthDayText = monthDayRole.toString();

				if (!mMonthDayInfoItem) {
					mMonthDayInfoItem = new HbTextItem(this);
					HbStyle::setItemName(mMonthDayInfoItem,
							 QLatin1String("monthDayInfoTextItem"));

				}

				mMonthDayInfoItem->setText(monthDayText);
				mMonthDayInfoItem->setElideMode(Qt::ElideNone);
			}
			
			// Get the focus data
			monthFocusRole = itemList.at(CalendarNamespace::CalendarMonthFocusRole);
			if (monthFocusRole.canConvert<QString>()) {
				QString focusIconPath = monthFocusRole.toString();
				if (!mFocusIndicatorItem) {
					mFocusIndicatorItem = new HbFrameItem(this);
					mFocusIndicatorItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
					mFocusIndicatorItem->setZValue(-1);
					HbStyle::setItemName(mFocusIndicatorItem, QLatin1String("focusIconItem"));
				}
				mFocusIndicatorItem->frameDrawer().setFrameGraphicsName(focusIconPath);
			}
			
			// Get the today indicator role
			underlineEnabled = itemList.at(CalendarNamespace::CalendarMonthUnderlineRole).value<bool>();
			drawUnderline(underlineEnabled);
			
			// Get the event indicator data
			monthEventRole = itemList.at(CalendarNamespace::CalendarMonthEventRole);
			if (monthEventRole.canConvert<QString>()) {
				QString eventIconPath = monthEventRole.toString();
				if (!mEventIndicatorItem) {
					mEventIndicatorItem = new HbIconItem(this);
					HbStyle::setItemName(mEventIndicatorItem, QLatin1String("eventIconItem"));
				}
				mEventIndicatorItem->setIconName(eventIconPath);
			}
			
			// Get the text color
			monthTextColorRole = itemList.at(CalendarNamespace::CalendarMonthTextColorRole);
			if (monthTextColorRole.canConvert<QColor>()) {
				QColor monthTextColor = monthTextColorRole.value<QColor>();
				mMonthDayInfoItem->setTextColor(monthTextColor);
			}
		}
	}
	
}

/*!
 Function to create the underline icon item
 */
void CalenGridItemPrototype::drawUnderline(bool underlineEnabled)
{
	if (underlineEnabled) {
		if (!mTodayIndicatorItem) {
			mTodayIndicatorItem = new HbIconItem(this);
			HbStyle::setItemName(mTodayIndicatorItem,
								 QLatin1String("todayIndicatorItem"));
			if (mTodayUnderLineColor.isValid()) {
				mTodayIndicatorItem->setColor(mTodayUnderLineColor);
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
 Function overwritten to avoid default behavior
 */
void CalenGridItemPrototype::pressStateChanged(bool pressed,bool animate)
{
	Q_UNUSED(pressed)
	Q_UNUSED(animate)		
	// Just overrode it as we dont want to have any default behavior
}

/*!
 Function overwritten to avoid default behavior
 */
void CalenGridItemPrototype::pressStateChanged(bool animate)
{
	Q_UNUSED(animate)
	// Just overrode it as we dont want to have any default behavior
}

bool CalenGridItemPrototype::canSetModelIndex(const QModelIndex& index)
{
	Q_UNUSED(index)
	return true;
}

// End of file  --Don't remove this.
