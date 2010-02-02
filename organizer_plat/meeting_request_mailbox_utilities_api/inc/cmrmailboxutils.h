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
* Description:   ECom interface for executing meeting request mailbox utils.
*
*/



#ifndef __CMRMAILBOXUTILS_H__
#define __CMRMAILBOXUTILS_H__


//  INCLUDES
#include <e32base.h>
#include <msvids.h>
#include <msvapi.h>
#include <badesca.h>
#include <MeetingRequestUids.hrh>
#include <ecom/ecom.h>

// FORWARD DECLARATIONS
class CCalEntry;
class CCalAttendee;

class MMRMailboxUtilsObserver
    {
    public: // Data types
    
    enum TEventType
        {
        EMRNullEvent,       // null type for initialization and testing
        EMRMailboxCreated,  // one or more mailboxes created
        EMRMailboxChanged,  // one or more mailboxes changed
        EMRMailboxDeleted,  // one or more mailboxes deleted
        EMRDefaultSet,      // default mailbox (re)set
        EMRDefaultChanged,  // default mailbox entry modified
        EMRDefaultDeleted   // default mailbox deleted
        };
        
    public:
    
        /**
        * This method reports CMsvSession events that affect the list of
        * MR capable mailboxes.
        * @param aType event type
        */
        virtual void HandleMRMailboxEventL( TEventType aType ) = 0;
    };

class CMRMailboxUtils : public CBase
    {    
    public: // Data types
    
    /**
    * Information about a specific mailbox identified by iEntryId.
    */
    struct TMailboxInfo
        {
        TPtrC iName;
        TPtrC iEmailAddress;
        TUid iMtmUid;        
        TMsvId iEntryId;
        };
                
    public: // Constructors and destructors

        /**
        * Constructs CMailboxUtils ECom plug-in.
        *
        * @param aMsvSession, ownership not transferred
        * @return instantiated object
        */
        static CMRMailboxUtils* NewL( CMsvSession* aMsvSession = NULL );

        /**
        * Destructor.
        */
        ~CMRMailboxUtils();
    
    public: // New functions
        
        /**
        * Gets Calendar's default MR mailbox information.
        * Return value is KErrNone if default has been set, KErrNotFound if
        * it hasn't been set.
        * If default mailbox has been set and it still exists, then it's info
        * is assigned to aBoxInfo. If it has been set but doesn't
        * exist then the first one from the list of mailboxes is set as
        * default and returned in aBoxInfo. In other cases 
        * KMsvNullIndexEntryId is assigned to TMRUtilsMailBoxInfo::iEntryId.
        * @param aBoxInfo struct which will be modified
        * @return KErrNone or KErrNotFound if default hasn't been set.
        */
        virtual TInt GetDefaultMRMailBoxL( TMailboxInfo& aBoxInfo ) = 0;

        /**
        * Sets Calendar's default MR mailbox.
        * @param aDefaultBox box to be set as default
        * @return KErrNone of a system wide error code
        */           
        virtual TInt SetDefaultMRMailBoxL( TMsvId aDefaultBox ) = 0;

        /**
        * Lists all MR capable mail boxes available in the phone.
        * @param aMailboxes array for listing mail boxes
        */    
        virtual void ListMailBoxesL( RArray<TMailboxInfo>& aMailBoxes ) = 0;

        /**
        * Resolves MTM Uid based on the entry. This is done by finding out
        * phone owner, the corresponding mailbox, and the MTM for that mailbox.
        * @param aCalEntry calendar entry to evaluate
        * @param aMtmUid uid of the MTM
        * @return KErrNone or a system wide error code
        */                 
        virtual TInt GetSourceMtmUidL( const CCalEntry& aCalEntry, 
                                       TUid& aMtmUid ) = 0;     

        /**
        * Compares CCalEntry's phone owner field to its attendee list,
        * and returns matching attendee, or NULL if phone owner hasn't
        * been set or this attendee was not found.
        * @param aCalEntry meeting request entry
        * @return attendee, ownership is not transferred
        */
        virtual CCalAttendee* ThisAttendeeL( const CCalEntry& aCalEntry ) = 0;
            
        /**
        * Compares CCalEntry's phone owner field to organizer field,
        * and returns ETrue if they match.
        * If phone owner hasn't been set this method returns EFalse.
        * @param aCalEntry entry to evaluate
        * @return ETrue if phone owner is organizer
        */
        virtual TBool IsOrganizerL( const CCalEntry& aCalEntry ) = 0;
        
        /**
        * If phone owner field hasn't been set so far, this method compares
        * organizer field to the mailboxes in the device, and if no match
        * was found compares each attendee to the mailboxes. If match was found,
        * phone owner field is set, otherwise KErrNotFound is returned.
        * If aPrimaryBox is given, then that is used in matching before any
        * other mailbox. If not given, then default MR mailbox will have
        * the highest presedence followed by other MR-capable mailboxes.
        * @param aCalEntry entry to modify
        * @param aPrimaryBox primary mailbox for matching, if applicable
        * @return KErrNone or a system wide error code
        */
        virtual TInt SetPhoneOwnerL(
            CCalEntry& aCalEntry,
            TMsvId aPrimaryBox = KMsvNullIndexEntryId ) = 0;
            
        /**
        * Adds observer for CMRMailboxUtils.
        * @param aObserver observer to add
        */
        virtual void AddObserverL( MMRMailboxUtilsObserver& aObserver ) = 0;
        
        /**
        * Removes observer, panics if observer hasn't been added.
        * @param aObserver observer to remove
        */
        virtual void RemoveObserver( MMRMailboxUtilsObserver& aObserver ) = 0;
        
        /**
        * Accessor method for getting CMsvSession reference.
        * Leaves with a system wide error code if session doesn't exist due
        * to a problem with Messaging Server. Reference should only be used
        * immediatelly after querying it, since server may later request closing
        * the session.
        * @return session reference
        */
        virtual CMsvSession& MsvSessionL() = 0;            
            
    private: // data
    
        // for ECom to identify plug-in instance
        TUid iDtor_ID_Key;   
    };

#include <cmrmailboxutils.inl>

#endif //__CMRMAILBOXUTILS_H__
