/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*       ECom interface for executing Meeting Request Utils.
*
*/



#ifndef __CMRUTILS_H__
#define __CMRUTILS_H__

//  INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include <ecom/ecomresolverparams.h>
#include <msvstd.h>
#include <msvids.h>
#include <MeetingRequestUids.hrh>

// FORWARD DECLARATIONS
class CCalEntry;
class CCalSession;
class CMsvSession;
class CCalInstance;

_LIT8( KMRUtilsDefaultImpl, "S60DefaultImpl" );

// CLASS DECLARATION

/**
* Observer class for Meeting Request Utils.
*/
class MMRUtilsObserver
    {
    public: // Data types
    
        /**
        * Calendar engine status.
        */
        enum TMRUtilsCalEngStatus
            {
            ENotReady,               // not ready now, wait for completion
            EAvailableWithoutTombs,  // tombs are omitted but normal db ok
            EAvailable,              // both normal and tombstone db ok
            ENotAvailable            // error, nothing available
            };
    
    public: // New functions
    
        /**
        * Method for handling calendar engine status.
        * @param aStatus engine status
        */
        virtual void HandleCalEngStatus( TMRUtilsCalEngStatus aStatus ) = 0;
        
        /**
        * Method for handling asynchronous operation progress and status.
        * @param aType operation code
        * @param aPercentageCompleted
        * @param aStatus completion code
        */       
        virtual void HandleOperation( TInt /*aType*/,
                                      TInt /*aPercentageCompleted*/, 
                                      TInt /*aStatus*/ ) {};
    };


/**
* ECom Interface for executing Meeting Request Utils.
* It is required that the client waits until it is notified
* that the engine is available before calling utility methods.
*/
class CMRUtils : public CBase
    {
    public: // Data types
        
        /**
        * Codes of asynchronous operations defined in this API.
        */ 
        enum TOpCode
            {
            ENoOperation = 0,       // no operation
            EDeleteEntries,         // delete some entry(s)
            ELastOperation          // to allow extending op codes
            };        
    
    public: // Constructors and destructors

        /**
        * Constructs an ECom plug-in.
        * Use descriptor representing the MTM UID to resolve proper plug-in.
        * The default plug-in is constructed with KMRUtilsDefaultImpl.
        * Client may give NULL pointer for aMsvSession if it is not available.
        *
        * @param aMtmUid MTM UID name with enclosing square brackets "[...]"
        * @param aObserver mandatory utils feedback observer
        * @param aCalSession calendar session reference
        * @param aMsvSession, ownership not transferred
        * @return instantiated plug-in
        */
        static CMRUtils* NewL( const TDesC8& aMtmUid,
                               MMRUtilsObserver& aObserver,
                               CCalSession& aCalSession,
                               CMsvSession* aMsvSession );

        /**
        * Destructor.
        */
        virtual ~CMRUtils();
        
    public: // New functions

        /**
        * Deletes given aEntries from the database and possibly allows
        * user to cancel meeting or decline participation, depending on
        * participation type.
        * This method will leave with KErrNotSupported if caller doesn't
        * have CEikonEnv available.
        * @param aEntry entry to be deleted
        * @param aMailbox mailbox to use for possible cancel/response
        * @return KErrNone or a system wide error code  
        */
        virtual TInt DeleteWithUiL( const CCalEntry& aEntry,
                                    TMsvId aMailbox ) = 0;
          
        /**
        * Deletes given instance from the database and possibly allows
        * user to cancel meeting or decline participation, depending on
        * participation type.
        * This method takes ownership of aInstance.
        * This method will leave with KErrNotSupported if caller doesn't
        * have CEikonEnv available.
        * @param aInstance instance to be deleted, ownership transferred
        * @param aMailbox mailbox to use for possible cancel/response
        * @return KErrNone or a system wide error code  
        */                          
        virtual TInt DeleteWithUiL( CCalInstance* aInstance,
                                    TMsvId aMailbox ) = 0;

        /**
        * Sends the given entry. Recipients are determined based on the
        * method type (request, response etc.).
        * This method will leave with KErrNotSupported if caller doesn't
        * have CEikonEnv available.
        * @param aEntry entry to be sent
        * @param aMailbox mailbox to use for sending     
        */             
        virtual void SendWithUiL( const CCalEntry& aEntry,
                                  TMsvId aMailbox ) = 0;            
        		
        /**
        * Allows extending this API without breaking BC.
        * @param aExtensionUid specifies
        * @return extension of the requested type
        */
        virtual TAny* ExtensionL( TUid aExtensionUid ) = 0;                
        
    protected: // New functions
    
        /**        
        * Internal template method called immediatelly after
        * instantiation of the ECom plug-in.
        * @param aCalSession calendar session reference
        * @param aMsvSession, may be NULL
        */
        virtual void SessionSetupL( CCalSession& aCalSession,
                                    CMsvSession* aMsvSession ) = 0;
        
    protected: // data
            
        // not own    
        MMRUtilsObserver* iObserver;
    
        // for ECom to identify plug-in instance
        TUid iDtor_ID_Key;                         
    };

#include <CMRUtils.inl>

#endif // __CMRUTILS_H__

// End of File
