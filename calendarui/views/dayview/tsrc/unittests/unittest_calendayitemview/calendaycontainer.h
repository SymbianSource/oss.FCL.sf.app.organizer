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
 *
 */

#ifndef CALENDAYCONTAINER_H
#define CALENDAYCONTAINER_H

//System includes
#include <QDateTime>

#include "../../../../../mw/hb/src/hbwidgets/itemviews/hbabstractitemcontainer_p.h"

//User includes

//Forward declarations
#include "calendayinfo.h"

class CalenDayContainer : public HbAbstractItemContainer
{
    Q_OBJECT

public:
    
    /**
     * Constructor. 
     */
    CalenDayContainer(QGraphicsItem *parent = 0)
    {
        Q_UNUSED(parent);
    }

    /**
     * Destructor.
     */
    ~CalenDayContainer(){}

    /**
     * 
     */
    void itemAdded (int index, HbAbstractViewItem *item, bool animate)
    {
        Q_UNUSED(index);
        Q_UNUSED(item);
        Q_UNUSED(animate);
    }
    
    /**
     * 
     */
    void itemRemoved (HbAbstractViewItem *item, bool animate)
    {
        Q_UNUSED(item);
        Q_UNUSED(animate);
    }
    
    /**
     * 
     */
    //void reset();
    
    /**
     * 
     */
    void viewResized (const QSizeF &size)
    {
        Q_UNUSED(size);
    }
    
    /**
     * Sets day's info structer to the container.
     * 
     * @param dayInfo Day info.
     */
    void setDayInfo( CalenDayInfo* dayInfo )
    {
        Q_UNUSED(dayInfo);
    }
    
public slots:

    /**
     * Slot handles layout switch.
     * @param orientation Current device orientation
     */
    void orientationChanged(Qt::Orientation orientation)
    {
        Q_UNUSED(orientation);
    }
    
protected:

    /**
     * 
     */
    HbAbstractViewItem * createDefaultPrototype() const{ return 0;}
    
    /**
     * 
     */
   // void setItemModelIndex(HbAbstractViewItem *item, const QModelIndex &index);
    
    /**
     * Updates geometry of a timed event.
     */
   // void updateTimedEventGeometry(HbAbstractViewItem *item, 
   //                               const QModelIndex &index);
    
    /**
     * Updates geometry of a all-day events	
     */
   // void updateAllDayEventGeometry(HbAbstractViewItem *item, 
   //                               const QModelIndex &index);
    
    /**
     * Structure with event layout values.
     * 
     * eventAreaX X value for event area start.
     * eventAreaWidth The width of event area.
     * eventMargin Margins between the multiple events.
     * slotHeight Half hour slot's height.
     * unitInPixels no. of pixels in 1un
     */

    /*struct LayoutValues {
					LayoutValues()
						:eventAreaX(0),
						 eventAreaWidth(0),
						 eventMargin(0),
						 slotHeight(0),
						 unitInPixels(0),
						 maxColumns(0)
						 {}
						
    	qreal eventAreaX;
    	qreal eventAreaWidth;
    	qreal eventMargin;
    	qreal slotHeight;
    	qreal unitInPixels;
    	int maxColumns;
    };
    */
    /**
     * Gets event layout values.
     * 
     * @param layoutValues structure to be filled with layout data
     */
    //void getTimedEventLayoutValues(LayoutValues& layoutValues);
    
    
    /**
     *  creates absorbers which prevent touching to small items
     *  (according to UI spec items smaller than ... are untouchable)
     */
    //void createTouchEventAbsorbers();
    
    /**
     * 
     */
    //TouchEventAbsorber* crateAbsorberBetweenSlots(int startSlot, int endSlot);
    
private:
    
    //bool mGeometryUpdated;

    //QDateTime mDateTime;
    
    /**
     * Day event info.
     * Not own.
     */
    //CalenDayInfo* mInfo;
    
   // LayoutValues mLayoutValues;
    //QList<TouchEventAbsorber*> mAbsorbers;
};



class TouchEventAbsorber : public HbWidget
    	{
	Q_OBJECT
    public:
    	TouchEventAbsorber(QGraphicsItem *parent=0);
    	~TouchEventAbsorber();
    	
    protected:
        void gestureEvent(QGestureEvent *event);
        
#ifdef _DEBUG
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
				QWidget *widget);
#endif
        
    	};

#endif // CALENDAYCONTAINER_H

// End of File
