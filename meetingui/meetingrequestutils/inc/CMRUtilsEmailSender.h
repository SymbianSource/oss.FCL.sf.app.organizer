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
*       Class which takes care of e-mail editing and sending.
*
*/



#ifndef __CMRUTILSEMAILSENDER_H__
#define __CMRUTILSEMAILSENDER_H__

//  INCLUDES
#include <e32base.h>
#include <msvstd.h>
#include <CMRUtils.h>
#include "CMRUtilsInternal.h"

// FORWARD DECLARATIONS
class CCalSession;
class CCalEntry;
class MMRUtilsObserver;
class CClientMtmRegistry;
class CSendUi;
class CMessageData;
class CStringResourceReader;

// CLASS DECLARATION

/**
* Interface for a class which takes care of e-mail editing and sending.
*/
class MMRUtilsEmailSender
    {
    public: // Constructors and destructors
    
        /**
        * Destructor, allows deletion through this interface.
        */
        virtual ~MMRUtilsEmailSender() {};    
            
     public: // New functions
     
        /**
        * @see CMRUtils
        */
        virtual void ReplyToL( CMRUtilsInternal::TMailRecipients aRecipients,
                               const CCalEntry& aCalEntry,
                               TMsvId aMailBox,
                               const TDesC& aSenderAddr ) = 0;
    
        /**
        * @see CMRUtils
        */
        virtual void SendL( const CCalEntry& aCalEntry, TMsvId aMailBox ) = 0;        
    };

/**
* Implementation class for MMRUtilsEmailSender interface.
*/
class CMRUtilsEmailSender : public CBase, public MMRUtilsEmailSender
    {
    public: // Constructors and destructors

        /**
        * Symbian two-phased constructor.
        * @param aCalSession calendar session reference
        * @param aObserver observer reference
        * @return instantiated object
        */
        static CMRUtilsEmailSender* NewL( CCalSession& aCalSession );        

        /**
        * Destructor.
        */
        ~CMRUtilsEmailSender();

    public: // From MMRUtilsEmailSender
    
        void ReplyToL( CMRUtilsInternal::TMailRecipients aRecipients,
                       const CCalEntry& aCalEntry,
                       TMsvId aMailBox,
                       const TDesC& aSenderAddr );
    
        void SendL( const CCalEntry& aCalEntry, TMsvId aMailBox );
        
    protected: // New functions

        /**
        * Helper function which adds recipients determined by aRecipients
        * to aMsgData.
        * @param aRecipients enumeration
        * @param aCalEntry calendar entry containing attendees
        * @param aSenderAddr sender's e-mail address
        * @param aMsgData data where recipients are added
        */
        void AddRecipientsToMsgL( CMRUtilsInternal::TMailRecipients aRecipients,
                                  const CCalEntry& aCalEntry,
                                  const TDesC& aSenderAddr,
                                  CMessageData& aMsgData ) const;
          
        /**
        * Helper function which adds recipients determined by aCalEntry
        * method (request, cancel etc.) to aMsgData.
        * @param aCalEntry calendar entry containing attendees
        * @param aMsgData data where recipients are added
        */                                            
        void AddRecipientsToMsgL( const CCalEntry& aCalEntry,
                                  CMessageData& aMsgData ) const;
                                 
        /**
        * Helper function which adds one recipient to aMsgData, leaves with
        * KErrArgument if aRecipient is NULL or does not have e-mail address.
        * @param aRecipient calendar user, ownership not transferred
        * @param aMsgData data where recipient is added
        */ 
        void AddRecipientToMsgL( const CCalUser* aRecipient,
                                 CMessageData& aMsgData ) const;                                 

        /**
        * Helper function which creates e-mail subject with prefix.
        * @param aSubject subject without prefix
        * @param aReply ETrue if reply, EFalse if forward
        * @return new subject string, ownership transferred
        */                                  
        HBufC* CreateSubjectPrefixStringL( const TDesC& aSubject,
                                           TBool aReply ) const;

        /**
        * Helper function which sets method (e.g. "REPLY") to aString.
        * @param aString string to contain method
        * @param aCalEntry entry which specifies the method
        */                                                                             
        void SetMethodStringL( TDes8& aString,
                               const CCalEntry& aCalEntry );
                               
        
        /**
        * Used with TCleanupOperation to delete tmp attachment file.
        * @param aPtr, HBufC pointer to attachment file name
        */             
        static void CleanupTmpAttachment( TAny* aPtr );                                                 
        
    protected: // Constructors and destructors
    
        /**
        * C++ default constructor.
        * @param aCalSession calendar session reference
        * @param aObserver observer reference
        */
        CMRUtilsEmailSender( CCalSession& aCalSession );

        /**
        *  Constructor, second phase.
        */
        void ConstructL();            
        
    protected: // data
    
        // File server session
        RFs iFs;
    
        // Calendar session
        CCalSession& iCalSession;
                
        // Resource reader, own
        CStringResourceReader* iResReader;
        
        // SendUi, own
        CSendUi* iSendUi;
    };

#endif // __CMRUTILSEMAILSENDER_H__

// End of File
