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




// INCLUDE FILES
#include "MRHelpers.h"
#include "MREntryConsultant.h"
#include <caluser.h> // CCalUser
#include <miuthdr.h> // CImHeader
#include <msvapi.h> // CMsvSession
#include <msvuids.h> // KUidMsvMessageEntry
#include <miutpars.h> // TImMessageField
#include <bautils.h> // BaflUtils
#include <coemain.h> // CCoeEnv

// CONSTANTS

/// Unnamed namespace for local definitions
namespace {

_LIT( KMailtoMatchPattern, "mailto:*" ); // this is never localized
const TInt KMailtoLength = 7; // "mailto:" length

enum TPanicCode
    {
    EPanicNullMsvId = 1,
    EIllegalMsvEntryType,
    EPanicNoOrganizer
    };

_LIT( KPanicMsg, "MRHelpers" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
    
}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// MRHelpers::CopyAttendeeL
// ----------------------------------------------------------------------------
//    
CCalAttendee* MRHelpers::CopyAttendeeL( CCalAttendee& aSource )
	{
	CCalAttendee* copy = CopyAttendeeLC( aSource );
	CleanupStack::Pop( copy );
	return copy;
	}

// ----------------------------------------------------------------------------
// MRHelpers::CopyAttendeeLC
// ----------------------------------------------------------------------------
//    
CCalAttendee* MRHelpers::CopyAttendeeLC( CCalAttendee& aSource )
	{
	CCalAttendee* copy = CCalAttendee::NewL( aSource.Address(), 
	                                         aSource.SentBy() );
    CleanupStack::PushL( copy );
    copy->SetCommonNameL( aSource.CommonName() );
    copy->SetRoleL( aSource.RoleL() );
	copy->SetStatusL( aSource.StatusL() );
	copy->SetResponseRequested( aSource.ResponseRequested() );
	return copy;
	}

// ----------------------------------------------------------------------------
// MRHelpers::CopyUserL
// ----------------------------------------------------------------------------
//    
CCalUser* MRHelpers::CopyUserL( CCalUser& aSource )
	{
	CCalUser* copy = CopyUserLC( aSource );
	CleanupStack::Pop( copy );
	return copy;
	}

// ----------------------------------------------------------------------------
// MRHelpers::CopyUserLC
// ----------------------------------------------------------------------------
//    
CCalUser* MRHelpers::CopyUserLC( CCalUser& aSource )
	{
	CCalUser* copy = CCalUser::NewL( aSource.Address(), 
	                                 aSource.SentBy() );
    CleanupStack::PushL( copy );
    copy->SetCommonNameL( aSource.CommonName() );
	return copy;
	}

// ----------------------------------------------------------------------------
// MRHelpers::CopyEntryL
// ----------------------------------------------------------------------------
//          
CCalEntry* MRHelpers::CopyEntryL(
    const CCalEntry& aEntry,
    CCalEntry::TMethod aMethod,
    TCopyFields aCopyType )
    {
	CCalEntry* copy = CopyEntryLC( aEntry, aMethod, aCopyType );
	CleanupStack::Pop( copy );
	return copy;    
    }
	
// ----------------------------------------------------------------------------
// MRHelpers::CopyEntryLC
// ----------------------------------------------------------------------------
//          
CCalEntry* MRHelpers::CopyEntryLC(
    const CCalEntry& aEntry,
    CCalEntry::TMethod aMethod,
    TCopyFields aCopyType )
    {
    CCalEntry* copy = NULL;
    HBufC8* uid = aEntry.UidL().AllocLC();
    if ( MREntryConsultant::IsModifyingEntryL( aEntry ) )
        {
        copy = CCalEntry::NewL( aEntry.EntryTypeL(),
                                uid,
                                aMethod,
                                aEntry.SequenceNumberL(),
                                aEntry.RecurrenceIdL(),
                                aEntry.RecurrenceRangeL() );
        }    
    else
        {
        copy = CCalEntry::NewL( aEntry.EntryTypeL(),
                                uid,
                                aMethod,
                                aEntry.SequenceNumberL() );
        }
    CleanupStack::Pop( uid ); // ownership transferred to the copy entry
    CleanupStack::PushL( copy );

    CopyFieldsL( aEntry, *copy, aCopyType );
            
    return copy;
    } 

// ----------------------------------------------------------------------------
// MRHelpers::CopyFieldsL
// ----------------------------------------------------------------------------
//   
void MRHelpers::CopyFieldsL(
    const CCalEntry& aSource,
    CCalEntry& aTarget,
    TCopyFields aCopyType )
    {
    if ( aCopyType == ECopyFull )
        {
        CCalEntry::TMethod tmpMethod( aTarget.MethodL() );    
        aTarget.CopyFromL( aSource );
        // We must re-set the method to the original value
        aTarget.SetMethodL( tmpMethod );
        }
    else
        {
        if ( aCopyType == ECopyOrganizer )
            {
            __ASSERT_DEBUG( aSource.OrganizerL(), Panic( EPanicNoOrganizer ) );
            CCalUser* organizer = CopyUserLC( *( aSource.OrganizerL() ) );
            aTarget.SetOrganizerL( organizer );
            CleanupStack::Pop(); // ownership transferred
            }
        
        // these are required for entries (start and end time actually not
        // for cancels and responses, but won't do any harm either):
        aTarget.SetStartAndEndTimeL( aSource.StartTimeL(), aSource.EndTimeL() );
        aTarget.SetDTStampL( aSource.DTStampL() );
        }
    }
    
// ----------------------------------------------------------------------------
// MRHelpers::SenderAddressLC
// ----------------------------------------------------------------------------
//    
HBufC* MRHelpers::SenderAddressLC(
    CMsvSession& aSession,
    TMsvId aMsgId,
    TBool aStripAlias )
    {
	__ASSERT_DEBUG( aMsgId != KMsvNullIndexEntryId, Panic( EPanicNullMsvId ) );
    CMsvEntry* entry = aSession.GetEntryL( aMsgId );
    __ASSERT_ALWAYS( entry->Entry().iType == KUidMsvMessageEntry,
                     Panic( EIllegalMsvEntryType ) );

    // Read sender from the message headers and allocate heap descriptor:

    CleanupStack::PushL( entry );
    CMsvStore* store = entry->ReadStoreL();
    CleanupStack::PushL( store );
    CImHeader* header = CImHeader::NewLC();
    header->RestoreL( *store );    
    TPtrC senderPtr( header->From() );
    if ( aStripAlias )
        {
        TImMessageField addrParser;
        senderPtr.Set(
            addrParser.GetValidInternetEmailAddressFromString( senderPtr ) );
        }
    HBufC* senderAddr = senderPtr.AllocL();
    CleanupStack::PopAndDestroy( 3, entry ); // header, store, entry
    CleanupStack::PushL( senderAddr );            
    return senderAddr;
    }

// ----------------------------------------------------------------------------
// MRHelpers::AddressWithoutMailtoPrefix
// ----------------------------------------------------------------------------
//    
TPtrC MRHelpers::AddressWithoutMailtoPrefix( const TDesC& aAddress )
	{
	TPtrC addrWithoutPrefix;
    if ( aAddress.MatchF( KMailtoMatchPattern ) != KErrNotFound )
    	{
        addrWithoutPrefix.Set( aAddress.Mid( KMailtoLength ) );
        }
    else
    	{
        addrWithoutPrefix.Set( aAddress );
        }
    return addrWithoutPrefix;
	}
	
// ----------------------------------------------------------------------------
// MRHelpers::GetCorrectDllDriveL
// ----------------------------------------------------------------------------
//    
void MRHelpers::GetCorrectDllDriveL( TFileName& aDriveName )
    {
    TParse parse;
    Dll::FileName( aDriveName );
	User::LeaveIfError( parse.Set( aDriveName, NULL, NULL ) );
    aDriveName = parse.Drive(); // contains drive, e.g. "c:"
    }

// ----------------------------------------------------------------------------
// MRHelpers::LoadResourceL
// ----------------------------------------------------------------------------
//    
TInt MRHelpers::LoadResourceL(
    const TDesC& aResourceFile,
    const TDesC& aResourcePath )
    {
    TFileName pathAndFile;
    MRHelpers::GetCorrectDllDriveL( pathAndFile ); // contains drive, e.g. "c:"    
    pathAndFile.Append( aResourceFile ); // e.g. "c:MyResource.rsc"
    
    TParse parse;
    parse.Set( pathAndFile, &aResourcePath, NULL );
    pathAndFile = parse.FullName(); // now we have full (unlocalized) file name
    
    // Find the resource file for the nearest language
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(),
                                    pathAndFile );
    TInt offset = CCoeEnv::Static()->AddResourceFileL( pathAndFile );
    return offset;
    }

//  End of File
