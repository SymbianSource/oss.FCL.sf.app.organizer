/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Definition of AgendaEventViewerPrivate class
*
*/

// System includes.
#include <QDebug>
#include <QFile>
#include <QDir>

// User includes.
#include <agendautil.h>
#include <agendaentry.h>
#include "agendaeventviewer_p.h"
#include "agendaeventviewer.h"
#include "agendaeventview.h"

/*!
	\class AgendaEventViewerPrivate

	Private class implementation of AgendaEventViewer.It is responsible for
	launching the agenda event viewer.
 */


/*!
	Constructor

	\param agendaUtil Pointer to AgendaUtil.
	\param parent Pointer to QObject.
 */
AgendaEventViewerPrivate::AgendaEventViewerPrivate(
		AgendaUtil *agendaUtil, QObject *parent)
: QObject(parent)
{
	qDebug() <<"AgendaEventViewerPrivate::AgendaEventViewerPrivate -->";

	// Get the q-pointer.from parent
	q_ptr = static_cast<AgendaEventViewer *> (parent);

	// Check for agendaUtil sent by client. If null agendaUtil will be
	// created and owned by AgendaEventViewerPrivate
	if (!agendaUtil) {
		mAgendaUtil = new AgendaUtil(parent);
		mViewerOwnsAgendaUtil = true;
	} else {
		mAgendaUtil = agendaUtil;
		mViewerOwnsAgendaUtil = false;
	}

	// Register for the entry change signal when the same entry is updated
	// by other application.This is to handle the db conflict changes.
	connect(
			mAgendaUtil, SIGNAL(entriesChanged(QList<ulong> )),
			this, SLOT(handleEntriesChanged(QList<ulong> )));

	qDebug() <<"AgendaEventViewerPrivate::AgendaEventViewerPrivate <--";
}

/*!
	Destructor.
 */
AgendaEventViewerPrivate::~AgendaEventViewerPrivate()
{
	qDebug() <<"AgendaEventViewerPrivate::~AgendaEventViewerPrivate -->";

	if (mViewerOwnsAgendaUtil) {
		delete mAgendaUtil;
		mAgendaUtil = 0;
	}

	qDebug() <<"AgendaEventViewerPrivate::~AgendaEventViewerPrivate <--";
}

/*!
	Launches the event viewer.Id is used for fetching
	the calendar entry information.

	\param id Local Uid of the calendar entry to be viewed
 */
void AgendaEventViewerPrivate::view(const ulong id, 
                                    AgendaEventViewer::Actions action)
{
	qDebug() <<"AgendaEventViewerPrivate::view(id) -->";
	
	AgendaEntry entry = mAgendaUtil->fetchById(id);
	
	if (entry.isNull()) {
		return;
	}
	// Construct the agenda event view
	mAgendaEventView = new AgendaEventView(this);
	mAgendaEventView->execute(entry, action);

	qDebug() <<"AgendaEventViewerPrivate::view(id) <--";
}

/*!
	Launches the event viewer.File handle of any vcs/ics
	file can be given as input to view the calendar entry information.

	\param fileHandle reference to the file handle of vcs/ics file
 */
void AgendaEventViewerPrivate::view(const QFile &fileHandle, 
                                    AgendaEventViewer::Actions action)
{
	qDebug() <<"AgendaEventViewerPrivate::view(fileHandle) -->";

	// Using calendar importer read the filehandle and generate agenda entry
	QString filePath = fileHandle.fileName();
	QString nativeFilePath = QDir::toNativeSeparators(filePath);
	AgendaEntry entry;
	mAgendaUtil->importvCalendar(nativeFilePath, entry);
	if (!entry.isNull()) {
		mAgendaEventView = new AgendaEventView(this);
		mAgendaEventView->execute(entry, action);
	} else {
	    q_ptr->viewingCompleted(QDateTime::currentDateTime().date());
	}
	qDebug() <<"AgendaEventViewerPrivate::view(fileHandle) <--";
}

/*!
	Launches the event viewer.AgendaEntry can be given as input to view the
	calendar entry information
 */
void AgendaEventViewerPrivate::view(AgendaEntry entry, 
                                    AgendaEventViewer::Actions action)
{
	qDebug() <<"AgendaEventViewerPrivate::view(entry) -->";

	if (entry.isNull()) {
			return;
		}
	// Construct the agenda event view
	mAgendaEventView = new AgendaEventView(this);
	mAgendaEventView->execute(entry, action);

	qDebug() <<"AgendaEventViewerPrivate::view(entry) <--";
}

/*!
	Emits the signal viewing completed to the clients

	\param status true if viewing completed otherwise false.
 */
void AgendaEventViewerPrivate::viewingCompleted(const QDate date)
{
	qDebug() <<"AgendaEventViewerPrivate::viewingCompleted -->";

	emit q_ptr->viewingCompleted(date);

	// Cleanup viewer.
	if (mAgendaEventView) {
		mAgendaEventView->deleteLater();
	}

	qDebug() <<"AgendaEventViewerPrivate::viewingCompleted -->";
}

/*!
	Emits the signal editing started to the clients
 */
void AgendaEventViewerPrivate::editingStarted()
{
	qDebug() <<"AgendaEventViewerPrivate::editingStarted -->";

	emit q_ptr->editingStarted();

	qDebug() <<"AgendaEventViewerPrivate::editingStarted -->";
}

/*!
	Emits the signal editing completed to the clients
 */
void AgendaEventViewerPrivate::editingCompleted()
{
	qDebug() <<"AgendaEventViewerPrivate::editingCompleted -->";

	emit q_ptr->editingCompleted();

	qDebug() <<"AgendaEventViewerPrivate::editingCompleted -->";
}

/*!
	Emits the signal deleting started to the clients
 */
void AgendaEventViewerPrivate::deletingStarted()
{
	qDebug() <<"AgendaEventViewerPrivate::deletingStarted -->";

	emit q_ptr->deletingStarted();

	qDebug() <<"AgendaEventViewerPrivate::deletingStarted -->";
}

/*!
	Emits the signal deleting completed to the clients
 */
void AgendaEventViewerPrivate::deletingCompleted()
{
	qDebug() <<"AgendaEventViewerPrivate::deletingCompleted -->";

	emit q_ptr->deletingCompleted();

	qDebug() <<"AgendaEventViewerPrivate::deletingCompleted -->";
}

// End of file
