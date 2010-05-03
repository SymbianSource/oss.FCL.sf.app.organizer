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
* Description: Contains main, the entry point for notes application.
*
*/

// System includes
#include <QScopedPointer>
#include <QTranslator>
#include <hbapplication.h>
#include <hbmainwindow.h>

// User includes
#include "notesappcontroller.h"

/*!
	The main() function.

	Responsible for constructing the NotesAppController object and showing the
	main window.
 */
int main(int argc, char *argv[])
{
	// Create and initialize an HbApplication instance
	HbApplication app(argc, argv);

	// Main window for providing the scene context
	HbMainWindow window;
	window.setRenderHints(
			QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	window.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

	// Load the translation file.
	QTranslator notesViewsTranslator;
	notesViewsTranslator.load("notes",":/translations");
	app.installTranslator(&notesViewsTranslator);

	// Construct the application controller.
	QScopedPointer<NotesAppController> controller(new NotesAppController);
	Q_UNUSED(controller)

	// Show the main window.
	window.show();
	// Start the event loop for the application
	return app.exec();
}

// End of file	--Don't remove this.
