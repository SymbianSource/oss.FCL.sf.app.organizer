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
* Description:  CalenDayView implementation.
*
*/

// System includes
#include <QGraphicsSceneEvent>
#include <hbmainwindow.h>
#include <hbaction.h>

// User includes
#include "calendayview.h"
#include "calendocloader.h"
#include "calendayviewwidget.h"
#include "calenservices.h"
#include "calencommon.h"
#include "calencontext.h"
#include "calendateutils.h"

// ----------------------------------------------------------------------------
// CalenDayView::CalenDayView
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
CalenDayView::CalenDayView(MCalenServices &services):
CalenNativeView(services),
mSoftKeyAction(NULL),
mGoToTodayAction(NULL),
mActionTaken(false)
{
    // No implementation yet
}

// ----------------------------------------------------------------------------
// CCalenDayView::~CalenDayView
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
CalenDayView::~CalenDayView()
{
    // No implementation yet
}

// ----------------------------------------------------------------------------
// CCalenDayView::setupView
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayView::setupView(CalenDocLoader *docLoader)
{
    if (!docLoader) {
        // Nothing can be done. Simply return
        return;
    }
    // Store the document loader for reference later
	mDocLoader = docLoader;
	
	// Listen to orientation change events
	connect(&(mServices.MainWindow()), SIGNAL(orientationChanged(Qt::Orientation)),
	        this, SLOT(orientationChanged(Qt::Orientation)));
	
	// Get the pointer to the content widget
	mDayViewWidget = qobject_cast<CalenDayViewWidget*>(mDocLoader->findWidget(CALEN_DAYVIEW_WIDGET));
	if (!mDayViewWidget) {
	    qFatal("calendayview.cpp : Unable to find the content widget");
	}
	mDayViewWidget->setupWidget(this);
	
	// Initialize all the menu and toolbar actions
	setupActions();
}

// ----------------------------------------------------------------------------
// CCalenDayView::doPopulation
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenDayView::doPopulation()
    {
    // The content widget has not been constructed. Don't do anything
    if (!mDayViewWidget) {
        return;
    }
    // Get the day for which this view is being shown from the context
    mDate = mServices.Context().focusDateAndTimeL();
    
    // Check if the current day being shown is "Today"
    if (mGoToTodayAction) {
        if (mDate.date() == CalenDateUtils::today().date()) {
            // Hide the "Go to today" option
            mGoToTodayAction->setVisible(false);
        } else {
            mGoToTodayAction->setVisible(true);
        }
    }
    
    // Set self as the current view
    // mServices.MainWindow().setCurrentView(this);
    
    // Dont override the soft key behavior if day view is the first view
    if (ECalenDayView != mServices.getFirstView()) {
		mSoftKeyAction = new HbAction(Hb::BackAction);
		setNavigationAction(mSoftKeyAction);
		// Connect to the signal triggered by clicking on back button.
		connect(mSoftKeyAction, SIGNAL(triggered()), this,
		        SLOT(launchMonthView()));
	}
    // Initialize the content widget
    mDayViewWidget->showWidget();
    
    // Population is complete, issue a notification
    populationComplete();
    }

// ----------------------------------------------------------------------------
// CCalenDayView::HandleNotification
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayView::HandleNotification(const TCalenNotification notification)
{
    Q_UNUSED(notification)
    // No implementation yet
}

// ----------------------------------------------------------------------------
// CCalenDayView::docLoader
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
CalenDocLoader* CalenDayView::docLoader()
{
    return mDocLoader;
}

// ----------------------------------------------------------------------------
// CCalenDayView::hasEvents
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenDayView::hasEvents(bool yes)
{
    if (mDeleteAction) {
        mDeleteAction->setVisible(yes);
    }
}

// ----------------------------------------------------------------------------
// CalenDayView::handleLocaleChange
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenDayView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    // TODO: Remove these after gestures are available
    mTapPoint = event->pos();
    event->accept();
}

// ----------------------------------------------------------------------------
// CalenDayView::handleLocaleChange
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenDayView::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // TODO: Remove these after gestures are available
    QPointF curPos = event->pos();
    if (abs(curPos.y() - mTapPoint.y()) > 20) {
        event->accept();
        return;
    }
    if (curPos.x() - mTapPoint.x() > 50) {
        mTapPoint = QPointF(0, 0);
        mServices.IssueCommandL(ECalenShowPrevDay);
        mActionTaken = true;
    } else if (curPos.x() - mTapPoint.x() < -50) {
        mTapPoint = QPointF(0, 0);
        mServices.IssueCommandL(ECalenShowNextDay);
        mActionTaken = true;
    }
    event->accept();
}

// ----------------------------------------------------------------------------
// CCalenDayView::eventFilter
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
bool CalenDayView::eventFilter(QObject *source, QEvent *event)
{
    // TODO : remove this line after gestures are available
    mActionTaken = false;
    Q_UNUSED(source)
    if (event->type() == QEvent::GraphicsSceneMousePress) {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
        mousePressEvent(mouseEvent);
    } else if (event->type() == QEvent::GraphicsSceneMouseMove) {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
        mouseMoveEvent(mouseEvent);
    }
    if (mActionTaken) {
        // Swipe gesture has been enforced.
        // Do not pass the event to the source
        return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// CCalenDayView::createToolBar
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenDayView::setupActions()
{
	// Get the actions associated with this view
	HbAction *newEventAction = qobject_cast<HbAction *>
                                (mDocLoader->findObject(CALEN_DAYVIEW_MENU_NEW_EVENT));
	if (!newEventAction) {
	    qFatal("calendayview.cpp : Unable to find new event action");
	}
	// Connect to the signal triggered by new event action
	connect(newEventAction, SIGNAL(triggered()), mDayViewWidget, SLOT(createNewEvent()));
	
	mGoToTodayAction = qobject_cast<HbAction *>
                        (mDocLoader->findObject(CALEN_DAYVIEW_MENU_GO_TO_TODAY));
	if (!mGoToTodayAction) {
	    qFatal("calendayview.cpp : Unable to find go to today action");
	}
	// Connect to the signal triggered by new event action
	connect(mGoToTodayAction, SIGNAL(triggered()), mDayViewWidget, SLOT(goToToday()));
	
	HbAction *goToDateAction = qobject_cast<HbAction *>
                                (mDocLoader->findObject(CALEN_DAYVIEW_MENU_GO_TO_DATE));
	if (!goToDateAction) {
	    qFatal("calendayview.cpp : Unable to find go to date action");
	}
	// Connect to the signal triggered by new event action
	connect(goToDateAction, SIGNAL(triggered()), this, SLOT(goToDate()));
	
	mDeleteAction = qobject_cast<HbAction *>
                                (mDocLoader->findObject(CALEN_DAYVIEW_MENU_DELETE));
	if (!mDeleteAction) {
	    qFatal("calendayview.cpp : Unable to find delete action");
	}
	// Connect to the signal triggered by new event action
	connect(mDeleteAction, SIGNAL(triggered()), mDayViewWidget, SLOT(deleteEntries()));
	
	HbAction *settingsAction = qobject_cast<HbAction *>
                                (mDocLoader->findObject(CALEN_DAYVIEW_MENU_SETTINGS));
	if (!settingsAction) {
	    qFatal("calendayview.cpp : Unable to find settings action");
	}
	// Connect to the signal triggered by new event action
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(launchSettingsView()));
}

// ----------------------------------------------------------------------------
// CCalenDayView::onLocaleChanged
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenDayView::onLocaleChanged(int reason)
{
    Q_UNUSED(reason)
    // Notify the content widget about the change
    if(mDayViewWidget) {
        mDayViewWidget->handleLocaleChange();
    }
}

// ----------------------------------------------------------------------------
// CCalenDayView::orientationChanged
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenDayView::orientationChanged(Qt::Orientation orientation)
{
    // Notify the content widget about the change
    if (mDayViewWidget) {
        mDayViewWidget->orientationChanged(orientation);
    }
}

// ----------------------------------------------------------------------------
// CCalenDayView::launchMonthView
// ----------------------------------------------------------------------------
//    
void CalenDayView::launchMonthView()
{
    // Issue the command to launch the month view
    mServices.IssueCommandL(ECalenMonthView);
}

// End of file	--Don't remove this.
