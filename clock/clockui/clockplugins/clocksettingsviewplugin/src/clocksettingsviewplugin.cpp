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
* Definition file for class ClockSettingsViewPlugin.
*
*/
// System includes
#include <QtPlugin>

// User includes
#include "clockregionalsettingsview.h"
#include "clocksettingsviewplugin.h"

/*!
	\class ClockSettingsViewPlugin

	\brief The class ClockSettingsViewPlugin is a plugin to show
	clock settings View 
*/

/*!
	Constructor.
 */
ClockSettingsViewPlugin::ClockSettingsViewPlugin()
{
}

/*!
	Destructor.
 */
ClockSettingsViewPlugin::~ClockSettingsViewPlugin()
{
}

/*!
	Launch the clock  regional settings view.
 */
void ClockSettingsViewPlugin::launchRegionalSettingsView()
{
	//The view is removed from main window and deleted once you select back on view
	ClockRegionalSettingsView *view = new ClockRegionalSettingsView();
	view->showView();
}

Q_EXPORT_PLUGIN2(clocksettingsviewplugin, ClockSettingsViewPlugin)

// End of file	--Don't remove this
