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
* Definition file for class ClockViewManager.
*
*/

// System includes
#include <HbMainWindow>
#include <HbInstance>
#include <HbApplication>
#include <HbActivityManager>
#include <HbToolBar>

// User includes
#include "clockviewmanager.h"
#include "clockdocloader.h"
#include "clockmainview.h"
#include "clockcommon.h"
#include "clockdatatypes.h"
#include "clockworldview.h"

/*!
	\class ClockViewManager

	This is the heart of the clock application. This is responsible for
	managing the clock views.
 */

/*!
	Constructor.

	\param controller A reference to the object of ClockAppController.
 */
ClockViewManager::ClockViewManager(
		ClockAppControllerIf &controllerIf, QObject *parent)
:QObject(parent),
 mAppControllerIf(controllerIf),
 mWorldClockView(0)
{
    // Activity Reason from Activity Manager
    int activityReason = qobject_cast<HbApplication*>(qApp)->activateReason();
    
    if (Hb::ActivationReasonActivity == activityReason) {
        // Application is started from an activity
		// extract activity data
        QVariant data = qobject_cast<HbApplication*>(qApp)->activateData();
        // restore state from activity data
        QByteArray serializedModel = data.toByteArray();
        QDataStream stream(&serializedModel, QIODevice::ReadOnly);
        int activityId;
        stream >> activityId;
        
        if (MainView == activityId) {
            // Load the main view at the start up.
            loadMainView();
			// Delay loading of other views till main view is loaded.
        	HbMainWindow *window = hbInstance->allMainWindows().first();
        	connect(
                window, SIGNAL(viewReady()),
                this, SLOT(loadOtherViews()));

        }
        else if (WorldClock == activityId) {
            //no implentation yet, UI specs are not clear
        }

    }
    else {
        // Load the main view at the start up.
        loadMainView();
        // Delay loading of other views till main view is loaded.
        HbMainWindow *window = hbInstance->allMainWindows().first();
        connect(
                window, SIGNAL(viewReady()),
                this, SLOT(loadOtherViews()));
    }

}

/*!
	Destructor.
 */
ClockViewManager::~ClockViewManager()
{
	// No implementation yet.
}

/*!
	Switches to view specified by view.

	\param view View Id.
 */
void ClockViewManager::showView(ClockViews view)
{
	HbMainWindow *window = hbInstance->allMainWindows().first();

	switch (view) {
		case MainView:
		    // set captured screenshot as invalid as main view 
			// is the current view
		    mMainView->captureScreenShot(false);
			window->removeView(window->currentView());
			window->addView(mMainView);
			window->setCurrentView(mMainView);
			break;

		case WorldClock:
		    if (mMainView) {
				// capture main view as the screenshot for future use 
				// to save the main view as an activity, if application is 
				// exited/Quit from world view
                mMainView->captureScreenShot(true);
		    }
			window->removeView(window->currentView());
			window->addView(mWorldClockView);
			window->setCurrentView(mWorldClockView);
			break;

		default:
			break;
	}
}

/*!
	Loads the clock main view.
 */
void ClockViewManager::loadMainView()
{
	bool loadSuccess;

	// Construct the document loader instance
	ClockDocLoader *docLoader = new ClockDocLoader();

	// Load the application xml.
	docLoader->load(CLOCK_MAIN_VIEW_DOCML, &loadSuccess);

	// Find the main view.
	mMainView = static_cast<ClockMainView *> (
			docLoader->findWidget(CLOCK_MAIN_VIEW));
	
	// Disable animation effect on toolbar.
	HbEffect::disable(mMainView->toolBar()->graphicsItem());

	// Setup the view.
	mMainView->setupView(mAppControllerIf, docLoader);

	// Set the main view to the window
	hbInstance->allMainWindows().first()->addView(mMainView);
}

/*!
	Loads the world clock view.
 */
void ClockViewManager::loadWorldClockView()
{
	// Construct the document loader instance
	ClockDocLoader *docLoader = new ClockDocLoader();

	bool loadSuccess;

	// Construct the world list view from doc loader.
	docLoader->load(CLOCK_WORLD_VIEW_DOCML, &loadSuccess);

	// Get the world list view.
	mWorldClockView = static_cast<ClockWorldView *> (
			docLoader->findWidget(CLOCK_WORLD_VIEW));
	
	// Disable animation effect on toolbar.
	HbEffect::disable(mWorldClockView->toolBar()->graphicsItem());

	mWorldClockView->setupView(mAppControllerIf, docLoader);
}

/*!
	Load other views
 */
void ClockViewManager::loadOtherViews()
{
	mMainView->setupAfterViewReady();
	
	// Load world clock view
	loadWorldClockView();

	// Disconnect the signal viewReady as all the views are loaded.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	disconnect(
			window, SIGNAL(viewReady()),
			this, SLOT(loadOtherViews()));
	
	// Need to emit this signal after the view is fully constructed & populated
	// with actual data and ready to be used. So entry view & instance view
	// needs to be created so that a new entry can also be created. Finally
	// NotesApplication object needs to emit applicationReady Signal.
	emit appReady();
}

// End of file	--Don't remove this.
