/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
 *
*/


//debug
#include "calendarui_debug.h"

// INCLUDE FILES
#include "AIAgendaPluginEngine.h"
#include "AIAgendaPluginEngineImpl.h"


// ============================ MEMBER FUNCTIONS ===============================
// (CAIAgendaPluginEngine)

// ---------------------------------------------------------
// CAIAgendaPluginEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CAIAgendaPluginEngine::ConstructL(MPluginDataChangeObserver& aDataChangeObserver, TObserverType aObserverType)
    {
    TRACE_ENTRY_POINT;
    
    iEngineImpl = CAIAgendaPluginEngineImpl::NewL( aDataChangeObserver, aObserverType );
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CAIAgendaPluginEngine::NewL
// Two-phased constructor.
// Create instance of concrete ECOM interface implementation
// ---------------------------------------------------------
//
CAIAgendaPluginEngine* CAIAgendaPluginEngine::NewL(TAny* aParams)
    {
    TRACE_ENTRY_POINT;
    
    TCtorParams* params = static_cast<TCtorParams*>(aParams);
    CAIAgendaPluginEngine* self = new( ELeave )CAIAgendaPluginEngine();
    CleanupStack::PushL( self );

    self->ConstructL( *params->iDataChangeObserver, params->iObserverType );

    CleanupStack::Pop( self );
    
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------
// CAIAgendaPluginEngine::~CAIAgendaPluginEngine
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
CAIAgendaPluginEngine::~CAIAgendaPluginEngine(void)
    {
    TRACE_ENTRY_POINT;
    
    delete iEngineImpl;
    
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// CAIAgendaPluginEngine::Refresh
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CAIAgendaPluginEngine::Refresh(void)
    {
    TRACE_ENTRY_POINT;
    
    if( iEngineImpl )
        {
        iEngineImpl->SafeRefresh();
        }
    
    TRACE_EXIT_POINT;
    }


// End of File
