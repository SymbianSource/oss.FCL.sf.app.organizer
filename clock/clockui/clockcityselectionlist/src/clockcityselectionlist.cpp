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
* Definition file for class ClockCitySelectionList.
*
*/

//System Include
#include<HbApplication>
#include<QTranslator>


// User includes
#include "clockcityselectionlist.h"
#include "clockcityselectionlist_p.h"
#include "clockdatatypes.h"
#include "timezoneclient.h"

/*!
	\class ClockCitySelectionList

	TODO: Write a complete description of the object. Mention about API usage
	as well.
 */

/*!
	\fn citySelected(LocationInfo *info)

	This signal is emitted before the cityselectionlist is closed.

	\param info Pointer to an object of LocationInfo containing the selected
	city info. If the info is 0, it means that nothing was selected.
 */

/*!
	Constructor.

	\param client Pointer to TimezoneClinet.
	\param parent of type QObject.
 */
ClockCitySelectionList::ClockCitySelectionList(
		TimezoneClient *client, QObject *parent)
:QObject(parent),
 d_ptr(new ClockCitySelectionListPrivate(client, this))
{
	// Load the translation file and install the editor specific translator
	mTranslator = new QTranslator;
	//QString lang = QLocale::system().name();
	//QString path = "Z:/resource/qt/translations/";
	mTranslator->load("clockcityselectionlist",":/translations");
	// TODO: Load the appropriate .qm file based on locale
	//bool loaded = mTranslator->load("caleneditor_" + lang, path);
	HbApplication::instance()->installTranslator(mTranslator);
}

/*!
	Default constructor.

	\param parent of type QObject.
 */
ClockCitySelectionList::ClockCitySelectionList(QObject *parent)
:QObject(parent),
 d_ptr(new ClockCitySelectionListPrivate(0, this))
{
	// Load the translation file and install the editor specific translator
	mTranslator = new QTranslator;
	//QString lang = QLocale::system().name();
	//QString path = "Z:/resource/qt/translations/";
	mTranslator->load("clockcityselectionlist",":/translations");
	// TODO: Load the appropriate .qm file based on locale
	//bool loaded = mTranslator->load("caleneditor_" + lang, path);
	HbApplication::instance()->installTranslator(mTranslator);

}

ClockCitySelectionList::~ClockCitySelectionList()
{
    // Remove the translator
    HbApplication::instance()->removeTranslator(mTranslator);
    if (mTranslator) {
        delete mTranslator;
         mTranslator = 0;
    }
}

/*!
	Shows the city selection list. Connect to signal citySelected before calling this function
	to make sure that the notification is reached upon selecting a city.
 */
void ClockCitySelectionList::showCityList()
{
	d_ptr->showCityList();
}

/*!
	Closes the city list selection list. This is a convenient API for the client to close the list
	if required. This would emit the citySelected signal without any selected city information.
 */
void ClockCitySelectionList::closeCityList()
{
	d_ptr->closeCityList();
}

// End of file	--Don't remove this.
