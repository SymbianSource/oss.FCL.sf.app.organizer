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
* Definition file for NotesMainView class.
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
#include <HbLabel>
#include <HbAbstractViewItem>
#include <HbAbstractItemView>
#include <HbGroupBox>
#include <HbListViewItem>

// User includes
#include "agendaeventviewer.h"
#include "notesmainview.h"
#include "notescommon.h"
#include "notesdocloader.h"
#include "agendautil.h"
#include "notesmodel.h"
#include "notessortfilterproxymodel.h"
#include "noteseditor.h"

/*!
	\class NotesMainView
	\brief The main view of the notes application. Responsible for displaying
			notes and todos.

	\sa NotesViewManager
 */

/*!
	Constructs the NotesMainView object.

	\param parent The parent of type QGraphicsWidget.
 */
NotesMainView::NotesMainView(QGraphicsWidget *parent)
:HbView(parent),
 mSelectedItem(0),
 mDeleteAction(0)
{
	qDebug() << "notes: NotesMainView::NotesMainView -->";

	// Nothing yet.

	qDebug() << "notes: NotesMainView::NotesMainView <--";
}

/*!
	Destructor.
 */
NotesMainView::~NotesMainView()
{
	qDebug() << "notes: NotesMainView::~NotesMainView -->";

	if (mDocLoader) {
		delete mDocLoader;
		mDocLoader = 0;
	}

	qDebug() << "notes: NotesMainView::~NotesMainView <--";
}

/*!
	Called by the NotesViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The NotesAppController object.
	\param docLoader Pointer to NotesDocLoader object.
 */
void NotesMainView::setupView(
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

	NotesSortFilterProxyModel *subModel =
			new NotesSortFilterProxyModel(*mAgendaUtil, this);
	subModel->setDynamicSortFilter(true);
	subModel->setSourceModel(mProxyModel);

	// Get the list object from the document and update the model.
	mListView = static_cast<HbListView *> (mDocLoader->findWidget("listView"));
	Q_ASSERT_X(
			mListView,
			"notesmainview.cpp",
			"Unable to find list view.");
	// Update the list view model.
	mListView->setModel(subModel);

	// Setup the operations that can be done with a list view.
	connect(
			mListView, SIGNAL(activated(const QModelIndex &)),
			this, SLOT(handleItemReleased(const QModelIndex &)));
	connect(
			mListView,
			SIGNAL(longPressed(HbAbstractViewItem *, const QPointF &)),
			this,
			SLOT(handleItemLongPressed(HbAbstractViewItem *, const QPointF &)));
	connect(
			mNotesModel, SIGNAL(rowAdded(QModelIndex)),
			this, SLOT(scrollTo(QModelIndex)));

	// Get the view heading label
	mViewHeading = static_cast<HbLabel *> (
			mDocLoader->findWidget("viewHeading"));

	// Get the toolbar/menu actions.
	mAddNoteAction = static_cast<HbAction *> (
			mDocLoader->findObject("newNoteAction"));
	Q_ASSERT_X(
			mAddNoteAction,
			"notesmainview.cpp",
			"Unable to find addNoteAction.");
	connect(
			mAddNoteAction, SIGNAL(triggered()),
			this, SLOT(createNewNote()));

	mAllNotesAction = static_cast<HbAction *> (
			mDocLoader->findObject("allNotesAction"));
	Q_ASSERT_X(
			mAllNotesAction,
			"notesmainview.cpp",
			"Unable to find allNotesAction.");
	mAllNotesAction->setCheckable(true);
	mAllNotesAction->setChecked(true);
	connect(
			mAllNotesAction, SIGNAL(changed()),
			this, SLOT(handleActionStateChanged()));

	mViewCollectionAction = static_cast<HbAction *> (
			mDocLoader->findObject("collectionsViewAction"));
	Q_ASSERT_X(
			mViewCollectionAction,
			"notescollectionview.cpp",
			"Unable to find viewCollectionAction.");

	connect(
			mViewCollectionAction, SIGNAL(triggered()),
			this, SLOT(displayCollectionView()));

	mSubTitle = static_cast<HbGroupBox *>(
			mDocLoader->findWidget("viewHeading"));

	// Handles the orientation change for list items
	HbMainWindow *window = hbInstance->allMainWindows().first();
	handleOrientationChanged(window->orientation());
	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(handleOrientationChanged(Qt::Orientation)));

	// Update sub heading text for main view.
	updateSubTitle();

	connect(
			mAgendaUtil, SIGNAL(entryAdded(ulong)),
			this,SLOT(updateSubTitle(ulong)));
	connect(
			mAgendaUtil, SIGNAL(entryDeleted(ulong)),
			this,SLOT(updateSubTitle(ulong)));
	connect(
			mAgendaUtil, SIGNAL(entryUpdated(ulong)),
			this, SLOT(updateSubTitle(ulong)));
	qDebug() << "notes: NotesMainView::setupView <--";
}

/*!
	Slot which gets called when `+ New note' action is triggered from the view
	toolbar. This is responsible for launching the editor to create a new note.
 */
void NotesMainView::createNewNote()
{
	qDebug() << "notes: NotesMainView::createNewNote -->";

	// Here we Display an editor to the use to enter text.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));
	mNotesEditor->create(NotesEditor::CreateNote);

	qDebug() << "notes: NotesMainView::createNewNote <--";
}

/*!
	Handles the pressing of a list item in the view.

	Here we open the editor for viewing/editing.

	\param index Reference to the QModelIndex representing the view item.
	\sa HbAbstractViewItem
 */
void NotesMainView::handleItemReleased(const QModelIndex &index)
{
	qDebug() << "notes: NotesMainView::handleItemReleased -->";

	// Sanity check.
	if (!index.isValid()) {
		qDebug() << "notes: NotesMainView::handleItemReleased <--";

		return;
	}

	// First get the id of the note and get the corresponding information from
	// agendautil.
	ulong noteId = index.data(NotesNamespace::IdRole).value<qulonglong>();

	if (0 >= noteId) {
		qDebug() << "notes: NotesMainView::handleItemReleased <--";

		// Something wrong.
		return;
	}

	// Get the entry details.
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);
	if (entry.isNull()) {
		qDebug() << "notes: NotesMainView::handleItemReleased <--";

		// Entry invalid.
		return;
	}

	if(AgendaEntry::TypeTodo == entry.type()) {
		// Construct agenda event viewer.
		mAgendaEventViewer = new AgendaEventViewer(mAgendaUtil, this);

		connect(
				mAgendaEventViewer, SIGNAL(viewingCompleted(bool)),
				this, SLOT(handleViewingCompleted(bool)));
		// Launch agenda event viewer
		mAgendaEventViewer->view(
				entry, AgendaEventViewer::ActionEditDelete);
	}else if(AgendaEntry::TypeNote == entry.type()) {
		// Construct notes editor.
		mNotesEditor = new NotesEditor(mAgendaUtil, this);
		connect(
				mNotesEditor, SIGNAL(editingCompleted(bool)),
				this, SLOT(handleEditingCompleted(bool)));

		// Launch the notes editor with the obtained info.
		mNotesEditor->edit(entry);
	}

	qDebug() << "notes: NotesMainView::handleItemReleased <--";
}

/*!
	Displays a list item specific context menu.

	\param item The HbAbstractViewItem that was long pressed.
	\param coords The position where mouse was pressed.

	\sa HbAbstractViewItem, HbListView, HbMenu.
 */
void NotesMainView::handleItemLongPressed(
		HbAbstractViewItem *item, const QPointF &coords)
{
	qDebug() << "notes: NotesMainView::handleItemLongPressed -->";

	mSelectedItem = item;

	ulong noteId = item->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	// Display a context specific menu.
	HbMenu *contextMenu = new HbMenu();

	// Add actions to the context menu.
	if (AgendaEntry::TypeTodo == entry.type()) {
		mEditTodoAction =
				contextMenu->addAction(hbTrId("txt_common_menu_edit"));
		connect(
				mEditTodoAction, SIGNAL(triggered()),
				this, SLOT(editTodo()));
	}

	mDeleteAction =
			contextMenu->addAction(hbTrId("txt_common_menu_delete"));
	connect(
			mDeleteAction, SIGNAL(triggered()),
			this, SLOT(deleteNote()));

	if (AgendaEntry::TypeNote == entry.type()) {
		if (entry.favourite()) {
			mMakeFavouriteAction = contextMenu->addAction(
					hbTrId("txt_notes_menu_remove_from_favorites"));

			connect(
					mMakeFavouriteAction, SIGNAL(triggered()),
					this, SLOT(markNoteAsFavourite()));

		} else {
			mMakeFavouriteAction = contextMenu->addAction(
					hbTrId("txt_notes_menu_mark_as_favorite"));

			connect(
					mMakeFavouriteAction, SIGNAL(triggered()),
					this, SLOT(markNoteAsFavourite()));
		}

	} else if (AgendaEntry::TypeTodo == entry.type()) {
		if (AgendaEntry::TodoNeedsAction == entry.status()) {
			mTodoStatusAction = contextMenu->addAction(
					hbTrId("txt_notes_menu_mark_as_done"));

			connect(
					mTodoStatusAction , SIGNAL(triggered()),
					this, SLOT(markTodoStatus()));

		} else if (AgendaEntry::TodoCompleted == entry.status()) {
			mTodoStatusAction = contextMenu->addAction(
					hbTrId("txt_notes_menu_mark_as_not_done"));

			connect(
					mTodoStatusAction , SIGNAL(triggered()),
					this, SLOT(markTodoStatus()));
		}
	}

	// Show the menu.
	contextMenu->exec(coords);

	qDebug() << "notes: NotesMainView::handleItemLongPressed <--";
}

/*!
	Slot to delete a selected note.
 */
void NotesMainView::deleteNote()
{
	qDebug() << "notes: NotesMainView::deleteNote -->";

	Q_ASSERT(mSelectedItem);

	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		qDebug() << "notes: NotesMainView::deleteNote <--";

		return;
	}
	ulong noteId =
			index.data(NotesNamespace::IdRole).value<qulonglong>();
	if (!noteId) {
		qDebug() << "notes: NotesMainView::deleteNote <--";

		return;
	}

	// Delete the given note.
	mAgendaUtil->deleteEntry(noteId);

	mSelectedItem = 0;

	qDebug() << "notes: NotesMainView::deleteNote <--";
}

/*!
	Marks to-do entry as done or undone based on the completed value

	\param entry reference to the agenda entry
	\param status is true if to-do entry to be marked as done
			is false if to-do entry to be marked as undone
 */
void NotesMainView::markTodoStatus()
{
	qDebug() << "notes: NotesMainView::markTodoStatus -->";

	ulong noteId = mSelectedItem->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	QDateTime currentDateTime = QDateTime::currentDateTime();

	if (AgendaEntry::TodoNeedsAction == entry.status()) {
		mAgendaUtil->setCompleted(entry, true, currentDateTime);
	} else if (AgendaEntry::TodoCompleted == entry.status()) {
		mAgendaUtil->setCompleted(entry, false, currentDateTime);
	}

	qDebug() << "notes: NotesMainView::markTodoStatus <-- ";
}

/*!
	Marks/unmarks the note as favourite.
 */
void NotesMainView::markNoteAsFavourite()
{
	qDebug() << "notes : NotesMainView::markNoteAsFavourite -->";

	ulong noteId = mSelectedItem->modelIndex().data(
				NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.favourite()) {
		entry.setFavourite(0);
	} else {
		entry.setFavourite(1);
	}
	mAgendaUtil->updateEntry(entry);

	qDebug() << "notes : NotesMainView::markNoteAsFavourite <--";
}

/*!
	Slot to handle the signal editingCompleted by the notes editor.

	\param status Boolean value indicating whether the note was saved or not.

	\sa NotesEditor.
 */
void NotesMainView::handleEditingCompleted(bool status)
{
	qDebug() << "notes: NotesMainView::handleEditingCompleted -->";

	Q_UNUSED(status)

	// Cleanup.
	mNotesEditor->deleteLater();

	qDebug() << "notes: NotesMainView::handleEditingCompleted <--";
}

/*!
	Displays the collections' view.
 */
void NotesMainView::displayCollectionView()
{
	qDebug() << "notes: NotesMainView::displayCollectionView -->";

	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesCollectionViewId);

	qDebug() << "notes: NotesMainView::displayCollectionView <--";
}

/*!
	Slot where the list view is made to scroll to the QModelIndex index.

	\param index QModelIndex to be scrolled to.
 */
void NotesMainView::scrollTo(QModelIndex index)
{
	qDebug() << "notes: NotesMainView::scrollTo -->";

	mListView->scrollTo(index, HbAbstractItemView::EnsureVisible);

	qDebug() << "notes: NotesMainView::scrollTo <--";
}

/*!
	Slot to handle viewing Completed signal from agenda event viewer

	\param status Indicates the status of viewing
 */
void NotesMainView::handleViewingCompleted(bool status)
{
	qDebug() << "notes: NotesMainView::handleViewingCompleted -->";

	Q_UNUSED(status)

	mAgendaEventViewer->deleteLater();

	qDebug() << "notes: NotesMainView::handleViewingCompleted <--";
}

/*!
	Slot to handle the case when the state of an action has changed.
 */
void NotesMainView::handleActionStateChanged()
{
	qDebug() << "notes: NotesMainView::handleActionStateChanged -->";

	mAllNotesAction->setChecked(true);

	qDebug() << "notes: NotesMainView::handleActionStateChanged <--";
}

/*!
	Launches the to-do editor to edit the to-do entry
 */

void NotesMainView::editTodo()
{
	qDebug() << "notes: NotesMainView::editTodo -->";

	// Get the selected list item index
	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		qDebug() << "notes: NotesMainView::editTodo <--";
		return;
	}
	ulong todoId =
			index.data(NotesNamespace::IdRole).value<qulonglong>();
	if (!todoId) {
		qDebug() << "notes: NotesMainView::editTodo <--";

		return;
	}

	// Construct notes editor.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));

	// Launch the to-do editor with the obtained info.
	mNotesEditor->edit(todoId);

	qDebug() << "notes: NotesMainView::editTodo <--";
}

/*!
	Handles the orientation changes.Updates the list
	item when orientation is changed

	\param orientation Value of the orientation
 */
void NotesMainView::handleOrientationChanged(Qt::Orientation orientation)
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

/*!
	Updates the sub heading text
 */
void NotesMainView::updateSubTitle(ulong id)
{
	Q_UNUSED(id)

	// Get the number of notes and to-do entries.
	QList<ulong> entries = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags)
			(AgendaUtil::IncludeNotes
			| AgendaUtil::IncludeCompletedTodos
			| AgendaUtil::IncludeIncompletedTodos));
	int c= entries.count();
	mSubTitle->setHeading(
			hbTrId("txt_notes_subhead_ln_notes",entries.count()));
}
// End of file	--Don't remove this.
