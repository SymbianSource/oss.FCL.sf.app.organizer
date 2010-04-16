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
* Definition file for NotesFavoriteView class.
*
*/

// System includes
#include <QDebug>
#include <QDateTime>
#include <HbListView>
#include <HbListWidget>
#include <HbAction>
#include <HbTextEdit>
#include <HbInstance>
#include <HbMainWindow>
#include <HbMenu>
#include <HbDialog>
#include <HbLabel>
#include <HbAbstractViewItem>
#include <HbGroupBox>
#include <HbListViewItem>

// User includes
#include "notesfavoriteview.h"
#include "notescommon.h"
#include "notesdocloader.h"
#include "agendautil.h"
#include "notesmodel.h"
#include "notessortfilterproxymodel.h"
#include "noteseditor.h"

/*!
	\class NotesFavoriteView
	\brief The main view of the notes application. Responsible for displaying
			notes and todos.

	\sa NotesViewManager
 */

/*!
	Constructs the NotesFavoriteView object.

	\param parent The parent of type QGraphicsWidget.
 */
NotesFavoriteView::NotesFavoriteView(QGraphicsWidget *parent)
:HbView(parent),
 mSelectedItem(0),
 mDeleteAction(0)
{
	qDebug() << "notes: NotesFavoriteView::NotesFavoriteView -->";

	// Nothing yet.

	qDebug() << "notes: NotesFavoriteView::NotesFavoriteView <--";
}

/*!
	Destructor.
 */
NotesFavoriteView::~NotesFavoriteView()
{
	qDebug() << "notes: NotesFavoriteView::~NotesFavoriteView -->";

	if (mDocLoader) {
	    delete mDocLoader;
	    mDocLoader = 0;
    }

	qDebug() << "notes: NotesFavoriteView::~NotesFavoriteView <--";
}

/*!
	Called by the NotesViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The NotesAppController object.
	\param docLoader Pointer to NotesDocLoader object.
 */
void NotesFavoriteView::setupView(
		NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader)
{
	qDebug() << "notes: NotesMainView::setupView -->";

	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;
	mNotesModel = mAppControllerIf->notesModel();
	mAgendaUtil = mAppControllerIf->agendaUtil();

	mProxyModel = new NotesSortFilterProxyModel(*mAgendaUtil, this);
	mProxyModel->setDynamicSortFilter(true);
	mProxyModel->setSourceModel(mNotesModel->sourceModel());

	// Apply the filter
	mProxyModel->setFilterRole(NotesNamespace::FavouriteRole);
	mProxyModel->setFilterRegExp(QRegExp(QString("favourites")));

	NotesSortFilterProxyModel *subModel =
			new NotesSortFilterProxyModel(*mAgendaUtil, this);
	subModel->setDynamicSortFilter(true);
	subModel->setSourceModel(mProxyModel);

	// Get the list object from the document and update the model.
	mListView = static_cast<HbListView *>
			(mDocLoader->findWidget("favoritesListView"));
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

	qDebug() << "notes: NotesMainView::setupView <--";
}

/*!
	Slot which gets called when `+ New note' action is triggered from the view
	toolbar. This is responsible for launching the editor to create a new note.
 */
void NotesFavoriteView::createNewNote()
{
	qDebug() << "notes: NotesFavoriteView::createNewNote -->";

	// Here we Display an editor to the use to enter text.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));
	mNotesEditor->create(NotesEditor::CreateNote);

	qDebug() << "notes: NotesFavoriteView::createNewNote <--";
}

/*!
	Handles the pressing of a list item in the view.

	Here we open the editor for viewing/editing.

	\param index Reference to the QModelIndex representing the view item.
	\sa HbAbstractViewItem
 */
void NotesFavoriteView::handleItemReleased(const QModelIndex &index)
{
	qDebug() << "notes: NotesFavoriteView::handleItemReleased -->";

	// Sanity check.
	if (!index.isValid()) {
		qDebug() << "notes: NotesFavoriteView::handleItemReleased <--";

		return;
	}

	// First get the id of the note and get the corresponding information from
	// agendautil.
	ulong noteId = index.data(NotesNamespace::IdRole).value<qulonglong>();

	if (0 >= noteId) {
		qDebug() << "notes: NotesFavoriteView::handleItemReleased <--";

		// Something wrong.
		return;
	}

	// Get the entry details.
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.isNull()) {
		qDebug() << "notes: NotesFavoriteView::handleItemReleased <--";

		// Entry invalid.
		return;
	}

	// Now launch the editor with the obtained info.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));
	mNotesEditor->edit(entry);

	qDebug() << "notes: NotesFavoriteView::handleItemReleased <--";
}

/*!
	Displays a list item specific context menu.

	\param item The HbAbstractViewItem that was long pressed.
	\param coords The position where mouse was pressed.

	\sa HbAbstractViewItem, HbListView, HbMenu.
 */
void NotesFavoriteView::handleItemLongPressed(
		HbAbstractViewItem *item, const QPointF &coords)
{
	qDebug() << "notes: NotesFavoriteView::handleItemLongPressed -->";

	mSelectedItem = item;

	// Get the entry of the selected item.
	ulong noteId = item->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	// Display a context specific menu.
	HbMenu *contextMenu = new HbMenu();

	// Add actions to the context menu.
	mDeleteAction =
			contextMenu->addAction(hbTrId("txt_common_menu_delete"));
	connect(
			mDeleteAction, SIGNAL(triggered()),
			this, SLOT(deleteNote()));

	mRemoveFavoriteAction =
			contextMenu->addAction(
					hbTrId("txt_notes_menu_remove_from_favorites"));

	connect(
			mRemoveFavoriteAction, SIGNAL(triggered()),
			this, SLOT(markNoteAsNotFavourite()));

	mMarkTodoAction =
			contextMenu->addAction(
					hbTrId("txt_notes_menu_make_it_as_todo_note"));
	connect(
			mMarkTodoAction, SIGNAL(triggered()),
			this, SLOT(markNoteAsTodo()));

	// Show the menu.
	contextMenu->exec(coords);

	qDebug() << "notes: NotesFavoriteView::handleItemLongPressed <--";
}

/*!
	Deletes the note.
 */
void NotesFavoriteView::deleteNote()
{
	qDebug() << "notes: NotesFavoriteView::deleteNote -->";

	Q_ASSERT(mSelectedItem);

	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		qDebug() << "notes: NotesFavoriteView::deleteNote <--";

		return;
	}
	ulong entryId =
			index.data(NotesNamespace::IdRole).value<qulonglong>();
	if (!entryId) {
		qDebug() << "notes: NotesFavoriteView::deleteNote <--";

		return;
	}

	// Delete the given note.
	mAgendaUtil->deleteEntry(entryId);

	mSelectedItem = 0;

	qDebug() << "notes: NotesFavoriteView::deleteNote <--";
}

/*!
	Removes the note from favorites.
 */
void NotesFavoriteView::markNoteAsNotFavourite()
{
	qDebug() << "notes : NotesFavoriteView::markNoteAsNotFavourite -->";

	ulong noteId = mSelectedItem->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.favourite()) {
		entry.setFavourite(0);
	}
	mAgendaUtil->updateEntry(entry);

	qDebug() << "notes : NotesFavoriteView::markNoteAsNotFavourite <--";
}

/*!
    Slot to make a note as to-do.
 */
void NotesFavoriteView::markNoteAsTodo()
{
	qDebug() << "notes : NotesFavoriteView::markNoteAsTodo -->";

	Q_ASSERT(mSelectedItem);

	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		qDebug() << "notes: NotesFavoriteView::markNoteAsTodo <--";

		return;
	}
	ulong noteId = index.data(NotesNamespace::IdRole).value<qulonglong> ();
	if (!noteId) {
		qDebug() << "notes: NotesFavoriteView::markNoteAsTodo <--";

		return;
	}
	// Get the entry details.
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.isNull()) {
		qDebug() << "notes: NotesFavoriteView::markNoteAsTodo <--";

		// Entry invalid.
		return;
	}

	// Here change the type of modified note and destroy the noteeditor and
	// construct the to-do editor.
	entry.setType(AgendaEntry::TypeTodo);

	QDateTime dueDateTime;
	dueDateTime.setDate(QDate::currentDate());
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
	mAgendaUtil->cloneEntry(entry, AgendaEntry::TypeTodo);

	// Delete the old entry.
	mAgendaUtil->deleteEntry(entry.id());

	qDebug() << "notes : NotesFavoriteView::markNoteAsTodo <--";
}

/*!
	Slot to handle the signal editingCompleted by the notes editor.

	\param status Boolean value indicating whether the note was saved or not.

	\sa NotesEditor.
 */
void NotesFavoriteView::handleEditingCompleted(bool status)
{
	qDebug() << "notes: NotesFavoriteView::handleEditingCompleted -->";

	Q_UNUSED(status)

	// Cleanup.
	mNotesEditor->deleteLater();

	qDebug() << "notes: NotesFavoriteView::handleEditingCompleted <--";
}

/*!
	Directs the view manager to display the Collections view.
 */
void NotesFavoriteView::displayCollectionView()
{
	qDebug() << "notes: NotesFavoriteView::displayCollectionView -->";

	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesCollectionViewId);

	qDebug() << "notes: NotesFavoriteView::displayCollectionView <--";
}

/*!
	Directs the view manager to display the All notes view.
 */
void NotesFavoriteView::displayAllNotesView()
{
	qDebug() << "notes: NotesFavoriteView::displayAllNotesView -->";

	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesMainViewId);

	qDebug() << "notes: NotesFavoriteView::displayAllNotesView <--";
}

/*!
	Slot to handle the case when the state of an action has changed.
 */
void NotesFavoriteView::handleActionStateChanged()
{
	qDebug() << "notes: NotesFavoriteView::handleActionStateChanged -->";

	mAllNotesAction->setChecked(true);

	qDebug() << "notes: NotesFavoriteView::handleActionStateChanged <--";
}

/*!
	Handles the orientation changes.Updates the list
	item when orientation is changed

	\param orientation Value of the orientation
 */
void NotesFavoriteView::handleOrientationChanged(Qt::Orientation orientation)
{
	HbListViewItem *prototype = mListView->listItemPrototype();

	if (Qt::Horizontal == orientation) {
		prototype->setStretchingStyle(HbListViewItem::StretchLandscape);

		// Set the text in landscape mode
		mAllNotesAction->setText(hbTrId("txt_notes_button_all"));
		mViewCollectionAction->setText(hbTrId("txt_notes_button_collections"));
		mAddNoteAction->setText(hbTrId("txt_notes_button_new_note"));
	} else {
		prototype->setStretchingStyle(HbListViewItem::NoStretching);

		// Set empty text in portriat mode so that only icons are visible.
		mAllNotesAction->setText("");
		mViewCollectionAction->setText("");
		mAddNoteAction->setText("");
	}
}

// End of file	--Don't remove this.

