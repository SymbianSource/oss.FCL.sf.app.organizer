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
* Description: CCalenThaiSettingItem.
*
*/

//debug
#include "calendarui_debug.h"

#include "CalenThaiSettingItem.h"

#include "CalenThaiPlugin.h"
#include <CalenThaiPluginData.rsg>

#include <StringLoader.h>


const TInt KThaiSettingItemId = 55000; //FIXME does this has to be globally unique ?

/////////////////////////////////////////////////////////////////////////
// CCalenThaiSettingItem
////////////////////////////////////////////////////////////////////////

CCalenThaiSettingItem* CCalenThaiSettingItem::NewLC(TInt aOrdinal, CCalenThaiPlugin& aPlugin)
    {
    TRACE_ENTRY_POINT;
    
    CCalenThaiSettingItem* self = new (ELeave) CCalenThaiSettingItem( aPlugin );
    CleanupStack::PushL(self);
    // Construct setting item with parametrized values
    HBufC* itemTitle = StringLoader::LoadLC(R_CALEN_THAI_SETTING_TITLE);
    self->ConstructL( EFalse, aOrdinal, *itemTitle, NULL, 
              R_CALEN_THAI_SETTING_PAGE, EAknCtPopupSettingList, 
              NULL, R_CALEN_THAI_SETTING_TEXTS);
    CleanupStack::PopAndDestroy( itemTitle );
    // no pop, because this is NewLC 
    
    TRACE_EXIT_POINT;
    return self;
    }

CCalenThaiSettingItem::CCalenThaiSettingItem( CCalenThaiPlugin& aPlugin )
    : CAknBinaryPopupSettingItem(KThaiSettingItemId, iThaiEnabled), 
      iPlugin( aPlugin )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

CCalenThaiSettingItem::~CCalenThaiSettingItem()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

void CCalenThaiSettingItem::StoreL()
    {
    TRACE_ENTRY_POINT;
    
    CAknBinaryPopupSettingItem::StoreL();
    //iPlugin.StoreEnabledStatusL( iThaiEnabled );
    
    TRACE_EXIT_POINT;
    }

void CCalenThaiSettingItem::LoadL()
    {
    TRACE_ENTRY_POINT;
    
    //iThaiEnabled = iPlugin.LoadEnabledStatusL();
    CAknBinaryPopupSettingItem::LoadL();
    
    TRACE_EXIT_POINT;
    }




