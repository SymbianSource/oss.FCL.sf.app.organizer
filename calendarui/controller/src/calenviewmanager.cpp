/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar view manager
 *
*/


//system includes
#include <QDebug>
#include <hbmainwindow.h>

//user includes
#include "calenviewmanager.h"
#include "calencontroller.h"
#include "hb_calencommands.hrh"
#include "calenservicesimpl.h"
#include "calenmonthview.h"
#include "calendayview.h"
#include "calenmonthview.h"
#include "agendaeventviewer.h"
#include "calennotificationhandler.h"
#include "CalenUid.h"
#include "calenactionuiutils.h"
#include "calensettingsview.h"
#include "calendocloader.h"
#include "calendarui_debug.h"
#include "calencommon.h"

// ----------------------------------------------------------------------------
// CalenViewManager::CalenViewManager
// 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenViewManager::CalenViewManager( CCalenController& aController,
                                         bool isFromServiceFrmwrk)
: mController(aController)
{
	TRACE_ENTRY_POINT;
	
	mCalenEventViewer = NULL;
	mMonthViewDocLoader = NULL;
	mDayViewDocLoader = NULL;	
	mDayViewAltDocLoader = NULL;
	mCalenDayView = NULL;
	mCalenMonthView = NULL;
	mCalenDayViewAlt = NULL;
	
	// Connect to instance view and entry view creation signals from agenda
	// interface
	connect(mController.agendaInterface(), SIGNAL(instanceViewCreationCompleted(int)),
	        this, SLOT(handleInstanceViewCreation(int)));
	connect(mController.agendaInterface(), SIGNAL(entryViewCreationCompleted(int)),
		        this, SLOT(handleEntryViewCreation(int)));
	
	if (isFromServiceFrmwrk) {
		// Dont load any views until our remote slot gets called in
		// calenserviceprovider.cpp
		// Just have an empty mainwindow
	} else {
		// Do the normal startup
		// Load the month view and active it and add it to main window
		mFirstView = ECalenMonthView;
		loadMonthView();
		ActivateDefaultViewL(ECalenMonthView);
		// Connect to the view ready signal so that we construct other view 
		// once this view is shown
		connect(&mController.MainWindow(), SIGNAL(viewReady()), 
						this, SLOT(handleMainViewReady()));
		
		mController.MainWindow().addView(mCalenMonthView);
		mController.MainWindow().setCurrentView(mCalenMonthView);
	}
	
	TRACE_EXIT_POINT;
}

// ----------------------------------------------------------------------------
// CalenViewManager::~CalenViewManager
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenViewManager::~CalenViewManager()
{
	TRACE_ENTRY_POINT;

	delete mCalenMonthView;
	delete mCalenDayView;
	delete mSettingsView;
	delete mDayViewDocLoader;
	delete mDayViewAltDocLoader;
	delete mMonthViewDocLoader;
	TRACE_EXIT_POINT;
}

// ----------------------------------------------------------------------------
// CalenViewManager::constructAndActivateView
// Constructs and activates the requested view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::constructAndActivateView(int view)
{
	TRACE_ENTRY_POINT;
	// We are here because, some other application is launching calendar with 
	// the view, hence connect to viewReady() signal to do any lazy loading
	// in the slot
	
	// Connect to the view ready signal so that we construct other view 
	// once this view is shown
	connect(&mController.MainWindow(), SIGNAL(viewReady()), 
					this, SLOT(handleMainViewReady()));
	if (view == ECalenMonthView) {
		mFirstView = ECalenMonthView;
		loadMonthView();
		ActivateDefaultViewL(ECalenMonthView);
		// Add month view to mainwindow.
		mController.MainWindow().addView(mCalenMonthView);
		mController.MainWindow().setCurrentView(mCalenMonthView);
	} else if (view == ECalenDayView) {
		mFirstView = ECalenDayView;
		loadDayView();
		ActivateDefaultViewL(ECalenDayView);
		// Add day view to mainwindow.
		mController.MainWindow().addView(mCalenDayView);
		mController.MainWindow().setCurrentView(mCalenDayView);
	}
	TRACE_EXIT_POINT;
}

// ----------------------------------------------------------------------------
// CalenViewManager::loadMonthView
// Loads month view frm the docml
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::loadMonthView()
{
	bool loadSuccess = false;
	Qt::Orientation currentOrienation = mController.MainWindow().orientation();
	// Create the month view docloader object.
	mMonthViewDocLoader = new CalenDocLoader(mController);
	mMonthViewDocLoader->load(CALEN_MONTHVIEW_XML_FILE, &loadSuccess);
	Q_ASSERT_X(loadSuccess, "calenviewmanager.cpp", 
											"Unable to load month view XML");
	// Based on the current orientation, load the appropriate section
	if (Qt::Vertical == currentOrienation) {
		mMonthViewDocLoader->load(CALEN_MONTHVIEW_XML_FILE, 
											CALEN_PORTRAIT, &loadSuccess);
		} else {
			mMonthViewDocLoader->load(CALEN_MONTHVIEW_XML_FILE, 
											CALEN_LANDSCAPE, &loadSuccess);
		}
	Q_ASSERT_X(loadSuccess, "calenviewmanager.cpp", "Unable to load XML");
	
	// Get the calenmonth view from the loader.
	mCalenMonthView = static_cast<CalenMonthView *> 
							(mMonthViewDocLoader->findWidget(CALEN_MONTHVIEW));
	Q_ASSERT_X(mCalenMonthView, "calenviewmanager.cpp", 
											"Unable to load calenMonth view");
	
	// Setup the month view.
	mCalenMonthView->setupView(mMonthViewDocLoader);
}

// ----------------------------------------------------------------------------
// CalenViewManager::loadDayView
// Loads the day view frm the docml
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::loadDayView()
{
	bool loadSuccess = false;
	// Create the day view docloader object.
	mDayViewDocLoader = new CalenDocLoader(mController);
	
	// Load default section
	mDayViewDocLoader->load(CALEN_DAYVIEW_XML_FILE, &loadSuccess);
	if (!loadSuccess) {
		qFatal("calenviewmanager.cpp : Unable to load XML");
	}
	
	// Get the calenday view from the loader
	mCalenDayView = static_cast<CalenDayView *> (mDayViewDocLoader->findWidget(CALEN_DAYVIEW));
	if (!mCalenDayView) {
		qFatal("calenviewmanager.cpp : Unable to find day view");
	}
	
	// Setup the day view
	mCalenDayView->setupView(mDayViewDocLoader);
	
	// The following code is done to provide swipe support
	// in day view. Idea is to create two views and keep
	// switiching between these two using view switch effects
	// to provide an illusion of flow to the user
	loadAlternateDayView();
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleMainViewReady
// Slot to handle viewReady() signal from mainwindow
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleMainViewReady()
{
	// Construct the month view part that is kept for lazy loading
	if (mCalenMonthView) {
		mCalenMonthView->doLazyLoading();
	}
	
	// Construct other views
	constructOtherViews();
	
	// disconnect the view ready signal as we dont need it anymore
	disconnect(&mController.MainWindow(), SIGNAL(viewReady()), 
			   this, SLOT(handleMainViewReady()));
}

// ----------------------------------------------------------------------------
// CalenViewManager::constructOtherViews
// Constructs the other views apart frm firstview and adds them to main window
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::constructOtherViews()
{
	// Load all other views except mFirstView
	
	// NOTE: Right now, since Calendar has only two views, month view 
	// and day view, when client launches day view, then there is no need
	// to construct the month view as per UI REQ., but tomorrow if new views
	// come after day view, then we need to construct those views if they are
	// native views. Right now, there is a event viewer but its not a native
	// view. Hence, if day view is launched, dont construct month view
	if (mFirstView == ECalenDayView) {
		// Construct other views as mentioned above
		return;
	} else {
		// Load all other views 
		loadDayView();
	}

	// Setup the settings view
	mSettingsView = new CalenSettingsView(mController.Services());
}

// ----------------------------------------------------------------------------
// CalenViewManager::getFirstView
// Returns the first view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
int CalenViewManager::getFirstView()
{
	return mFirstView;
}

// ----------------------------------------------------------------------------
// CalenViewManager::showNextDay
// other items were commented in a header
// ----------------------------------------------------------------------------
//
void CalenViewManager::showNextDay()
{
    // Set the context for the next day
    QDateTime currentDay = mController.Services().Context().focusDateAndTimeL();
    currentDay = currentDay.addDays(1);
    mController.Services().Context().setFocusDateL(currentDay, ECalenDayView);
    mCurrentViewId = ECalenDayView;
    // Check which is the currently activated view
    if (mController.MainWindow().currentView() == mCalenDayView) {
        HbEffect::add(mCalenDayView,
                      ":/fxml/view_hide",
                      "hide");
        HbEffect::add(mCalenDayViewAlt,
                      ":/fxml/view_show",
                      "show");
        // Set the other day view as the current view
        // and animate to provide illusion of swipe
        mCalenDayViewAlt->doPopulation();
        mController.MainWindow().setCurrentView(mCalenDayViewAlt, true, Hb::ViewSwitchUseNormalAnim);
    } else {
        HbEffect::add(mCalenDayViewAlt,
                      ":/fxml/view_hide",
                      "hide");
        HbEffect::add(mCalenDayView,
                      ":/fxml/view_show",
                      "show");
        mCalenDayView->doPopulation();
        mController.MainWindow().setCurrentView(mCalenDayView, true, Hb::ViewSwitchUseNormalAnim);
    }
}

// ----------------------------------------------------------------------------
// CalenViewManager::showPrevDay
// other items were commented in a header
// ----------------------------------------------------------------------------
//
void CalenViewManager::showPrevDay()
{
    QDateTime currentDay = mController.Services().Context().focusDateAndTimeL();
    currentDay = currentDay.addDays(-1);
    mController.Services().Context().setFocusDateL(currentDay, ECalenDayView);
    mCurrentViewId = ECalenDayView;
    if (mController.MainWindow().currentView() == mCalenDayView) {
        HbEffect::add(mCalenDayView,
                      ":/fxml/view_hide_back",
                      "hide");
        HbEffect::add(mCalenDayViewAlt,
                      ":/fxml/view_show_back",
                      "show");
        mCalenDayViewAlt->doPopulation();
        mController.MainWindow().setCurrentView(mCalenDayViewAlt, true, Hb::ViewSwitchUseNormalAnim);
    } else {
        HbEffect::add(mCalenDayViewAlt,
                      ":/fxml/view_hide_back",
                      "hide");
        HbEffect::add(mCalenDayView,
                      ":/fxml/view_show_back",
                      "show");
        mCalenDayView->doPopulation();
        mController.MainWindow().setCurrentView(mCalenDayView, true, Hb::ViewSwitchUseNormalAnim);
    }
}

// ----------------------------------------------------------------------------
// CalenViewManager::removeDayViews
// other items were commented in a header
// ----------------------------------------------------------------------------
//
void CalenViewManager::removeDayViews()
{
    mCalenDayView->clearListModel();
    mCalenDayViewAlt->clearListModel();
    mController.MainWindow().removeView(mCalenDayView);
    mController.MainWindow().removeView(mCalenDayViewAlt);
}

// ----------------------------------------------------------------------------
// CalenViewManager::~ActivateDefaultViewL
//  Activates the default view, as retrieved from settings.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::ActivateDefaultViewL(int defaultView)
{
	TRACE_ENTRY_POINT;
	mCurrentViewId = defaultView;
	if (ECalenMonthView == defaultView) {
		mCalenMonthView->doPopulation();
	} else if (ECalenDayView == defaultView) {
		mCalenDayView->doPopulation();
	}
	TRACE_EXIT_POINT;
}

// ----------------------------------------------------------------------------
// Refresh current view.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
void CalenViewManager::refreshCurrentViewL()
{
	TRACE_ENTRY_POINT;
	activateCurrentView();
	TRACE_EXIT_POINT;
}

// ----------------------------------------------------------------------------
// Activate current view.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
void CalenViewManager::activateCurrentView()
{
	TRACE_ENTRY_POINT;
	switch (mCurrentViewId) {
		case ECalenMonthView:
		    mCalenMonthView->doPopulation();
		    mController.MainWindow().setCurrentView(mCalenMonthView);
			break;
		case ECalenDayView:
		    if (mController.MainWindow().currentView() == mCalenDayView) {
		        // This happens when settings view or event viewer is opened
		        // from the agenda view. Simply repopulate the view
		    	if (mCalenDayView) {
					mCalenDayView->doPopulation();
					mController.MainWindow().setCurrentView(mCalenDayView);
		    	}
		    } else if (mController.MainWindow().currentView() == mCalenDayViewAlt){
		        // This happens when settings view or event viewer is opened
		        // from the agenda view. Simply repopulate the view
		    	if (mCalenDayViewAlt) {
					mCalenDayViewAlt->doPopulation();
					mController.MainWindow().setCurrentView(mCalenDayViewAlt);
		    	}
		    } else {
		        // This is called whenever the day view is opened from the month
		        // view. Since the day view is not added to the mainwindow,
		        // add the day views to mainwindow and set any one of them as 
		        // current view
		    	if (mCalenDayView) {
		    		mCalenDayView->doPopulation();
					mController.MainWindow().addView(mCalenDayView);
					mController.MainWindow().setCurrentView(mCalenDayView);
					mController.MainWindow().addView(mCalenDayViewAlt);
		    	}
		    }
			break;
		case ECalenLandscapeDayView:
			// For later implementation
			break;
	}
	TRACE_EXIT_POINT;
}

// ----------------------------------------------------------------------------
// CalenViewManager::launchEventView
// Launch event view.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::launchEventView()
{
	MCalenContext& context = mController.Services().Context();
	AgendaEntry viewEntry= mController.Services().agendaInterface()->fetchById(
			context.instanceId().mEntryLocalUid );
	if (viewEntry.isRepeating() 
			&& viewEntry.type() != AgendaEntry::TypeTodo) {
		QDateTime startDateTime = context.focusDateAndTimeL();
		viewEntry.setStartAndEndTime(startDateTime, 
		                     startDateTime.addSecs(viewEntry.durationInSecs()));
	}
	mCalenEventViewer = new AgendaEventViewer(
			mController.Services().agendaInterface(), this);
	connect(mCalenEventViewer, SIGNAL(viewingCompleted(const QDate)),
	        this, SLOT(handleViewingCompleted(const QDate)));
	connect(mCalenEventViewer, SIGNAL(editingStarted()),
	        this, SLOT(handleEditingStarted()));
	connect(mCalenEventViewer, SIGNAL(editingCompleted()),
	        this, SLOT(handleEditingCompleted()));
	connect(mCalenEventViewer, SIGNAL(deletingStarted()),
		        this, SLOT(handleDeletingStarted()));
	connect(mCalenEventViewer, SIGNAL(deletingCompleted()),
			        this, SLOT(handleDeletingCompleted()));


	// Launch agenda event viewer
	mCalenEventViewer->view(viewEntry, AgendaEventViewer::ActionEditDelete);
}

// ----------------------------------------------------------------------------
// CalenViewManager::loadAlternateDayView
// other items were commented in a header
// ----------------------------------------------------------------------------
//
void CalenViewManager::loadAlternateDayView()
{
    bool loadSuccess = false;
    // Create the day view docloader object.
    mDayViewAltDocLoader = new CalenDocLoader(mController);

    // Load default section
    mDayViewAltDocLoader->load(CALEN_DAYVIEW_XML_FILE, &loadSuccess);
    if (!loadSuccess) {
        qFatal("calenviewmanager.cpp : Unable to load XML");
    }

    // Get the calenday view from the loader
    mCalenDayViewAlt = static_cast<CalenDayView *> (mDayViewAltDocLoader->findWidget(CALEN_DAYVIEW));
    if (!mCalenDayViewAlt) {
        qFatal("calenviewmanager.cpp : Unable to find alternate day view");
    }

    // Setup the day view
    mCalenDayViewAlt->setupView(mDayViewAltDocLoader);
}

// ----------------------------------------------------------------------------
// CalenViewManager::HandleCommandL
// Handles view manager commands.
// @return ETrue if command is handled, EFalse otherwise
// ----------------------------------------------------------------------------
//
TBool CalenViewManager::HandleCommandL(const TCalenCommand& command)
{
	TRACE_ENTRY_POINT;
	TBool commandUsed(EFalse);
	
	switch (command.Command()) {
		case ECalenMonthView:
		    removeDayViews();
			// Add month view to mainwindow.
			mController.MainWindow().addView(mCalenMonthView);
			mCurrentViewId = ECalenMonthView;
			activateCurrentView();
			break;
		case ECalenDayView:
			// Remove month view from mainwindow.
			mController.MainWindow().removeView(mCalenMonthView);
			mCurrentViewId = ECalenDayView;
			activateCurrentView();
			break;
		case ECalenEventView:
			launchEventView();
			break;
		case ECalenLandscapeDayView:
			mCurrentViewId = ECalenLandscapeDayView;
			activateCurrentView();
			break;
		case ECalenStartActiveStep:
			activateCurrentView();
			break;
		case ECalenShowNextDay:
		    showNextDay();
		    break;
		case ECalenShowPrevDay:
            showPrevDay();
		    break;
	}
	TRACE_EXIT_POINT;
	return commandUsed;
}

// ----------------------------------------------------------------------------
// CalenViewManager::HandleNotification
//  Handles notifications.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::HandleNotification(
                                         const TCalenNotification notification)
{
	TRACE_ENTRY_POINT;
	switch (notification) {
		case ECalenNotifySystemLocaleChanged:
		case ECalenNotifyExternalDatabaseChanged:
		case ECalenNotifyDialogClosed:
		case ECalenNotifyMultipleEntriesDeleted:
		case ECalenNotifyEntrySaved:
		case ECalenNotifyEntryDeleted:
		case ECalenNotifyInstanceDeleted:
		case ECalenNotifyEntryClosed:
		case ECalenNotifySettingsClosed:
			mController.Services().IssueCommandL(ECalenStartActiveStep);
			break;
		default:
			break;
	}
	TRACE_EXIT_POINT;
}

// ----------------------------------------------------------------------------
// CalenViewManager::settingsView
//  Returns the settings view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenSettingsView* CalenViewManager::settingsView()
{
	return mSettingsView;
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleViewingCompleted
//  Slot to handle signal viewingCompleted by the agenda event viewer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleViewingCompleted(const QDate date)
{
	qDebug() <<"calendar: CalenViewManager::handleEditingCompleted -->";
	
	// Cleanup.
	mCalenEventViewer->deleteLater();
	if (!date.isNull() && date.isValid()) {
	mController.Services().Context().setFocusDateL(QDateTime(date), 
	                                               ECalenDayView);
	}
	mController.Services().IssueNotificationL(ECalenNotifyEntryClosed);
	
	qDebug() <<"calendar: CalenViewManager::handleEditingCompleted <--";
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleEditingStarted
//  Slot to handle signal editingStarted by the agenda event viewer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleEditingStarted()
{
	qDebug() <<"calendar: CalenViewManager::handleEditingStarted -->";
	
	mController.IssueCommandL(ECalenEditEntryFromViewer);
	
	qDebug() <<"calendar: CalenViewManager::handleEditingStarted <--";
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleEditingCompleted
//  Slot to handle signal editingCompleted by the agenda event viewer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleEditingCompleted()
{
	qDebug() <<"calendar: CalenViewManager::handleEditingStarted -->";
	
	mController.Services().IssueNotificationL(ECalenNotifyEditorClosedFromViewer);
	
	qDebug() <<"calendar: CalenViewManager::handleEditingStarted <--";
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleDeletingStarted
//  Slot to handle signal deletingStarted by the agenda event viewer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleDeletingStarted()
{
	qDebug() <<"calendar: CalenViewManager::handleEditingStarted -->";
	
	mController.IssueCommandL(ECalenDeleteEntryFromViewer);
	
	qDebug() <<"calendar: CalenViewManager::handleEditingStarted <--";
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleDeletingCompleted
//  Slot to handle signal deletingCompleted by the agenda event viewer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleDeletingCompleted()
{
	qDebug() <<"calendar: CalenViewManager::handleEditingStarted -->";
	
	mController.Services().IssueNotificationL(ECalenNotifyEntryDeleted);
	
	qDebug() <<"calendar: CalenViewManager::handleEditingStarted <--";
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleInstanceViewCreation
//  Slot to handle completion of instance view creation
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleInstanceViewCreation(int status)
{
	Q_UNUSED(status);
	mCalenMonthView->fetchEntriesAndUpdateModel();
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleDeletingCompleted
//  Slot to handle completion of entry view creation
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleEntryViewCreation(int status)
{
	// Nothing Yet
	Q_UNUSED(status);
}
// End of file	--Don't remove this.
