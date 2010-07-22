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
 * Description:  CalenDayContentWidget implementation.
 *
 */

// System includes
#include <QGraphicsLinearLayout>
#include <hbmodeliterator.h>

// User includes
#include "calendaymodelmanager.h"
#include "calendaycontentwidget.h"
#include "calendayitemview.h"
#include "calendaycontentscrollarea.h"

/*!
 \class CalenDayContentWidget
 \brief Content widget is a container class for horizontal layouted widgets.
 */

/*!
 \brief Constructor
 
 \param parent The parent of central widget
 */
CalenDayContentWidget::CalenDayContentWidget(
    CalenDayModelManager &modelManager,
    QGraphicsItem *parent) :
    HbWidget(parent), mLayout(NULL), mModelManager(modelManager)
{
    mWidgets.clear();
    initializeViews(parent);
}

/*!
 \brief Destructor
 */
CalenDayContentWidget::~CalenDayContentWidget()
{
    // Just clear widget list - parent object will destroy its children
    mWidgets.clear();
}

/*!
 \brief Adds the widget to central widget and layouts it horizontally.
 
 Ownership of item is passed to CalenDayContentWidget.
 
 \param item Widget to be added
 \param where Place where widget should be added (as first/last item)
 */
void CalenDayContentWidget::add(HbWidget* item, CalenWidgetPosition where)
{
    // Create linear, horizontal layout if not exist
    if (!mLayout) {
        mLayout = new QGraphicsLinearLayout(Qt::Horizontal, NULL);
        mLayout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
        mLayout->setSpacing(0.0);
    }

    switch (where) {
        case ECalenFirstWidget: {
            mLayout->insertItem(0, item);
            mWidgets.insert(0, item);
            break;
        }
        case ECalenLastWidget:
        default: {
            mLayout->addItem(item);
            mWidgets.append(item);
        }
    }

    // If layout has no parent - apply it to central widget
    if (mLayout && !mLayout->parentLayoutItem()) {
        setLayout(mLayout);
    }
}

/*!
 \brief Takes the widget and removes it from central widget.
 
 Ownership of item is passed to caller.
 
 \param which Indicates which widget should be taken (first/last)
 \return Pointer to HbWidget or NULL if widget was not found
 */
HbWidget* CalenDayContentWidget::take(CalenWidgetPosition which)
{
    Q_ASSERT(mLayout);

    HbWidget* itemToTake = NULL;
    switch (which) {
        case ECalenFirstWidget: {
            itemToTake = mWidgets.takeFirst();
            break;
        }
        case ECalenLastWidget:
        default: {
            itemToTake = mWidgets.takeLast();
        }
    }

    if (itemToTake) {
        mLayout->removeItem(itemToTake);
    }

    return itemToTake;
}

/*!
 \brief Removed the widget from central widget.
 
 Widget is removed and deleted.
 
 \param which Indicates which widget should be removed (first/last)
 */
void CalenDayContentWidget::remove(CalenWidgetPosition which)
{
    Q_ASSERT(mLayout);

    HbWidget* itemToRemove = NULL;
    switch (which) {
        case ECalenFirstWidget: {
            itemToRemove = mWidgets.takeFirst();
            break;
        }
        case ECalenLastWidget:
        default: {
            itemToRemove = mWidgets.takeLast();
        }
    }

    if (itemToRemove) {
        mLayout->removeItem(itemToRemove);
        delete itemToRemove;
    }
}

/*!
 \brief Performs relayout of widgets when it is needed.
 
 Relayout is done depending on last scroll move:
 - if widget was scrolled to next: first widget is taken and added as last
 - if widget was scrolled to prev: last widget is taken and inserted as first
 
 Emits widgetsRelayoutFinished SIGNAL when relayout is over.
 
 \param scrollTo Direction of last movement
 */
void CalenDayContentWidget::relayoutWidgets(CalenScrollDirection scrollTo)
{
    HbWidget* widget = NULL;
    switch (scrollTo) {
        case ECalenScrollToNext: {
            widget = take(ECalenFirstWidget);
            add(widget, ECalenLastWidget);
            break;
        }
        case ECalenScrollToPrev:
        default: {
            widget = take(ECalenLastWidget);
            add(widget, ECalenFirstWidget);
        }
    }
    emit widgetsRelayoutFinished(scrollTo);
}

/*!
 \brief SLOT handles simultanous vertical scrolling of owned widgets.
 
 \param newPos New scroll position
 */
void CalenDayContentWidget::widgetScrolled(const QPointF &newPos)
{
    for (int i = 0; i < mWidgets.count(); i++) {
        CalenDayItemView *view =
            static_cast<CalenDayItemView *> (mWidgets[i]);
        view->scrollVertically(newPos);
    }
    emit scrollPositionChanged(newPos);
}

/*!
 \brief Initializes content widgets and adds them to layout.
 
 \param parent Parent object 
 */
void CalenDayContentWidget::initializeViews(QGraphicsItem *parent)
{
    // Create item views
    HbModelIterator *iterator(0);

    iterator = new HbModelIterator(&mModelManager.getModel(
        CalenDayModelManager::PreviousDay));
    CalenDayItemView *prevItemView = new CalenDayItemView(
        mModelManager.getServices(), iterator, 0);

    iterator = new HbModelIterator(&mModelManager.getModel(
        CalenDayModelManager::CurrentDay));
    CalenDayItemView *currItemView = new CalenDayItemView(
        mModelManager.getServices(), iterator, 0);

    iterator = new HbModelIterator(&mModelManager.getModel(
        CalenDayModelManager::NextDay));
    CalenDayItemView *nextItemView = new CalenDayItemView(
        mModelManager.getServices(), iterator, 0);

    // Connecting views with widgetScrolled SLOT 
    // to support simultanous vertical scrolling
    connect(prevItemView, SIGNAL(scrollPositionChanged(const QPointF&)), this,
        SLOT(widgetScrolled(const QPointF&)));
    connect(currItemView, SIGNAL(scrollPositionChanged(const QPointF&)), this,
        SLOT(widgetScrolled(const QPointF&)));
    connect(nextItemView, SIGNAL(scrollPositionChanged(const QPointF&)), this,
        SLOT(widgetScrolled(const QPointF&)));

    // Install event filters to receive events necessary for gesture handling
    CalenDayContentScrollArea* scrollArea =
        static_cast<CalenDayContentScrollArea*> (parent);
    prevItemView->installEventFilter(scrollArea);
    currItemView->installEventFilter(scrollArea);
    nextItemView->installEventFilter(scrollArea);

    // Add views to layout
    add(prevItemView);
    add(currItemView);
    add(nextItemView);
}

// End of File
