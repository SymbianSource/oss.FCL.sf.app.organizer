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
* Description:  main implementation.
*
*/


#include <QtGui>
#include <QTranslator>
#include <xqserviceutil.h>
#include <hbapplication.h> 
#include <hbmainwindow.h>  
#include "calencontroller.h"
#include "calenserviceprovider.h"


 int main(int argc, char *argv[])
    {
    HbApplication app(argc, argv);
    
    // Main window for providing the scene context
	HbMainWindow window;
	window.setRenderHints(
			QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	window.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	
    //For translation, loading and installing translator
	QTranslator translator;
    QString lang = QLocale::system().name();
    QString path = "Z:/resource/qt/translations/";
    // TODO: Load the appropriate .qm file based on locale
    //bool loaded = translator.load("calendar_" + lang, path);
    bool loaded = translator.load("calendar_en_GB",":/translations");
    app.installTranslator(&translator);

    //Backup and restore code  need to write here.
	
    // Check if calendar is launched thru XQService framework
    bool isFromServiceFrmwrk = XQServiceUtil::isService();
    CCalenController *controller = new CCalenController(isFromServiceFrmwrk);
    
    // Create the Calendar service provider
    CalenServiceProvider service(controller);
    return app.exec();
    }


 //End of file
