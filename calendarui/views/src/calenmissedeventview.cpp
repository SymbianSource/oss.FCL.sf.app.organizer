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
* Description:   Missed Event View implementation
*
*/



// system includes
#include <StringLoader.h>
#include <aknbutton.h>
#include <akntoolbar.h>
#include <finditem.hrh>
#include <finditemmenu.h>
#include <Calendar.rsg>
#include <missedalarm.h>
#include <missedalarmstore.h>
#include <calenagendautils.h>
#include <calentry.h>
#include <calentryview.h>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <CalenStatusPaneUtils.h>
#include <caleninstanceid.h>            // TCalenInstanceId

// user includes
#include "calenmissedeventview.h"
#include "calendarui_debug.h"
#include "calenmissedeventcontainer.h"
#include <calenview.h>
#include "CalenUid.h"
#include "calenlocationutil.h"
#include "calenentryutil.h"
#include "CleanupResetAndDestroy.h"

// Button position of the MSK CBA button
const TInt KSK2CBAPosition = 2;
_LIT8( KCommandMissedEventView,"MEV" );

// ----------------------------------------------------------------------------
// CCalenMissedEventView::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenMissedEventView* CCalenMissedEventView::NewL( MCalenServices& aServices )
	{
	TRACE_ENTRY_POINT;
	
	CCalenMissedEventView* self = new( ELeave )CCalenMissedEventView( aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventView::ConstructL
// Constructor
// ----------------------------------------------------------------------------
//
void CCalenMissedEventView::ConstructL()
	{
	TRACE_ENTRY_POINT;
	
	CommonConstructL( R_CALEN_MISSED_EVENT_VIEW );
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventView::CCalenMissedEventView
// C++ constructor.
// ----------------------------------------------------------------------------
//
CCalenMissedEventView::CCalenMissedEventView( MCalenServices& aServices )
    : CCalenNativeView( aServices ),
    iPopulationStep( ENothingDone )
    {
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
    }
 
// ----------------------------------------------------------------------------
// CCalenMissedEventView::~CCalenMissedEventView
// Destructor
// ----------------------------------------------------------------------------
//
CCalenMissedEventView::~CCalenMissedEventView()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventView::ActiveStepL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CCalenView::TNextPopulationStep CCalenMissedEventView::ActiveStepL()
    {
    TRACE_ENTRY_POINT;
	
	CCalenView::TNextPopulationStep nextStep = CCalenView::EDone;
	CCalenMissedEventContainer* cnt = static_cast<CCalenMissedEventContainer*>( iContainer );
	
    switch( iPopulationStep )
        {
        case ENothingDone:
        	{
			iPopulationStep = EBuildTextEditor;
			nextStep = CCalenView::EKeepGoing;
        	}
        	break;
        case EBuildTextEditor:
        	{
        	cnt->BuildTextEditorL();
 			iPopulationStep = ERequestInstanceView;       	
 			nextStep = CCalenView::EKeepGoing;
        	}
        	break;
        case ERequestInstanceView:
        	{
        	cnt->RequestInstanceViewL();
			iPopulationStep = EAddField; 
 			nextStep = CCalenView::EKeepGoing;			       	
        	}
        	break;
        case EAddField:
        	{
        	cnt->AddFieldsL();
			iPopulationStep = EPopulationDone;   
 			nextStep = CCalenView::EKeepGoing;			     	
        	}
        	break;
        case EPopulationDone: 
        default:    
        	{
        	cnt->CompletePopulationL();
        	RedrawStatusPaneL();
        	
        	//no tool bar in missed event view
        	MCalenToolbar* toolbar = iServices.ToolbarOrNull(); 
        	    if(toolbar && toolbar->IsVisible())
        	      {
        	      toolbar->SetToolbarVisibilityL(EFalse);  
        	      } 
        	nextStep = CCalenView::EDone;
        	}
        	break;
        }
    TRACE_EXIT_POINT;  
    return nextStep;  
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventView::CancelPopulation
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenMissedEventView::CancelPopulation()
    {
    TRACE_ENTRY_POINT;
    
    iPopulationStep = ENothingDone;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventView::LocalisedViewNameL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
const TDesC& CCalenMissedEventView::LocalisedViewNameL( CCalenView::TViewName /*aViewName*/ )
    {
    TRACE_ENTRY_POINT;
    _LIT(KEV,"Missed Event View");
    TRACE_EXIT_POINT;
    return KEV;
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventView::ViewIcon
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenMissedEventView::ViewIconL() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventView::DoActivateImplL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventView::DoActivateImplL( const TVwsViewId& aPrevViewId,
                                      TUid aCustomMessageId ,
                                      const TDesC8& aCustomMessage )
    {
    TRACE_ENTRY_POINT;
	
    iShowCloseButtonOnCba =
        ( aCustomMessageId == KUidCalenShowCloseCba ? ETrue : EFalse );
    
    // for handling missedeventview activation from sn/indicator
    // when only one missed alarm is there
    // issue command ECalenMissedEventViewFromIdle to set the context,
    // clear the missed alarm before launching missed event view
    if(aPrevViewId.iAppUid!=KUidCalendar)
        {
        // get the custom view message
        HBufC8* temp = aCustomMessage.AllocLC();
        TPtr8 des = temp->Des();
        des.UpperCase();
        
        if( des.Find( KCommandMissedEventView) != KErrNotFound )
            {
            iServices.IssueCommandL(ECalenMissedEventViewFromIdle);
            }
        CleanupStack::PopAndDestroy( temp );
        }
    
    UpdateCbaL();
    
   
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventView::DoDeactivateImpl
// Take action before deactivating the view
// ----------------------------------------------------------------------------
//
void CCalenMissedEventView::DoDeactivateImpl()
    {
    TRACE_ENTRY_POINT;
    
    iPreviousViewId.iViewUid = KNullUid;
    MCalenToolbar* toolbar = iServices.ToolbarOrNull(); 
    if(toolbar)
        {
        toolbar->SetToolbarVisibilityL(ETrue);  
        }

        
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenMissedEventView::OnLocaleChangedL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventView::OnLocaleChangedL(TInt /*aReason*/)  
    {
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenMissedEventView::RedrawStatusPaneL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventView::RedrawStatusPaneL()  
    {
    TRACE_ENTRY_POINT;

    CCalenMissedEventContainer* cnt = static_cast<CCalenMissedEventContainer*>( iContainer );
    HBufC* titleText = StringLoader::LoadLC( cnt->GetTitleTextId(), iCoeEnv );
    iSPUtils->UnderLineTitleText( EFalse );
    iSPUtils->SetTitleText( titleText );    // ownership passed
    CleanupStack::Pop( titleText );
    iSPUtils->RefreshStatusPane();

    TRACE_EXIT_POINT;
    }
  
// ----------------------------------------------------------------------------
// CCalenMissedEventView::HandleCommandL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventView::HandleCommandL(TInt aCommand)  
    {
    TRACE_ENTRY_POINT;

    CCalenMissedEventContainer* cnt = static_cast<CCalenMissedEventContainer*>( iContainer );
	switch(aCommand)
		{
		case ECalenCmdPromptThenEdit:
			{
			iServices.IssueCommandL(ECalenEditCurrentEntry);
			break;	
			}
        case ECalenCompleteTodo:
        case ECalenRestoreTodo:            
        	{        
            CCalenNativeView::HandleCommandL(aCommand);         
        	}
            break;
        case EAknSoftkeyClose:   
        case EAknSoftkeyBack:
			{
		
	        iServices.IssueNotificationL(ECalenNotifyMissedEventViewClosed);
			break;
			}
        case EAknSoftkeyExit: 
            {
                      
            CCalenNativeView::HandleCommandL(aCommand);
            }
            break;
        case ECalenCmdFindPhoneNum:
            {
            cnt->OnCmdFindPhoneNumL();
             }
             break;
        case ECalenCmdFindEmail:
            {
            cnt->OnCmdFindEmailL();
            }
            break;
        case ECalenCmdFindURL:
            {
            cnt->OnCmdFindUrlL();
            }
        default:
            if(cnt->GetFindItemMenu()->CommandIsValidL(aCommand))
                {
                cnt->GetFindItemMenu()->HandleItemFinderCommandL(aCommand);
                return;
                }
            CCalenNativeView::HandleCommandL(aCommand);
            break;
        }

    TRACE_EXIT_POINT;
    }  

// ----------------------------------------------------------------------------
// CCalenMissedEventView::DynInitMenuPaneL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenMissedEventView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
    TRACE_ENTRY_POINT;
    CCalenMissedEventContainer* cnt = static_cast<CCalenMissedEventContainer*>( iContainer );
    switch(aResourceId)
    	{
	    case R_CALEN_EVENT_VIEW_MENUPANE:
        case R_CALEN_EVENT_VIEW_LONGTAP_MENUPANE:
	    	{
	    	CCalenEntryUtil* eventViewData = cnt->GetEventViewData();
            if(eventViewData)
                {
                if(eventViewData->EntryType() == CCalEntry::ETodo)
                      {
                      if(eventViewData->Status() == CCalEntry::ETodoCompleted)
                          {
                          aMenuPane->DeleteMenuItem(ECalenCompleteTodo);
                          }
                      else
                          {
                          aMenuPane->DeleteMenuItem(ECalenRestoreTodo);
                          }
                      }             
                  else
                      {
                      aMenuPane->DeleteMenuItem(ECalenCompleteTodo);
                      aMenuPane->DeleteMenuItem(ECalenRestoreTodo);
                      }
                }
            
		  
		  	 
		  	 if(CCalenLocationUtil::IsMapProviderAvailableL())
            	{
	            if(cnt->IsEventHasMapLocationL() || cnt->IsEventHasNoLocationTextL())
		            {
		            aMenuPane->DeleteMenuItem( ECalenGetLocationAndReplace );
		            }
		        if(!cnt->IsEventHasMapLocationL())
			        {
			        aMenuPane->DeleteMenuItem( ECalenShowLocation );	
			        }	
            	}
            else // No map provider available, remove both option items
	            {
	            aMenuPane->DeleteMenuItem( ECalenGetLocationAndReplace );
	            aMenuPane->DeleteMenuItem( ECalenShowLocation );	
	            }
			//as no toolbar in missedeventview no need to handle thees commands
		  	aMenuPane->DeleteMenuItem( ECalenCmdPromptThenEdit );
		  	aMenuPane->DeleteMenuItem( ECalenDeleteCurrentEntry ); 
		  	aMenuPane->DeleteMenuItem( ECalenSend );
		    break;
		  	}
		 default:
		    {
		      cnt->GetFindItemMenu()->UpdateItemFinderMenuL(aResourceId,aMenuPane); 
		    }
			break;
		}
    
    TRACE_EXIT_POINT;	
	}
	
// ----------------------------------------------------------------------------
// CCalenMissedEventView::Id
// Returns unique view id
// ----------------------------------------------------------------------------
//
TUid CCalenMissedEventView::Id() const
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	
    return KUidCalenMissedEventView;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventView::CreateContainerImplL
// create container
// ----------------------------------------------------------------------------
//
CCalenContainer* CCalenMissedEventView::CreateContainerImplL() 
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	
    return new( ELeave )CCalenMissedEventContainer( this, iServices );
	}

// ---------------------------------------------------------
// CCalenMissedEventView::UpdateCbaL
// Set CBA button 
// ---------------------------------------------------------
//
void CCalenMissedEventView::UpdateCbaL()
    {
    TRACE_ENTRY_POINT;  
    
    CEikButtonGroupContainer*  cba = Cba();
    
    if(iShowCloseButtonOnCba)
        {
        cba->SetCommandL( KSK2CBAPosition, R_CALEN_CLOSE_CBA_BUTTON );
        }
    else
        {
        cba->SetCommandL( KSK2CBAPosition, R_CALEN_BACK_CBA_BUTTON);
        }
    
    cba->DrawNow();
       
    TRACE_EXIT_POINT;
    }
// ----------------------------------------------------------------------------
// CCalenMissedEventView::ClearViewSpecificDataL
// Clear view specific data
// ----------------------------------------------------------------------------
//
void CCalenMissedEventView::ClearViewSpecificDataL()
	{
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventView::CyclePosition
// Returns view cycle position
// ----------------------------------------------------------------------------
//
CCalenView::TCyclePosition CCalenMissedEventView::CyclePosition() const
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	
    return ENoCyclePosition;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventView::AddToolbarButtonsL
// Adds the necessary toolbar buttons on the existing toolbar for event viewer
// ----------------------------------------------------------------------------
//
void CCalenMissedEventView::AddToolbarButtonsL()
	{
	TRACE_ENTRY_POINT;
	
	// Get the existing toolbar from MCalenservices
	MCalenToolbar* toolbarImpl = iServices.ToolbarOrNull();
	
	if(toolbarImpl)  // If toolbar exists
		{
		CAknToolbar& toolbar = toolbarImpl->Toolbar();
		
		// Create the new buttons for event viewer toolbar
		// If layout is mirrored the button order is reversed.
	    // For Non mirrored layouts each button is appended to the toolbar
	    // For mirrored layouts each button is inserted at index 0.
	    TBool mirrored( AknLayoutUtils::LayoutMirrored() );
	    
	    CAknButton* button = NULL;
	    CGulIcon* icon = NULL;
	    
	    // First button: Send
	    icon = iServices.GetIconL( MCalenServices::ECalenViewerSendIcon );
	    // put icon onto cleanup stack before its ownership is transferred to CAknButton
	    button = CreateButtonL( icon, _L(""), R_CALEN_TB_SEND, toolbar );
	    if( mirrored )
	        {
	        // Insert the button at index 0
	        toolbar.AddItemL( button, EAknCtButton, ECalenSend, 0, 0 );
	        }
	    else
	        {
	        // Append the button
	        toolbar.AddItemL( button, EAknCtButton, ECalenSend, 0, 0 );
	        }
	    
	    // Second button: Edit
	    icon = iServices.GetIconL( MCalenServices::ECalenViewerEditIcon );
	    // put icon onto cleanup stack before its ownership is transferred to CAknButton
	    button = CreateButtonL( icon, _L(""), R_CALEN_TB_EDIT, toolbar );
	    if( mirrored )
	        {
	        // Insert the button at index 0
	        toolbar.AddItemL( button, EAknCtButton, ECalenEditCurrentEntry, 0, 0 );
	        }
	    else
	        {
	        // Append the button
	        toolbar.AddItemL( button, EAknCtButton, ECalenEditCurrentEntry, 0, 1 );
	        }
	    
	    // Third button: Delete
	    icon = iServices.GetIconL( MCalenServices::ECalenViewerDeleteIcon );
	    // put icon onto cleanup stack before its ownership is transferred to CAknButton
	    button = CreateButtonL( icon, _L(""), R_CALEN_TB_DELETE, toolbar );
	    if( mirrored )
	        {
	        // Insert the button at index 0
	        toolbar.AddItemL( button, EAknCtButton, ECalenDeleteCurrentEntry, 0, 0 );
	        }
	    else
	        {
	        // Append the button
	        toolbar.AddItemL( button, EAknCtButton, ECalenDeleteCurrentEntry, 0, 2 );
	        }
		}
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenMissedEventView::RemoveToolbarButtonsL
// Removes the event viewer toolbar buttons and unhides the view cycling buttons
// ----------------------------------------------------------------------------
//
void CCalenMissedEventView::RemoveToolbarButtonsL()
	{
	TRACE_ENTRY_POINT;
	
	MCalenToolbar* toolbarImpl = iServices.ToolbarOrNull();
	if(toolbarImpl) // If toolbar exists
		{
		CAknToolbar& toolbar = toolbarImpl->Toolbar();
		if(&toolbar)
		    {
		    // Remove the viewer toolbar buttons
		    toolbar.RemoveItem(ECalenDeleteCurrentEntry); // Delete button
		    toolbar.RemoveItem(ECalenEditCurrentEntry);  // Edit button
		    toolbar.RemoveItem(ECalenSend);  // Send button
		    }
		}
	
	TRACE_EXIT_POINT;
	}
	
// ----------------------------------------------------------------------------
// CCalenMissedEventView::CreateButtonL
// Create calendar toolbar buttons
// ----------------------------------------------------------------------------
CAknButton* CCalenMissedEventView::CreateButtonL( CGulIcon* aIcon, 
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
    button = CAknButton::NewL( aIcon, NULL, NULL, NULL,
                                            aText, tooltipText->Des(), flags, 0 );
    CleanupStack::PopAndDestroy( tooltipText );

    button->SetIconScaleMode( EAspectRatioNotPreserved );
    button->SetFocusing( EFalse );
    button->SetBackground( &aToolbar );

    TRACE_EXIT_POINT;
    return button;
    }    

//end of file
