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
* Description: Definition of AgendaEventViewer class
*
*/

// User Includes.
#include "agendaeventviewer.h"
#include "agendaeventviewer_p.h"
#include "agendaeventview.h"

/*!
	\class AgendaEventViewer

	To view entry in agenda event viewer.
	To delete this class object use deleteLater().
 */

/*!
	\fn void AgendaEventViewer::viewingCompleted(bool status = true)

	Signal is emitted when viewing of the agenda entry is complete
	After receiving this signal use deleteLater() to detele this class object.

 */

/*!
	\fn void AgendaEventViewer::editingStarted()

	Signal is emitted when editing of the agenda entry is start

 */

/*!
	\fn void AgendaEventViewer::editingCompleted()

	Signal is emitted when editing of the agenda entry is complete

 */

/*!
	\fn void AgendaEventViewer::deletingStarted()

	Signal is emitted when deleting of the agenda entry is start

 */

/*!
	\fn void AgendaEventViewer::deletingCompleted()

	Signal is emitted when deleting of the agenda entry is complete

 */

/*!
	Constructor.

	\param parent Pointer to QObject.
 */
AgendaEventViewer::AgendaEventViewer(QObject *parent)
:QObject(parent), d_ptr(new AgendaEventViewerPrivate(0, this))
{

}

/*!
	Constructor

	 \param agendaUtil Pointer to AgendaUtil.
	 \param parent Pointer to QObject.
 */
AgendaEventViewer::AgendaEventViewer(AgendaUtil *agendaUtil, QObject *parent)
: QObject(parent), d_ptr(new AgendaEventViewerPrivate(agendaUtil, this))
{
	// Nothing
}

/*!
	Destructor.
 */
AgendaEventViewer::~AgendaEventViewer()
{
	if (d_ptr) {
		delete d_ptr;
	}
}

/*!
	Launches the event viewer.Id is used for fetching
	the calendar entry information.

	\param id Local Uid of the calendar entry to be viewed
 */
void AgendaEventViewer::view(const ulong id, Actions action)
{
	d_ptr->view(id, action);
}

/*!
	Launches the event viewer.File handle of any vcs/ics file can be given as 
	input to view the calendar entry information.

	\param fileHandle reference to the file handle of vcs/ics file
 */
void AgendaEventViewer::view(const QFile &fileHandle, Actions action)
{
	d_ptr->view(fileHandle, action);
}

/*!
	Launches the event viewer.
	AgendaEntry can be given as input to view the calendar entry information.
	
	\param entry Object of calendar entry to be viewed
 */
void AgendaEventViewer::view(AgendaEntry entry, Actions action)
{
	d_ptr->view(entry, action);
}

// End of file
