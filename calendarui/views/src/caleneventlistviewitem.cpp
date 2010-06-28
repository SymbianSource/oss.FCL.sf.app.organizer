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

// End of file
