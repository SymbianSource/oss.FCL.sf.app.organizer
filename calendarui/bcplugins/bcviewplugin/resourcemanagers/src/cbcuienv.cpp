/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include "cbcuienv.h"

#include <alf/alfenv.h>
#include <alf/alfdisplay.h>

#include <apgcli.h> // RApaLsSession
#include <coemain.h>

//namespace
//    {
//    const TUid KUidCalendar = {0x10005901};
//    }

CBCUiEnv::CBCUiEnv()
    {
    // Do nothing
    }

CBCUiEnv::~CBCUiEnv()
    {
    delete iEnv;
    }

CBCUiEnv* CBCUiEnv::NewL()
    {
    CBCUiEnv* self = new (ELeave) CBCUiEnv();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CBCUiEnv::ConstructL()
    {
    // Create Alf environment
    iEnv = CAlfEnv::NewL();

    //set up alf dipslay
    // FIXME: Where to get ClientRect()?
    TRect rect( TSize(480,640));//(240, 320));
    iDisplay = &iEnv->NewDisplayL( rect, CAlfEnv::ENewDisplayAsCoeControl );
    iDisplay->SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );    
    
    RApaLsSession ls;
    TApaAppInfo appInfo;
    User::LeaveIfError( ls.Connect() );
    User::LeaveIfError( ls.GetAppInfo( appInfo, TUid::Uid(0x10005901) ) );
    TFileName appFullName = appInfo.iFullName;
    ls.Close();

    // Load images and create freestyle texture manager
    TFileName imagePath;
    TParse appNameParse;
    User::LeaveIfError( appNameParse.Set( appFullName, NULL, NULL ) );
    imagePath = appNameParse.Drive();
    TFileName privatePath;
    CCoeEnv::Static()->FsSession().PrivatePath( privatePath );
    imagePath.Append( privatePath );
    iEnv->TextureManager().SetImagePathL( imagePath );    
    }

CAlfEnv& CBCUiEnv::AlfEnv()
    {
    return *iEnv;
    }

CAlfDisplay& CBCUiEnv::AlfDisplay()
    {
    return *iDisplay;
    }

TAny* CBCUiEnv::UiEnvExtension( TUid /*aExtensionUid*/ )
    {
    return NULL;
    }

// End of file
