/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Calendar Application UI. Delegates responsibilities to 
*                Calendar Controller
*
*/



// Debug
#include "calendarui_debug.h"           // Calendar debug macros

// Include files
#include "CalenAppUi.h"                 // CCalenAppUi
#include "CalenDoc.h"                   // CCalenDocument
#include "calencontroller.h"            // CCalenController
#include "CalendarVariant.hrh"          // Calendar variant flags

#include <aknconsts.h>                  // KAknMaxWsBufferSize
#include <aknenv.h>                     // CAknEnv
#include <featmgr.h>                    // FeatureManager
#include <calennotificationhandler.h>   // ECalenNotifyResourceChanged

// ----------------------------------------------------------------------------
// CCalenAppUi::CCalenAppUi
// C++ default constructor
// ----------------------------------------------------------------------------
//
CCalenAppUi::CCalenAppUi()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAppUi::ConstructL
// Second phase construction
// ----------------------------------------------------------------------------
//
void CCalenAppUi::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    // Initialise the feature manager
    FeatureManager::InitializeLibL();

    // Enable Skinning and MSK support
    //Single click
    BaseConstructL(
            EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );

    // Create the Calendar controller and pass ownership to the document.
    // The controller can only be created after BaseConstructL has been called.
    iController = CCalenController::NewL( *this );
    static_cast<CCalenDocument*>( Document() )->SetController( iController );
    
    // Request window server command buffer size to be maximum.
    // Lot of drawing happens in Calendar views and maximum buffer size
    // reduces flickering
    CAknEnv::Static()->RequestWsBuffer( KAknMaxWsBufferSize );

    // Uncomment following lines to debug drawing code
    // #ifdef __WINS__
    //     RWsSession &ws = iEikonEnv->WsSession();
    //     ws.SetAutoFlush(ETrue);
    //     CEikonEnv::Static()->InfoMsg( _L("Auto flush ON!"));
    // #endif // __WINS__
    

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAppUi::~CCalenAppUi
// Destructor
// ----------------------------------------------------------------------------
//
CCalenAppUi::~CCalenAppUi()
    {
    TRACE_ENTRY_POINT;

    // Free the feature manager
    FeatureManager::UnInitializeLib();

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAppUi::HandleCommandL
// Handles application exit commands.  All other commands are handled by the
// controller or by customisation plugins.
// ----------------------------------------------------------------------------
//
void CCalenAppUi::HandleCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;

    if( aCommand == EAknSoftkeyExit || aCommand == EEikCmdExit
    	|| aCommand == EAknCmdExit )
        {
        iController->BroadcastNotification(ECalenNotifyDeleteInstanceView);
        iController->IssueCommandL(aCommand);
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAppUi::ProcessCommandParametersL
// Passes the command line parameters to the controller for parsing.
// ----------------------------------------------------------------------------
//
TBool CCalenAppUi::ProcessCommandParametersL(TApaCommand aCommand,
                                             TFileName& aDocumentName,
                                             const TDesC8& aTail)
    {
    TRACE_ENTRY_POINT;

    iController->ProcessCommandParametersL( aCommand, aDocumentName, aTail );

    TRACE_EXIT_POINT;
    return CAknViewAppUi::ProcessCommandParametersL( aCommand,
                                                                             aDocumentName,
                                                                             aTail );
    }

// ----------------------------------------------------------------------------
// CCalenAppUi::HandleMessageL
// Takes care of message handling.
// ----------------------------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse CCalenAppUi::HandleMessageL(
                                    TUint32 aClientHandleOfTargetWindowGroup,
                                    TUid aMessageUid,
                                    const TDesC8& aMessageParameters)
    {
    TRACE_ENTRY_POINT;
    
    MCoeMessageObserver::TMessageResponse response( EMessageNotHandled );

#ifdef RD_CALEN_EXTERNAL_CAL
    if (CurrentView())
        {
        CurrentView()->ReleaseServiceHandler();
        }
#endif //RD_CALEN_EXTERNAL_C

    // If aMessageParameters is set Calendar has been launched by another
    // application (probably CCalenLauncher or Active Idle).
    if (aMessageParameters.Length())
        {
        TFileName dummy;
        TApaCommand cmd=EApaCommandRun;
        ProcessCommandParametersL( cmd, dummy, aMessageParameters );
        response = EMessageHandled;
        }

    if ( response == EMessageNotHandled )
        {
        response = CAknViewAppUi::HandleMessageL( 
                                          aClientHandleOfTargetWindowGroup,
                                          aMessageUid,
                                          aMessageParameters);
        }

    TRACE_EXIT_POINT;
    return response;
    }

// ----------------------------------------------------------------------------
// CCalenAppUi::HandleResourceChangeL
// Called by the framework on resource changes.  Broadcasts a notification of
// ECalenNotifyResourceChanged to all registered notification handlers.
// ----------------------------------------------------------------------------
//
void CCalenAppUi::HandleResourceChangeL( TInt aType )
    {
    TRACE_ENTRY_POINT;

    CAknViewAppUi::HandleResourceChangeL( aType );

    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        iController->BroadcastNotification( ECalenNotifyResourceChanged );
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenAppUi::HandleForegroundEventL
// Called by the framework on switch current application to foreground/background
// Broadcasts a nofification of ECalenNotifyAppForegrounded / ECalenNotifyAppBackgrounded
// ----------------------------------------------------------------------------
void CCalenAppUi::HandleForegroundEventL( TBool aForeground )
    {
    TRACE_ENTRY_POINT;
    
    // have to call base class's method first
    CAknViewAppUi::HandleForegroundEventL( aForeground );
    
    if( aForeground )   // switch from background to foreground
        {
        iController->BroadcastNotification( ECalenNotifyAppForegrounded );
        }
    else    // switch from foreground to background
        {
        iController->BroadcastNotification( ECalenNotifyAppBackgrounded );
        }
    
    TRACE_EXIT_POINT;
    
    }
//
//  End of File
