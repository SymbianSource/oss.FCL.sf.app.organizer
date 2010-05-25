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
* Description:   Event View implementation
*
*/



// system includes
#include <StringLoader.h>
#include <Calendar.rsg>
#include <aknbutton.h>
#include <akntoolbar.h>
#include <calencommands.hrh>            // Calendar commands
#include <calencontext.h>
#include <CalenStatusPaneUtils.h>
#include <calcalendarinfo.h>
#include <caleninstanceid.h>

// user includes
#include "caleneventview.h"
#include "calendarui_debug.h"
#include "caleneventviewcontainer.h"
#include <calenview.h>
#include "CalenUid.h"
#include "calenlocationutil.h"
#include "calenentryutil.h"
#include "calendar.hrh"
#include <finditem.hrh>
#include <finditemmenu.h>
#include "calenattachmentmodel.h" 

// Button position of the MSK CBA button
const TInt KSK2CBAPosition = 2;

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CCalenEventView::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenEventView* CCalenEventView::NewL( MCalenServices& aServices )
	{
	TRACE_ENTRY_POINT;
	
	CCalenEventView* self = new( ELeave )CCalenEventView( aServices );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
	}

// ----------------------------------------------------------------------------
// CCalenEventView::ConstructL
// Constructor
// ----------------------------------------------------------------------------
//
void CCalenEventView::ConstructL()
	{
	TRACE_ENTRY_POINT;
	
	CommonConstructL( R_CALEN_EVENT_VIEW );
	
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenEventView::CCalenEventView
// C++ constructor.
// ----------------------------------------------------------------------------
//
CCalenEventView::CCalenEventView( MCalenServices& aServices )
    : CCalenNativeView( aServices ),
    iPopulationStep( ENothingDone )
    {
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
    }
 
// ----------------------------------------------------------------------------
// CCalenEventView::~CCalenEventView
// Destructor
// ----------------------------------------------------------------------------
//
CCalenEventView::~CCalenEventView()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenEventView::ActiveStepL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CCalenView::TNextPopulationStep CCalenEventView::ActiveStepL()
    {
    TRACE_ENTRY_POINT;
	
	CCalenView::TNextPopulationStep nextStep = CCalenView::EDone;
	CCalenEventViewContainer* cnt = static_cast<CCalenEventViewContainer*>( iContainer );
	
    switch( iPopulationStep )
        {
        case ENothingDone:
        	{
			iPopulationStep = ERequestInstanceView;
			nextStep = CCalenView::EKeepGoing;
        	}
        	break;
        case ERequestInstanceView:
            {
            TInt err = cnt->RequestInstanceViewL();
            if(err != KErrNotFound)
                {
				iPopulationStep = EBuildTextEditor;
            	nextStep = CCalenView::EKeepGoing;                
                }
			else
				{
				// the intended cal entry is not found in the database.
                // it has been deleted by some other app or by synching from server.
				nextStep = CCalenView::EDone;
				}

            }
            break; 	
        case EBuildTextEditor:
            {
            cnt->BuildTextEditorL();
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
        	UpdateToolbarButtonsL();
        	nextStep = CCalenView::EDone;
        	}
        	break;
        }
    TRACE_EXIT_POINT;  
    return nextStep;  
    }

// ----------------------------------------------------------------------------
// CCalenEventView::CancelPopulation
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenEventView::CancelPopulation()
    {
    TRACE_ENTRY_POINT;
    
    iPopulationStep = ENothingDone;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEventView::LocalisedViewNameL
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
const TDesC& CCalenEventView::LocalisedViewNameL( CCalenView::TViewName /*aViewName*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_ENTRY_POINT;
    
    _LIT(KEV,"Event View");
    //HBufC* ret = NULL;

   /* switch ( aViewName )
        {
        case CCalenView::EMenuName:
            if ( !iMenuName )
                {
                iMenuName = StringLoader::LoadL( R_CALEN_QTN_EVENT_VIEW, iCoeEnv );
                }
            ret = iMenuName;
            break;
        case CCalenView::ESettingsName:
            if ( !iSettingsName )
                {
                iSettingsName = StringLoader::LoadL( R_CALEN_QTN_EVENT_VIEW,
                                                     iCoeEnv );
                }
            ret = iSettingsName;
            break;
        default:
            ASSERT( EFalse );
            break;
        }*/
    TRACE_EXIT_POINT;
    return KEV;
    }

// ----------------------------------------------------------------------------
// CCalenEventView::ViewIcon
// From CCalenView
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenEventView::ViewIconL() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }


// ----------------------------------------------------------------------------
// CCalenEventView::DoActivateImplL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventView::DoActivateImplL( const TVwsViewId& aPrevViewId,
                                      TUid aCustomMessageId ,
                                      const TDesC8& /*aCustomMessage */)
    {
    TRACE_ENTRY_POINT;
	
    
    if(aCustomMessageId == KUidCalenShowAlarmCba)
        {
        iEventViewCBA = EShowAlarm;
        }
	else if(aCustomMessageId == KUidCalenShowAlarmStopOnlyCba)
		{		    	
		iEventViewCBA =	EShowAlarmStopOnly;
		}
	else if(aCustomMessageId == KUidCalenShowCloseCba)
		{
		iEventViewCBA = EShowClose;	
		}
	else
		{
		if( aPrevViewId.iAppUid == KUidCalendar && aPrevViewId.iViewUid != KNullUid)    // switch from internal view
	        {
	        //Updating RSK CBA button of Event view depending up on from where it is activated
	       	iEventViewCBA = EShowBack;	        
		    }
		}             

    UpdateCbaL();
    
    // Draw event viewer toolbar by adding Edit, Delete and Send buttons
	AddToolbarButtonsL();
	
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEventView::DoDeactivateImpl
// ----------------------------------------------------------------------------
//
void CCalenEventView::DoDeactivateImpl()
    {
    TRACE_ENTRY_POINT;
    // After forced exit need to Reset CCalenAttachmentModel 
    //to refresh the model after opening the calender next time
    if (iServices.GetAttachmentData()->NumberOfItems()) 
        {
        iServices.GetAttachmentData()->Reset();
        }
    iPreviousViewId.iViewUid = KNullUid;

    // Remove the toolbar buttons for event viewer before exiting from event view
    TRAP_IGNORE(RemoveToolbarButtonsL());
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenEventView::OnLocaleChangedL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventView::OnLocaleChangedL(TInt aReason)
    {
    TRACE_ENTRY_POINT;

      if(iContainer)
          {
          if(IsContainerFocused())
              {
              if( (aReason & EChangesLocale) || (aReason & EChangesSystemTime) )
                  {
                  CCalenEventViewContainer* cnt = static_cast<CCalenEventViewContainer*>( iContainer );
                  cnt->HandleLocaleChangedL();
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
// CCalenEventView::RedrawStatusPaneL
// Redraw status pane when Form is closed
// (other items were commented in a header)
// ----------------------------------------------------------------------------
//
void CCalenEventView::RedrawStatusPaneL()  
    {
    TRACE_ENTRY_POINT;

    CCalenEventViewContainer* cnt = static_cast<CCalenEventViewContainer*>( iContainer );
    HBufC* titleText = StringLoader::LoadLC( cnt->GetTitleTextId(), iCoeEnv );
    iSPUtils->UnderLineTitleText( EFalse );
    iSPUtils->SetTitleText( titleText );    // ownership passed
    CleanupStack::Pop( titleText );
    iSPUtils->RefreshStatusPane();

    TRACE_EXIT_POINT;
    }
  
// ----------------------------------------------------------------------------
// CCalenEventView::HandleCommandL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventView::HandleCommandL(TInt aCommand)  
    {
    TRACE_ENTRY_POINT;

    CCalenEventViewContainer* cnt = static_cast<CCalenEventViewContainer*>( iContainer );
    MCalenToolbar* toolbarImpl = iServices.ToolbarOrNull();
    switch(aCommand)
		{
		case ECalenCmdPromptThenEdit:
			{
			iServices.IssueCommandL(ECalenEditCurrentEntry);
			break;	
			}
        case ECalenCompleteTodo:
        case ECalenRestoreTodo:
        // TODO: Uncomment this when enabling attachment support
        case ECalenViewAttachmentList:    
        	{        
        	CCalenNativeView::HandleCommandL(aCommand);
        	}
            break;
        case EAknSoftkeyBack:
			{
			TRAP_IGNORE(RemoveToolbarButtonsL());
	        iServices.IssueNotificationL(ECalenNotifyEntryClosed);
			break;
			}
		case  ECalenStopAlarm:
            {
            iServices.IssueNotificationL(ECalenNotifyAlarmStopped);
            iEventViewCBA = EShowBack;
            UpdateCbaL();
            
            //show the toolbar again.
            if( toolbarImpl ) // If toolbar exists
                {
                CAknToolbar& toolbar = toolbarImpl->Toolbar();
                toolbar.SetItemDimmed( ECalenSend, EFalse, ETrue );
                toolbar.SetItemDimmed( ECalenEditCurrentEntry, EFalse, ETrue );
                toolbar.SetItemDimmed( ECalenDeleteCurrentEntry, EFalse, ETrue );
                }
            }
            break;
        case ECalenSnoozeAlarm:
            {
            iServices.IssueNotificationL(ECalenNotifyAlarmSnoozed);
            }            
            break;
            
        case EAknCmdHideInBackground:
            {
            if( IsAlarmActiveInViewer() )
                {
                iServices.IssueNotificationL(ECalenNotifyAlarmSnoozed);
                }
            else
                {
                if(cnt->GetFindItemMenu()->CommandIsValidL(aCommand))
                    {
                    CAknToolbar& toolbar = toolbarImpl->Toolbar();
                    toolbar.SetToolbarVisibility( EFalse );
                    cnt->GetFindItemMenu()->HandleItemFinderCommandL( aCommand );
                    toolbar.SetToolbarVisibility( ETrue );
                    return;
                    }
                CCalenNativeView::HandleCommandL(aCommand);
                }
            }
            break;
       case EAknSoftkeyEmpty:  // so that it will not crash in stop-snooze case.
            {
            // nothing to do
            }
            break;
        default:
            if(cnt->GetFindItemMenu()->CommandIsValidL(aCommand))
                {
                CAknToolbar& toolbar = toolbarImpl->Toolbar();
                toolbar.SetToolbarVisibility( EFalse );
                cnt->GetFindItemMenu()->HandleItemFinderCommandL( aCommand );
                toolbar.SetToolbarVisibility( ETrue );
                return;
                }
            CCalenNativeView::HandleCommandL(aCommand);
            break;
        }

    TRACE_EXIT_POINT;
    }  

// ----------------------------------------------------------------------------
// CCalenEventView::IsAlarmActiveInViewer
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenEventView::IsAlarmActiveInViewer()
    {
    TRACE_ENTRY_POINT;
    
    if( iEventViewCBA == EShowAlarm || iEventViewCBA == EShowAlarmStopOnly )
        {    
           TRACE_EXIT_POINT;
           
           return ETrue;
        }
    else
        return EFalse;
    }

// ----------------------------------------------------------------------------
// CCalenEventView::DynInitMenuPaneL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
	{
    TRACE_ENTRY_POINT;
    CCalenEventViewContainer* cnt = static_cast<CCalenEventViewContainer*>( iContainer );
    TCalCollectionId colId = iServices.Context().InstanceId().iColId;
    // get multiple db data from services
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    iServices.GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);
    
    HBufC* calendarFileName = iServices.GetCalFileNameForCollectionId(colId).AllocLC();
    TInt index = calendarInfoList.Find( *calendarFileName, 
            CCalenEventViewContainer::CalendarInfoIdentifierL);
   CleanupStack::PopAndDestroy(calendarFileName);
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
	    	    
	    	    // TODO: Uncomment this when enabling attachment support
	    	    
	    	    if(!eventViewData->AttachmentCount())
	    	        {
	    	        aMenuPane->DeleteMenuItem(ECalenViewAttachmentList);
	    	        }
	    	    }
		  	 cnt->GetFindItemMenu()->AddItemFindMenuL(cnt->GetItemFinder(),aMenuPane,EFindItemMenuPlaceHolder,KNullDesC);
		  	 
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
		  	if(!(calendarInfoList[index]->Enabled()))
		  	    {
                aMenuPane->DeleteMenuItem(ECalenSend);
                aMenuPane->DeleteMenuItem(ECalenCmdPromptThenEdit);
                aMenuPane->DeleteMenuItem(ECalenDeleteCurrentEntry);
		  	    }
		  	
		  	 
		    break;
		  	}
		 default:
		    {
		      cnt->GetFindItemMenu()->UpdateItemFinderMenuL(aResourceId,aMenuPane); 
		    }
			break;
		}
    CleanupStack::PopAndDestroy(&calendarInfoList);
    TRACE_EXIT_POINT;	
	}
	
// ----------------------------------------------------------------------------
// CCalenEventView::Id
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TUid CCalenEventView::Id() const
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	
    return KUidCalenEventView;
	}

// ----------------------------------------------------------------------------
// CCalenEventView::CreateContainerImplL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenContainer* CCalenEventView::CreateContainerImplL() 
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	
    return new( ELeave )CCalenEventViewContainer( this, iServices );
	}

// ---------------------------------------------------------
// CCalenEventView::UpdateCbaL
// Set CBA button 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CCalenEventView::UpdateCbaL()
    {
    TRACE_ENTRY_POINT;  
      
    CEikButtonGroupContainer*  cba = Cba();
    switch(iEventViewCBA)
    {
        
    	case EShowClose:
	    	{
	    	 // if launched from soft notification/indicator
        	cba->SetCommandL( KSK2CBAPosition, R_CALEN_CLOSE_CBA_BUTTON);
    		cba->DrawNow();		
	    	}
	    	break;
    	
    	case EShowAlarm:
	    	{
	    	SetCbaL(R_CALEN_EVENT_VIEW_ALARM_CBA);	
	    	}
	    	break;
    	
    	case EShowAlarmStopOnly:
	    	{
	    	SetCbaL(R_CALEN_EVENT_VIEW_ALARM_STOP_ONLY_CBA);	
	    	}
	    	break;
    	
    	case EShowBack:
	    	{
	    	SetCbaL(R_CALEN_EVENT_VIEW_FROM_ALARM_CBA/*R_CALEN_BACK_CBA_BUTTON*/);	
	    	}
	    	break;
    	
    	default:
	    	{
	    	cba->SetCommandL(KSK2CBAPosition,R_CALEN_EXIT_CBA_BUTTON);
    		cba->DrawNow();	
	    	}    	
    		break;
    	
    	
    }    
       
    TRACE_EXIT_POINT;
    }
// ----------------------------------------------------------------------------
// CCalenEventView::ClearViewSpecificDataL
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenEventView::ClearViewSpecificDataL()
	{
    TRACE_ENTRY_POINT;

    TRACE_EXIT_POINT;	
	}

// ----------------------------------------------------------------------------
// CCalenEventView::CyclePosition
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenView::TCyclePosition CCalenEventView::CyclePosition() const
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;	
    return ENoCyclePosition;
	}

// ----------------------------------------------------------------------------
// CCalenEventView::AddToolbarButtonsL
// Adds the necessary toolbar buttons on the existing toolbar for event viewer
// ----------------------------------------------------------------------------
//
void CCalenEventView::AddToolbarButtonsL()
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
	        toolbar.AddItemL( button, EAknCtButton, ECalenSend, 0, 0);
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
	    // dim the tool bar items. 
	    if( iEventViewCBA == EShowAlarm || iEventViewCBA == EShowAlarmStopOnly )
	        {
	        toolbar.SetItemDimmed( ECalenSend, ETrue, ETrue );
	        toolbar.SetItemDimmed( ECalenEditCurrentEntry, ETrue, ETrue );
	        toolbar.SetItemDimmed( ECalenDeleteCurrentEntry, ETrue, ETrue );
	        }
		}
    TRACE_EXIT_POINT;
	}

// ----------------------------------------------------------------------------
// CCalenEventView::RemoveToolbarButtonsL
// Removes the event viewer toolbar buttons and unhides the view cycling buttons
// ----------------------------------------------------------------------------
//
void CCalenEventView::RemoveToolbarButtonsL()
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
// CCalenEventView::CreateButtonL
// Create calendar toolbar buttons
// ----------------------------------------------------------------------------
CAknButton* CCalenEventView::CreateButtonL( CGulIcon* aIcon, 
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
    
// ----------------------------------------------------------------------------
// CCalenEventView::UpdateToolbarButtonsL
// Updates the toolbar buttons on the existing toolbar for event viewer
// ----------------------------------------------------------------------------
//  

void CCalenEventView::UpdateToolbarButtonsL()
    {
    // Get the existing toolbar from MCalenservices
    MCalenToolbar* toolbarImpl = iServices.ToolbarOrNull();
    CAknToolbar& toolbar = toolbarImpl->Toolbar();
    TCalCollectionId colId = iServices.Context().InstanceId().iColId;
      
    // get multiple db data from services
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    iServices.GetAllCalendarInfoL(calendarInfoList);
    CleanupClosePushL(calendarInfoList);
    
    HBufC* calendarFileName = iServices.GetCalFileNameForCollectionId(colId).AllocLC();
    TInt index = calendarInfoList.Find( *calendarFileName, 
    CCalenEventViewContainer::CalendarInfoIdentifierL);
    CleanupStack::PopAndDestroy(calendarFileName);
    
    
    if(!(calendarInfoList[index]->Enabled()))
        {
        toolbar.SetItemDimmed( ECalenSend, ETrue, ETrue );
        toolbar.SetItemDimmed( ECalenDeleteCurrentEntry, ETrue, ETrue );
        toolbar.SetItemDimmed( ECalenEditCurrentEntry, ETrue, ETrue );
        }
    else
        {
        toolbar.SetItemDimmed( ECalenSend, EFalse, ETrue );
        toolbar.SetItemDimmed( ECalenDeleteCurrentEntry, EFalse, ETrue );
        toolbar.SetItemDimmed( ECalenEditCurrentEntry, EFalse, ETrue );
        }
    CleanupStack::PopAndDestroy(&calendarInfoList);
    }
//end of file
