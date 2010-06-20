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
* Description:   This is the header file for the CClockAppUi class.
*
*/

#ifndef __CLOCK_APPUI_H__
#define __CLOCK_APPUI_H__

// System includes
#include <aknViewAppUi.h>
#include <AknTabObserver.h>
#include <iaupdateobserver.h>     
#include <iaupdate.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>

// User includes
#include "clkcommon.h"

// Forward declarations
class CEnvironmentChangeNotifier;
class CAknNavigationControlContainer;
class CAknTabGroup;
class CAknNavigationDecorator;
class CIAUpdate;
class CIAUpdateParameters;
class CTransitionEffectManager;
class MTouchFeedback;

// Class declaration
/**
* @class CClockAppUi
* @brief The CAknViewAppUi inheriting class.
* @exe clock.exe
*/
class CClockAppUi : public CAknViewAppUi,
                    public MAknTabObserver,
                    public MIAUpdateObserver
	{
    
    public: // enums
    
        enum TTransitionDirection 
            {
            ETransitionUndefined = 0,
            ETransitionLeft,
            ETransitionRight
            };    
    
	public:            // Constructor and destructor

		/**
		* @brief C++ default constructor
		*/
	    CClockAppUi();
		
        /**
        * @brief Destructor.
        */
        ~CClockAppUi();
        
	public:            // From base classes
	
		/**
		* @brief From CAknViewAppUi. Handles menu commands.
		* @param aCommandId The id of the command to be handled.
		*/
		void HandleCommandL( TInt aCommandId );
		
		/**
        * @brief From MAknTabObserver. Called when tab is changed.
        * @param aIndex The index of the Active tab.
        * @see MAknTabObserver
        */
		void TabChangedL( TInt aIndex );
		
		/**
		* @brief From CCoeAppUi. Called when a key is pressed.
		* @param aKeyEvent Encapsulates the key event.
		* @param aType The type of the event.
		* @return TKeyResponse Return if the event was handled or not.
		* @see CCoeAppUi
		*/
		TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
		
		/**
		* @brief From CAknAppUi. Called for all window server events.
		* @param aForeground ETrue if the app is being brought to the foreground. EFalse otherwise.
		*/
		void HandleForegroundEventL( TBool aForeground );
			
		/**
		* @brief From MIAUpdateObserver.
		* @param aErrorCode The error code of the observed update operation.
		*                  KErrNone for successful completion,otherwise a system wide error code.
		* @param aAvailableUpdates Number of the updates that were found available.
		*/
		void CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates );
		
		/**
		* @brief From MIAUpdateObserver.
		* @param aErrorCode The error code of the completed update operation.
		*                   KErrNone for successful completion, 
		*                   otherwise a system wide error code.
		* @param aResultDetails Details about the completed update operation.
		*                       Ownership is transferred.
		*/
		void UpdateComplete( TInt aErrorCode, CIAUpdateResult* aResultDetails );
		
		/**
		* @brief From MIAUpdateObserver.
		* @param aErrorCode The error code of the observed query operation.
		*                   KErrNone for successful completion, 
		*                   otherwise a system wide error code.
		* @param aUpdateNow ETrue informs that an update operation should be started.
		*                   EFalse informs that there is no need to start an update
		*                   operation.
        */
		void UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow );
		
	public:            // New functions
	    
	    /**
	    * @brief Handles the environment changes.
	    * @param aEnvChanged Of type TClockEnvChanged.
	    */
	    void HandleEnvChangeL( TClockEnvChanged aEnvChanged );
	    
	    /**
        * @brief Sets the main view title pane text.
        * @param aResourceId Resource from which the title pane text is read.
        */
	    void SetTitlePaneTextL( TInt aResourceId );

	    
	    /**
        * @brief Sets or Resets the appHidden flag.
        * @param aAppHidden ETrue to hide, else EFalse.
        */
        void SetAppHidden( TBool aAppHidden );
        
        /**
        * @brief Returns the value of iAppHidden
        * @return TBool ETrue if the app was Exit hidden.
        */
        TBool IsAppExitHidden();
        
        TBool TransitionOngoing() const;
        
        void SetTransitionOngoing( TBool aTransitionOngoing );
        
        TInt TransitionGroupId() const;
        
        void SetTransitionGroupId( TInt aId ); 
        
        TTransitionDirection TransitionDirection() const;
        
        void  SetTransitionDirection( TTransitionDirection aId );  
        
        void DoDisappearTransition( CCoeControl* aControl );
        
        void DoAppearTransition( CCoeControl* aControl );
        
        
        /**
        * @brief Hides or Un-hides the navigation pane.
        * @param aVisible ETrue, pane will be visible. EFalse will be hidden.
        * @param aViewId The ID used to activate the corresponding tab.
        */
        void MakeNavigationPaneVisible( TBool aVisible, TInt aViewId );
        
        /**
        * @brief Handles IAD related updates.
        */
        void HandleIADUpdateL();

        
        //papercut Ana/Digital No24Rpeat Start- rajender
		/**
        * @brief returns handle to tactile feedback.
        */
        MTouchFeedback* GetFeedback();
		//papercut Ana/Digital No24Rpeat End- rajender
		
	private:           // New functions
	
		/**
		* @brief Symbian OS constructor. Performs the second phase of construction. May leave.
		*/
		void ConstructL();
		
		/**
		* @brief Callback function for environment change notifications.
		* @param aPtr The TAny* to 'this'.
		* @return TInt Symbian OS error code.
		*/
		static TInt NotifierCallbackL( TAny* aPtr );
		
		/**
        * @brief Opens the date and time settings view
        */
		void OpenSettingsViewL();
		
		/**
        * @brief Resets the state of the application to what it was initially.
        */
        void ResetToInitialStateL();
        
        /**
        * @brief Sets the navigation pane with tabs.
        */
        void SetupNavigationPaneL();
        
	private:           // Data
	    
	    /**
	    * @var iEnvChangeNotifier
	    * @brief An object of CEnvironmentChangeNotifier. Used to get notifications for changes in
	    *        environment.
	    */
	    CEnvironmentChangeNotifier*         iEnvChangeNotifier;
	    
	    /**
        * @var iAppHidden 
        * @brief ETrue if app is hidden in the background.
        */
        TBool                               iAppHidden;
        
        /**
        * @var iNaviPane
        * @brief The navigation pane decorator.
        */
        CAknNavigationControlContainer*     iNaviPane;
        
        /**
        * @var iTabGroup
        * @brief The tab group.
        */
        CAknTabGroup*						iTabGroup;
        
        /**
        * @var iNavigationDecorator
        * @brief The navigation decorator.
        */
        CAknNavigationDecorator*			iNavigationDecorator;

        /**
        * @var iIADUpdateFlag
        * @brief the flag is true for the first time and it is made false after IAD updates
        *        And it is made true again while exiting.
        */
        TBool                               iIADUpdateFlag;
        
        /**
        * @var iUpdate
        * @brief Object of CIAUpdate class.
        */
        CIAUpdate*                          iUpdate;
        
        /**
        * @var iParameters
        * @brief Object of CIAUpdateParameters class.
        */
        CIAUpdateParameters*                iParameters;
        
        TBool iTransitionOngoing;
        TInt  iTransitionGroupId;
        TBool   iTactileFeedbackSupported;
        TTransitionDirection iTransitionDirection;
        
        MTouchFeedback*  iFeedback;
	};

#endif 		// __CLOCK_APPUI_H__

// End of file
