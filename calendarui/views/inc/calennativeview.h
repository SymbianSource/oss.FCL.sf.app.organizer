/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is the base class of all Calendar views.
 *
*/


#ifndef CALENNATIVEVIEW_H
#define CALENNATIVEVIEW_H

//  System includes
#include <hblabel.h>
#include <hblistwidget.h>
#include <hbabstractviewitem.h>

// User includes
#include "calennotificationhandler.h"   // MCalenNotificationHandler
#include "calenview.h"

const int WORKAROUND_TO_LIMIT_MAX_SPEED=8000;

class MCalenServices;
class HbDateTimePicker;

class  CalenNativeView : public CalenView,
                        public MCalenNotificationHandler
    {
    Q_OBJECT
    
	public:  // Constructors and destructor
        /**
         * Destructor.
         */
	    virtual ~CalenNativeView();
	    HbWidget* ControlOrNull();
	    bool isPlugin();
	    virtual void populationComplete();
		TBool pluginEnabled();
		
	protected:  // New functions
	    CalenNativeView( MCalenServices& services );
	    
	    void checkInfobarL();
	    void HandleNotification( const TCalenNotification notification );
	    virtual void onLocaleChanged(int reason)=0;

	protected slots:
	
        void goToDate();
        void goToSelectedDate();
        void deleteBeforeDate();
        void deleteAllEntries();
        void launchSettingsView();
	    virtual void changeOrientation(Qt::Orientation orientation);
	    
    protected:
    
        MCalenServices&      mServices; // not owned.
        TBool mPluginEnabled;
	
    private:
        HbWidget* mInfobar;//plugin label
        HbDateTimePicker* mDatePicker;
	};

#endif  // CALENNATIVEVIEW_H

// End of file
