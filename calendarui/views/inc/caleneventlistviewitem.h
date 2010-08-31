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

public:
    /**
     * @brief Constructor
     * @param CalenAgendaViewWidget
     */
    CalenEventListViewItem(CalenAgendaViewWidget *parent = 0);
    
    /**
     * @brief Destructor
     */
    ~CalenEventListViewItem();

    /**
     * @brief creates the instance  of the class
     */
    HbAbstractViewItem * createItem();
    
    /**
     * @brief crestes the item of the list
     */
    void updateChildItems();
    
    /**
     * @brief check if all day icon is present
     */
    bool hasAllDayIcon();
    
    /**
     * @brief sets teh all day icon 
     */
    void setAllDayLeftIcon(bool icon = false);
    
private:  
    CalenAgendaViewWidget* mParent; 
    bool mAllDayIcon ;
};

#endif      // __CALEN_EVENT_LIST_VIEW_ITEM_H__
