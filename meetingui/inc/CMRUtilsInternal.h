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
*       Internal view of the CMRUtils.
*
*/



#ifndef __CMRUTILSINTERNAL_H__
#define __CMRUTILSINTERNAL_H__

//  INCLUDES
#include <CMRUtils.h>
#include <MMRUtilsTombsExt.h>
#include <msvstd.h>
#include <CalUser.h>

// FORWARD DECLARATIONS
class CCalInstanceView;

// CLASS DECLARATION


class CMRUtilsInternal :
public CMRUtils,
public MMRUtilsTombsExt
    {
    public: // Data types
    
        /**
        * Enumeration determining mail recipients.
        */        
        enum TMailRecipients
            {
            EOrganizer, // meeting organizer
            ESender,    // message sender
            EAll        // all, but not declined attendees
            };         
    
        /**
        * Codes of asynchronous operations.
        */ 
        enum TOpCode
            {
            EFirstOperation = CMRUtils::ELastOperation,
            EDeleteTombstones,       // delete some tombstone(s)
            };
                
    public: // Constructors and destructors

        /**
        * Constructs an ECom plug-in, KMeetingRequestUtilsImplUID implementation.
        * Client may give NULL pointer for aMsvSession if it is not available.
        *
        * @param aObserver mandatory utils feedback observer
        * @param aCalSession calendar session reference
        * @param aMsvSession, ownership not transferred
        * @return instantiated plug-in
        */
        static CMRUtilsInternal* NewL( MMRUtilsObserver& aObserver,
                                       CCalSession& aCalSession,
                                       CMsvSession* aMsvSession );

        /**
        * Destructor.
        */
        virtual ~CMRUtilsInternal();
        
    public: // New functions
    
        /**
        * Accessor for Agenda Model CCalEntryView owned by this utility.
        * Returns a valid pointer if status is EAvailable or
        * EAvailableWithoutTombs, otherwise NULL.        
        * @return entry view pointer, ownership not transferred
        */  
        virtual CCalEntryView* EntryView() = 0;       

        /**
        * Accessor for Agenda Model CCalInstanceView owned by this utility.
        * Returns a valid pointer if status is EAvailable or
        * EAvailableWithoutTombs, otherwise NULL.        
        * @return instance view pointer, ownership not transferred
        */  
        virtual CCalInstanceView* InstanceView() = 0; 

        /**
        * Fetches entries from the Agenda Model based on time range.
        * In case of repeating entries the entire sequence must
        * be within range (but modifying entries are considered separately).
        * Ownership of entries is transferred to caller.
        * See CCalEntryView documentation for other fetch variants.
        * @param aCalEntryArray will contain fetched entries
        * @param aTimeRange criteria
        */
        virtual void FetchEntriesL(
            RPointerArray<CCalEntry>& aCalEntryArray,
            const CalCommon::TCalTimeRange& aTimeRange ) = 0;
          
        /**
        * Fetches entry from the Agenda Model based on Uid and recurrence id.
        * See CCalEntryView documentation for other fetch variants. For fetching
        * originating entry use TCalTime set to Time::NullTTime().
        * Returns NULL if no matching entry was found.
        * @param aUid criteria
        * @param aRecurrenceId criteria        
        * @return entry, ownership is transferred to caller
        */  
        virtual CCalEntry* FetchEntryL( const TDesC8& aUid,
                                        const TCalTime& aRecurrenceId ) = 0;

        /**
        * Updates the given entry in the Agenda Model.
        * @param aCalEntry entry to update
        * @return KErrNone or a system wide error code
        */      
        virtual TInt UpdateEntryL( const CCalEntry& aCalEntry ) = 0;

        /**
        * This method deletes and re-creates the tombstone db file.
        * Reseting causes asynchronous initialization and client is notified
        * first with MMRUtilsObserver::ENotReady and later on with a completion
        * status code.
        */
        virtual void ResetTombsFileL() = 0;
        
        /**
        * This method deletes tombstones determined by aCalTimeRange.
        * This is asynchronous, user gets notified with
        * MMRUtilsObserver::HandleOperation( EDeleteTombstones,... )
        * @param aCalTimeRange range determining which entries to delete
        */
        virtual void DeleteTombstonesL(
            const CalCommon::TCalTimeRange& aCalTimeRange ) = 0;

        /**
        * Check entry: GUID, sequence number, existence in Agenda Model and
        * tombstone database. Same as CMRUtilsTombsExt::StoreL() but doesn't
        * actually store.
        * @param aCalEntry entry to check
        * @return result code
        */
        virtual TMRUtilsDbResult CheckEntryCondL( const CCalEntry& aCalEntry ) = 0;
        
        /**
        * Allows user to set attendee statuses and optionally edit and send
        * response to aEntries. If array contains multiple entries,
        * the same response value and information is sent to all given entries.
        * Cancelled or outdated entries are omitted, and so are those that
        * don't contain phone user as attendee whose response is requested.
        * Return values that are used are:
        * KErrNone if attendee status of one or more entries was set
        * KErrArgument if couldn't find any valid entry to respond
        * KErrCancel if user cancels responding (after setting attendee status)
        * @param aEntries entries belonging to same recurring meeting
        * @param aStatus responding status
        * @param aMailbox mailbox to use for sending response        
        * @return KErrNone or a system wide error code
        */
        virtual TInt RespondWithUiL( const RPointerArray<CCalEntry>& aEntries,
                                     CCalAttendee::TCalStatus aStatus,
                                     TMsvId aMailbox ) = 0;
                            
        /**
        * Allows organizer cancel, and optionally to edit and send 
        * cancellation of aEntries, i.e. recurring meeting or instance(s) of it.
        * Already cancelled, unsent or outdated entries are omitted,
        * and so are those attendees that have declined participation
        * to a particular entry. Return values that are used are:
        * KErrNone if one or more entries was cancelled
        * KErrArgument if couldn't find any valid entry to cancel
        * KErrCancel if user cancels sending (after canceling entry(s))
        * @param aEntries entries belonging to same recurring meeting
        * @param aMailbox mailbox to use for sending cancellation        
        * @return KErrNone or a system wide error code    
        */ 
        virtual TInt CancelWithUiL( const RPointerArray<CCalEntry>& aEntries,
                                    TMsvId aMailbox ) = 0;
    
        /**
        * Open SendUI editor for replying to the recipient(s) with an e-mail.
        * aSenderAddr is omitted if aRecipients does not contain MR sender.
        * @param aRecipients enumeration determining the recipients
        * @param aCalEntry entry to be used as a base for sending reply
        * @param aSenderAddr specifies the sender, if applicable
        * @param aMailbox mailbox to use for sending reply        
        */          
        virtual void ReplyToL( TMailRecipients aRecipients,
                               const CCalEntry& aCalEntry,
                               const TDesC& aSenderAddr,
                               TMsvId aMailbox ) = 0;
                               
        /**
        * Reads "from" field of the given mail message. Created descriptor
        * is left in the cleanup stack, ownership transferred to the caller.
        * Leaves with KErrNotSupported if client gave NULL MsvSession in
        * constructor.
        * @param aMessage mail message to read
        * @return "from" field
        */
		virtual HBufC* MailHeaderFromFieldLC( TMsvId aMessage ) = 0;

        /**
        * Reads "subject" field of the given mail message. Created descriptor
        * is left in the cleanup stack, ownership transferred to the caller.
        * Leaves with KErrNotSupported if client gave NULL MsvSession in
        * constructor.
        * @param aMessage mail message to read
        * @return "subject" field
        */		
		virtual HBufC* MailHeaderSubjectFieldLC( TMsvId aMessage ) = 0;
    };

#include "CMRUtilsInternal.inl"

#endif // __CMRUTILSINTERNAL_H__

// End of File
