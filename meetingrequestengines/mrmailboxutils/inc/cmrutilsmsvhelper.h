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
* Description: Class for handling message entries and using CMsvSession.
*
*/



#ifndef __CMRUTILSMSVHELPER_H__
#define __CMRUTILSMSVHELPER_H__

//  INCLUDES
#include "cmrmailboxutils.h"
#include <mmrmbutilsmbinfohandler.h>

#include <e32base.h>
#include <msvapi.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CImSmtpSettings;
class CEmailAccounts;
class CClientMtmRegistry;
class CMRMailBoxInfo;

// same as in imum's MuiuDynamicSettingsArray.hrh & imas.hrh
const TInt KMREmailAddrLength = 100;

// CLASS DECLARATION

/**
* 
*/
class  CMRUtilsMsvHelper
: public CBase, 
  public MMRMBUtilsMBInfoHandler,
  public MMsvSessionObserver
    {
    public: // Constructors and destructors

        static CMRUtilsMsvHelper* NewL(
            CMsvSession* aSession,
            MMsvSessionObserver& aParentObserver );

        /**
        * Destructor.
        */
        ~CMRUtilsMsvHelper();
        
    public: // New functions

        /**
        * Get session reference.
        * @return session reference
        */        
        CMsvSession& MsvSessionL();
                                    
    public: // From MMRMBUtilsMBInfoHandler

        const RPointerArray<CMRMailBoxInfo>& MRMailBoxes() const;
            
        /**
        * Information about the specific mailbox.
        * Ownership is not transferred.
        * @param aEntryId entry id
        * @return mailbox info or NULL if mailbox not found
        */        
        const CMRMailBoxInfo* MRMailBoxInfoL( TInt iId) const;

        /**
        * Information about all mailboxes available.
        * @param aMailBoxes array to be filled with information
        */
        void ListMRMailBoxesL(
            RArray<CMRMailboxUtils::TMailboxInfo>& aMailBoxes );
        
        /**
        * Extension API
        */
        TAny* ExtensionL( TUid aExtensionUid );  
                                  
    protected: // From MMsvSessionObserver
    
        void HandleSessionEventL( TMsvSessionEvent aEvent, 
                                  TAny *aArg1, 
                                  TAny *aArg2, 
                                  TAny *aArg3 );

    protected: // New functions
        
        /**
        * E-mail address of a specific mailbox.
        */                                                          
        //TBuf<KMREmailAddrLength> EMailAddressL( TMsvEntry& entry );    
          void  EMailAddressL( TMsvEntry& entry );  
        /**
        * Looks up SMTP service which is related to given service.
        * This is used for reading SMTP settings.
        */
        TMsvId CorrespondingSmtpServiceL( TMsvEntry& entry ) const;
                
        /**
        * Loads and returns SMTP settings.
        * Item is left to cleanup stack, ownership is transferred.
        */
        CImSmtpSettings* SmtpSettingsLC( TMsvId aSmtpService );
                    
        /**
        * Helper method for checking whether given mtm supports MR sending.
        * @param aMtmUid specifies the mtm to evaluate
        * @return ETrue if supports, EFalse otherwise        
        */
        TBool IsMRCapableL( TUid aMtmUid );
    
        /**
        * Helper which constructs a session if it has been closed due
        * to an preceding server request of closing the session.
        */
        void EnsureSessionExistsL();
        
        /**
        * Checks whether given entry selection contains at least one
        * MR mailbox.
        */
        TBool ContainsMRMailBoxEntryL(
            const CMsvEntrySelection& aEntries ) const;
        
        /**
        * Resets iMRMailBoxes and re-reads all MR capable mailboxes
        * in the device.
        */
        void FindMRMailBoxesL();      
        
        TBool AddToListIfMRMailBoxesL( const CMsvEntrySelection& aEntries );
        
        TBool AddToListIfMRMailBoxL( TMsvId aEntryId ); 
                
    protected: // Constructors and destructors
    
        /**
        * C++ default constructor.
        */
        CMRUtilsMsvHelper( CMsvSession* aSession,
                           MMsvSessionObserver& aParentObserver );

        /**
        *  Constructor, second phase.
        */
        void ConstructL();  
        
    protected: // data
    
        MMsvSessionObserver& iParentObserver;
        
        // array items owned
        RPointerArray<CMRMailBoxInfo> iMRMailBoxes;
             
        // own
        CEmailAccounts* iEmailSettings;
        
        CClientMtmRegistry* iMtmReg;     
    
        // ownership determined by the iOwnSession
        CMsvSession* iSession;
        
        TBool iOwnSession;
    private:
   TBuf<KMREmailAddrLength> iEmailAddr;
    };

#endif // __CMRUTILSMSVHELPER_H__

// End of File
