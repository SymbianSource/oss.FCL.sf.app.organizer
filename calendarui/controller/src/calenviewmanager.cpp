/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Calendar view manager
*
*/


#include <aknViewAppUi.h>
#include <centralrepository.h>
#include <AknQueryDialog.h>
#include <Calendar.rsg>
#include <calencommonui.rsg>
#include <akntoolbar.h>
#include <calendateutils.h>
#include <calencommandhandler.h>
#include <calencommands.hrh>                // Calendar commands
#include <calentoolbar.h>
#include <akntoolbarextension.h>
#include <calenactionuiutils.h>

#include "calendarui_debug.h"
#include "calenviewmanager.h"
#include "CalenUid.h"
#include "calencontroller.h"
#include "calenviewpopulator.h"
#include "calenglobaldata.h"
#include "CalendarPrivateCRKeys.h"          // includes CalendarInternalCRKeys.h
#include "calenmonthview.h"                 // Native month view
#include "calenweekview.h"                  // Native week view
#include "calendayview.h"                   // Native day view
#include "calentodoview.h"                  // Native todo view
#include "caleneventview.h"                 // Event View
#include "calenmissedalarmsview.h"			// Missed alarms view
#include "calenmissedeventview.h"			// Missed event view
#include "calensetting.h"                   // CCalenSetting::TViewType
#include "calencmdlinelauncher.h"           // Command line launcher
#include "calenservicesimpl.h"
#include "CleanupResetAndDestroy.h"
#include "calentoolbarimpl.h"
#include "calencustomisationmanager.h"
#include "calenviewinfo.h"
#include "calentitlepane.h"
#include "calenicons.h"
#include "calendummyview.h"

const TInt KArrayGranularity = 5;

// ----------------------------------------------------------------------------
// CCalenViewManager::NewL
// 1st phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenViewManager* CCalenViewManager::NewL( CAknViewAppUi& aAppUi,
                                            CCalenController& aController )
    {
    TRACE_ENTRY_POINT;

    CCalenViewManager* self = new( ELeave ) CCalenViewManager( aAppUi,aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::CCalenViewManager
// C++ default Constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenViewManager::CCalenViewManager( CAknViewAppUi& aAppUi, 
                                      CCalenController& aController )
    : iAppUi( aAppUi ), iController( aController )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::~CCalenViewManager
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenViewManager::~CCalenViewManager()
    {
    TRACE_ENTRY_POINT;

    delete iPopulator;
    delete iToolbar;

    if( iSetting )
        {
        iSetting->Release();
        }

    if( iGlobalData )
        {
        iGlobalData->Release();
        }

    iViewInfoArray.ResetAndDestroy();
    iKnownPlugins.Reset();
    
    delete iRemovedActiveView;
    delete iViewRemovalCallback;
    delete iIcons;

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::ConstructL
// 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::ConstructL()
    {
    TRACE_ENTRY_POINT;

    iGlobalData = CCalenGlobalData::InstanceL();
    iPopulator = CCalenViewPopulator::NewL( iController );

    // Only create a toolbar impl if touch is enabled and a CAknToolbar exists
    if( AknLayoutUtils::PenEnabled() )
        {
        CAknAppUi* appUi = static_cast<CAknAppUi*>( CEikonEnv::Static()->EikAppUi() );
        if (appUi->CurrentFixedToolbar())
            {
            iToolbar = CCalenToolbarImpl::NewL( iController );
            }
        }

    iSetting = CCalenSetting::InstanceL();
    iPreviousViewId.iViewUid = KNullUid;
    iAvoidRepopulation = EFalse;
    iStartupComplete = ETrue;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::SetCustomisationManagerL
// Creates custom views.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::ConstructCustomViewsL(
                             CCalenCustomisationManager& aCustomisationManager )
    {
    TRACE_ENTRY_POINT;

    iCustomisationManager = &aCustomisationManager;
    ConstructCustomViewsL();
    ConstructNativeViewsL();

    TUid defViewUid = iSetting->DefaultView();
    ActivateDefaultViewL( defViewUid );
    	
    // Register for view activation events
    iAppUi.AddViewActivationObserverL( this );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::ViewInfoArray
// Get info array
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
RPointerArray<CCalenViewInfo>& CCalenViewManager::ViewInfoArray()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iViewInfoArray;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::CustomiszationManager
// Return reference to the customisation manager
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenCustomisationManager& CCalenViewManager::CustomisationManager()
    {
    TRACE_ENTRY_POINT;

    ASSERT( iCustomisationManager );

    TRACE_EXIT_POINT;
    return *iCustomisationManager;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::ConstructNativeViewsL
// Constructs the S60 native views and registers them with the view server
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::ConstructNativeViewsL()
    {
    TRACE_ENTRY_POINT;

    CCalenTitlePane::NewAndSwapL(iAppUi.StatusPane());
    MCalenServices& services = iController.Services();
    
    
    // Add the native views for the todo view, week view, day view , month
    // view and event view unless they are already provided by custom views 
    // in ROM.This is enforced by the customisation manager rejecting any 
    // views that try to replace the native views but are not in ROM.

    if (CreateNativeViewL(KUidCalenMonthView))
        {
        // Views take ownership of services object instantly. 
        // No need for cleanup stack.
        CCalenMonthView* monthView = CCalenMonthView::NewL(services);
        AddNativeViewL(monthView); // takes ownership immediately.
        }

    if (CreateNativeViewL(KUidCalenWeekView))
        {
        // Views take ownership of services object instantly. 
        // No need for cleanup stack.
        CCalenWeekView* weekView = CCalenWeekView::NewL(services);
        AddNativeViewL(weekView); // takes ownership immediately.
        }
        
    if (CreateNativeViewL(KUidCalenDayView))
        {  
        // Views take ownership of services object instantly. 
        // No need for cleanup stack.
        CCalenDayView* dayView = CCalenDayView::NewL(services);
        AddNativeViewL(dayView); // takes ownership immediately.
        }
    
    if (CreateNativeViewL(KUidCalenTodoView))
        {
        // Views take ownership of services object instantly. 
        // No need for cleanup stack.
        CCalenTodoView* todoView = CCalenTodoView::NewL(services);
        AddNativeViewL(todoView); // takes ownership immediately.
        }
	
	if( CreateNativeViewL(KUidCalenEventView))
		{
        // Views take ownership of services object instantly. 
		// No need for cleanup stack.
        CCalenEventView* eventView = CCalenEventView::NewL(services);
        AddNativeViewL(eventView); // takes ownership immediately.
		}

	if( CreateNativeViewL(KUidCalenMissedAlarmsView))
		{
        // Views take ownership of services object instantly. 
		// No need for cleanup stack.
        CCalenMissedAlarmsView* missedAlarmsView = CCalenMissedAlarmsView::NewL(services);
        AddNativeViewL(missedAlarmsView); // takes ownership immediately.
		}

	if( CreateNativeViewL(KUidCalenMissedEventView))
		{
        // Views take ownership of services object instantly. 
		// No need for cleanup stack.
        CCalenMissedEventView* missedEventView = CCalenMissedEventView::NewL(services);
        AddNativeViewL(missedEventView); // takes ownership immediately.
		}
	
	if(CreateNativeViewL(KUidCalenDummyView))
	    {
	    CCalenDummyView* dummyView = CCalenDummyView::NewL(services);
	    AddNativeViewL(dummyView);
	    }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::CreateNativeViewL
// Checks to see if the native view needs to created.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenViewManager::CreateNativeViewL(TUid aViewUid)
    {
    TRACE_ENTRY_POINT;
    
    TBool createNativeView = ETrue;
    TBool (*compareFn)(const TUid*, const CCalenViewInfo&) 
                                         = CCalenViewInfo::ViewInfoIdentifier;
                                            
    TInt position = iViewInfoArray.Find( aViewUid, compareFn );
    if ( position != KErrNotFound )
        {
        // A plugin is trying to replace a native view, find if it is rom
        // based or not
        TBool romBased = CustomisationManager().IsViewRomBased( aViewUid );
        if ( romBased )
            {
            createNativeView = EFalse;
            }
        else
            {
            // A non-rom plugin cannot replace the native views, so the view is
            // removed
            iAppUi.RemoveView( aViewUid );
            
            // Remove the entry from the view info array
            iViewInfoArray.Remove( position );
            
            createNativeView = ETrue;
            }
        }
        
    TRACE_EXIT_POINT;
    return createNativeView;
    }
 
// ----------------------------------------------------------------------------
// CCalenViewManager::ConstructCustomViewsL
// Constructs any custom views from all the plugins and registers them 
// with the view server
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::ConstructCustomViewsL()
    {
    TRACE_ENTRY_POINT;

    // Get the array of active plugins
    const RArray<TUid>& plugins = CustomisationManager().ActivePlugins();

    // For every active plugin, discover if it offers any customised views
    // and adds them to the view server.
    TInt numPlugins = plugins.Count();
    for( TInt pluginIndex( 0 ); pluginIndex < numPlugins; ++pluginIndex )
        {
        TUid pluginUid = plugins[pluginIndex];
        ConstructCustomViewL( pluginUid );
        }

    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenViewManager::ConstructCustomViewsL
// Constructs the custom views from a particular plugin and registers them
//  with the view server
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::ConstructCustomViewL( TUid aPluginUid )
    {
    TRACE_ENTRY_POINT;

    iKnownPlugins.AppendL( aPluginUid );

    RPointerArray<CCalenView> customViews;
    CleanupResetAndDestroyPushL( customViews );

    CustomisationManager().GetCustomViewsL( aPluginUid, customViews );
    for( TInt viewIndex( customViews.Count()-1 ); viewIndex >= 0; --viewIndex )
        {
        CCalenView* customView = customViews[viewIndex];
        iAppUi.AddViewL( customView );
        customViews.Remove( viewIndex );
        }
    CleanupStack::PopAndDestroy(); // customViews

    TRACE_EXIT_POINT;
    }    

// ----------------------------------------------------------------------------
// CCalenViewManager::ActivateDefaultViewL
// Activates the default view (retrieved from settings).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::ActivateDefaultViewL( TUid aDefaultView )
    {
    TRACE_ENTRY_POINT;

    // Find the default view in the view cycle list
    TInt position = iViewInfoArray.Find(
                            aDefaultView, CCalenViewInfo::ViewInfoIdentifier );
    if( position != KErrNotFound )
        {
        iAppUi.SetDefaultViewL( *iAppUi.View( aDefaultView ) );
        iViewCycleIndex = position;
        }
	else
	    {
	    CRepository* repository = CRepository::NewL( KCRUidCalendar );
	    CleanupStack::PushL(repository);
	    TInt tmp( static_cast<TInt>( KUidCalenMonthView.iUid ) );
	    TInt position = iViewInfoArray.Find( KUidCalenMonthView, CCalenViewInfo::ViewInfoIdentifier );
	    User::LeaveIfError( repository->Set( KCalendarDefaultStartView, tmp ) );
	    iAppUi.SetDefaultViewL( *iAppUi.View( KUidCalenMonthView ) );
	    iViewCycleIndex = position;
	    CleanupStack::PopAndDestroy(repository);  
	    }
    TRACE_EXIT_POINT;

    }

// ----------------------------------------------------------------------------
// CCalenViewManager::InterruptPopulationL
// Interrupts the population of the current view. When the editors are launched
// the view population is interrupted as it will be repopulated when the
// the editor closes.
// ----------------------------------------------------------------------------
//
void CCalenViewManager::InterruptPopulationL()
    {
    TRACE_ENTRY_POINT;

    // Cancel population of current view, if it's ongoing.
    iPopulator->InterruptPopulationL();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::RemoveCurrentViewFromMenu
// Removes the current view from the cascading view switch menu
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::RemoveCurrentViewFromMenu( CEikMenuPane* aMenuPane )
    {
    TRACE_ENTRY_POINT;

    TUid uid = CurrentView();

    if( uid == KUidCalenMonthView )
        {
        aMenuPane->DeleteMenuItem( ECalenMonthView );
        }
    else if( uid == KUidCalenWeekView )
        {
        aMenuPane->DeleteMenuItem( ECalenWeekView );
        }
    else if( uid == KUidCalenDayView )
        {
        aMenuPane->DeleteMenuItem( ECalenDayView );
        }
    else if( uid == KUidCalenTodoView )
        {
        aMenuPane->DeleteMenuItem( ECalenTodoView );
        }
    else
        {
        // Assert as this point should never be reached
        ASSERT( 0 );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::AddNativeViewL
// Adds a view to the array.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::AddNativeViewL( CCalenView* aView )
    {
    TRACE_ENTRY_POINT;

    CleanupStack::PushL( aView );
    iAppUi.AddViewL( aView );
    CleanupStack::Pop( aView );

    const TDesC& menuName = aView->LocalisedViewNameL( CCalenView::EMenuName );
    const TDesC& settingsName = aView->LocalisedViewNameL( CCalenView::ESettingsName );

    TUid viewUid = aView->Id();
    

    CCalenViewInfo* viewInfo = CCalenViewInfo::NewL( aView->Id(),
                                                     KUidCalendar,
                                                     menuName,
                                                     settingsName,
                                                     aView->CyclePosition() );
        
    // Discover if a native view has been hidden by a plugin.                                             
    TBool hidden = iCustomisationManager->HiddenView( viewUid );
    viewInfo->Hide( hidden );
    
    // Append to view info array
    iViewInfoArray.InsertInOrderAllowRepeatsL( viewInfo, 
                                       CCalenViewInfo::CyclePositionComparison );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::HandleCommandL
// Handles view manager commands.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool  CCalenViewManager::HandleCommandL( const TCalenCommand& aCommand )
    {
    TRACE_ENTRY_POINT;
    
    TBool commandUsed(EFalse);
    
    switch( aCommand.Command() )
        {
        case ECalenMonthView:
            {
            RequestActivationL( KUidCalenMonthView );
            }
            break;
        case ECalenWeekView:
            {
            RequestActivationL( KUidCalenWeekView );
            }
            break;
        case ECalenDayView:
            {
            // reset the flag iForwardedToDayView as view switching is active
            if(iPreviousToDayView.iViewUid!=KNullUid)
                {
                iPreviousToDayView.iViewUid = KNullUid;
                }
            RequestActivationL( KUidCalenDayView, KCalenDummyUid, KNullDesC8() );
            if(iController.IsLaunchFromExternalApp())
                {
                iAvoidRepopulation = ETrue;
                }
            }
            break;
        case ECalenTodoView:
            {
            if (iAvoidRepopulation)
                {
                iAvoidRepopulation = EFalse;
                }
            RequestActivationL( KUidCalenTodoView );
            }
            break;
        case ECalenEventView:
            {
            // Fix for EJCU-7LKC2C :: to prevent the display of blank view, 
            // just set the iAvoidRepopulation to EFalse, 
            // so that view is populated properly

            if (iAvoidRepopulation)
                {
                iAvoidRepopulation = EFalse;
                }

        	RequestActivationL( KUidCalenEventView );
            }
        	break;
        case ECalenForwardsToDayView:
            {
            // set the view iPreviousToDayView to handle the day view's cba
            // when returning from event view.
            // From month/week view -> day view -> event view -> day view
            iPreviousToDayView = iCurrentViewId;
            RequestActivationL( KUidCalenDayView, KUidCalenShowBackCba );
            }
            break;
        case ECalenNextView:
            {
            CycleNextViewL();
            }
            break;
        case ECalenPrevView:
            {
            CyclePrevViewL();
            }
            break;
        case ECalenSwitchView:
            {
            ShowSwitchViewQueryL();
            }
            break;
        case ECalenStartActiveStep:
            {
            StartActiveStepL();
            }
            break;
        case ECalenFasterAppExit:
        	{
        	HandleFasterAppExitCommandL();
        	}
            break;
        case ECalenGotoToday:
            {
            // get today's date
            TTime today = CalenDateUtils::Today();

            // get the context    
            MCalenContext& context = iController.Services().Context();
            TCalTime todayCalTime;
            todayCalTime.SetTimeLocalL( today );
            // set today's date to the context
            context.SetFocusDateAndTimeL( todayCalTime, iCurrentViewId  );
            
            SetRepopulation(EFalse);
            // reactivate the current view
            RequestActivationL(iCurrentViewId.iViewUid);

			// dim "today" toolbar item since focus is on today            
            iToolbar->Toolbar().SetItemDimmed( ECalenGotoToday, ETrue, ETrue);
            }
            break;

        default:
            break;
        }

    TRACE_EXIT_POINT;
    return commandUsed;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::CalenCommandHandlerExtensionL
// Dummy implementation.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TAny* CCalenViewManager::CalenCommandHandlerExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::CurrentView
// Returns the current view's view uid.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TUid CCalenViewManager::CurrentView() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iCurrentViewId.iViewUid;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::HandleViewActivation
// From MCoeViewActivationObserver
// Called just before a view in this application is activated by the view server
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::HandleViewActivation( const TVwsViewId& aNewlyActivatedViewId,
                                             const TVwsViewId& aViewIdToBeDeactivated )
    {
    TRACE_ENTRY_POINT;

    TBool externalViewSwitch( EFalse );
    // See if this is an internal or external view switch request
    if( aViewIdToBeDeactivated.iAppUid != KUidCalendar )
        {
        externalViewSwitch = ETrue;
        }

    // If this is an internal view switch (view cycling from the '*' key)
    // then ActivateLocalViewL will handle any necessary state changes.
    // If this is an external view switch then we need to find out if Calendar was already
    // running or if it is being launched to a specific view by another application
    // calling ActivateViewL with Calendars UID and the UID of one of the standard views.
    // In this case we need to set the current view in the state or we would get the
    // default view (from the settings) activated instead of the requested view.
    if( externalViewSwitch )
        {
        // Check that the view being activated belongs to Calendar.  Although I don't see
        // how this would be called if this was false anyway.
        if( aNewlyActivatedViewId.iAppUid == KUidCalendar )
            {
            // Make sure that any when any open dialogs are closed Calendar will not close as well.
            // This could be true if Calendar was originally launched directly to the editor, and
            // so should be closed when the dialog is closed.  If an external view switch request
            // arrives, we want the dialog to close but Calendar to remain open.
            // Otherwise we get a CONE 44 panic.
            //iIsExitOnDlgClose = EFalse;

            iCurrentViewId = aNewlyActivatedViewId;
            }
        }

    iViewsActivated = ETrue;
    
    // check for iAvoidRepopulation to avoid repopulation whenever
    // 1) Application comes to foreground
    // 2) Applictaion is opened after fake exit
    if(!iAvoidRepopulation || iController.IsLaunchFromExternalApp() )
        {
        TRAPD(error,StartActiveStepL());
        if(error!=KErrNone)
            {
            // do avoid warning
            }
        }
    
    // Reset the flag iAvoidRepopulation
    if (iAvoidRepopulation)
        {
        iAvoidRepopulation = EFalse;
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::RemoveDeActivatedView
// Asyncronous callback function to remove the current view after it has been 
// disabled.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CCalenViewManager::RemoveDeActivatedView( TAny* aObject )
    {
    TRACE_ENTRY_POINT;
    
    CCalenViewManager* thisPtr( static_cast<CCalenViewManager*>( aObject ) );
    TRAP_IGNORE( thisPtr->RemoveDeActivatedViewL() );
    
    TRACE_EXIT_POINT;
    return 0;
    }
 
// ----------------------------------------------------------------------------
// CCalenViewManager::RemoveDeActivatedViewL.
// (Leaving version)
// Asyncronous callback function to remove the current view after it has been 
// disabled.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//   
void CCalenViewManager::RemoveDeActivatedViewL()
    {
    TRACE_ENTRY_POINT;
    
    TUid removedViewUid = iRemovedActiveView->ViewUid();
    TUid removedPluginUid = iRemovedActiveView->PluginUid();
    
    // The view isn't the current view, so we
    // can remove it directly from the view server
    iAppUi.RemoveView( removedViewUid );

    // Delete the view info
    delete iRemovedActiveView;;
    iRemovedActiveView = NULL;
    
    // Unload the plugin.
    RArray<TUid> pluginArray;
    CleanupClosePushL( pluginArray );
    
    pluginArray.AppendL( removedPluginUid );
    iCustomisationManager->UnloadPluginsL( pluginArray );
    CleanupStack::PopAndDestroy(); // pluginArray
    
    if(!iController.IsFasterAppFlagEnabled())
        {
        // Refresh the current view by simulating a settings close
        iController.BroadcastNotification( ECalenNotifySettingsClosed );
        }
    else
        {
        iController.BroadcastNotification( ECalenNotifyCheckPluginUnloading );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::CycleNextViewL
// Requests activation for the next view in the view cycle
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::CycleNextViewL()
    {
    TRACE_ENTRY_POINT;

    // Increment until we get to a valid view.
    do{
        ++iViewCycleIndex;

        if( iViewCycleIndex >= iViewInfoArray.Count() )
            {
            iViewCycleIndex = 0;
            }
        }
    while( iViewInfoArray[iViewCycleIndex]->CyclePosition() == CCalenView::ENoCyclePosition );

    RequestActivationL( iViewInfoArray[iViewCycleIndex]->ViewUid() );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::CyclePrevViewL
// Requests activation for the previous view in the view cycle
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::CyclePrevViewL()
    {
    TRACE_ENTRY_POINT;

    // Decrement until we get to a valid view.
    do{
        --iViewCycleIndex;

        if( iViewCycleIndex < 0 )
            {
            iViewCycleIndex = iViewInfoArray.Count()-1;
            }
        }
    while ( iViewInfoArray[iViewCycleIndex]->CyclePosition() == CCalenView::ENoCyclePosition );

    RequestActivationL( iViewInfoArray[iViewCycleIndex]->ViewUid() );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::RequestActivationL
// Request activation of a specific view.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::RequestActivationL( const TVwsViewId& aViewId )
    {
    TRACE_ENTRY_POINT;
    
    RequestActivationL( aViewId.iViewUid );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::RequestActivationL
// Call this to try to activate a view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::RequestActivationL( const TUid& aViewUid,
                                            const TUid& aMessageId,
                                            const TDesC8& aMessage )
    {
    TRACE_ENTRY_POINT;

    // Cancel population of current view, if it's ongoing.
    iPopulator->InterruptPopulationL();
    // We start population of the newly activated view in HandleViewActivationL.
    
    // cache the previousviewid as we are getting aViewUid as reference.
    TUid cachePreviousViewId = iCurrentViewId.iViewUid;
    
    iCurrentViewId.iViewUid = aViewUid;
    // Update the view cycle index as iViewInfoArray would have changed
	iViewCycleIndex = iViewInfoArray.Find( iCurrentViewId.iViewUid, 
                                            CCalenViewInfo::ViewInfoIdentifier );
    
	if( iController.IsFasterAppFlagEnabled() )
		{
	    // Leave the application in background
	    iAppUi.HideInBackground();
	    // Disable bring-to-foreground on view activation
		iAppUi.SetCustomControl(1); 
		
		// activate the view
		iAppUi.ActivateLocalViewL( aViewUid, KCalenHideInBackGround, KNullDesC8() );
	
		// Enable bring-to-foreground on view activation.
		iAppUi.SetCustomControl(0);  
		}
	else
		{
		iAppUi.ActivateLocalViewL( aViewUid, aMessageId, aMessage );
		}
	
	// set the previous view id
	if(cachePreviousViewId != KUidCalenEventView) 
	    {
        iPreviousViewId.iViewUid = cachePreviousViewId; 
	    }	
	TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::StartActiveStepL
// Starts population of the current view.
// ----------------------------------------------------------------------------
//
void CCalenViewManager::StartActiveStepL()
    {
    TRACE_ENTRY_POINT;
    
    // check for current viewid
    // populate the view only if iCurrentViewId is set
    if(iCurrentViewId.iViewUid != KNullUid)
        {
        iPopulator->InterruptPopulationL();
        iPopulator->BeginPopulationL(reinterpret_cast<CCalenView*>(iAppUi.View(CurrentView())));
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::ShowSwitchViewQueryL
// Prompts the user to chose a view to switch to. If cancelled, returns
// KErrCancel, otherwise one of ECalen*view.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::ShowSwitchViewQueryL()
    {
    TRACE_ENTRY_POINT;

    // Create list of view names and uids
    CDesCArrayFlat* viewNames = new( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( viewNames );

    RArray<TUid> viewUids;
    CleanupClosePushL( viewUids );
											
	const RArray<TUid>& activePlugins = CustomisationManager().ActivePlugins();											
											
    for( TInt index( 0 ); index < iViewInfoArray.Count(); ++index )
        {
        CCalenViewInfo& viewInfo = *( iViewInfoArray[index] );
        if( index != iViewCycleIndex )
            {
            TUid pluginUid = viewInfo.PluginUid();
            TInt position = activePlugins.Find( pluginUid );
	        	
            // If the view is from plugin or native view with cycle position,
            // then add it to the switch view list
            if(( position != KErrNotFound ) ||
                 ( viewInfo.CyclePosition() != CCalenView::ENoCyclePosition ))
                {
                // Fetch the view name
                TUid viewUid = viewInfo.ViewUid();
                const TDesC& viewName = viewInfo.MenuName();
                viewNames->AppendL( viewName );
                viewUids.AppendL(viewUid );      
                }
            }
        }
        
    // Show list query.
    TInt choice = KErrCancel; //KErrNotFound;
    CAknListQueryDialog* dlg = new( ELeave ) CAknListQueryDialog( &choice );
    dlg->PrepareLC( R_CALENDAR_SWITCH_VIEW_QUERY ); // pushes dlg to CS
    dlg->SetItemTextArray( viewNames );
    dlg->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // Set title
    CAknPopupHeadingPane* heading = dlg->Heading();
    HBufC* title = NULL;
    title = CCoeEnv::Static()->AllocReadResourceLC( R_CALENDAR_SWITCH_VIEW_QUERY_TITLE );
    heading->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );

    if( dlg->RunLD() )
        {
        // user made a choice
        TUid viewUid = viewUids[choice];
        RequestActivationL( viewUid );
        }

    CleanupStack::PopAndDestroy(); // viewUids
    CleanupStack::PopAndDestroy( viewNames );

    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenViewManager::HandleNotification
// Calls back when notifications that it has been registered for are broadcast
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::HandleNotification(const TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    PIM_TRAPD_HANDLE( HandleNotificationL( aNotification ) );
  
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenViewManager::HandleNotificationL
// Called from HandleNotification() when notifications that it has been
//  registered for are broadcast
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::HandleNotificationL( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    switch( aNotification )
        {
        case ECalenNotifyPluginEnabledDisabled:
            {
            UpdatePluginListL();
            }
            break;
        case ECalenNotifySettingsClosed:
            {
            // Nothing has changed, refresh statuspane only.
            iAppUi.StatusPane()->DrawNow();
            }
            break;
        case ECalenNotifySettingsChanged:
            {
            HandleSettingsChangeNotificationL();
            }
            break;       
        case ECalenNotifyEntryDeleted:
        case ECalenNotifyInstanceDeleted:    
			{
			HandleEntryDeleteNotificationL();
			}
			break;
        case ECalenNotifySystemLocaleChanged:	
        case ECalenNotifyEntrySaved:
        case ECalenNotifyMultipleEntriesDeleted:
        case ECalenNotifyExternalDatabaseChanged:
        case ECalenNotifyCancelDelete:
        case ECalenNotifyCalendarFieldChanged:
        case ECalenNotifyMarkedEntryCompleted:
        case ECalenNotifyCalendarInfoCreated:
        case ECalenNotifyCalendarInfoUpdated:
            {
            StartActiveStepL();
            }
            break;
        case ECalenNotifyCalendarFileDeleted:
            {
            if(iCurrentViewId.iViewUid==KUidCalenEventView)
                {
                //If the entry which is being viewed belongs to a calendar 
                //that is deleted we check for collection ids of entry and 
                //calendar session if they are same return to previous view
                TPtrC calFileName = iGlobalData->Context().GetCalendarFileNameL();
                TPtrC calFileNameForColId = 
                        iGlobalData->GetCalFileNameForCollectionId(iGlobalData->Context().InstanceId().iColId);
                if(!calFileNameForColId.CompareF(calFileName))
                    {
                    ActivateDefaultViewL(iPreviousToDayView.iViewUid);
                    }
                }
            else
                {
                // refresh the current view
                StartActiveStepL();
                }
            }
            break;         
        case ECalenNotifyEntryClosed:
            {
            HandleEntryClosedNotificationL();
            }
            break;
        case ECalenNotifySystemTimeChanged:
            {
            HandleSystemTimeChangeNotificationL();
            }
            break;
        case ECalenNotifyAppForegrounded:
        	{
        	// check for system time change whenever fake exit is done
        	// or application comes to foreground
            if(!iStartupComplete)
                {
                iController.CheckSystemTimeAtStartUpL();
                }
        	
            if( iController.IsFasterAppFlagEnabled() )
			    {
			    iAppUi.HideApplicationFromFSW(EFalse);
			    iController.SetFasterAppFlag( EFalse );
			    
			    ReloadAllPluginsL();
			    }
        	}
        	break;
        case ECalenNotifyDayViewClosed:
            {
            if(iPreviousToDayView.iViewUid!= KNullUid)
                {
                // activate the previous view from where day view is launched
                // From month/week view -> day view
                RequestActivationL(iPreviousToDayView.iViewUid);
                }
            }
            break;
        case ECalenNotifyAppBackgrounded:
            {
            // set the flag iAvoidRepopulation to prevent repopulation
            // whenever application is brought to foreground
            iAvoidRepopulation = ETrue;
            
            if( iController.IsFasterAppFlagEnabled() )
                {
                iController.RemoveDeadCalendarsL();
                }
            }
            break;
        case ECalenNotifyViewPopulationComplete:
            {
            if(iStartupComplete)
                {
                iController.CheckSystemTimeAtStartUpL();
                iStartupComplete = EFalse;
                }
            break;
            }
        case ECalenNotifyAttachmentViewerClosed:
        		{
            if( iCurrentViewId.iViewUid==KUidCalenEventView)
                {
                StartActiveStepL();
                }
            }            
        case ECalenNotifyAttachmentRemoved:
        case ECalenNotifyAttachmentAdded:    
            {
            if( iCurrentViewId.iViewUid==KUidCalenEventView && 
                !iAvoidRepopulation    )
                {
                StartActiveStepL();
                }
            }
        case ECalenNotifyDeleteFailed:
        // Do nothing on deletion failed
        default:
            break; 
        }

    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenViewManager::UpdatePluginListL
// Discovers if a plugin has been enabled or disabled 
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::UpdatePluginListL()
    {
    TRACE_ENTRY_POINT;
    
    const RArray<TUid>& activePlugins = CustomisationManager().ActivePlugins();
    
    // Check for any new plugins
    TInt activeCount = activePlugins.Count();
    for( TInt index( 0 ); index < activeCount; ++index )
        {
        // For every active plugin, check to see if it is in the known list,
        // if it isn't construct any custom views.
        TUid pluginUid = activePlugins[index];
        TInt position = iKnownPlugins.Find( pluginUid );
        if( position == KErrNotFound )
            {
            ConstructCustomViewL( pluginUid );
            }
        }

    RArray<TUid> disabledPlugins;
    CleanupClosePushL( disabledPlugins );
        
    // Check for any disabled plugins
    TInt knownCount = iKnownPlugins.Count();
    for( TInt index( knownCount - 1 ); index >= 0; --index )
        {
        // For every known plugin, check to see if it is in the active list,
        // if it isn't add to the disable plugin list, and remove from the
        // known list.
        TUid pluginUid = iKnownPlugins[index];
        TInt position = activePlugins.Find( pluginUid );
        if ( position == KErrNotFound )
            {
            disabledPlugins.AppendL( pluginUid );
            iKnownPlugins.Remove( index );
            }
        }
    
    TInt disabledPluginCount = disabledPlugins.Count();
    if( disabledPluginCount != 0 )
        {
        RemoveDisabledPluginsViewsL( disabledPlugins );
        // Re sort the view info array
        iViewInfoArray.Sort(  CCalenViewInfo::CyclePositionComparison );
        }
    else
        {      
        // Update the view cycle index as iViewInfoArray would have changed
        iViewCycleIndex = iViewInfoArray.Find( iCurrentViewId.iViewUid, CCalenViewInfo::ViewInfoIdentifier );       
        }
    UpdateToolbarNextViewIconL(iCurrentViewId.iViewUid);
    
    CleanupStack::PopAndDestroy(); // disabledPlugins
    TRACE_EXIT_POINT;
    } 
    
// ----------------------------------------------------------------------------
// CCalenViewManager::RemoveDisabledPluginsViewsL
// Removes any custom views provided by disabled plugins
// from the view server.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenViewManager::RemoveDisabledPluginsViewsL( RArray<TUid>& aDisabledPlugins )
    {
    TRACE_ENTRY_POINT;

    // Find what views are provided by the disabled plugins
    TInt disabledPluginCount = aDisabledPlugins.Count();
    RArray<TUid> removedViews;
    CleanupClosePushL( removedViews );
    
    TUid activeViewUid = CurrentView();
    
    for( TInt index( disabledPluginCount - 1 ); index >= 0; --index )
        {
        TUid pluginUid = aDisabledPlugins[index];
        
        // Does this plugin offer any views
        TInt position = iViewInfoArray.Find( pluginUid, 
                                             CCalenViewInfo::ViewPluginIdentifier );
        TBool alreadySet = EFalse;
        while( position != KErrNotFound )
            {
            CCalenViewInfo* view = iViewInfoArray[position];
            TUid viewUid = view->ViewUid();
            
            // Remove from the view cycle list
            iViewInfoArray.Remove( position );
            
            RArray<TInt> hiddenViews;
            iCustomisationManager->GetHiddenViewIdL(pluginUid, hiddenViews);
            
            if( viewUid == activeViewUid )
                {
                // Removing the active view is done in three parts
                // 1) It is removed from the viewInfo list
                // 2) When settings is closed, the view is switched
                // to the next view in the view cycle list.
                // 3) When the view is deactivated it can be deleted.
                
                // Store the information about the current view.
                iRemovedActiveView = view;
                
                // Update the view cycle index so that hidden view is launched while cycling next view
                if(hiddenViews.Count())
                    {
                    // Find the index of teh hidden view
                    TInt viewInfoIndex = iViewInfoArray.Find(TUid::Uid(hiddenViews[0]), CCalenViewInfo::ViewInfoIdentifier);
                    iViewCycleIndex = viewInfoIndex - 1;  // Decrementing it as CycleNextView() funciton will increment it   
                    }
                else if(!alreadySet)
                    {
                    TInt nextViewCycleIndex = iViewCycleIndex - 1; // To update the view cycle index in plugin disable case
            
                    // Update the view cycle index as iViewInfoArray would have changed
                    iViewCycleIndex = iViewInfoArray.Find( iCurrentViewId.iViewUid, CCalenViewInfo::ViewInfoIdentifier ); 
                    
                    // If the current active view has been removed,
                    if(iViewCycleIndex == -1)
                        {
                            iViewCycleIndex = nextViewCycleIndex;
                        } 
                    alreadySet = ETrue;
                    }
                
                hiddenViews.Reset();
                // Remove the plugin from the disabled plugin list
                // to stop the plugin being deleted.
                aDisabledPlugins.Remove( index );
                }
            else
                {
                // The view isn't the current view, so we
                // can remove it directly from the view server
                iAppUi.RemoveView( viewUid );

                // Delete the view info
                delete view;
                if(!alreadySet)
                    {
	                TInt nextViewCycleIndex = iViewCycleIndex - 1; // To update the view cycle index in plugin disable case
                        
	                // Update the view cycle index as iViewInfoArray would have changed
	                iViewCycleIndex = iViewInfoArray.Find( iCurrentViewId.iViewUid, CCalenViewInfo::ViewInfoIdentifier ); 
                
	                // If the current active view has been removed,
	                if(iViewCycleIndex == -1)
	                    {
	                        iViewCycleIndex = nextViewCycleIndex;
	                    }
	                alreadySet = ETrue;
                    }
               
                }
                
            position = iViewInfoArray.Find( pluginUid, 
                                        CCalenViewInfo::ViewPluginIdentifier );
            }
        }
        
    // Unload the disabled plugins
    iCustomisationManager->UnloadPluginsL( aDisabledPlugins );
        
    CleanupStack::PopAndDestroy(); // removedViews
    
    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenViewManager::ToolbarOrNull
// Provides access to the calendar toolbar if one is available
// ----------------------------------------------------------------------------
MCalenToolbar* CCalenViewManager::ToolbarOrNull()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    if (iToolbar)
        {
        if (iToolbar->IsICalenToolBar())
            {
            return iToolbar;
            }
        }
    return NULL;
    }        
    
// ----------------------------------------------------------------------------
// CCalenViewManager::ViewsActivated
// Returns if the first view activation on start-up has taken place
// ----------------------------------------------------------------------------
TBool CCalenViewManager::ViewsActivated() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iViewsActivated;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::UpdateToolbarNextViewIconL
// Updates the nextview icon on the toolbar when default view is not month view
// ----------------------------------------------------------------------------
void CCalenViewManager::UpdateToolbarNextViewIconL(TUid aViewUid)
	{
	// Set the view cycle index based on the newly activated view,
    // if the view is in the cycle array.
    TInt index = iViewInfoArray.Find( aViewUid, CCalenViewInfo::ViewInfoIdentifier );
    if( index != KErrNotFound )
        {
        iViewCycleIndex = index;
        if( iToolbar )
            {
            TInt nextVwIndex = iViewCycleIndex;
            do{
               nextVwIndex = ( nextVwIndex + 1 )%(iViewInfoArray.Count());
              }
            while( iViewInfoArray[nextVwIndex]->CyclePosition() == CCalenView::ENoCyclePosition );
            
            CCalenView* nextview = static_cast<CCalenView*>( 
                                        iAppUi.View( iViewInfoArray[nextVwIndex]->ViewUid() ) );
            iToolbar->SetNextViewIcon( nextview->ViewIconL() );
            }
        }
	}

// ----------------------------------------------------------------------------
// CCalenViewManager::HandleFasterAppExitCommandL
// Handles ECalenFasterAppExit command
// ----------------------------------------------------------------------------
void CCalenViewManager::HandleFasterAppExitCommandL()
    {
    TRACE_ENTRY_POINT;
    
    if(iToolbar)
        {
        iToolbar->SetToolbarExtensionFocus(EFalse);
        }
    
    // For Handling : When the default view is changed keeping that view open, 
    // from day view changing the default view from month view to day view, 
    // we need to reset the previous view id.
    if(iPreviousToDayView.iViewUid!=KNullUid)
        {
        iPreviousToDayView.iViewUid = KNullUid;
        }
    
    // For handling specific case::Calendar exited from FSW 
    // iAvoidRepopulation is set when app backgrounded.
    // Reset the flag for activating the view in background
    if(iAvoidRepopulation)
        {
        iAvoidRepopulation = EFalse;
        }
    
    // In case of fasterapp exit, first activate the dummy view before deleting the plugin views
    // Get the default view before we remove plugins
    iPreviousViewId.iViewUid = KNullUid;
    
    iController.SetExitOnDialogFlag( EFalse ); // for making iisexitondialogclose EFalse.

    TUid defView = iSetting->DefaultView();
   
    // unload all plugins
    iCustomisationManager->DisableAllPluginsL();
    ActivateViewOnFakeExitL(defView);
    // Though the current view is active view, there is no need to issue a callback as we are exiting the whole application.
    if( iRemovedActiveView )
        {
        
        if( !iViewRemovalCallback )
            {
            TCallBack callback( RemoveDeActivatedView, this );
            iViewRemovalCallback = new( ELeave ) CAsyncCallBack( callback,
                                                    CActive::EPriorityStandard );
            }
        iViewRemovalCallback->CallBack();
        }

    TRACE_EXIT_POINT;
    }
// ----------------------------------------------------------------------------
// CCalenViewManager::HandleSettingsChangeNotificationL
// Handles ECalenNotifySettingsChanged notification
// ----------------------------------------------------------------------------
void CCalenViewManager::HandleSettingsChangeNotificationL()
    {
    TRACE_ENTRY_POINT;
    
    if( iController.IsFasterAppFlagEnabled() )
        {
        TUid newViewUid = iSetting->DefaultView();
        if(IsNativeView(newViewUid))
            {
            TUid oldViewUid = CurrentView();
            if( newViewUid.iUid != oldViewUid.iUid )
                {
                RequestActivationL( newViewUid );
                }
            }
        
        // If the plugins are activated using general settings.
        // unload all plugins.Only load the plugins when application
        // comes to foreground
        const RArray<TUid>& plugins = CustomisationManager().ActivePlugins();
        if(plugins.Count())
            {
            // unload all plugins
            iCustomisationManager->DisableAllPluginsL();
            }
        }

    if( iRemovedActiveView )
        {
        // If the active view has been disabled, the next view in 
        // the view cycle list is activated and the current view
        // is removed asyncronously.
        CycleNextViewL();

        if( !iViewRemovalCallback )
            {
            TCallBack callback( RemoveDeActivatedView, this );
            iViewRemovalCallback = new( ELeave ) CAsyncCallBack( callback,
                                                    CActive::EPriorityStandard );
            }
    
        iViewRemovalCallback->CallBack();
        }
    else if(iCustomisationManager->HiddenView(iCurrentViewId.iViewUid))
         {
         // Get the uid of the plugin view that is hiding the current view
         TUid viewUid = iCustomisationManager->GetReplacePluginViewIdL(iCurrentViewId.iViewUid);
             
         // update the view cycle index before activating the plugin view
         iViewCycleIndex = iViewInfoArray.Find( viewUid, 
                                    CCalenViewInfo::ViewInfoIdentifier );
         RequestActivationL(viewUid);
         }
   else
        {
        StartActiveStepL();
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::HandleEntryDeleteNotificationL
// Handles ECalenNotifyEntryDeleted and ECalenNotifyInstanceDeleted
// notifications
// ----------------------------------------------------------------------------
void CCalenViewManager::HandleEntryDeleteNotificationL()
    {
    TRACE_ENTRY_POINT;
    
    if(iCurrentViewId.iViewUid == KUidCalenEventView)   
        {
        // Activate the previous view when an event is deleted from
        // the event view 
        if(iPreviousViewId.iViewUid != KNullUid)
            {
            if(iPreviousToDayView.iViewUid != KNullUid)
                {
                RequestActivationL(iPreviousViewId.iViewUid, KUidCalenShowBackCba);
                }
            else
                {
                RequestActivationL(iPreviousViewId.iViewUid);
                }
            }
        }
    else
        {
        // refresh the current view
        StartActiveStepL();             
        }
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::HandleEntryClosedNotificationL
// Handles ECalenNotifyEntryClosed notification
// ----------------------------------------------------------------------------
void CCalenViewManager::HandleEntryClosedNotificationL()
    {
    TRACE_ENTRY_POINT;
	
	// reset tha flag iAvoidRepopulation to refresh the view.
	iAvoidRepopulation = EFalse;
    
    // if previous view is native view activate that view
    // otherwise commandlauncher will handle  
    if(iPreviousViewId.iViewUid != KNullUid)
        {
        // if iPreviousToDayView is active activate the day view with "Back" cba.
        if(iPreviousToDayView.iViewUid!=KNullUid)
            {
            RequestActivationL(iPreviousViewId.iViewUid, KUidCalenShowBackCba);
            }
        else
            {
            if( !iController.GetExitOnDialogFlag() )
                {
                RequestActivationL(iPreviousViewId.iViewUid);
                }
            }
        }

	// reset tha flag iAvoidRepopulation to avoid the repopulation.
	iAvoidRepopulation = ETrue;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenViewManager::HandleSystemTimeChangeNotificationL
// Handles ECalenNotifySystemTimeChanged notification
// ----------------------------------------------------------------------------
void CCalenViewManager::HandleSystemTimeChangeNotificationL()
    {
    TRACE_ENTRY_POINT;
    
    if( iController.IsFasterAppFlagEnabled() )
        {
        //Set the context whenever system time is changed
        TUid newViewUid = iSetting->DefaultView();
        MCalenContext& context = iController.Services().Context();
        TCalTime focusTime = context.DefaultCalTimeForViewsL();
        context.SetFocusDateAndTimeL( focusTime,
                                      TVwsViewId( KUidCalendar, newViewUid ));

        // reset tha flag iAvoidRepopulation to refresh the view whenever
        // system time is changed
        iAvoidRepopulation = EFalse;
        
        if(IsNativeView(newViewUid))
            {
            // activate the default view in background
            RequestActivationL( newViewUid );
            }
        
        // set the flag iAvoidRepopulation to avoid repopulation
        iAvoidRepopulation = ETrue;
        }
    else
        {
        // refresh the current view
        StartActiveStepL();
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenViewManager::IconsL
// Create a CCalenIcons object if neccessary and return a reference
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CCalenIcons& CCalenViewManager::IconsL()
    {
    TRACE_ENTRY_POINT;

    if (!iIcons)
        {
        // Icons
        iIcons = CCalenIcons::NewL();
        }

    TRACE_EXIT_POINT;
    return *iIcons;
    }

// -----------------------------------------------------------------------------
// CCalenViewManager::GetNextViewIconL
// Gets next view icon 
// -----------------------------------------------------------------------------
CGulIcon* CCalenViewManager::GetNextViewIconL()
    {
    TRACE_ENTRY_POINT;
    
    // Set the view cycle index based on the newly activated view,
    // if the view is in the cycle array.
    TInt index = iViewInfoArray.Find( iCurrentViewId.iViewUid, CCalenViewInfo::ViewInfoIdentifier );
    if( index != KErrNotFound )
        {
        iViewCycleIndex = index;
        TInt nextVwIndex = iViewCycleIndex;
        do
            {
            nextVwIndex = ( nextVwIndex + 1 )%(iViewInfoArray.Count());
            }while( iViewInfoArray[nextVwIndex]->CyclePosition() == CCalenView::ENoCyclePosition );
        
        // Get the next view icon
        CCalenView* nextview = static_cast<CCalenView*>( iAppUi.View( iViewInfoArray[nextVwIndex]->ViewUid() ) );
        return( nextview->ViewIconL() );
        }
             
    TRACE_EXIT_POINT;
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCalenViewManager::SetRepopulation
// Resets the flag iAvoidRepopulation to activate the view.
// -----------------------------------------------------------------------------
void CCalenViewManager::SetRepopulation(TBool aRePopulate)
    {
    TRACE_ENTRY_POINT;
    // to prevent the display of blank view, 
    // set the iAvoidRepopulation to EFalse, 
    // so that view is populated properly
    
    iAvoidRepopulation = aRePopulate;
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenViewManager::CalenToolbar
// Returns calendar toolbar
// -----------------------------------------------------------------------------
CCalenToolbarImpl* CCalenViewManager::CalenToolbar()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iToolbar;
    }

// -----------------------------------------------------------------------------
// CCalenViewManager::ReloadAllPluginsL
// Reload all plugins
// -----------------------------------------------------------------------------
void CCalenViewManager::ReloadAllPluginsL()
    {
    TRACE_ENTRY_POINT;
    
    // load all plugins
    iCustomisationManager->DoPluginLoadingL();
    UpdatePluginListL();
    
    // only activate plugin view if it is default view
    TUid defaultViewUid = iSetting->DefaultView();
    if(!IsNativeView(defaultViewUid))
        {
        iAvoidRepopulation = EFalse;
                
        // Find the default view in the view cycle list
        TInt position = iViewInfoArray.Find(
                defaultViewUid, CCalenViewInfo::ViewInfoIdentifier );
        if( position != KErrNotFound )
            {
            TVwsViewId targetViewId( KUidCalendar, defaultViewUid);
            RequestActivationL(targetViewId);
            iViewCycleIndex = position;
            }
        else
            {
            // if plugin providing default view is already uninstalled
            // activate month view as default view
            CRepository* repository = CRepository::NewL( KCRUidCalendar );
            CleanupStack::PushL(repository);
            TInt tmp( static_cast<TInt>( KUidCalenMonthView.iUid ) );
            TInt position = iViewInfoArray.Find( KUidCalenMonthView, CCalenViewInfo::ViewInfoIdentifier );
            User::LeaveIfError( repository->Set( KCalendarDefaultStartView, tmp ) );
            iAppUi.SetDefaultViewL( *iAppUi.View( KUidCalenMonthView ) );
            TVwsViewId targetViewId( KUidCalendar, KUidCalenMonthView);
            RequestActivationL(targetViewId);
            iViewCycleIndex = position;
            CleanupStack::PopAndDestroy(repository);  
            }
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenViewManager::GetPreviousViewUid
// Rest of the details are commented in header.
// -----------------------------------------------------------------------------
TUid CCalenViewManager::GetPreviousViewUid()
    {
    return iPreviousViewId.iViewUid;
    }

// -----------------------------------------------------------------------------
// CCalenViewManager::IsNativeView
// Check for native view
// -----------------------------------------------------------------------------
TBool CCalenViewManager::IsNativeView(TUid aViewUid)
    {
    TRACE_ENTRY_POINT;

    if( (aViewUid == KUidCalenMonthView)||
        (aViewUid == KUidCalenWeekView) ||
        (aViewUid == KUidCalenDayView) ||
        (aViewUid == KUidCalenTodoView) )
        {
        TRACE_EXIT_POINT;
        return ETrue;
        }
    TRACE_EXIT_POINT;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCalenViewManager::ActivateViewOnFakeExitL
// Check for native view
// -----------------------------------------------------------------------------
void CCalenViewManager::ActivateViewOnFakeExitL(TUid aDefView)
    {
    TRACE_ENTRY_POINT;
    
    if(IsNativeView(aDefView))
        {
        // activate the view in background
        RequestActivationL( aDefView, KCalenHideInBackGround, KNullDesC8() );
        }
    else
        {
        RequestActivationL( KUidCalenDummyView, KCalenHideInBackGround, KNullDesC8() );
        }
    
    // set the flag to avoid repopulation when application is
    // opened after faster exit
    iAvoidRepopulation = ETrue;
    
    // set the context 
    MCalenContext& context = iController.Services().Context();
    TCalTime focusTime = context.DefaultCalTimeForViewsL();
    context.SetFocusDateAndTimeL( focusTime,
                                  TVwsViewId( KUidCalendar, aDefView) );
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenViewManager::ActivateLocalViewL
// Activate the local view if application already in back ground
// -----------------------------------------------------------------------------
void CCalenViewManager::ActivateLocalViewL(TUid aDefView)
    {
    iAppUi.ActivateLocalViewL( aDefView );      
    }
// End of file
