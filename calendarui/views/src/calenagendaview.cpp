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
* Description:  CalenAgendaView implementation.
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
#include "calenagendaview.h"
#include "calendocloader.h"
#include "calenagendaviewwidget.h"
#include "calenservices.h"
#include "calencommon.h"
#include "calencontext.h"
#include "calendateutils.h"
#include "calenconstants.h"

// ----------------------------------------------------------------------------
// CalenAgendaView::CalenAgendaView
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
CalenAgendaView::CalenAgendaView(MCalenServices &services):
CalenNativeView(services),
mSoftKeyAction(NULL),
mGoToTodayAction(NULL),
mSwitchToDayViewAction(NULL),
mActionTaken(false),
mIsAboutToQuitEventConnected(false)
{
    // No implementation yet
    grabGesture(Qt::SwipeGesture);
}

// ----------------------------------------------------------------------------
// CalenAgendaView::~CalenAgendaView
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
CalenAgendaView::~CalenAgendaView()
{
    // No implementation yet
}

// ----------------------------------------------------------------------------
// CalenAgendaView::setupView
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaView::setupView(CalenDocLoader *docLoader)
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
	mAgendaViewWidget = qobject_cast<CalenAgendaViewWidget*>(mDocLoader->findWidget(CALEN_AGENDAVIEW_WIDGET));
	if (!mAgendaViewWidget) {
	    qFatal("calenagendaview.cpp : Unable to find the content widget");
	}
	mAgendaViewWidget->setupWidget(this);
	
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
// CalenAgendaView::doPopulation
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenAgendaView::doPopulation()
    {
    // The content widget has not been constructed. Don't do anything
    if (!mAgendaViewWidget) {
        return;
    }
    // Get the day for which this view is being shown from the context
    mDate = mServices.Context().focusDateAndTime();
    
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
    if (ECalenAgendaView != mServices.getFirstView()) {
		mSoftKeyAction = new HbAction(Hb::BackNaviAction);
		setNavigationAction(mSoftKeyAction);
		// Connect to the signal triggered by clicking on back button.
		connect(mSoftKeyAction, SIGNAL(triggered()), this,
		        SLOT(launchMonthView()));
		if (mSwitchToDayViewAction) {
			mSwitchToDayViewAction->setVisible(true);
				}
	} else {
		if (mSwitchToDayViewAction) {
			mSwitchToDayViewAction->setVisible(false);
		}
	}
    // Initialize the content widget
    mAgendaViewWidget->showWidget();
    
	//set Current Activity as day view
    mActivityId = ECalenAgendaView;

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
void CalenAgendaView::refreshViewOnGoToDate()
{
	// Get the day for which this view is being shown from the context
	mDate = mServices.Context().focusDateAndTime();
	
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
	mAgendaViewWidget->showWidget();
}

// ----------------------------------------------------------------------------
// CalenAgendaView::HandleNotification
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaView::HandleNotification(const TCalenNotification notification)
{
    Q_UNUSED(notification)
    // No implementation yet
}

// ----------------------------------------------------------------------------
// CalenAgendaView::docLoader
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
CalenDocLoader* CalenAgendaView::docLoader()
{
    return mDocLoader;
}

/*
	Function to listen for gestures
*/
void CalenAgendaView::gestureEvent(QGestureEvent *event)
{
    if(HbSwipeGesture *gesture = qobject_cast<HbSwipeGesture *>(event->gesture(Qt::SwipeGesture))) {
        if (gesture->state() == Qt::GestureStarted) {
            if(QSwipeGesture::Left == gesture->sceneHorizontalDirection()) {
                mServices.IssueCommandL(ECalenShowNextDay);
                event->accept(Qt::SwipeGesture);
            } else if(QSwipeGesture::Right == gesture->sceneHorizontalDirection()) {
                mServices.IssueCommandL(ECalenShowPrevDay);
               event->accept(Qt::SwipeGesture);
            }
        }
    } 
}

// ----------------------------------------------------------------------------
// CalenAgendaView::createToolBar
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
void CalenAgendaView::setupActions()
{
	// Get the actions associated with this view
	HbAction *newEventAction = qobject_cast<HbAction *>
                                (mDocLoader->findObject(CALEN_AGENDAVIEW_MENU_NEW_EVENT));
	if (!newEventAction) {
	    qFatal("calenagendaview.cpp : Unable to find new event action");
	}
	// Connect to the signal triggered by new event action
	connect(newEventAction, SIGNAL(triggered()), mAgendaViewWidget, SLOT(createNewEvent()));
	
	mGoToTodayAction = qobject_cast<HbAction *>
                        (mDocLoader->findObject(CALEN_AGENDAVIEW_MENU_GO_TO_TODAY));
	if (!mGoToTodayAction) {
	    qFatal("calenagendaview.cpp : Unable to find go to today action");
	}
	// Connect to the signal triggered by go to Today action
	connect(mGoToTodayAction, SIGNAL(triggered()), mAgendaViewWidget, SLOT(goToToday()));
	
	HbAction *goToDateAction = qobject_cast<HbAction *>
                                (mDocLoader->findObject(CALEN_AGENDAVIEW_MENU_GO_TO_DATE));
	if (!goToDateAction) {
	    qFatal("calenagendaview.cpp : Unable to find go to date action");
	}
	// Connect to the signal triggered by go to Date action
	connect(goToDateAction, SIGNAL(triggered()), this, SLOT(goToDate()));
	
	mSwitchToDayViewAction = qobject_cast<HbAction *>
	                                (mDocLoader->findObject(CALEN_AGENDAVIEW_MENU_SWITCH_TO_DAYVIEW));
	if (!mSwitchToDayViewAction) {
		    qFatal("calenagendaview.cpp : Unable to find go to date action");
		}
	// Connect to the signal triggered by switch to dayview action
	connect(mSwitchToDayViewAction, SIGNAL(triggered()), this, SLOT(launchDayView()));
	
	HbAction *settingsAction = qobject_cast<HbAction *>
                                (mDocLoader->findObject(CALEN_AGENDAVIEW_MENU_SETTINGS));
	if (!settingsAction) {
	    qFatal("calenagendaview.cpp : Unable to find settings action");
	}
	// Connect to the signal triggered by settings action
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(launchSettingsView()));
}

// ----------------------------------------------------------------------------
// CalenAgendaView::onLocaleChanged
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
//    
void CalenAgendaView::onLocaleChanged(int reason)
{
    Q_UNUSED(reason)
    // Notify the content widget about the change
    if(mAgendaViewWidget) {
    mAgendaViewWidget->handleLocaleChange();
    }
}

// ----------------------------------------------------------------------------
// CalenAgendaView::orientationChanged
// Rest of the details are commented in the header
// ----------------------------------------------------------------------------
// 
void CalenAgendaView::orientationChanged(Qt::Orientation orientation)
{
    // Notify the content widget about the change
    if (mAgendaViewWidget) {
    mAgendaViewWidget->orientationChanged(orientation);
    }
}

// ----------------------------------------------------------------------------
// CalenAgendaView::launchMonthView
// ----------------------------------------------------------------------------
//    
void CalenAgendaView::launchMonthView()
{
    // Issue the command to launch the month view
    mServices.IssueCommandL(ECalenMonthView);
	// month view launched now, disconnect to get the call backs for saveActivity 
	// on aboutToQuit signal
    disconnectAboutToQuitEvent();
}

// ----------------------------------------------------------------------------
// CalenAgendaView::launchDayView
// ----------------------------------------------------------------------------
//    
void CalenAgendaView::launchDayView()
{
    // Issue the command to launch the day view
    mServices.IssueCommandL(ECalenDayView);
}

// ----------------------------------------------------------------------------
// clears the list model 
// ----------------------------------------------------------------------------
// 
void CalenAgendaView::clearListModel()
    {
	// day view is removed from the list disconnect for aboutToQuit events
    disconnectAboutToQuitEvent();
    mAgendaViewWidget->clearListModel();
    }

// ----------------------------------------------------------------------------
// disconnectAboutToQuitEvent disconnects for the aboutToQuit events
// ----------------------------------------------------------------------------
// 
void CalenAgendaView::disconnectAboutToQuitEvent()
    {
    if (mIsAboutToQuitEventConnected)
        {
        disconnect(qobject_cast<HbApplication*>(qApp), SIGNAL(aboutToQuit()), this, SLOT(saveActivity()));
        mIsAboutToQuitEventConnected = false;
        }
    }

// End of file	--Don't remove this.
