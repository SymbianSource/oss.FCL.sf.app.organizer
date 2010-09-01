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
* Description:   ECom search interface definition.
*
*/



#ifndef CALENDARSEARCHPLUGINPANIC_H
#define CALENDARSEARCHPLUGINPANIC_H


_LIT( KCalendarSearchPlugIn,        "CCalendarSearchPlugIn");

enum TCalendarSearchPlugInPanics
    {
        ECalendarSearchPlugInWrongConstructionParameters = 1,
        ECalendarSearchPlugInActiveSearchingExist,
        ECalendarSearchPlugInNoOperation,
        ECalendarSearchPlugWrongState
    };
    

#endif // CALENDARSEARCHPLUGINPANIC_H
