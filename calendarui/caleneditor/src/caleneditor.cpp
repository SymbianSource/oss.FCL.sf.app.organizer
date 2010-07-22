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
* Definition of CalenEditor class.
*
*/

// System includes.
#include <QDateTime>

// User includes
#include <agendaentry.h>
#include <agendautil.h>

#include "caleneditor.h"
#include "caleneditor_p.h"

/*!
	\class CalenEditor
 */
/*!
	Constructor.

	\param parent HbView pointer.
 */
CalenEditor::CalenEditor(QObject *parent)
:QObject(parent),d_ptr(new CalenEditorPrivate(0, this))
{
	
}

/*!
	Constructor.

	\param agendaUtil pointer to agenda utility
	\param parent HbView pointer.
 */
CalenEditor::CalenEditor(AgendaUtil *agendaUtil, QObject *parent)
:QObject(parent), d_ptr(new CalenEditorPrivate(agendaUtil, this))
{
	
}

/*!
	Destructor.
 */
CalenEditor::~CalenEditor()
{
	if (d_ptr) {
		delete d_ptr;
	}
}

/*!
	Shows the caleneditor by parsing a .vcs file which will be edited.
	\param handle reference to QFile handle.
	\param launchCalendar bool to tell whether calendar has to be launched after 
	 coming back from editor
 */
void CalenEditor::edit(const QFile &handle, bool launchCalendar)
{
	d_ptr->edit(handle, launchCalendar);
}

/*!
	Shows the caleneditor, by parsing an AgendaEntry.
	\param entry An object of AgendaEntry.
	\param launchCalendar bool to tell whether calendar has to be launched after 
	 coming back from editor
 */
void CalenEditor::edit(AgendaEntry entry, bool launchCalendar)
{
	if(entry.isNull()) {
		return;
	}
	d_ptr->edit(entry, launchCalendar);
}

/*!
	Shows the caleneditor, by using the entry id provided.
	\param id entry id.
 */
void CalenEditor::edit(ulong id, bool launchCalendar)
{
	d_ptr->edit(id, launchCalendar);
}

/*!
	Creates a new entry based on the create type.It launches the
	editor for editing
	\param QDateTime newEntryDateTime is the event start time.
	\param launchCalendar bool to tell whether calendar has to be launched after 
	 coming back from editor
	\param type type of the editor to be shown for creating new entry.
 */
void CalenEditor::create(QDateTime newEntryDateTime, bool launchCalendar, 
                         CalenEditor::CreateType type)
{
	d_ptr->create(type, newEntryDateTime, launchCalendar);
}

/*!
	Creates a new entry based on the create type by copying the
	details of passed entry.It launches the editor for editing
	\param entry AgendaEntry from which new entry has to be created
	\param launchCalendar bool to tell whether calendar has to be launched after 
	 coming back from editor
	\param type type of the editor to be shown for creating new entry.
 */
void CalenEditor::create(AgendaEntry entry, bool launchCalendar, 
                         CalenEditor::CreateType type)
{
	if(entry.isNull()) {
		return;
	}
	d_ptr->create(type, entry, launchCalendar);
}

// End of file	--Don't remove this.
