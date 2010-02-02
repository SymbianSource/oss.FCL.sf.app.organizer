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
// CClockMainView::RemoveAlarmL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockMainView::RemoveAlarmL()
    {
	__PRINTS( "CClockMainView::RemoveAlarmL - Entry" );
	
    // First get the index of the selected item in the list.
    TInt itemIndex( iContainer->ListBox()->CurrentItemIndex() );
    
    if( KErrNone >= itemIndex )
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
        
        // Display the confirmation note to the user about alarm deletion.
        CAknConfirmationNote* confNote = new( ELeave ) CAknConfirmationNote;
        HBufC* confNoteText = StringLoader::LoadLC( R_QTN_CLOCK_ALARM_NOTE_DELETED, iCoeEnv );
        // Display the note.
        confNote->ExecuteLD( *confNoteText );
        // Cleanup.
        CleanupStack::PopAndDestroy( confNoteText );
        
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
    
    if( ( KClockAppMainViewId == aCustomMessageId ) && iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
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
    iContainer->ActivateL();
    
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
	TRAP_IGNORE( clockDocument->StoreDataL() );
	
	if( iContainer )
		{
	
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
		    if ( iContainer->ListBox()->CurrentItemIndex() == 0 )
		        {
		        SetNewAlarmL(); 
		        }
		    else
		        {
		        ResetAlarmL(); 
		        } 
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
        	iContainer->ListBox()->SetCurrentItemIndexAndDraw( itemCount -1 );
        	
        	
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

    // There are no alarms set or the current focussed listitem is "New Alarm" then remove alarm specific options 
    if( KZeroAlarms >= alarmCount || ( KErrNone == iContainer->ListBox()->CurrentItemIndex() ) )
        {
        // There are no alarms set. Remove the irrelevant options from the menu.
        aMenuPane->DeleteMenuItem( EClockNewAlarm );
        aMenuPane->DeleteMenuItem( EClockResetAlarm );
        aMenuPane->DeleteMenuItem( EClockRemoveAlarm );
        aMenuPane->DeleteMenuItem( EClockActivateAlarm );           
        aMenuPane->DeleteMenuItem( EClockDeactivateAlarm );                                 
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
                }
            }
        else        
            {
            // Alarm is already inactive. Remove the deactivate option from the menu. 
            aMenuPane->DeleteMenuItem( EClockDeactivateAlarm );
            }
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

// End of file
