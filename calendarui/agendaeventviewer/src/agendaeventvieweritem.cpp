/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Definition of AgendaEventViewerItem class
*
*/

#include <QtGui>
#include <QVariant>

// System Includes.
#include <hbstyleloader.h>
#include <hbstyle.h>
#include <hbtextitem.h>
#include <hbiconitem.h>

// User Includes.
#include "agendaeventvieweritem.h"

/*!
 \class AgendaEventViewerItem

 This class creates all the primitives for AgendaEventViewer item.
 */

/*!
 Constructor.

 \param parent Pointer to QGraphicsItem.
 */
AgendaEventViewerItem::AgendaEventViewerItem(QGraphicsItem *parent) :
	HbWidget(parent), mPrimaryText(NULL), mSecondaryText(NULL),
	        mPrimaryIcon(NULL), mSecondaryIcon(NULL),mPrimaryRightIcon(NULL),
	        mPrimaryLeftIcon(NULL)
{

	// Path for widgetml and css files.
	HbStyleLoader::registerFilePath(":/");

}

/*!
 Destructor.
 */
AgendaEventViewerItem::~AgendaEventViewerItem()
{
	HbStyleLoader::unregisterFilePath(":/");

}

/*!
 Sets the data for EventViewer items
 To set textitem data use Qt::DisplayRole
 To set iconitem data use Qt::DecorationRole
 \param itemData string list of itemdata
 \param role qt role of text and icon item
 */
void AgendaEventViewerItem::setEventViewerItemData(const QStringList &itemData,
                                                   int role)
{
	if (!itemData.isEmpty()) {
		QString firstItemData(QString::null);
		QString secondItemData(QString::null);
		QString thirdItemData(QString::null);
        if (itemData.count() == 2) {
            firstItemData = itemData.at(0);
            secondItemData = itemData.at(1);
        }
        else if (itemData.count() == 3) {
			firstItemData = itemData.at(0);
			secondItemData = itemData.at(1);
			thirdItemData = itemData.at(2);
		} else {
			firstItemData = itemData.at(0);
		}

		if (role == Qt::DisplayRole) {
			if (!firstItemData.isEmpty()) {
				if (!mPrimaryText) {
					mPrimaryText = new HbTextItem(this);
					HbStyle::setItemName(mPrimaryText, "primaryTextItem");
					mPrimaryText->setElideMode(Qt::ElideNone);
				}
				mPrimaryText->setText(firstItemData);
			} else {
				if (mPrimaryText) {
					delete mPrimaryText;
					mPrimaryText = NULL;
				}
			}

			if (!mSecondaryText) {
				mSecondaryText = new HbTextItem(this);
				HbStyle::setItemName(mSecondaryText, "secondaryTextItem");
				mSecondaryText->setTextWrapping(Hb::TextWordWrap);
			}
			
			if (!secondItemData.isEmpty()) {
				
				mSecondaryText->setText(secondItemData);
			} else {
				mSecondaryText->setText("");
				}
		} else {
			if (role == Qt::DecorationRole) {
               if (!firstItemData.isEmpty()) {
                   if (!mPrimaryLeftIcon) {
                   mPrimaryLeftIcon = new HbIconItem(this);
                   }
                   HbStyle::setItemName(mPrimaryLeftIcon, "primaryLeftIconItem");
                   mPrimaryLeftIcon->setVisible(true);
                   mPrimaryLeftIcon->setIconName(firstItemData);

               } else {
                   if (mPrimaryLeftIcon) {
                       HbStyle::setItemName(mPrimaryLeftIcon,"");
                       mPrimaryLeftIcon->setVisible(false); 
                   }
                   
               }
				if (!secondItemData.isEmpty()) {
					if (!mPrimaryRightIcon) {
					mPrimaryRightIcon = new HbIconItem(this);
					}
					HbStyle::setItemName(mPrimaryRightIcon, "primaryRightIconItem");
					mPrimaryRightIcon->setVisible(true);
					mPrimaryRightIcon->setIconName(secondItemData);

				} else {
					if (mPrimaryRightIcon) {
						HbStyle::setItemName(mPrimaryRightIcon,"");
						mPrimaryRightIcon->setVisible(false); 
					}
					
				}
				if (!thirdItemData.isEmpty()) {
					if (!mSecondaryIcon) {
						mSecondaryIcon = new HbIconItem(this);
						HbStyle::setItemName(mSecondaryIcon, 
						                     "secondaryIconItem");
					}
					mSecondaryIcon->setIconName(thirdItemData);

				} else {
					if (mSecondaryIcon) {
						delete mSecondaryIcon;
						mSecondaryIcon = NULL;
					}

				}

			}
		}
	}
	repolish();
}

// End of file	--Don't remove this.
