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
* Description: This is the Definition file for NotesModelHandler class.
*
*/

// System includes
#include <QtGui>
// User includes
#include "notesmodelhandler.h"
#include "agendautil.h"
#include "notesmodel.h"

/*!
	\class NotesModelHandler

	This is responsible for holding the notesmodel and agendautil objects.
 */

/*!
	Default constructor.
 */
NotesModelHandler::NotesModelHandler(QObject *parent)
:QObject(parent),
 mAgendaUtil(0),
 mNotesModel(0)
{
	QT_TRAP_THROWING(mAgendaUtil = new AgendaUtil(this));
	Q_ASSERT_X(mAgendaUtil, "notesviewmanager.cpp", "mAgendaUtil is 0");

	// Construct the source model here.
	mNotesModel = new NotesModel(mAgendaUtil, this);
}

/*!
	Destructor
 */
NotesModelHandler::~NotesModelHandler()
{
	if (mAgendaUtil) {
		delete mAgendaUtil;
		mAgendaUtil = 0;
	}
	if (mNotesModel) {
		delete mNotesModel;
		mNotesModel = 0;
	}
}

/*!
	Returns a pointer to the agenda interface.

	\return AgendaUtil pointer.
 */
AgendaUtil *NotesModelHandler::agendaInterface()
{
	Q_ASSERT(mAgendaUtil);

	return mAgendaUtil;
}

/*!
	Returns a pointer to the notes application model.

	\return NotesModel pointer.
 */
NotesModel *NotesModelHandler::notesModel()
{
	Q_ASSERT(mNotesModel);

	return mNotesModel;
}

// End of file	--Don't remove this.
