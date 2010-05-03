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
* Description: Definition file for class NotesDocLoader.
*
*/


// User includes
#include "notesdocloader.h"
#include "notesmainview.h"
#include "notescollectionview.h"
#include "notestodoview.h"
#include "notesfavoriteview.h"
#include "notesnoteview.h"
#include "notescommon.h"

/*!
	\class NotesDocLoader

	Custom document loader class for notes view manager.
 */
/*!
	From HbDocumentLoader.
	Creates and returns an object of type and assigns name as its object name.

	\param type The type name as mentioned in docml.
	\param name The name of the object as mentioned in docml.
	\return QObject* Pointer to the constructed QObject.
	\sa HbDocumentLoader
 */
QObject* NotesDocLoader::createObject(const QString &type, const QString &name)
{
	if (NOTES_MAIN_VIEW == name) {
		QObject *object = new NotesMainView();
		object->setObjectName(name);
		return object;
	} else if (NOTES_COLLECTION_VIEW == name) {
		QObject *object = new NotesCollectionView();
		object->setObjectName(name);
		return object;
    } else if (NOTES_TODO_VIEW == name) {
		QObject *object = new NotesTodoView();
		object->setObjectName(name);
		return object;
    } else if (NOTES_FAVORITES_VIEW == name) {
		QObject *object = new NotesFavoriteView();
		object->setObjectName(name);
		return object;
	} else if (NOTES_NOTE_VIEW == name) {
		QObject *object = new NotesNoteView();
		object->setObjectName(name);
		return object;
    }

	return HbDocumentLoader::createObject(type, name);
}

// End of file	--Don't remove this.
