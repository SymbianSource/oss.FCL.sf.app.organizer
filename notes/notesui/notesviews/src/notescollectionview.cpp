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
#include <QDebug>
#include <HbListWidget>
#include <HbListWidgetItem>
#include <HbAction>
#include <HbTextEdit>
#include <HbInstance>
#include <HbMainWindow>
#include <HbMenu>
#include <HbDialog>
#include <HbLabel>
#include <HbAbstractViewItem>
#include <HbGroupBox>

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
	qDebug() << "notes: NotesCollectionView::NotesCollectionView -->";

	// Nothing yet.

	qDebug() << "notes: NotesCollectionView::NotesCollectionView <--";
}

/*!
	Destructor.
 */
NotesCollectionView::~NotesCollectionView()
{
	qDebug() << "notes: NotesCollectionView::~NotesCollectionView -->";

	if (mDocLoader) {
		delete mDocLoader;
		mDocLoader = 0;
	}

	qDebug() << "notes: NotesCollectionView::~NotesCollectionView <--";
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
	qDebug() << "notes: NotesCollectionView::setupView -->";

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
	mFavouriteModel->setSourceModel(mAppControllerIf->notesModel()->sourceModel());
	connect(
			mFavouriteModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
			this, SLOT(updateFavouritesCount(QModelIndex, int, int)));
	connect(
			mFavouriteModel, SIGNAL(rowsRemoved(QModelIndex, int, int)),
			this, SLOT(updateFavouritesCount(QModelIndex, int, int)));

	// Get the list object from the document and update the model.
	mListWidget = static_cast<HbListWidget *> (
			mDocLoader->findWidget("listWidget"));
	connect(
			mListWidget, SIGNAL(activated(HbListWidgetItem *)),
			this, SLOT(handleActivated(HbListWidgetItem *)));

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

	// Check orientation and update the toolbar action's text
	// icons in potriat mode and icons + text in landscape mode.
	HbMainWindow *window = hbInstance->allMainWindows().first();
	updateToolbarTexts(window->orientation());
	connect(
			window, SIGNAL(orientationChanged(Qt::Orientation)),
			this, SLOT(updateToolbarTexts(Qt::Orientation)));

	qDebug() << "notes: NotesCollectionView::setupView <--";
}

/*!
	Displays all notes view.
 */
void NotesCollectionView::displayAllNotesView()
{
	qDebug() << "notes: NotesMainView::displayAllNotesView -->";

	// Switch to collections view.
	mAppControllerIf->switchToView(NotesNamespace::NotesMainViewId);

	qDebug() << "notes: NotesMainView::displayAllNotesView <--";
}

/*!
	Refreshes the content of the collection view if needed.
 */
void NotesCollectionView::resetCollectionView()
{
	qDebug() << "notes: NotesCollectionView::resetCollectionView -->";

	QString countString("(%1)");

	// Get the count of to-dos.
	QList<ulong> entries = mAgendaUtil->entryIds(
				(AgendaUtil::FilterFlags)
				(AgendaUtil::IncludeCompletedTodos
				| AgendaUtil::IncludeIncompletedTodos));
	// Update the count of to-do's.
	HbListWidgetItem *item = mListWidget->item(0);
	item->setSecondaryText(countString.arg(QString::number(entries.count())));

	// Get the count of notes.
	entries = mAgendaUtil->entryIds(AgendaUtil::IncludeNotes);
	// Update the count of notes in the view.
	item = mListWidget->item(2);
	item->setSecondaryText(countString.arg(QString::number(entries.count())));

	qDebug() << "notes: NotesCollectionView::resetCollectionView <--";
}

/*!
	Create a new Note
 */
void NotesCollectionView::createNewNote()
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
	Handles editing complete of the notes editor
 */
void NotesCollectionView::handleEditingCompleted(bool status)
{
	qDebug() << "notes: NotesMainView::handleEditingCompleted -->";

	Q_UNUSED(status)

	// Refresh the content of the view.
	resetCollectionView();

	// Cleanup.
	mNotesEditor->deleteLater();

	qDebug() << "notes: NotesMainView::handleEditingCompleted <--";
}

/*!
	Updates the view data in case of changes in the database.
	Handles the case when entry is added or deleted by notes application.
 */
void NotesCollectionView::updateData(ulong id)
{
	qDebug() << "notes: NotesMainView::updateData -->";

	Q_UNUSED(id)

	// Refresh the content of the view.
	resetCollectionView();

	qDebug() << "notes: NotesMainView::updateData <--";
}

/*!
	Updates the view data in case of changes in the database.
	Handles the case when db is updated by a different client.
 */
void NotesCollectionView::updateData(QList<ulong> ids)
{
	qDebug() << "notes: NotesMainView::updateData -->";

	Q_UNUSED(ids)

	// Refresh the content of the view.
	resetCollectionView();

	qDebug() << "notes: NotesMainView::updateData <--";
}

/*!
	Handles the case when a list item is activated and the corresponding
	collections view (viz., notes, to-dos, favourites) is opened.

	\param item The item that was activated.
 */
void NotesCollectionView::handleActivated(HbListWidgetItem *item)
{
	QString secondary = item->secondaryText();
	QString primary = item->text();

	switch (mListWidget->row(item)) {
		case 0:
			// To-do item selected. Switch to to-do view.
			mAppControllerIf->switchToView(NotesNamespace::NotesTodoViewId);
			break;

		case 1:
			// Favorites item selected. Switch to favorites view.
			mAppControllerIf->switchToView(
					NotesNamespace::NotesFavoritesViewId);
			break;

		case 2:
			// Recent notes item selected.
			mAppControllerIf->switchToView(
					NotesNamespace::NotesNoteViewId);
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
	QString countString("(%1)");
	HbListWidgetItem *item = mListWidget->item(1);
	item->setSecondaryText(countString.arg(mFavouriteModel->rowCount()));
}

/*!
	Populate the content of the view.
 */
void NotesCollectionView::populateListView()
{
	qDebug() << "notes: NotesCollectionView::populateListView -->";

	QString countString(hbTrId("[%1]"));
	// Add To-do's item.
	HbListWidgetItem *item = new HbListWidgetItem;
	item->setText(hbTrId("txt_notes_list_todos"));
	// Get the number of to-do entries.
	QList<ulong> entries = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags)
			(AgendaUtil::IncludeCompletedTodos
			| AgendaUtil::IncludeIncompletedTodos));
	item->setSecondaryText(countString.arg(QString::number(entries.count())));
	mListWidget->addItem(item);

	// Add Favorites item.
	item = new HbListWidgetItem;
	item->setText(hbTrId("txt_notes_list_favorites"));
	item->setSecondaryText(countString.arg(mFavouriteModel->rowCount()));
	mListWidget->addItem(item);

	// Get the number of notes.
	entries = mAgendaUtil->entryIds(AgendaUtil::IncludeNotes);
	// Add Recent notes item.
	item = new HbListWidgetItem;
	item->setText(hbTrId("txt_notes_list_recent_notes"));
	item->setSecondaryText(countString.arg(QString::number(entries.count())));
	mListWidget->addItem(item);

	qDebug() << "notes: NotesCollectionView::populateListView <--";
}

/*!
	Slot to handle the case when the state of an action has changed.
 */
void NotesCollectionView::handleActionStateChanged()
{
	qDebug() << "notes: NotesCollectionView::handleActionStateChanged -->";

	mViewCollectionAction->setChecked(true);

	qDebug() << "notes: NotesCollectionView::handleActionStateChanged <--";
}

/*!
	Update the toolbar actions texts on orientation change.
 */
void NotesCollectionView::updateToolbarTexts(Qt::Orientation orientation)
{
	if (Qt::Horizontal == orientation) {
		// Set the text in landscape mode
		mAllNotesAction->setText(hbTrId("txt_notes_button_all"));
		mViewCollectionAction->setText(hbTrId("txt_notes_button_collections"));
		mAddNoteAction->setText(hbTrId("txt_notes_button_new_note"));
	} else if( Qt::Vertical == orientation) {
		// Set empty text in portriat mode so that only icons are visible.
		mAllNotesAction->setText("");
		mViewCollectionAction->setText("");
		mAddNoteAction->setText("");
	}
}

// End of file	--Don't remove this.
