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
#include <hbpangesture.h>
#include <hbswipegesture.h>
#include <hbapplication.h> // hbapplication
#include <hbactivitymanager.h> // Activity Manager

// User includes
#include "calendayview.h"
#include "calendocloader.h"
#include "calendayviewwidget.h"
#include "calenservices.h"
#include "calencommon.h"
#include "calencontext.h"
#include "calendateutils.h"
#include "calenconstants.h"

// ----------------------------------------------------------------------------
// CalenDayView::CalenDayView
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
CalenDayView::CalenDayView(MCalenServices &services):
CalenNativeView(services),
mSoftKeyAction(NULL),
mGoToTodayAction(NULL),
mActionTaken(false),
mIsAboutToQuitEventConnected(false)
{
    // No implementation yet
    grabGesture(Qt::SwipeGesture);
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
	// get a poitner to activity manager
	HbActivityManager* activityManager = qobject_cast<HbApplication*>(qApp)->activityManager();

	// clean up any previous versions of this activity, if any, i.e. activityName, from the activity manager. 
	// Ignore return value, first boot would always return False. bool declared 
	// only for debugging purpose.
	bool ok = activityManager->removeActivity(activityName);
	
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
		mSoftKeyAction = new HbAction(Hb::BackNaviAction);
		setNavigationAction(mSoftKeyAction);
		// Connect to the signal triggered by clicking on back button.
		connect(mSoftKeyAction, SIGNAL(triggered()), this,
		        SLOT(launchMonthView()));
	}
    // Initialize the content widget
    mDayViewWidget->showWidget();
    
	//set Current Activity as day view
    mActivityId = ECalenDayView;

	// connect to receive a call back on Day View exit. Call back would result in saveActivity 
	// to be called in Native View
    if (!mIsAboutToQuitEventConnected) // check if already not connected
        {
        connect(qobject_cast<HbApplication*>(qApp), SIGNAL(aboutToQuit()), this, SLOT(saveActivity()));
		mIsAboutToQuitEventConnected = true;
        }


    // Population is complete, issue a notification
    populationComplete();
    }

/*!
 Funtion to refresh the current view upon selecting a date
 from GoToDate popup
 */
void CalenDayView::refreshViewOnGoToDate()
{
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
	
	// Initialize the content widget
	mDayViewWidget->showWidget();
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

/*
	Function to listen for gestures
*/
void CalenDayView::gestureEvent(QGestureEvent *event)
{
    if(HbSwipeGesture *gesture = qobject_cast<HbSwipeGesture *>(event->gesture(Qt::SwipeGesture))) {
        if (gesture->state() == Qt::GestureStarted) {
            if(QSwipeGesture::Left == gesture->horizontalDirection()) {
                mServices.IssueCommandL(ECalenShowNextDay);
                event->accept(Qt::SwipeGesture);
            } else if(QSwipeGesture::Right == gesture->horizontalDirection()) {
                mServices.IssueCommandL(ECalenShowPrevDay);
               event->accept(Qt::SwipeGesture);
            }
        }
    } 
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
	// month view launched now, disconnect to get the call backs for saveActivity 
	// on aboutToQuit signal
    disconnectAboutToQuitEvent();
}

// ----------------------------------------------------------------------------
// CCalenDayView::clearListModel
// clears the list model 
// ----------------------------------------------------------------------------
// 
void CalenDayView::clearListModel()
    {
	// day view is removed from the list disconnect for aboutToQuit events
    disconnectAboutToQuitEvent();
    mDayViewWidget->clearListModel();
    }

// ----------------------------------------------------------------------------
// disconnectAboutToQuitEvent disconnects for the aboutToQuit events
// ----------------------------------------------------------------------------
// 
void CalenDayView::disconnectAboutToQuitEvent()
    {
    if (mIsAboutToQuitEventConnected)
        {
        disconnect(qobject_cast<HbApplication*>(qApp), SIGNAL(aboutToQuit()), this, SLOT(saveActivity()));
        mIsAboutToQuitEventConnected = false;
        }
    }

// End of file	--Don't remove this.
