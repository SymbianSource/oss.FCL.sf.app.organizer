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
* Description: This is the Definition file for NotesAppController class.
*
*/

// User includes
#include "notesappcontroller.h"
#include "notesappcontrollerifimpl.h"
#include "notesviewmanager.h"
#include "notesmodelhandler.h"

/*!
	\class NotesAppController

	This is the heart of notes application. It constructs and owns the
	NotesViewManager and NotesModelHandler objects.
 */

/*!
	Default constructor.
 */
NotesAppController::NotesAppController(QObject *parent)
:QObject(parent),
 mViewManager(0),
 mNotesModelHandler(0),
 mIfImpl(0)
{
	// Construct the interface implementation.
	mIfImpl = new NotesAppControllerIfImpl(this, this);

	// Construct the model handler.
	mNotesModelHandler = new NotesModelHandler(this);
	Q_ASSERT_X(
			mNotesModelHandler, "notesappcontroller.cpp",
			"NotesModelHandler is 0");

	// Construct the view manager.
	mViewManager = new NotesViewManager(*mIfImpl, this);
	Q_ASSERT_X(
			mViewManager, "notesappcontroller.cpp",
			"NotesViewManager is 0");
}

/*!
	Destructor.
 */
NotesAppController::~NotesAppController()
{
	if (mViewManager) {
		delete mViewManager;
		mViewManager = 0;
	}
	if (mNotesModelHandler) {
		delete mNotesModelHandler;
		mNotesModelHandler = 0;
	}
	if (mIfImpl) {
		delete mIfImpl;
		mIfImpl = 0;
	}
}

// End of file	--Don't remove this.
