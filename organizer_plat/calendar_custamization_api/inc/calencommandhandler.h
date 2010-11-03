/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Interface for calendar command handler.
* 
*/

#ifndef MCALENCOMMANDHANDLER_H
#define MCALENCOMMANDHANDLER_H

// System includes
#include <vwsdef.h>
#include <calencontext.h>
#include <caleninstanceid.h>
#include <caltime.h>

// Forward declarations
class CAknToolbar;
class CEikMenuPane;

// Class declaration
/**
 * @class TCalenCommand
 * @brief Local class used to wrap the current command and information
 * to restore the current context
 */
class TCalenCommand
    {
	public:
		/**
		 * @brief Default C++ constructor
		 */
		TCalenCommand();
        
		/**
		 * @brief Sets the command and context info
		 * 
		 * @param aCommand The command id to be set
		 * @param aContext The context to be set
		 */
		void SetCommandAndContextL( TInt aCommand,
									MCalenContext& aContext );
		
		/**
		 * @brief Gets the command id that has been set
		 * 
		 * @return TInt The command id
		 */
		TInt Command() const;

		/**
		 * @brief Gets the context info that has been set
		 *
		 * @param aContext The context info
		 */
		void GetContextL( MCalenContext& aContext ) const;

	private:
	
		/**
		 * @var iCommand
		 * @brief The command
		 */
		TInt				iCommand;
		
		/**
		 * @var iFocusDateAndTime
		 * @brief The focussed date and time
		 */
		TCalTime			iFocusDateAndTime;
		
		/**
		 * @var iInstanceId
		 * @brief The instance id
		 */
		TCalenInstanceId	iInstanceId;
		
		/**
		 * @var iViewId
		 * @brief The view id
		 */
		TVwsViewId			iViewId;
    };

#include "calencommandhandler.inl"

// Class declaration
/**
 * @class MCalenCommandHandler 
 * @brief An interface class that has to be implemented to handle any commands
 */
class MCalenCommandHandler
    {
	public:
		
		/**
		 * @brief Handles aCommand
		 * 
		 * @param aCommand The command id to be handled
		 */
		virtual TBool HandleCommandL( const TCalenCommand& aCommand ) = 0;
    
		/**
		 * @brief Allows extending this API without breaking BC.
		 * 
		 * @param aExtensionUid Specifies the extension UID
		 * @return TAny* Extension of the requested type
		 */
		virtual TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid ) = 0; 
    };

#endif // MCALENCOMMANDHANDLER_H

// End of file
