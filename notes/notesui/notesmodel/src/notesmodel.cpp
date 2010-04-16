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
* Definition file for class NotesModel.
*
*/

// System includes
#include <QDebug>
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QTimer>
#include <QDateTime>
#include <HbIcon>

// User includes
#include "notesmodel.h"
#include "agendautil.h"
#include "agendaentry.h"
#include "notescommon.h"

/*!
	\class NotesModel

	This is the source model class for the notes application views. This owns
	a QStandardItemModel which is a model for the data containing notes, todos
	and reminders.
 */

/*!
	Constructor.

	\param agendaUtil Pointer to the object of AgendaUtil.
	\param parent Parent of type QObject.
 */
NotesModel::NotesModel(AgendaUtil *agendaUtil, QObject *parent)
:QObject(parent),
 mAgendaUtil(agendaUtil)
{
	qDebug() << "notes: NotesModel::NotesModel -->";

	// Construct the source model.
	mSourceModel = new QStandardItemModel(0, 1, this);

	connect(
			mAgendaUtil, SIGNAL(entriesChanged(QList<ulong>)),
			this, SLOT(populateSourceModel(QList<ulong>)));

	// Connect to entryAdded signal of agendautil.
	connect(
			mAgendaUtil, SIGNAL(entryAdded(ulong)),
			this, SLOT(addEntryToModel(ulong)));
	// Connect to entryDeleted signal of agendautil.
	connect(
			mAgendaUtil, SIGNAL(entryDeleted(ulong)),
			this, SLOT(removeEntryFromModel(ulong)));
	// Connect to entryUpdated signal of agendautil.
	connect(
			mAgendaUtil, SIGNAL(entryUpdated(ulong)),
			this, SLOT(updateSourceModel(ulong)));

	// Populate the model in a different thread.
	QTimer::singleShot(1, this, SLOT(populateSourceModel()));

	qDebug() << "notes: NotesModel::NotesModel <--";
}

/*!
	Destructor.
 */
NotesModel::~NotesModel()
{
	qDebug() << "notes: NotesModel::~NotesModel -->";

	// Nothing yet.

	qDebug() << "notes: NotesModel::~NotesModel <--";
}

/*!
	Returns the source model to be used with a view.

	\return QAbstractItemModel
	\sa QAbstractItemModel, HbListView.
 */
QAbstractItemModel *NotesModel::sourceModel()
{
	qDebug() << "notes: NotesModel::sourceModel -->";

	Q_ASSERT(mSourceModel);

	qDebug() << "notes: NotesModel::sourceModel <--";

	return mSourceModel;
}

/*!
	Populates the source model.
 */
void NotesModel::populateSourceModel()
{
	qDebug() << "notes: NotesModel::populateSourceModel -->";

	// Clear the model if it has any data already
	mSourceModel->clear();
	mSourceModel->setColumnCount(1);
	mNotesCount = mInCompTodoCount = mCompTodoCount = 0;

	// The order of appending the items to the model is:
	// 1. Notes, 2. Incompleted to-dos, 3. Completed to-dos.
	// First get only the notes and populate the model.
	QList<ulong> entryIds = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags) (AgendaUtil::IncludeNotes));

	// Add the notes to the model.
	appendNotesToModel(entryIds);

	// Get the incompleted to-dos.
	entryIds.clear();
	entryIds = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags) (AgendaUtil::IncludeIncompletedTodos));

	// Add the incompleted to-dos to the model.
	appendInCompTodosToModel(entryIds);

	// Get the completed to-dos.
	entryIds.clear();
	entryIds = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags) (AgendaUtil::IncludeCompletedTodos));

	// Add the completed to-dos to the model.
	appendCompTodosToModel(entryIds);

	qDebug() << "notes: NotesModel::populateSourceModel <--";
}

/*!
	Updates the source model with the given entry `id'.
	The entry could be either have undergone a status change or have just been
	modified.

	\param id Of type ulong, identifies the entry which was changed.
 */
void NotesModel::updateSourceModel(ulong id)
{
	qDebug() << "notes: NotesModel::updateSourceModel -->";

	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if (entry.isNull()) {
		return;
	}

	// Entry can be updated in many ways.
	// 1. Notes can just change their content or can be marked/unmarked as
	// favourites. So for any change with a note, we move the model index
	// representing the note to the top of the list.
	if (AgendaEntry::TypeNote == entry.type()) {
		QModelIndex foundIndex;
		for (int iter = 0; iter < mNotesCount; iter++) {
			foundIndex = mSourceModel->index(iter, 0);
			if (!foundIndex.isValid()) {
				continue;
			}

			ulong noteId = foundIndex.data(
					NotesNamespace::IdRole).value<qulonglong>();
			if (id == noteId) {
				break;
			}
		}
		QStandardItem *item = mSourceModel->takeItem(foundIndex.row());
		mSourceModel->removeRow(foundIndex.row());
		mNotesCount--;

		// Insert the note to the 0th postion.
		insertNoteToModel(foundIndex, id);

	} else if (AgendaEntry::TypeTodo == entry.type()) {
		// 2. To-dos content can be modified.
		// So we check the status of the to-do. If the status has changed, we
		// check the previous status and remove the model index from the
		// corresponding section. Else we just update the to-do contents.
		QModelIndex foundIndex;
		for (int iter = mNotesCount;
				iter < mNotesCount + mInCompTodoCount + mCompTodoCount + 1;
				iter++) {
			foundIndex = mSourceModel->index(iter, 0);
			if (!foundIndex.isValid()) {
				continue;
			}

			ulong noteId = foundIndex.data(
					NotesNamespace::IdRole).value<qulonglong>();
			if (id == noteId) {
				break;
			}
		}

		// Get the status of the to-do.
		AgendaEntry::Status entryStatus = (AgendaEntry::Status) foundIndex.data(
				NotesNamespace::StatusRole).value<int>();

		if (entry.status() == entryStatus) {
			if (AgendaEntry::TodoNeedsAction == entryStatus) {
				// Make sure that the old model index is removed.
				mSourceModel->removeRow(foundIndex.row());
				mInCompTodoCount--;

				// Now insert the to-do.
				insertInCompTodoToModel(foundIndex, id);

			} else if (AgendaEntry::TodoCompleted == entryStatus) {
				// Make sure that the old model index is removed.
				mSourceModel->removeRow(foundIndex.row());
				mCompTodoCount--;

				// Now insert the completed to-do.
				insertCompTodoToModel(foundIndex, id);

			}
		} else {
			if (AgendaEntry::TodoNeedsAction == entryStatus) {
				// Here the to-do was marked completed. Hence we need to remove
				// the incompleted to-do and insert a completed to-do.
				mSourceModel->removeRow(foundIndex.row());
				mInCompTodoCount--;

				// Now insert the completed to-do.
				insertCompTodoToModel(foundIndex, id);

			} else if (AgendaEntry::TodoCompleted == entryStatus) {
				// Here the to-do was marked incompleted. Hence we need to
				// remove the completed to-do and insert an incompleted to-do.
				mSourceModel->removeRow(foundIndex.row());
				mCompTodoCount--;

				// Now insert the incompleted to-do.
				insertInCompTodoToModel(foundIndex, id);

			}
		}
	}
	qDebug() << "notes: NotesModel::updateSourceModel <--";
}

/*!
	Populates the source model. This slot is called when we get a db change
	notification from agenda server caused by another session.

	\param ids List of ids containing the entries that got changed.
 */
void NotesModel::populateSourceModel(QList<ulong> ids)
{
	qDebug() << "notes: NotesModel::populateSourceModel(ids) -->";

	Q_UNUSED(ids)

	QTimer::singleShot(1, this, SLOT(populateSourceModel()));

	qDebug() << "notes: NotesModel::populateSourceModel(ids) <--";
}

/*!
	Adds an entry to the model i.e., creates a QModelIndex and inserts it to the
	model.

	\param id The id of the entry to be added to the model.
 */
void NotesModel::addEntryToModel(ulong id)
{
	qDebug() << "notes: NotesModel::addEntryToModel -->";

	// We have different logic for adding a note or an incompleted to-do or a
	// completed to-do.
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if (entry.isNull()) {
		return;
	}
	bool notify = false;
	QModelIndex indexToNotify;

	if (AgendaEntry::TypeNote == entry.type()) {
		// All new notes are inserted at the top of the list by default.
		// Create a new model index at the 0th position.
		notify = insertNoteToModel(indexToNotify, id);

	} else if (AgendaEntry::TypeTodo == entry.type()) {
		if (AgendaEntry::TodoNeedsAction == entry.status()) {
			// Try to insert the to-do at its appropriate position.
			notify = insertInCompTodoToModel(indexToNotify, id);

		} else if (AgendaEntry::TodoCompleted == entry.status()) {
			// Try to insert the to-do at its appropriate position.
			insertCompTodoToModel(indexToNotify, id);
		}
	}

	if (notify) {
		emit rowAdded(indexToNotify);
	}

	qDebug() << "notes: NotesModel::addEntryToModel <--";
}

/*!
	Deletes an entry from model.

	\param Deletes the entry from model and hence updating the view.
 */
void NotesModel::removeEntryFromModel(ulong id)
{
	for (int iter = 0; iter < mSourceModel->rowCount(); iter++) {
		QModelIndex mdlIndex = mSourceModel->index(iter, 0);

		if (!mdlIndex.isValid()) {
			return;
		}

		ulong noteId = mdlIndex.data(
				NotesNamespace::IdRole).value<qulonglong>();
		AgendaEntry::Type entryType = (AgendaEntry::Type) mdlIndex.data(
				NotesNamespace::TypeRole).value<int>();
		AgendaEntry::Status entryStatus = (AgendaEntry::Status) mdlIndex.data(
				NotesNamespace::StatusRole).value<int>();

		if (id == noteId) {
			mSourceModel->removeRow(mdlIndex.row());

			// Update the note, to-do counts depending on the type.
			if (entryType == AgendaEntry::TypeNote) {
				mNotesCount--;
			} else if (entryType == AgendaEntry::TypeTodo) {
				if (entryStatus == AgendaEntry::TodoCompleted) {
					mCompTodoCount--;
				} else if (entryStatus == AgendaEntry::TodoNeedsAction) {
					mInCompTodoCount--;
				}
			}
			break;
		}
	}
}

/*!
	Modifies the content of a given `row' with the data of the entry given by
	`id'.

	\param id The id of the entry.
	\param row The row corresponding to the given id.
 */
void NotesModel::modifyEntryInModel(ulong id, int row)
{
	qDebug() << "notes: NotesModel::modifyEntryInModel -->";

	// Get the model index.
	QModelIndex modelIndex = mSourceModel->index(row, 0);
	Q_ASSERT(modelIndex.isValid());

	// Fetch the entry
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	Q_ASSERT(!entry.isNull());

	// Set the note id.
	mSourceModel->setData(
			modelIndex, (qulonglong) id, NotesNamespace::IdRole);
	// Set the type of the note.
	mSourceModel->setData(
			modelIndex, entry.type(), NotesNamespace::TypeRole);

	// Set the display data to the index.
	if (AgendaEntry::TypeNote == entry.type()) {
		// Read modification time from agenda entry
		QString displayText;
		QString timeText(qtTrId("txt_notes_dblist_val_modified_on_1_2"));
		QString modifiedDateText =
				entry.startTime().date().toString("dd/MM/yyyy");
		QString modifiedTimeText =
						entry.startTime().time().toString("hh:mm ap");
		displayText = timeText.arg(modifiedDateText,modifiedTimeText);

		QStringList stringList;
		stringList << entry.description().left(15) << displayText;
		// Set the data.
		mSourceModel->setData(
				modelIndex, stringList, Qt::DisplayRole);
		// Set the favourite icon.
		if (entry.favourite()) {
			QList<QVariant> iconList;
			iconList.append(QVariant(QVariant::Invalid));
			iconList.append(HbIcon("qtg_mono_favourites"));
			mSourceModel->setData(modelIndex, iconList, Qt::DecorationRole);
		} else {
			QList<QVariant> iconList;
			iconList.append(QVariant(QVariant::Invalid));
			iconList.append(QVariant(QVariant::Invalid));
			mSourceModel->setData(modelIndex, iconList, Qt::DecorationRole);
		}
	} else if (AgendaEntry::TypeTodo == entry.type()) {
		QStringList stringList;
		stringList << entry.summary();
		if (AgendaEntry::TodoNeedsAction == entry.status()) {
			// Read due date from agenda entry
			QString displayText;
			QString timeText(hbTrId("txt_notes_dblist_val_due_on_1"));
			QString dueDateText =
					entry.startTime().toString("dd/MM/yyyy");
			displayText = timeText.arg(dueDateText);
			stringList << displayText;
		}
		// Set the data.
		mSourceModel->setData(
				modelIndex, stringList, Qt::DisplayRole);

		// Set the to-do done/undone icon based on the status.
		QList<QVariant> iconList;
		if (AgendaEntry::TodoCompleted == entry.status()) {
			iconList.append(HbIcon("qtg_small_todo_done"));
		} else {
			iconList.append(HbIcon("qtg_small_todo"));
		}

		// Set the alarm icon if reminder is set.
		if (!entry.alarm().isNull()) {
			iconList.append(HbIcon("qtg_mono_alarm"));
		} else {
			iconList.append(QVariant(QVariant::Invalid));
		}
		// Set the icons.
		mSourceModel->setData(modelIndex, iconList, Qt::DecorationRole);
	}

	qDebug() << "notes: NotesModel::modifyEntryInModel <--";
}

/*!
	Appends notes to the model.

	\param ids QList of uids containing the notes.
 */
void NotesModel::appendNotesToModel(QList<ulong> &ids)
{
	qDebug() << "notes: NotesModel::appendNotesToModel -->";

	// Iterate and add notes to the model.
	mSourceModel->insertRows(mSourceModel->rowCount(), ids.count());
	int rowCount = mSourceModel->rowCount();
	for (int idIter = 0, modelIter = rowCount - ids.count();
			idIter < ids.count(); idIter++, modelIter++) {
		// Fetch the note details.
		ulong id = ids[rowCount - 1 - idIter];
		AgendaEntry entry = mAgendaUtil->fetchById(id);

		if (AgendaEntry::TypeNote != entry.type()) {
			continue;
		}

		// Create a model index.
		QModelIndex mdlIndex = mSourceModel->index(qAbs(modelIter), 0);

		// Set the note id.
		mSourceModel->setData(
				mdlIndex, (qulonglong) id, NotesNamespace::IdRole);
		// Set the type of the note.
		mSourceModel->setData(
				mdlIndex, entry.type(), NotesNamespace::TypeRole);
		// Set the favourite property.
		mSourceModel->setData(
				mdlIndex, entry.favourite(), NotesNamespace::FavouriteRole);

		// Set the display data now.
		// Read modification time from agenda entry
		QString displayText;
		QString timeText(hbTrId("txt_notes_dblist_val_modified_on_1_2"));
		QString modifiedDateText =
				entry.lastModifiedDateTime().date().toString("dd/MM/yyyy");
		QString modifiedTimeText =
						entry.lastModifiedDateTime().time().toString("hh:mm ap");
		displayText = timeText.arg(modifiedDateText,modifiedTimeText);

		QStringList stringList;
		stringList << entry.description().left(100) << displayText;
		// Set the data.
		mSourceModel->setData(
				mdlIndex, stringList, Qt::DisplayRole);

		if (entry.favourite()) {
			// Set the favourite icon.
			QList<QVariant> iconList;
			iconList.append(QVariant(QVariant::Invalid));
			iconList.append(HbIcon("qtg_mono_favourites"));
			mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);
		} else {
			QList<QVariant> iconList;
			iconList.append(QVariant(QVariant::Invalid));
			iconList.append(QVariant(QVariant::Invalid));
			mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);
		}

		// Update the notes count.
		mNotesCount++;
	}

	qDebug() << "notes: NotesModel::appendNotesToModel <--";
}

/*!
	Appends incompleted to-dos to the model.

	\param ids QList of uids containing the incompleted to-dos.
 */
void NotesModel::appendInCompTodosToModel(QList<ulong> &ids)
{
	qDebug() << "notes: NotesModel::appendInCompTodosToModel -->";

	// Iterate and add incomplete to-do to the model.
	mSourceModel->insertRows(mSourceModel->rowCount(), ids.count());
	int rowCount = mSourceModel->rowCount();
	for (int idIter = 0, modelIter = rowCount - ids.count();
			idIter < ids.count(); idIter++, modelIter++) {
		// Fetch the to-do details.
		ulong id = ids[idIter];
		AgendaEntry entry = mAgendaUtil->fetchById(id);

		if (AgendaEntry::TypeTodo != entry.type()) {
			continue;
		}

		if (AgendaEntry::TodoNeedsAction != entry.status()) {
			continue;
		}

		// Create a model index.
		QModelIndex mdlIndex = mSourceModel->index(modelIter, 0);

		// Set the to-do id.
		mSourceModel->setData(
				mdlIndex, (qulonglong) id, NotesNamespace::IdRole);
		// Set the type of the to-do.
		mSourceModel->setData(
				mdlIndex, entry.type(), NotesNamespace::TypeRole);
		// Set the status of the to-do.
		mSourceModel->setData(
				mdlIndex, (int) entry.status(), NotesNamespace::StatusRole);

		// Set the display data now.
		QStringList stringList;
		stringList << entry.summary();
		if (AgendaEntry::TodoNeedsAction == entry.status()) {
			// Read due date from agenda entry
			QString displayText;
			QString timeText(hbTrId("txt_notes_dblist_val_due_on_1"));
			QString dueDateText =
					entry.startTime().toString("dd/MM/yyyy");
			displayText = timeText.arg(dueDateText);
			stringList << displayText;
		}
		// Set the data.
		mSourceModel->setData(mdlIndex, stringList, Qt::DisplayRole);
		// Set the to-do done/undone icon based on the status.
		QList<QVariant> iconList;
		iconList.append(HbIcon("qtg_small_todo"));

		// Set the alarm icon if reminder is set.
		if (!entry.alarm().isNull()) {
			iconList.append(HbIcon("qtg_mono_alarm"));
		} else {
			iconList.append(QVariant(QVariant::Invalid));
		}

		// Set the icons.
		mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);

		// Update the incompleted to-do count.
		mInCompTodoCount++;
	}

	qDebug() << "notes: NotesModel::appendInCompTodosToModel <--";
}

/*!
	Appends incompleted to-dos to the model.

	\param ids QList of uids containing the completed to-dos.
 */
void NotesModel::appendCompTodosToModel(QList<ulong> &ids)
{
	qDebug() << "notes: NotesModel::appendCompTodosToModel -->";

	// Iterate and add complete to-do to the model.
	mSourceModel->insertRows(mSourceModel->rowCount(), ids.count());
	int rowCount = mSourceModel->rowCount();
	for (int idIter = 0, modelIter = rowCount - ids.count();
			idIter < ids.count(); idIter++, modelIter++) {
		// Fetch the to-do details.
		ulong id = ids[idIter];
		AgendaEntry entry = mAgendaUtil->fetchById(id);

		if (AgendaEntry::TypeTodo != entry.type()) {
			continue;
		}

		if (AgendaEntry::TodoCompleted != entry.status()) {
			continue;
		}

		// Create a model index.
		QModelIndex mdlIndex = mSourceModel->index(modelIter, 0);

		// Set the to-do id.
		mSourceModel->setData(
				mdlIndex, (qulonglong) id, NotesNamespace::IdRole);
		// Set the type of the to-do.
		mSourceModel->setData(
				mdlIndex, entry.type(), NotesNamespace::TypeRole);
		// Set the status of the to-do.
		mSourceModel->setData(
				mdlIndex, (int) entry.status(), NotesNamespace::StatusRole);

		// Set the display data now.
		QStringList stringList;
		stringList << entry.summary();
		if (AgendaEntry::TodoCompleted == entry.status()) {
			// Read completed date from agenda entry
			QString displayText;
			QString timeText(hbTrId("txt_notes_dblist_val_completed_on_1"));
			QString completedTimeText =
					entry.completedDateTime().toString("dd/MM/yyyy");
			displayText = timeText.arg(completedTimeText);
			stringList << displayText;
		}
		// Set the data.
		mSourceModel->setData(mdlIndex, stringList, Qt::DisplayRole);

		// Set the to-do done icon.
		QList<QVariant> iconList;
		iconList.append(HbIcon("qtg_small_todo_done"));
		// To-do is already completed. No need to set alarm.
		iconList.append(QVariant(QVariant::Invalid));

		// Set the icons.
		mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);

		// Update the completed to-do count.
		mCompTodoCount++;
	}

	qDebug() << "notes: NotesModel::appendCompTodosToModel <--";
}

/*!
	Inserts a note at the 0th position.

	\param index Will contain the model index of the newly created row.
	\param id The id of the note to be inserted.
	\return bool true if the insertion was successful, false otherwise.
 */
bool NotesModel::insertNoteToModel(QModelIndex &index, ulong id)
{
	qDebug() << "notes: NotesModel::insertNoteToModel -->";

	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if (entry.isNull()) {
		return false;
	}

	mSourceModel->insertRows(0, 1);
	QModelIndex mdlIndex = mSourceModel->index(0, 0);

	// Now set the details of this note.
	// Set the note id.
	mSourceModel->setData(
			mdlIndex, (qulonglong) id, NotesNamespace::IdRole);
	// Set the type of the note.
	mSourceModel->setData(
			mdlIndex, entry.type(), NotesNamespace::TypeRole);
	// Set the favourite property.
	mSourceModel->setData(
			mdlIndex, entry.favourite(), NotesNamespace::FavouriteRole);

	// Set the display data now.
	// Read modification time from agenda entry
	QString displayText;
	QString timeText(hbTrId("txt_notes_dblist_val_modified_on_1_2"));
	QString modifiedDateText =
			entry.lastModifiedDateTime().date().toString("dd/MM/yyyy");
	QString modifiedTimeText =
					entry.lastModifiedDateTime().time().toString("hh:mm ap");
	displayText = timeText.arg(modifiedDateText,modifiedTimeText);

	QStringList stringList;
	stringList << entry.description().left(100) << displayText;
	// Set the data.
	mSourceModel->setData(
			mdlIndex, stringList, Qt::DisplayRole);

	// Set the favourite icon.
	if (entry.favourite()) {
		QList<QVariant> iconList;
		iconList.append(QVariant(QVariant::Invalid));
		iconList.append(HbIcon("qtg_mono_favourites"));
		mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);
	} else {
		QList<QVariant> iconList;
		iconList.append(QVariant(QVariant::Invalid));
		iconList.append(QVariant(QVariant::Invalid));
		mSourceModel->setData(mdlIndex, iconList, Qt::DecorationRole);
	}
	// Update the notes count.
	mNotesCount++;

	index = mdlIndex;

	qDebug() << "notes: NotesModel::insertNoteToModel <--";

	return true;
}

/*!
	Inserts an incomplete to-do at a position based on the id of the
	to-do given.

	\param index This will hold the model index of the newly inserted to-do.
	\param id The id of the to-do to be inserted.
	\return bool true if the Insertion was successful, false otherwise.
 */
bool NotesModel::insertInCompTodoToModel(QModelIndex &index, ulong id)
{
	qDebug() << "notes: NotesModel::insertInCompTodoToModel -->";

	bool success = false;

	// Fetch the entry first.
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if (entry.isNull()) {
		return success;
	}


	// First fetch the list of incompleted to-dos.
	QList<ulong> entryIds = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags) (AgendaUtil::IncludeIncompletedTodos));

	// Get the index of the added to-do in the above list.
	int newTodoIndex = -1;
	for (int iter = 0; iter < entryIds.count(); iter++) {
		if (id == entryIds[iter]) {
			newTodoIndex = iter;
			break;
		}
	}

	// We display the to-dos in the same order that we get from the
	// agenda server. This is a new to-do that has been added. Hence we
	// have to insert a model index at the same position as we found the
	// id in the list of to-dos above.
	// Calculate the position.
	if (-1 != newTodoIndex) {
		mSourceModel->insertRows(mNotesCount + newTodoIndex, 1);
		QModelIndex newModelIndex =
				mSourceModel->index(mNotesCount + newTodoIndex, 0);

		// We continue only if the index is valid.
		if (newModelIndex.isValid()) {
			// Things look ok, start adding data to the model index.
			// Set the to-do id.
			mSourceModel->setData(
					newModelIndex, (qulonglong) id, NotesNamespace::IdRole);
			// Set the type of the to-do.
			mSourceModel->setData(
					newModelIndex, entry.type(), NotesNamespace::TypeRole);
			// Set the status of the to-do.
			mSourceModel->setData(
					newModelIndex, (int) entry.status(),
					NotesNamespace::StatusRole);

			// Set the display data now.
			QStringList stringList;
			stringList << entry.summary();
			if (AgendaEntry::TodoNeedsAction == entry.status()) {
				// Read due date from agenda entry
				QString displayText;
				QString timeText(hbTrId("txt_notes_dblist_val_due_on_1"));
				QString dueDateText =
						entry.startTime().toString("dd/MM/yyyy");
				displayText = timeText.arg(dueDateText);
				stringList << displayText;
			}
			// Set the data.
			mSourceModel->setData(
					newModelIndex, stringList, Qt::DisplayRole);
			// Set the to-do done/undone icon based on the status.
			QList<QVariant> iconList;
			iconList.append(HbIcon("qtg_small_todo"));

			// Set the alarm icon if reminder is set.
			if (!entry.alarm().isNull()) {
				iconList.append(HbIcon("qtg_mono_alarm"));
			} else {
				iconList.append(QVariant(QVariant::Invalid));
			}
			// Set the icons.
			mSourceModel->setData(
					newModelIndex, iconList, Qt::DecorationRole);

			// Update the incompleted to-do count.
			mInCompTodoCount++;

			success = true;
			index = newModelIndex;
		}
	}

	qDebug() << "notes: NotesModel::insertInCompTodoToModel <--";

	return success;
}

/*!
	Inserts a complete to-do at a position based on the id of the
	to-do given.

	\param index This will hold the model index of the newly inserted to-do.
	\param id The id of the to-do to be inserted.
	\return bool true if the Insertion was successful, false otherwise.
 */
bool NotesModel::insertCompTodoToModel(QModelIndex &index, ulong id)
{
	qDebug() << "notes: NotesModel::insertCompTodoToModel -->";

	bool success = false;

	// Fetch the entry first.
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	if (entry.isNull()) {
		return success;
	}

	// Now fetch all the incomplete to-dos again and add them.
	QList<ulong> entryIds = mAgendaUtil->entryIds(
			(AgendaUtil::FilterFlags) (AgendaUtil::IncludeCompletedTodos));

	// Get the index of the added to-do in the above list.
	int newTodoIndex = -1;
	for (int iter = 0; iter < entryIds.count(); iter++) {
		if (id == entryIds[iter]) {
			newTodoIndex = iter;
			break;
		}
	}

	// We display the to-dos in the same order that we get from the
	// agenda server. This is a new to-do that has been added. Hence we
	// have to insert a model index at the same position as we found the
	// id in the list of to-dos above.
	// Calculate the position.
	if (-1 != newTodoIndex) {
		mSourceModel->insertRows(
				mNotesCount + mInCompTodoCount + newTodoIndex, 1);
		QModelIndex newModelIndex = mSourceModel->index(
						mNotesCount + mInCompTodoCount + newTodoIndex, 0);

		// We continue only if the index is valid.
		if (newModelIndex.isValid()) {
			// Things look ok, start adding data to the model index.
			AgendaEntry entry = mAgendaUtil->fetchById(id);

			// Set the to-do id.
			mSourceModel->setData(
					newModelIndex, (qulonglong) id, NotesNamespace::IdRole);
			// Set the type of the to-do.
			mSourceModel->setData(
					newModelIndex, entry.type(), NotesNamespace::TypeRole);
			// Set the status of the to-do.
			mSourceModel->setData(
					newModelIndex, (int) entry.status(),
					NotesNamespace::StatusRole);

			// Set the display data now.
			QStringList stringList;
			stringList << entry.summary();
			if (AgendaEntry::TodoCompleted == entry.status()) {
				// Read completed date from agenda entry
				QString displayText;
				QString timeText(hbTrId("txt_notes_dblist_val_completed_on_1"));
				QString completedTimeText =
						entry.completedDateTime().toString("dd/MM/yyyy");
				displayText = timeText.arg(completedTimeText);
				stringList << displayText;
			}
			// Set the data.
			mSourceModel->setData(
					newModelIndex, stringList, Qt::DisplayRole);

			// Set the to-do done icon.
			QList<QVariant> iconList;
			iconList.append(HbIcon("qtg_small_todo_done"));
			// To-do is already completed. No need to set alarm.
			iconList.append(QVariant(QVariant::Invalid));

			// Set the icons.
			mSourceModel->setData(
					newModelIndex, iconList, Qt::DecorationRole);

			// Update the completed to-do count.
			mCompTodoCount++;

			success = true;
			index = newModelIndex;
		}
	}

	qDebug() << "notes: NotesModel::insertCompTodoToModel <--";

	return success;
}

// End of file	--Don't remove this.
