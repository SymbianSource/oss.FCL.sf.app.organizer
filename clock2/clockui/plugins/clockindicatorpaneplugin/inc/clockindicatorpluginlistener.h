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
* Description:   This is the header file for the CClockIndicatorPluginListener class.
*
*/

#ifndef __CLOCK_INDICATOR_PLUGIN_LISTENER_H__
#define __CLOCK_INDICATOR_PLUGIN_LISTENER_H__

// System includes
#include <e32base.h>
#include <asshddefs.h>

// Forward declarations
class CClockIndicatorPanePlugin;

// Class declaration
/**
* @class CClockIndicatorPluginListener
* @brief This is an active object responsible for listening for changes in the alarm queue.
*		 Constructed and used by CClockIndicatorPanePlugin
* @see CClockIndicatorPanePlugin
*/
class CClockIndicatorPluginListener : public CActive
	{
	public:				// New functions
	
		/**
  		* @brief Two phases constructor. Performs the first phase of object construction.
  		* @param aPanePlugin Pointer to the plugin object.
  		* @return CClockIndicatorPluginListener* Pointer to an instance of CClockIndicatorPluginListener.
  		*/
  		static CClockIndicatorPluginListener* NewL( CClockIndicatorPanePlugin* aPanePlugin );
  		
		/**
		* @brief The Destructor.
		*/
  		~CClockIndicatorPluginListener();
  		
	public:				// New functions
  	
  		/**
  		* @brief Sets the object active and asks the plugin to subscribe for notification.
  		*/
  		void NotifyChange();
  		
  		/**
  		* @brief Cancels all pending requests and asks the plugin to un-subscribe for any subscriptions for notification.
  		*/
		void NotifyChangeCancel();	
  		
  	public:				// From base classes
  	
  		/**
  		* @brief From CActive, called after a call to SetActive is made.
  		*/
  		void RunL();
  		
  		/**
  		* @brief From CActive. Implements cancellation of an outstanding request.
  		*/
  		void DoCancel();
  		
  	private:			// New functions
  	
		/**
		* @brief C++ default constructor. MUST NOT Leave.
		*/
  		CClockIndicatorPluginListener( CClockIndicatorPanePlugin* aPanePlugin );
	
		/** 
		* @breif Symbian OS constructor, performs the second phase of 2 phase construction. Might Leave.
		*/
	    void ConstructL();
	
	private:			// Data
	
		/**
		* @var iPanePlugin
		* @brief Object of the plugin.
		*/
		CClockIndicatorPanePlugin*				iPanePlugin;
	};

#endif			// __CLOCK_INDICATOR_PLUGIN_LISTENER_H__

// End of file
