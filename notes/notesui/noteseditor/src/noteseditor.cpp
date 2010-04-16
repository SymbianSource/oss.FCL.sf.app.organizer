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
* Definition of NotesEditor class.
*
*/

// User includes
#include "noteseditor.h"
#include "noteseditor_p.h"
#include "agendautil.h"
#include "agendaentry.h"

/*!
	\class NotesEditor

	TODO: Detailed description here, along with usage details.
 */

/*!
	\fn void NotesEditor::editingCompleted(bool status = true)

	This signal is emitted when editing/creation of a note/to-do is completed.

	\param status This indicates whether the note/to-do was saved or not.
					This will be `true' if the note was saved by the user.
					`false' otherwise.
 */

/*!
	Constructor.

	\param parent QObject pointer.
 */
NotesEditor::NotesEditor(QObject *parent)
:QObject(parent),d_ptr(new NotesEditorPrivate(0, parent))
{
	// Nothing yet.
}


/*!
	Constructor.

	\param agendaUtil pointer to agenda utility
	\param parent QObject pointer.
 */
NotesEditor::NotesEditor(AgendaUtil *agendaUtil, QObject *parent)
:QObject(parent), d_ptr(new NotesEditorPrivate(agendaUtil, parent))
{
	// Nothing yet.
}

/*!
	Destructor.
 */
NotesEditor::~NotesEditor()
{
	if (d_ptr) {
		delete d_ptr;
	}
}

/*!
	Shows the noteseditor. The argument acts as the description for the note.

	\param string reference to file string/buffer.
 */
void NotesEditor::edit(const QString &string)
{
	d_ptr->edit(string);
}

/*!
	Shows the noteseditor by parsing a .vcs which could be of a To-do or a plain
	.txt file which will be edited as a note.

	\param handle reference to QFile handle.
 */
void NotesEditor::edit(const QFile &handle)
{
	d_ptr->edit(handle);
}

/*!
	Shows the noteseditor, by parsing an AgendaEntry.

	\param entry An object of AgendaEntry.
 */
void NotesEditor::edit(AgendaEntry entry)
{
	d_ptr->edit(entry);
}

/*!
	Shows the noteseditor, by fetching the note using the entry id provided.

	\param id entry id of the note.
 */
void NotesEditor::edit(ulong id)
{
	d_ptr->edit(id);
}

/*!
	Creates a new note or to-do based on the create type.It lanuches the
	respective editors for editing

	\type type of the editor to be shown for creating new note and new to-do
 */
void NotesEditor::create(NotesEditor::CreateType type)
{
	d_ptr->create(type);
}

/*!
	Closes the notes editor

	\param type Type of the close required.
 */
ulong NotesEditor::close(NotesEditor::CloseType type)
{
	return d_ptr->close(type);
}

// End of file	--Don't remove this.
