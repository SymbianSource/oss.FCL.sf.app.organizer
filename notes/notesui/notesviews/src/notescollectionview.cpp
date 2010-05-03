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
* Definition file for NotesCollectionView class.
*
*/

// System includes
#include <HbListWidget>
#include <QStandardItemModel>
#include <HbListView>
#include <HbAction>
#include <HbTextEdit>
#include <HbInstance>
#include <HbMainWindow>
#include <HbMenu>
#include <HbDialog>
#include <HbLabel>
#include <HbAbstractViewItem>
#include <HbGroupBox>
#include <HbStyleLoader>

// User includes
#include "notescollectionview.h"
#include "notescommon.h"
#include "notesdocloader.h"
#include "agendautil.h"
#include "notesmodel.h"
#include "noteseditor.h"
#include "notessortfilterproxymodel.h"

/*!
	\class NotesCollectionView
	\brief The main view of the notes application. Responsible for displaying
			notes and todos.

	\sa NotesViewManager
 */

/*!
	Constructs the NotesCollectionView object.

	\param parent The parent of type QGraphicsWidget.
 */
NotesCollectionView::NotesCollectionView(QGraphicsWidget *parent)
:HbView(parent)
{
	// Nothing yet.
}

/*!
	Destructor.
 */
NotesCollectionView::~NotesCollectionView()
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
void NotesCollectionView::setupView(
		NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader)
{
	mDocLoader = docLoader;
	mAppControllerIf = &controllerIf;
	mNotesModel = mAppControllerIf->notesModel();
	mAgendaUtil = mAppControllerIf->agendaUtil();

	connect(
			mAgendaUtil, SIGNAL(entriesChanged(QList<ulong>)),
			this, SLOT(updateData(QList<ulong>)));

	connect(
			mAgendaUtil, SIGNAL(entryAdded(ulong)),
			this, SLOT(updateData(ulong)));

	connect(
			mAgendaUtil, SIGNAL(entryDeleted(ulong)),
			this, SLOT(updateData(ulong)));

	mFavouriteModel = new NotesSortFilterProxyModel(*mAgendaUtil, this);
	mFavouriteModel->setDynamicSortFilter(true);
	mFavouriteModel->setFilterRole(NotesNamespace::FavouriteRole);
	mFavouriteModel->setFilterRegExp(QRegExp("favourites"));
	mFavouriteModel->setSourceModel(
			mAppControllerIf->notesModel()->sourceModel());
	connect(
			mFavouriteModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
			this, SLOT(updateFavouritesCount(QModelIndex, int, int)));
	connect(
			mFavouriteModel, SIGNAL(rowsRemoved(QModelIndex, int, int)),
			this, SLOT(updateFavouritesCount(QModelIndex, int, int)));

	// Get the list view from the document and update the model.
	mListView = static_cast<HbListView *> (
			mDocLoader->findWidget("listView"));
	connect(
			mListView, SIGNAL(activated(const QModelIndex &)),
			this, SLOT(handleActivated(const QModelIndex &)));

	// Populate the content of the view.
	populateListView();

	// Get the toolbar/menu actions.
	mAllNotesAction = static_cast<HbAction *> (
			mDocLoader->findObject("allNotesAction"));
	Q_ASSERT_X(
			mAllNotesAction,
			"notescollectionview.cpp",
			"Unable to find allNotesAction.");
	connect(
			mAllNotesAction, SIGNAL(triggered()),
			this, SLOT(displayAllNotesView()));

	mViewCollectionAction = static_cast<HbAction *> (
			mDocLoader->findObject("collectionsViewAction"));
	Q_ASSERT_X(
			mViewCollectionAction,
			"notescollectionview.cpp",
			"Unable to find viewCollectionAction.");
	mViewCollectionAction->setCheckable(true);
	mViewCollectionAction->setChecked(true);
	connect(
			mViewCollectionAction, SIGNAL(changed()),
			this, SLOT(handleActionStateChanged()));
	connect(
			mViewCollectionAction, SIGNAL(triggered()),
			this, SLOT(resetCollectionView()));

	mAddNoteAction = static_cast<HbAction *> (
			mDocLoader->findObject("newNoteAction"));
	Q_ASSERT_X(
			mAddNoteAction,
			"notescollectionview.cpp",
			"Unable to find addNoteAction.");
	connect(
			mAddNoteAction, SIGNAL(triggered()),
			this, SLOT(createNewNote()));
}

/*!
	Displays all notes view.
 */
void NotesCollectionView::displayAllNotesView()
{
	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesMainViewId);
}

/*!
	Refreshes the content of the collection view if needed.
 */
void NotesCollectionView::resetCollectionView()
{
	QString countString(hbTrId("txt_notes_list_note_count"));

	// Update the count of to-do's.
	QModelIndex mdlIndex = mListView->model()->index(0, 0);
	QStringList todoStringList;
	todoStringList.append(hbTrId("txt_notes_list_todos"));
	todoStringList.append(countString.arg(QString::number(todosCount())));
	mListView->model()->setData(mdlIndex, todoStringList, Qt::DisplayRole);

	// Update the count of notes in the view.
	mdlIndex = mListView->model()->index(2, 0);
	QStringList notesStringList;
	notesStringList.append(hbTrId("txt_notes_list_plain_notes"));
	notesStringList.append(
			countString.arg(QString::number(recentNotesCount())));
	mListView->model()->setData(mdlIndex, notesStringList, Qt::DisplayRole);
}

/*!
	Create a new Note
 */
void NotesCollectionView::createNewNote()
{
	// Here we Display an editor to the use to enter text.
	mNotesEditor = new NotesEditor(mAgendaUtil, this);
	connect(
			mNotesEditor, SIGNAL(editingCompleted(bool)),
			this, SLOT(handleEditingCompleted(bool)));

	mNotesEditor->create(NotesEditor::CreateNote);
}

/*!
	Handles editing complete of the notes editor
 */
void NotesCollectionView::handleEditingCompleted(bool status)
{
	Q_UNUSED(status)

	// Refresh the content of the view.
	resetCollectionView();

	// Cleanup.
	mNotesEditor->deleteLater();
}

/*!
	Updates the view data in case of changes in the database.
	Handles the case when entry is added or deleted by notes application.
 */
void NotesCollectionView::updateData(ulong id)
{
	Q_UNUSED(id)

	// Refresh the content of the view.
	resetCollectionView();
}

/*!
	Updates the view data in case of changes in the database.
	Handles the case when db is updated by a different client.
 */
void NotesCollectionView::updateData(QList<ulong> ids)
{
	Q_UNUSED(ids)

	// Refresh the content of the view.
	resetCollectionView();
}

/*!
	Handles the case when a list item is activated and the corresponding
	collections view (viz., notes, to-dos, favourites) is opened.

	\param index The index of the item that was activated.
 */
void NotesCollectionView::handleActivated(const QModelIndex &index)
{

	switch (index.row()) {
		case 0: {
			if (todosCount()) {
				// To-do item selected.
				// Switch to to-do view if to-do entries exist.
				mAppControllerIf->switchToView(NotesNamespace::NotesTodoViewId);
			}
		}
		break;

		case 1: {
			if (mFavouriteModel->rowCount()) {
				// Favorites item selected.
				// Switch to favorites view if favorites exist.
				mAppControllerIf->switchToView(
						NotesNamespace::NotesFavoritesViewId);
			}
		}
		break;

		case 2: {
			if (recentNotesCount()) {
				// Recent notes item selected.
				// Switch to recent notes view if notes exist.
				mAppControllerIf->switchToView(
						NotesNamespace::NotesNoteViewId);
			}
		}
		break;

		default:
			// Nothing yet.
			break;
	}
}

/*!
	Updates the number of favourites displayed.

	\sa QAbstractItemModel
 */
void NotesCollectionView::updateFavouritesCount(
		const QModelIndex &index, int start, int end)
{
	Q_UNUSED(index)
	Q_UNUSED(start)
	Q_UNUSED(end)

	// Update the count of notes in the view.
	QString countString(hbTrId("txt_notes_list_note_count"));
	QModelIndex mdlIndex = mListView->model()->index(1, 0);
	QStringList favStringList;
	favStringList.append(hbTrId("txt_notes_list_favorites"));
	favStringList.append(countString.arg(mFavouriteModel->rowCount()));
	mListView->model()->setData(mdlIndex, favStringList, Qt::DisplayRole);
}

/*!
	Slot to handle the case when the state of an action has changed.
 */
void NotesCollectionView::handleActionStateChanged()
{
	mViewCollectionAction->setChecked(true);
}

/*!
	Populate the content of the view.
 */
void NotesCollectionView::populateListView()
{
	QStandardItemModel *model = new QStandardItemModel(this);
	model->setColumnCount(1);

	QString countString(hbTrId("txt_notes_list_note_count"));

	// Add To-do's item.
	QStandardItem *item = new QStandardItem();
	QStringList todoStringList;
	todoStringList.append(hbTrId("txt_notes_list_todos"));
	todoStringList.append(countString.arg(QString::number(todosCount())));
	item->setData(todoStringList, Qt::DisplayRole);
	model->appendRow(item);

	// Add Favorites item.
	item = new QStandardItem();
	QStringList favStringList;
	favStringList.append(hbTrId("txt_notes_list_favorites"));
	favStringList.append(countString.arg(mFavouriteModel->rowCount()));
	item->setData(favStringList, Qt::DisplayRole);
	model->appendRow(item);

	// Add Recent notes item.
	item = new QStandardItem();
	QStringList notesStringList;
	notesStringList.append(hbTrId("txt_notes_list_plain_notes"));
	notesStringList.append(
			countString.arg(QString::number(recentNotesCount())));
	item->setData(notesStringList, Qt::DisplayRole);
	model->appendRow(item);

	HbStyleLoader::registerFilePath(":/style");
	mListView->setLayoutName("custom");
	mListView->setModel(model);
}


/*!
	Returns the todos count

	\return int todos count.
 */
int NotesCollectionView::todosCount()
{
	QList<ulong> entries = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags)
			(AgendaUtil::IncludeCompletedTodos
			| AgendaUtil::IncludeIncompletedTodos));
	return entries.count();
}

/*!
	Returns recent notes count.

	\return int recent notes count.
 */
int NotesCollectionView::recentNotesCount()
{
	QList<ulong> entries = mAgendaUtil->entryIds(AgendaUtil::IncludeNotes);
	return entries.count();
}

// End of file	--Don't remove this.
