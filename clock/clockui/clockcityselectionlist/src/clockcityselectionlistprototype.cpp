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
* The definition file for class ClockCitySelectionListPrototype.
*
*/

// System includes
#include <QGraphicsItem>
#include <HbTextItem>

// User includes
#include "clockcityselectionlistprototype.h"

/*!
	\class ClockCitySelectionListPrototype

	This is the custom prototype class for HbListView. It sub-classes
	HbListViewItem and re-implements HbListViewItem::createItem() and
	HbListViewItem::updateChildItems().

	\sa HbListViewItem, HbAbstractViewItem.
 */

/*!
	Default constructor.

	\param parent of type QGraphicsItem.
 */
ClockCitySelectionListPrototype::ClockCitySelectionListPrototype(
		QGraphicsItem *parent)
:HbListViewItem(parent),
 mCityNameItem(0)
{
	// Nothing yet.
}

/*!
	Destructor.
 */
ClockCitySelectionListPrototype::~ClockCitySelectionListPrototype()
{
	if (mCityNameItem) {
		delete mCityNameItem;
		mCityNameItem = 0;
	}
}

/*!
	From HbListViewItem.
	Constructs the custom list view item.

	\sa HbListViewItem, HbAbstractViewItem.
 */
HbAbstractViewItem *ClockCitySelectionListPrototype::createItem()
{
	return new ClockCitySelectionListPrototype(*this);
}

/*!
	From HbListViewItem.
	Updates the child items of the cutsom prototype.

	\sa HbListViewItem, HbAbstractViewItem.
 */
void ClockCitySelectionListPrototype::updateChildItems()
{
	HbListViewItem::updateChildItems();

	if (!mCityNameItem) {
		mCityNameItem = new HbTextItem(this);
		style()->setItemName(mCityNameItem, "cityNameItem");
		mCityNameItem->setTextWrapping(Hb::TextWordWrap);
	}
	mCityNameItem->setText(
			modelIndex().data(Qt::UserRole + 100).value<QString>());
}

// End of file	--Don't remove this.
