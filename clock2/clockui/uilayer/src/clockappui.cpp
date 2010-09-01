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
* Description:   This is the source file for the CClockAppUi class.
*
*/

// System includes
#include <e32base.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include <bacntf.h>
#include <clock.rsg>
#include <akntitle.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include <clockapp_tab.mbg>
#include <AknsConstants.h>
#include <featmgr.h>  
#include <e32property.h>
#include <startupdomainpskeys.h>
#include <touchfeedback.h>
#include <gfxtranseffect/gfxtranseffect.h> 

// User includes
#include "clock.h"
#include "clockappui.h"
#include "clockmainview.h"
#include "clockworldview.h"
#include "clkdatetimeview.h"
#include "clkuialarmmodel.h"
#include "clockdocument.h"
#include "clock.hrh"
#include "clkdatetimeview.hrh"
#include "clock_debug.h"

// Constants
const TInt KClockNotiferPriority( CActive::EPriorityIdle );
const TInt KMaxTitleLength( 64 );
const TInt KInitialEvent( EChangesLocale |
                          EChangesMidnightCrossover |
                          EChangesThreadDeath |
                          EChangesPowerStatus |
                          EChangesSystemTime |
                          EChangesFreeMemory |
                          EChangesOutOfMemory );

// constants for control effects
const TInt KGfxControlPageSwitchDisappear = 4;
const TInt KGfxControlPageSwitchAppear = 3;

const TUid KControlUid1 = {0x2000B47C};  
const TUid KControlUid2 = {0x2000B47D};

// Literals
_LIT( KIconFileName, "\\resource\\apps\\clockapp_tab.mif" );
_LIT( KIAParamExec, "clock.exe" );

// ---------------------------------------------------------
// CClockAppUi::CClockAppUi
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockAppUi::CClockAppUi()
    {
    // No implementation yet.
    }

// ---------------------------------------------------------
// CClockAppUi::~CClockAppUi
// rest of the details are commented in the header
// ---------------------------------------------------------
//
CClockAppUi::~CClockAppUi()
    {
    // No more observing tab change events.
    if( iTabGroup )
        {
        iTabGroup->SetObserver( NULL );
        }
    if( iEnvChangeNotifier )
        {
        delete iEnvChangeNotifier;
        iEnvChangeNotifier = NULL;
        }
    if( iNavigationDecorator )
        {
        delete iNavigationDecorator;
        iNavigationDecorator = NULL;
        }
    // These should be deleted already when they are not needed any more, otherwise
       // the client-server connection is kept open.
       // These are here just in case they were not deleted earlier.
    if( iUpdate )
        {
        delete iUpdate;
        iUpdate = NULL;
        }
    if( iParameters )
        {
        delete iParameters;
        iParameters = NULL;
        }
    }

// ---------------------------------------------------------
// CClockAppUi::ConstructL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::ConstructL()
	{
	//single click integration
    BaseConstructL( EAknEnableMSK | EAknEnableSkin |
            EAknSingleClickCompatible );

	
	iIADUpdateFlag = ETrue;
	
	// Get the alarm model.
	CClkUiAlarmModel* alarmModel = static_cast< CClockDocument* > ( Document() )->AlarmModel();
	
	// Construct the view and add it to the view server stack.
	CClockMainView* clockMainView = CClockMainView::NewL( alarmModel );
	AddViewL( clockMainView );
	
	// Construct the world view and add it to the view server stack.
	CClockWorldView* clockWorldView = CClockWorldView::NewL();
	AddViewL( clockWorldView );
	
	// Construct the datetime settings view.
    CClkDateTimeView* dateTimeView = CClkDateTimeView::NewLC( KClockAppMainViewId, ETrue );
    AddViewL( dateTimeView );
    CleanupStack::Pop( dateTimeView );

    iTactileFeedbackSupported = FeatureManager::FeatureSupported( KFeatureIdTactileFeedback );
    
	// Start the model.
	alarmModel->Start();
	
	// Construct and start the environment change notifier.
	TCallBack callBack( NotifierCallbackL, this );
	iEnvChangeNotifier = CEnvironmentChangeNotifier::NewL( KClockNotiferPriority, callBack );
	if( !iEnvChangeNotifier->IsActive() )
	    {
	    iEnvChangeNotifier->Start();
	    }
	
	// Set the above view as default.
	SetDefaultViewL( *clockMainView );
	
    // Setup Navigation Pane.
    SetupNavigationPaneL();
	
	// Clock is one of the applications which is launched in the phone boot and it runs in background. If thats the case,
	// the view is not activated when it is in background and it happens when clock is opened for the first time.
	// If the app is in background, we activate the main view here to ensure faster bootup when the application is opened for the first time.
	if( !IsForeground() )
		{
		if( ExitHidesInBackground() )
			{
			// First hide application in the background.
            HideInBackground();
            // Set a flag indicating that the app is hidden.
            SetAppHidden( ETrue );
            // Prevent the control from getting displayed immediately.
            SetCustomControl( 1 );
            // Reset to initial state
            ResetToInitialStateL();
            // This allows the prevented control to be displayed after the application is
            // reopened.
            // SetCustomControl( 0 );
			}
		}
	}

// ---------------------------------------------------------
// CClockAppUi::NotifierCallbackL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TInt CClockAppUi::NotifierCallbackL( TAny* aPtr )
    {
    // Get the this object.
    CClockAppUi* self = static_cast< CClockAppUi* >( aPtr );
    
    // Get the changes in environment.
    TInt envChanges( self->iEnvChangeNotifier->Change() );
    
    if( KInitialEvent <= envChanges )
        {
        // We're not concerned about handling environment changes in that range.
        return FALSE;
        }
    
    if( envChanges & ( EChangesMidnightCrossover |
                       EChangesLocale |
                       EChangesSystemTime ) )
        {
        // We have to handle one of these changes.
        self->HandleEnvChangeL( EEnvChanged );
        }
    
    return FALSE;
    }

// ---------------------------------------------------------
// CClockAppUi::HandleCommandL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::HandleCommandL( TInt aCommandId )
    {
    switch( aCommandId )
    	{
    	case EClockSettings:
    	    {
    	    // Open datetime settings view
    	    OpenSettingsViewL();
    	    }
    	    break;
    	    
    	case EClockMainViewExit:
    	case EClockWorldExit:
    	case EClkSettExitCmd:
    	case EClockAlarmExit:
    	case EAknSoftkeyExit:
    	    {
    	    TInt deviceState;
    	    RProperty::Get( KPSUidStartup, KPSGlobalSystemState , deviceState );

    	    if(  deviceState == ESwStateCharging || deviceState == ESwStateAlarm  ) 
    	        {

                Exit();

    	        }
    	    else
                {
                if( ExitHidesInBackground() )
                    {
                    HandleCommandL( EAknCmdHideInBackground );
                    }
                else
                    {
                    Exit();
                    }
                }
    	    }
    	    break;
    	    
    	case EAknCmdHideInBackground:
    	    {
    	    // First hide application in the background.
            HideInBackground();
            // Set a flag indicating that the app is hidden.
            SetAppHidden( ETrue );
            
            iIADUpdateFlag = ETrue;
            // Prevent the control from getting displayed immediately.
            SetCustomControl( 1 );
            // Reset to initial state
            ResetToInitialStateL();

    	    }
    	    break;
    	    
    	case EEikCmdExit:
        case EAknCmdExit:
            {
            iIADUpdateFlag = ETrue;
            
            // In these two cases, we do a hard exit i.e., we close the application.
            Exit();
            }
    	
    	default:
    		{
    		// No implementation yet.
    		}
    		break;
    	}
    }

// ---------------------------------------------------------
// CClockAppUi::TabChangedL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::TabChangedL( TInt aTabIndex )
    {
    TInt activeTabId( iTabGroup->TabIdFromIndex( aTabIndex ) );
    
    // Switch on the active tab id and activate the corresponding view.
    switch( activeTabId )
        {
        case EClockAppMainViewId:
            {
            // The main view.
            iTransitionOngoing = ETrue;
            iTransitionDirection = ETransitionLeft;
            ActivateLocalViewL( KClockAppMainViewId );
            }
            break;
        
        case EClockAppWorldViewId:
            {
            // The clockworld view.
            iTransitionOngoing = ETrue;
            iTransitionDirection = ETransitionRight;
            ActivateLocalViewL( KClockAppWorldViewId );
            }
            break;
                    
        default:
            {
            // No implementaiton yet.
            }
            break;
        }
    }

// ---------------------------------------------------------
// CClockAppUi::HandleKeyEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
TKeyResponse CClockAppUi::HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse keyResponse( EKeyWasNotConsumed );
    
    if( iTabGroup )
        {
        CAknTabGroup* tabGroup = NULL;
        
        if ( iNavigationDecorator )
            {
            // Get the pointer to the tab group.
            tabGroup = ( CAknTabGroup* ) iNavigationDecorator->DecoratedControl();
            // Pass the event to the tab group so that it handles the event.
            keyResponse = tabGroup->OfferKeyEventL( aKeyEvent, aType );
            }
        }
    
    // Return the result.
    return keyResponse;
    }

// ---------------------------------------------------------
// CClockAppUi::HandleForegroundEventL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::HandleForegroundEventL( TBool aForeground )
    {
    __PRINTS( "CClockAppUi::HandleForegroundEventL - Entry" );
    
    // commented as a part of the error ESLM-83LG82.
    /*if( aForeground )
        {
        __PRINTS( "CClockAppUi::HandleForegroundEventL - aForeground ETrue" );
        
        if( iIADUpdateFlag )
            {
            __PRINTS( "CClockAppUi::HandleForegroundEventL - iIADUpdateFlag ETrue" );
            
            // check for IAD updates
            HandleIADUpdateL();
            
            __PRINTS( "CClockAppUi::HandleForegroundEventL - Checking for updates" );
            
            iIADUpdateFlag = EFalse;
            __PRINTS( "CClockAppUi::HandleForegroundEventL - iIADUpdateFlag EFalse" );
            }
        }*/

    // Inform the world clock view about the change
    CClockWorldView* clockWorldView = static_cast< CClockWorldView* > ( View( KClockAppWorldViewId ) );
    if( clockWorldView )
        {
        clockWorldView->HandleForegroundEventL( aForeground );
        }
    
    CClockMainView* clockMainView = static_cast< CClockMainView* >( View( KClockAppMainViewId ) );
    if( clockMainView )
        {
        clockMainView->HandleForegroundEventL( aForeground );
        }
		
	CAknAppUi::HandleForegroundEventL( aForeground );
    
    __PRINTS( "CClockAppUi::HandleForegroundEventL - Exit" );
    }

// ---------------------------------------------------------
// CClockAppUi::CheckUpdatesComplete
// rest of the details commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::CheckUpdatesComplete( TInt aErrorCode, TInt aAvailableUpdates )
    {
    __PRINTS( "CClockAppUi::CheckUpdatesComplete - Entry" );
    
    __PRINT( "aErrorCode: %d", aErrorCode );

    if ( aErrorCode == KErrNone )
        {
        if ( aAvailableUpdates > 0 )
            {
            __PRINT( "aAvailableUpdates: %d", aAvailableUpdates );
            // There were some updates available. Let's ask if the user wants to update them.
            iUpdate->UpdateQuery();      
            
            __PRINTS( "CClockAppUi::CheckUpdatesComplete - update available" );
            }
        else
            {
            __PRINTS( "CClockAppUi::CheckUpdatesComplete - no update available" );
			
            // No updates available. CIAUpdate object could be deleted already now, since we are not 
            // going to call anything else from the API. This will also close down the IAUpdate server.
            delete iUpdate;
            iUpdate = NULL;
            delete iParameters;
            iParameters = NULL;           
            }
        }
    
    __PRINTS( "CClockAppUi::CheckUpdatesComplete - Exit" );
    }

// ---------------------------------------------------------
// CClockAppUi::UpdateComplete
// rest of the details commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::UpdateComplete( TInt /*aErrorCode*/, CIAUpdateResult* aResult )
    {
    __PRINTS( "CClockAppUi::UpdateComplete - Entry" );
    //Comment out the aErrorCode when Prints are enabled
    __PRINT( "aErrorCode: %d", aErrorCode );

    delete aResult; // Ownership was transferred, so this must be deleted by the client

    // We do not need the client-server session anymore, let's delete the object to close the session
    delete iUpdate;
    iUpdate = NULL;
    delete iParameters;
    iParameters = NULL;
    
    __PRINTS( "CClockAppUi::UpdateComplete - Exit" );
    }

// ---------------------------------------------------------
// CClockAppUi::UpdateQueryComplete
// rest of the details commented in the header
// ---------------------------------------------------------
//    
void CClockAppUi::UpdateQueryComplete( TInt aErrorCode, TBool aUpdateNow )
    {
    __PRINTS( "CClockAppUi::UpdateQueryComplete - Entry" );
    
    __PRINT( "aErrorCode: %d", aErrorCode );
    
    if ( aErrorCode == KErrNone )
        {
        if ( aUpdateNow )
            {
            __PRINTS( "CClockAppUi::UpdateQueryComplete - aUpdateNow is true" );
            // User choosed to update now, so let's launch the IAUpdate UI.
            iUpdate->ShowUpdates( *iParameters );
            }
        else
            {
            __PRINTS( "CClockAppUi::UpdateQueryComplete - update later" );
            // The answer was 'Later'. CIAUpdate object could be deleted already now, since we are not 
            // going to call anything else from the API. This will also close down the IAUpdate server.
            delete iUpdate;
            iUpdate = NULL;
            delete iParameters;
            iParameters = NULL;                
            }
        }
    __PRINTS( "CClockAppUi::UpdateQueryComplete - Exit" );
    }

// ---------------------------------------------------------
// CClockAppUi::HandleEnvChangeL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::HandleEnvChangeL( TClockEnvChanged aEnvChanged )
    {
    // Notify the views about the change, so that they can update themselves.
    CClockMainView* mainView = static_cast< CClockMainView* > ( View( KClockAppMainViewId ) );
    mainView->HandleEnvChangeL( aEnvChanged );
    
    // Notify the views about the change, so that they can update themselves.
    CClockWorldView* clockworldView = static_cast< CClockWorldView* > ( View( KClockAppWorldViewId ) );
    clockworldView->HandleEnvChangeL( aEnvChanged );
    }

// ---------------------------------------------------------
// CClockAppUi::OpenSettingsViewL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::OpenSettingsViewL()
    {
    // Set the title pane text first
    SetTitlePaneTextL( R_CLOCK_TITLE_SETTINGS );
    
    // Activate the date and time settings view
    ActivateLocalViewL( KClkDateTimeViewId );
    
    // Don't display the navigation pane when switching to settings view.
    if( iNavigationDecorator )
        {
        iNavigationDecorator->MakeVisible( EFalse );
        }
    
    // Activate the date and time settings view
    ActivateLocalViewL( KClkDateTimeViewId );
    }

// ---------------------------------------------------------
// CClockAppUi::SetTitlePaneTextL
// rest of the details are commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::SetTitlePaneTextL( TInt aResourceId )
    {
    CEikStatusPane* statusPane = StatusPane();
    
    // Set the title pane text
    TBuf< KMaxTitleLength > titleText;
    iCoeEnv->ReadResourceL( titleText, aResourceId );
    
    CAknTitlePane* titlePane = STATIC_CAST( CAknTitlePane*, statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    titlePane->SetTextL( titleText );
    titlePane->DrawNow();
    }

// ---------------------------------------------------------
// CClockAppUi::ResetToInitialStateL
// rest of the details commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::ResetToInitialStateL()
    {
    TVwsViewId viewId;
    
    viewId.iAppUid = KClockAppUid;
    viewId.iViewUid = KClockAppMainViewId;
                        
    // Switch to the main view
    if( iTabGroup->ActiveTabId() == EClockAppWorldViewId )
        {
        ActivateLocalViewL( KClockAppMainViewId, TUid::Uid(KClockHideInBackground), KNullDesC8() );
        //TRAP_IGNORE( CAknViewAppUi::CreateActivateViewEventL( viewId, TUid::Uid(KClockHideInBackground), KNullDesC8 ) ) ;
        }
    else
        {
        // ActivateLocalViewL( KClockAppMainViewId, KClockAppMainViewId, KNullDesC8() );
        TRAP_IGNORE( CAknViewAppUi::CreateActivateViewEventL( viewId, TUid::Uid(KClockHideInBackground), KNullDesC8 ) ) ;
        }
	// Update the tab.
	iTabGroup->SetActiveTabById( EClockAppMainViewId );
    }

// ---------------------------------------------------------
// CClockAppUi::SetupNavigationPaneL
// rest of the details commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::SetupNavigationPaneL()
    {
    // Add the tabs for each view in the navigation pane.
    // Gets a pointer to the status pane.
    CEikStatusPane* statusPane = StatusPane();

    // Fetch pointer to the default navi pane control
    iNaviPane = static_cast< CAknNavigationControlContainer*>
                ( statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
                
    iNavigationDecorator = iNaviPane->CreateTabGroupL( this );
    iNavigationDecorator->SetControlType( CAknNavigationDecorator::ETabGroup );
    
    // Construct the tabgroup and append tabs to it.
    iTabGroup = ( CAknTabGroup* ) iNavigationDecorator->DecoratedControl();
    // Set observer for tab changes.
    iTabGroup->SetObserver( this );
    // Set tab width.
    iTabGroup->SetTabFixedWidthL ( EAknTabWidthWithTwoTabs);
    
    // Load the images to be displayed in the tabs.
    // 1. The alarm/main view icon.
    CFbsBitmap* tabImageBitmap( NULL );
    CFbsBitmap* tabImageMask( NULL );
    
    AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                                 KAknsIIDQgnPropClkClockTab4,
                                 KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG7,
                                 tabImageBitmap,
                                 tabImageMask,
                                 KIconFileName,
                                 EMbmClockapp_tabQgn_prop_clk_clock_tab4,
                                 EMbmClockapp_tabQgn_prop_clk_clock_tab4_mask,
                                 TRgb( NULL, NULL, 255 ) );
    
    // Add the mainview tab.
    iTabGroup->AddTabL( EClockAppMainViewId, tabImageBitmap, tabImageMask );
    
    // 2. The clock world icon.
    tabImageBitmap = NULL;
    tabImageMask = NULL;
    
    AknsUtils::CreateColorIconL( AknsUtils::SkinInstance(),
                                 KAknsIIDQgnPropClkWrldClkTab4,
                                 KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG7,
                                 tabImageBitmap,
                                 tabImageMask,
                                 KIconFileName,
                                 EMbmClockapp_tabQgn_prop_clk_wrld_clk_tab4,
                                 EMbmClockapp_tabQgn_prop_clk_wrld_clk_tab4_mask,
                                 TRgb( NULL, NULL, 255 ) );
    // Add the world view tab.
    iTabGroup->AddTabL( EClockAppWorldViewId, tabImageBitmap, tabImageMask );
    
    iNaviPane->PushL( *iNavigationDecorator );
    // Activate the main view tab.
    iTabGroup->SetActiveTabById( EClockAppMainViewId );
    }

// ---------------------------------------------------------
// CClockAppUi::SetAppHidden
// rest of the details commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::SetAppHidden( TBool aAppHidden )
    {
    iAppHidden = aAppHidden;
    }

// ---------------------------------------------------------
// CClockAppUi::IsAppExitHidden
// rest of the details commented in the header
// ---------------------------------------------------------
//  
TBool CClockAppUi::IsAppExitHidden()
    {
    return iAppHidden;
    }

// ---------------------------------------------------------
// CClockAppUi::MakeNavigationPaneVisible
// rest of the details commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::MakeNavigationPaneVisible( TBool aVisible, TInt aViewId )
    {
    if( iNavigationDecorator && iTabGroup )
        {
        iNavigationDecorator->MakeVisible( aVisible );
        
        if( aVisible )
            {
            iTabGroup->SetActiveTabById( aViewId );
            }
        }
    }

// ---------------------------------------------------------
// CClockAppUi::HandleIADUpdateL
// rest of the details commented in the header
// ---------------------------------------------------------
//
void CClockAppUi::HandleIADUpdateL()
    {
    __PRINTS( "CClockAppUi::HandleIADUpdateL - Entry" );

    if( FeatureManager::FeatureSupported( KFeatureIdIAUpdate ) )
        {
        // IAD update feature is supported.
        if( !iUpdate )
            {
            TRAP_IGNORE( iUpdate = CIAUpdate::NewL( *this ) );
            }       
        __PRINTS( "CClockAppUi::HandleIADUpdateL - iUpdate created" );

        if ( iUpdate ) // IAUpdate is supported in the phone
            {
            iParameters = CIAUpdateParameters::NewL();

            // Search for updates to IAUpdateExampleClient, We know that the SIS package
            // uid is KClockAppUid, so let's use that as a search criteria.
            iParameters->SetUid( KClockAppUid );

            __PRINT( "App Uid: %d", KClockAppUid );

            // We want clock.exe to be started after update is finished
            iParameters->SetCommandLineExecutableL( KIAParamExec );

            __PRINTS( "CClockAppUi::HandleIADUpdateL - clock.exe" );

           
            // Check the updates
            iUpdate->CheckUpdates( *iParameters );
            }
        }
    __PRINTS( "CClockAppUi::HandleIADUpdateL - Exit" );
    }
	
TBool CClockAppUi::TransitionOngoing() const
    {
    return iTransitionOngoing;
    }

void CClockAppUi::SetTransitionOngoing( TBool aTransitionOngoing )
    {
    iTransitionOngoing = aTransitionOngoing;
    }

void CClockAppUi::SetTransitionGroupId( TInt aId )
    {
    iTransitionGroupId = aId;
    }

TInt CClockAppUi::TransitionGroupId() const
    {
    return iTransitionGroupId;
    }

CClockAppUi::TTransitionDirection CClockAppUi::TransitionDirection() const
    {
    return iTransitionDirection;
    }

void  CClockAppUi::SetTransitionDirection( CClockAppUi::TTransitionDirection aDirection )
    {
    iTransitionDirection = aDirection;
    }

void  CClockAppUi::DoDisappearTransition( CCoeControl* aControl )
    {
    if( aControl && iTransitionOngoing )
        {
        if( iTransitionDirection == ETransitionLeft )
            {
            GfxTransEffect::Register( aControl, KControlUid2 );      
            }
        else
            {
            GfxTransEffect::Register( aControl, KControlUid1 );   
            }
        GfxTransEffect::Begin( aControl, KGfxControlPageSwitchDisappear );
        aControl->MakeVisible( EFalse );
        GfxTransEffect::SetDemarcation( aControl, aControl->Rect() );
        GfxTransEffect::End( aControl );
        GfxTransEffect::EndGroup( iTransitionGroupId );
        iTransitionOngoing = EFalse;
        }
    }

void  CClockAppUi::DoAppearTransition( CCoeControl* aControl )
    {
    if( aControl && iTransitionOngoing )
        {
        if( iTransitionDirection == ETransitionLeft )
            {
            GfxTransEffect::Register( aControl, KControlUid2 );      
            }
        else
            {
            GfxTransEffect::Register( aControl, KControlUid1 );   
            }
        iTransitionGroupId = GfxTransEffect::BeginGroup();
        GfxTransEffect::Begin( aControl, KGfxControlPageSwitchAppear );
        aControl->MakeVisible( ETrue );
        GfxTransEffect::SetDemarcation( aControl, aControl->Rect() );
        GfxTransEffect::End( aControl );
        }
    }

MTouchFeedback* CClockAppUi::GetFeedback()
	{
    if( !iFeedback && iTactileFeedbackSupported )
    	{
        iFeedback = MTouchFeedback::Instance();
        }
    return iFeedback;
    }
// End of file
