/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Missed alarms view description   
*
*/



// System Includes
#include <aknlists.h>
#include <StringLoader.h>
#include <eikmenup.h>
#include <hlplch.h>
#include <featmgr.h>
#include <aknbutton.h>
#include <akntoolbar.h>
#include <Calendar.rsg>
#include <CalenDefaultEditorsData.rsg>
#include <calencommands.hrh>            // Calendar commands
#include <CalenStatusPaneUtils.h>
#include <calennotificationhandler.h>
#include <calencontext.h>

// User Includes
#include "calendarui_debug.h"
#include "calenmissedalarmsview.h"
#include "calenmissedalarmscontainer.h"
#include "calencontainer.h"
#include "calentitlepane.h"
#include "CalenUid.h"

_LIT8( KCommandMissedAlarmsView,"MAV" );
const TInt KSK2CBAPosition = 2;

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsView::NewLC
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenMissedAlarmsView* CCalenMissedAlarmsView::NewL( MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;
		
	CCalenMissedAlarmsView* self = new( ELeave )CCalenMissedAlarmsView(aServices);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
    }
    
// -----------------------------------------------------------------------------
// CCalenMissedAlarmsView::ConstructL
// Performs the second phase construction.
// -----------------------------------------------------------------------------
//
void CCalenMissedAlarmsView::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    CommonConstructL( R_MISSED_ALARMS_VIEW );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::CCalenEventView
// C++ constructor.
// ----------------------------------------------------------------------------
//
CCalenMissedAlarmsView::CCalenMissedAlarmsView( MCalenServices& aServices )
    : CCalenNativeView( aServices ),
    iPopulationStep( ENothingDone )
    {
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
    }
 
// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::~CCalenMissedAlarmsView
// Destructor
// ----------------------------------------------------------------------------
//
CCalenMissedAlarmsView::~CCalenMissedAlarmsView()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::ActiveStepL
// From CCalenView:State machine for view population
// ----------------------------------------------------------------------------
//
CCalenView::TNextPopulationStep CCalenMissedAlarmsView::ActiveStepL()
    {
    TRACE_ENTRY_POINT;
	
	CCalenView::TNextPopulationStep nextStep = CCalenView::EDone;
		
     switch( iPopulationStep )
        {
        case ENothingDone:
        	{
        	iPopulationStep = EPopulationDone;   
 			nextStep = CCalenView::EKeepGoing;
			}
        	break;
        case EPopulationDone:
	       	{
	       	// update missed alarm list
	       	Container()->UpdateMissedAlarmsArrayL();

	       	// populate with missed alarms
	       	Container()->PopulateWithMissedAlarmsL();
       	    
       	    // complete population
       	    Container()->CompletePopulationL();
       	    RedrawStatusPaneL();
       	    UpdateCbaL();
       	    
       	    // dim clear and clear all buttons
       	    if(Container()->MissedAlarmsCount()==0)
       	        {
       	        DimClearAndClearAllButtons();
       	        }

        	nextStep = CCalenView::EDone;
        	}
        	break;	
        }
    TRACE_EXIT_POINT;  
    return nextStep;  
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::CancelPopulation
// From CCalenView:Cencel view population
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsView::CancelPopulation()
    {
    TRACE_ENTRY_POINT;
    
    iPopulationStep = ENothingDone;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::LocalisedViewNameL
// From CCalenView:Localised view name
// ----------------------------------------------------------------------------
//
const TDesC& CCalenMissedAlarmsView::LocalisedViewNameL( CCalenView::TViewName aViewName )
    {
    TRACE_ENTRY_POINT;
	
    HBufC* ret = NULL;

    switch ( aViewName )
        {
        case CCalenView::EMenuName:
            if ( !iMenuName )
                {
                iMenuName = StringLoader::LoadL( R_CALE_TITLE_MISSED_ALARMS, iCoeEnv );
                }
            ret = iMenuName;
            break;
        case CCalenView::ESettingsName:
            if ( !iSettingsName )
                {
                iSettingsName = StringLoader::LoadL( R_CALE_TITLE_MISSED_ALARMS,
                                                     iCoeEnv );
                }
            ret = iSettingsName;
            break;
        default:
            ASSERT( EFalse );
            break;
        }

    TRACE_EXIT_POINT;
    return *ret;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::ViewIcon
// From CCalenView: Returns view specific icon
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenMissedAlarmsView::ViewIconL() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    // using clear all missed alarms icon as view icon
    return iServices.GetIconL( MCalenServices::ECalenClearAllMissedAlarms );
    }

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsView::HandleCommandL
// Handles the commands that are passed from softkey generated commands and menus.
// -----------------------------------------------------------------------------
//
void CCalenMissedAlarmsView::HandleCommandL(TInt aCommand)
	{
	TRACE_ENTRY_POINT;
	
	switch(aCommand)
		{
		case ECalenMissedEventView:
	    case EAknSoftkeyOpen:
			{
			iHighlightedRowNumber = Container()->CurrentItemIndex();
			iServices.IssueCommandL(ECalenMissedEventView);
			break;
			}
		case ECalenCmdClear:
		    {
		    iHighlightedRowNumber = Container()->CurrentItemIndex();
		    iServices.IssueCommandL(aCommand);
		    break;
		    }
        case ECalenCmdClearAll:
        case ECalenCmdGotoCalendar:            
            {
            iHighlightedRowNumber = 0;
            iServices.IssueCommandL(aCommand);
            }
            break;
        case EAknSoftkeyBack:
        case EAknSoftkeyClose:
            {
            iHighlightedRowNumber = 0;
            iServices.IssueNotificationL(ECalenNotifyMissedAlarmViewClosed);
            }
            break;
        default:
            CCalenNativeView::HandleCommandL(aCommand);
            break;
        	}
 	
    TRACE_EXIT_POINT;	
	}        

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::DoActivateImplL
// Take action before activating the view
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsView::DoActivateImplL( const TVwsViewId& aPrevViewId,
                                      TUid aCustomMessageId ,
                                      const TDesC8& aCustomMessage )
    {
    TRACE_ENTRY_POINT;
	
    // if missed alarms view launched from soft notification/indicator
    if( (aPrevViewId.iAppUid != KUidCalendar )||
        (aCustomMessageId == KUidCalenShowCloseCba ) )
        {
        iShowCloseButtonOnCba = ETrue;
        
        // get the custom view message
        HBufC8* temp = aCustomMessage.AllocLC();
        TPtr8 des = temp->Des();
        des.UpperCase();
        
        if( des.Find( KCommandMissedAlarmsView) != KErrNotFound )
            {
            iServices.IssueCommandL(ECalenMissedAlarmsViewFromIdle);
            }
        CleanupStack::PopAndDestroy( temp );
        }
    else
        {
        iShowCloseButtonOnCba = EFalse; 
        }
    
    UpdateCbaL();
    
    // Draw Missed Alarm toolbar by adding clear, clearall and gotocalendar buttons
    AddToolbarButtonsL();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::DoDeactivateImpl
// Take action before view deactivation
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsView::DoDeactivateImpl()
    {
    TRACE_ENTRY_POINT;
    
    // Remove the toolbar buttons from Missed Alarm View and
    // enable native view toolbar buttons
    TRAP_IGNORE(RemoveToolbarButtonsL());
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::OnLocaleChangedL
// Handle locale changes
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsView::OnLocaleChangedL(TInt aReason)  
    {
    TRACE_ENTRY_POINT;
    
    if(iContainer)
        {
        if(IsContainerFocused())
            {
            if (aReason & EChangesLocale)
                {
                BeginRepopulationL();
                }
            else
                {
                RedrawStatusPaneL();
                }
            }
        else
            {
            iLocChangeReason = EChangesLocale;
            }
        }
        
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::RedrawStatusPaneL
// Redraw status pane
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsView::RedrawStatusPaneL()  
    {
    TRACE_ENTRY_POINT;
	
    HBufC* titleText = StringLoader::LoadLC( R_CALE_TITLE_MISSED_ALARMS, iCoeEnv );
    iSPUtils->UnderLineTitleText( EFalse );
    iSPUtils->SetTitleText( titleText );    // ownership passed
    CleanupStack::Pop( titleText );
    iSPUtils->RefreshStatusPane();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::ClearViewSpecificDataL
// Clear view specific data
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsView::ClearViewSpecificDataL()
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::CyclePosition
// Returns view's cycle position
// ----------------------------------------------------------------------------
//
CCalenView::TCyclePosition CCalenMissedAlarmsView::CyclePosition() const
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	

    return ENoCyclePosition;
	}

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsView::Id
// Returns the unique id of the view.
// -----------------------------------------------------------------------------
//
 TUid CCalenMissedAlarmsView::Id() const
 	{
 	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;	
	
 	return KUidCalenMissedAlarmsView;
 	}	

 // -----------------------------------------------------------------------------
 // CCalenMissedAlarmsView::DynInitMenuPaneL
 // Called by the framework for dynamically initializing the menu pane.
 // -----------------------------------------------------------------------------
 // 
void CCalenMissedAlarmsView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
	TRACE_ENTRY_POINT;
	
	switch(aResourceId)
		{
		case R_MISSED_ALARMS_MENUPANE:
			{
			if(Container()->MissedAlarmsCount() == 0)
				{
				aMenuPane->DeleteMenuItem(ECalenMissedEventView);
				aMenuPane->DeleteMenuItem(ECalenCmdClear);
				aMenuPane->DeleteMenuItem(ECalenCmdClearAll);	
				}
			break;
			}
		default: 
			break;
		}
			
    TRACE_EXIT_POINT;	
	}

// -----------------------------------------------------------------------------
// CCalenMissedAlarmsView::UpdateCbaL
// Updates the cba for Missed Alarms View
// -----------------------------------------------------------------------------
//
void CCalenMissedAlarmsView::UpdateCbaL()
	{
	TRACE_ENTRY_POINT;
	
	CEikButtonGroupContainer*  cba = Cba();
    cba->SetCommandSetL(R_MISSED_ALARMS_CBA);
    
    if(iShowCloseButtonOnCba)
        {
        cba->SetCommandL(KSK2CBAPosition,R_CALEN_CLOSE_CBA_BUTTON);
        }

    if(Container()->MissedAlarmsCount())
    	{
    	// msk "open" is visible if there are missed alarms
    	cba->MakeCommandVisible( EAknSoftkeyOpen, ETrue );
    	}
    else
    	{
    	// msk empty if there are no missed alarms
    	cba->MakeCommandVisible( EAknSoftkeyOpen, EFalse );
    	}
    
    cba->DrawNow();	
    
	TRACE_EXIT_POINT;    	
	}
	
// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::CreateContainerImplL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContainer* CCalenMissedAlarmsView::CreateContainerImplL() 
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	
    return new( ELeave )CCalenMissedAlarmsContainer( this, 
                                              iServices, iHighlightedRowNumber );
	}

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::Container
// Return the container pointer.
// ----------------------------------------------------------------------------
//
CCalenMissedAlarmsContainer* CCalenMissedAlarmsView::Container()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	
	return static_cast<CCalenMissedAlarmsContainer*>( iContainer );
	}

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::AddToolbarButtonsL
// Adds missed alarms view's toolbar buttons 
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsView::AddToolbarButtonsL()
    {
    TRACE_ENTRY_POINT;
    
    // Get the existing toolbar from MCalenservices
    MCalenToolbar* toolbarImpl = iServices.ToolbarOrNull();
    
    if(toolbarImpl)  // If toolbar exists
        {
        CAknToolbar& toolbar = toolbarImpl->Toolbar();
        
        // Create the new buttons for Missed alarm view toolbar
        // If layout is mirrored the button order is reversed.
        // For Non mirrored layouts each button is appended to the toolbar
        // For mirrored layouts each button is inserted at index 0.
        TBool mirrored( AknLayoutUtils::LayoutMirrored() );
        
        CAknButton* button = NULL;
        CGulIcon* icon = NULL;

        // First button: Clear
        icon = iServices.GetIconL( MCalenServices::ECalenClearMissedAlarms );
        // put icon onto cleanup stack before its 
        // ownership is transferred to CAknButton
        button = CreateButtonL( icon, _L(""), R_CALE_MAV_TB_CLEAR, toolbar );
        if( mirrored )
            {
            // Insert the button at index 0
            toolbar.AddItemL( button, EAknCtButton, ECalenCmdClear, 0, 0 );
            }
        else
            {
            // Append the button
            toolbar.AddItemL( button, EAknCtButton, ECalenCmdClear, 0, 0 );
            }
        
        // Second button: Clear All
        icon = iServices.GetIconL( MCalenServices::ECalenClearAllMissedAlarms );
        // put icon onto cleanup stack before its
        // ownership is transferred to CAknButton
        button = CreateButtonL( icon, _L(""), R_CALE_MAV_TB_CLEARALL, toolbar );
        if( mirrored )
            {
            // Insert the button at index 0
            toolbar.AddItemL( button, EAknCtButton, ECalenCmdClearAll, 0, 0 );
            }
        else
            {
            // Append the button
            toolbar.AddItemL( button, EAknCtButton, ECalenCmdClearAll, 0, 1 );
            }
        
        // Third button: Go To Calendar
        icon = iServices.GetIconL( MCalenServices::ECalenDayViewIcon );
        // put icon onto cleanup stack before its
        // ownership is transferred to CAknButton
        button = CreateButtonL( icon, _L(""), R_CALE_MAV_TB_GOTO_CALE, toolbar );
        if( mirrored )
            {
            // Insert the button at index 0
            toolbar.AddItemL( button, EAknCtButton, ECalenCmdGotoCalendar, 0, 0 );
            }
        else
            {
            // Append the button
            toolbar.AddItemL( button, EAknCtButton, ECalenCmdGotoCalendar, 0, 2 );
            }
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::RemoveToolbarButtonsL
// Removes the missed alarm view's toolbar buttons 
// ----------------------------------------------------------------------------
//
void CCalenMissedAlarmsView::RemoveToolbarButtonsL()
    {
    TRACE_ENTRY_POINT;
    
    MCalenToolbar* toolbarImpl = iServices.ToolbarOrNull();
    if(toolbarImpl) // If toolbar exists
        {
        CAknToolbar& toolbar = toolbarImpl->Toolbar();
        if(&toolbar)
            {
            // Remove the Missed alarm view's toolbar buttons
            toolbar.RemoveItem(ECalenCmdClear); // Clear button
            toolbar.RemoveItem(ECalenCmdClearAll);  // Clear All button
            toolbar.RemoveItem(ECalenCmdGotoCalendar);  // Go To calendar button
            }
        }
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::CreateButtonL
// Create missed alarms view toolbar buttons
// ----------------------------------------------------------------------------
CAknButton* CCalenMissedAlarmsView::CreateButtonL( CGulIcon* aIcon, 
                                                   const TDesC& aText,
                                                   TInt aTooltipID,
                                                   CAknToolbar& aToolbar )
    {
    TRACE_ENTRY_POINT;

    TInt flags = 0;
    CAknButton* button = NULL;
    
    CleanupStack::PushL( aIcon );
    HBufC* tooltipText = StringLoader::LoadLC( aTooltipID );
    CleanupStack::Pop( tooltipText );
    // put icon onto cleanup stack before its ownership is transferred to CAknButton
    CleanupStack::Pop( aIcon );
    CleanupStack::PushL( tooltipText );
    button = CAknButton::NewL( aIcon, NULL, NULL, NULL, aText, 
                               tooltipText->Des(), flags, 0 );
    CleanupStack::PopAndDestroy( tooltipText );

    button->SetIconScaleMode( EAspectRatioNotPreserved );
    button->SetFocusing( EFalse );
    button->SetBackground( &aToolbar );

    TRACE_EXIT_POINT;
    return button;
    }    

// ----------------------------------------------------------------------------
// CCalenMissedAlarmsView::DimClearAndClearAllButtons
// Dim clear and clear all toolbar buttons when there are no
// missed alarm entries to clear
// ----------------------------------------------------------------------------
void CCalenMissedAlarmsView::DimClearAndClearAllButtons()
    {
    TRACE_ENTRY_POINT;
    
    MCalenToolbar* toolbarImpl = iServices.ToolbarOrNull();
    if(toolbarImpl) 
        {
        CAknToolbar& toolbar = toolbarImpl->Toolbar();

        // dim clear and clear all toolbar buttons
        toolbar.SetItemDimmed(ECalenCmdClear,ETrue,ETrue);
        toolbar.SetItemDimmed(ECalenCmdClearAll,ETrue,ETrue);
        }

    TRACE_EXIT_POINT;
    }

// end of file	
