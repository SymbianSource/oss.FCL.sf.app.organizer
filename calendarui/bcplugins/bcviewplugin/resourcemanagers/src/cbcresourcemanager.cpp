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


#include "cbcresourcemanager.h"
#include "cbcuienv.h"
#include "cbclayouthandler.h"
#include "cbctextstylemanager.h"
#include "cbcbrushmanager.h"

CBCResourceManager* CBCResourceManager::NewL()
    {
    CBCResourceManager* self = new (ELeave) CBCResourceManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CBCResourceManager::CBCResourceManager()
    {
    // Do nothing
    }

CBCResourceManager::~CBCResourceManager()
    {
    delete iBrushManager;
    delete iTextStyleManager;
    delete iLayoutHandler;
    delete iUiEnv;
    }

void CBCResourceManager::ConstructL()
    {
    iUiEnv = CBCUiEnv::NewL();
    
    iLayoutHandler = CBCLayoutHandler::NewL( iUiEnv->AlfEnv() );
    iTextStyleManager = 
        CBCTextStyleManager::NewL( iUiEnv->AlfEnv(), *iLayoutHandler );
    iBrushManager = 
        CBCBrushManager::NewL( iUiEnv->AlfEnv(), *iLayoutHandler );    
    }

MBCUiEnv& CBCResourceManager::UiEnv()
    {
    return *iUiEnv;
    }

CBCLayoutHandler& CBCResourceManager::LayoutHandler()
    {
    return *iLayoutHandler;
    }

CBCBrushManager& CBCResourceManager::BrushManager()
    {
    return *iBrushManager;
    }

CBCTextStyleManager& CBCResourceManager::TextStyleManager()
    {
    return *iTextStyleManager;
    }

TAny* CBCResourceManager::ResourceManagerExtension( TUid /*aExtensionUid*/ )
    { 
    return NULL; 
    }
// end of file

