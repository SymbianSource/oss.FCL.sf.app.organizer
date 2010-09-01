/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Role of this class is to show setting view and to set data.
 *
*/

#include <aknlists.h>
#include <AknsBasicBackgroundControlContext.h>
#include <akntitle.h>
#include <aknsettingitemlist.h>
#include <bldvariant.hrh> // For FeatureIds (lint warns without)
#include <featmgr.h>
#include <hlplch.h>
#include <StringLoader.h>
#include <csxhelp/cale.hlp.hrh>
#include <Calendar.rsg>
#include <calencommands.hrh>            // Calendar commands

#include "calendarui_debug.h"
#include "calensettingsui.h"
#include "calensettingsuicontainer.h"
#include "calendar.hrh"
#include "CalenUid.h"
#include "calenglobaldata.h"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CCalenSettingsUi::NewL
// 1st phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenSettingsUi* CCalenSettingsUi::NewL( 
                                    CCalenCustomisationManager& aCustomisationManager )
    {
    TRACE_ENTRY_POINT;

    CCalenSettingsUi* self = new(ELeave) CCalenSettingsUi( aCustomisationManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUi::CCalenSettingsUi
// First stage of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenSettingsUi::CCalenSettingsUi( CCalenCustomisationManager& aCustomisationManager )
    : iCustomisationManager( aCustomisationManager ) , iPageStatus(EFalse)
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUi::ConstructL
// 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUi::ConstructL()
    {
    TRACE_ENTRY_POINT;

    CAknDialog::ConstructL( R_CALENDAR_SETTING_MENUBAR );

    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();

    // Set empty navi pane label
    iNaviContainer = static_cast<CAknNavigationControlContainer*>(
                                 sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    iNaviContainer->PushDefaultL();

    iGlobalData = CCalenGlobalData::InstanceL();

    // Set title text
    CAknTitlePane* tp = static_cast<CAknTitlePane*>(
                                 sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    HBufC* titleText = StringLoader::LoadLC( R_CALEN_SETTING_FORM_TITLE, iCoeEnv );
    tp->SetTextL( *titleText );
    CleanupStack::PopAndDestroy( titleText );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUi::~CCalenSettingsUi
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenSettingsUi::~CCalenSettingsUi()
    {
    TRACE_ENTRY_POINT;

    if( iNaviContainer )
        {
        // Remove navi pane used by settings view
        iNaviContainer->Pop(); 
        // iNaviContainer is not owned
        iNaviContainer = NULL; 
        }
    delete iContainer;
    delete iListBox;
    delete iBgContext;

    if( iGlobalData )
        {
        iGlobalData->Release();
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUi::OfferKeyEventL
// Handles key presses.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TKeyResponse CCalenSettingsUi::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                                    TEventCode aType )
    {
    TRACE_ENTRY_POINT;

    if( aKeyEvent.iCode == EKeyNo )
        {
        // Red cancel button.
        TryExitL( EKeyNo );

        TRACE_EXIT_POINT;
        
        // Chain this one up to the main app so it closes calendar app.
        return EKeyWasNotConsumed; 
        }
    else if( aKeyEvent.iCode == EKeyEscape )
        {
        // For exiting from FSW
        TryExitL( EAknSoftkeyBack );

        TRACE_EXIT_POINT;
        return EKeyWasNotConsumed;
        }
    else if(aKeyEvent.iScanCode == EStdKeyYes) //Process Send key
        {
        TRACE_EXIT_POINT;
        return EKeyWasNotConsumed;
        }
    else
        {
        iListBox->OfferKeyEventL( aKeyEvent, aType );
        
        // Draw because pressing up/down means we have to redraw the backgrouns skin.
        DrawDeferred();

        TRACE_EXIT_POINT;
        
        // Don't let other key events chain up to the calendar app.
        return EKeyWasConsumed; 
        }
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUi::ProcessCommandL
// Processes commands.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUi::ProcessCommandL( TInt aCommandId )
    {
    TRACE_ENTRY_POINT;

    CAknDialog::ProcessCommandL( aCommandId );

    switch(aCommandId)
        {
        case ECalenCmdChange:
            if(!iPageStatus)
                {
                iContainer->HandleListBoxSelectionL( ECalenCmdChange ,iPageStatus );
                }
            break;
        case EAknCmdHelp:
            OnCmdHelpL();
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
// CCalenSettingsUi::OkToExitL
// Returns ETrue if the button given is allowed to close settings.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenSettingsUi::OkToExitL( TInt aButtonId )
    {
    TRACE_ENTRY_POINT;

    TBool retVal = EFalse;

    switch( aButtonId )
        {
        case EAknSoftkeyChange:
            if(!iPageStatus)
                {
                iContainer->HandleListBoxSelectionL( EAknSoftkeyEdit ,iPageStatus);
                }
            break;
        case EAknSoftkeyBack:
            if(!iPageStatus)
                {
                retVal = CAknDialog::OkToExitL( aButtonId );
                }
            break;
        default:
            retVal = CAknDialog::OkToExitL( aButtonId );
            break;
        }

    TRACE_EXIT_POINT;
    return retVal;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUi::HandlePointerEventL
// Handle pointer events
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUi::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	TRACE_ENTRY_POINT;
	   
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
            }
            break;
        case TPointerEvent::EButton1Up:
            {
            iListBox->HandlePointerEventL(aPointerEvent);
            }
            break;
        case TPointerEvent::EDrag:
            {
            iListBox->HandlePointerEventL(aPointerEvent);
            }
            break;
        default:
            break;
        }
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenSettingsUi::PreLayoutDynInitL
// Called before laying out control.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUi::PreLayoutDynInitL()
    {
    TRACE_ENTRY_POINT;

    // Get the background skin
    iBgContext = CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain,
                                    CEikonEnv::Static()->EikAppUi()->ClientRect(),
                                    ETrue );

    CAknSettingStyleListBox *lb;
    iListBox = lb = new( ELeave ) CAknSettingStyleListBox;
    iListBox->SetListBoxObserver( this );

    iContainer = CCalenSettingsUiContainer::NewL( this,
                                                  *lb,
                                                  *iGlobalData,
                                                  iCustomisationManager );

    iContainer->ConstructListBoxL();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSettingsUi::SizeChanged
// From CCoeControl. Called when the size changes.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSettingsUi::SizeChanged()
    {
    TRACE_ENTRY_POINT;

    CAknDialog::SizeChanged();

    TRect rect = Rect();

    if( iBgContext )
        {
        iBgContext->SetRect( rect );
        }

    if( iListBox )
        {
   		TRect parent;
   		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, parent); 
     	AknLayoutUtils::LayoutControl(iListBox, parent, AknLayout::list_gen_pane(0));  
     	
        TRect zerodRect( TPoint(0, 0), parent.Size() );
        iListBox->SetRect( zerodRect );
        }

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSettingsUi::GetHelpContext
// Gets help context
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSettingsUi::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    TRACE_ENTRY_POINT;

    aContext.iMajor = KUidCalendar;
    aContext.iContext = KCALE_HLP_SETTINGS;

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSettingsUi::Draw
// Draws the background skin
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSettingsUi::Draw( const TRect& aRect ) const
    {
    TRACE_ENTRY_POINT

    CWindowGc& gc = SystemGc();
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
    TRect zerodRect( TPoint(0, 0), mainPane.Size() );
    iBgContext->SetRect( zerodRect );
    iBgContext->SetParentPos( mainPane.iTl );
    AknsDrawUtils::Background( AknsUtils::SkinInstance(), iBgContext, this, gc, aRect );

    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenSettingsUi::HandleResourceChange
// Handles resource changes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenSettingsUi::HandleResourceChange( TInt aType )
    {
    TRACE_ENTRY_POINT;

    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPane );
        SetRect( mainPane );
        TRect zerodRect( TPoint( 0, 0 ), mainPane.Size() );
        iBgContext->SetRect( zerodRect );
        iBgContext->SetParentPos( mainPane.iTl );
        iListBox->SetRect( zerodRect );
        }

    CAknDialog::HandleResourceChange( aType );

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenSettingsUi::MopSupplyObject
// Supplies skinned mop object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TTypeUid::Ptr CCalenSettingsUi::MopSupplyObject( TTypeUid aId )
    {
    TRACE_ENTRY_POINT;

    if( aId.iUid == MAknsControlContext::ETypeId )
        {
        ASSERT( iBgContext );
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }

    TRACE_EXIT_POINT;
    return CAknDialog::MopSupplyObject( aId );
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUi::CountComponentControls
// Returns the number of controls contained in this control.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenSettingsUi::CountComponentControls() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return 1;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUi::ComponentControl
// Returns the control contained in this control.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCoeControl* CCalenSettingsUi::ComponentControl( TInt /*aIndex*/ ) const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUi::OnCmdHelpL
// Handles Help command.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUi::OnCmdHelpL()
    {
    TRACE_ENTRY_POINT;

    if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(),
                                            iAvkonAppUi->AppHelpContextL() );
        }   

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenSettingsUi::HandleListBoxEventL
// Control Passed to list box to handle the commands. 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenSettingsUi::HandleListBoxEventL(CEikListBox* /*aListBox*/, 
                                           TListBoxEvent aEventType)
    {
    TRACE_ENTRY_POINT;
    switch( aEventType )
        {
         //Single click integration
        case EEventItemSingleClicked:
        case EEventEnterKeyPressed:
            {
            if(!iPageStatus )
                {
                iContainer->HandleListBoxSelectionL( EAknSoftkeyEdit, iPageStatus );
                }
            break;
            }
    		default:
            break;
        }
    }
// End of file
