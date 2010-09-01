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
* Description:   Static helper methods
*
*/




#ifndef MRHELPERS_H
#define MRHELPERS_H

//  INCLUDES
#include <e32base.h>
#include <msvstd.h>
#include <calentry.h>

// FORWARD DECLARATIONS
class CCalAttendee;
class CCalUser;
class CMsvSession;

/**
*  Static helpers methods for ICalUI.
*
*  @since Series S60 3.0
*/
class MRHelpers
    {    
    public: // Data types
    
        enum TCopyFields
            {
            ECopyFull,
            ECopyOrganizer,
            ECopySkeleton
            };
            
    public: // New functions
    
        /**
        * Creates a new copy of the given attendee.
        *
        * @param aSource 
        * @return attendee, ownership is transferred to caller.
        */
        static CCalAttendee* CopyAttendeeL( CCalAttendee& aSource );
        
        /**
        * Creates a new copy of the given attendee. Item is
        * left in the cleanup stack.
        *
        * @param aSource 
        * @return attendee, ownership is transferred to caller.
        */        
        static CCalAttendee* CopyAttendeeLC( CCalAttendee& aSource );        
        
        /**
        * Creates a new copy of the given cal user.
        *
        * @param aSource 
        * @return user, ownership is transferred to caller.
        */
        static CCalUser* CopyUserL( CCalUser& aSource );
        
        /**
        * Creates a new copy of the given cal user. Item is
        * left in the cleanup stack.
        *
        * @param aSource 
        * @return user, ownership is transferred to caller.
        */        
        static CCalUser* CopyUserLC( CCalUser& aSource );
        
        /**
        * Creates a copy with given method type.
        * Start/end time and DTSTAMP are set even for a skeleton.
        * @param aEntry source entry
        * @param aCopyType specifies copied data
        * @return instantiated entry, ownership transferred to caller
        */
        static CCalEntry* CopyEntryL( const CCalEntry& aEntry,
                                      CCalEntry::TMethod aMethod,
                                      TCopyFields aCopyType );
        
        /**
        * Creates a copy with given method type.
        * Start/end time and DTSTAMP are set even for a skeleton.
        * Copied entry is left in the cleanup stack.
        * @param aEntry source entry
        * @param aCopyType specifies copied data
        * @return instantiated entry, ownership transferred to caller
        */
        static CCalEntry* CopyEntryLC( const CCalEntry& aEntry,
                                       CCalEntry::TMethod aMethod,
                                       TCopyFields aCopyType );
                                       
        /**
        * Copies entry fields according to the copy type specification.
        * Unlike CCalEntry::CopyFromL() this function never copies
        * the entry method property.
        * @param aSource source for copying
        * @param aTarget target for copying
        * @param aCopyType specifies what data to copy
        */
        static void CopyFieldsL( const CCalEntry& aSource,
                                 CCalEntry& aTarget,
                                 TCopyFields aCopyType );
        
        /**
        * Finds out e-mail sender address, creates and assigns a
        * heap descriptor which is left to cleanupstack.
        * @param aSession opened session reference
        * @param aMsgId e-mail message entry id
        * @param aStripAlias instructs whether to strip possible alias name
        * @return address descriptor, ownership is transferred to caller
        */
        static HBufC* SenderAddressLC( CMsvSession& aSession,
                                       TMsvId aMsgId,
                                       TBool aStripAlias );

        /**
        * Returns TPtrC to all/part of the aAddress parameter
        * so that possible "mailto:" prefix is stripped out.
        *
        * @param aAddress address which might contain the prefix
        * @return TPtrC which represents only the address
        */            
        static TPtrC AddressWithoutMailtoPrefix( const TDesC& aAddress );
        
        
        /**
        * Parses drive name of dll(eg. "c:") and sets it to
        * aDriveName parameter.
        *
        * @param aDriveName TFileName where parsed drive letter
        * shall be set.
        */
        static void GetCorrectDllDriveL(TFileName& aDriveName);
        
        /**
        * 
        */
        static TInt LoadResourceL( const TDesC& aResourceFile,
                                   const TDesC& aResourcePath );
    };

#endif      // MRHELPERS_H

// End of File

