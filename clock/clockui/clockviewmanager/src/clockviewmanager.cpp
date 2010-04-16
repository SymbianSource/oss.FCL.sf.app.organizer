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
#include <QDebug>
#include <HbMainWindow>
#include <HbInstance>

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
 mAppControllerIf(controllerIf)
{
	qDebug("clock: ClockViewManager::ClockViewManager() -->");

	// Load the document and the views.
	loadViews();

	qDebug("clock: ClockViewManager::ClockViewManager() <--");
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
			window->removeView(window->currentView());
			window->addView(mMainView);
			window->setCurrentView(mMainView);
			break;

		case WorldClock:
			window->removeView(window->currentView());
			window->addView(mWorldClockView);
			window->setCurrentView(mWorldClockView);
			break;

		default:
			break;
	}
}

/*!
	Loads the views from the docml file.
 */
void ClockViewManager::loadViews()
{
	qDebug() << "clock: ClockViewManager::loadViews -->";

	// Load the main view.
	loadMainView();
	// Load the world clock view.
	loadWorldClockView();

	// Set the main view to the window
	hbInstance->allMainWindows().first()->addView(mMainView);

	qDebug() << "clock: ClockViewManager::loadViews <--";
}

/*!
	Loads the clock main view.
 */
void ClockViewManager::loadMainView()
{
	qDebug() << "clock: ClockViewManager::loadMainView -->";

	bool loadSuccess;

	// Construct the document loader instance
	ClockDocLoader *docLoader = new ClockDocLoader();

	// Load the application xml.
	docLoader->load(CLOCK_MAIN_VIEW_DOCML, &loadSuccess);
	Q_ASSERT_X(
			loadSuccess,
			"viewmanager.cpp",
			"Unable to load the main view app xml");

	// Find the main view.
	mMainView = static_cast<ClockMainView *> (
			docLoader->findWidget(CLOCK_MAIN_VIEW));

	// Setup the view.
	mMainView->setupView(mAppControllerIf, docLoader);

	qDebug() << "clock: ClockViewManager::loadMainView <--";
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
	docLoader->load(":/xml/worldclockview.docml", &loadSuccess);

	// Get the world list view.
	mWorldClockView =
			static_cast<ClockWorldView *> (docLoader->findWidget("worldClockView"));

	mWorldClockView->setupView(mAppControllerIf, docLoader);

/*	// Get the world list widget.
	WorldListWidget *listWidget =
			qobject_cast<WorldListWidget *> (docLoader->findWidget("worldListWidget"));
	Q_ASSERT_X(listWidget,
	           "viewmanager.cpp", "Unable to load the world list widget");
	listWidget->setupWidget(mWorldListView, docLoader);*/
}

// End of file	--Don't remove this.
