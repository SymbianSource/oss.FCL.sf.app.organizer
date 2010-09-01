/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar setting item which opens to a settings
*                 page when clicked on.
*
*/


//debug
#include "calendarui_debug.h"

// INCLUDES
#include "calennestedsettingitem.h"
#include <Calendar.rsg>
#include <aknsettingpage.h>
#include "calennestedsettingdialog.h"
#include <akntitle.h>
#include <eikspane.h>

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CCalenNestedSettingItem::CCalenCheckboxSettingItem
// Constructor. Does not take ownership of aSettings.
// ----------------------------------------------------------------------------
//
CCalenNestedSettingItem::CCalenNestedSettingItem( TInt aId,
                                                  RPointerArray<CAknSettingItem>& aUnownedSettings,
                                                  const TDesC& aTitle )
    : CAknSettingItem( aId ),
      iUnownedSettings( aUnownedSettings ),
      iTitle( aTitle )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNestedSettingItem::~CCalenNestedSettingItem
// Destructor.
// ----------------------------------------------------------------------------
//
CCalenNestedSettingItem::~CCalenNestedSettingItem()
    {
    TRACE_ENTRY_POINT;

    iOwnedSettings.ResetAndDestroy();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNestedSettingItem::CompleteConstructionL
// Called by the framework to complete construction.
// ----------------------------------------------------------------------------
//
void CCalenNestedSettingItem::CompleteConstructionL()
    {
    TRACE_ENTRY_POINT;

    // We need to take ownership of the settings during construction.
    // Keep them in order.
    while ( iUnownedSettings.Count() )
        {
        iOwnedSettings.AppendL( iUnownedSettings[0] );
        iUnownedSettings.Remove( 0 );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNestedSettingItem::EditItemL
// From CAknSettingItem. Edits the item.
// ----------------------------------------------------------------------------
//
void CCalenNestedSettingItem::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    TRACE_ENTRY_POINT;

    // Set the status pane title to the plugin name.
    CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
    CAknTitlePane* tp = static_cast<CAknTitlePane*>(
                                 sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    // Copy the current text. We'll set it back when the dialog closes.
    HBufC* oldTitle = tp->Text()->AllocLC();
    tp->SetTextL( iTitle );

    CCalenNestedSettingDialog* dlg = CCalenNestedSettingDialog::NewL( iOwnedSettings );
    TInt retValue = dlg->ExecuteLD( R_CALEN_SETTING_DIALOG );

    // Restore old title.
    tp->SetTextL( *oldTitle );
    CleanupStack::PopAndDestroy( oldTitle );

    TRACE_EXIT_POINT;
    }

// End of file
