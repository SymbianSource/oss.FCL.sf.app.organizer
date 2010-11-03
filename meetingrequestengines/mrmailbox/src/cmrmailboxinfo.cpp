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
* Description: Provides class methods for containing information for a mailbox. 
*
*/


// INCLUDES
#include "cmrmailboxinfo.h"


// Unnamed namespace for local definitions
namespace
{
}  // namespace

// ========================= MEMBER FUNCTIONS ================================


// ----------------------------------------------------------------------------
// CMRMailBoxInfo::CMRMailBoxInfo
// Default constructor
// ----------------------------------------------------------------------------
//
CMRMailBoxInfo::CMRMailBoxInfo(
    TUid aMtmUid,
    TMsvId aEntryId )
    : iMtmUid( aMtmUid ),
      iEntryId( aEntryId )
    {
    }

// ----------------------------------------------------------------------------
// CMRMailBoxInfo::~CMRMailBoxInfo
// Destructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CMRMailBoxInfo::~CMRMailBoxInfo()
    {
    delete iMailBoxName;    
    delete iEmailAddress;
    }

// ----------------------------------------------------------------------------
// CMRMailBoxInfo::NewL
// Symbian two-phased constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
EXPORT_C CMRMailBoxInfo* CMRMailBoxInfo::NewL(
    const TDesC& aName,
    const TDesC& aAddress,
    TUid aMtmUid,
    TMsvId aEntryId )
	{
	CMRMailBoxInfo* self = new( ELeave ) CMRMailBoxInfo( aMtmUid, aEntryId );
	CleanupStack::PushL( self );
	self->ConstructL( aName, aAddress );
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// CMRMailBoxInfo::ConstructL
// 2nd phase constructor.
// ----------------------------------------------------------------------------
//
void CMRMailBoxInfo::ConstructL(    
    const TDesC& aName,
    const TDesC& aAddress )
    {
    iMailBoxName = aName.AllocL();        
    iEmailAddress = aAddress.AllocL();
    }

// ----------------------------------------------------------------------------
// CMRMailBoxInfo::MtmUid
// ----------------------------------------------------------------------------
//    
EXPORT_C TUid CMRMailBoxInfo::MtmUid() const
    {
    return iMtmUid;    
    }

// ----------------------------------------------------------------------------
// CMRMailBoxInfo::EntryId
// ----------------------------------------------------------------------------
//    
EXPORT_C TMsvId CMRMailBoxInfo::EntryId() const
    {
    return iEntryId;    
    }
    
// ----------------------------------------------------------------------------
// CMRMailBoxInfo::Address
// ----------------------------------------------------------------------------
//
EXPORT_C TPtrC CMRMailBoxInfo::Address() const
    {
    return *iEmailAddress;    
    }
    
// ----------------------------------------------------------------------------
// CMRMailBoxInfo::Name
// ----------------------------------------------------------------------------
//
EXPORT_C TPtrC CMRMailBoxInfo::Name() const 
    {
    return *iMailBoxName;
    }
    
// End of file
