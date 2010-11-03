/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Drawing PreView Listbox data.
*
*/

// INCLUDE FILES
#include    "bcpreviewlistboxdata.h"
#include    "bcpreviewdefines.h"

#include    <eikenv.h>
#include    <eiklbi.h>
#include    <AknUtils.h>

// ================= MEMBER FUNCTIONS =======================

// Destructor
CBCPreviewListboxData::~CBCPreviewListboxData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// Constructor
CBCPreviewListboxData::CBCPreviewListboxData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// Two-phased constructor.
CBCPreviewListboxData* CBCPreviewListboxData::NewL()
    {
    TRACE_ENTRY_POINT;
    CBCPreviewListboxData* self = new(ELeave) CBCPreviewListboxData();
    self->ConstructLD();

    return self;
    TRACE_EXIT_POINT;
    }

// End of File
