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
 * Definition file for NotesNoteView class.
 *
 */

// System includes
#include <QDateTime>
#include <HbListView>
#include <HbListWidget>
#include <HbAction>
#include <HbMenu>
#include <HbAbstractViewItem>
#include <HbGroupBox>
#include <HbListViewItem>
#include <HbInstance>

// User includes
#include "notesnoteview.h"
#include "notescommon.h"
#include "notesdocloader.h"
#include "notesmodel.h"
#include "notessortfilterproxymodel.h"
#include "noteseditor.h"
#include "agendautil.h"


/*!
	\class NotesNoteView
	\brief The Note view of the notes application. Responsible for displaying
			the all notes.

	\sa NotesViewManager
 */

/*!
	Constructs the NotesNoteView object.

	\param parent The parent of type QGraphicsWidget.
 */
NotesNoteView::NotesNoteView(QGraphicsWidget *parent)
:HbView(parent),
 mSelectedItem(0),
 mDeleteAction(0)
 {
	// Nothing yet.
 }

/*!
	Destructor.
 */
NotesNoteView::~NotesNoteView()
{
	if (mDocLoader) {
		delete mDocLoader;
		mDocLoader = 0;
	}
}

/*!
	Called by the NotesViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The NotesAppController object.
	\param docLoader Pointer to NotesDocLoader object.
 */
void NotesNoteView::setupView(
		NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader)
{
	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;
	mNotesModel = mAppControllerIf->notesModel();
	mAgendaUtil = mAppControllerIf->agendaUtil();

	mProxyModel = new NotesSortFilterProxyModel(*mAgendaUtil, this);
	mProxyModel->setDynamicSortFilter(true);
	mProxyModel->setSourceModel(mNotesModel->sourceModel());

	mProxyModel->setFilterRole(NotesNamespace::TypeRole);
	mProxyModel->setFilterRegExp(QRegExp(QString("note")));

	NotesSortFilterProxyModel *subModel =
			new NotesSortFilterProxyModel(*mAgendaUtil, this);
	subModel->setDynamicSortFilter(true);
	subModel->setSourceModel(mProxyModel);

	// Get the list object from the document and update the model.
	mListView = static_cast<HbListView *>
	(mDocLoader->findWidget("noteListView"));
	// Update the list view model.
	mListView->setModel(subModel);
	// Setup the operations that can be done with a list view.
	connect(
			mListView, SIGNAL(released(const QModelIndex &)),
			this, SLOT(handleItemReleased(const QModelIndex &)));
	connect(
			mListView,
			SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
			this,
			SLOT(handleItemLongPressed(HbAbstractViewItem *, const QPointF &)));

	// Get the toolbar/menu actions.
	mAddNoteAction = static_cast<HbAction *> (
			mDocLoader->findObject("newNoteAction"));
	connect(
			mAddNoteAction, SIGNAL(triggered()),
			this, SLOT(createNewNote()));

	mAllNotesAction = static_cast<HbAction *> (
			mDocLoader->findObject("allNotesAction"));
	connect(
			mAllNotesAction, SIGNAL(triggered()),
			this, SLOT(displayAllNotesView()));

	mViewCollectionAction = static_cast<HbAction *> (
			mDocLoader->findObject("displayCollectionsAction"));
	mViewCollectionAction->setCheckable(true);
	mViewCollectionAction->setChecked(true);
	connect(
			mViewCollectionAction, SIGNAL(changed()),
			this, SLOT(handleActionStateChanged()));
	connect(
			mViewCollectionAction, SIGNAL(triggered()),
			this, SLOT(displayCollectionView()));

	// Handles the orientation change for list items
	HbMainWindow *window = hbInstance->allMainWindows().first();
	handleOrientationChanged(window->orientation());
	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(handleOrientationChanged(Qt::Orientation)));

	// Set the graphics size for the icons.
	HbListViewItem *prototype = mListView->listItemPrototype();
	prototype->setGraphicsSize(HbListViewItem::SmallIcon);
}

/*!
	Opens the note editor to create a new note.
 */
void NotesNoteView::createNewNote()
{
	// Here we Display an editor to the use to enter text.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));
	mNotesEditor->create(NotesEditor::CreateNote);
}

/*!
	Handles the pressing of a list item in the view.

	Here we open the editor for viewing/editing.

	\param index Reference to the QModelIndex representing the view item.
	\sa HbAbstractViewItem
 */
void NotesNoteView::handleItemReleased(const QModelIndex &index)
{
	// Sanity check.
	if (!index.isValid()) {
		return;
	}

	// First get the id of the note and get the corresponding information from
	// agendautil.
	ulong noteId = index.data(NotesNamespace::IdRole).value<qulonglong>();

	if (0 >= noteId) {
		// Something wrong.
		return;
	}

	// Get the entry details.
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.isNull()) {
		// Entry invalid.
		return;
	}

	// Now launch the editor with the obtained info.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));
	mNotesEditor->edit(entry);
}

/*!
	Displays a list item specific context menu.

	\param item The HbAbstractViewItem that was long pressed.
	\param coords The position where mouse was pressed.

	\sa HbAbstractViewItem, HbListView, HbMenu.
 */
void NotesNoteView::handleItemLongPressed(
		HbAbstractViewItem *item, const QPointF &coords)
{
	mSelectedItem = item;

	ulong noteId = item->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	// Display a context specific menu.
	HbMenu *contextMenu = new HbMenu();

	// Add actions to the context menu.
	mOpenAction =
			contextMenu->addAction(hbTrId("txt_common_menu_open"));
	connect(
			mOpenAction, SIGNAL(triggered()),
			this, SLOT(openNote()));

	mDeleteAction =
			contextMenu->addAction(hbTrId("txt_common_menu_delete"));
	connect(
			mDeleteAction, SIGNAL(triggered()),
			this, SLOT(deleteNote()));

	if (AgendaEntry::TypeNote == entry.type()) {
		if (entry.favourite()) {
			mMakeFavouriteAction =
					contextMenu->addAction(
							hbTrId("txt_notes_menu_remove_from_favorites"));

			connect(
					mMakeFavouriteAction, SIGNAL(triggered()),
					this, SLOT(markNoteAsFavourite()));

		} else {
			mMakeFavouriteAction =
					contextMenu->addAction(
							hbTrId("txt_notes_menu_mark_as_favorite"));

			connect(
					mMakeFavouriteAction, SIGNAL(triggered()),
					this, SLOT(markNoteAsFavourite()));
		}
		mMarkTodoAction = contextMenu->addAction(
				hbTrId("txt_notes_menu_make_it_as_todo_note"));
		connect(
				mMarkTodoAction, SIGNAL(triggered()),
				this, SLOT(markNoteAsTodo()));
	}

	// Show the menu.
	contextMenu->exec(coords);
}

/*!
	Slot to delete a selected note.
 */
void NotesNoteView::deleteNote()
{
	Q_ASSERT(mSelectedItem);

	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		return;
	}
	ulong noteId =
			index.data(NotesNamespace::IdRole).value<qulonglong>();
	if (!noteId) {
		return;
	}
	// Emitting the signal , deletion would be handle in view manager.
	emit deleteEntry(noteId);

	mSelectedItem = 0;
}

/*!
    Marks/unmarks the note as favourite.
 */
void NotesNoteView::markNoteAsFavourite()
{
	ulong noteId = mSelectedItem->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.favourite()) {
		entry.setFavourite(0);
	} else {
		entry.setFavourite(1);
	}
	mAgendaUtil->updateEntry(entry);
}

/*!
    Slot to make a note as to-do.
 */
void NotesNoteView::markNoteAsTodo()
{
	Q_ASSERT(mSelectedItem);

	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		return;
	}
	ulong noteId =
			index.data(NotesNamespace::IdRole).value<qulonglong>();
	if (!noteId) {
		return;
	}
	// Get the entry details.
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.isNull()) {
		// Entry invalid.
		return;
	}

	// Here change the type of modified note and destroy the noteeditor and
	// construct the to-do editor.
	entry.setType(AgendaEntry::TypeTodo);

	QDateTime dueDateTime;
	QDate currentDate(QDate::currentDate());
	dueDateTime.setDate(
			QDate(currentDate.year(),currentDate.month(),currentDate.day()+1));
	dueDateTime.setTime(QTime::fromString("12:00 am", "hh:mm ap"));

	entry.setStartAndEndTime(dueDateTime, dueDateTime);

	entry.setSummary(entry.description().left(80));

	if (80 > entry.description().length()) {
		entry.setDescription("");
	}

	// Remove favourite if marked so.
	entry.setFavourite(0);

	// Set the status of the to-do.
	entry.setStatus(AgendaEntry::TodoNeedsAction);

	// First clone the todoEntry for the new type.
	mAgendaUtil->cloneEntry(
			entry, AgendaEntry::TypeTodo);

	// Delete the old entry.
	mAgendaUtil->deleteEntry(entry.id());

}

/*!
	Slot to handle the signal editingCompleted by the notes editor.

	\param status Boolean value indicating whether the note was saved or not.

	\sa NotesEditor.
 */
void NotesNoteView::handleEditingCompleted(bool status)
{
	Q_UNUSED(status)

	// Cleanup.
	mNotesEditor->deleteLater();
}

/*!
	Directs the view manager to display the Collections view.
 */
void NotesNoteView::displayCollectionView()
{
	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesCollectionViewId);

}

/*!
	Directs the view manager to display the All notes view.
 */
void NotesNoteView::displayAllNotesView()
{
	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesMainViewId);
}

/*!
	Slot to handle the case when the state of an action has changed.
 */
void NotesNoteView::handleActionStateChanged()
{
	mAllNotesAction->setChecked(true);
}

/*!
	Handles the orientation changes.Updates the list
	item when orientation is changed

	\param orientation Value of the orientation
 */
void NotesNoteView::handleOrientationChanged(Qt::Orientation orientation)
{
	HbListViewItem *prototype = mListView->listItemPrototype();

	if (Qt::Horizontal == orientation) {
		prototype->setStretchingStyle(HbListViewItem::StretchLandscape);
	} else {
		prototype->setStretchingStyle(HbListViewItem::NoStretching);
	}
}

/*
	Opens the notes editor to edit the note.
 */
void NotesNoteView::openNote()
{
	ulong noteId = mSelectedItem->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();

	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	// Construct notes editor.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));

	// Launch the notes editor with the obtained info.
	mNotesEditor->edit(entry);
}
// End of file	--Don't remove this.

