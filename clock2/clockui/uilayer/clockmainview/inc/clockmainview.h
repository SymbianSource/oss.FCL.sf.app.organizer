/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This is the header file for the CClockMainView class.
*
*/

#ifndef __CLOCK_MAIN_VIEW_H__
#define __CLOCK_MAIN_VIEW_H__

// System includes
#include <aknview.h>

// User includes
#include "clkcommon.h"
#include "clkmdlobserver.h"

// Forward declarations
class CClockMainContainer;
class CClockAlarmArray;
class CClkUiAlarmModel;
class CClkUiAlarmIdleObserver;
class CClockAppUi;

// Class declaration
/**
* @class CClockMainView
* @brief The CAknView inheriting class.
* @exe clock.exe
*/
class CClockMainView : public CAknView,
                       public MClkModelObserver
	{
	public:        // Constructor and destructor
			
		/**
		* @brief Create a CClockMainView object. Performs the first phase of 2-phase construction.
		* @param aAlarmModel The alarm model.
		* @return CClockMainView* A pointer to the created instance of CClockMainView
		*/
		static CClockMainView* NewL( CClkUiAlarmModel* aAlarmModel );
		
		/**
		* @brief Destructor
		*/
		~CClockMainView();
		
	public:        // From base classes
	
		/**
		* @brief From CAknView. Returns this view Id.
		* @return TUid The id of the view.
		*/
	    TUid Id() const;

		/**
		* @param aCommand The id of the command to be handled.
		* @brief From MEikCommandObserver. Handles a user command relating to this view.
		*/
	    void HandleCommandL( TInt aCommand );
	    
		/**
		* @brief From MClkUiMdlObserver. Notification obtained from the model for changes.
		* @param aReason The cause of notification.
		*/
	    void HandleUpdateL( TInt aReason );
	    
		/**
		* @brief From MEikMenuObserver. Dynamically initializes the menu items.
		* @param aResourceId The resource which triggered the action.
		* @param aMenuPane Pointer to the menupane of type CEikMenuPane.
		*/
	    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
	    
		/**
		* @brief From MEikMenuObserver. Dynamically initializes the menubar.
		* @param aResourceId The resource which triggered the action.
		* @param aMenuBar Pointer to the menupane of type CEikMenuBar.
		*/
        void DynInitMenuBarL( TInt aResourceId, CEikMenuBar* aMenuBar );
        
        /**        
        * @brief From CEikAppUi.
        *        Called when the application switches from foreground to background and vice versa.
        * @param aForeground ETrue if foreground, EFalse if sent to background.
        */
        void HandleForegroundEventL( TBool aForeground );
	    
	public:        // New functions
	    
		/**
		* @brief Returns the application rect to the container.
		* @return TRect The application rect.
		*/
	    TRect GetApplicationRect();
	    
		/**
		* @brief Handles the environment changes. Called from CClockAppUi.
		* @param aEnvChanged Of type TClockEnvChanged.
		*/
        void HandleEnvChangeL( TClockEnvChanged aEnvChanged );
        
		/**
		* @brief Checks if the alarm editor is already open.
		* @return ETrue if the alarm editor is already open, EFalse otherwise.
		*/
        TBool IsAlarmEditorOpen();

        /**
        * @brief Returns the application's appui.
        * @return CClockAppUi The pointer to the clock application ui.
        */
        CClockAppUi* ClockApplicationUi();
	    
	protected:     // From base classes
	
		/**
		* @discussion From CAknView. Activates this view.
		* @see CAknView.
		*/
	    void DoActivateL( const TVwsViewId& aPrevViewId,
	    				  TUid aCustomMessageId,
	    				  const TDesC8& aCustomMessage );

		/**
		* @brief From CAknView. Deactivates this view.
		*/
	    void DoDeactivate();
	    
	private:       // New functions
	
		/**
		* @brief Performs the second phase of 2-phase construction. May leave.
		* @param aAlarmModel The alarm model.
		*/
        void ConstructL( CClkUiAlarmModel* aAlarmModel);
        
        /**
		* @brief C++ default constructor
		*/
        CClockMainView();
        
		/**
		* @brief Displays the alarm editor for the user to set the alarm.
		*/
        void SetNewAlarmL();
        
		/**
		* @brief Deletes the selected alarm.
		*/
        void RemoveAlarmL();
        
		/**
		* @brief Activates the focused alarm item in the list.
		*/
        void ActivateAlarmL();
        
		/**
		* @brief Deactivates the focused alarm item in the list.
		*/
        void DeactivateAlarmL();
        
		/**
		* @brief Resets the focused alarm item in the list.
		*/
        void ResetAlarmL();
        
        /**
         * @brief Checks if DST changes will be applied in the next 24 hours.
         * @return TBool ETrue if remaining time note has to be displayed.
         */
        TBool CheckForDstChangesL();
        
        /**
        * @brief Displays the remaining time for the alarm to expire.
        */
        void DisplayRemainingTimeL();

        
	private:		// Data
	
		/**
		* @var iContainer
		* @brief The container object.
		*/
		CClockMainContainer*              iContainer;
		
		/**
		* @var iAlarmArray
		* @brief The alarms array.
		*/
		CClockAlarmArray*                 iAlarmArray;
		
		/**
		* @var iAlarmInfo
		* @brief Holds the alarm information entered by the user in the editor.
		*/
		SClkAlarmInfo                     iAlarmInfo;
		
		/**
		* @var iAlarmId
		* @brief The alarm id of the latest alarm.
		*/
        TAlarmId                          iAlarmId;
		
		/**
		* @var iAlarmModel
		* @brief The alarm model object.
		*/
		CClkUiAlarmModel*                 iAlarmModel;
		
		/**
		* @var iModelObserver
		* @brief The model observer.
		*/
		CClkUiAlarmIdleObserver*          iModelObserver;
		
		/**
		* @var iOpenedByMSK
		* @brief Flag which indicates if the menu was opened by the MSK or not.
		*/
		TBool                             iOpenedByMSK;
		
		/**
		* @var iAlarmEditorOpen
		* @brief Flag which indicates if the alarm editor is open or not.
		*/
		TBool                             iAlarmEditorOpen;

	};

#endif 		// __CLOCK_MAIN_VIEW_H__

// End of file
