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
* Description: Implementation for meeting request utils factory   
*
*/



// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "MRUtilsFactory.h"
#include "CMRUtilsEmailSender.h"
#include "CMRUtilsCalDbMgr.h"
#include "CMRUtilsUiServices.h"
#include <eikenv.h>
#include "ICalUILog.h"


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

enum TPanicCode
    {
    };

_LIT( KPanicMsg, "MRUtilsFactory" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
    
}  // namespace

// ----------------------------------------------------------------------------
// MEMBER FUNCTIONS
// ----------------------------------------------------------------------------
//

// ----------------------------------------------------------------------------
// MRUtilsFactory::CreateEmailSenderL
// ----------------------------------------------------------------------------
//        
MMRUtilsEmailSender* MRUtilsFactory::CreateEmailSenderL(
    CCalSession& aCalSession )
    {
    LOG("MRUtilsFactory::CreateEmailSenderL");
    return CMRUtilsEmailSender::NewL( aCalSession );
    }

MMRUtilsCalDbMgr* MRUtilsFactory::CreateCalDbMgrL(
    CCalSession& aCalSession,
    MMRUtilsObserver& aObserver )
    {
    return CMRUtilsCalDbMgr::NewL( aCalSession, aObserver );
    }
    
MMRUtilsUiServices* MRUtilsFactory::CreateUiServicesL(
    CMRUtilsInternal& aMRUtils,
    CCalSession& aCalSession,
    CMsvSession* aMsvSession,
    CMRMailboxUtils& aMRMailboxUtils )
    {
    LOG("MRUtilsFactory::CreateUiServicesL()");
    
    if ( !CEikonEnv::Static() )
        { 
        LOG("MRUtilsFactory::CreateUiServicesL() -> return null");
        return NULL;
        }
    else
        {
        LOG("MRUtilsFactory::CreateUiServicesL() creating instance");
        return CMRUtilsUiServices::NewL( aMRUtils,
                                         aCalSession,
                                         aMsvSession,
                                         aMRMailboxUtils );
        LOG("MRUtilsFactory::CreateUiServicesL() instance created");
        }
    }
    
// End of file
