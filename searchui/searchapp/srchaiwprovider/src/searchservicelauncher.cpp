/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:      This class is used to launch InDevice Search with a list of parameters
*
*/






// System includes
#include <AiwMenu.h>
#include <AiwCommon.h>
#include <AiwGenericParam.h>
#include <e32cmn.h>
#include <coemain.h>
#include <e32def.h>
#include <apgtask.h>
#include <s32mem.h>
#include <AknLaunchAppService.h>
#include <AiwGenericParam.h>

#include <apgcli.h>
#include <barsc.h>
#include <bautils.h>

// User Includes
#include "searchservicelauncher.h"

// -------------------------------------------------------------------------------------------------
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSearchServiceLauncher* CSearchServiceLauncher::NewL()
    {
    CSearchServiceLauncher* self = CSearchServiceLauncher::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
// -------------------------------------------------------------------------------------------------
// Symbian OS 2 phased constructor.
// -------------------------------------------------------------------------------------------------
//
CSearchServiceLauncher* CSearchServiceLauncher::NewLC()
    {
    CSearchServiceLauncher* self = new ( ELeave ) CSearchServiceLauncher();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -------------------------------------------------------------------------------------------------
// Destructor
// -------------------------------------------------------------------------------------------------
// 
CSearchServiceLauncher::~CSearchServiceLauncher()
    {
    }

// -------------------------------------------------------------------------------------------------
// Launch In-Device-Search with a list of parameters
// -------------------------------------------------------------------------------------------------
//
void CSearchServiceLauncher::SearchL( TInt /*aFunction*/, const CAiwGenericParamList& aParamList )
    {
	TApaAppInfo *appInfo = new ( ELeave ) TApaAppInfo;
    CleanupStack::PushL ( appInfo );
    RApaLsSession apaSession;
    User::LeaveIfError( apaSession.Connect() );
    CleanupClosePushL( apaSession );
    TInt error = apaSession.GetAppInfo( *appInfo, KSrchAppUid );
    User::LeaveIfError(error);
    CleanupStack::PopAndDestroy( &apaSession ); // apaSession

    if ( iLaunchAppService )
        {
        delete iLaunchAppService;
        iLaunchAppService = NULL;
        }

    iLaunchAppService = CAknLaunchAppService::NewL(appInfo->iUid, this, const_cast<CAiwGenericParamList*>(&aParamList));

    CleanupStack::PopAndDestroy( appInfo ); // appInfo
    }
    

// -------------------------------------------------------------------------------------------------
// Performs the second phase construction.
// -------------------------------------------------------------------------------------------------
//
void CSearchServiceLauncher::ConstructL()
    {
    }

    
// -------------------------------------------------------------------------------------------------
// Performs the first phase construction.
// -------------------------------------------------------------------------------------------------
//
CSearchServiceLauncher::CSearchServiceLauncher()
    {
    }




//end of file
