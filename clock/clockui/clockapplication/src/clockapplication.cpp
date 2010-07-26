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
* Description: Definition file for class ClockApplication.
*
*/


// User includes
#include "clockapplication.h"
#include "clockappcontroller.h"

/*!
	\class ClockApplication

	This class creates the ClockAppController object.
 */
/*!
	Constructor.
 */
ClockApplication::ClockApplication(int &argc, char *argv[]):
 HbApplication(argc, argv)
{
}

/*!
	Destructor.
 */
ClockApplication::~ClockApplication()
{
	if (mController) {
		delete mController;
		mController = 0;
	}
}

/*!
	creates the ClockAppController object.
*/
void ClockApplication::createController()
{
	mController = new ClockAppController;
	connect(mController, SIGNAL(appReady()), this, SLOT(handleAppReady()));
}

/*!
	Start the event loop for the application.
*/
bool ClockApplication::execution()
{
	int ret = 0;
	ret = exec();
	return ret;
}

/*!
	Emits the applicationReady signal.
 */
void ClockApplication::handleAppReady()
{
	emit applicationReady();
	disconnect(mController, SIGNAL(appReady()), this, SLOT(handleAppReady()));
}

// End of file	--Don't remove this.
