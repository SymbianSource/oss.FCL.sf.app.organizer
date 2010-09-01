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
* Description:   This is the source file for the CClockMainView class.
*
*/

// System includes
#include <clock.rsg>
#include <avkon.hrh>
#include <aknViewAppUi.h>
#include <aknnotewrappers.h>
#include <aknlists.h>
#include <StringLoader.h>
#include <tz.h>
#include <vtzrules.h>
#include <clockalarmeditor.h>
#include <hlplch.h>
#include <touchfeedback.h>

// User includes
#include "clockmainview.h"
#include "clockmaincontainer.h"
#include "clockalarmarray.h"
#include "clockappui.h"
#include "clockdocument.h"
#include "clock.h"
#include "clock.hrh"
#include "clkuialarmmodel.h"
#include "clkuialarmidleobserver.h"
#include "clkcommon.h"
#include "clock_debug.h"

// Constants
const TInt KMaxCharsInNote( 32 );
const TInt KZerothRule( 0 );
const TInt KDaysInWeek( 7 );
const TInt KOneMinuteInMicrosecond( 1000000 * 60 );
const TInt KOneHourInMinute( 60 );
const TInt KOneMinute( 1 );
const TInt KOneHour( 1 );
const TInt KNoDifference( 0 );

_LIT( KDateTimeFormatter, "%-B%:0%J%:1%T%:3%+B" );  // For the date and time displayed in the rollover note.



// Literals

// ---------------------------------------------------------
// CClockMainView::NewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockMainView* CClockMainView::NewL( CClkUiAlarmModel* aAlarmModel )
    {
	__PRINTS( "CClockMainView::NewL - Entry" );
	
    CClockMainView* self = new ( ELeave ) CClockMainView;
    CleanupStack::PushL( self );
    
    self->ConstructL( aAlarmModel );
    
    CleanupStack::Pop( self );
	
	__PRINTS( "CClockMainView::NewL - Exit" );
	
    return self;
    }

// ---------------------------------------------------------
// CClockMainView::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::ConstructL( CClkUiAlarmModel* aAlarmModel )
	{
	__PRINTS( "CClockMainView::ConstructL - Entry" );
	
	BaseConstructL( R_CLOCK_MAIN_VIEW );
	
	// Save the model.
	iAlarmModel = aAlarmModel;
	
	// Construct the alarm array.
	iAlarmArray = CClockAlarmArray::NewL( iAlarmModel, iCoeEnv );
	
    if( !iModelObserver )
        {
        iModelObserver = CClkUiAlarmIdleObserver::NewL( this, KAlarmIdleObserverPriority );
        // Set the observer for the model.
        iAlarmModel->SetModelObserver( iModelObserver );
        }
	
	__PRINTS( "CClockMainView::ConstructL - Exit" );
	}

// ---------------------------------------------------------
// CClockMainView::CClockMainView
// rest of the details are commented in the header
// ---------------------------------------------------------
//
//Single click integration
CClockMainView::CClockMainView()

    {
	__PRINTS( "CClockMainView::CClockMainView - Entry" );
	
	// No implementation yet.
	
	__PRINTS( "CClockMainView::CClockMainView - Exit" );
	}

// ---------------------------------------------------------
// CClockMainView::SetNewAlarmL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::SetNewAlarmL()
    {
	__PRINTS( "CClockMainView::SetNewAlarmL - Entry" );
	
	// Hide the navigation pane.
	CClockAppUi* appUi = static_cast< CClockAppUi* > ( AppUi() );
	appUi->MakeNavigationPaneVisible( EFalse, EClockAppMainViewId );
	
    // Mark the alarm editor as open.
    iAlarmEditorOpen = ETrue;
    
    // Construct the alarm editor.
    CClockAlarmEditor *alarmEditor = CClockAlarmEditor::NewL();

    // Launch the editor. iAlarmId will give either alarm ID 
    // or EClockAlarmExit or EClockAlarmDelete.
    iAlarmId = alarmEditor->LaunchL();
	
	// Update the title pane text.
    static_cast< CClockAppUi* > ( AppUi() )->SetTitlePaneTextL( R_CLOCK_TITLE_MAIN_VIEW );
	
	 // Show the navigation pane again.
    appUi->MakeNavigationPaneVisible( ETrue, EClockAppMainViewId );
    
    if( EClockAlarmExit == iAlarmId )
        {		
        // Mark the alarm editor as closed.
		iAlarmEditorOpen = EFalse;

		// Cleanup.
		if( alarmEditor )
			{
			delete alarmEditor;
			alarmEditor = NULL;
			}
		
			
        // Let the appui handle the exit.
        AppUi()->HandleCommandL( EClockAlarmExit );
        
        __PRINTS( "CClockMainView::SetNewAlarmL:EClockAlarmExit - Exit" );
        
        return;
        }

    if( EClockAlarmDelete == iAlarmId )
        {		
		
        // Mark the alarm editor as closed.
		iAlarmEditorOpen = EFalse;

		// Cleanup.
		if( alarmEditor )
			{
			delete alarmEditor;
			alarmEditor = NULL;
			}
         
        // Do nothing. Just close the form without saving any data.
        __PRINTS( "CClockMainView::SetNewAlarmL:EClockAlarmDelete - Exit" );
        
        return;
        }

    // If the user pressed cancel and if the alarm id is valid. 
    if( KErrNone != iAlarmId )
        {
        // Update the latest alarm details.
        iAlarmModel->ClockAlarmInfo( iAlarmId, iAlarmInfo );
        
        if(CheckForDstChangesL())
            {
            DisplayRemainingTimeL();
            }

        // First update the model with the latest alarm id.
        iAlarmModel->SaveLatestAlarmId( iAlarmId );
        
        // Update the container with the newly added alarm.
        iContainer->UpdateAlarmListL( iAlarmInfo, iAlarmId );
        
       
        }
    else
        {
        // No implementation yet.
        }
    
    // Mark the alarm editor as closed.
    iAlarmEditorOpen = EFalse;

    // Cleanup.
    if( alarmEditor )
        {
        delete alarmEditor;
        alarmEditor = NULL;
        }
   
   
	__PRINTS( "CClockMainView::SetNewAlarmL - Exit" );
    }

// ---------------------------------------------------------
//papercut clock2 Remove_Snooze Start - rajender
// CClockMainView::RemoveSnoozeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::RemoveSnoozeL()
    {  
    __PRINTS( "CClockMainView::RemoveSnoozeL - Entry" );       
    // First get the index of the selected item in the list.
    TInt itemIndex( iContainer->ListBox()->CurrentItemIndex() );
    
    if( KErrNone > itemIndex )
        {
        __PRINTS( "CClockMainView::RemoveSnoozeL - Exit" );
        return;
        }

        SClkAlarmInfo alarmInfo;
        TAlarmId alarmId;
        // Get the alarm id from alarm model.
        iAlarmArray->GetSortedAlmIdInfo( itemIndex, alarmId, alarmInfo );
        
        // Delete the alarm.
        User::LeaveIfError( iAlarmModel->AlarmRemoveSnooze( alarmId ) );
                
        // Update the container.
        iContainer->ListBox()->HandleItemAdditionL();
        
        // Display the confirmation note to the user about alarm deletion.
        //CAknConfirmationNote* confNote = new( ELeave ) CAknConfirmationNote;
        //HBufC* confNoteText = StringLoader::LoadLC( R_QTN_CLOCK_SNOOZE_NOTE_REMOVED, iCoeEnv );
        // Display the note.
        //confNote->ExecuteLD( *confNoteText );
        // Cleanup.
        //CleanupStack::PopAndDestroy( confNoteText );
        
        __PRINTS( "CClockMainView::RemoveSnoozeL - Exit" );        
        
    }

// ---------------------------------------------------------
// CClockMainView::RemoveAlarmL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::RemoveAlarmL( TAlarmId aAlarmId )
    {
    __PRINTS( "CClockMainView::RemoveAlarmL - Entry" );
    
    // First get the index of the selected item in the list.
    TInt itemIndex( iAlarmArray->ListBoxIndex( aAlarmId ) );
    
    if( KErrNone > itemIndex )
        {
        __PRINTS( "CClockMainView::RemoveAlarmL - Exit" );
        
        return;
        }
        // Delete the alarm.
        User::LeaveIfError( iAlarmModel->DeleteClockAlarm( aAlarmId ) );
        // Update the container.
        iContainer->RemoveAlarmL( itemIndex );
        
    __PRINTS( "CClockMainView::RemoveAlarmL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainView::RemoveAlarmL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::RemoveAlarmL()
    {
	__PRINTS( "CClockMainView::RemoveAlarmL - Entry" );
	
    // First get the index of the selected item in the list.
    TInt itemIndex( iContainer->ListBox()->CurrentItemIndex() );
    
    if( KErrNone > itemIndex )
        {
		__PRINTS( "CClockMainView::RemoveAlarmL - Exit" );
		
        return;
        }
    
    
        SClkAlarmInfo alarmInfo;
        TAlarmId alarmId;
        // Get the alarm id from alarm model.
        iAlarmArray->GetSortedAlmIdInfo( itemIndex, alarmId, alarmInfo );
        
        // Delete the alarm.
        User::LeaveIfError( iAlarmModel->DeleteClockAlarm( alarmId ) );
        // Update the container.
        iContainer->RemoveAlarmL( itemIndex );
        
        
       
      
	
	__PRINTS( "CClockMainView::RemoveAlarmL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainView::ActivateAlarmL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::ActivateAlarmL()
    {
	__PRINTS( "CClockMainView::ActivateAlarmL - Entry" );
	
    // First get the focused list item.
    TInt itemIndex( iContainer->ListBox()->CurrentItemIndex() ); 
    // Id of the alarm to be activated.
    TAlarmId alarmId;
    // Dummy alarm info.
    SClkAlarmInfo alarmInfo;
    
    // Get the id of the alarm at the index in the list.
    iAlarmArray->GetSortedAlmIdInfo( itemIndex, alarmId, alarmInfo );

    // Activate the alarm.
    User::LeaveIfError( iAlarmModel->ClockAlarmEnable( alarmId, EAlarmStatusEnabled ) );
    // Update the listbox.
    iContainer->ListBox()->SetCurrentItemIndexAndDraw( iAlarmArray->ListBoxIndex( alarmId ) );
	
	__PRINTS( "CClockMainView::ActivateAlarmL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainView::DeactivateAlarmL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::DeactivateAlarmL()
    {
	__PRINTS( "CClockMainView::DeactivateAlarmL - Entry" );
	
    // First get the focused list item.
    TInt itemIndex( iContainer->ListBox()->CurrentItemIndex() ); 
    // Id of the alarm to be deactivated.
    TAlarmId alarmId;
    // Dummy alarm info.
    SClkAlarmInfo alarmInfo;

    // Get the id of the alarm at the index in the list.
    iAlarmArray->GetSortedAlmIdInfo( itemIndex, alarmId, alarmInfo );

    // Deactivate the alarm.
    User::LeaveIfError( iAlarmModel->ClockAlarmEnable( alarmId, EAlarmStatusDisabled ) );
    // Update the listbox.
    iContainer->ListBox()->SetCurrentItemIndexAndDraw( iAlarmArray->ListBoxIndex( alarmId ) );
	
	__PRINTS( "CClockMainView::DeactivateAlarmL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainView::ResetAlarmL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::ResetAlarmL()
    {
	__PRINTS( "CClockMainView::ResetAlarmL - Entry" );
	
    // First get the focused list item.
    TInt itemIndex( iContainer->ListBox()->CurrentItemIndex() ); 
    // Id of the alarm to be deactivated.
    TAlarmId alarmId;
    // Dummy alarm info.
    SClkAlarmInfo alarmInfo;

    // Get the id of the alarm at the index in the list.
    iAlarmArray->GetSortedAlmIdInfo( itemIndex, alarmId, alarmInfo );
    
    // Hide the navigation pane.
    CClockAppUi* appUi = static_cast< CClockAppUi* > ( AppUi() );
    appUi->MakeNavigationPaneVisible( EFalse, EClockAppMainViewId );
    
    // Mark the alarm editor as open.
    iAlarmEditorOpen = ETrue;
    
    // Construct the alarm editor.
    CClockAlarmEditor* alarmEditor = CClockAlarmEditor::NewL( alarmId );
    
    // Launch the editor.iAlarmId will give either alarm ID 
    // or EClockAlarmExit or EClockAlarmDelete.
    iAlarmId = alarmEditor->LaunchL();
	
	// Update the title pane text.
    static_cast< CClockAppUi* > ( AppUi() )->SetTitlePaneTextL( R_CLOCK_TITLE_MAIN_VIEW );
	
	 // Show the navigation pane again.
    appUi->MakeNavigationPaneVisible( ETrue, EClockAppMainViewId );

    if(  EClockAlarmDelete == iAlarmId )
        {
        // Mark the alarm editor as closed.
        iAlarmEditorOpen = EFalse;
    
        if( alarmEditor )
            {
            delete alarmEditor;
            alarmEditor = NULL;
            }
        __PRINTS( "CClockMainView::ResetAlarmL:EClockAlarmDelete - Exit" ); 
        
        // do the removing of the alarm
        RemoveAlarmL( alarmId );
        
        return;
        }    

    if( EClockAlarmExit == iAlarmId )
        {		
        // Mark the alarm editor as closed.
		iAlarmEditorOpen = EFalse;
    
		if( alarmEditor )
			{
			delete alarmEditor;
			alarmEditor = NULL;
			}
		
        // Let the appui handle the exit.
        AppUi()->HandleCommandL( EClockAlarmExit );
       
        __PRINTS( "CClockMainView::ResetAlarmL:EClockAlarmExit - Exit" );
        
        return;
        }
    
    if( EClockAlarmDiscardChanges == iAlarmId )
        {		
		  // Mark the alarm editor as closed.
		iAlarmEditorOpen = EFalse;
		
		// Cleanup.
		if( alarmEditor )
			{
			delete alarmEditor;
			alarmEditor = NULL;
			}
		
        // Dont do anything. Simply return.
        
        __PRINTS( "CClockMainView::ResetAlarmL:EClockAlarmDiscardChanges - Exit" );
                
        return;
        }
    
    // If the user pressed cancel and if the alarm id is valid.
    if( KErrNone != iAlarmId )
        {
        // Delete the previous alarm.
        User::LeaveIfError( iAlarmModel->DeleteClockAlarm( alarmId ) );
        
        // Remove this alarm from the list.
        iContainer->RemoveAlarmL( itemIndex );
        
        // Update the latest alarm details.
        iAlarmModel->ClockAlarmInfo( iAlarmId, iAlarmInfo );
        
		if(CheckForDstChangesL())
            {
            DisplayRemainingTimeL();
            }

        // First update the model with the latest alarm id.
        iAlarmModel->SaveLatestAlarmId( iAlarmId );
        
        // Update the container with the new alarm.
        iContainer->UpdateAlarmListL( iAlarmInfo, iAlarmId );
        }
    
    // Mark the alarm editor as closed.
    iAlarmEditorOpen = EFalse;
    
    if( alarmEditor )
        {
        delete alarmEditor;
        alarmEditor = NULL;
        }
   
   
	__PRINTS( "CClockMainView::ResetAlarmL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainView::~CClockMainView
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockMainView::~CClockMainView()
	{
	__PRINTS( "CClockMainView::~CClockMainView - Entry" );
	
	if( iAlarmArray )
	    {
	    delete iAlarmArray;
	    iAlarmArray = NULL;
	    }
	if( iContainer )
	    {
	    ( AppUi() )->RemoveFromStack( iContainer );
	    delete iContainer;
	    iContainer = NULL;
	    }
	if( iModelObserver )
	    {
		if( iAlarmModel )
			{
			iAlarmModel->SetModelObserver( NULL );
			}
	    delete iModelObserver;
	    iModelObserver = NULL;
	    }
		
	__PRINTS( "CClockMainView::~CClockMainView - Exit" );
	}

// ---------------------------------------------------------
// CClockMainView::Id
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TUid CClockMainView::Id() const
	{
	__PRINTS( "CClockMainView::Id - Entry" );
	
	__PRINTS( "CClockMainView::Id - Exit" );
	
	return KClockAppMainViewId;
	}

// ---------------------------------------------------------
// CClockMainView::DoActivateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
        					      TUid aCustomMessageId,
        					      const TDesC8& /*aCustomMessage*/ )
    {
	__PRINTS( "CClockMainView::DoActivateL - Entry" );
	
    if( KClockHideInBackground == aCustomMessageId.iUid )
		{
		CClockAppUi* appUi = static_cast< CClockAppUi* > ( AppUi() );
		
		// Enable bring-to-foreground on view activation.
		appUi->SetCustomControl( 0 );
		}  
	
    // Re-initialize the alarm id array.
    iAlarmArray->InitIdList();
    
    // To update the scroll bar when opened from indicator pane.
    if( iContainer )
        {
        iContainer->ListBox()->SetCurrentItemIndexAndDraw( KZerothIndex );
        }
    
    CClockAppUi* clockAppUi( static_cast< CClockAppUi* > ( AppUi() ) );
    if( !iContainer )
    	{
    	iContainer = CClockMainContainer::NewL( this, ClientRect(), iAlarmArray );
    	( AppUi() )->AddToStackL( *this, iContainer );
    	iContainer->SetMopParent( this );    	
    	
    	if( clockAppUi->IsAppExitHidden() )
    	    {
    	    iContainer->ListBox()->SetCurrentItemIndexAndDraw( KZerothIndex );
    	    clockAppUi->SetAppHidden( EFalse );
    	    
    	    }
    	else
            {

            // Focus on the "New Alarm" whenever the main view is activated 


            iContainer->ListBox()->SetCurrentItemIndexAndDraw( KErrNone);
            }
    	}

    // Update the date and day text.
    iContainer->SetDayInfoL();
    
    // Update the location information text.
    iContainer->SetZoneInfoL();
    
    // Update the DST information in the container.
    iContainer->UpdateDSTStateL();
    
    // Update the title pane text.
    clockAppUi->SetTitlePaneTextL( R_CLOCK_TITLE_MAIN_VIEW );
      
    // If opened from clock, display the navigation pane again.
    clockAppUi->MakeNavigationPaneVisible( ETrue, EClockAppMainViewId );
    
    // Activate the container.
    if( clockAppUi->TransitionOngoing() )
        {
        //clockAppUi->DoAppearTransition( iContainer );
        iContainer->ActivateL();
        }
    else
        {
        iContainer->ActivateL();
        }
    
	__PRINTS( "CClockMainView::DoActivateL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainView::DoDeactivate
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::DoDeactivate()
	{
	__PRINTS( "CClockMainView::DoDeactivate - Entry" );
	
	// Ask the document to save the user data in the document file.
	CClockDocument* clockDocument = static_cast< CClockDocument* > ( AppUi()->Document() );

	
	if( iContainer )
		{
	    CClockAppUi* clockAppUi( static_cast< CClockAppUi* > ( AppUi() ) );
        if( clockAppUi->TransitionOngoing() )
            {
            //clockAppUi->DoDisappearTransition( iContainer );
            }
		// First remove the container from the control stack
		( AppUi() )->RemoveFromStack( iContainer );
		// And then destroy it.
		delete iContainer;
		iContainer = NULL;
		}
	
	__PRINTS( "CClockMainView::DoDeactivate - Exit" );
    }

// ---------------------------------------------------------
// CClockMainView::HandleCommandL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::HandleCommandL( TInt aCommand )
	{
	__PRINTS( "CClockMainView::HandleCommandL - Entry" );
	
	switch( aCommand )
		{
		//Single click integration
	    case EClockSelect:
		    {
		    ResetAlarmL();
		    }
		    break; 

	    case EClockNewAlarm:
		    {
		    // Display the alarm editor for the user to set the alarm.
		    SetNewAlarmL();
		    }
		    break;
		    
		case EClockResetAlarm:
		    {
		    // Reset the chosen alarm.
		    ResetAlarmL();
		    }
		    break;
		    
		case EClockDeactivateAlarm:
		    {
		    // Deactivate the focused alarm item in the list.
		    DeactivateAlarmL();
		    }
		    break;
		    
		case EClockActivateAlarm:
		    {
		    // Activate the focused alarm item in the list.
		    ActivateAlarmL();
		    }
		    break;
		    
		case EClockRemoveAlarm:
		    {
		    // Delete the selected alarm.
		    RemoveAlarmL();
		    }
		    break;
		case EClockRemoveSnooze:
		    {
		    // Remove snooze on the selected alarm
		    RemoveSnoozeL();
		    }
		    break;
	      case EClockSwitchClockType:
            {
            // Switches clock type
            SwitchClockTypeL();
            }
            break;
		case EClockSettings:
		    {
		    // Let the appui handle the command.
		    AppUi()->HandleCommandL( EClockSettings );
		    }
		    break;
		    
		case EClockMainViewExit:
		case EAknSoftkeyExit:
		    {
		    AppUi()->HandleCommandL( aCommand );
		    }
		    break;
			
	    case EAknCmdHelp:  //EClockWorldHelp:
			{
			// Launch clock main view specific help.
			HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(), iAvkonAppUi->AppHelpContextL() );
			}
			break;
		    
		default:
			{
			// No implementation yet.
			}
			break;
		}
		
	__PRINTS( "CClockMainView::HandleCommandL - Exit" );
	}

// ---------------------------------------------------------
// CClockMainView::HandleUpdateL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::HandleUpdateL( TInt /*aReason*/ )
    {
	__PRINTS( "CClockMainView::HandleUpdateL - Entry" );

    // Get the currently focussed item.	
	TInt currentlyFocussedItem( KErrNotFound );
	if( iContainer )
	    {
		currentlyFocussedItem = iContainer->ListBox()->CurrentItemIndex();
	    } 
	// Update the alarm array.
	iAlarmArray->InitIdList();
	
    // We redraw the container when an update is received from the uimodel (be it alarm or clockserver).
    if( iContainer )
        {
        iContainer->DrawNow();
        iContainer->ListBox()->HandleItemAdditionL();
        
        // In case the currently focussed item is invalid,set the focus to last item and set the msk text accordingly. 
        if( KErrNotFound == currentlyFocussedItem ) 
            {
            TInt itemCount = iContainer->ListBox()->Model()->ItemTextArray()->MdcaCount();
			if( itemCount > 0 )
                {
                iContainer->ListBox()->SetCurrentItemIndexAndDraw( itemCount -1 );
                }
            else
                {
                iContainer->ListBox()->SetCurrentItemIndexAndDraw( KZeroAlarms );
                }
        	
        	
            }    
        }
	
	__PRINTS( "CClockMainView::HandleUpdateL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainView::DynInitMenuPaneL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
	__PRINTS( "CClockMainView::DynInitMenuPaneL - Entry" );

	//Single click integration
    if( ( R_CLOCK_MAIN_VIEW_MENUPANE != aResourceId ) )


        {
		__PRINTS( "CClockMainView::DynInitMenuPaneL - Exit" );
		
        return;
        }

    // TODO: Initialize FeatureManager and remove Help option if its not supported.
    /*if( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        aMenuPane->DeleteMenuItem( EAlarmHelpAlarmClock );
        }*/

	
    TInt alarmCount = iAlarmArray->MdcaCount();
    TBool deleteRemoveSnooze( EFalse );

    // There are no alarms set 
    if( KZeroAlarms >= alarmCount || iContainer->IsNewAlaramButtonFocused() )
        {
        // There are no alarms set. Remove the irrelevant options from the menu.
        aMenuPane->DeleteMenuItem( EClockResetAlarm );
        aMenuPane->DeleteMenuItem( EClockRemoveAlarm );
        aMenuPane->DeleteMenuItem( EClockActivateAlarm );           
        aMenuPane->DeleteMenuItem( EClockDeactivateAlarm );                                 
        deleteRemoveSnooze = ETrue;
        }
    else
        {
        // There is atleast one alarm set. Get the details to update the menu.
        SClkAlarmInfo alarmInfo;
        TAlarmId alarmId;
        
       
        iAlarmArray->GetSortedAlmIdInfo( iContainer->ListBox()->CurrentItemIndex(), alarmId, alarmInfo );       

        if( alarmInfo.iStatus == EAlarmStatusEnabled ) 
            {
            // Alarm is already active. Remove the activate option.
            aMenuPane->DeleteMenuItem( EClockActivateAlarm ); 
            
                      

            if( ( EAlarmRepeatDefintionRepeatOnce == alarmInfo.iRepeat ) || 
                ( EAlarmRepeatDefintionRepeatNext24Hours == alarmInfo.iRepeat) )
                {
                // Alarm is not recurring. Remove the deactivate option from the menu.
                aMenuPane->DeleteMenuItem( EClockDeactivateAlarm ); 
                // no snooze removing needed for not recurring
                deleteRemoveSnooze = ETrue;
                }
            // if the alarm is not snoozed, Remove the 'Remove snooze' option
            if (alarmInfo.iState != EAlarmStateSnoozed )
                {
                deleteRemoveSnooze = ETrue;
                }
            }
        else        
            {
            // Alarm is already inactive. Remove the deactivate option from the menu. 
            aMenuPane->DeleteMenuItem( EClockDeactivateAlarm );
            deleteRemoveSnooze = ETrue;
            }
        }
       if( deleteRemoveSnooze )
           {
           aMenuPane->DeleteMenuItem( EClockRemoveSnooze );
           }
		
	__PRINTS( "CClockMainView::DynInitMenuPaneL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainView::DynInitMenuBarL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
//Single click integration
void CClockMainView::DynInitMenuBarL( TInt /*aResourceId*/, CEikMenuBar* /*aMenuBar*/ )

    {
	__PRINTS( "CClockMainView::DynInitMenuBarL - Entry" );
	
    
	
	__PRINTS( "CClockMainView::DynInitMenuBarL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainView::HandleForegroundEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::HandleForegroundEventL( TBool aForeground )
    {
    __PRINTS( "CClockMainView::HandleForegroundEventL - Entry" );
    
    if( aForeground )
        {
        CClockAppUi* clockAppUi( static_cast< CClockAppUi* > ( AppUi() ) );
        if( clockAppUi->IsAppExitHidden() )
            {
            if( iContainer )
                {
                iContainer->ResetMarqueeLabelL();
                iContainer->ListBox()->SetCurrentItemIndexAndDraw( KZerothIndex );
                }
            clockAppUi->SetAppHidden( EFalse );
           
            }
        }
    
    __PRINTS( "CClockMainView::HandleForegroundEventL - Exit" );
    }
// ---------------------------------------------------------
// CClockMainView::GetApplicationRect
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TRect CClockMainView::GetApplicationRect()
    {
	__PRINTS( "CClockMainView::GetApplicationRect - Entry" );
	
	__PRINTS( "CClockMainView::GetApplicationRect - Exit" );
	
    // Return the applicationrect from appui.
    return AppUi()->ApplicationRect();
    }



// ---------------------------------------------------------
// CClockMainView::HandleEnvChangeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::HandleEnvChangeL( TClockEnvChanged /*aEnvChanged*/ )
    {
	__PRINTS( "CClockMainView::HandleEnvChangeL - Entry" );
	
    if( iContainer )
        {
        // Update the main view information.
        iContainer->SetDayInfoL();
        
        // Update the location information text.
        iContainer->SetZoneInfoL();
        
        // Update the DST information.
        iContainer->UpdateDSTStateL();
        
        // Update the container.
        iContainer->DrawNow();
        }
	
	__PRINTS( "CClockMainView::HandleEnvChangeL - Exit" );
    }

// ---------------------------------------------------------
// CClockMainView::IsAlarmEditorOpen
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockMainView::IsAlarmEditorOpen()
    {
	__PRINTS( "CClockMainView::IsAlarmEditorOpen - Entry" );
	
	__PRINTS( "CClockMainView::IsAlarmEditorOpen - Exit" );
	
    return iAlarmEditorOpen;
    }

// ---------------------------------------------------------
// CClockMainView::ClockApplicationUi
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockAppUi* CClockMainView::ClockApplicationUi()
    {
    __PRINTS( "CClockMainView::ClockApplicationUi - Entry" );

    __PRINTS( "CClockMainView::ClockApplicationUi - Exit" );
    
    return static_cast< CClockAppUi* > ( AppUi() );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::DisplayRemainingTimeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::DisplayRemainingTimeL()
    {
    __PRINTS( "CClockAlarmEditorImpl::DisplayRemainingTimeL - Entry" );
    
    HBufC* stringHolder( NULL );

    TTime currentTime;
    currentTime.HomeTime();     

    TTime alarmTime = iAlarmInfo.iOrigExpiryTime;
    TBool oneDayDifference( EFalse );    

    // Check for the day of the alarm.
    if( currentTime < alarmTime )
        {
        currentTime += TTimeIntervalDays( 1 );
        // Check if the alarm is for the current day.
        oneDayDifference = ( currentTime < alarmTime )? ETrue : EFalse;
        }

    TInt alarmDay( alarmTime.DayNoInWeek() );
    const TInt KTimeStringLength( 10 );
    TBuf< KTimeStringLength > timeString;

    HBufC* timeFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO, iCoeEnv );
    CleanupStack::Pop( timeFormat );

    alarmTime.FormatL( timeString, *timeFormat );

    CDesCArrayFlat* stringArray = new( ELeave ) CDesCArrayFlat( 2 );
    CDesCArrayFlat* workDaysList = iCoeEnv->ReadDesCArrayResourceL( R_CLOCK_WEEK_DAYS_ARRAY );

    // The day on which alarm is set.
    stringArray->AppendL( ( *workDaysList )[ alarmDay ] );
    // The time string.
    stringArray->AppendL( timeString );

    // Choose the appropriate Repeat type.
    switch( iAlarmInfo.iRepeat )
        {
        case EAlarmRepeatDefintionRepeatNext24Hours:
        case EAlarmRepeatDefintionRepeatOnce:
            {
            if( oneDayDifference )
                {
                stringHolder = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_NOTE_ONCE_NEXT, *stringArray, iEikonEnv );
                }
            }
            break;
        
        case EAlarmRepeatDefintionRepeatDaily:
            {
            stringHolder = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_NOTE_DAY_CLK, timeString, iEikonEnv );
            }
            break;
            
        case EAlarmRepeatDefintionRepeatWeekly:
            {
            stringHolder = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_NOTE_WEEK_CLK, *stringArray, iEikonEnv );
            }
            break;
            
        case EAlarmRepeatDefintionRepeatWorkday:
            {
            stringHolder = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_NOTE_WDAY_CLK, timeString, iEikonEnv );
            }
            break;
            
        default:
            {
            // No implementation yet.
            }
            break;
        }
    
    // Cleanup.
    delete timeFormat;
    delete stringArray;
    delete workDaysList;        

    // Need to show the confirmation note.
    if( ( EAlarmStatusEnabled == iAlarmInfo.iStatus ) && stringHolder )
        {
        // TODO: to be verified todo this or not.
        CAknConfirmationNote* confirmationNote = new( ELeave ) CAknConfirmationNote( ETrue );

        TPtr stringHolderPtr = stringHolder->Des();
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( stringHolderPtr );    

        confirmationNote->ExecuteLD( *stringHolder );

        // Cleanup
        CleanupStack::PopAndDestroy( stringHolder );
        } 

    // Dislpay the second note.
    if( !oneDayDifference )
        {
        TTime homeTime;
        homeTime.HomeTime();

        TDateTime currentDate( homeTime.DateTime() );
        TDateTime alarmDate( iAlarmInfo.iAlarmTime.DateTime() );

        alarmDate.SetYear( currentDate.Year() );
        alarmDate.SetMonth( currentDate.Month() );
        alarmDate.SetDay( currentDate.Day() );

        TTime newAlarmTime( alarmDate );

        if( newAlarmTime < homeTime )
            {
            newAlarmTime += TTimeIntervalDays( 1 );
            }

        // Construct the remaining time.            
        TTimeIntervalMicroSeconds remainingTime( newAlarmTime.MicroSecondsFrom( homeTime ) );
        TInt64 tempInt = ( remainingTime.Int64() ) / KOneMinuteInMicrosecond;
        TInt remainingMinutes = I64INT( tempInt );

        CArrayFix< TInt >* timeArray = new( ELeave ) CArrayFixFlat< TInt >( 2 );
        CleanupStack::PushL( timeArray );

        TInt remainingHours( remainingMinutes / KOneHourInMinute );

        remainingMinutes -= remainingHours * KOneHourInMinute;

        timeArray->AppendL( remainingHours );
        timeArray->AppendL( remainingMinutes );

        // Alarm is with in 1 day. Choose the appropriate strings to be displayed.
        switch( remainingHours )
            {
            case KOneMinute:
                {
                if( KOneMinute == remainingMinutes )
                    {
                    stringHolder = StringLoader::LoadLC( R_CLOCK_ALARM_CONF_SINGULAR, *timeArray, iEikonEnv );
                    }
                else
                    {
                    stringHolder = StringLoader::LoadLC( R_CLOCK_ALARM_CONF_HOUR_SEV_MIN, *timeArray, iEikonEnv );
                    }
                }
                break;
                
            default:
                {
                if( KOneMinute == remainingMinutes )
                    {
                    stringHolder = StringLoader::LoadLC( R_CLOCK_ALARM_CONF_SEV_HOURS_MIN, *timeArray, iEikonEnv );
                    }
                else
                    {
                    stringHolder = StringLoader::LoadLC( R_CLOCK_ALARM_CONF_PLURAL, *timeArray, iEikonEnv );
                    }
                }
                break;
            }
        
        // TODO: to be verified todo this or not.
        if( ( EAlarmStatusEnabled == iAlarmInfo.iStatus )  && stringHolder )
            {
            CAknConfirmationNote* confirmationNote = new( ELeave ) CAknConfirmationNote( ETrue );

            TPtr stringHolderPtr = stringHolder->Des();
            AknTextUtils::DisplayTextLanguageSpecificNumberConversion( stringHolderPtr );    

            confirmationNote->ExecuteLD( *stringHolder ); 
            }

        // Cleanup.        
        CleanupStack::PopAndDestroy( stringHolder );
        CleanupStack::PopAndDestroy( timeArray );
        }
    
    __PRINTS( "CClockAlarmEditorImpl::DisplayRemainingTimeL - Exit" );
    }

// ---------------------------------------------------------
// CClockAlarmEditorImpl::CheckForDstChangesL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TBool CClockMainView::CheckForDstChangesL()
    {
    __PRINTS( "CClockAlarmEditorImpl::CheckForDstChangesL - Entry" );
    
    // User to be notified whether DST rollover happens in a day or 
    // has happen within a day if he tries to change the time.
    TBool returnValue( ETrue );
    
    // Establish connection with RTz to get the timezone ID
    RTz tzHandle;
    User::LeaveIfError( tzHandle.Connect() );
    CleanupClosePushL( tzHandle );

    // The timezone ID (current)
    CTzId* currentTZId = tzHandle.GetTimeZoneIdL();
    CleanupStack::PushL( currentTZId );

    // The current time in UTC
    TTime currentTime;
    currentTime.UniversalTime();

    // hometime (local time)
    TTime homeTime;
    homeTime.HomeTime();

    //(Year, Month, Day, Hour, Minute, Second, Micrsecond)
    TDateTime dateTime( homeTime.DateTime().Year(), EJanuary, 1, FALSE, FALSE, FALSE, FALSE );

    TTime tempTime( dateTime ); 

    // Get the current rules for the timezone
    CTzRules* currentRules = tzHandle.GetTimeZoneRulesL( *currentTZId, tempTime, currentTime, ETzUtcTimeReference );
    CleanupStack::PushL( currentRules );

    // CVTzActualisedRules encapsulates the rules for a specific year.
    // Every year has a dummy rule and further DST rules if DST is applicable (if Ohlson provides them)
    CVTzActualisedRules *vActualisedRules = CVTzActualisedRules::NewL(
            homeTime.DateTime().Year(), 
            homeTime.DateTime().Year());
    CleanupStack::PushL( vActualisedRules );

    // The dummy rule is always the begining of the year.
    // For example there is only 1 rule for India/NewDelhi but USA/Atlanta has 3 rules.
    currentRules->GetActualisedRulesL( *vActualisedRules );

    const TInt ruleCount( vActualisedRules->Count() );
    TInt ruleMatchIndex( KNoDifference );

    TTimeIntervalSeconds secondsDifference;
    TTime ruleMatchTime;

    // Fetch lowest time offset for the year residing at aTime. 
    // This is used to determine if DST is on.
    for( TInt ruleIndex( FALSE ); ruleIndex < ruleCount; ++ruleIndex )
        {
        const TVTzActualisedRule& actualisedRule = ( *vActualisedRules )[ ruleIndex ];

        // Only check for the same year as requested (aTime)
        if( actualisedRule.iTimeOfChange.DateTime().Year() == homeTime.DateTime().Year() )
            {               
            iAlarmInfo.iAlarmTime.SecondsFrom( actualisedRule.iTimeOfChange, secondsDifference );
            // Considering the time reference is important as America (North & South) uses
            // the Wall time (local time) reference where as whole of Europe refers to time
            // in terms of UTC time. Correspondingly, the choise of local time or utc time 
            // has to be made.
            TTime ruleTime;

            if( ETzUtcTimeReference == actualisedRule.iTimeReference )
                {
                ruleTime = currentTime;
                }
            else if( ETzWallTimeReference == actualisedRule.iTimeReference )
                {
                ruleTime = homeTime;
                }
            else if( ETzStdTimeReference == actualisedRule.iTimeReference )
                {
                // TODO: Testing so far hasn't encountered a rule in this time reference. 
                // If in case an error is found, corresponding code can be added here.
                // No support from symbian for this.
                }

            TDateTime sevenDays( FALSE, EJanuary, KDaysInWeek, FALSE, FALSE, FALSE, FALSE );
            TTime tempTime( sevenDays );
            TTime newTime( ruleTime.Int64() + tempTime.Int64() );

            TTimeIntervalDays temp;
            temp = newTime.DaysFrom( ruleTime );

            if( ( secondsDifference.Int() >= KNoDifference ) &&
                ( newTime > iAlarmInfo.iAlarmTime ) && 
                ( actualisedRule.iTimeOfChange < iAlarmInfo.iAlarmTime ) && 
                ( ruleTime < actualisedRule.iTimeOfChange ) )
                {
                // If there is a match, save the index and break.
                // We've got the rule and there's no need to continue with other rules.
                ruleMatchIndex = ruleIndex;
                ruleMatchTime = actualisedRule.iTimeOfChange;
                break;
                }
            }
        }

    if( ruleMatchIndex > KZerothRule )
        {
        // There's a match, display the information note about DST change.
        TTime displayTime;
        TTimeIntervalHours oneHour( KOneHour );         

        displayTime = iAlarmInfo.iAlarmTime;

        TBuf< KMaxCharsInNote > dateTimeString;

        // dateString will have "11:59 pm" - as above, but no seconds
        displayTime.FormatL( dateTimeString, KDateTimeFormatter );

        // If This note is displayed,
        CAknInformationNote* informationNote = new( ELeave ) CAknInformationNote( ETrue );

        HBufC* noteText = StringLoader::LoadLC(
                R_QTN_CLOCK_NOTE_ALARM_DST_ROLLOVER, 
                dateTimeString, 
                iCoeEnv);               
        TInt error = informationNote->ExecuteLD( noteText->Des() );

        // Don't display the second note. Not necessary to show both notes.
        returnValue = ETrue;
        CleanupStack::PopAndDestroy( noteText );                                
        } 

    tzHandle.Close();
    CleanupStack::PopAndDestroy( vActualisedRules);
    CleanupStack::PopAndDestroy( currentRules );
    CleanupStack::PopAndDestroy( currentTZId );
    CleanupStack::PopAndDestroy( &tzHandle );
    
    __PRINTS( "CClockAlarmEditorImpl::CheckForDstChangesL - Exit" );

    return returnValue;
    }

void CClockMainView::GiveVibes()
    {
    CClockAppUi* clockAppUi( static_cast< CClockAppUi* > ( AppUi() ) );
    if ( clockAppUi->GetFeedback() &&
            clockAppUi->GetFeedback()->TouchFeedbackSupported() )
        {
        clockAppUi->GetFeedback()->InstantFeedback( ETouchFeedbackBasic );
        }    
    }

void CClockMainView::SwitchClockTypeL()
    {
    TLocale phoneLocale;
    // Set the new clock format with the locale.
    phoneLocale.SetClockFormat( ( EClockAnalog == phoneLocale.ClockFormat() )
            ? EClockDigital : EClockAnalog );
    phoneLocale.Set();
    if( iContainer )
        {
        iContainer->SwitchClockTypeL();
        }
    }

// End of file
