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
* Header file for NotesFavoriteView class.
*
*/

#ifndef NOTESFAVORITEVIEW_H
#define NOTESFAVORITEVIEW_H

// System includes
#include <QObject>
#include <HbView>

// User includes
#include "notesviewsdefines.h"
#include "notesappcontrollerif.h"

// Forward declarations
class QGraphicsWidget;
class HbListView;
class HbAction;
class HbAbstractViewItem;
class NotesModel;
class NotesDocLoader;
class AgendaUtil;
class NotesSortFilterProxyModel;
class NotesEditor;

class NotesFavoriteView : public HbView
{
	Q_OBJECT

public:
	NOTESVIEWS_EXPORT NotesFavoriteView(QGraphicsWidget *parent = 0);
	NOTESVIEWS_EXPORT virtual ~NotesFavoriteView();

public:
	NOTESVIEWS_EXPORT void setupView(
			NotesAppControllerIf &controllerIf, NotesDocLoader *docLoader);

private slots:
	void createNewNote();
	void handleItemReleased(const QModelIndex &index);
	void handleItemLongPressed(
			HbAbstractViewItem *item, const QPointF &coords);
	void deleteNote();
	void markNoteAsNotFavourite();
	void markNoteAsTodo();

	void handleEditingCompleted(bool status);
	void displayCollectionView();
	void displayAllNotesView();

	void handleActionStateChanged();
	void handleOrientationChanged(Qt::Orientation);

private:
	HbListView *mListView;
	HbAbstractViewItem *mSelectedItem;

	HbAction *mAllNotesAction;
	HbAction *mViewCollectionAction;
	HbAction *mAddNoteAction;

	HbAction *mDeleteAction;
	HbAction *mRemoveFavoriteAction;
	HbAction *mMarkTodoAction;

	AgendaUtil *mAgendaUtil;

	NotesAppControllerIf *mAppControllerIf;
	NotesDocLoader *mDocLoader;
	NotesModel *mNotesModel;
	NotesSortFilterProxyModel *mProxyModel;
	NotesEditor *mNotesEditor;
};

#endif // NOTESFAVORITEVIEW_H

// End of file	--Don't remove this.
