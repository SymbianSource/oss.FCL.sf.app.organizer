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

#ifndef MBCRESOURCEMANAGER_H
#define MBCRESOURCEMANAGER_H

#include <e32cmn.h> //TUid

class MBCUiEnv;
class CBCLayoutHandler;
class CBCBrushManager;
class CBCTextStyleManager;

class MBCResourceManager
    {
    public:  // Destructor
        /**
         * Destructor.
         */         
        virtual ~MBCResourceManager() { }
    
    public:
        virtual MBCUiEnv& UiEnv() = 0;
        virtual CBCBrushManager& BrushManager() = 0;
        virtual CBCLayoutHandler& LayoutHandler() = 0;
        virtual CBCTextStyleManager& TextStyleManager() = 0;
        //TextStyleManager();
        //
        
        virtual TAny* ResourceManagerExtension( TUid /*aExtensionUid*/ ){ return NULL; }
    };

#endif // MBCRESOURCEMANAGER_H
