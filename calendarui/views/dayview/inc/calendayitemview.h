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
* Description:  Day view control of calendar
*
*/

#ifndef CALENDAYITEMVIEW_H
#define CALENDAYITEMVIEW_H

//System includes
#include <hbabstractitemview.h>
#include <hbmenu.h>

//User includes

//Forward declarations
class CalenDayInfo;
class CalenDayContainer;
class MCalenServices;

class CalenDayItemView : public HbAbstractItemView
{
    Q_OBJECT

public:
    enum CalendarPanDirection {
        CalendarPanNotSet,
        CalendarPanVertical,
        CalendarPanHorizontal
    };

    enum CalendaRecycleDays {
        CalendarNoRecycle,
        CalendarBackwardRecycle,
        CalendarForwardRecycle
    };

    /**
     * Constructor.
     */
    CalenDayItemView(MCalenServices &services, HbModelIterator* iterator, QGraphicsItem *parent);

    /**
     * Destructor.
     */
    ~CalenDayItemView();

    /**
     * 
     */
    void scrollTo(const QModelIndex &index, HbAbstractItemView::ScrollHint hint = EnsureVisible);

public slots:

    /**
     * 
     */
    virtual void reset();
    
    /**
     * Handles signal that is emitted when reset() is called, before the model's
     * internal state (e.g. persistent model indexes) has been invalidated.
     */
    void modelAboutToBeReset();
    
    /**
     * Handles signal that is emitted when reset() is called, after the model's
     * internal state (e.g. persistent model indexes) has been invalidated.
     */
    void modelReset();

    /**
     * 
     */
    void scrollVertically(const QPointF &newPosition);
    
private slots:
    
    /**
     * This function is called when a touch down event is received within Abstract view item that is representing index.
     */
    void itemPressed(const QPointF &position);
    
    /**
     * This function is called when a touch release event is received within Abstract view item that is representing index.
     */
    void itemReleased(const QPointF &position);
    
    /**
     * This function is called when the item specified by index is activated by the user.
     */
    void itemActivated(const QPointF &position);
    
    /**
     * This function is called when long press event is received within Abstract view item viewItem. 
     * 
     * @param item The item.
     * @param coords is scene position where the long press event happened
     */
    void itemLongPressed(HbAbstractViewItem*, QPointF);
    
    /**
     * Slot which is called whenever the orientation of the device changes
     * @param orientation Current device orientation
     */
    void orientationChanged(Qt::Orientation orientation);
    
    /**
     * Opens currently selected model item.
     */
    void openSelectedItem();
    
    /**
     * Edits currently selected model item.
     */
    void editSelectedItem();
    
    /**
     * Deletes currently selected model item.
     */
    void deleteSelectedItem(); 

private:
    
    /**
     * Connects item view's slots.
     */
    void setupSlots();
    
    /**
     * Creates Context menu.
     */
    void setupContextMenu();
    
    /**
     * Sets context to currently selected model item and issues calendar command.
     * 
     * @param command Calendar command.
     */
    bool issueCommandOnSelectedItem( int command );
    
    /**
     * Clears current selection in Selection Model.
     */
    void clearSelectionInModel();

private:

    /**
     * Calendar services. Not owned.
     */
    MCalenServices  &mServices;
    
    /**
     * Item container.
     */
    CalenDayContainer* mContainer;
    
    /**
     * Day event info.
     * Own.
     */
    CalenDayInfo* mInfo;
    
    /**
     * context menu
     */
    HbMenu* mContextMenu;
};

#endif // CALENDAYITEMVIEW_H

// End of File
