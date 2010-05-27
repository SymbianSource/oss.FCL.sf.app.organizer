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

/*!
 \class CalenGridItemPrototype

 Prototype class for HbGridView.
 */

/*!
 Constructor.
 */
CalenGridItemPrototype::CalenGridItemPrototype(QColor todayIndColor, QColor activeColor, QColor inActiveColor,
                                               QGraphicsWidget *parent) :
	HbGridViewItem(parent),
	mTodayUnderLineColor(todayIndColor),
	mActiveTextColor(activeColor),
	mInActiveTextColor(inActiveColor),
	mCurrentDateColor(Qt::black),
	mGridBorderColor(Qt::gray),
	mEventIndicatorItem(0),
	mMonthDayInfoItem(0),
	mFocusIndicatorItem(0),
	mTodayIndicatorItem(0)
	{
	}

/*!
	Constructs all the primitives
*/
void CalenGridItemPrototype::createPrimitives()
{
	if (!mMonthDayInfoItem) {
		mMonthDayInfoItem = new HbTextItem(this);
		HbStyle::setItemName(mMonthDayInfoItem,
				 QLatin1String("monthDayInfoTextItem"));
		mMonthDayInfoItem->setElideMode(Qt::ElideNone);
	}
		
	if (!mFocusIndicatorItem) {
		mFocusIndicatorItem = new HbFrameItem(this);
		mFocusIndicatorItem->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
		mFocusIndicatorItem->setZValue(-1);
		HbStyle::setItemName(mFocusIndicatorItem, QLatin1String("focusIconItem"));
	}
	
	if (!mEventIndicatorItem) {
		mEventIndicatorItem = new HbIconItem(this);
		HbStyle::setItemName(mEventIndicatorItem, QLatin1String("eventIconItem"));
	}
}

/*!
 From HbAbstractViewItem.

 \return CalenGridItemPrototype* Pointer to the constructed object.
 \sa HbAbstractViewItem, HbGridViewItem
 */
HbAbstractViewItem *CalenGridItemPrototype::createItem()
{
	CalenGridItemPrototype* item = new CalenGridItemPrototype(*this);
	item->createPrimitives();
	
	// Set the default frame to NULL so that HbGridView does not add its 
	// default frame for items
	HbFrameBackground frame;
	frame.setFrameGraphicsName(QString(""));
	item->setDefaultFrame(frame);
	
	return item;
}

/*!
 From HbAbstractViewItem.

 \sa HbAbstractViewItem, HbGridViewItem
 */
void CalenGridItemPrototype::updateChildItems()
{		
	// Here update content of each item.
	QVariant monthDayRole;
	bool monthFocusRole;
	bool underlineEnabled = false;
	bool monthEventRole;
	bool monthTextColorRole;
	QVariant itemData = modelIndex().data(Qt::UserRole + 1);
	if (itemData.isValid()) {
		if (itemData.canConvert<QVariantList>()) {
		
			// Get the item list
			QVariantList itemList = itemData.toList();
			// Get the day text
			monthDayRole = itemList.at(CalendarNamespace::CalendarMonthDayRole);
			if (monthDayRole.canConvert<QString>()) {
				mMonthDayInfoItem->setText(monthDayRole.toString());
			}
			
			// Get the focus data
			monthFocusRole = itemList.at(CalendarNamespace::CalendarMonthFocusRole).value<bool>();
			if (monthFocusRole) {
				mFocusIndicatorItem->frameDrawer().setFrameGraphicsName(focusIconName);
			} else {
				mFocusIndicatorItem->frameDrawer().setFrameGraphicsName(QString(""));
			}
			
			// Get the today indicator role
			underlineEnabled = itemList.at(CalendarNamespace::CalendarMonthUnderlineRole).value<bool>();
			drawUnderline(underlineEnabled);
			
			// Get the event indicator data
			monthEventRole = itemList.at(CalendarNamespace::CalendarMonthEventRole).value<bool>();
			if (monthEventRole) {
				// Set the event indicator
				//QString iconName(focusIconName);
				mEventIndicatorItem->setIconName(eventIndname);
			} else {
				mEventIndicatorItem->setIconName(QString(""));
			}
			
			// Get the text color
			monthTextColorRole = itemList.at(CalendarNamespace::CalendarMonthTextColorRole).value<bool>();
			if (monthTextColorRole) {
				// Set the active text color
				mMonthDayInfoItem->setTextColor(mActiveTextColor);
			} else {
				// Set the inactive text color
				mMonthDayInfoItem->setTextColor(mInActiveTextColor);
			}
		}
	}
	
	// base class implementation
	HbGridViewItem::updateChildItems();
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
