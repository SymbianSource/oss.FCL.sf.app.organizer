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

//System includes
#include <QTime>
#include <QGraphicsLinearLayout>
#include <QGesture>

#ifdef _DEBUG
	#include <QPainter>
#endif

#include <hbabstractitemview.h>
#include <hbtextitem.h>
#include <hbmodeliterator.h>
#include <hbinstance.h>

//User includes
#include "calendaycontainer.h"
#include "calendayutils.h"
#include "calendayeventspane.h"
#include "calendayitem.h"
#include "calendaymodel.h"
#include "calendayinfo.h"
#include "calendayview.h"

// -----------------------------------------------------------------------------
// CalenDayContainer()
// Constructor
// -----------------------------------------------------------------------------
//
CalenDayContainer::CalenDayContainer(QGraphicsItem *parent) :
    HbAbstractItemContainer(parent), mGeometryUpdated(false), mInfo(0)
{
    getTimedEventLayoutValues(mLayoutValues);
    
    // Get the height of element
    qreal paneHeight = CalenDayUtils::instance()->hourElementHeight();
    
    QGraphicsLinearLayout* timeLinesLayout = new QGraphicsLinearLayout(
        Qt::Vertical, this);
    for (int i = 0; i < 24; i++) {
        CalenDayEventsPane* element = new CalenDayEventsPane(this);
        element->setPreferredHeight(paneHeight);
        element->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		// Draw top line at midnight
        if (i == 0) {
            element->drawTopLine(true);
        }
        timeLinesLayout->addItem(element);
    }
    timeLinesLayout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
    timeLinesLayout->setSpacing(0.0);

    setLayout(timeLinesLayout);
}

// -----------------------------------------------------------------------------
// ~CalenDayContainer()
// Destructor
// -----------------------------------------------------------------------------
//
CalenDayContainer::~CalenDayContainer()
{
}

// -----------------------------------------------------------------------------
// itemAdded()
// 
// -----------------------------------------------------------------------------
//
void CalenDayContainer::itemAdded( int index, HbAbstractViewItem *item, 
                                     bool animate )
{
    Q_UNUSED( index )
    Q_UNUSED( item )
    Q_UNUSED( animate )
}

// -----------------------------------------------------------------------------
// reset()
// 
// -----------------------------------------------------------------------------
//
void CalenDayContainer::reset()
{
	// remove absorbers if exist
	if (mAbsorbers.count())
		{
		qDeleteAll(mAbsorbers);
		mAbsorbers.clear();
		}
	
	// shrink event area when all-day events available after reset
	getTimedEventLayoutValues(mLayoutValues);
	
    // position need to be maintained while changing model
    QPointF position(pos());
    HbAbstractItemContainer::reset();
    setPos( position );
}

// -----------------------------------------------------------------------------
// itemRemoved()
// 
// -----------------------------------------------------------------------------
//
void CalenDayContainer::itemRemoved( HbAbstractViewItem *item, bool animate )
{
    Q_UNUSED( item )
    Q_UNUSED( animate )
}

// -----------------------------------------------------------------------------
// viewResized()
// 
// -----------------------------------------------------------------------------
//
void CalenDayContainer::viewResized (const QSizeF &size)
{
    resize(size);
    if (!mGeometryUpdated) {
        mGeometryUpdated = true;
        updateGeometry();
    }
}

// -----------------------------------------------------------------------------
// createDefaultPrototype()
// 
// -----------------------------------------------------------------------------
//
HbAbstractViewItem * CalenDayContainer::createDefaultPrototype() const
{
    CalenDayItem *calendarViewItem = new CalenDayItem;
    return calendarViewItem;
}

// -----------------------------------------------------------------------------
// setItemModelIndex()
// 
// -----------------------------------------------------------------------------
//
void CalenDayContainer::setItemModelIndex(HbAbstractViewItem *item, 
                                            const QModelIndex &index)
{
    QVariant variant = index.data( CalenDayEntry );
    AgendaEntry entry = variant.value<AgendaEntry>();
    
    if (entry.isTimedEntry()) {
        updateTimedEventGeometry( item, index );
        item->setParentItem(this);
    	}
    else if( entry.type() == AgendaEntry::TypeEvent ){
        updateAllDayEventGeometry( item, index );
        item->setParentItem(this);
    	} 
    else {
    	item->setVisible(false);
    }
    
    // last item
    if (index.row() == index.model()->rowCount() - 1) {
    	createTouchEventAbsorbers();
    }

    HbAbstractItemContainer::setItemModelIndex(item, index);
}

// -----------------------------------------------------------------------------
// updateTimedEventGeometry()
// Updates geometry of a timed event.
// -----------------------------------------------------------------------------
//
void CalenDayContainer::updateTimedEventGeometry(HbAbstractViewItem *item, 
                                                   const QModelIndex &index)
{
//safety check
    if ( !mInfo ) {
        return;
    }
    
    QVariant variant = index.data( CalenDayEntry );
    AgendaEntry entry = variant.value<AgendaEntry>();
    
//1. get 'virtual' event position from DayInfo
//TODO: k.g.: Day Info should store model index instead of keeping redundant data
    SCalenApptInfo apptInfo;
    apptInfo.iIndex = index;
    
    QDateTime start;
    QDateTime end;
    QDateTime currentDate;
    currentDate = static_cast<const CalenDayModel*>(index.model())->modelDate();
    CalenDayUtils::instance()->getEventValidStartEndTime( start, end, entry,
                                                          currentDate );
    apptInfo.iStartTime = start;
    apptInfo.iEndTime = end;
    
    TCalenInstanceId id = TCalenInstanceId::nullInstanceId();
    id.mEntryLocalUid = index.row(); //index.row() - temporary ID
    id.mInstanceTime = apptInfo.iStartTime; 
    apptInfo.iId = id;
    apptInfo.iAllDay = 0;
    apptInfo.iColor = 0xffff;
    
    int startSlot, endSlot, columnIdx, columns;
    mInfo->GetLocation( apptInfo, startSlot, endSlot, columnIdx, columns );
    

//2. set timed event's geometry
    qreal eventStartX(mLayoutValues.eventAreaX );
    qreal eventStartY(0.0);
    qreal eventWidth(mLayoutValues.eventAreaWidth);
    qreal eventHeight(0.0);
       
    //event's startY/height
    eventStartY = startSlot * mLayoutValues.slotHeight;
    eventHeight = (endSlot - startSlot) * mLayoutValues.slotHeight;

    //event's startX/width
    eventWidth /= columns;
    
    //In case when eventWidth will be smaller then 3.0un we need to 
    //make spacings between events smaller.
    //Check whether it's possible to shring them so the bubbles 
    //width can stay at 3.0un (time stripe + frame margins).
    qreal minWidth = 3.0 * mLayoutValues.unitInPixels; 
    if(eventWidth - mLayoutValues.eventMargin < minWidth){
        
        //Calculate new margin value
        //from totalMarginSpace we need to subtract 
        //mLayoutValues.eventMargin because first margin is always 1.5un
        qreal totalMarginSpace =  mLayoutValues.eventAreaWidth - minWidth * columns - mLayoutValues.eventMargin;
        qreal newMarginValue = totalMarginSpace / (columns - 1);
        
        //check if we managed to pack all the events into space we have
        if(newMarginValue > 0){
            
            eventWidth = minWidth;
        }
        else{
            //there's not enough space
            //new minWidth it's 1.5un (time stripe only)
            minWidth = 1.5 * mLayoutValues.unitInPixels; 
            totalMarginSpace =  mLayoutValues.eventAreaWidth - minWidth * columns - mLayoutValues.eventMargin;
            newMarginValue = totalMarginSpace / (columns - 1);
            eventWidth = minWidth;
        }
        
        //First column margin should be always 1.5un (mLayoutValues.eventMargin)
        eventStartX += columnIdx * (eventWidth + newMarginValue) + mLayoutValues.eventMargin;
    }
    else{
        //add margins between the event
        eventStartX += columnIdx * eventWidth + mLayoutValues.eventMargin;
        eventWidth -= mLayoutValues.eventMargin;
    }
    
    QRectF eventGeometry( eventStartX, eventStartY, eventWidth, eventHeight );
    item->setGeometry(eventGeometry);}


// -----------------------------------------------------------------------------
// updateAllDayEventGeometry()
// Updates geometry of a timed event.
// -----------------------------------------------------------------------------
//
void CalenDayContainer::updateAllDayEventGeometry(HbAbstractViewItem *item, 
                                                   const QModelIndex &index)
{
	//safety check
	if ( !mInfo ) {
	      return;
	}
	    
	QVariant variant = index.data( CalenDayEntry );
	AgendaEntry entry = variant.value<AgendaEntry>();
	    
	//1. get 'virtual' event position from DayInfo
	//TODO: k.g.: Day Info should store model index instead of keeping redundant data    
	SCalenApptInfo apptInfo;
	apptInfo.iIndex = index;
	
	
	QDateTime start;
    QDateTime end;
    QDateTime currentDate;
    currentDate = static_cast<const CalenDayModel*>(index.model())->modelDate();
    CalenDayUtils::instance()->getEventValidStartEndTime( start, end, entry,
                                                          currentDate );
    apptInfo.iStartTime = start;
    apptInfo.iEndTime = end;
	
	TCalenInstanceId id = TCalenInstanceId::nullInstanceId();
	id.mEntryLocalUid = index.row(); //index.row() - temporary ID
	id.mInstanceTime = apptInfo.iStartTime; 
	apptInfo.iId = id;
	apptInfo.iAllDay = true;
	apptInfo.iColor = 0xffff;
	
	int startSlot, endSlot, columnIdx, columns;
	mInfo->GetLocation( apptInfo, startSlot, endSlot, columnIdx, columns );
	
	//2. set timed event's geometry
	qreal eventStartX(0.0);
	qreal eventStartY(0.0);
	qreal eventWidth(mLayoutValues.eventAreaX);
	qreal eventHeight = (endSlot - startSlot) * mLayoutValues.slotHeight;

	
	//event's startX/width
	if ( columns > 1 ) {
		eventWidth /= columns;
		eventStartX += columnIdx * eventWidth + mLayoutValues.eventMargin;
		//add margins between the event
		eventWidth -= mLayoutValues.eventMargin;
	} else {
		eventStartX += mLayoutValues.eventMargin;
		eventWidth -= mLayoutValues.eventMargin;
	}
	
	QRectF eventGeometry( eventStartX, eventStartY, eventWidth, eventHeight );
	item->setGeometry(eventGeometry);
	    
}


// -----------------------------------------------------------------------------
// movingBackwards()
// 
// -----------------------------------------------------------------------------
//
void CalenDayContainer::getTimedEventLayoutValues(LayoutValues& layoutValues)
{
    // get the width of content area
    qreal contentWidth = CalenDayUtils::instance()->contentWidth();
//1.time column width -> eventAreaX[out]
    HbStyle style;
    HbDeviceProfile deviceProfile;
    layoutValues.unitInPixels = deviceProfile.unitValue();
    
    if ( mInfo && mInfo->AlldayCount())
    	{ // 9.5 -> all-day area width 
    	layoutValues.eventAreaX = 9.5 * layoutValues.unitInPixels;
    	}
    else
    	{
    	layoutValues.eventAreaX = 0;
    	}
    
//2. event area width -> eventAreaWidth[out]
    qreal emptyRightColumnWidth(0.0);
    emptyRightColumnWidth = 6.0 * layoutValues.unitInPixels; //pix (according to UI spec)
    layoutValues.eventAreaWidth = contentWidth - emptyRightColumnWidth - layoutValues.eventAreaX ;
//3. margins between the overlapping events -> eventMargin[out]
    layoutValues.eventMargin = 1.5 * layoutValues.unitInPixels;
//4. half-hour slot'h height -> slotHeight[out]
    //curent slot height corresponds to half an hour
    layoutValues.slotHeight = 
        CalenDayUtils::instance()->hourElementHeight() / 2;
    
    // 8.2 un (min. touchable event) from layout guide
    layoutValues.maxColumns = layoutValues.eventAreaWidth / (8.2 * layoutValues.unitInPixels);  
}

// -----------------------------------------------------------------------------
// setDayInfo()
// Sets day's info structer to the container.
// -----------------------------------------------------------------------------
//
void CalenDayContainer::setDayInfo( CalenDayInfo* dayInfo )
{
    mInfo = dayInfo;
}

// -----------------------------------------------------------------------------
// orientationChanged()
// Slot handles layout switch.
// -----------------------------------------------------------------------------
//
void CalenDayContainer::orientationChanged(Qt::Orientation orientation)
{
	getTimedEventLayoutValues(mLayoutValues);
	
    Q_UNUSED( orientation )
    QList<HbAbstractViewItem *> items = this->items();
    int count(items.count());
    for (int i = 0; i < count; i++) {
        QModelIndex modelIndex = items[i]->modelIndex();
        if (modelIndex.isValid()) {
            QVariant variant = modelIndex.data(CalenDayEntry);
            AgendaEntry entry = variant.value<AgendaEntry> ();
            if (entry.isTimedEntry()) {
                updateTimedEventGeometry(items[i], modelIndex);
            }
        }
    }
    
    createTouchEventAbsorbers();
}

// -----------------------------------------------------------------------------
// createTouchEventAbsorbers()
// Creates absorbers which prevent touching to small items
// -----------------------------------------------------------------------------
//
void CalenDayContainer::createTouchEventAbsorbers()
{
	// remove absorbers if exist
	if (mAbsorbers.count())
		{
		qDeleteAll(mAbsorbers);
		mAbsorbers.clear();
		}
	
	const QList<CalenTimeRegion>& regionList = mInfo->RegionList();
	
	for(int i=0; i < regionList.count(); i++)
		{
		if(regionList[i].iColumns.count() > mLayoutValues.maxColumns )
			{
			TouchEventAbsorber* absorber = 
				crateAbsorberBetweenSlots(regionList[i].iStartSlot, regionList[i].iEndSlot);
			
			mAbsorbers.append(absorber);
			}
		}
	
}

// -----------------------------------------------------------------------------
// crateAbsorberBetweenSlots()
// Creates single absorber in given location
// -----------------------------------------------------------------------------
//
TouchEventAbsorber *CalenDayContainer::crateAbsorberBetweenSlots
												(int startSlot, int endSlot)
{
    TouchEventAbsorber *absorber = new TouchEventAbsorber(this);
    absorber->setZValue(1000);
    absorber->setVisible(true);
    
    absorber->setGeometry( mLayoutValues.eventAreaX,			// x
			startSlot * mLayoutValues.slotHeight,				// y
			mLayoutValues.eventAreaWidth,						// w
			(endSlot-startSlot) * mLayoutValues.slotHeight ); 	// h
    
    return absorber;
}


// -----------------------------------------------------------------------------
// TouchEventAbsorber::gestureEvent()
// Handles tap event on overlapping area (currently it leads to Agenda View - 
// as described in UI spec)
// -----------------------------------------------------------------------------
//
void TouchEventAbsorber::gestureEvent(QGestureEvent *event)
{
    QTapGesture *tapGesture = qobject_cast<QTapGesture*> (event->gesture(
        Qt::TapGesture));
    
    if (tapGesture && tapGesture->state() == Qt::GestureFinished)
    	{
    	CalenDayView* dayView = static_cast<CalenDayView*>
                (CalenDayUtils::instance()->mainWindow()->currentView());
    	
    	dayView->changeView(ECalenAgendaView);
    	}
}

// -----------------------------------------------------------------------------
// TouchEventAbsorber()
// default ctor
// -----------------------------------------------------------------------------
//
TouchEventAbsorber::TouchEventAbsorber(QGraphicsItem *parent) : HbWidget(parent)
{
#ifdef _DEBUG
    setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif	
    grabGesture(Qt::TapGesture);    	    
}

// -----------------------------------------------------------------------------
// TouchEventAbsorber()
// default dtor
// -----------------------------------------------------------------------------
//
TouchEventAbsorber::~TouchEventAbsorber()
{
	
}

// -----------------------------------------------------------------------------
// TouchEventAbsorber::paint()
// used for debugging purposes to see absorbers areas
// -----------------------------------------------------------------------------
//
#ifdef _DEBUG
void TouchEventAbsorber::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
								QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)
	
	painter->save();
	QPen pen;
	pen.setWidth(2);
	pen.setColor(Qt::red);
	painter->setPen(pen);
	painter->drawRect(boundingRect());
	painter->restore();
}
#endif
// End of File
