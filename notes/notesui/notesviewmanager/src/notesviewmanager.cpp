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
* Description: Definition file for class NotesDocLoader.
*
*/

// System includes
#include <qdebug.h>
#include <qglobal.h>
#include <QtGui>
#include <HbMainWindow>
#include <HbView>
#include <HbInstance>
#include <HbListView>

// User includes
#include "notesviewmanager.h"
#include "notesmainview.h"
#include "notescollectionview.h"
#include "notestodoview.h"
#include "notesfavoriteview.h"
#include "notesnoteview.h"
#include "notesdocloader.h"
#include "notescommon.h"
#include "notesmodel.h"
#include "notessortfilterproxymodel.h"
#include "agendautil.h"

/*!
	\class NotesViewManager

	This is the heart of the notes application. This is responsible for
	managing the notes views and owning the agenda interface object.
	This also is the source for the notes list model.
 */
/*!
	Constructor.

	\param controller A reference to the object of NotesAppController.
 */
NotesViewManager::NotesViewManager(
		NotesAppControllerIf &controllerIf, QObject *parent)
:QObject(parent),
 mAppControllerIf(controllerIf)
{
	qDebug() << "notes: NotesViewManager::NotesViewManager -->";

	// The document loader and views.
	loadViews();

	qDebug() << "notes: NotesViewManager::NotesViewManager <--";
}

/*!
	Destructor.
 */
NotesViewManager::~NotesViewManager()
{
	// No implementation yet
}

/*!
	Switches to view specified by viewId.

	\param viewId View Id.
 */
void NotesViewManager::switchToView(NotesNamespace::NotesViewIds viewId)
{
	HbMainWindow *window = hbInstance->allMainWindows().first();

	switch (viewId) {
		case NotesNamespace::NotesMainViewId:
			window->removeView(window->currentView());
			window->addView(mMainView);
			window->setCurrentView(mMainView);
			break;

		case NotesNamespace::NotesCollectionViewId:
			window->removeView(window->currentView());
			window->addView(mCollectionView);
			window->setCurrentView(mCollectionView);
			break;

		case NotesNamespace::NotesTodoViewId:
			window->removeView(window->currentView());
			window->addView(mTodoView);
			window->setCurrentView(mTodoView);
			break;

		case NotesNamespace::NotesFavoritesViewId:
			window->removeView(window->currentView());
			window->addView(mFavoriteView);
			window->setCurrentView(mFavoriteView);
			break;

		case NotesNamespace::NotesNoteViewId:
			window->removeView(window->currentView());
			window->addView(mNoteView);
			window->setCurrentView(mNoteView);
			break;

		default:
			break;
	}
}

/*!
	Loads the views from the docml file.
 */
void NotesViewManager::loadViews()
{
	qDebug() << "notes: NotesViewManager::loadViews -->";

	// Load the main view.
	loadNotesMainView();
	// Load the collection view.
	loadNotesCollectionView();
	// Load the to-do view.
	loadTodoView();
	// Load the favorites view.
	loadFavoritesView();
	// Load the recent notes view.
	loadNoteView();

	// Set the main view to the window
	hbInstance->allMainWindows().first()->addView(mMainView);

	qDebug() << "notes: NotesViewManager::loadViews <--";
}

/*!
	Loads the notes main view.
 */
void NotesViewManager::loadNotesMainView()
{
	qDebug() << "notes: NotesViewManager::loadNotesMainView -->";

	bool loadSuccess;

	// Construct the document loader instance
	NotesDocLoader *docLoader = new NotesDocLoader();

	// Load the application xml.
	docLoader->load(NOTES_MAIN_VIEW_XML, &loadSuccess);
	Q_ASSERT_X(
			loadSuccess,
			"notesviewmanager.cpp",
			"Unable to load the main view app xml");

	// Find the main view.
	mMainView = static_cast<NotesMainView *> (
			docLoader->findWidget(NOTES_MAIN_VIEW));
	Q_ASSERT_X(
			mMainView, "notesviewmanager.cpp", "Unable to find the main view.");
	// Setup the view.
	mMainView->setupView(mAppControllerIf, docLoader);

	qDebug() << "notes: NotesViewManager::loadNotesMainView <--";
}

/*!
	Loads the notes collection view.
 */
void NotesViewManager::loadNotesCollectionView()
{
	qDebug("notes: NotesViewManager::loadNotesCollectionView -- Entry");

	bool loadSuccess;

	// Construct the document loader instance
	NotesDocLoader *docLoader = new NotesDocLoader();

	// Load the application xml.
	docLoader->load(NOTES_COLLECTION_VIEW_XML, &loadSuccess);

	// Find the collection view.
	mCollectionView = static_cast<NotesCollectionView *> (
			docLoader->findWidget(NOTES_COLLECTION_VIEW));
	// Setup the view.
	mCollectionView->setupView(mAppControllerIf, docLoader);

	qDebug() << "notes: NotesViewManager::loadNotesCollectionView <--";
}

/*!
	Loads the to-do view.
 */
void NotesViewManager::loadTodoView()
{
	qDebug("notes: NotesViewManager::loadTodoView -->");

	bool loadSuccess;

	// Construct the document loader instance
	NotesDocLoader *docLoader = new NotesDocLoader();

	// Load the application xml.
	docLoader->load(NOTES_TODO_VIEW_XML, &loadSuccess);

	// Find the to-do view.
	mTodoView = static_cast<NotesTodoView *> (
			docLoader->findWidget(NOTES_TODO_VIEW));
	// Setup the view.
	mTodoView->setupView(mAppControllerIf, docLoader);

	qDebug() << "notes: NotesViewManager::loadTodoView <--";
}

/*!
	Loads the favorites view.
 */
void NotesViewManager::loadFavoritesView()
{
	qDebug("notes: NotesViewManager::loadFavoritesView -->");

	bool loadSuccess;

	// Construct the document loader instance
	NotesDocLoader *docLoader = new NotesDocLoader();

	// Load the application xml.
	docLoader->load(NOTES_FAVORITES_VIEW_XML, &loadSuccess);

	// Find the favorites view.
	mFavoriteView = static_cast<NotesFavoriteView *> (
			docLoader->findWidget(NOTES_FAVORITES_VIEW));
	// Setup the view.
	mFavoriteView->setupView(mAppControllerIf, docLoader);

	qDebug() << "notes: NotesViewManager::loadFavoritesView <--";
}

/*!
	Loads the recent notes view.
 */
void NotesViewManager::loadNoteView()
{
	qDebug("notes: NotesViewManager::loadNoteView -->");

	bool loadSuccess;

	// Construct the document loader instance
	NotesDocLoader *docLoader = new NotesDocLoader();

	// Load the application xml.
	docLoader->load(NOTES_NOTE_VIEW_XML, &loadSuccess);

	// Find the note view.
	mNoteView = static_cast<NotesNoteView *> (
			docLoader->findWidget(NOTES_NOTE_VIEW));
	// Setup the view.
	mNoteView->setupView(mAppControllerIf, docLoader);

	qDebug() << "notes: NotesViewManager::loadNoteView <--";
}

// End of file	--Don't remove this.
