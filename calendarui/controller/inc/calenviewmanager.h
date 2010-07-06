/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar view manager
*
*/


#ifndef CALENVIEWMANAGER_H
#define CALENVIEWMANAGER_H

// INCLUDES

#include <vwsdef.h>
#include <QDate>

#include "calennotificationhandler.h"   // MCalenNotificationHandler
#include "hb_calencommands.hrh"
#include "calencommandhandler.h"

class CalenNativeView;
class CCalenController;
class CalenMonthView;
class CalenAgendaView;
class AgendaEventViewer;
class CalenLandscapeDayView;
class CalenSettingsView;
class CalenDocLoader;

/**
 * The action ui handles events passed from the controller and delegates
 * them to the appropriate place (i.e. the specific action ui classes).
 */
class  CalenViewManager :	public QObject,
							public MCalenNotificationHandler,
							public MCalenCommandHandler
    {   
    
	Q_OBJECT
	
    public:  // Construction and destruction
		/**
		 * Constructor
		 */
		CalenViewManager ( CCalenController& aController);
		
		/**
		 * Second Phase Constructor
		 */
		void SecondPhaseConstruction();
		
        /**
         * Destructor
         */                             
        virtual ~CalenViewManager();
	    
	public: 
       /**
         * Handles view manager commands.
         * @return ETrue if command is handled, EFalse otherwise
         */
        TBool HandleCommandL( const TCalenCommand& aCommand );
        
        /**
         * Refresh current view.
         */
        void refreshCurrentViewL();
        
        /**
         * Returns the settings view
         */
        CalenSettingsView* settingsView();
		/*
		* @brief Returns CCalenController
		*/
		CCalenController&  calenController();
		/**
		 * Constructs and activates the requested view
		 */
		void constructAndActivateView(int view);
		/**
		 * Returns the first view
		 */
		int getFirstView();
		
		void showNextDay();
		
		void showPrevDay();
		
		void removePreviousView();
		
		void constructOtherViews();
		
		void launchSettingsView();
		
		
	public: // from MCalenNotificationHandler
   
        /**
         * Handles notifications
         */
        void HandleNotification( const TCalenNotification aNotification );
        
	private:
        /**
         * Loads month view frm the docml
         */
        void loadMonthView();
        /**
		 * Loads day view frm the docml
		 */
        void loadAgendaView();
	    /**
	     * Activates the default view, as retrieved from settings.
	     */
	    void ActivateDefaultViewL( int defaultView );
	    
	    /**
          * Activate current view.
          */
		void activateCurrentView();
		
		/**
		 * Launch event view.
		 */
		void launchEventView();
		
		/**
		 * @brief Constructs the auxilliary view used
		 * to provide illusion of swiping to next or prev
		 * day
		 */
		void loadAlternateAgendaView();
        
    private slots:
		void handleMainViewReady();
		void handleViewingCompleted(const QDate date);
		void handleEditingStarted();
		void handleEditingCompleted();
		void handleDeletingStarted();
		void handleDeletingCompleted();
		void handleInstanceViewCreation(int status);
		void handleEntryViewCreation(int status);
		void handleDayViewReady();
		
    private:  // Data        

		CCalenController		&mController;
		CalenMonthView			*mCalenMonthView;
		CalenAgendaView			*mCalenAgendaView;
		AgendaEventViewer		*mCalenEventViewer;
		CalenLandscapeDayView	*mCalenLandscapeDayView;
		CalenSettingsView		*mSettingsView;
		CalenDocLoader			*mAgendaViewDocLoader;
		CalenDocLoader			*mMonthViewDocLoader;
		CalenAgendaView         *mCalenAgendaViewAlt;
		CalenDocLoader          *mAgendaViewAltDocLoader;
		CalenNativeView			*mCalenDayView;
        
		int  mCurrentViewId;
		int  mPreviousViewsId;
		int  mFirstView;
        };

#endif // CALENVIEWMANAGER_H

// End of file
