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
* Description:  CalenEventListViewItem class implementation
*
*/

// System includes
#include <hbextendedlocale.h>

// User includes
#include "caleneventlistviewitem.h"

CalenEventListViewItem::CalenEventListViewItem(QGraphicsItem *parent) :
HbListViewItem(parent)
{
	HbExtendedLocale locale = HbExtendedLocale::system();
    timeFormat12 = (HbExtendedLocale::Time12 == locale.timeStyle()) ? true : false;
}

CalenEventListViewItem::~CalenEventListViewItem()
{
    // No implementation yet
}

HbAbstractViewItem* CalenEventListViewItem::createItem()
{
    return new CalenEventListViewItem(*this);
}

void CalenEventListViewItem::updateChildItems()
{
    HbListViewItem::updateChildItems();
}

bool CalenEventListViewItem::timeFormat12Hr()
{
    return timeFormat12;
}

void CalenEventListViewItem::setTimeFormat12Hr(bool format)
{
    timeFormat12 = format;
}


// End of file
