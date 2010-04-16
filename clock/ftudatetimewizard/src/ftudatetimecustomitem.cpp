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
 * Description: Implementation for the FtuDateTimeCustomItem class
 *
 */

// System includes
#include <QObject>
#include <hblistwidget.h>
#include <hblistwidgetitem.h>
#include <hbpushbutton.h>
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>


// User includes
#include "ftudatetimecustomitem.h"

/*!
    \class FtuDateTimeCustomItem
    This object creates the Custom item
 */

/*!
    Constructor.
 */
FtuDateTimeCustomItem::FtuDateTimeCustomItem(QGraphicsItem *parent) :
HbDataFormViewItem(parent)
{
}

/*!
    Destructor.
 */
FtuDateTimeCustomItem::~FtuDateTimeCustomItem()
{
	// Nothing yet.
}

/*!
    Creates the custom item.
 */
HbAbstractViewItem* FtuDateTimeCustomItem::createItem()
{
	return new FtuDateTimeCustomItem(*this);
}

/*!
    Checks if model index can be set or not
 */
bool FtuDateTimeCustomItem::canSetModelIndex(const QModelIndex &index) const
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

/*!
    
 */
void FtuDateTimeCustomItem::restore()
{
    HbDataFormModelItem::DataItemType itemType =
            static_cast<HbDataFormModelItem::DataItemType>(
				modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
    if(itemType == (HbDataFormModelItem::CustomItemBase)) {
    	HbDataFormModel* model = static_cast<HbDataFormModel*>(
    			static_cast<HbAbstractViewItem*>(this)->itemView()->model());
    	HbDataFormModelItem* modlItem = model->itemFromIndex(modelIndex()); 
    	mPushButton->setProperty("text", modlItem->contentWidgetData("text"));
    }
}
/*!
    Creates and returns the custom item.
 */
HbWidget* FtuDateTimeCustomItem::createCustomWidget()
{
	/*HbPushButton* pushButton = new HbPushButton(this);
	return pushButton;*/
	mPushButton = new HbPushButton(this);
	return mPushButton;
}

// End of file  --Don't remove this.
