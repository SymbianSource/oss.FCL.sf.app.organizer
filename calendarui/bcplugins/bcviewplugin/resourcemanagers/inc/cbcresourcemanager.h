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


#ifndef CBCRESOURCEMANAGER_H
#define CBCRESOURCEMANAGER_H

#include <e32base.h>
#include "mbcresourcemanager.h"

class CBCLayoutHandler;
class CBCBrushManager;
class CBCTextStyleManager;
class MBCUiEnv;
class CBCUiEnv;

class CBCResourceManager : public CBase, public MBCResourceManager
    {
    public:
        static CBCResourceManager* NewL();
        ~CBCResourceManager();
        
    public:
        MBCUiEnv& UiEnv();
        CBCBrushManager& BrushManager();
        CBCLayoutHandler& LayoutHandler();        
        CBCTextStyleManager& TextStyleManager();
        TAny* ResourceManagerExtension( TUid aExtensionUid );
        
    private:
        CBCResourceManager();
        void ConstructL();
        
    private:
        /// Own:
        CBCLayoutHandler* iLayoutHandler;
        /// Own:
        CBCBrushManager* iBrushManager;
        /// Own:
        CBCTextStyleManager* iTextStyleManager;
        /// Own:
        CBCUiEnv* iUiEnv;
    };

#endif // CBCRESOURCEMANAGER_H
