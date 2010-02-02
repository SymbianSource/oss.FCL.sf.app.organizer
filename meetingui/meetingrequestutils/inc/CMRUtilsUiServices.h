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
*       UI Extension class for Meeting Request Utils.
*
*/



#ifndef CMRUTILSUISERVICES_H
#define CMRUTILSUISERVICES_H

// INCLUDES
#include "MMRUtilsUiServices.h"
#include "MRHelpers.h"
#include <msvapi.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CMRUtilsInternal;
class CCalSession;
class MMRUtilsEmailSender;
class CMRMailboxUtils;

// CLASS DECLARATION

/**
 * Implements MMRUtilsUiServices.
 */
class CMRUtilsUiServices
    : public CBase, 
      public MMRUtilsUiServices,
      public MMsvSessionObserver
    {
    public: // Constructors and destructors

        static CMRUtilsUiServices* NewL( CMRUtilsInternal& aMRUtils,
                                         CCalSession& aCalSession,
                                         CMsvSession* aMsvSession,
                                         CMRMailboxUtils& aMRMailboxUtils );

        /**
        * Destructor.
        */
        ~CMRUtilsUiServices();

    protected:  // From MMRUtilsUiServices
    
        TInt RespondWithUiL( const RPointerArray<CCalEntry>& aEntries,
                             CCalAttendee::TCalStatus aStatus,
                             TMsvId aMailbox );
                            
        TInt CancelWithUiL( const RPointerArray<CCalEntry>& aEntries,
                            TMsvId aMailbox );
                                    
        TInt DeleteWithUiL( const CCalEntry& aEntry, TMsvId aMailbox );
                            
        TInt DeleteWithUiL( CCalInstance* aInstance, TMsvId aMailbox );
                            
        void ReplyToL( CMRUtilsInternal::TMailRecipients aRecipients,
                       const CCalEntry& aCalEntry,
                       const TDesC& aSenderAddr,
                       TMsvId aMailbox );
                               
        void SendL( const CCalEntry& aCalEntry, TMsvId aMailbox );
        
    protected: // From MMsvSessionObserver
    
        void HandleSessionEventL( TMsvSessionEvent aEvent,
                                  TAny* aArg1,
                                  TAny* aArg2,
                                  TAny* aArg3 );
        
    protected: // New functions
                
        /**
        * Method which performs, if applicable, response/cancel operation
        * as a sub-operation of delete. The result of sub-operation is
        * neglected.
        * @param aEntry entry to be deleted
        * @param aInstance, may be NULL           
        * @param aMailbox mailbox used for sending response/cancel
        */
        void PerformDeleteSubOpL( const CCalEntry& aEntry,
                                  const CCalInstance* aInstance,
                                  TMsvId aMailbox );
                            
        /**
        * Goes through entries in the array, checks which should be
        * cancelled/responded, and updates database status.
        * @param aEntries
        * @param aValidIndexes
        * @param aIsCancellation ETrue if operation is cancellation
        * @param aStatus used if aIsCancellation = EFalse
        */        
        void EvaluateAndUpdateL( const RPointerArray<CCalEntry>& aEntries,
                                 RArray<TInt>& aValidIndexes,
                                 TBool aIsCancellation,
                                 CCalAttendee::TCalStatus aStatus =
                                    CCalAttendee::ENeedsAction );
              
        /**
        * Checks whether the given entry can be cancelled.
        * @param aEntry entry to evaluate
        * @return ETrue if can be cancelled        
        */
        TBool IsValidForCancelL( const CCalEntry& aEntry );                                  

        /**
        * Checks whether the given entry can be responded to.
        * @param aEntry entry to evaluate
        * @return ETrue if can be responded to        
        */
        TBool IsValidForResponseL( const CCalEntry& aEntry );
        
        /**
        * Helper method for checking that request is not cancelled or
        * out of date.
        * @param aEntry entry to evaluate
        * @return ETrue if request is valid
        */
        TBool IsValidRequestL( const CCalEntry& aEntry ) const;
        
        /**
        * Updates entry status in db.
        * @param aEntry
        * @param aIsCancellation ETrue if operation is cancellation
        * @param aStatus used if aIsCancellation = EFalse
        */        
        void UpdateDbStatusL( const CCalEntry& aEntry,
                              TBool aIsCancellation,
                              CCalAttendee::TCalStatus aStatus ) const;

        /**
        * Queries user for sending options and if requested sends
        * cancellations or responsed. Utilizes QuerySendChoiceL
        * and CreateToBeSentL for doing it's tasks.
        * Parameter aInstance is used only in context of deleting one
        * instance, in that case also aEntries contains only one entry.
        * @param aEntries array of entries
        * @param aValidIndexes indexes in aEntries valid for response/cancel
        * @param aInstance, may be NULL
        * @param aMailbox mailbox used for sending response/cancel
        * @param aIsCancellation ETrue if operation is cancellation
        * @param aStatus used if aIsCancellation = EFalse
        */
        TInt QueryAndSendL( const RPointerArray<CCalEntry>& aEntries,
                            const RArray<TInt>& aValidIndexes,
                            const CCalInstance* aInstance,
                            TMsvId aMailbox,
                            TBool aIsCancellation,
                            CCalAttendee::TCalStatus aStatus =
                                CCalAttendee::ENeedsAction );
                                            
        /**
        * Queries send/edit choice for a cancellation or response.
        * Returns command code or an error code.
        * @param aBase base entry for data shown in the query
        * @param aIsCancellation ETrue if operation is cancellation
        * @param aStatus used if sending response, not cancellation
        * @return command code or an error code.
        */
        TInt QuerySendChoiceL( const CCalEntry& aBase,
                               TBool aIsCancellation,
                               CCalAttendee::TCalStatus aStatus ) const;

        /**
        * Creates a response or cancellation based on the given request.
        * @param aBase base entry for creating response
        * @param aInstance, may be NULL                
        * @param aDescription description text for the response
        * @param aIsCancellation ETrue if operation is cancellation        
        * @return created entry, ownership is transferred
        */
        CCalEntry* CreateToBeSentLC( const CCalEntry& aBase,
                                     const CCalInstance* aInstance,
                                     const TDesC& aDescription,
                                     TBool aIsCancellation ) const;
                                       
        /**
        * Creates a response based on the given request.
        * @param aBase base entry for creating response
        * @param aInstance, may be NULL             
        * @param aDescription description text for the response   
        * @return created entry, ownership is transferred
        */
        CCalEntry* CreateResponseLC( const CCalEntry& aBase,
                                     const CCalInstance* aInstance,
                                     const TDesC& aDescription ) const;

        /**
        * Creates a cancellation based on the given request.
        * @param aBase base entry for creating cancellation
        * @param aInstance, may be NULL               
        * @param aDescription description text for the cancellation 
        * @return created entry, ownership is transferred
        */
        CCalEntry* CreateCancelLC( const CCalEntry& aBase,
                                   const CCalInstance* aInstance,
                                   const TDesC& aDescription ) const;
        
        /**
        * Creates a new entry based on given arguments.
        * Entry is left in the cleanupstack
        * @param aBase base entry
        * @param aInstance, may be NULL
        * @param aMethod method type for the new entry
        * @param aSequenceNumber sequence number for the new entry                    
        * @param aCopyType specifies what data is copied
        * @return created entry, ownership is transferred        
        */                           
        CCalEntry* CreateFromLC( const CCalEntry& aBase,
                                const CCalInstance* aInstance,
                                CCalEntry::TMethod aMethod,
                                TInt aSequenceNumber,
                                MRHelpers::TCopyFields aCopyType ) const;

        /**
        * Looks up SMTP service which is related to given service.
        * This is used for reading SMTP settings.
        */
        TMsvId CorrespondingSmtpServiceL( TMsvId aRelatedService ); 
        
        /**
        * Helper method for lazy creation of msv session.
        */
        void EnsureMsvSessionExistsL();
                                             
    protected: // Constructors and destructors
    
        /**
        * C++ default constructor.
        */
        CMRUtilsUiServices( CMRUtilsInternal& aMRUtils,
                            CMsvSession* aMsvSession,
                            CMRMailboxUtils& aMRMailboxUtils );

        /**
        *  Constructor, second phase.
        */
        void ConstructL( CCalSession& aCalSession );
        
    protected: // Data
    
        // resource file offset
        TInt iResourceFileOffset;
        
        // msv session ownership
        TBool iOwnMsvSession;
        
        // MR Utils reference
        CMRUtilsInternal& iMRUtils;
        
        CMRMailboxUtils& iMRMailboxUtils;
                
        // ownership determined by iOwnMsvSession
        CMsvSession* iMsvSession;        
        
        // own, email sender utility
        MMRUtilsEmailSender* iEmailSender;
    };

#endif  // CMRUTILSUISERVICES_H

// End of File
