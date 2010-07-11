/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CalenAgendaView implementation.
*
*/

//  User includes
#include "CalenRegionalInfoData.h"
#include "CalenLunarChinesePlugin.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CalenRegionalInfo::CCalenRegionalInfo
// -----------------------------------------------------------------------------
//
CalenRegionalInfo::CalenRegionalInfo(CCalenLunarChinesePlugin &plugin): mPlugin(plugin)
    {

    }

// -----------------------------------------------------------------------------
// CCalenRegionalInfo::~CCalenRegionalInfo
// -----------------------------------------------------------------------------
//
CalenRegionalInfo::~CalenRegionalInfo()
    {

    }

// -----------------------------------------------------------------------------
// CCalenRegionalInfo::showRegionalDetailsL
// its a slot ,calls when user clicked show lunar data option
// -----------------------------------------------------------------------------
//
void CalenRegionalInfo::showRegionalDetails()
    {
    mPlugin.ShowDetailsL();
    }
