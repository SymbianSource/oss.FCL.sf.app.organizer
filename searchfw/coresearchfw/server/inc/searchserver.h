/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file is header file for SearchServer
*
*/



#ifndef C_SEARCH_SERVER_H
#define C_SEARCH_SERVER_H

// INCLUDE FILES
#include <e32base.h>
#include <searchcscommon.h>
#include "searchmanagerobserver.h"
#include "searchmanager.h"
#include "searchplugininterface.h"
#include "searchmanagercallbacktimer.h"
#include "searchservershutdown.h"
// FORWARD DECLARATIONS
class CSearchServerSession;
class CSearchContentResource;
class CSearchDocumentId;
class CSearchSnippetInfo;
class CSearchSnippet;
class CSearchPlugin;


// CLASS DECLARATION
/**
* Server side class of search service Client/Server derived from CServer2.
*/


class CSearchServer : public CServer2
	{
	public:
		enum { EPriority = 950 };
	
	public:	// Constructors and destructor
		/**
		* Symbian OS 2 phased constructor.
		*
		* @return	A pointer to the created instance of CSearchServer.
		*/
		static CSearchServer* NewL();

		/**
		* Symbian OS 2 phased constructor.
		*
		* @return	A pointer to the created instance of CSearchServer.
		*/
		static CSearchServer* NewLC();

    	/**
    	* Destructor
    	*/
    	virtual ~CSearchServer();
			
		// New functions
        /**
        * Informs the server that a session was created.
        *
        * 
        *
        */
		void SessionCreatedL( );
		
        
    	/**
        * Informs the server that a session was destroyed.
        *
        * 
        */
		void SessionDestroyedL( );
		
		// Functions from base classes
		/**
		* From CServer2.
		*
		* Creates a server-side session object.
		*/
		virtual CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage)  const;

	private:
	    // More constructors
		/** 
		* Performs the first phase of two phase construction.
		*
		* @param	aPriority	Priority of the server
		*/
		CSearchServer( TInt aPriority );

    	/**
    	* Performs the second phase construction.
    	*/
    	void ConstructL();
	
		// New functions
		/**
		* Returns the version number of the server's server.
		*
		* @return	Version information.
		*/
		TVersion Version() const
		{
		   	return( TVersion( KSearchServerMajorVersionNumber, KSearchServerMinorVersionNumber,
										 KSearchServerBuildVersionNumber ) );
		};

		/**
		* From CServer2.
		* Returns the Error.
		*
		* @param aError error found
		* @returna Error.
		*/
		TInt RunError( TInt aError ) ;

	    // Data
        
       	/**
		* Counter incremented each time a session is created.  It is used to create a TUid for
		* each session.
		*/
	    TInt iIncrementCount;
       	/**
		* Pointer to the SearchPluginInterface
		* Own.
		*/
        CSearchPluginInterface* iSearchPluginInterface;

       	/**
		* Pointer to the Call back timer for starting main operation
		* Own.
		*/
   		CSearchseverShutdown * iServertimer;
	};

#endif  // SEARCH_SERVER_H

// End of File
