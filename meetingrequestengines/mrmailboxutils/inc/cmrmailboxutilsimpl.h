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
* Description: Meeting request mailbox utils ECom implementation header.
*
*/



#ifndef __CMRMAILBOXUTILSIMPL_H__
#define __CMRMAILBOXUTILSIMPL_H__


//  INCLUDES
#include <cmrmailboxutils.h>
#include <msvapi.h>

// FORWARD DECLARATIONS
class CMRUtilsMsvHelper;
class CMRMBUtilsMBInfoHandler;
class CMRUtilsSettingsMgr;
class CCalUser;

class CMRMailboxUtilsImpl
    : public CMRMailboxUtils,
      public MMsvSessionObserver,
      public MMRMailboxUtilsObserver
    {    
    public: // Constructors and destructors

        /**
        * ECom plug-in instantiation method.
        *
        * @param aMsvSession, ownership not transferred
        * @return instantiated object
        */
        static CMRMailboxUtilsImpl* NewL( CMsvSession* aMsvSession );

        /**
        * Destructor.
        */
        ~CMRMailboxUtilsImpl();
    
    protected: // From CMRMailboxUtils
        
        TInt GetDefaultMRMailBoxL( TMailboxInfo& aBoxInfo );

        TInt SetDefaultMRMailBoxL( TMsvId aDefaultBox );

        void ListMailBoxesL( RArray<TMailboxInfo>& aMailBoxes );

        TInt GetSourceMtmUidL( const CCalEntry& aCalEntry, TUid& aMtmUid );

        CCalAttendee* ThisAttendeeL( const CCalEntry& aCalEntry );
            
        TBool IsOrganizerL( const CCalEntry& aCalEntry );
        
        TInt SetPhoneOwnerL( CCalEntry& aCalEntry,
                             TMsvId aPrimaryBox = KMsvNullIndexEntryId );                             
                             
        void AddObserverL( MMRMailboxUtilsObserver& aObserver );
        
        void RemoveObserver( MMRMailboxUtilsObserver& aObserver );
        
        CMsvSession& MsvSessionL();
        
    protected: // From MMsvSessionObserver
    
        /**
        * CMRUtilsMsvHelpers performs first-hand filtering of
        * uninteresting events.
        */
        void HandleSessionEventL( TMsvSessionEvent aEvent, 
                                  TAny *aArg1, 
                                  TAny *aArg2, 
                                  TAny *aArg3 );
                                  
    protected: // From MMRMailboxUtilsObserver
        void HandleMRMailboxEventL( 
                MMRMailboxUtilsObserver::TEventType aType );
    
    protected: // New functions
        
        /**
        * Returns TPtrC to all/part of the aAddress parameter
        * so that possible "mailto:" prefix is stripped out.
        *
        * @param aAddress address which might contain the prefix
        * @return TPtrC which represents only the address
        */                        
        TPtrC AddressWithoutMailtoPrefix( const TDesC& aAddress );
        
        /**
        * Helper method for finding out phone owner.
        * @param aCalEntry entry containing matched organizer and attendees
        * @param aMailBoxes contains a list of mailboxes to evaluate
        * @param aMatchIndex contains matching mailbox if owner was found
        * @return phone owner, NULL if not found, ownership not transferred
        */
        CCalUser* PhoneOwnerL(
            const CCalEntry& aCalEntry,
            const RArray<CMRMailboxUtils::TMailboxInfo>& aMailBoxes,
            TInt& aMatchIndex );   
          
        /**
        * Helper method for checking whether aUser is phone owner.
        * @param aUser either an organizer or an attendee
        * @param aMailBoxes contains a list of mailboxes to evaluate
        * @param aMatchIndex contains matching mailbox if owner was found
        * @return ETrue if aUser is phone owner
        */  
        TBool IsPhoneOwnerL(
            const CCalUser& aUser,
            const RArray<CMRMailboxUtils::TMailboxInfo>& aMailBoxes,
            TInt& aMatchIndex );
            
        /**
        * Helper method for notifying all observers.
        */
        void NotifyObserversL( MMRMailboxUtilsObserver::TEventType aEvent );            
            
    protected: // Constructors
    
        /**
        * C++ default constructor.
        */
        CMRMailboxUtilsImpl();
        
        /**
        *  Constructor, second phase.
        */
        void ConstructL( CMsvSession* aMsvSession );            
            
    private:   // data
    
        // observers not own
        RPointerArray<MMRMailboxUtilsObserver> iObservers;
        
        // Msv helper, own
        CMRUtilsMsvHelper* iMsvHelper;              
        
        // Database manager, own
        CMRUtilsSettingsMgr* iSettingsMgr;
        
        CMRMBUtilsMBInfoHandler* iCmailHandler;
    };

#endif //__CMRMAILBOXUTILSIMPL_H__
