/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition file for class CalenDocLoader.
 *
*/


// User includes
#include "calendocloader.h"
#include "calendayview.h"
#include "calendayviewwidget.h"
#include "calenmonthview.h"
#include "calenmonthgrid.h"
#include "calenpreviewpane.h"
#include "calencontroller.h"
#include "calenthicklinesdrawer.h"
#include "calencommon.h"
#include "calenpluginlabel.h"

// ----------------------------------------------------------------------------
// CalenDocLoader::CalenDocLoader
// C++ default Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenDocLoader::CalenDocLoader(CCalenController &controller) :
	mController(controller)
	{

	}

// ----------------------------------------------------------------------------
// CalenDocLoader::CalenDocLoader
// C++ default Destructor.
// ----------------------------------------------------------------------------
//
CalenDocLoader::~CalenDocLoader()
{

}
// ----------------------------------------------------------------------------
// CalenDocLoader::createObject
// @brief From HbDocumentLoader
// Called whenever the docml file is parsed to create objects.
// Over-riding it since we need to create custom views and widgets.
// @param type The object type which is being created. For ex: HbWidget.
// @param name The name of the created object.
// @return QObject* Pointer to the created object.
// ----------------------------------------------------------------------------
//
QObject *CalenDocLoader::createObject(const QString &type,
                                             const QString &name)
{
    MCalenServices &services = mController.Services(); 
	if (name == QString(CALEN_DAYVIEW)) {
		QObject *calenDayView = new CalenDayView(services);
		calenDayView->setObjectName(name);
		return calenDayView;
	} else if (name == QString(CALEN_DAYVIEW_WIDGET)) {
	    QObject *calenDayViewWidget = new CalenDayViewWidget(services, this);
	    calenDayViewWidget->setObjectName(name);
	    return calenDayViewWidget;
	} else if (name == QString(CALEN_MONTHVIEW)) { // For Month View
        QObject *calenMonthView = new CalenMonthView(services);
        calenMonthView->setObjectName(name);
        return calenMonthView;
	} else if (name == QString(CALEN_MONTHGRID)) {
        QObject *calenMonthGrid = new CalenMonthGrid();
        calenMonthGrid->setObjectName(name);
        return calenMonthGrid;
	} else if(name == QString(CALEN_PREVPREVIEWPANE)) {
        QObject *prevPreviewPane =
        new CalenPreviewPane(services);
        prevPreviewPane->setObjectName(name);
        return prevPreviewPane;
    } else if(name == QString(CALEN_CURRPREVIEWPANE)) {
        QObject *currPreviewPane =
        new CalenPreviewPane(services);
        currPreviewPane->setObjectName(name);
        return currPreviewPane;
    } else if(name == QString(CALEN_NEXTPREVIEWPANE)) {
        QObject *nextPreviewPane =
        new CalenPreviewPane(services);
        nextPreviewPane->setObjectName(name);
        return nextPreviewPane;
    } else if (name == QString(CALEN_DAYNAMES_WIDGET)) {
	    QObject *calenDayNamesWidget = 
        new CalenThickLinesDrawer(CalendarNamespace::CalenDayNameWidget);
	    calenDayNamesWidget->setObjectName(name);
	    return calenDayNamesWidget;
	} else if (name == QString(CALEN_WEEKNUMBERS_WIDGET)) {
	    QObject *calenSWeekNumWidget =
        new CalenThickLinesDrawer(CalendarNamespace::CalenWeekNumWidget);
	    calenSWeekNumWidget->setObjectName(name);
	    return calenSWeekNumWidget;
	} else if (name == QString(CALEN_PREVREGIONALINFO)) {
		QObject *calenPrevRegionalLabel = new CalenPluginLabel(services);
		calenPrevRegionalLabel->setObjectName(name);
		return calenPrevRegionalLabel;
	} else if (name == QString(CALEN_CURRREGIONALINFO)) {
		QObject *calencurrRegionalLabel = new CalenPluginLabel(services);
		calencurrRegionalLabel->setObjectName(name);
		return calencurrRegionalLabel;
	} else if (name == QString(CALEN_NEXTREGIONALINFO)) {
		QObject *calenNextRegionalLabel = new CalenPluginLabel(services);
		calenNextRegionalLabel->setObjectName(name);
		return calenNextRegionalLabel;
	} else {
		return HbDocumentLoader::createObject(type, name);
	}
}

// End of file	--Don't remove this.
