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
* This file contains the NotesAppControllerIfImpl class definition.
*
*/

// User includes
#include "notesappcontrollerifimpl.h"
#include "notesmodelhandler.h"
#include "notesappcontroller.h"
#include "notesviewmanager.h"

/*!
	\class NotesAppControllerIfImpl

	This class implements the NotesAppControllerIf interface which is used
	by the views and other componets to access the services of
	notesviewmanager and notesmodelhandler.
 */

/*!
	Constructor.

	\param controller Pointer to an object of NotesAppController.
	\param parent QObject pointer.
 */
NotesAppControllerIfImpl::NotesAppControllerIfImpl(
		NotesAppController *controller, QObject *parent)
:QObject(parent),
 mAppController(controller)
{
	// Nothing yet.
}

/*!
	Destructor.
 */
NotesAppControllerIfImpl::~NotesAppControllerIfImpl()
{
	// Nothing yet.
}

/*!
	From NotesAppControllerIf.
	Returns a pointer to agendautil object.

	\return AgendaUtil* Pointer to agendautil object.
	\sa NotesAppControllerIf
 */
AgendaUtil *NotesAppControllerIfImpl::agendaUtil()
{
	return mAppController->mNotesModelHandler->agendaInterface();
}

/*!
	From NotesAppControllerIf.
	Returns a pointer to notesmodel object.

	\return NotesModel* Pointer to notesmodel object.
	\sa NotesAppControllerIf
 */
NotesModel *NotesAppControllerIfImpl::notesModel()
{
	return mAppController->mNotesModelHandler->notesModel();
}

/*!
	From NotesAppControllerIf.
	Issues a request to NotesViewManager to switch to a given view.

	\param viewId The id of the view to be switched to.
	\sa NotesAppControllerIf
 */
void NotesAppControllerIfImpl::switchToView(NotesNamespace::NotesViewIds viewId)
{
	mAppController->mViewManager->switchToView(viewId);
}

// End of file	--Don't remove this.
