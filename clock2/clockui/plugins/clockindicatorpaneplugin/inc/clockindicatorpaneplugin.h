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
* Description:   This is the header file for the CClockIndicatorPanePlugin class.
*
*/

#ifndef __CLOCK_INDICATORPANE_PLUGIN_H__
#define __CLOCK_INDICATORPANE_PLUGIN_H__

// System includes
#include <AknIndicatorPlugin.h>
#include <eikenv.h>
#include <asclisession.h>

// Constants
const TInt KPluginUid( 0x10275094 );

// Forward declarations
class CClockIndicatorPluginListener;
class RASCliSession;

// Class declaration
/**
* @class CClockIndicatorPanePlugin
* @brief Clock side Implementation of CAknIndicatorPlugin to display the Alarm indicator
*		 on the Status pane.
* @see CAknIndicatorPlugin
*/
class CClockIndicatorPanePlugin : public CAknIndicatorPlugin
	{
  	public:				// New functions
  	
  		/**
  		* @brief Two phases constructor. Performs the first phase of object construction.
  		* @return CClockIndicatorPanePlugin* Pointer to an instance of CClockIndicatorPanePlugin.
  		*/
  		static CClockIndicatorPanePlugin* NewL();

		/**
		* @brief The Destructor.
		*/
  		~CClockIndicatorPanePlugin();
  		
  	public:				// New functions
  	
  		/**
  		* @brief Subscribes with the alarm server for a change in alarm settings.
  		* @param aRequestStatus Of type TRequestStatus updated when there's a change.
  		* @param aAlarmId Will have the id of the alarm which undergoes a change.
  		*/
  		void NotifyChange( TRequestStatus& aStatus, TAlarmId& aAlarmId );
  		
  		/**
  		* @brief Cancel for previous subscriptions.
  		*/
  		void NotifyChangeCancel();
  		
  		/**
  		* @brief Updates the plugin data.
  		*/
  		void UpdatePluginDataL();
  		
  	public: 			// From base classes.
        
		/**
		* @brief From CAknIndicatorPlugin. Called when the user taps on the indicator entry.
		* @param aUid UID of the indicator. 
		* @see CAknIndicatorPlugin.
		*/
		void HandleIndicatorTapL( const TInt aUid );
		
		/**
		* @brief From CAknIndicatorPlugin. Called to get the text to be displayed in the indicator
		* 		 popup along with the icon. The text will be of type link text.
		* @param aUid Is the UID of the plugin.
		* @param aTextType Is the type of text, in our case it will be a link text.
		*/
		HBufC* TextL( const TInt aUid, TInt& aTextType );
	
  	private:			// New functions
  		
		/**
		* @brief C++ default constructor. MUST NOT Leave.
		*/
  		CClockIndicatorPanePlugin();
	
		/** 
		* @breif Symbian OS constructor, performs the second phase of 2 phase construction. Might Leave.
		*/
	    void ConstructL();
	    
  	private:			// Data
  	
  		/**
  		* @var iUid
  		* @brief The Uid of the plugin.
  		*/
  		TInt								iUid;
  		
  		/**
  		* @var iEnv
  		* @brief The control environment.
  		*/
  		CEikonEnv*							iEnv;
  		
  		/**
  		* @var iAlarmServerClient
  		* @brief The client handle to the alarm server session.
  		*/
  		RASCliSession						iAlarmServerClient;
  		
  		/**
  		* @var iListener
  		* @brief Pointer to the alarm change listener. Owned.
  		*/
  		CClockIndicatorPluginListener*		iListener;
  		
  		/**
  		* @var iSubscribed
  		* @brief ETrue if subscription for notification from AlarmServer is active.
  		*/
  		TBool								iSubscribed;
	};
	
#endif 			// __CLOCK_INDICATORPANE_PLUGIN_H__

// End of file
