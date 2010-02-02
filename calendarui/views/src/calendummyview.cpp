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
* Description:  Missed alarms view description   
*
*/


// System Includes
#include <Calendar.rsg>
#include <StringLoader.h>
#include <CalenStatusPaneUtils.h>
// User Includes
#include "calendarui_debug.h"
#include "calendummyview.h"
#include "calendummyviewcontainer.h"
#include "CalenUid.h"

// -----------------------------------------------------------------------------
// CCalenDummyView::NewLC
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenDummyView* CCalenDummyView::NewL( MCalenServices& aServices )
    {
    TRACE_ENTRY_POINT;
		
	CCalenDummyView* self = new( ELeave )CCalenDummyView(aServices);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
    }
    
// -----------------------------------------------------------------------------
// CCalenDummyView::ConstructL
// Performs the second phase construction.
// -----------------------------------------------------------------------------
//
void CCalenDummyView::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    CommonConstructL( R_CALEN_DUMMY_VIEW );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyView::CCalenEventView
// C++ constructor.
// ----------------------------------------------------------------------------
//
CCalenDummyView::CCalenDummyView( MCalenServices& aServices )
    : CCalenNativeView( aServices ),
    iPopulationStep( ENothingDone )
    {
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
    }
 
// ----------------------------------------------------------------------------
// CCalenDummyView::~CCalenDummyView
// Destructor
// ----------------------------------------------------------------------------
//
CCalenDummyView::~CCalenDummyView()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenDummyView::ActiveStepL
// From CCalenView:State machine for view population
// ----------------------------------------------------------------------------
//
CCalenView::TNextPopulationStep CCalenDummyView::ActiveStepL()
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
        	nextStep = CCalenView::EDone;
        	}
        	break;
        default:
            break;
        }
    TRACE_EXIT_POINT;  
    return nextStep;  
    }

// ----------------------------------------------------------------------------
// CCalenDummyView::CancelPopulation
// From CCalenView:Cencel view population
// ----------------------------------------------------------------------------
//
void CCalenDummyView::CancelPopulation()
    {
    TRACE_ENTRY_POINT;
    
    iPopulationStep = ENothingDone;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyView::LocalisedViewNameL
// From CCalenView:Localised view name
// ----------------------------------------------------------------------------
//
const TDesC& CCalenDummyView::LocalisedViewNameL( CCalenView::TViewName aViewName )
    {
    TRACE_ENTRY_POINT;
	
    HBufC* ret = NULL;

    switch ( aViewName )
        {
        case CCalenView::EMenuName:
            if ( !iMenuName )
                {
                iMenuName = StringLoader::LoadL( R_QTN_CALE_TITLE_CALENDAR, iCoeEnv );
                }
            ret = iMenuName;
            break;
        case CCalenView::ESettingsName:
            if ( !iSettingsName )
                {
                iSettingsName = StringLoader::LoadL( R_QTN_CALE_TITLE_CALENDAR,
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
// CCalenDummyView::ViewIcon
// From CCalenView: Returns view specific icon
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenDummyView::ViewIconL() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCalenDummyView::HandleCommandL
// Handles the commands that are passed from softkey generated commands and menus.
// -----------------------------------------------------------------------------
//
void CCalenDummyView::HandleCommandL(TInt aCommand)
	{
	TRACE_ENTRY_POINT;
	
	switch(aCommand)
		{
        default:
            CCalenNativeView::HandleCommandL(aCommand);
            break;
        	}
 	
    TRACE_EXIT_POINT;	
	}        

// ----------------------------------------------------------------------------
// CCalenDummyView::DoActivateImplL
// Take action before activating the view
// ----------------------------------------------------------------------------
//
void CCalenDummyView::DoActivateImplL( const TVwsViewId& /*aPrevViewId*/,
                                      TUid /*aCustomMessageId */,
                                      const TDesC8& /*aCustomMessage*/ )
    {
    TRACE_ENTRY_POINT;
	
    RedrawStatusPaneL();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyView::DoDeactivateImpl
// Take action before view deactivation
// ----------------------------------------------------------------------------
//
void CCalenDummyView::DoDeactivateImpl()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyView::OnLocaleChangedL
// Handle locale changes
// ----------------------------------------------------------------------------
//
void CCalenDummyView::OnLocaleChangedL(TInt aReason)  
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
// CCalenDummyView::RedrawStatusPaneL
// Redraw status pane
// ----------------------------------------------------------------------------
//
void CCalenDummyView::RedrawStatusPaneL()  
    {
    TRACE_ENTRY_POINT;
	
    HBufC* titleText = StringLoader::LoadLC( R_QTN_CALE_TITLE_CALENDAR, iCoeEnv );
    iSPUtils->UnderLineTitleText( EFalse );
    iSPUtils->SetTitleText( _L(" ").Alloc()/*titleText*/ );    // ownership passed
    CleanupStack::Pop( titleText );
    iSPUtils->RefreshStatusPane();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenDummyView::ClearViewSpecificDataL
// Clear view specific data
// ----------------------------------------------------------------------------
//
void CCalenDummyView::ClearViewSpecificDataL()
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenDummyView::CyclePosition
// Returns view's cycle position
// ----------------------------------------------------------------------------
//
CCalenView::TCyclePosition CCalenDummyView::CyclePosition() const
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	

    return ENoCyclePosition;
	}

// -----------------------------------------------------------------------------
// CCalenDummyView::Id
// Returns the unique id of the view.
// -----------------------------------------------------------------------------
//
 TUid CCalenDummyView::Id() const
 	{
 	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;	
	
 	return KUidCalenDummyView;
 	}	

// ----------------------------------------------------------------------------
// CCalenDummyView::CreateContainerImplL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContainer* CCalenDummyView::CreateContainerImplL() 
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	
    return new( ELeave )CCalenDummyViewContainer( this, 
                                              iServices );
	}

// ----------------------------------------------------------------------------
// CCalenDummyView::Container
// Return the container pointer.
// ----------------------------------------------------------------------------
//
CCalenDummyViewContainer* CCalenDummyView::Container()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	
	return static_cast<CCalenDummyViewContainer*>( iContainer );
	}

// ----------------------------------------------------------------------------
// CCalenDummyViewContainer::UpdateSizeL
// Do layout changes after calendar settings are
// changed.
// Implementation for month view without week numbers.
// ----------------------------------------------------------------------------
//
void CCalenDummyViewContainer::UpdateSizeL()
    {
    TRACE_ENTRY_POINT;


    TRACE_EXIT_POINT;
    }

// end of file	
