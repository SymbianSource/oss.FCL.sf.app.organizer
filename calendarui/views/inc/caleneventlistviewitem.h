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
* Description:  CalenEventListViewItem class definition
*
*/

#ifndef __CALEN_EVENT_LIST_VIEW_ITEM_H__
#define __CALEN_EVENT_LIST_VIEW_ITEM_H__

// System includes
#include <hblistviewitem.h>

class CalenEventListViewItem : public HbListViewItem
{
    Q_OBJECT
    Q_PROPERTY(bool timeFormat12 READ timeFormat12Hr WRITE setTimeFormat12Hr)
public:
    CalenEventListViewItem(QGraphicsItem *parent = 0);
    ~CalenEventListViewItem();

    HbAbstractViewItem * createItem();
    void updateChildItems();
    bool timeFormat12Hr();
    void setTimeFormat12Hr(bool format);
private:
    bool timeFormat12;
};

#endif      // __CALEN_EVENT_LIST_VIEW_ITEM_H__
