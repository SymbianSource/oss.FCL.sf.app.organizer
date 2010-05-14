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
* Definition file for class ClockAlarmListItemPrototype.
*
*/

// System includes
#include <HbPushButton>

// User includes
#include "clockalarmlistitemprototype.h"
#include "clockdatatypes.h"
#include "clockmainview.h"

/*!
	\class ClockAlarmListItemPrototype

	TODO: Write description here.
 */

/*!
	Default constructor.

	\parent of type QGraphicsItem.
 */
ClockAlarmListItemPrototype::ClockAlarmListItemPrototype(QGraphicsItem *parent)
:HbListViewItem(parent)
{
	// Nothing yet.
}

/*!
	Destructor.
 */
ClockAlarmListItemPrototype::~ClockAlarmListItemPrototype()
{
	// Nothing yet.
}

/*!
	Called for every view item in the list.

	\sa HbAbstractViewItem, HbListViewItem.
 */
void ClockAlarmListItemPrototype::updateChildItems()
{
	// First allow the base class to update.
	HbListViewItem::updateChildItems();

	QVariant alarmIconRole = modelIndex().data(AlarmIcon).toString();
	if (alarmIconRole.isValid()) {
		if (alarmIconRole.canConvert<QString> ()) {
			QString alarmIconPath = alarmIconRole.toString();
			if (!malarmIconItem) {
				malarmIconItem = new HbPushButton(this);
				HbStyle::setItemName(
						malarmIconItem, QLatin1String("alarmIconItem"));
				connect(
						malarmIconItem, SIGNAL(clicked()),
						this, SLOT(handleAlarmStatusChanged()));
			}
			malarmIconItem->setIcon(alarmIconPath);
		}
	}
}

/*!
	Called to construct a HbListViewItem.

	\return AlarmListItemPrototype* Pointer to the constructed view item.
 */
ClockAlarmListItemPrototype *ClockAlarmListItemPrototype::createItem()
{
	return new ClockAlarmListItemPrototype(*this);
}

/*!
	SLOT which handles when the alarm push button is pressed.
 */
void ClockAlarmListItemPrototype::handleAlarmStatusChanged()
{
	int row = modelIndex().row();
	emit static_cast<ClockAlarmListItemPrototype*>(
			prototype())->alarmStatusHasChanged(row);
}

// End of file	--Don't remove this.
