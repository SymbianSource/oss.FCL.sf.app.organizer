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
* Description:  Calendar action ui
*
*/


// INCLUDES
#include <aknViewAppUi.h>
#include <avkon.hrh>
#include <aknnavi.h>
#include <akntitle.h>
#include <eikspane.h>
#include <Calendar.rsg>
#include <calencommonui.rsg>
#include <calenservices.h>
#include <calentoolbar.h>

#include "calendarui_debug.h"
#include "calenactionui.h"
#include "calencontroller.h"
#include "calendeleteui.h"
#include "caleneditui.h"
#include "calenlocationui.h"
#include "calensettingsui.h"
#include "calennotifier.h"
#include "calenmultipledbui.h"
#include "calenattachmentui.h"
#include "CalenUid.h"
#include "calenviewmanager.h"

// ----------------------------------------------------------------------------
// CCalenActionUi::NewL
// 1st phase of construction
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenActionUi* CCalenActionUi::NewL( CCalenController& aController )
    {
    TRACE_ENTRY_POINT;

    CCalenActionUi* self = new( ELeave ) CCalenActionUi( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TRACE_EXIT_POINT;
    return self;
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::CCalenActionUi
// C++ default constructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenActionUi::CCalenActionUi( CCalenController& aController )
    : iController( aController )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::ConstructL
// 2nd phase of construction.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenActionUi::ConstructL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::~CCalenActionUi
// Destructor.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
CCalenActionUi::~CCalenActionUi()
    {
    TRACE_ENTRY_POINT;

    delete iDeleteUi;
    delete iEditUi;
    delete iLocationUi;
    delete iAttachmentUi;
    
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::GetCommandHandlerL
// Gets a command handler
// ----------------------------------------------------------------------------
//
MCalenCommandHandler* CCalenActionUi::GetCommandHandlerL( TInt aCommand )
    {
    MCalenCommandHandler* handler = NULL;

    if( aCommand >= ECalenEditCommandBase &&
        aCommand < ECalenDeleteCommandBase )
        {
        if(!iEditUi)
            {
            iEditUi = CCalenEditUi::NewL(iController);
            }
        handler = iEditUi;
        }
    else if( aCommand >= ECalenDeleteCommandBase &&
             aCommand < ECalenSettingsCommandBase )
        {
        if(!iDeleteUi)
            {
            iDeleteUi = CCalenDeleteUi::NewL( iController );
            }
        handler = iDeleteUi;
        }
    else if( aCommand >= ECalenSettingsCommandBase &&
             aCommand < ECalenMapCommandBase )
        {
        handler = this;
        }
    else if( aCommand >= ECalenMapCommandBase &&
             aCommand < ECalenAttachmentCommandBase )
        {
        if(!iLocationUi)
            {
            iLocationUi = CCalenLocationUi::NewL( iController );
            }
        handler = iLocationUi;
        }
    else if( aCommand >= ECalenAttachmentCommandBase && 
             aCommand < ECalenLastCommand )
        {
        if(!iAttachmentUi)
            {
            iAttachmentUi = CCalenAttachmentUi::NewL(iController);
            }
        handler = iAttachmentUi;
        }

    TRACE_EXIT_POINT;
    return handler;
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::HandleCommmandL
// Handles Calendar commands. Delegates to appropriate action ui.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
/*TBool CCalenActionUi::HandleActionUiCommandL( TInt aCommand )
    {
    TRACE_ENTRY_POINT;

    // Lazy initialisation for action uis.
    TBool handled( EFalse );

    if( aCommand >= ECalenEditCommandBase && aCommand < ECalenLastCommand )
        {
        if( !iEditUi )
            {
            iEditUi = CCalenEditUi::NewL( iController );
            }
        handled = iEditUi->HandleActionUiCommandL( aCommand );

        if( !handled )
            {
            if( !iDeleteUi )
                {
                iDeleteUi = CCalenDeleteUi::NewL( iController );
                }
            handled = iDeleteUi->HandleActionUiCommandL( aCommand );
            }

        if( !handled && aCommand == ECalenShowSettings )
            {
            handled = ETrue;

            ShowSettingsL();
            }
        }

    TRACE_EXIT_POINT;
    return handled;
    }*/


// ----------------------------------------------------------------------------
// CCalenActionUi::HandleCommandL
// Handles key presses.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CCalenActionUi::HandleCommandL( const TCalenCommand& aCommand )
    {
    TRACE_ENTRY_POINT;
    TBool continueCommand(EFalse);

    switch(aCommand.Command())
        {
        case ECalenShowSettings:
            {
            ShowSettingsL();
            }
            break;
        case ECalenShowCalendars:
            {
            ShowCalendarsL();
            }
            break;
        default:
            break;
        }

    TRACE_EXIT_POINT;
    return continueCommand;
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::CalenCommandHandlerExtensionL
// Dummy implementation.
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TAny* CCalenActionUi::CalenCommandHandlerExtensionL( TUid /*aExtensionUid*/ )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return NULL;
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::ShowCalendarsL
// Shows the multiple db calendar's dialog
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenActionUi::ShowCalendarsL()
    {
    TRACE_ENTRY_POINT;
    // Create settings own titlepane and navipane, and swap with existing ones
    CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
    
    // Titlepane
    CAknTitlePane* newtp = new( ELeave ) CAknTitlePane();
    CleanupStack::PushL( newtp );
    CCoeControl* oldtp = sp->SwapControlL( TUid::Uid(EEikStatusPaneUidTitle), newtp );
    CleanupStack::Pop( newtp ); // ownership is passed to statuspane
    TRect oldRect( 0, 0, 0, 0 );
    if( oldtp )
        {
        CleanupStack::PushL( oldtp );
        oldRect = oldtp->Rect();
        CCoeControl* ctrl = sp->ContainerControlL( TUid::Uid( EEikStatusPaneUidTitle ));
        newtp->SetContainerWindowL( *ctrl );
        newtp->ConstructL();
        newtp->SetRect(oldRect);
        newtp->ActivateL();
        }        
    // NaviPane
    CAknNavigationControlContainer* newnp = new( ELeave )CAknNavigationControlContainer();
    CleanupStack::PushL( newnp );
    CCoeControl* oldnp = sp->SwapControlL( TUid::Uid( EEikStatusPaneUidNavi ), newnp );
    CleanupStack::Pop( newnp ); // ownership is passed to statuspane
    if( oldnp )
        {
        CleanupStack::PushL( oldnp );
        oldRect = oldnp->Rect();
        CCoeControl* ctrl = sp->ContainerControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
        newnp->SetContainerWindowL( *ctrl );
        newnp->ConstructL();
        newnp->SetRect( oldRect );
        newnp->PushDefaultL();
        newnp->ActivateL();
        }
            
        // Hide the toolbar before we display settings menu
        MCalenToolbar* toolbar = iController.Services().ToolbarOrNull();
        if(toolbar)
            {
            toolbar->SetToolbarVisibilityL(EFalse);  
            }
        
        // defer settings notifications before launching the settings  
        CCalenNotifier& notifier = iController.Notifier();
        notifier.DeferSettingsNotifications();
        
        CCalenMultipleDbUi* dlg = CCalenMultipleDbUi::NewL(iController);
        TInt retValue = KErrNone;
        // Trap showing settings so settings watcher is always resumed.
        PIM_TRAPD_HANDLE( retValue = dlg->ExecuteLD( R_CALEN_MULTIPLEDB_DIALOG ) );
        notifier.ResumeSettingsNotifications();
        
        // Unhide the toolbar when manage calendar view is closed
        TUid activeViewUid = iController.ViewManager().CurrentView();
        if(toolbar && (activeViewUid != KUidCalenMissedAlarmsView ) &&(activeViewUid != KUidCalenMissedEventView ) )
            {
            toolbar->SetToolbarVisibilityL(ETrue); 
            }
        
    // When setting is closed, swap back old titlepane and navipane
    if( oldnp && sp->SwapControlL( TUid::Uid(EEikStatusPaneUidNavi), oldnp ) )
        {
        CleanupStack::Pop( oldnp );
        delete newnp;
        oldnp->ActivateL();
        }
    if( oldtp && sp->SwapControlL( TUid::Uid(EEikStatusPaneUidTitle), oldtp ) )
        {
        CleanupStack::Pop( oldtp );
        delete newtp;
        oldtp->ActivateL();
        }
    
    iController.BroadcastNotification( ECalenNotifyCalendarFieldChanged );

    if( retValue == EAknCmdExit ||
        retValue == EKeyEscape )
        {
        // iController.IssueCommandL( EAknCmdExit );
        // FIX ME ::
        // Issuing the command from here will not exit the application properly
        // because aknviewappui is not exited
        // Calling ProcessCommandL of aknviewappui will exits the application
        iController.AppUi().ProcessCommandL(EAknCmdExit);
        }
    else if(retValue == EAknSoftkeyExit)
        {
        // iController.IssueCommandL( EAknCmdExit );
        // FIX ME ::
        // Issuing the command from here will not exit the application properly
        // because aknviewappui is not exited
        // Calling ProcessCommandL of aknviewappui will exits the application
        iController.AppUi().ProcessCommandL(EAknSoftkeyExit);
        }

    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CCalenActionUi::ShowSettingsL
// Shows the settings dialog
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CCalenActionUi::ShowSettingsL()
    {
    TRACE_ENTRY_POINT;
    
    // Create settings own titlepane and navipane, and swap with existing ones
    CEikStatusPane* sp = CEikonEnv::Static()->AppUiFactory()->StatusPane();
    
    // Titlepane
    CAknTitlePane* newtp = new( ELeave ) CAknTitlePane();
    CleanupStack::PushL( newtp );
    CCoeControl* oldtp = sp->SwapControlL( TUid::Uid(EEikStatusPaneUidTitle), newtp );
    CleanupStack::Pop( newtp ); // ownership is passed to statuspane
    TRect oldRect( 0, 0, 0, 0 );
    if( oldtp )
        {
        CleanupStack::PushL( oldtp );
        oldRect = oldtp->Rect();
        CCoeControl* ctrl = sp->ContainerControlL( TUid::Uid( EEikStatusPaneUidTitle ));
        newtp->SetContainerWindowL( *ctrl );
        newtp->ConstructL();
        newtp->SetRect(oldRect);
        newtp->ActivateL();
        }        
    
    // NaviPane
    CAknNavigationControlContainer* newnp = new( ELeave )CAknNavigationControlContainer();
    CleanupStack::PushL( newnp );
    CCoeControl* oldnp = sp->SwapControlL( TUid::Uid( EEikStatusPaneUidNavi ), newnp );
    CleanupStack::Pop( newnp ); // ownership is passed to statuspane
    if( oldnp )
        {
        CleanupStack::PushL( oldnp );
        oldRect = oldnp->Rect();
        CCoeControl* ctrl = sp->ContainerControlL( TUid::Uid( EEikStatusPaneUidNavi ) );
        newnp->SetContainerWindowL( *ctrl );
        newnp->ConstructL();
        newnp->SetRect( oldRect );
        newnp->PushDefaultL();
        newnp->ActivateL();
        }
        
    // Hide the toolbar before we display settings menu
    MCalenToolbar* toolbar = iController.Services().ToolbarOrNull();
    if(toolbar)
        {
        toolbar->SetToolbarVisibilityL(EFalse);  
        }
    
    // defer settings notifications before launching the settings  
    CCalenNotifier& notifier = iController.Notifier();
    notifier.DeferSettingsNotifications();
    
    CCalenSettingsUi* dlg = CCalenSettingsUi::NewL(iController.CustomisationManager());
    TInt retValue = KErrNone;
    // Trap showing settings so settings watcher is always resumed.
    PIM_TRAPD_HANDLE( retValue = dlg->ExecuteLD( R_CALEN_SETTING_DIALOG ) );
    notifier.ResumeSettingsNotifications();
    
    // Unhide the toolbar when settings is closed
    TUid activeViewUid = iController.ViewManager().CurrentView();
    if(toolbar && (activeViewUid != KUidCalenMissedAlarmsView ) &&(activeViewUid != KUidCalenMissedEventView ) )
        {
        toolbar->SetToolbarVisibilityL(ETrue); 
        }
    
    // When setting is closed, swap back old titlepane and navipane
    if( oldnp && sp->SwapControlL( TUid::Uid(EEikStatusPaneUidNavi), oldnp ) )
        {
        CleanupStack::Pop( oldnp );
        delete newnp;
        oldnp->ActivateL();
        }
    if( oldtp && sp->SwapControlL( TUid::Uid(EEikStatusPaneUidTitle), oldtp ) )
        {
        CleanupStack::Pop( oldtp );
        delete newtp;
        oldtp->ActivateL();
        }
    
    iController.BroadcastNotification( ECalenNotifySettingsClosed );

      if( retValue == EAknCmdExit ||
        retValue == EKeyEscape )
        {
        // iController.IssueCommandL( EAknCmdExit );
        // FIX ME ::
        // Issuing the command from here will not exit the application properly
        // because aknviewappui is not exited
        // Calling ProcessCommandL of aknviewappui will exits the application
        iController.AppUi().ProcessCommandL(EAknCmdExit);
        }
    else if(retValue == EAknSoftkeyExit)
        {
        // iController.IssueCommandL( EAknCmdExit );
        // FIX ME ::
        // Issuing the command from here will not exit the application properly
        // because aknviewappui is not exited
        // Calling ProcessCommandL of aknviewappui will exits the application
        iController.AppUi().ProcessCommandL(EAknSoftkeyExit);
        }
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenActionUi::IsEditorActive
// Tells framework whether editor is active or not
// -----------------------------------------------------------------------------
//
TBool CCalenActionUi::IsEditorActive()
    {
    if(iEditUi)
        {
        return(iEditUi->IsEditorActive());
        }
    else
        return EFalse;
    }

// End of file
