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
#include <QGesture>
#include <QGraphicsLinearLayout>
#include <hbabstractviewitem.h>
#include <hbinstance.h>
#include <hbaction.h>

//User includes
#include "calendayitemview.h"
#include "calendayutils.h"
#include "calendaycontainer.h"
#include "calendayinfo.h"
#include "calendaymodel.h"
#include "calenservices.h"
#include "calencontext.h"
#include "calenagendautils.h"
#include "CalenUid.h"


/*!
   \brief Constructor
*/
CalenDayItemView::CalenDayItemView( MCalenServices &services, HbModelIterator *iterator, QGraphicsItem *parent ) 
: HbAbstractItemView(mContainer = new CalenDayContainer(), iterator, parent),
  mServices( services )
{
    mInfo = new CalenDayInfo( CalenDayInfo::ETwo );
    mContainer->setDayInfo( mInfo );
	
    //setup Selection Model
    QItemSelectionModel* selModel = new QItemSelectionModel(this->model());
    if ( selModel ) {
        setSelectionMode( HbAbstractItemView::SingleSelection );
        setSelectionModel( selModel );
    }
    
    setScrollDirections(Qt::Vertical);
    setScrollingStyle(HbScrollArea::PanWithFollowOn);
    setVerticalScrollBarPolicy(HbScrollArea::ScrollBarAlwaysOff);
    
    grabGesture(Qt::PanGesture);
    ungrabGesture(Qt::SwipeGesture);
    
    //TODO: KG: Animate item pressed?! HbAbstractItemView::TouchDown
    setEnabledAnimations( HbAbstractItemView::None );
    
    // Get the width of content and fix its size
    qreal contentWidth = CalenDayUtils::instance()->contentWidth();
    setMinimumWidth(contentWidth);
    setMaximumWidth(contentWidth);
      
    setupSlots();
    setupContextMenu();
}

/*!
   \brief Destructor
*/
CalenDayItemView::~CalenDayItemView()
{
    delete mInfo;
}

/*!
   \brief Scrolls to given index
*/
void CalenDayItemView::scrollTo(const QModelIndex &index, HbAbstractItemView::ScrollHint hint)
{
    HbAbstractItemView::scrollTo(index, hint);
}

/*!
   \brief Currently empty implementation.
*/
void CalenDayItemView::reset()
{

}


/*!
   \brief Handles signal that is emitted when reset() is called, before the model's
          internal state (e.g. persistent model indexes) has been invalidated.
*/
void CalenDayItemView::modelAboutToBeReset()
{
    
}

/*!
   \brief Handles signal that is emitted when reset() is called, before the model's
          internal state (e.g. persistent model indexes) has been invalidated.
*/
void CalenDayItemView::modelReset()
{
    if ( !mInfo ) {
        return;
    }
    //reset day info
    mInfo->Reset();

    CalenDayModel* model(0);
    model = static_cast<CalenDayModel*>(this->model());
    if ( !model ) {
        return;
    }
    
    mContainer->setDate(model->modelDate().date());
    int rowCount = model->rowCount( );
    SCalenApptInfo apptInfo; 
    for( int row = 0; row < rowCount; row++ ) {
        QModelIndex idx = model->index( row, 0 );
        QVariant variant = idx.data( CalenDayEntry );
        AgendaEntry entry = variant.value<AgendaEntry>();
        apptInfo.iIndex = idx;
        
        QDateTime start;
        QDateTime end;
        QDateTime currDate( model->modelDate() );
        CalenDayUtils::instance()->getEventValidStartEndTime( start, end, 
                                                              entry, currDate );

        apptInfo.iStartTime = start;
        apptInfo.iEndTime = end;
        
        TCalenInstanceId id = TCalenInstanceId::nullInstanceId();//index.row() - temporary ID
        id.mEntryLocalUid = row;
        id.mInstanceTime = apptInfo.iStartTime; 
        apptInfo.iId = id;
        apptInfo.iColor = 0xffff;
        
        if ( entry.isTimedEntry() && !CalenAgendaUtils::isAlldayEvent(entry))
        	{
            apptInfo.iAllDay = false;
            mInfo->InsertTimedEvent( apptInfo );
        	}
        else if(CalenAgendaUtils::isAlldayEvent(entry)) //all-day event
        	{
        	apptInfo.iAllDay = true;
        	mInfo->InsertAlldayEvent( apptInfo );
        	}
    }
    
    HbAbstractItemView::reset();
}

/*!
   \brief Scrolls view vertically to the given position.
   
   \param newPosition position to scroll to.
*/
void CalenDayItemView::scrollVertically( const QPointF &newPosition )
{
    QPointF currentPos = contentWidget()->pos();
    if (abs(newPosition.y()) != abs(currentPos.y())) {
        currentPos.setY(newPosition.y());
        scrollContentsTo(currentPos, 0);
    }
}

/*!
   \brief This function is called when a touch press event is received 
          within Abstract view item that is representing index.
          Currently it does nothing.
*/
void CalenDayItemView::itemPressed( const QPointF &position )
{
    Q_UNUSED( position )
}

/*!
   \brief This function is called when a touch release event is received 
          within Abstract view item that is representing index.
          Currently it does nothing.
*/
void CalenDayItemView::itemReleased( const QPointF &position )
{
    Q_UNUSED( position )
}

/*!
   \brief This function is called when the item specified by index is activated by the user.
   
   \param position Not used.
*/
void CalenDayItemView::itemActivated( const QPointF &position )
{
    Q_UNUSED( position )
    HbAbstractViewItem *item = qobject_cast<HbAbstractViewItem *>(sender());
    QModelIndex index = item->modelIndex();
    QItemSelectionModel* selModel( selectionModel() );
    if ( index.isValid() && selModel) {
        selModel->select( index, QItemSelectionModel::Current | QItemSelectionModel::Select );
        openSelectedItem();
    }
}

/*!
   \brief This function is called when long press event is received within Abstract view item viewItem.

   \param item Pointer to pressed item.
   \param coord Position where item was pressed.
*/
void CalenDayItemView::itemLongPressed(HbAbstractViewItem* item, QPointF coords)
{
    QModelIndex index = item->modelIndex();
    QItemSelectionModel* selModel( selectionModel() );
    if ( index.isValid() && selModel && mContextMenu) {
        selModel->select( index, QItemSelectionModel::Current | QItemSelectionModel::Select );
        mContextMenu->setPreferredPos( coords, HbPopup::BottomRightCorner );
        mContextMenu->open();
    }
}

/*!
   \brief Slot which is called whenever the orientation of the device changes.

   \param orientation New orientation.
*/
void CalenDayItemView::orientationChanged( Qt::Orientation orientation )
{
    // Update the width of screen
    qreal contentWidth = CalenDayUtils::instance()->contentWidth();
    setMinimumWidth(contentWidth);
    setMaximumWidth(contentWidth);
    if( mContainer ) {
        mContainer->orientationChanged( orientation );
    }
}

/*!
   \brief Opens currently selected model item.
*/
void CalenDayItemView::openSelectedItem()
{
    issueCommandOnSelectedItem( ECalenEventView );
}

/*!
   \brief Edits currently selected model item.
*/
void CalenDayItemView::editSelectedItem()
{
    issueCommandOnSelectedItem( ECalenEditCurrentEntry );
}

/*!
   \brief Deletes currently selected model item.
*/
void CalenDayItemView::deleteSelectedItem()
{
    issueCommandOnSelectedItem( ECalenDeleteCurrentEntry );
}

/*!
   \brief Connects item view's slots.
*/
void CalenDayItemView::setupSlots()
{
    // Connect to main window's orientationChanged signal to handle orientation
    // switching
    QObject::connect(CalenDayUtils::instance()->mainWindow(), 
        SIGNAL(orientationChanged(Qt::Orientation)), this,
        SLOT(orientationChanged(Qt::Orientation)));

    // Connect with model signals
    if (this->model()) {
        QObject::connect(this->model(), SIGNAL(modelAboutToBeReset()), this,
            SLOT(modelAboutToBeReset()));
        QObject::connect(this->model(), SIGNAL(modelReset()), this,
            SLOT(modelReset()));
    }
    if (mContainer) {
        // Connect with base class to enable handling item press by model index
        QObject::connect(mContainer, SIGNAL(itemCreated(HbAbstractViewItem *)),
            this, SLOT(itemCreated(HbAbstractViewItem*)));
    }
    QObject::connect(this, SIGNAL(pressed(const QPointF&)), this,
        SLOT(itemPressed(const QPointF&)));
    QObject::connect(this, SIGNAL(released(const QPointF&)), this,
        SLOT(itemReleased(const QPointF&)));
    QObject::connect(this, SIGNAL(activated(const QPointF&)), this,
        SLOT(const QPointF&));
    QObject::connect(this, SIGNAL(longPressed(HbAbstractViewItem*, QPointF)),
        this, SLOT(itemLongPressed(HbAbstractViewItem*, QPointF)));
}

/*!
   \brief Creates context menu.
*/
void CalenDayItemView::setupContextMenu()
{
    mContextMenu = new HbMenu();
    HbAction *openAction = mContextMenu->addAction(hbTrId("txt_common_menu_open"));
    if (openAction) {
        connect(openAction, SIGNAL(triggered()), this, SLOT(openSelectedItem()));
    }
    HbAction *editAction = mContextMenu->addAction(hbTrId("txt_common_menu_edit"));
    if (editAction) {
        connect(editAction, SIGNAL(triggered()), this, SLOT(editSelectedItem()));
    }
    HbAction *deleteAction = mContextMenu->addAction(hbTrId("txt_common_menu_delete"));
    if (deleteAction) {
        connect(deleteAction, SIGNAL(triggered()), this,
            SLOT(deleteSelectedItem()));
    }
}

/*!
   \brief Sets context to currently selected model item and issues calendar command.
   
   \param command A command to be issued.
*/
bool CalenDayItemView::issueCommandOnSelectedItem( int command )
{
    bool retVal(false);
    QItemSelectionModel* selModel(selectionModel());
    if (!selModel){
        return retVal;
    }
    QModelIndex index( selModel->currentIndex() );
    if( index.isValid() ) {
        retVal = true;
        QVariant variant = index.data( CalenDayEntry );
        AgendaEntry entry = variant.value<AgendaEntry>();
        QDateTime start;
        QDateTime end;
        CalenDayModel* model(0);
        model = static_cast<CalenDayModel*>(this->model());
        QDateTime currDate( model->modelDate() );
        CalenDayUtils::instance()->getEventValidStartEndTime( start, end, 
                                                              entry, currDate );
        TRAPD(error,
                mServices.Context().setFocusDateAndTimeAndInstance(start,
                         TCalenInstanceId::create(entry));
                mServices.IssueCommandL( command );
              );
        if( error ) {
            retVal = false;
        }
        clearSelectionInModel();
    }
    return retVal;
}

/*!
   \brief Clears current selection in Selection Model.
*/
void CalenDayItemView::clearSelectionInModel()
{
    QItemSelectionModel* selModel(selectionModel());
    if (selModel) {
        selModel->clear();
    }
}

// End of File
