/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#include "CalenEditorsPlugin.h"

//#include "CalenUids.hrh"
// These are from CalenUids.hrh but the file is causing problems as it's not exported.
#define KCalenEditorsImplementation             0x102071F4    // Calendar Default Editor Implementation UID

#include <CalendarVariant.hrh>

inline CCalenEditorsPlugin* CCalenEditorsPlugin::NewL()
    {
    TAny* interface = REComSession::CreateImplementationL(TUid::Uid(KCalenEditorsImplementation), _FOFF(CCalenEditorsPlugin, iDtor_ID_Key));   
    return reinterpret_cast<CCalenEditorsPlugin*>(interface);
    }    

inline CCalenEditorsPlugin::~CCalenEditorsPlugin()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    REComSession::FinalClose();
    }
