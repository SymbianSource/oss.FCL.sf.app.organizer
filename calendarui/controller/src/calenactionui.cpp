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
#include "calendarui_debug.h"
#include "calenactionui.h"
#include "calencontroller.h"
#include "calendeleteui.h"
#include "caleneditui.h"
#include "calennotifier.h"
#include "calenservices.h"
#include "calensettingsview.h"
#include "calenviewmanager.h"
#include <hbmainwindow.h>

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

    if ( aCommand >= ECalenEditCommandBase
      && aCommand < ECalenDeleteCommandBase )
        {
        if( !iEditUi )
            {
            iEditUi = CCalenEditUi::NewL( iController );
            }
        handler = iEditUi;
        }
    else if ( aCommand >= ECalenDeleteCommandBase
      && aCommand < ECalenSettingsCommandBase )
        {
        if( !iDeleteUi )
            {
            iDeleteUi = CalenDeleteUi::NewL( iController );
            }
        handler = iDeleteUi;
        }
    else if ( aCommand >= ECalenSettingsCommandBase
      && aCommand < ECalenMapCommandBase )
        {
        handler = this;
        }
    else if ( aCommand >= ECalenMapCommandBase
      && aCommand < ECalenLastCommand )
        {
        }
        
    TRACE_EXIT_POINT;
    return handler;
    }

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
    
    if(aCommand.Command()==ECalenShowSettings)
        {  
        iController.ViewManager().launchSettingsView();
        }
    
    TRACE_EXIT_POINT;
    return continueCommand; 
    }

// End of file
