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
*  Description : Class looking after alarm fields for forms.
*
*/

//debug
#include "calendarui_debug.h"
//User includes
#include "CalenLunarSettingItem.h"
#include "CalenKoreanLunarPlugin.h"

//system includes
#include <CalenKoreanLunarPluginData.rsg>
#include <StringLoader.h>

const TInt KLunarSettingItemId = 55000; //FIXME does this has to be globally unique ?

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CCalenLunarSettingItem::NewLC()
// ---------------------------------------------------------------------------
//
CCalenLunarSettingItem* CCalenLunarSettingItem::NewLC( TInt aOrdinal, 
        CCalenKoreanLunarPlugin& aPlugin )
    {
    TRACE_ENTRY_POINT;

    CCalenLunarSettingItem* self = new (ELeave) CCalenLunarSettingItem( aPlugin );
    CleanupStack::PushL(self);
    // Construct setting item with parametrized values
    HBufC* itemTitle = StringLoader::LoadLC(R_CALEN_LUNAR_SETTING_TITLE2);
    self->ConstructL( EFalse, aOrdinal, *itemTitle, NULL,
              R_CALEN_LUNAR_SETTING_PAGE, EAknCtPopupSettingList,
              NULL, R_CALEN_LUNAR_SETTING_TEXTS);
    CleanupStack::PopAndDestroy( itemTitle );
    // no pop, because this is NewLC

    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// CCalenLunarSettingItem::CCalenLunarSettingItem()
// ---------------------------------------------------------------------------
//
CCalenLunarSettingItem::CCalenLunarSettingItem( CCalenKoreanLunarPlugin& aPlugin )
    : CAknBinaryPopupSettingItem( KLunarSettingItemId, iLunarEnabled ),
      iPlugin( aPlugin )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarSettingItem::~CCalenLunarSettingItem()
// ---------------------------------------------------------------------------
//
CCalenLunarSettingItem::~CCalenLunarSettingItem()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarSettingItem::StoreL()
// ---------------------------------------------------------------------------
//
void CCalenLunarSettingItem::StoreL()
    {
    TRACE_ENTRY_POINT;

    CAknBinaryPopupSettingItem::StoreL();
    iPlugin.StoreEnabledStatusL( iLunarEnabled );

    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CCalenLunarSettingItem::LoadL()
// ---------------------------------------------------------------------------
//
void CCalenLunarSettingItem::LoadL()
    {
    TRACE_ENTRY_POINT;

    iLunarEnabled = iPlugin.LoadEnabledStatusL();
    CAknBinaryPopupSettingItem::LoadL();

    TRACE_EXIT_POINT;
    }

// End of file
