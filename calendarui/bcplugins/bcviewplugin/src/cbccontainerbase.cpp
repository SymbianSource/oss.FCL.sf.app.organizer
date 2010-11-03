/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for business calendar containers
*
*/

// SYSTEM INCLUDES
#include <calencommands.hrh>
#include <CalenInterimUtils2.h>
#include <calenservices.h>

// TOOLKIT INCLUDES
#include <alf/alfenv.h>
#include <alf/alfevent.h>
#include <alf/alfcontrolgroup.h>

// PROJECT INCLUDES
#include "cbccontainerbase.h"
#include "cbcviewbase.h"
#include "calendarui_debug.h"
#include "calendar.hrh"
#include "calenuid.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CBCContainerBase::~CBCContainerBase
// Destructor
// ---------------------------------------------------------------------------
//
CBCContainerBase::~CBCContainerBase()
    {
    TRACE_ENTRY_POINT;    
    TRACE_EXIT_POINT;
    }


// ---------------------------------------------------------
// CBCContainerBase::CBCContainerBase
// C++ constructor can NOT contain any code that may leave
// ---------------------------------------------------------
//
CBCContainerBase::CBCContainerBase( 
        CAlfEnv& aEnv, 
        CBCViewBase* aView, 
        MCalenServices& aServices, 
        MBCResourceManager& aResManager 
        )
    : CAlfControl(),
    iAlfEnv( aEnv),
    iView( aView),
    iServices( aServices),
    iResManager( aResManager)
    {
    TRACE_ENTRY_POINT;
    iActiveUpdate=ETrue;
    iResourceChangeAllowed=ETrue;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCContainerBase::ConstructL
// ---------------------------------------------------------------------------
//
void CBCContainerBase::ConstructL()
    {
    TRACE_ENTRY_POINT;    
    ConstructImplL();
    TRACE_EXIT_POINT;
    }
        
// ---------------------------------------------------------
// CBCContainerBase::OfferEventL
// From CAlfControl.
// ---------------------------------------------------------
//   
TBool CBCContainerBase::OfferEventL( const TAlfEvent& aEvent )
    {
    TRACE_ENTRY_POINT;
    TBool consumed( EFalse );
    
    if(iResourceChangeAllowed)
        {
        if ( aEvent.IsKeyEvent() && aEvent.Code() == EEventKey )
            {
            switch ( aEvent.KeyEvent().iCode )
                {
                case CALEN_VIEW_CYCLING_NEXT:
                    {
                    if ( iView )
                        {
                        iView->HandleCommandL( ECalenGotoToday );
                        consumed = ETrue;
                        }
                    }
                    break;
                case CALEN_VIEW_CYCLING_PREV:
                    {
                    HandleViewCyclingL(); // Virtual method implemented by views
                    consumed = ETrue;
                    }
                    break;
                default:
                    {
                    consumed = ( OnKeyDownEventL( aEvent.KeyEvent(), aEvent.Code() ) == EKeyWasConsumed );
                    }
                    break;
                }
            }
        }
    TRACE_EXIT_POINT;
    return consumed;
    }

// ---------------------------------------------------------------------------
// CBCContainerBase::AllowActiveUpdate()
// When set to true ActiveUpdate is allowed
// ---------------------------------------------------------------------------
//
void CBCContainerBase::AllowActiveUpdate( TBool aFlag )
    {
    TRACE_ENTRY_POINT;
    iActiveUpdate=aFlag;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCContainerBase::IsActiveUpdate()
// Returns current status of active update flag
// ---------------------------------------------------------------------------
//
TBool CBCContainerBase::IsActiveUpdate()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iActiveUpdate;
    }

// ---------------------------------------------------------------------------
// CBCContainerBase::StartActiveUpdate
// iActive object is activated for View Refresh
// ---------------------------------------------------------
//
void CBCContainerBase::StartActiveUpdate()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCContainerBase::SuspendActiveUpdate
// Suspend refresh
// ---------------------------------------------------------------------------
//
void CBCContainerBase::SuspendActiveUpdate() const
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCContainerBase::BackgroundActiveUpdate
// Active schedule priority set to background.
// ---------------------------------------------------------------------------
//
void CBCContainerBase::BackgroundActiveUpdate()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CBCContainerBase::IsMRViewersEnabledL
// Returns ETrue if Meeting Request viewers are enabled.
// ---------------------------------------------------------------------------
//
TBool CBCContainerBase::IsMRViewersEnabledL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    return iServices.InterimUtilsL().MRViewersEnabledL();
    }

// ---------------------------------------------------------------------------
// CBCContainerBase::InstanceViewL
// Returns a reference to the calendar instance view.
// ---------------------------------------------------------------------------
//
CCalInstanceView& CBCContainerBase::InstanceViewL()
    {
    TRACE_ENTRY_POINT;
    CCalInstanceView* instanceView( NULL );    
    RArray<TInt> colIdArray;
    CBCViewBase::GetActiveCollectionidsL( iServices, colIdArray );
        
    if(colIdArray.Count() > 0)
        {
        instanceView = iServices.InstanceViewL(colIdArray);
        }
    else
        {
        instanceView = iServices.InstanceViewL();
        }
    colIdArray.Reset();
    TRACE_EXIT_POINT;
    return *instanceView;
    }

// ---------------------------------------------------------
// CBCContainerBase::RefershL
//  
// ---------------------------------------------------------
//   
void CBCContainerBase::Refresh()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CBCContainerBase::UpdateStatusPaneAndExtensionsL
// ----------------------------------------------------------------------------
//
void CBCContainerBase::UpdateStatusPaneAndExtensionsL()
    {
    TRACE_ENTRY_POINT;
    iView->SetStatusPaneFromActiveContextL();
    TRACE_EXIT_POINT;
    }

// ----------------------------------------------------------------------------
// CBCContainerBase::HandleNaviLabelEventL
// ----------------------------------------------------------------------------
void CBCContainerBase::HandleNaviLabelEventL( TInt aEventID )
    {
    TRACE_ENTRY_POINT;
    if(aEventID == ECCalenNaviLabelGo)
        {
        iView->OnCmdGotoDateL();
        }
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCContainerBase::RefreshLayoutL
// -----------------------------------------------------------------------------
//
void CBCContainerBase::RefreshLayoutL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCContainerBase::NotifyChangeDateL
// Notify that Active date is changed. Each contianer will redraw.
// -----------------------------------------------------------------------------
//
void CBCContainerBase::NotifyChangeDateL()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// CFSCalenContainer::NotifyDocChange
// Notify that Agenda file is changed.
// ---------------------------------------------------------------------------
//
void CBCContainerBase::NotifyDocChange()
    {
    TRACE_ENTRY_POINT;
    Refresh();
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CBCContainerBase::OnKeyDownEventL
// Standard key event handler
// -----------------------------------------------------------------------------
//
TKeyResponse CBCContainerBase::OnKeyDownEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TRACE_ENTRY_POINT;
    TKeyResponse response(EKeyWasNotConsumed);
        
    TInt code = aKeyEvent.iScanCode;
    
    if ( ('0' <= code && code <= '9') || // digit keys
              ('A' <= code && code <= 'Z') ) // character keys
        {
        //TODO: port IsShowingDeleteWaitNote when function available
//        if ( ! iView->IsShowingDeleteWaitNote() )
        
        // If editor is being opened or is about to be opened
        // put event (any event) into queue.
        //
        iServices.QueueKeyEvent(aKeyEvent, aType);
        
        TInt cmd =
            iView->Id() == KUidCalenTodoView ?
            ECalenNewTodo : ECalenNewMeeting;
        iView->HandleCommandL( cmd );

        response = EKeyWasConsumed;
        }   
    TRACE_EXIT_POINT;
    return response;    
    }

// End of file
