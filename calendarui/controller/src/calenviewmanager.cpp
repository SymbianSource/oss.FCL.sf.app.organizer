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
#include <hbmainwindow.h>
#include <hbinstance.h>
#include <hbapplication.h> // hbapplication
#include <xqserviceutil.h> // service utils

//user includes
#include "calenviewmanager.h"
#include "calencontroller.h"
#include "hb_calencommands.hrh"
#include "calenservicesimpl.h"
#include "calenmonthview.h"
#include "calenagendaview.h"
#include "calenmonthview.h"
#include "agendaeventviewer.h"
#include "calennotificationhandler.h"
#include "CalenUid.h"
#include "calenactionuiutils.h"
#include "calensettingsview.h"
#include "calendocloader.h"
#include "calendarui_debug.h"
#include "calencommon.h"
#include "calendayview.h"
#include "agendautil.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenviewmanagerTraces.h"
#endif

// ----------------------------------------------------------------------------
// CalenViewManager::CalenViewManager
// 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenViewManager::CalenViewManager( CCalenController& aController)
: mController(aController)
{
	OstTraceFunctionEntry0( CALENVIEWMANAGER_CALENVIEWMANAGER_ENTRY );
	
	// Following block intializes member variables
	mCalenEventViewer = NULL;
	mMonthViewDocLoader = NULL;
	mAgendaViewDocLoader = NULL;	
	mAgendaViewAltDocLoader = NULL;
	mCalenAgendaView = NULL;
	mCalenMonthView = NULL;
	mCalenAgendaViewAlt = NULL;
	mSettingsView = NULL;
	mCalenDayView = NULL;
	
	// Connect to instance view and entry view creation signals from agenda
	// interface
	connect(mController.agendaInterface(), SIGNAL(instanceViewCreationCompleted(int)),
	        this, SLOT(handleInstanceViewCreation(int)));
	connect(mController.agendaInterface(), SIGNAL(entryViewCreationCompleted(int)),
		        this, SLOT(handleEntryViewCreation(int)));
	connect(mController.agendaInterface(), SIGNAL(entriesChanged(QList<ulong>)),
								this, SLOT(handleEntriesChanged(QList<ulong>)));
	OstTraceFunctionExit0( CALENVIEWMANAGER_CALENVIEWMANAGER_EXIT );
}

void CalenViewManager::SecondPhaseConstruction()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_SECONDPHASECONSTRUCTION_ENTRY );
    
    // Check the Application Startup reason from Activity Manager
    int activityReason = qobject_cast<HbApplication*>(qApp)->activateReason();
    
    // Check if calendar is launched thru XQService framework
    bool isFromServiceFrmWrk = XQServiceUtil::isService(); // Since activateReason 
    //of hbapplication is not returning right value if the activity is started 
    //as services so using the above line temporarily untill a fix is available in 
    // hbappliacation. Need to remove this line after the fix is available for hbapplcation

    
    if (Hb::ActivationReasonActivity == activityReason) // Check if application is started 
    // as an activity
        {
        // Application is started from an activity
        // Extract activity data
        QVariant data = qobject_cast<HbApplication*>(qApp)->activateData();
        // Restore state from activity data
        QByteArray serializedModel = data.toByteArray();
        QDataStream stream(&serializedModel, QIODevice::ReadOnly);
        int viewId; // int declared for debugging purpose
        stream >> viewId; // read stream into an int
        
        mFirstView = viewId;
        if (ECalenMonthView == viewId) // Check if Activity was stored for month view
            {
            loadMonthView(); // Load month view
            }
        else if (ECalenAgendaView == viewId) // Check if Activity was stored for agenda view
            {
        	loadAgendaView(); // Load agenda view
            }
        
        ActivateDefaultViewL(viewId);
        // Connect to the view ready signal so that we can construct other views 
        // once this view is ready
        connect(&mController.MainWindow(), SIGNAL(viewReady()), 
                this, SLOT(handleMainViewReady()));

        if (ECalenMonthView == viewId) // Check if Activity was stored for month view
            {
            mController.MainWindow().addView(mCalenMonthView); // Add month view to main window
            mController.MainWindow().setCurrentView(mCalenMonthView); // Set month view as current view
            } 
        else if (ECalenAgendaView == viewId) // Check if Activity was stored for agenda view
            {
            mController.MainWindow().addView(mCalenAgendaView); // Add agenda view to main window
            mController.MainWindow().setCurrentView(mCalenAgendaView); // Set agenda view as current view
            }
        } else if (isFromServiceFrmWrk/*Hb::ActivationReasonService == activityReason*/) {
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
    

    OstTraceFunctionExit0( CALENVIEWMANAGER_SECONDPHASECONSTRUCTION_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::~CalenViewManager
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenViewManager::~CalenViewManager()
{
    OstTraceFunctionEntry0( DUP1_CALENVIEWMANAGER_CALENVIEWMANAGER_ENTRY );
    
	if (mSettingsView) {
		delete mSettingsView;
		mSettingsView = 0;
	}
	if (mAgendaViewDocLoader) {
		delete mAgendaViewDocLoader;
		mAgendaViewDocLoader = 0;
	}
	if (mAgendaViewAltDocLoader) {
		delete mAgendaViewAltDocLoader;
		mAgendaViewAltDocLoader = 0;
	}
	if (mMonthViewDocLoader) {
		delete mMonthViewDocLoader;
		mMonthViewDocLoader = 0;
	}
	
	OstTraceFunctionExit0( DUP1_CALENVIEWMANAGER_CALENVIEWMANAGER_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::constructAndActivateView
// Constructs and activates the requested view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::constructAndActivateView(int view)
{
	OstTraceFunctionEntry0( CALENVIEWMANAGER_CONSTRUCTANDACTIVATEVIEW_ENTRY );
	
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
	} else if (view == ECalenAgendaView) {
		mFirstView = ECalenAgendaView;
		loadAgendaView();
		ActivateDefaultViewL(ECalenAgendaView);
		// Add agenda view to mainwindow.
		mController.MainWindow().addView(mCalenAgendaView);
		mController.MainWindow().setCurrentView(mCalenAgendaView);
	}
	OstTraceFunctionExit0( CALENVIEWMANAGER_CONSTRUCTANDACTIVATEVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::loadMonthView
// Loads month view frm the docml
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::loadMonthView()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_LOADMONTHVIEW_ENTRY );
    
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
	
	OstTraceFunctionExit0( CALENVIEWMANAGER_LOADMONTHVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::loadAgendaView
// Loads the agenda view frm the docml
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::loadAgendaView()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_LOADAGENDAVIEW_ENTRY );
    
	bool loadSuccess = false;
	// Create the agenda view docloader object.
	mAgendaViewDocLoader = new CalenDocLoader(mController);
	
	// Load default section
	mAgendaViewDocLoader->load(CALEN_AGENDAVIEW_XML_FILE, &loadSuccess);
	if (!loadSuccess) {
		qFatal("calenviewmanager.cpp : Unable to load XML");
	}
	
	// Get the calenagenda view from the loader
	mCalenAgendaView = static_cast<CalenAgendaView *> (mAgendaViewDocLoader->findWidget(CALEN_AGENDAVIEW));
	if (!mCalenAgendaView) {
		qFatal("calenviewmanager.cpp : Unable to find agenda view");
	}
	
	// Setup the agenda view
	mCalenAgendaView->setupView(mAgendaViewDocLoader);
	
	// The following code is done to provide swipe support
	// in agenda view. Idea is to create two views and keep
	// switiching between these two using view switch effects
	// to provide an illusion of flow to the user
	loadAlternateAgendaView();
	
	OstTraceFunctionExit0( CALENVIEWMANAGER_LOADAGENDAVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleMainViewReady
// Slot to handle viewReady() signal from mainwindow
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleMainViewReady()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_HANDLEMAINVIEWREADY_ENTRY );
    
	// Construct the month view part that is kept for lazy loading
	if (mCalenMonthView) {
		mCalenMonthView->doLazyLoading();
	}
	
	// Construct other views
	constructOtherViews();
	
    // Install the event filter for the controller once the view is ready
    // so that system language/locale changes can be handled
	//hbInstance->allMainWindows().first()->installEventFilter(&mController);
	
	// disconnect the view ready signal as we dont need it anymore
	disconnect(&mController.MainWindow(), SIGNAL(viewReady()), 
			   this, SLOT(handleMainViewReady()));
	
	OstTraceFunctionExit0( CALENVIEWMANAGER_HANDLEMAINVIEWREADY_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::constructOtherViews
// Constructs the other views apart frm firstview and adds them to main window
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::constructOtherViews()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_CONSTRUCTOTHERVIEWS_ENTRY );
    
	// Load all other views except mFirstView
	
	// NOTE: Right now, since Calendar has only two views, month view 
	// and agenda view, when client launches agenda view, then there is no need
	// to construct the month view as per UI REQ., but tomorrow if new views
	// come after agenda view, then we need to construct those views if they are
	// native views. Right now, there is a event viewer but its not a native
	// view. Hence, if agenda view is launched, dont construct month view
	if (mFirstView != ECalenAgendaView) // check if agenda view is not already loaded
		{
		// Load all other views 
		loadAgendaView();
		
		if (!mCalenDayView) {
			mCalenDayView = new CalenDayView(mController.Services());
		}
	}
	else //agenda view was launched as first view
	    {
		// No implementation yet. UI specs not clear
		// to be commented in with some more code once UI specs is frozen
		// for agenda view launching as first view after it was saved as activity
		// when it was launched from month view
        // loadMonthView();
        // mCalenMonthView->doLazyLoading();
	    }

	// Setup the settings view
	mSettingsView = new CalenSettingsView(mController.Services());
	
	OstTraceFunctionExit0( CALENVIEWMANAGER_CONSTRUCTOTHERVIEWS_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::getFirstView
// Returns the first view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
int CalenViewManager::getFirstView()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_GETFIRSTVIEW_ENTRY );
    
	OstTraceFunctionExit0( CALENVIEWMANAGER_GETFIRSTVIEW_EXIT );
	return mFirstView;
}

// ----------------------------------------------------------------------------
// CalenViewManager::showNextDay
// other items were commented in a header
// ----------------------------------------------------------------------------
//
void CalenViewManager::showNextDay()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_SHOWNEXTDAY_ENTRY );
    
    // Set the context for the next day
    QDateTime currentDay = mController.Services().Context().focusDateAndTime();
    currentDay = currentDay.addDays(1);
    mController.Services().Context().setFocusDate(currentDay);
    mCurrentViewId = ECalenAgendaView;
    // Check which is the currently activated view
    if (mController.MainWindow().currentView() == mCalenAgendaView) {
        mCalenAgendaView->disconnectAboutToQuitEvent(); // disconnect mCalenAgendaView to get aboutToQuit Events
        HbEffect::add(mCalenAgendaView,
                      ":/fxml/view_hide",
                      "hide");
        HbEffect::add(mCalenAgendaViewAlt,
                      ":/fxml/view_show",
                      "show");
        // Set the other agenda view as the current view
        // and animate to provide illusion of swipe
		// It would also connect for aboutToQuit events
        mCalenAgendaViewAlt->doPopulation();
        mController.MainWindow().setCurrentView(mCalenAgendaViewAlt, true, Hb::ViewSwitchUseNormalAnim);
    } else {
    mCalenAgendaViewAlt->disconnectAboutToQuitEvent(); // disconnect mCalenAgendaViewAlt to get aboutToQuit Events
        HbEffect::add(mCalenAgendaViewAlt,
                      ":/fxml/view_hide",
                      "hide");
        HbEffect::add(mCalenAgendaView,
                      ":/fxml/view_show",
                      "show");
		// It would also connect for aboutToQuit events
        mCalenAgendaView->doPopulation();
        mController.MainWindow().setCurrentView(mCalenAgendaView, true, Hb::ViewSwitchUseNormalAnim);
    }
    OstTraceFunctionExit0( CALENVIEWMANAGER_SHOWNEXTDAY_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::showPrevDay
// other items were commented in a header
// ----------------------------------------------------------------------------
//
void CalenViewManager::showPrevDay()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_SHOWPREVDAY_ENTRY );
    
    QDateTime currentDay = mController.Services().Context().focusDateAndTime();
    currentDay = currentDay.addDays(-1);
    mController.Services().Context().setFocusDate(currentDay);
    mCurrentViewId = ECalenAgendaView;
    if (mController.MainWindow().currentView() == mCalenAgendaView) {
    mCalenAgendaView->disconnectAboutToQuitEvent(); // disconnect mCalenAgendaView to get aboutToQuit Events
        HbEffect::add(mCalenAgendaView,
                      ":/fxml/view_hide_back",
                      "hide");
        HbEffect::add(mCalenAgendaViewAlt,
                      ":/fxml/view_show_back",
                      "show");
		// It would also connect for aboutToQuit events
        mCalenAgendaViewAlt->doPopulation();
        mController.MainWindow().setCurrentView(mCalenAgendaViewAlt, true, Hb::ViewSwitchUseNormalAnim);
    } else {
    mCalenAgendaViewAlt->disconnectAboutToQuitEvent(); // disconnect mCalenAgendaViewAlt to get aboutToQuit Events
        HbEffect::add(mCalenAgendaViewAlt,
                      ":/fxml/view_hide_back",
                      "hide");
        HbEffect::add(mCalenAgendaView,
                      ":/fxml/view_show_back",
                      "show");
		// It would also connect for aboutToQuit events
        mCalenAgendaView->doPopulation();
        mController.MainWindow().setCurrentView(mCalenAgendaView, true, Hb::ViewSwitchUseNormalAnim);
    }
    
    OstTraceFunctionExit0( CALENVIEWMANAGER_SHOWPREVDAY_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::removePreviousView
// Remove the previous view from main window
// ----------------------------------------------------------------------------
//
void CalenViewManager::removePreviousView()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_REMOVEPREVIOUSVIEW_ENTRY );
    
	if (ECalenAgendaView == mCurrentViewId) {
		mCalenAgendaView->clearListModel();
		mCalenAgendaViewAlt->clearListModel();
		mController.MainWindow().removeView(mCalenAgendaView);
		mController.MainWindow().removeView(mCalenAgendaViewAlt);
	} else if (ECalenDayView == mCurrentViewId) {
		mController.MainWindow().removeView(mCalenDayView);
	} else {
		if (ECalenMonthView == mCurrentViewId) {
			mController.MainWindow().removeView(mCalenMonthView);
		}
	}

	OstTraceFunctionExit0( CALENVIEWMANAGER_REMOVEPREVIOUSVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::~ActivateDefaultViewL
//  Activates the default view, as retrieved from settings.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::ActivateDefaultViewL(int defaultView)
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_ACTIVATEDEFAULTVIEWL_ENTRY );
    
	mCurrentViewId = defaultView;
	if (ECalenMonthView == defaultView) {
		mCalenMonthView->doPopulation();
	} else if (ECalenAgendaView == defaultView) {
		mCalenAgendaView->doPopulation();
	}
	OstTraceFunctionExit0( CALENVIEWMANAGER_ACTIVATEDEFAULTVIEWL_EXIT );
}

// ----------------------------------------------------------------------------
// Refresh current view.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
void CalenViewManager::refreshCurrentViewL()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_REFRESHCURRENTVIEWL_ENTRY );
    
	activateCurrentView();
	
	OstTraceFunctionExit0( CALENVIEWMANAGER_REFRESHCURRENTVIEWL_EXIT );
}

// ----------------------------------------------------------------------------
// Activate current view.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
void CalenViewManager::activateCurrentView()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_ACTIVATECURRENTVIEW_ENTRY );
    
	switch (mCurrentViewId) {
		case ECalenMonthView:
		    mCalenMonthView->doPopulation();
		    mController.MainWindow().setCurrentView(mCalenMonthView);
			break;
		case ECalenAgendaView:
		    if (mController.MainWindow().currentView() == mCalenAgendaView) {
		        // This happens when settings view or event viewer is opened
		        // from the agenda view. Simply repopulate the view
		    	if (mCalenAgendaView) {
		    		mCalenAgendaView->doPopulation();
					mController.MainWindow().setCurrentView(mCalenAgendaView);
		    	} 
		    } else if (mController.MainWindow().currentView() == mCalenAgendaViewAlt){
		        // This happens when settings view or event viewer is opened
		        // from the agenda view. Simply repopulate the view
		    	if (mCalenAgendaViewAlt) {
		    		mCalenAgendaViewAlt->doPopulation();
		    		mController.MainWindow().setCurrentView(mCalenAgendaViewAlt);
		    	}
		    } else {
		        // This is called whenever the agenda view is opened from the month
		        // view. Since the agenda view is not added to the mainwindow,
		        // add the agenda views to mainwindow and set any one of them as 
		        // current view
		    	if (mCalenAgendaView) {
		    		mCalenAgendaView->doPopulation();
		    		mController.MainWindow().addView(mCalenAgendaView);
		    		mController.MainWindow().setCurrentView(mCalenAgendaView);
		    		mController.MainWindow().addView(mCalenAgendaViewAlt);
		    	}
		    }
			break;
		case ECalenShowSettings:
		    mSettingsView->refreshView();
			break;
		case ECalenDayView:
			mCalenDayView->doPopulation();
			mController.MainWindow().setCurrentView(mCalenDayView);
			break;
	}
	
	OstTraceFunctionExit0( CALENVIEWMANAGER_ACTIVATECURRENTVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::launchEventView
// Launch event view.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::launchEventView()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_LAUNCHEVENTVIEW_ENTRY );
    
    // capture cureent view in case app closed/quits from AgendaEventViewer
    if (mCalenMonthView) {
    mCalenMonthView->captureScreenshot(true);
    }
    else if (mCalenAgendaView) {
    	mCalenAgendaView->captureScreenshot(true);
    }
	MCalenContext& context = mController.Services().Context();
	AgendaEntry viewEntry= mController.Services().agendaInterface()->fetchById(
			context.instanceId().mEntryLocalUid );
	if (viewEntry.isRepeating() 
			&& viewEntry.type() != AgendaEntry::TypeTodo) {
		QDateTime startDateTime = context.focusDateAndTime();
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
	OstTraceFunctionExit0( CALENVIEWMANAGER_LAUNCHEVENTVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::loadAlternateAgendaView
// other items were commented in a header
// ----------------------------------------------------------------------------
//
void CalenViewManager::loadAlternateAgendaView()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_LOADALTERNATEAGENDAVIEW_ENTRY );
    
    bool loadSuccess = false;
    // Create the agenda view docloader object.
    mAgendaViewAltDocLoader = new CalenDocLoader(mController);

    // Load default section
    mAgendaViewAltDocLoader->load(CALEN_AGENDAVIEW_XML_FILE, &loadSuccess);
    if (!loadSuccess) {
        qFatal("calenviewmanager.cpp : Unable to load XML");
    }

    // Get the calenagenda view from the loader
    mCalenAgendaViewAlt = static_cast<CalenAgendaView *> (mAgendaViewAltDocLoader->findWidget(CALEN_AGENDAVIEW));
    if (!mCalenAgendaViewAlt) {
        qFatal("calenviewmanager.cpp : Unable to find alternate agenda view");
    }

    // Setup the agenda view
    mCalenAgendaViewAlt->setupView(mAgendaViewAltDocLoader);
    OstTraceFunctionExit0( CALENVIEWMANAGER_LOADALTERNATEAGENDAVIEW_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::HandleCommandL
// Handles view manager commands.
// @return ETrue if command is handled, EFalse otherwise
// ----------------------------------------------------------------------------
//
TBool CalenViewManager::HandleCommandL(const TCalenCommand& command)
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_HANDLECOMMANDL_ENTRY );
    
	TBool commandUsed(EFalse);
	
	switch (command.Command()) {
		case ECalenMonthView:
			// First remove the previous native view before 
			// we set the mCurrentViewId with the current view
			removePreviousView();
			// Add month view to mainwindow.
			mController.MainWindow().addView(mCalenMonthView);
			mCurrentViewId = ECalenMonthView;
			activateCurrentView();
			break;
		case ECalenAgendaView:
			// First remove the previous native view before 
			// we set the mCurrentViewId with the current view
			removePreviousView();
			mCurrentViewId = ECalenAgendaView;
			activateCurrentView();
			break;
		case ECalenDayView:
            {
            // First add new view and use QueuedConnection to assure that
            // view is ready before setting it as the current view
			mController.MainWindow().addView(mCalenDayView);
			QMetaObject::invokeMethod(this, "handleDayViewReady", 
			    Qt::QueuedConnection);
			}
			break;
		case ECalenEventView:
			launchEventView();
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
	OstTraceFunctionExit0( CALENVIEWMANAGER_HANDLECOMMANDL_EXIT );
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
    OstTraceFunctionEntry0( CALENVIEWMANAGER_HANDLENOTIFICATION_ENTRY );
    
	switch (notification) {
		case ECalenNotifyExternalDatabaseChanged:
		case ECalenNotifyDialogClosed:
		case ECalenNotifyMultipleEntriesDeleted:
		case ECalenNotifyEntrySaved:
		case ECalenNotifyEntryDeleted:
		case ECalenNotifyInstanceDeleted:
		case ECalenNotifyEntryClosed:
		case ECalenNotifySystemLocaleChanged:
		case ECalenNotifySystemLanguageChanged: {
            activateCurrentView(); 
            if (mCalenMonthView) {
                mCalenMonthView->captureScreenshot();
            } else if (mCalenAgendaView) {
                mCalenAgendaView->captureScreenshot();
            }
		}
		    break;
		case ECalenNotifySettingsClosed: {
		    //when setting view closed , switch to the previous view
		    mCurrentViewId = mPreviousViewsId ;
			mController.Services().IssueCommandL(ECalenStartActiveStep);
			
			 // invalidate captured screenshots as either agenda view is activated now
			if (mCalenMonthView) {
			mCalenMonthView->captureScreenshot();
			} else if (mCalenAgendaView) {
				mCalenAgendaView->captureScreenshot();
			}

		    }
			break;
		default:
			break;
	}
	OstTraceFunctionExit0( CALENVIEWMANAGER_HANDLENOTIFICATION_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::settingsView
//  Returns the settings view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenSettingsView* CalenViewManager::settingsView()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_SETTINGSVIEW_ENTRY );
    
	OstTraceFunctionExit0( CALENVIEWMANAGER_SETTINGSVIEW_EXIT );
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
	OstTraceFunctionEntry0( CALENVIEWMANAGER_HANDLEVIEWINGCOMPLETED_ENTRY );
	
	// Cleanup.
	mCalenEventViewer->deleteLater();
	mController.Services().IssueNotificationL(ECalenNotifyEntryClosed);
	
	// invalidate captured screenshots as either agenda view is activated now
	if (mCalenMonthView) {
	mCalenMonthView->captureScreenshot();
	} else if (mCalenAgendaView) {
		mCalenAgendaView->captureScreenshot();
	}
	OstTraceFunctionExit0( CALENVIEWMANAGER_HANDLEVIEWINGCOMPLETED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleEditingStarted
//  Slot to handle signal editingStarted by the agenda event viewer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleEditingStarted()
{
	OstTraceFunctionEntry0( CALENVIEWMANAGER_HANDLEEDITINGSTARTED_ENTRY );
	
	mController.IssueCommandL(ECalenEditEntryFromViewer);
	
	OstTraceFunctionExit0( CALENVIEWMANAGER_HANDLEEDITINGSTARTED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleEditingCompleted
//  Slot to handle signal editingCompleted by the agenda event viewer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleEditingCompleted()
{
	OstTraceFunctionEntry0( CALENVIEWMANAGER_HANDLEEDITINGCOMPLETED_ENTRY );
	
	mController.Services().IssueNotificationL(ECalenNotifyEditorClosedFromViewer);
	
	OstTraceFunctionExit0( CALENVIEWMANAGER_HANDLEEDITINGCOMPLETED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleDeletingStarted
//  Slot to handle signal deletingStarted by the agenda event viewer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleDeletingStarted()
{
	OstTraceFunctionEntry0( CALENVIEWMANAGER_HANDLEDELETINGSTARTED_ENTRY );
	
	mController.IssueCommandL(ECalenDeleteEntryFromViewer);
	
	OstTraceFunctionExit0( CALENVIEWMANAGER_HANDLEDELETINGSTARTED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleDeletingCompleted
//  Slot to handle signal deletingCompleted by the agenda event viewer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleDeletingCompleted()
{
	OstTraceFunctionEntry0( CALENVIEWMANAGER_HANDLEDELETINGCOMPLETED_ENTRY );
	
	mController.Services().IssueNotificationL(ECalenNotifyEntryDeleted);

	// invalidate captured screenshots as either month view or agenda view is activated now
    if (mCalenMonthView) {
    mCalenMonthView->captureScreenshot();
    } else if (mCalenAgendaView) {
    	mCalenAgendaView->captureScreenshot();
    }
	
    OstTraceFunctionExit0( CALENVIEWMANAGER_HANDLEDELETINGCOMPLETED_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleInstanceViewCreation
//  Slot to handle completion of instance view creation
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleInstanceViewCreation(int status)
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_HANDLEINSTANCEVIEWCREATION_ENTRY );
    
	Q_UNUSED(status);
	// handleInstanceViewCreation function is called only once. Now that the instance
	// view creation is successfull. Events need to be populated on screen
	// Ideal colution should be to call a uniform function, e.g. PopulateEvents
	// where PopulateEvents should be implemeted by all views. Since the current
	// solution for the month view implements the construction in two phases so 
	// it needs to be refactored and a common solution needs to be put here. So 
	// that code doesn't break if another view is added tomorow
	if (mCalenMonthView) {
		mCalenMonthView->fetchEntriesAndUpdateModel();
	}
	else if (mCalenAgendaView) {
		mCalenAgendaView->doPopulation();
	}
	
	OstTraceFunctionExit0( CALENVIEWMANAGER_HANDLEINSTANCEVIEWCREATION_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleDeletingCompleted
//  Slot to handle completion of entry view creation
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleEntryViewCreation(int status)
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_HANDLEENTRYVIEWCREATION_ENTRY );
    
	// Nothing Yet
	Q_UNUSED(status);

	OstTraceFunctionExit0( CALENVIEWMANAGER_HANDLEENTRYVIEWCREATION_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleDayViewReady
// Launches day view when it is added to MainWindow and ready to be displayed
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleDayViewReady() 
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_HANDLEDAYVIEWREADY_ENTRY );
    
    // Removes current view
    // Notice: removing view should be done after new view is set as current to
    // avoid situation that there is no current view in application
    removePreviousView();
    
    // Sets and activates day view
    mCurrentViewId = ECalenDayView;
    activateCurrentView();
	OstTraceFunctionExit0( CALENVIEWMANAGER_HANDLEDAYVIEWREADY_EXIT );
}

// ----------------------------------------------------------------------------
// CalenViewManager::handleEntriesChanged
// this function will be called when someone else has changed the database
// ----------------------------------------------------------------------------
//
void CalenViewManager::handleEntriesChanged(QList<ulong> ids)
{
	// Update and refresh the view.
	activateCurrentView();
}

// ----------------------------------------------------------------------------
// CalenViewManager::launchSettingsView
// Launches settings view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CalenViewManager::launchSettingsView()
{
    OstTraceFunctionEntry0( CALENVIEWMANAGER_LAUNCHSETTINGSVIEW_ENTRY );
    
    mPreviousViewsId = mCurrentViewId ;  
    mCurrentViewId = ECalenShowSettings;
    mSettingsView->initializeForm();
    mController.Services().MainWindow().setCurrentView(mSettingsView);
    
    // capture cureent view in case app closed/quits from settings view
    if (mCalenMonthView){
    mCalenMonthView->captureScreenshot(true);
    } else if(mCalenAgendaView){
    	mCalenAgendaView->captureScreenshot(true);
    }
    
    OstTraceFunctionExit0( CALENVIEWMANAGER_LAUNCHSETTINGSVIEW_EXIT );
}

// End of file	--Don't remove this.
