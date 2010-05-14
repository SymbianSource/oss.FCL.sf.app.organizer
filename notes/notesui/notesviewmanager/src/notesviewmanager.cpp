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
#include <qglobal.h>
#include <QtGui>
#include <HbMainWindow>
#include <HbView>
#include <HbInstance>
#include <HbListView>
#include <HbMessageBox>
#include <HbAction>

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
	HbMainWindow *window = hbInstance->allMainWindows().first();

	mAgendaUtil = mAppControllerIf.agendaUtil();

	connect(
			mAgendaUtil, SIGNAL(instanceViewCreationCompleted(int)),
			this,SLOT(handleInstanceViewCreationCompleted(int)));

	// Load the main view at the start up.
	loadNotesMainView();

	// Delay loading of other views till main view is loaded.
	connect(
			window, SIGNAL(viewReady()),
			this, SLOT(loadOtherViews()));
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
	Loads the notes main view.
 */
void NotesViewManager::loadNotesMainView()
{
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
	// Connect to main view signal for entry deletion.
	connect (
			mMainView, SIGNAL(deleteEntry(ulong)),
			this, SLOT(deleteEntryFromView(ulong)));

	// Set the main view to the window
	hbInstance->allMainWindows().first()->addView(mMainView);
}

/*!
	Loads the notes collection view.
 */
void NotesViewManager::loadNotesCollectionView()
{
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
}

/*!
	Loads the to-do view.
 */
void NotesViewManager::loadTodoView()
{

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
	// Connect to to-do view signal for entry deletion.
	connect (
			mTodoView, SIGNAL(deleteEntry(ulong)),
			this, SLOT(deleteEntryFromView(ulong)));
}

/*!
	Loads the favorites view.
 */
void NotesViewManager::loadFavoritesView()
{
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

	// Connect to favourite view signal for entry deletion.
	connect (
			mFavoriteView, SIGNAL(deleteEntry(ulong)),
			this, SLOT(deleteEntryFromView(ulong)));
}

/*!
	Loads the recent notes view.
 */
void NotesViewManager::loadNoteView()
{
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

	connect(
			mNoteView, SIGNAL(deleteEntry(ulong)),
			this, SLOT(deleteEntryFromView(ulong)));
}

/*!
	Loads other views from the docml file.
 */
void NotesViewManager::loadOtherViews()
{
	mMainView->setupAfterViewReady();

	// Load the collection view.
	loadNotesCollectionView();
	// Load the to-do view.
	loadTodoView();
	// Load the favorites view.
	loadFavoritesView();
	// Load the recent notes view.
	loadNoteView();

	// Disconnect the signal viewReady as all the views are loaded.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	disconnect(
			window, SIGNAL(viewReady()),
			this, SLOT(loadOtherViews()));
}

/*!
	 Delete the entry.
 */
void NotesViewManager::deleteEntryFromView(ulong entryId)
{
	mEntryId = entryId;
	HbMessageBox *confirmationQuery = new HbMessageBox(
			HbMessageBox::MessageTypeQuestion);
	confirmationQuery->setDismissPolicy(HbDialog::NoDismiss);
	confirmationQuery->setTimeout(HbDialog::NoTimeout);
	confirmationQuery->setIconVisible(true);

	QString displayText;
	QString x;
	AgendaEntry entry = mAgendaUtil->fetchById(entryId);
	if (AgendaEntry::TypeTodo == entry.type()) {
		displayText += hbTrId("txt_notes_info_delete_todo_note");
	} else {
		displayText += hbTrId("txt_notes_info_delete_note");
	}

	confirmationQuery->setText(displayText);

	// Remove the default actions.
	QList<QAction *> defaultActions = confirmationQuery->actions();
	for (int index=0;index<defaultActions.count();index++) {
		confirmationQuery->removeAction(defaultActions[index]);
	}
	defaultActions.clear();

	// Add delete and cancel actions
	mDeleteAction = new HbAction(hbTrId("txt_notes_button_dialog_delete"));
	mCancelAction = new HbAction(hbTrId("txt_common_button_cancel"));

	confirmationQuery->addAction(mDeleteAction);
	confirmationQuery->addAction(mCancelAction);

	confirmationQuery->open(this, SLOT(selectedAction(HbAction*)));
}

/*!
	Slot to handle the delete action
 */
void NotesViewManager::selectedAction(HbAction *action)
{
	if (action == mDeleteAction) {
		// Delete the given note.
		mAgendaUtil->deleteEntry(mEntryId);
	}
}

/*!
	Slot to handle instance view creation complete.
 */
void NotesViewManager::handleInstanceViewCreationCompleted(int status)
{
	Q_UNUSED(status)

	// Update the title for main view.
	mMainView->updateTitle();

	// Populate collections view.
	mCollectionView->populateListView();

	// Update the title for to-do view.
	mTodoView->updateTitle();
}
// End of file	--Don't remove this.
