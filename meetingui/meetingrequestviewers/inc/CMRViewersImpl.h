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
*       Implementation of CMRViewers ECom API.
*
*/



#ifndef __CMRVIEWERSIMPL_H__
#define __CMRVIEWERSIMPL_H__

//  INCLUDES
#include <CMRViewers.h>
#include "CMRUtilsInternal.h"

// FORWARD DECLARATIONS
class CMRHandler;
class TCoeHelpContext;
class CMRMailboxUtils;

// CLASS DECLARATION

/**
* ECom implementation for executing meeting request viewers.
*/
class CMRViewersImpl : public CMRViewers, public MMRUtilsObserver
    {
    public: // Constructors and destructors

        /**
        * ECom plug-in instantiation method.
        * HBufC8* descriptor is received as a constructor parameter,
        * ownership is transferred to ECom plug-in.
        * @param aMtmUid descriptor used for resolving
        * @return instantiated ECom plug-in
        */
        static CMRViewersImpl* NewL( TAny* aMtmUid );

        /**
        * Destructor.
        */
        ~CMRViewersImpl();

    protected: // From MAgnEntryUi

        TInt ExecuteViewL( RPointerArray<CCalEntry>& aEntries,
                           const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
                           MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,                                   
                           MAgnEntryUiCallback& aCallback);

        void SetHelpContext( const TCoeHelpContext& aContext );
        
	protected:	// Functions from MMRUtilsObserver

		void HandleCalEngStatus( TMRUtilsCalEngStatus aStatus );

		void HandleOperation( TInt /*aType*/,
                              TInt /*aProgress*/,
                              TInt /*aStatus*/ ) {};

    protected: // Constructors and destructors

       /**
        * C++ default constructor.
        */
        CMRViewersImpl( TAny* aMtmUid );

       /**
        *  Constructor, second phase.
        */
        void ConstructL();
        
    protected: // New functions
    
        void EnsureUtilsExistL( CCalSession* aCalSession,
                                CMsvSession* aMsgSession );
    
        void DoHandleCalEngStatusL( TMRUtilsCalEngStatus aStatus );
        
        static TInt HandleEngStatusCheck( TAny* aThis );

    protected: // data

        // resource file offset
        TInt iResourceFileOffset;
        
        // current cal eng status
        TMRUtilsCalEngStatus iEngStatus;
            
        // not own, helpcontext id
        const TCoeHelpContext* iContext;
        
        // not own
        CCalSession* iCalSession;
        
        // not own
        CMsvSession* iMsgSession;
        
		// Meeting request utils, own
        CMRUtilsInternal* iMRUtils;
        
        // Meeting request mailbox utils, own
        CMRMailboxUtils* iMRMailboxUtils;    

        // own, descriptor used for ECom plug-in resolving
        HBufC8* iMtmUid;

        // own, meeting request dialog handling
        CMRHandler* iMRHandler;

        // own
        CIdle* iIdleTimer;
    };

#endif // __CMRVIEWERSIMPL_H__

// End of File
