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
* Implementation for the ClockAlarmCustomItem class.
*
*/

// System includes.
#include <HbPushButton>
#include <HbDataFormModelItem>
#include <HbAbstractItemView>
#include <HbDataFormModel>

// User includes.
#include "clockalarmcustomitem.h"

/*!
	\class ClockAlarmCustomItem

	This class is responsible for creating customitem for Alarm Time field in
	Alarmeditor.
 */

/*!
	Default Constructor.

	\param parent The parent.
 */
ClockAlarmCustomItem::ClockAlarmCustomItem(QGraphicsItem *parent)
:HbDataFormViewItem(parent)
{
	// Nothing yet.
}

/*!
	Destructor.
 */
ClockAlarmCustomItem::~ClockAlarmCustomItem()
{
	// Nothing yet.
}

/*!
	Creates customitem.

	\sa
 */
HbAbstractViewItem* ClockAlarmCustomItem::createItem()
{
	return new ClockAlarmCustomItem(*this);
}

/*!
	
	\sa
 */
bool ClockAlarmCustomItem::canSetModelIndex(const QModelIndex &index) const
{
	HbDataFormModelItem::DataItemType itemType = 
			static_cast<HbDataFormModelItem::DataItemType>(
			index.data(HbDataFormModelItem::ItemTypeRole).toInt());

	if(itemType == (HbDataFormModelItem::CustomItemBase)) {
		return true;
	} else {
		return false;
	}

}

void ClockAlarmCustomItem::restore()
{
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType>(
			modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
	if(itemType == (HbDataFormModelItem::CustomItemBase)) {

		HbDataFormModel* model =
				static_cast<HbDataFormModel*>(
				static_cast<HbAbstractViewItem*>(this)->itemView()->model());
		HbDataFormModelItem* modlItem = model->itemFromIndex(modelIndex()); 
		mCustomButton->setProperty("text", modlItem->contentWidgetData("text"));
	}

}

/*!
	Creates the custom widget

	\sa
 */
HbWidget* ClockAlarmCustomItem::createCustomWidget()
{
	mCustomButton = new HbPushButton(this);
	return mCustomButton;
}

// End of file	--Don't remove this.
