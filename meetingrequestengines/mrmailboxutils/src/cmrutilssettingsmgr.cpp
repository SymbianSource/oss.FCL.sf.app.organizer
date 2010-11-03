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
* Description:  Provides class methods which takes care of meeting request related settings. 
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "cmrutilssettingsmgr.h"
#include <ICalInternalCRKeys.h>

#include <msvids.h>
#include <centralrepository.h>

// CONSTANTS

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::NewL
// ----------------------------------------------------------------------------
//
CMRUtilsSettingsMgr* CMRUtilsSettingsMgr::NewL()
	{
	CMRUtilsSettingsMgr* self = new( ELeave ) CMRUtilsSettingsMgr();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::CMRUtilsSettingsMgr
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRUtilsSettingsMgr::CMRUtilsSettingsMgr()
    {    
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::~CMRUtilsSettingsMgr
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CMRUtilsSettingsMgr::~CMRUtilsSettingsMgr()
    {   
    }
    
// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::ConstructL
// ----------------------------------------------------------------------------
//    
void CMRUtilsSettingsMgr::ConstructL()
    {
    // No implementation
    }

// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::GetDefaultMRMailBoxL
// ----------------------------------------------------------------------------
//    
TInt CMRUtilsSettingsMgr::GetDefaultMRMailBoxL( TMsvId& aDefaultBox )
    {
    aDefaultBox = KMsvNullIndexEntryId;
    TInt value( 0 );
     
    //Create central repository
    CRepository* cenRep = CRepository::NewLC( KCrUidICalUi );
    
    //Get default meeting request mailbox id from CentRep
    TInt err = cenRep->Get( KDefaultMeetingReqMailbox, value ); 
    
    CleanupStack::PopAndDestroy( cenRep ); 
    
    //If some other error than KErrNotFound Leave
    if ( err != KErrNone && err != KErrNotFound )
        {
        User::LeaveIfError( err ); 
        }
    
    if ( value == KMsvNullIndexEntryId )
        {
        //No value for default mailbox was found
        return KErrNotFound;      
        }
    
    aDefaultBox = value; 
        
    return err;    
    }

// ----------------------------------------------------------------------------
// CMRUtilsSettingsMgr::SetDefaultMRMailBoxL
// ----------------------------------------------------------------------------
//                
TInt CMRUtilsSettingsMgr::SetDefaultMRMailBoxL( TMsvId aDefaultBox )
    {
    //Create central repository
    CRepository* cenRep = CRepository::NewLC( KCrUidICalUi );
    
    //Set new default meeting request mailbox id to CentRep
    TInt err = cenRep->Set( KDefaultMeetingReqMailbox,
                            static_cast<TInt>( aDefaultBox ) );
    
    CleanupStack::PopAndDestroy( cenRep ); 
    
    User::LeaveIfError( err );
    
    return KErrNone;   
    }

// End of file
