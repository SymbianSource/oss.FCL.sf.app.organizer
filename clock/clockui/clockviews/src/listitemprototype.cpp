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
* Definition file for class ListItemPrototype.
*
*/

// System includes
#include <QDebug>
#include <QGraphicsLinearLayout>
#include <HbAbstractViewItem>
#include <HbIconItem>
#include <HbStyle>
#include <HbTextItem>

// User includes
#include "listitemprototype.h"

/*!
	\class ListItemPrototype

	Prototype class for the World clock city list.
 */

/*!
	Default constructor.

	\parent of type QGraphicsItem.
 */
ListItemPrototype::ListItemPrototype(QGraphicsItem *parent)
:HbListViewItem(parent),
 mDayNightIcon(0),
 mDateText(0),
 mCityText(0),
 mOffsetText(0),
 mDstIcon(0),
 mTimeText(0)
{
	// Nothing yet.
}

/*!
	Destructor.
 */
ListItemPrototype::~ListItemPrototype()
{
	// Nothing yet.
}

/*!
	From HbListViewItem.

	\sa HbAbstractViewItem, HbListViewItem.
 */
HbAbstractViewItem *ListItemPrototype::createItem()
{
	return new ListItemPrototype(*this);
}

/*!
	Called for every view item in the list.

	\sa HbAbstractViewItem, HbListViewItem.
 */
void ListItemPrototype::updateChildItems()
{
	// First allow the base class to update.
	HbListViewItem::updateChildItems();

	// Create the individual items.
	// Create day-night icon.
	if (!mDayNightIcon) {
		mDayNightIcon = new HbIconItem(this);
		style()->setItemName(mDayNightIcon, "dayNightIcon");
	}
	mDayNightIcon->setIconName(
			modelIndex().data(Qt::UserRole + 1000).value<QString>());
			
	// Create the date text item.
	if (!mDateText) {
		mDateText = new HbTextItem(this);
		style()->setItemName(mDateText, "dateText");
	}
	mDateText->setText(modelIndex().data(Qt::UserRole + 1001).toString());

	// Create the city, country name text item.
	if (!mCityText) {
		mCityText = new HbTextItem(this);
		mCityText->setTextWrapping(Hb::TextNoWrap);
		style()->setItemName(mCityText, "cityText");
	}
	mCityText->setText(modelIndex().data(Qt::UserRole + 1002).toString());

	// Create the offset text item.
	if (!mOffsetText) {
		mOffsetText = new HbTextItem(this);
		style()->setItemName(mOffsetText, "offsetText");
	}
	mOffsetText->setText(modelIndex().data(Qt::UserRole + 1003).toString());

	// Create dst icon.
	if (!mDstIcon) {
		mDstIcon = new HbIconItem(this);
		style()->setItemName(mDstIcon, "dstIcon");
	}
	mDstIcon->setIconName(
			modelIndex().data(Qt::UserRole + 1004).value<QString>());

	// Create the time text item.
	if (!mTimeText) {
		mTimeText = new HbTextItem(this);
		style()->setItemName(mTimeText, "timeText");
	}
	mTimeText->setText(modelIndex().data(Qt::UserRole + 1005).toString());
}

// End of file	--Don't remove this.
