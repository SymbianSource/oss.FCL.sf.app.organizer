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
* Description:  Calendar controller
*
*/


#include <aknnotewrappers.h>            // CAknInformationNote
#include <aknViewAppUi.h>               // CAknViewAppUi
#include <StringLoader.h>               // Loads strings from resource
#include <aknappui.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>
#include <pathinfo.h>
#include <avkon.hrh>
#include "calenviewinfo.h"              // View information
#include <Calendar.rsg>                 // Calendar resourcess
#include <missedalarmstore.h>           // missed alarm store
#include <calsession.h>
#include <calencommandhandler.h>
#include <calencontext.h>
#include <calenconstants.h>             // KCalenLostAlarms etc
#include <calcalendarinfo.h>

#include "calendarui_debug.h"           // Debug macros
#include "calencontroller.h"            // CCalenController
#include "calenactionui.h"              // Default command handling
#include "calennotifier.h"              // Broadcasts system events
#include "calenviewmanager.h"           // Responsible for all view activations
#include "calenalarmmanager.h" 			// Alarm Manager
#include "calenservicesimpl.h"          // MCalenServices implementation
#include "calensetting.h"
#include "calencmdlinelauncher.h"       // Command line launcher
#include "calencustomisationmanager.h"  // Customisation Manager
#include "calenstatemachine.h"
#include "calenicons.h"
#include "calentoolbarimpl.h"
#include "calenmultipledbmanager.h"
#include "calenattachmentmodel.h"

const TInt KCustomCommandRangeStart     = ECalenLastCommand; 
const TInt KNumberOfCommandsPerServices = 100;
_LIT( KResourceFile, "calencommonui.rsc" );

// ----------------------------------------------------------------------------
// CCalenController::NewL
// First stage construction. This will leave if an instance of the controller
// already exists.  All access to an instance of the controller should be
// through the InstanceL function, except for the initial construction
// which should be handled by the appui.
// This is to prevent usage of this API through the services dll when Calendar
// application is not running.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenController* CCalenController::NewL( CAknViewAppUi& aAppUi )
    {
    TRACE_ENTRY_POINT;

    CCalenController* self = NULL;
    TAny* tlsPtr = Dll::Tls();

    // Check Thread local storage
    if( !tlsPtr )
        {
        // TLS is NULL, so no CCalenController has been created yet.
        self = new( ELeave ) CCalenController( aAppUi );
        CleanupStack::PushL( self );
        // Store a self pointer in TLS
        User::LeaveIfError( Dll::SetTls( static_cast<TAny*>( self ) ) );
        // Increment ref count right away. If we don't do it here, and someone
        // calls Controller::InstanceL in ConstructL and then ConstructL
        // leaves, we will double delete the controller.
        ++self->iRefCount;
        self->ConstructL();
        CleanupStack::Pop( self );
        }
    else
        {
        // An instance of the controller exists already.
        // This function should only have been called once, by CCalenAppUi
        User::Leave( KErrAlreadyExists );
        }

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenController::InstanceL
// Returns a pointer to the single instance of the Calendar Controller.
// Leaves with KErrNotReady if the controller has not already been created
// using NewL.  A leave here means that the Calendar application is not running
// but someone is trying to use the services API.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCalenController* CCalenController::InstanceL()
    {
    TRACE_ENTRY_POINT;

    CCalenController* self = NULL;
    TAny* tlsPtr = Dll::Tls();

    // Check Thread local storage
    if( !tlsPtr )
        {
        // The Controller has not yet been constructed.  Someone is probably
        // trying to use the Services API outside of a customisation
        // plugin.  We don't allow that.
        User::Leave( KErrNotReady );
        }
    else
        {
        self = static_cast<CCalenController*>( tlsPtr );
        }

    ++self->iRefCount;

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenController::ConstructL
// 2nd phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
       	TFileName fileName;
	// Get the complate path of the DLL from where it is currently loaded
	Dll::FileName( fileName );
	
	TFileName resFile;
	
	// Append the Drive letters ex., Z: or C:
	resFile.Append(fileName.Mid(0,2));
	resFile.Append(KDC_RESOURCE_FILES_DIR);
    resFile.Append(KResourceFile);
    
    BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(), resFile );
    
	iResourceFileOffset = CEikonEnv::Static()->AddResourceFileL( resFile );
	
    
	iStateMachine = CCalenStateMachine::NewL( *this );
    // Create the notifier.
    iNotifier = new( ELeave )CCalenNotifier( *this );

    // Get an instance of the global data
    iGlobalData = CCalenGlobalData::NewL( *iNotifier, iNotifier );
    iGlobalData->InitializeGlobalDataL();
    
    RArray<TCalenNotification> notificationArray;
    // Complete construction of the notifier and register the
    // global data for notifications
    iNotifier->ConstructL();
    
    notificationArray.Append(ECalenNotifyEntryInstanceViewCreated);
    notificationArray.Append(ECalenNotifyEntryInstanceViewCreationFailed);
    notificationArray.Append(ECalenNotifyDeleteInstanceView);
    notificationArray.Append(ECalenNotifyRealExit);
    notificationArray.Append(ECalenNotifyCalendarInfoCreated);
    notificationArray.Append(ECalenNotifyCalendarInfoUpdated);
    

    RegisterForNotificationsL( iGlobalData,notificationArray);
    notificationArray.Reset();
    
    // Create the cmd line handler
    iCmdLineLauncher = CCalenCmdLineLauncher::NewL( *this, iAppUi );
    
    // Create the services
    iServices = CCalenServicesImpl::NewL();
    
    // Create the action uis.
    iActionUi = CCalenActionUi::NewL( *this );
    
    // Create the settings
    iSetting = CCalenSetting::InstanceL();
    
    // Create the view manager, and register for notifications
    iViewManager = CCalenViewManager::NewL( iAppUi, *this );
    
    notificationArray.Append(ECalenNotifySettingsChanged);
    notificationArray.Append(ECalenNotifySettingsClosed);
    notificationArray.Append(ECalenNotifySystemLocaleChanged);
    notificationArray.Append(ECalenNotifyPluginEnabledDisabled);
    notificationArray.Append(ECalenNotifyEntrySaved);
    notificationArray.Append(ECalenNotifyEntryDeleted);
    notificationArray.Append(ECalenNotifyInstanceDeleted);
    notificationArray.Append(ECalenNotifyMultipleEntriesDeleted);
    notificationArray.Append(ECalenNotifyExternalDatabaseChanged);
    notificationArray.Append(ECalenNotifyDeleteFailed);
    notificationArray.Append(ECalenNotifyEntryClosed);
	notificationArray.Append(ECalenNotifyCancelDelete);
    notificationArray.Append(ECalenNotifySystemTimeChanged);
    notificationArray.Append(ECalenNotifyAppForegrounded);
    notificationArray.Append(ECalenNotifyDayViewClosed);
    notificationArray.Append(ECalenNotifyAppBackgrounded);
    notificationArray.Append(ECalenNotifyViewPopulationComplete);
    notificationArray.Append(ECalenNotifyCalendarFieldChanged);
    notificationArray.Append(ECalenNotifyCancelStatusUpdation);
    notificationArray.Append(ECalenNotifyMarkedEntryCompleted);
    notificationArray.Append(ECalenNotifyAttachmentAdded);
    notificationArray.Append(ECalenNotifyAttachmentViewerClosed);
    notificationArray.Append(ECalenNotifyAttachmentRemoved);
    notificationArray.Append(ECalenNotifyCalendarInfoCreated);
    notificationArray.Append(ECalenNotifyCalendarInfoUpdated);
    notificationArray.Append(ECalenNotifyCalendarFileDeleted);
    
    RegisterForNotificationsL( iViewManager, notificationArray );
    notificationArray.Reset();
                                                         
    // Create the customisation manager, and register for 
    // notifications
    iCustomisationManager = CCalenCustomisationManager::NewL( *this,
                                                              iSetting->PluginAvailability(),
                                                              *iServices,
                                                              iViewManager->ViewInfoArray() );
                                                                    
    notificationArray.Append(ECalenNotifySettingsChanged);
    notificationArray.Append(ECalenNotifyCheckPluginUnloading);
    notificationArray.Append(ECalenNotifyEComRegistryChanged);
    
    RegisterForNotificationsL( iCustomisationManager,notificationArray);
    notificationArray.Reset();
    
    // Some plugins may have been added or removed - update the settings.
    iSetting->UpdatePluginListL( *iCustomisationManager );
    
    // View manager constructs the custom views using the
    // customisation manager
    iViewManager->ConstructCustomViewsL( *iCustomisationManager );
    
    // for handling missed alarms/msk improvements for alarm
    iAlarmManager = CCalenAlarmManager::NewL(*this); 
    
    notificationArray.Append(ECalenNotifyLostAlarms);
    notificationArray.Append(ECalenNotifyMissedAlarmViewClosed);
    notificationArray.Append(ECalenNotifyMissedEventViewClosed);
    notificationArray.Append(ECalenNotifyEntryDeleted);
    notificationArray.Append(ECalenNotifyInstanceDeleted);
    notificationArray.Append(ECalenNotifyEntrySaved);
    notificationArray.Append(ECalenNotifyMultipleEntriesDeleted);
    notificationArray.Append(ECalenNotifySystemTimeChanged);
    notificationArray.Append(ECalenNotifyAlarmStopped);
    notificationArray.Append(ECalenNotifyAlarmSnoozed);
    notificationArray.Append(ECalenNotifyEntryClosed);
    notificationArray.Append(ECalenNotifyAppForegrounded);
    
    RegisterForNotificationsL( iAlarmManager, notificationArray );
    notificationArray.Reset();

    //iMultipleDbmanager = CCalenMultipleDbManager::NewL();
    
    iAttachmentData = CCalenAttachmentModel::NewL();
    
    notificationArray.Close();
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::CCalenController
// C++ default constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenController::CCalenController( CAknViewAppUi& aAppUi )
    : iAppUi( aAppUi ),
      iNextServicesCommandBase( KCustomCommandRangeStart ),
      iFasterApp( EFalse )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::~CCalenController
// Private destructor, called from Release() when reference count is 0.
// Frees all resources.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenController::~CCalenController()
    {
    TRACE_ENTRY_POINT;
    
    if ( iServices )
        {
        iServices->Release();
        }
        
    delete iActionUi;
    delete iNotifier;

    delete iViewManager;
    delete iStateMachine;
    
    if ( iSetting )
        {
        iSetting->Release();
        }

    if( iGlobalData )
        {
        iGlobalData->Release();
        }

    Dll::SetTls( NULL );

    delete iCmdLineLauncher;
    delete iCustomisationManager;

    if( iResourceFileOffset )
        {
        CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
        }
    //delete iMultipleDbmanager;
    
    if(iSystemTimeChangedMsgDelayer)
        {
        iSystemTimeChangedMsgDelayer->Cancel();
        delete iSystemTimeChangedMsgDelayer;
        iSystemTimeChangedMsgDelayer = NULL;
        }
    
    delete iAlarmManager;
    
    if(iAttachmentData)
        {
        delete iAttachmentData;
        iAttachmentData = NULL;
        }
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::Release
// Decrement the reference count of this singleton.
// When the reference count is 0, the controller will self delete and free
// all resources
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenController::Release()
    {
    TRACE_ENTRY_POINT;

    --iRefCount;
    
    // The controller owns its own instance of the services, therefore the
    // reference count will be one, immediatley before deletion.
    if (iRefCount == 1)
        {
        delete this;
        }

    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenController::ReleaseCustomisations
// Releases any plugins by deleting the customisation manager
// should only be called on exiting by the document.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenController::ReleaseCustomisations()
    {
    TRACE_ENTRY_POINT;
    
    delete iCustomisationManager;
    iCustomisationManager = NULL;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::IssueCommmandL
// Adds the passed command to the comand queue. Commands are handled
// asynchronously in HandleCommandL
// Returns EFalse if the passed command is not in the issuers command range
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CCalenController::IssueCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;
    TCalenCommand cmd;
    
    if( aCommand == EAknCmdHideInBackground ||
	  ( aCommand == EAknSoftkeyExit && iAppUi.ExitHidesInBackground() ) )
	    {
	    SetFasterAppFlag( ETrue );
	    aCommand = ECalenFasterAppExit;
	    }
    else if( aCommand == EAknCmdExit || aCommand == EEikCmdExit
             || aCommand == EAknSoftkeyExit )
        {
        if( iViewManager->CalenToolbar() )
            {
            iViewManager->CalenToolbar()->ResetCalendarToolbar();
            }
        }
    else
        {
        if((aCommand < ECalenViewCommandBase ) || (aCommand > iNextServicesCommandBase))
            {
            return EFalse;
            }
        }
    
    cmd.SetCommandAndContextL( aCommand, iGlobalData->Context() );

    TBool ret = iStateMachine->HandleCommandL( cmd );

    TRACE_EXIT_POINT;
    return ret;
    }


// ----------------------------------------------------------------------------
// CCalenController::RequestActivationL
// Request activation of a specific view
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::RequestActivationL( const TVwsViewId& aViewId )
    {
    TRACE_ENTRY_POINT;

    iViewManager->RequestActivationL( aViewId );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::BroadcastNotification
// Passes the notification to the Calendar Notifier.  The notification will
// then be broadcast to all observers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenController::BroadcastNotification( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;
    
    iNotifier->BroadcastNotification( aNotification );
        
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::RegisterForNotificationsL
// Registers the passed notification handler with the Calendar Notifier
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenController::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    iNotifier->RegisterForNotificationsL( aHandler, aNotification );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::RegisterForNotificationsL
// Registers the passed notification handler with the Calendar Notifier
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenController::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            RArray<TCalenNotification>& aNotifications )
    {
    TRACE_ENTRY_POINT;

    iNotifier->RegisterForNotificationsL( aHandler, aNotifications );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::CancelNotifications
// Removes the passed handler from the notifier.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenController::CancelNotifications( MCalenNotificationHandler* aHandler )
    {
    TRACE_ENTRY_POINT;

    iNotifier->CancelNotifications( aHandler );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::GetCommandHandlerL
// Searches for a command handler for a particular command.  Customisations
// are searched first.  If no customisation wants to handle the command it is
// handled by the view manager or the action uis
// ----------------------------------------------------------------------------
MCalenCommandHandler* CCalenController::GetCommandHandlerL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;
    
    MCalenCommandHandler* handler( NULL );

    // Stop non-published commands from being customised
    if ( aCommand != ECalenShowSettings
        && aCommand != ECalenSwitchView )
        {
        // See if a plugin wants the command
        handler = iCustomisationManager->GetCommandHandlerL( aCommand );
        }

    // See if the view manager wants the command
    if(!handler)
        {
        if( aCommand >= ECalenViewCommandBase
            && aCommand < ECalenEditCommandBase )
            {
            handler = iViewManager;
            }
     	else if(aCommand >= ECalenMissedAlarmCommandBase
      		&& aCommand < ECalenAttachmentCommandBase )
	    	{
	    	handler = iAlarmManager;
	    	} 
        else 
            {
            handler = iActionUi->GetCommandHandlerL(aCommand);
            }
        }

    // No command handler is an error  
    
    // return the handler
    TRACE_EXIT_POINT;
    return handler;
    }

// ----------------------------------------------------
//  CCalenController::CheckSystemTimeAtStartUpL
//  Check the system time change at the startup
// ----------------------------------------------------
//
void CCalenController::CheckSystemTimeAtStartUpL()
    {
    TRACE_ENTRY_POINT;

    if(iSystemTimeChangedMsgDelayer)
        {
        iSystemTimeChangedMsgDelayer->Cancel();
        delete iSystemTimeChangedMsgDelayer;
        iSystemTimeChangedMsgDelayer = NULL;
        }
    
    // Introduce delay (CPeriodic) before showing the note 
    // to allow time for the active view to display before
    // note.
    
    TCallBack callback;
    callback = TCallBack( SystemTimeChangeCallback, this );
                                     
    iSystemTimeChangedMsgDelayer = new (ELeave) CAsyncCallBack(
                            callback, CActive::EPriorityStandard);
    iSystemTimeChangedMsgDelayer->CallBack();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------
//  CCalenController::SystemTimeChangeCallback
//  This function is called when the System time is changed.
// ----------------------------------------------------
//
TInt CCalenController::SystemTimeChangeCallback(TAny* aThisPtr)
    {
    TRACE_ENTRY_POINT;

    PIM_TRAPD_HANDLE(
        static_cast<CCalenController*>(aThisPtr)->HandleSystemTimeChangeL());

    TRACE_EXIT_POINT;
    return 0;
    }

// ----------------------------------------------------------------------------
// CCalenController::HandleSystemTimeChangeL
// Checks to see if the system time was changed while Calendar was
// not running or in the background, potentially causing alarms to be missed
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::HandleSystemTimeChangeL()
    {
    TRACE_ENTRY_POINT;
    
    // get the system time change info
    TInt timeChanged = iNotifier->SystemTimeChangedL();

    switch( timeChanged )
        {
        case KCalenTimeZoneChanged:
            {
            ShowSystemChangeInfoNoteL( R_QTN_CALE_NOTE_SYSTEM_TIME_CHANGED );
            }
            break;
        case KCalenLostAlarms:
            {
            // Not displayed since missed alarms are handled in missed alarms view.
            // No need to show the info note the user.
            // Part of alarm improvement REQ for calendar.
            //ShowSystemChangeInfoNoteL( R_QTN_CALE_NOTE_MISSED_ALARMS );
            }
            break;
        case KNoUserInfoNoteDisplay:
        default:
            break;
        }
    
    // update system time change info to the cenrep
    iNotifier->UpdateSytemTimeChangeInfoL();
 
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::ShowSystemChangeInfoNoteL
// Displays an information note if the system time changed while Calendar
// was inactive
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::ShowSystemChangeInfoNoteL( TInt aResourceId )
    {
    TRACE_ENTRY_POINT;

    HBufC* buf = StringLoader::LoadLC( aResourceId, CEikonEnv::Static() );
    CAknInformationNote* dialog = new( ELeave ) CAknInformationNote();

    dialog->ExecuteLD( *buf );

    CleanupStack::PopAndDestroy( buf );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::NewServicesL
// Factory function for creating new MCalenServices objects
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C MCalenServices* CCalenController::NewServicesL()
    {
    TRACE_ENTRY_POINT;

    TInt commandRangeStart = iNextServicesCommandBase;
    TInt commandRangeEnd = commandRangeStart + KNumberOfCommandsPerServices;
    iNextServicesCommandBase = commandRangeEnd + 1;

    CCalenServicesImpl* svc = CCalenServicesImpl::NewL( commandRangeStart,
                                                                              commandRangeEnd );
    TRACE_EXIT_POINT;
    return svc;
    }

// ----------------------------------------------------------------------------
// CCalenController::ProcessCommandParametersL
// Takes care of commandline parameters.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenController::ProcessCommandParametersL( TApaCommand aCommand,
                                                       TFileName& aDocumentName,
                                                       const TDesC8& aTail )
    {
    TRACE_ENTRY_POINT;

    iCmdLineLauncher->ProcessCommandParametersL( aCommand, aDocumentName, aTail );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::Notifier
// Returns the notifier.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenNotifier& CCalenController::Notifier()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iNotifier;
    }

// ----------------------------------------------------------------------------
// CCalenController::SetExitOnDialogFlag
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::SetExitOnDialogFlag( TBool aFlag )
    {
    TRACE_ENTRY_POINT;
    
    iCmdLineLauncher->SetExitOnDialogclose( aFlag );
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::GetExitOnDialogFlag
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenController::GetExitOnDialogFlag()
    {
    TRACE_ENTRY_POINT;

    TBool tempVal;
    tempVal = iCmdLineLauncher->GetExitOnDialogStatus();
    return tempVal;

    TRACE_EXIT_POINT;
    }
    
// ----------------------------------------------------------------------------
// CCalenController::Services
// Returns the services
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
MCalenServices& CCalenController::Services()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iServices;
    }

// ----------------------------------------------------------------------------
// CCalenController::OfferMenuPaneL
// Offers the menu pane to plugins for customisation.
// Acts as a conduit between the services and the customisation manager.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C void CCalenController::OfferMenuPaneL( TInt aResourceId,
                                                                     CEikMenuPane* aMenuPane )
    {
    TRACE_ENTRY_POINT;

    iCustomisationManager->OfferMenuPaneL( aResourceId, aMenuPane );

    if( aResourceId == R_CALENDAR_CHANGE_VIEW_MENUPANE )
        {
        // The cascading view switch menu is being displayed
        // therefore the view manager needs to be asked to remove
        // the current view
        iViewManager->RemoveCurrentViewFromMenu( aMenuPane );
        }
    
    TUint32 missedAlarmsCount(0);
    // get the count from missed alarm store
    iAlarmManager->MissedAlarmStore()->CountL(missedAlarmsCount);
    
    //For adding "Missed Alarms" menu item for native views menu pane
    if(!missedAlarmsCount)
        {
        if( aResourceId == R_CALENDAR_MONTH_MENUPANE
            || aResourceId == R_CALENDAR_DAY_MENUPANE
            || aResourceId == R_CALENDAR_WEEK_MENUPANE
            || aResourceId == R_TODO_LIST_MENUPANE )
                {
                aMenuPane->DeleteMenuItem(ECalenMissedAlarmsView);
                }
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::Infobar
// Descriptor passed to plugins to get customised info bar text.
// Acts as a conduit between the services and the customisation manager.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CCalenController::Infobar( const TRect& aRect )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iCustomisationManager->Infobar( aRect );
    }

// ----------------------------------------------------------------------------
// CCalenController::Infobar
// Descriptor passed to plugins to get customised info bar text.
// Acts as a conduit between the services and the customisation manager.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CCalenController::Infobar()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iCustomisationManager->Infobar();
    }

// ----------------------------------------------------------------------------
// CCalenController::PreviewPane
// Descriptor passed to plugins to get customised preview pane text.
// Acts as a conduit between the services and the customisation manager.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CCalenController::PreviewPane( TRect& aRect )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iCustomisationManager->PreviewPane( aRect );
    }

// ----------------------------------------------------------------------------
// CCalenController::CustomPreviewPaneL
// Return custom preview pane
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
EXPORT_C MCalenPreview* CCalenController::CustomPreviewPaneL( TRect& aRect )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iCustomisationManager->CustomPreviewPaneL(aRect);
    }

// ----------------------------------------------------------------------------
// CCalenController::CustomisationManager
// Returns a reference to the customisation manager
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenCustomisationManager& CCalenController::CustomisationManager()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iCustomisationManager;
    }

// ----------------------------------------------------------------------------
// CCalenController::ViewManager
// Returns a reference to the view manager
// (other items were commented in a header).
// ----------------------------------------------------------------------------
CCalenViewManager& CCalenController::ViewManager()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iViewManager;
    }

// ----------------------------------------------------------------------------
// CCalenController::MissedAlarmStore
// Returns a reference to the Missed Alarm Store
// ----------------------------------------------------------------------------
CMissedAlarmStore* CCalenController::MissedAlarmStore()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;

    return iAlarmManager->MissedAlarmStore();
    }

// ----------------------------------------------------------------------------
// CCalenController::IsFasterAppFlagEnabled
// Returns ETrue if the application is fake exited
// else return EFalse.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
TBool CCalenController::IsFasterAppFlagEnabled()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	return iFasterApp;
    }

// ----------------------------------------------------------------------------
// CCalenController::SetFasterAppFlag
// Set the flag 'iFasterApp' to ETrue if application is fake exited
// and to EFalse once the application comes to foreground.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
void CCalenController::SetFasterAppFlag( TBool aFlag )
	{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	iFasterApp = aFlag;
	}

// ----------------------------------------------------------------------------
// CCalenController::AppUi
// Returns a reference to the appui
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CAknViewAppUi& CCalenController::AppUi()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iAppUi;
    }

// ----------------------------------------------------------------------------
// CCalenController::GetMissedAlarmsList
// Returns the missed alarms list
// ----------------------------------------------------------------------------
void CCalenController::GetMissedAlarmsList(RArray<TCalenInstanceId>& aMissedAlarmsList)
    {
    TRACE_ENTRY_POINT;
    iAlarmManager->GetMissedAlarmsList(aMissedAlarmsList);
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::Settings
// Returns a reference to the calendar settings
// ----------------------------------------------------------------------------
CCalenSetting& CCalenController::Settings()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iSetting;
    }

// ----------------------------------------------------------------------------
// CCalenController::GetIconL
// Get icon of specific type
// ----------------------------------------------------------------------------
//
CGulIcon* CCalenController::GetIconL( MCalenServices::TCalenIcons aIndex )
    {
    TRACE_ENTRY_POINT;
    
    // if view requests next view icon
    if(aIndex == MCalenServices::ECalenNextViewIcon)
        {
        return (iViewManager->GetNextViewIconL());
        }
    
    TRACE_EXIT_POINT;
    return iViewManager->IconsL().GetIconL(aIndex);
    }

// ----------------------------------------------------------------------------
// CCalenController::MultipleDbManager
// Returns a reference to the CCalenMultipleDbManager
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenMultipleDbManager& CCalenController::MultipleDbManager()
    {
    TRACE_ENTRY_POINT
    CCalenMultipleDbManager* tmp = NULL;
    TRACE_EXIT_POINT
    return *tmp;
    }

// ----------------------------------------------------------------------------
// CCalenController::StateMachine
// Returns a reference to the CCalenStateMachine
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenStateMachine& CCalenController::StateMachine()
    {
    TRACE_ENTRY_POINT
    TRACE_EXIT_POINT
    return *iStateMachine;
    }


// -----------------------------------------------------------------------------
// CCalenController::GetActiveCollectionidsL
// -----------------------------------------------------------------------------
//
void CCalenController::GetActiveCollectionidsL(
                                           RArray<TInt>& aCollectionIds)
    {
    TRACE_ENTRY_POINT
    RPointerArray<CCalCalendarInfo> calendarInfoList;
    CleanupClosePushL(calendarInfoList);
    iGlobalData->GetAllCalendarInfoL(calendarInfoList);
    
    for(TInt index=0;index<calendarInfoList.Count();index++)
        {
        if(calendarInfoList[index]->Enabled())
            {
            HBufC* calendarFileName = 
                calendarInfoList[index]->FileNameL().AllocLC();
            aCollectionIds.Append(
                 iGlobalData->CalSessionL(*calendarFileName).CollectionIdL());
            CleanupStack::PopAndDestroy(calendarFileName);
            }
        }
    
    CleanupStack::PopAndDestroy(&calendarInfoList);
    TRACE_EXIT_POINT
    }

// -----------------------------------------------------------------------------
// CCalenController::AttachmentData
// Returns a reference to the CCalenAttachmentModel
// ----------------------------------------------------------------------------
//
CCalenAttachmentModel& CCalenController::AttachmentData()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iAttachmentData;
    }
	
// -----------------------------------------------------------------------------
// CCalenController::IsEditorActive
// Tells framework whether editor is active or not
// -----------------------------------------------------------------------------
//
TBool CCalenController::IsEditorActive()
    {
    return (iActionUi->IsEditorActive());
    }

// -----------------------------------------------------------------------------
// CCalenController::AddCalendarL
// Adds a new calendar file with metadata set
// -----------------------------------------------------------------------------
//
void CCalenController::AddCalendarL(CCalCalendarInfo* aCalendarInfo)
    {
    TRACE_ENTRY_POINT;
    iGlobalData->AddCalendarL(aCalendarInfo);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenController::UpdateCalendarL
// Updates calendar file with new calendar info
// -----------------------------------------------------------------------------
//
void CCalenController::UpdateCalendarL(CCalCalendarInfo* aCalendarInfo)
    {
    TRACE_ENTRY_POINT;
    iGlobalData->UpdateCalendarL(aCalendarInfo);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenController::RemoveCalendarL
// Removes calendar file based on calendar file name
// -----------------------------------------------------------------------------
//
void CCalenController::RemoveCalendarL(const TDesC& aCalendarFileName)
    {
    TRACE_ENTRY_POINT;
    iGlobalData->RemoveCalendarL(aCalendarFileName);
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenController::RemoveCalendarL
// Removes all dead calendar files from the file system
// -----------------------------------------------------------------------------
//
void CCalenController::RemoveDeadCalendarsL()
    {
    TRACE_ENTRY_POINT;
    iGlobalData->RemoveDeadCalendarsL();
    TRACE_EXIT_POINT;
    }
// -----------------------------------------------------------------------------
// CCalenController::GetAllCalendarInfoL
// Get all available calendar info
// -----------------------------------------------------------------------------
//
void CCalenController::GetAllCalendarInfoL(
                RPointerArray<CCalCalendarInfo>& aCalendarInfoList)
    {
    TRACE_ENTRY_POINT;
    iGlobalData->GetAllCalendarInfoL(aCalendarInfoList);
    TRACE_EXIT_POINT;
    }
// End of file

