/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline functions for the CalenInstanceId class.
*
*/

// System includes
#include <e32def.h>
#include <calentry.h>
#include <calinstance.h>
#include <caleninstanceid.h>

// Constants
const TCalLocalUid KNullLocalUid = 0;
const CCalEntry::TType KNullEntryType = CCalEntry::EAppt;

// ----------------------------------------------------------------------------
// TCalenInstanceId::CreateL
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline TCalenInstanceId TCalenInstanceId::CreateL( const CCalInstance& aInstance )
    {
    return TCalenInstanceId( aInstance.Entry().LocalUidL(),
                             aInstance.Time().TimeLocalL(),
                             aInstance.Entry().EntryTypeL(),
                             aInstance.InstanceIdL().iCollectionId );
    }

// --------------------------------------------------------------------------
// TCalenInstanceId::CreateL
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline TCalenInstanceId TCalenInstanceId::CreateL( const CCalEntry& aEntry,
						   const TCalTime& aInstanceTime,
						   const TCalCollectionId aColId )
    {
    return TCalenInstanceId( aEntry.LocalUidL(),
                             aInstanceTime.TimeLocalL(),
                             aEntry.EntryTypeL(), 
                             aColId );
    }

// --------------------------------------------------------------------------
// TCalenInstanceId::CreateL
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline TCalenInstanceId TCalenInstanceId::CreateL( const TCalLocalUid& aLuid,
						   const TTime& aInstanceTime,
						   TBool aInstanceViewed,
						   const TCalCollectionId aColId )
    {
    return TCalenInstanceId( aLuid ,aInstanceTime, aInstanceViewed, aColId );
    }


// -----------------------------------------------------------------------
// TCalenInstanceId::NullInstanceIdL
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline TCalenInstanceId TCalenInstanceId::NullInstanceId()
    {
    return TCalenInstanceId();
    }

// ------------------------------------------------------------------------
// TCalenInstanceId::TCalenInstanceId
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline TCalenInstanceId::TCalenInstanceId()
    : iEntryLocalUid( KNullLocalUid ),
      iInstanceTime( Time::NullTTime() ),
      iType( KNullEntryType ),
      iColId( -1 )
    {
    // No implemenatation yet
    }

// ----------------------------------------------------------------------------
// TCalenInstanceId::TCalenInstanceId
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline TCalenInstanceId::TCalenInstanceId( const TCalLocalUid& aLuid,
					   const TTime& aInstanceTime,
					   CCalEntry::TType aType,
					   const TCalCollectionId aColId ) : iEntryLocalUid( aLuid ),
					   				     iInstanceTime( aInstanceTime ),
									     iType( aType ),
									     iColId( aColId )
    {
    // No implemenatation yet
    }

// ----------------------------------------------------------------------------
// TCalenInstanceId::TCalenInstanceId
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline TCalenInstanceId::TCalenInstanceId( const TCalLocalUid& aLuid,
					   const TTime& aInstanceTime,
					   TBool aInstanceViewed,
					   const TCalCollectionId aColId ) : iEntryLocalUid( aLuid ),
					   				     iInstanceTime( aInstanceTime ),
									     iInstanceViewed( aInstanceViewed ),
									     iColId( aColId )
    {
    // No implemenatation yet
    }

// ----------------------------------------------------------------------------
// TCalenInstanceId::operator==
// rest of the details are commented in the header
// ----------------------------------------------------------------------------
//
inline TBool TCalenInstanceId::operator==( const TCalenInstanceId& aX ) const
    {
    return aX.iEntryLocalUid == iEntryLocalUid
        && aX.iInstanceTime == iInstanceTime
        && aX.iColId == iColId;
    }

// End of file
