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
*       UI Extension API for Meeting Request Utils.
*
*/



#ifndef MMRUTILSUISERVICES_H
#define MMRUTILSUISERVICES_H

// INCLUDES
#include <e32def.h>
#include <msvstd.h>
#include <CMRUtils.h>
#include <msvids.h>
#include <CalUser.h>
#include "CMRUtilsInternal.h"

// FORWARD DECLARATIONS
class CCalEntry;
class CCalInstance;

// CLASS DECLARATION

/**
 * Extension API for CMRUtils. This extension contains UI functionality.
 */
class MMRUtilsUiServices
    {
    public: // Constructors and destructors
    
        /**
        * Destructor, allows deletion through this interface.
        */
        virtual ~MMRUtilsUiServices() {};      
        
    public:  // New functions
    
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
        * @param aResponse responding status
        * @param aMailbox mailbox to use for sending
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
        * @param aMailbox mailbox to use for sending    
        * @return KErrNone or a system wide error code    
        */ 
        virtual TInt CancelWithUiL(
            const RPointerArray<CCalEntry>& aEntries,
            TMsvId aMailbox ) = 0;
        
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
        * Open SendUI editor for replying to the recipient(s) with an e-mail.
        * aSenderAddr is omitted if aRecipients does not contain MR sender.
        * @param aRecipients enumeration determining the recipients
        * @param aCalEntry entry to be used as a base for sending reply
        * @param aSenderAddr specifies the sender, if applicable
        * @param aMailbox mailbox to use for sending    
        */          
        virtual void ReplyToL( CMRUtilsInternal::TMailRecipients aRecipients,
                               const CCalEntry& aCalEntry,
                               const TDesC& aSenderAddr,
                               TMsvId aMailbox ) = 0;

        /**
        * Send given meeting request. Sending recipients are determined based
        * on the request type, e.g. request, respond, update etc.
        * This method does not involve an UI editor.
        * @param aCalEntry entry to be sent
        * @param aMailbox mailbox to use for sending       
        */             
        virtual void SendL( const CCalEntry& aCalEntry,
                            TMsvId aMailbox ) = 0;
    };

#endif  // MMRUTILSUISERVICES_H

// End of File
