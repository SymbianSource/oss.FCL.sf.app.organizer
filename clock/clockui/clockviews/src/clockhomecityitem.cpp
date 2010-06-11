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
* Definition file for class ClockHomeCityItem.
*
*/

// System Includes.
#include <QVariantMap>
#include <HbStyleLoader>
#include <HbTextItem>
#include <HbIconItem>
#include <HbFrameItem>
#include <HbFrameDrawer>

// User Includes.
#include "clockhomecityitem.h"

/*!
	\class ClockHomeCityItem

	This class creates all the primitives for ClockHomeCityItem item.
 */

/*!
	Constructor.

	\param parent Pointer to QGraphicsItem.
 */
ClockHomeCityItem::ClockHomeCityItem(QGraphicsItem *parent) :
HbWidget(parent)
{
	// Path for widgetml and css files.
	HbStyleLoader::registerFilePath(":/style/clockhomecityitem.css");
	HbStyleLoader::registerFilePath(":/style/clockhomecityitem.widgetml");
	HbStyleLoader::registerFilePath(":/style/clockhomecityitem_color.css");
	
	// Set the frame to the homecity widget.
	HbFrameItem *frame = new HbFrameItem();
	frame->frameDrawer().setFrameType(HbFrameDrawer::NinePieces);
	frame->frameDrawer().setFrameGraphicsName("qtg_fr_groupbox");
	setBackgroundItem(frame->graphicsItem(), -2);

}

/*!
 Destructor.
 */
ClockHomeCityItem::~ClockHomeCityItem()
{
	HbStyleLoader::unregisterFilePath(":/style/clockhomecityitem.css");
	HbStyleLoader::unregisterFilePath(":/style/clockhomecityitem.widgetml");
	HbStyleLoader::unregisterFilePath(":/style/clockhomecityitem_color.css");
}

/*!
	Sets the data for HomeCity items.
	To set textitem data use Qt::DisplayRole
	To set iconitem data use Qt::DecorationRole
	\param itemData contains the data for all the primitives.
 */
void ClockHomeCityItem::setHomeCityItemData(const QVariantMap &itemData)
{
	if (!itemData.isEmpty()) {
		QMapIterator<QString, QVariant> iter(itemData);
		while (iter.hasNext()) {
			iter.next();
			int value = iter.key().toInt();
			switch(value) {
				case Date:
					if (!mDateText) {
						mDateText = new HbTextItem(this);
						HbStyle::setItemName(mDateText, "dateText");
					}
					mDateText->setText(iter.value().toString());
					break;
				case City:
					if (!mCityText) {
						mCityText = new HbTextItem(this);
						HbStyle::setItemName(mCityText, "cityText");
						mCityText->setTextWrapping(Hb::TextNoWrap);
					}
					mCityText->setText(iter.value().toString());
					break;
				case Time:
					if (!mTimeText) {
						mTimeText = new HbTextItem(this);
						HbStyle::setItemName(mTimeText, "timeText");
						mTimeText->setTextWrapping(Hb::TextNoWrap);
					}
					mTimeText->setText(iter.value().toString());
					break;
				case DayNightIndicator:
					if (!mDayNightIcon) {
						mDayNightIcon = new HbIconItem(this);
						HbStyle::setItemName(mDayNightIcon, "dayNightIcon");
					}
					mDayNightIcon->setIconName(iter.value().toString());
					break;
				case Dst:
					if (!mDstIcon) {
						mDstIcon = new HbIconItem(this);
						HbStyle::setItemName(mDstIcon, "dstIcon");
					}
					mDstIcon->setIconName(iter.value().toString());
					break;
				default:
					break;
			}
		}
		repolish();
	}
}

// End of file	--Don't remove this.
