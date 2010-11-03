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
* Description:   Static helper methods for analyzing entries
*
*/




#ifndef MRENTRYCONSULTANT_H
#define MRENTRYCONSULTANT_H

//  INCLUDES
#include <e32base.h>
#include <MAgnEntryUi.h>

// FORWARD DECLARATIONS
class CCalEntry;
class CCalUser;
class CCalAttendee;
class CMRUtilsInternal;
class CMRMailboxUtils;


/**
*  Static helpers methods for MR Viewers for analyzing entry
*  content and properties.
*
*  @since Series S60 3.0
*/
class MREntryConsultant
    {        	    
    public: // New functions

        /**
        * Tells the mailbox id to use for sending a request or a response.
        * From mail application we may only respond, and in that case we
        * use the incoming mailbox. From other places we must use the
        * default MR mailbox setting.
        * @param aInParams input parameters for AgnEntryUi
        * @param aUtils reference to MR Utils
        * @return mailbox id, KMsvNullIndexEntryId if not available
        */
        static TMsvId SendingMailBoxL(
            const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
            CMRMailboxUtils& aUtils );
            
        /**
        * Tells whether the entry belongs to a repeating meeting.
        * The definition of a repeating meeting is not that there must be
        * multiple instances, but that repeating properties exist (e.g. RRule).
        */
        static TBool IsRepeatingMeetingL( const CCalEntry& aEntry );
    
        /**
        * Tests whether given entry is an originating entry or a modifying
        * entry.
        * @param aEntry to evaluate
        * @return ETrue if modifying entry, EFalse otherwise
        */
        static TBool IsModifyingEntryL( const CCalEntry& aEntry );
        
        /**
        * Does entry need saving, i.e. is it new or does it differ from the one 
        * in the db?
        * TODO: Symbian API does not notice minor changes (e.g. description,
        * location). If taking this method in use consider that problem.
        * @param aEntry
        * @param aUtils
        * @return ETrue if needs saving
        */
        static TBool IsEditedL( const CCalEntry& aEntry,
                                CMRUtilsInternal& aUtils );
        
        /**
        * Has this entry been sent at all yet? It is sent if entry's
        * status is not ENullStatus.
        * @param aEntry
        * @return ETrue if request has been sent
        */
        static TBool IsSentL( const CCalEntry& aEntry );
        
        /**
        * Has this particular entry with it's latest saved content been sent?
        * This is determined from the DTSTAMP value which is null until
        * up to date entry is sent.
        * @param aEntry       
        * @return ETrue if entry has been sent
        */
        static TBool IsLatestSavedSentL( const CCalEntry& aEntry );
           
        /**
        * Checks if either entire meeting is cancelled, or if entry is
        * cancellation for this particular attendee.
        * @param aEntry
        * @return ETrue if meeting cancelled from user's point of view
        */
        static TBool IsCancelledL( const CCalEntry& aEntry,
                                   CMRMailboxUtils& aUtils );
                                   
        /**
        * Checks whether entry with the same UID and RECURRENCE-ID exists in
        * database. In strict mode also SEQUENCE and DTSTAMP are evaluated.
        * @param aEntry 
        * @param aUtils
        * @param aStrictMode to be used when entries must be identical
        * @return ETrue if database entry exists
        */   
        static TBool ExistsInDbL( const CCalEntry& aEntry,
                                  CMRUtilsInternal& aUtils,
                                  TBool aStrictMode = EFalse );

        /**
        * Checks whether entry is out of date or not.
        * @param aEntry entry to check
        * @return ETrue if entry is outdated
        */                                   
        static TBool IsEntryOutOfDateL( const CCalEntry& aEntry );
        
        /**
        * Returns phone owner's address, KNullDesC if not found.
        * @param aEntry entry to evaluate
        * @param aUtils utility reference        
        * @return address
        */                                   
        static const TDesC& PhoneOwnerAddrL( const CCalEntry& aEntry,
                                             CMRMailboxUtils& aUtils );
                                             
        /**
        * Searches for the given attendee in aEntry. This is usable e.g.
        * in case of response, when compare and update attendee in
        * response and request. Matching is done based on e-mail address.
        * @param aAttendee reference attendee to be found
        * @aEntry entry containing possibly multiple attendees
        * @return matching attendee or NULL, ownership is not transferred
        */
        static CCalAttendee* EqualAttendeeL( const CCalAttendee& aAttendee,
                                             const CCalEntry& aEntry );
                                             
        /**
        * Method which checks if start and end time span multiple days.
        * @param aEntry
        * @return ETrue if spans multiple days
        */
        static TBool SpansManyDaysL( const TCalTime& aStartTime,
                                     const TCalTime& aEndTime );
        
    private:
    
        CMRMailboxUtils* iMailboxUtils;    
    };

#endif      // MRENTRYCONSULTANT_H

// End of File

