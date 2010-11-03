/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Drawing Day View Listbox data.
 *                Event bar and Event icon, event indicator
 *
*/



//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include    "calendaylistboxdata.h"

#include    <eikenv.h>
#include    <AknUtils.h>

// ================= MEMBER FUNCTIONS =======================

// Destructor
CCalenDayListboxData::~CCalenDayListboxData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;  
    }

// Constructor
CCalenDayListboxData::CCalenDayListboxData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;  
    }

// Two-phased constructor.
CCalenDayListboxData* CCalenDayListboxData::NewL()
    {
    TRACE_ENTRY_POINT; 
    
    CCalenDayListboxData* self = new(ELeave) CCalenDayListboxData();
    // Construct list animation by default
    self->ConstructLD( KAknsIIDQsnAnimList );
    CleanupStack::PushL(self);
    CleanupStack::Pop(); // self
    
    TRACE_EXIT_POINT; 
    return self;
    }

// ----------------------------------------------------
// CCalenDayListboxData::Draw
// Drawing item data and highlight
// (other items were commented in a header).
// ----------------------------------------------------
//
void CCalenDayListboxData::Draw(
    TListItemProperties aProperties,// Item property
    CWindowGc& aGc,                 // Graphics context
    const TDesC* aText,             // Drawing item text
    const TRect& aRect,             // Item rectangle
    TBool aHighlight,               // ETrue: hilighted
    const TColors& aColors) const   // Item color
    {
    TRACE_ENTRY_POINT; 
    
    // Based on AVKON's CFormattedCellListBoxData
    // Removed code is commented out. 
    const TRect &aItemRect = aRect;
    //DrawDefaultHighlight(aGc, aItemRect, aHighlight);

    // Draw the actual items.
    DrawFormatted(aProperties,aGc,aText,aItemRect,aHighlight,aColors);

    aGc.Reset();
    
    TRACE_EXIT_POINT; 
    }

// End of File
