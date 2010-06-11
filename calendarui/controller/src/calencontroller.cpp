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


#include <e32std.h>
#include <hbmainwindow.h>
#include <hbwidget.h>
#include <hbinstance.h>

// User includes
#include "calencontroller.h"            // CCalenController
#include "calenviewmanager.h"           // Responsible for all view activations        
#include "calenservicesimpl.h"          // MCalenServices implementation
#include "calenactionui.h"              // Default command handling
#include "calencustomisationmanager.h"  // Customisation Manager
#include "calennotificationhandler.h"
#include "calennotifier.h"
#include "hb_calencommands.hrh"
#include "calendarui_debug.h"           // Debug macros
#include "calenstatemachine.h"
#include "calenservicesimpl.h"          // Calendar service implementation
#include "CalenUid.h"
#include "calencontextimpl.h"

// Constants
const TInt KCustomCommandRangeStart     = ECalenLastCommand; 
const TInt KNumberOfCommandsPerServices = 100;

// ----------------------------------------------------------------------------
// CCalenController::CCalenController
// Constructor
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenController::CCalenController(bool isFromServiceFrmwrk)
{
    TRACE_ENTRY_POINT;
    iIsFromServiceFrmWrk = isFromServiceFrmwrk;
    iNextServicesCommandBase = KCustomCommandRangeStart;
    iRefCount = 0;
    
    // Store the pointer in tls, also avoid multiple creations
    checkMultipleCreation();
    
    // Get an instance of AgendaUtil interface class
    // This will take care of
    mAgendaUtil = new AgendaUtil(this);

    iStateMachine = CCalenStateMachine::NewL( *this );
    
    // Create the notifier.
    iNotifier = new( ELeave )CalenNotifier( *iStateMachine );
    
    // Construct the context
    mContext = new CalenContextImpl(iNotifier);
    
    // Set the default context.Once will start use of calencmdlinelauncher,
    // Then need to remove this function
    SetDefaultContext();
    
    RArray<TCalenNotification> notificationArray;
    // Complete construction of the notifier and register the
    // global data for notifications
    iNotifier->ConstructL();
    
    // Create the services
    iServices = CalenServicesImpl::NewL();
    // Create the customisation manager, and register for 
    // notifications
    iCustomisationManager = CCalenCustomisationManager::NewL( *this,
                                                                  *iServices );
    // Create the view manager, and register for notifications
	iViewManager = new CalenViewManager(*this, isFromServiceFrmwrk );
	
	hbInstance->allMainWindows().first()->show();
    
    // Create the action uis.
    iActionUi = CCalenActionUi::NewL( *this );
                                                                    
    notificationArray.Append(ECalenNotifySettingsChanged);
    notificationArray.Append(ECalenNotifyCheckPluginUnloading);
    notificationArray.Append(ECalenNotifyEComRegistryChanged);
    
    RegisterForNotificationsL( iCustomisationManager,notificationArray);
    notificationArray.Reset();
    
    notificationArray.Append(ECalenNotifyViewPopulationComplete);
    notificationArray.Append(ECalenNotifyExternalDatabaseChanged);
    notificationArray.Append(ECalenNotifyMultipleEntriesDeleted);
    notificationArray.Append(ECalenNotifyDialogClosed);
    notificationArray.Append(ECalenNotifyEntrySaved);
    notificationArray.Append(ECalenNotifyEntryDeleted);
    notificationArray.Append(ECalenNotifyInstanceDeleted);
    notificationArray.Append(ECalenNotifySystemLocaleChanged);
    notificationArray.Append(ECalenNotifySystemTimeChanged);
    notificationArray.Append(ECalenNotifyEntryClosed);
    notificationArray.Append(ECalenNotifySettingsClosed);
    		
    RegisterForNotificationsL( iViewManager, notificationArray );
    notificationArray.Reset();  
    notificationArray.Close();
    TRACE_EXIT_POINT;
}

void CCalenController::checkMultipleCreation()
{
	TAny* tlsPtr = Dll::Tls();

	// Check Thread local storage
	if( !tlsPtr )
		{
		// Store a self pointer in TLS
		User::LeaveIfError( Dll::SetTls( static_cast<TAny*>( this ) ) );
		// Increment ref count right away. If we don't do it here, and someone
		// calls Controller::InstanceL in ConstructL and then ConstructL
		// leaves, we will double delete the controller.
		++this->iRefCount;
		}
	else
		{
		// An instance of the controller exists already.
		// This function should only have been called once, by CCalenAppUi
		User::Leave( KErrAlreadyExists );
		}	
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
CCalenController* CCalenController::InstanceL()
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
// CCalenController::ReleaseCustomisations
// Releases any plugins by deleting the customisation manager
// should only be called on exiting by the application.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::ReleaseCustomisations()
    {
    TRACE_ENTRY_POINT;
    
    delete iCustomisationManager;
    iCustomisationManager = NULL;
    
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
void CCalenController::Release()
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
// CCalenController::~CCalenController
// Private destructor, called from Release() when reference count is 0.
// Frees all resources.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenController::~CCalenController()
    {
    TRACE_ENTRY_POINT;

    if(iStateMachine) {
    	delete iStateMachine;
    	iStateMachine = NULL;
    }

    if ( iServices )
        {
        iServices->Release();
        }
    
    if( iNotifier )
		{
    	delete iNotifier;
    	iNotifier = NULL;
		}

    if(mContext) {
    	delete mContext;
    	mContext = NULL;
    }
	
   if( iActionUi )
	   {
	   delete iActionUi;
	   iActionUi = NULL;
	   }
   
    if( iViewManager )
		{
    	delete iViewManager;
    	iViewManager = NULL;
		}
    
    if( iCustomisationManager )
		{
    	delete iCustomisationManager;
    	iCustomisationManager = NULL;
		}
    
    
    Dll::SetTls( NULL );
    
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
TBool CCalenController::IssueCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;
    TCalenCommand cmd;
    cmd.SetCommandAndContextL( aCommand ,context());

    TBool ret = iStateMachine->HandleCommandL( cmd );
    TRACE_EXIT_POINT;
    return ret;
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
// CCalenController::ViewManager
// Returns a reference to the view manager
// (other items were commented in a header).
// ----------------------------------------------------------------------------
CalenViewManager& CCalenController::ViewManager()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	return *iViewManager;
    }
    
// ----------------------------------------------------------------------------
// CCalenController::MainWindow
// Returns a reference to the MainWindow
// (other items were commented in a header).
// ----------------------------------------------------------------------------
HbMainWindow& CCalenController::MainWindow()
    {
    TRACE_ENTRY_POINT;
    
    return *(hbInstance->allMainWindows().first());
	
	TRACE_EXIT_POINT;
    }    

// ----------------------------------------------------------------------------
// CCCalenController::BroadcastNotification
// Passes the notification to the Calendar Notifier.  The notification will
// then be broadcast to all observers
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::BroadcastNotification( TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    iNotifier->BroadcastNotification( aNotification );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCCalenController::RegisterForNotificationsL
// Registers the passed notification handler with the Calendar Notifier
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            TCalenNotification aNotification )
    {
    TRACE_ENTRY_POINT;

    iNotifier->RegisterForNotificationsL( aHandler, aNotification );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCCalenController::RegisterForNotificationsL
// Registers the passed notification handler with the Calendar Notifier
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::RegisterForNotificationsL( MCalenNotificationHandler* aHandler,
                                                            RArray<TCalenNotification>& aNotifications )
    {
    TRACE_ENTRY_POINT;

    iNotifier->RegisterForNotificationsL( aHandler, aNotifications );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCCalenController::CancelNotifications
// Removes the passed handler from the notifier.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenController::CancelNotifications( MCalenNotificationHandler* aHandler )
    {
    TRACE_ENTRY_POINT;

    iNotifier->CancelNotifications( aHandler );

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCCalenController::GetCommandHandlerL
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
    if ( !handler )
        {
        if (   aCommand >= ECalenViewCommandBase
            && aCommand < ECalenEditCommandBase )
            {
            handler = iViewManager;
            }
     	else if( aCommand >= ECalenMissedAlarmCommandBase
      		&& aCommand < ECalenLastCommand )
	    	{
	    	//handler = iAlarmManager;
	    	} 
        else 
            {
            handler = iActionUi->GetCommandHandlerL( aCommand );
            }
        }

    // No command handler is an error  
    
    // return the handler
    TRACE_EXIT_POINT;
    return handler;
    }

// ----------------------------------------------------------------------------
// CCCalenController::NewServicesL
// Factory function for creating new MCalenServices objects
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
MCalenServices* CCalenController::NewServicesL()
    {
    TRACE_ENTRY_POINT;
    
    TInt commandRangeStart = iNextServicesCommandBase;
    TInt commandRangeEnd = commandRangeStart + KNumberOfCommandsPerServices;
    iNextServicesCommandBase = commandRangeEnd + 1;

    CalenServicesImpl* svc = CalenServicesImpl::NewL( commandRangeStart,commandRangeEnd );
    TRACE_EXIT_POINT;
    return svc;
    }

// ----------------------------------------------------------------------------
// CCalenController::Notifier
// Returns the notifier.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CalenNotifier& CCalenController::Notifier()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return *iNotifier;
    }
    
// ----------------------------------------------------------------------------
// CCalenController::Infobar
// Descriptor passed to plugins to get customised info bar text.
// Acts as a conduit between the services and the customisation manager.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
HbWidget* CCalenController::Infobar()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iCustomisationManager->Infobar();
    }
// ----------------------------------------------------------------------------
// CCalenController::InfobarTextL
// @returns info bar text
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
QString* CCalenController::InfobarTextL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iCustomisationManager->InfobarTextL();
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
// CCalenController::SetDefaultContext
// Sets the default context for today
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//       
void CCalenController::SetDefaultContext()
    {
    TRACE_ENTRY_POINT;  
    QDateTime focusTime = mContext->defaultCalTimeForViewsL();
    mContext->setFocusDateAndTimeL(focusTime,KCalenDayViewUidValue );
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::OfferMenu
// Offers the menu to plugins for customisation.
// Acts as a conduit between the services and the customisation manager.
// (other items were commented in a header).
// ----------------------------------------------------------------------------

void CCalenController::OfferMenu(HbMenu* aHbMenu)
    {
    TRACE_ENTRY_POINT;
    iCustomisationManager->OfferMenu(aHbMenu);
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenController::agendaInterface
// returns the interface to the agenda database
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
AgendaUtil* CCalenController::agendaInterface()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return mAgendaUtil;
    }

// ----------------------------------------------------------------------------
// CCalenController::context
// returns the calendar context
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
MCalenContext& CCalenController::context()
    {
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
    return *mContext;
    }

// ----------------------------------------------------------------------------
// CCalenController::handleServiceManagerSlot
// Launches the requested view 
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CCalenController::handleServiceManagerSlot(int view, const QDateTime& dateTime)
{
	
	if (iIsFromServiceFrmWrk) {
		// Set the context properly
		mContext->setFocusDateAndTimeL(dateTime,KCalenMonthViewUidValue);
		// launch the appropriate view
		iViewManager->constructAndActivateView(view);
		
		// Construct other views
		iViewManager->constructOtherViews();

	} else { // Calendar was in backgroung but now its being brought to foreground
		// If current state is editing state or printing state
		// or deleting state or sending state, then dont do anything as
		// user might loose the data
		CCalenStateMachine::TCalenStateIndex currentState = iStateMachine->CurrentState();
		if ((currentState == CCalenStateMachine::ECalenEditingState) ||
			(currentState == CCalenStateMachine::ECalenDeletingState) ||
			(currentState == CCalenStateMachine::ECalenPrintingState) ||
			(currentState == CCalenStateMachine::ECalenSendingState)) {
			// simply return - we dont have anything to do
		} else {
			// Set the context properly
			mContext->setFocusDateAndTimeL(dateTime,KCalenMonthViewUidValue);
			IssueCommandL(view);
		}
	}
}

// ----------------------------------------------------------------------------
// CCalenController::getFirstView
// returns the first view with which calendar has been launched
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
int CCalenController::getFirstView()
{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	
	return iViewManager->getFirstView();
	
}

// End of file
