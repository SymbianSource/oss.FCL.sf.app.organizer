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
* Definition file for NotesTodoView class.
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
#include "agendaeventviewer.h"
#include "notestodoview.h"
#include "notescommon.h"
#include "notesdocloader.h"
#include "agendautil.h"
#include "notesmodel.h"
#include "notessortfilterproxymodel.h"
#include "noteseditor.h"

/*!
	\class NotesTodoView
	\brief The main view of the notes application. Responsible for displaying
			notes and todos.

	\sa NotesViewManager
 */

/*!
	Constructs the NotesTodoView object.

	\param parent The parent of type QGraphicsWidget.
 */
NotesTodoView::NotesTodoView(QGraphicsWidget *parent)
:HbView(parent),
 mSelectedItem(0),
 mDeleteAction(0)
{
	qDebug() << "notes: NotesTodoView::NotesTodoView -->";

	// Nothing yet.

	qDebug() << "notes: NotesTodoView::NotesTodoView <--";
}

/*!
	Destructor.
 */
NotesTodoView::~NotesTodoView()
{
	qDebug() << "notes: NotesTodoView::~NotesTodoView -->";

	if (mDocLoader) {
	    delete mDocLoader;
	    mDocLoader = 0;
    }

	qDebug() << "notes: NotesTodoView::~NotesTodoView <--";
}

/*!
	Called by the NotesViewManager after loading the view from the docml.
	The initializaion/setup of the view is done here.

	\param controller The NotesAppController object.
	\param docLoader Pointer to NotesDocLoader object.
 */
void NotesTodoView::setupView(
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

	mProxyModel->setFilterRole(NotesNamespace::TypeRole);
	mProxyModel->setFilterRegExp(QRegExp(QString("todo")));

	NotesSortFilterProxyModel *subModel =
			new NotesSortFilterProxyModel(*mAgendaUtil, this);
	subModel->setDynamicSortFilter(true);
	subModel->setSourceModel(mProxyModel);

	// Get the list object from the document and update the model.
	mListView = static_cast<HbListView *>
			(mDocLoader->findWidget("todoListView"));
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

	// Get the toolbar/menu actions.
	mAddTodoAction = static_cast<HbAction *> (
			mDocLoader->findObject("newTodoAction"));
	connect(
			mAddTodoAction, SIGNAL(triggered()),
			this, SLOT(createNewTodo()));

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

	mSubTitle = static_cast<HbGroupBox *>(
			mDocLoader->findWidget("subtitleGroupBox"));

	// Update sub heading text for to-do collections view.
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
	Opens the to-do editor to create a new to-do.
 */
void NotesTodoView::createNewTodo()
{
	qDebug() << "notes: NotesTodoView::createNewTodo -->";

	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));

	mNotesEditor->create(NotesEditor::CreateTodo);

	qDebug() << "notes: NotesTodoView::createNewTodo -->";
}

/*!
	Handles the pressing of a list item in the view.

	Here we open the editor for viewing/editing.

	\param index Reference to the QModelIndex representing the view item.
	\sa HbAbstractViewItem
 */
void NotesTodoView::handleItemReleased(const QModelIndex &index)
{
	qDebug() << "notes: NotesTodoView::handleItemReleased -->";

	// Sanity check.
	if (!index.isValid()) {
		qDebug() << "notes: NotesTodoView::handleItemReleased <--";

		return;
	}

	// First get the id of the to-do and get the corresponding information from
	// agendautil.
	ulong toDoId = index.data(NotesNamespace::IdRole).value<qulonglong>();

	if (0 >= toDoId) {
		qDebug() << "notes: NotesTodoView::handleItemReleased <--";

		// Something wrong.
		return;
	}

	// Construct agenda event viewer.
	mAgendaEventViewer = new AgendaEventViewer(mAgendaUtil, this);

	connect(
			mAgendaEventViewer, SIGNAL(viewingCompleted(bool)),
			this, SLOT(handleViewingCompleted()));
	// Launch agenda event viewer
	mAgendaEventViewer->view(toDoId, AgendaEventViewer::ActionEditDelete);

	qDebug() << "notes: NotesTodoView::handleItemReleased <--";
}

/*!
	Displays a list item specific context menu.

	\param item The HbAbstractViewItem that was long pressed.
	\param coords The position where mouse was pressed.

	\sa HbAbstractViewItem, HbListView, HbMenu.
 */
void NotesTodoView::handleItemLongPressed(
		HbAbstractViewItem *item, const QPointF &coords)
{
	qDebug() << "notes: NotesTodoView::handleItemLongPressed -->";

	mSelectedItem = item;

	// Get the entry of the selected item.
	ulong noteId = item->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	// Display a context specific menu.
	HbMenu *contextMenu = new HbMenu();

	// Add actions to the context menu.
	mEditAction = contextMenu->addAction(
			hbTrId("txt_common_menu_edit"));
	connect(
			mEditAction, SIGNAL(triggered()),
			this, SLOT(editTodo()));


	mDeleteAction = contextMenu->addAction(
			hbTrId("txt_common_menu_delete"));
	connect(
			mDeleteAction, SIGNAL(triggered()),
			this, SLOT(deleteTodo()));

	if (AgendaEntry::TypeTodo == entry.type()) {
		if (AgendaEntry::TodoNeedsAction == entry.status()) {
			mTodoStatusAction = contextMenu->addAction(
					hbTrId("txt_notes_menu_mark_as_done"));
			connect(
					mTodoStatusAction , SIGNAL(triggered()),
					this, SLOT(markTodoStatus()));

		} else if (AgendaEntry::TodoCompleted == entry.status()) {
			mTodoStatusAction =
					contextMenu->addAction(
							hbTrId("txt_notes_menu_mark_as_not_done"));
			connect(
					mTodoStatusAction , SIGNAL(triggered()),
					this, SLOT(markTodoStatus()));
		}
	}

	// Show the menu.
	contextMenu->exec(coords);

	qDebug() << "notes: NotesTodoView::handleItemLongPressed <--";
}

/*!
	Slot to delete a selected to-do.
 */
void NotesTodoView::deleteTodo()
{
	qDebug() << "notes: NotesTodoView::deleteTodo -->";

	Q_ASSERT(mSelectedItem);

	QModelIndex index = mSelectedItem->modelIndex();
	if (!index.isValid()) {
		qDebug() << "notes: NotesTodoView::deleteTodo <--";

		return;
	}
	ulong entryId =
			index.data(NotesNamespace::IdRole).value<qulonglong>();
	if (!entryId) {
		qDebug() << "notes: NotesTodoView::deleteTodo <--";

		return;
	}

	// Delete the given note.
	mAgendaUtil->deleteEntry(entryId);

	mSelectedItem = 0;

	qDebug() << "notes: NotesTodoView::deleteTodo <--";
}

/*!
	Marks to-do entry as done or undone based on the completed value.
 */
void NotesTodoView::markTodoStatus()
{
	qDebug() << "notes: NotesTodoView::markTodoStatus -->";

	ulong entryId = mSelectedItem->modelIndex().data(
			NotesNamespace::IdRole).value<qulonglong>();
	AgendaEntry entry = mAgendaUtil->fetchById(entryId);

	QDateTime currentDateTime = QDateTime::currentDateTime();

	if (AgendaEntry::TodoNeedsAction == entry.status()) {
		mAgendaUtil->setCompleted(entry, true, currentDateTime);
	} else if (AgendaEntry::TodoCompleted == entry.status()) {
		mAgendaUtil->setCompleted(entry, false, currentDateTime);
	}

	qDebug() << "notes: NotesTodoView::markTodoStatus <-- ";
}

/*!
	Opens to-do in an editor.
 */
void NotesTodoView::editTodo()
{
	qDebug() << "notes: NotesTodoView::editTodo -->";

	QModelIndex index = mSelectedItem->modelIndex();

	// Sanity check.
	if (!index.isValid()) {
		qDebug() << "notes: NotesTodoView::editTodo <--";

		return;
	}

	// First get the id of the note and get the corresponding information from
	// agendautil.
	ulong noteId = index.data(NotesNamespace::IdRole).value<qulonglong>();

	if (0 >= noteId) {
		qDebug() << "notes: NotesTodoView::editTodo <--";

		// Something wrong.
		return;
	}

	// Get the entry details.
	AgendaEntry entry = mAgendaUtil->fetchById(noteId);

	if (entry.isNull()) {
		qDebug() << "notes: NotesTodoView::editTodo <--";

		// Entry invalid.
		return;
	}

	// Now launch the editor with the obtained info.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));
	mNotesEditor->edit(entry);

	qDebug() << "notes: NotesTodoView::editTodo <--";
}

/*!
	Slot to handle the signal editingCompleted by the notes editor.

	\param status Boolean value indicating whether the note was saved or not.

	\sa NotesEditor.
 */
void NotesTodoView::handleEditingCompleted(bool status)
{
	qDebug() << "notes: NotesMainView::handleEditingCompleted -->";

	Q_UNUSED(status)

	// Cleanup.
	mNotesEditor->deleteLater();

	qDebug() << "notes: NotesMainView::handleEditingCompleted <--";
}

/*!
	Directs the view manager to display the Collections view.
 */
void NotesTodoView::displayCollectionView()
{
	qDebug() << "notes: NotesTodoView::displayCollectionView -->";

	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesCollectionViewId);

	qDebug() << "notes: NotesTodoView::displayCollectionView <--";
}

/*!
	Directs the view manager to display the All notes view.
 */
void NotesTodoView::displayAllNotesView()
{
	qDebug() << "notes: NotesTodoView::displayAllNotesView -->";

	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesMainViewId);

	qDebug() << "notes: NotesTodoView::displayAllNotesView <--";
}

/*!
	Slot to handle signal editingCompleted by the to-do editor
 */
void NotesTodoView::handleEditingCompleted()
{
	qDebug() <<"notes: NotesTodoView::handleEditingCompleted -->";

	// Cleanup.
	mNotesEditor->deleteLater();

	qDebug() <<"notes: NotesTodoView::handleEditingCompleted <--";
}

/*!
	Slot to handle signal handleViewingompleted by the agenda event viewer
 */
void NotesTodoView::handleViewingCompleted(bool status)
{
	qDebug() <<"notes: NotesTodoView::handleEditingCompleted -->";

	Q_UNUSED(status)
	// Cleanup.
	mAgendaEventViewer->deleteLater();

	qDebug() <<"notes: NotesTodoView::handleEditingCompleted <--";
}

/*!
	Slot to handle the case when the state of an action has changed.
 */
void NotesTodoView::handleActionStateChanged()
{
	qDebug() << "notes: NotesTodoView::handleActionStateChanged -->";

	mAllNotesAction->setChecked(true);

	qDebug() << "notes: NotesTodoView::handleActionStateChanged <--";
}

/*!
	Handles the orientation changes.Updates the list
	item when orientation is changed

	\param orientation Value of the orientation
 */
void NotesTodoView::handleOrientationChanged(Qt::Orientation orientation)
{
	HbListViewItem *prototype = mListView->listItemPrototype();

	if (Qt::Horizontal == orientation) {
		prototype->setStretchingStyle(HbListViewItem::StretchLandscape);

		// Set the text in landscape mode
		mAllNotesAction->setText(hbTrId("txt_notes_button_all"));
		mViewCollectionAction->setText(hbTrId("txt_notes_button_collections"));
		mAddTodoAction->setText(hbTrId("txt_notes_button_new_todo"));
	} else {
		prototype->setStretchingStyle(HbListViewItem::NoStretching);

		// Set empty text in portriat mode so that only icons are visible.
		mAllNotesAction->setText("");
		mViewCollectionAction->setText("");
		mAddTodoAction->setText("");
	}
}

/*!
	Updates the sub heading text
 */
void NotesTodoView::updateSubTitle(ulong id)
{
	Q_UNUSED(id)

	// Get the number of notes and to-do entries.
	QList<ulong> entries = mAgendaUtil->entryIds(
		AgendaUtil::IncludeIncompletedTodos);
	mSubTitle->setHeading(
			hbTrId("txt_notes_subhead_todos_ln_pending",entries.count()));
}

// End of file	--Don't remove this.
