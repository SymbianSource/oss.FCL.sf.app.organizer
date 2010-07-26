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
#include "calenagendaviewwidget.h"

class CalenEventListViewItem : public HbListViewItem
{
    Q_OBJECT
    Q_PROPERTY(bool leftAllDayIcon READ hasAllDayIcon WRITE setAllDayLeftIcon)
    Q_PROPERTY(bool repeatingIcon READ hasRepeatingIcon WRITE setRepeatingIcon) 
    Q_PROPERTY(bool alarmIcon READ hasAlarmIcon WRITE setAlarmIcon)
    Q_PROPERTY(bool exceptionIcon READ hasExceptionIcon WRITE setExceptionIcon)
public:
    CalenEventListViewItem(CalenAgendaViewWidget *parent = 0);
    ~CalenEventListViewItem();

    HbAbstractViewItem * createItem();
    void updateChildItems();
    bool hasAllDayIcon();
    void setAllDayLeftIcon(bool icon = false);
    bool hasRepeatingIcon();
    void setRepeatingIcon(bool icon = false);
    bool hasAlarmIcon();
    void setAlarmIcon(bool icon = false);
    bool hasExceptionIcon();
    void setExceptionIcon(bool icon = false);
private:  
    
    bool mAllDayIcon ;
    bool mExceptionIcon ;
    bool mAlarmIcon ;
    bool mRepeatingIcon ;
    CalenAgendaViewWidget* mParent; 
};

#endif      // __CALEN_EVENT_LIST_VIEW_ITEM_H__
