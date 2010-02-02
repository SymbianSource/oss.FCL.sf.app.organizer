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
* Description:   Setting page opened from a setting item
*
*/


//debug
#include "calendarui_debug.h"

// INCLUDES
#include "calennestedsettingdialog.h"
#include "calensettingsuid.h"
#include <AknsBasicBackgroundControlContext.h>
#include <aknlists.h>
#include <Calendar.rsg>
#include <aknsettingitemlist.h>
#include "calendar.hrh"
#include <bldvariant.hrh> // For FeatureIds (lint warns without)
#include <featmgr.h>
#include <hlplch.h>

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------------------------
// CCalenNestedSettingDialog::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CCalenNestedSettingDialog* CCalenNestedSettingDialog::NewL( RPointerArray<CAknSettingItem>& aSettings )
    {
    TRACE_ENTRY_POINT;

    CCalenNestedSettingDialog* self = new( ELeave )CCalenNestedSettingDialog;
    CleanupStack::PushL( self );
    self->ConstructL( aSettings );
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenNestedSettingDialog::ConstructL
// Second phase of construction
// ----------------------------------------------------------------------------
//
void CCalenNestedSettingDialog::ConstructL( RPointerArray<CAknSettingItem>& aSettings )
    {
    TRACE_ENTRY_POINT;

    CAknDialog::ConstructL( R_CALENDAR_SETTING_MENUBAR );

    // Copy array contents to our array. We don't have ownership of the contents.
    iSettingItemArray = new( ELeave )CAknSettingItemArray( 2, 0, 0 );
    for ( TInt i( 0 ); i < aSettings.Count(); ++i )
        {
        iSettingItemArray->AppendL( aSettings[i] );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNestedSettingDialog::CCalenNestedSettingDialog
// C++ constructor.
// ----------------------------------------------------------------------------
//
CCalenNestedSettingDialog::CCalenNestedSettingDialog()
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNestedSettingDialog::~CCalenNestedSettingDialog
// Destructor
// ----------------------------------------------------------------------------
//
CCalenNestedSettingDialog::~CCalenNestedSettingDialog()
    {
    TRACE_ENTRY_POINT;

    delete iListBox;
    delete iBgContext;
    delete iSettingItemArray;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenNestedSettingDialog::OfferKeyEventL
// Handles key presses.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenNestedSettingDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                        TEventCode aType )
    {
    TRACE_ENTRY_POINT;

    if( aKeyEvent.iCode == EKeyNo )
        {
        // Red cancel button.
        TryExitL( EKeyNo );

        TRACE_EXIT_POINT;
        return EKeyWasNotConsumed; // Chain this one up to the main app so it closes calendar app.
        }
    else
        {
        iListBox->OfferKeyEventL( aKeyEvent, aType );
        // Draw because pressing up/down means we have to redraw the backgrouns skin.
        DrawDeferred();

        TRACE_EXIT_POINT;
        return EKeyWasConsumed; // Don't let other key events chain up to the calendar app.
        }
    }

// ----------------------------------------------------------------------------
// CCalenNestedSettingDialog::ProcessCommandL
// Processes commands.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNestedSettingDialog::ProcessCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;

    CAknDialog::ProcessCommandL( aCommandId );

    switch ( aCommandId )
        {
        case EAknSoftkeyChange:
        case EAknCmdOpen:
        case ECalenCmdChange:
            {
            CAknSettingItem* item = iSettingItemArray->At( iListBox->CurrentItemIndex() );
            // Open edit dialog if EAknCmdOpen, invert the value otherwise
            item->EditItemL( aCommandId == EAknCmdOpen );
            item->StoreL();
            }
            break;
        case EAknCmdHelp:
            if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(),
                                                     iAvkonAppUi->AppHelpContextL() );
                }
            break;
        case EAknCmdExit:
        case EAknSoftkeyExit:
            TryExitL( aCommandId );
            break;
        default:
            break;
        }

    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenNestedSettingDialog::HandlePointerEventL
// Processes Touch commands.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNestedSettingDialog::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	   
   if( !AknLayoutUtils::PenEnabled() )
    {
    return;
    }
    TInt index = iListBox->CurrentItemIndex();
    switch( aPointerEvent.iType )
            {
            case TPointerEvent::EButton1Down:
                {
                    iListBox->HandlePointerEventL(aPointerEvent);      
                    if( iListBox->CurrentItemIndex() == index)
                    {
                    iFirstTap = ETrue;
                    }
                    else
                    {
                    iFirstTap = EFalse;      
                    }
                }
                break;
            case TPointerEvent::EButton1Up:
                {
                   if(iFirstTap)
                    {
		            CAknSettingItem* item = iSettingItemArray->At( iListBox->CurrentItemIndex() );
		            item->EditItemL( ETrue );
		            item->StoreL();
		            DrawDeferred();
		            }
		            
                }
                break;
            case TPointerEvent::EDrag:
                {
                    iListBox->HandlePointerEventL(aPointerEvent);
                    if( iFirstTap  &&  iListBox->CurrentItemIndex() != index )
                    {
                    iFirstTap = EFalse;  
                    }
                }
                break;
            default:
                break;
            }
	}
// ----------------------------------------------------------------------------
// CCalenNestedSettingDialog::OkToExitL
// Returns ETrue if the button given is allowed to close settings.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenNestedSettingDialog::OkToExitL( TInt aButtonId )
    {
    TRACE_ENTRY_POINT;

    TBool retVal = EFalse;

    switch( aButtonId )
        {
        case EAknSoftkeyChange:
            {
             CAknSettingItem* item = iSettingItemArray->At( iListBox->CurrentItemIndex() );
            // Open edit dialog if EAknCmdOpen, invert the value otherwise
            item->EditItemL( EFalse );
            item->StoreL();
            }
            break;
        case EAknSoftkeyBack:
        default:
            {
            retVal = CAknDialog::OkToExitL( aButtonId );
            }
            break;
        }

    TRACE_EXIT_POINT;
    return retVal;
    }

// ----------------------------------------------------------------------------
// CCalenNestedSettingDialog::PreLayoutDynInitL
// Called before laying out control.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenNestedSettingDialog::PreLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;

    // Get the background skin
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,
                                                           CEikonEnv::Static()->EikAppUi()->ClientRect(),
                                                           ETrue );

    iListBox = new( ELeave )CAknSettingStyleListBox;

    iListBox->ConstructL( this, EAknListBoxSelectionList );


    // Set array to listbox
    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray( iSettingItemArray );
    // Ownership retained by us
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );

    iSettingItemArray->RecalculateVisibleIndicesL();
    iListBox->CreateScrollBarFrameL();
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
                        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenNestedSettingDialog::SizeChanged
// From CCoeControl. Called when the size changes.
// -----------------------------------------------------------------------------
//
void CCalenNestedSettingDialog::SizeChanged()
    {
    TRACE_ENTRY_POINT;

    CAknDialog::SizeChanged();

    TRect rect = Rect();

    if( iBgContext )
        {
        iBgContext->SetRect( rect );
        }

    if ( iListBox )
        {
        iListBox->SetRect( rect );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenNestedSettingDialog::Draw
// Draws the background skin
// -----------------------------------------------------------------------------
//
void CCalenNestedSettingDialog::Draw( const TRect& aRect ) const
    {
    TRACE_ENTRY_POINT

    CWindowGc& gc = SystemGc();
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TRect zerodRect( TPoint(0, 0), mainPane.Size() );
    iBgContext->SetRect( zerodRect );
    iBgContext->SetParentPos( mainPane.iTl );
//    iListBox->SetRect( zerodRect );
    AknsDrawUtils::Background( AknsUtils::SkinInstance(), iBgContext, this, gc, aRect );

    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenNestedSettingDialog::HandleResourceChange
// Handles resource changes
// -----------------------------------------------------------------------------
//
void CCalenNestedSettingDialog::HandleResourceChange( TInt aType )
    {
    TRACE_ENTRY_POINT;

    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
        SetRect( mainPane );
        TRect zerodRect( TPoint(0, 0), mainPane.Size() );
        iBgContext->SetRect( zerodRect );
        iBgContext->SetParentPos( mainPane.iTl );
        iListBox->SetRect( zerodRect );
        }

    CAknDialog::HandleResourceChange( aType );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenNestedSettingDialog::MopSupplyObject
// Supplies skinned mop object
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalenNestedSettingDialog::MopSupplyObject( TTypeUid aId )
    {
    TRACE_ENTRY_POINT;

    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        ASSERT( iBgContext );
        TRACE_EXIT_POINT;
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }

    TRACE_EXIT_POINT;
    return CAknDialog::MopSupplyObject( aId );
    }

// ----------------------------------------------------------------------------
// CCalenNestedSettingDialog::CountComponentControls
// Returns the number of controls contained in this control.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenNestedSettingDialog::CountComponentControls() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return 1;
    }

// ----------------------------------------------------------------------------
// CCalenNestedSettingDialog::ComponentControl
// Returns the control contained in this control.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenNestedSettingDialog::ComponentControl( TInt /*aIndex*/ ) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iListBox;
    }

// End of file
