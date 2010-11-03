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
* Description:  bcviewplugin custom CBCDayContainer physics impl
*
*/

// SYSTEM INCLUDES
#include <aknphysics.h>
#include <coecntrl.h>

// PROJECT INCLUDES
#include "cbcdayphysicshandler.h"

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::CBCDayPhysicsHandler()
// ---------------------------------------------------------------------------
//
CBCDayPhysicsHandler::CBCDayPhysicsHandler( CCoeControl& aControl, \
                                            MAknPhysicsObserver& aPhysicsObserver )
: iControl( aControl ), 
  iPhysicsObserver( aPhysicsObserver )
    {
    
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::NewL
// ---------------------------------------------------------------------------
//
CBCDayPhysicsHandler* CBCDayPhysicsHandler::NewL( CCoeControl& aControl, \
                                                  MAknPhysicsObserver& aPhysicsObserver )
    {
    CBCDayPhysicsHandler* self = 
             new ( ELeave ) CBCDayPhysicsHandler( aControl, aPhysicsObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CBCDayPhysicsHandler::ConstructL()
    {
    iFeatureEnabled = CAknPhysics::FeatureEnabled();
    if ( iFeatureEnabled )
        {
        // Physics: enable needed pointer events        
        // Physics: Create physics
        // Give pointer to control that should be able to flick/drag
        iPhysics = CAknPhysics::NewL( *this,  &iControl );
        }
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::~CBCDayPhysicsHandler
// ---------------------------------------------------------------------------
//
CBCDayPhysicsHandler::~CBCDayPhysicsHandler()
    {
    delete iPhysics;
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::HandlePointerEventL
// ---------------------------------------------------------------------------
//
void CBCDayPhysicsHandler::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( !iFeatureEnabled )
        return;
    
    if ( iPhysics->OngoingPhysicsAction() == CAknPhysics::EAknPhysicsActionBouncing )
        { 
        // Block scrolling events while bouncing is active
        
        CCoeControl* windowOwningControl = &iControl;
        
        while ( windowOwningControl && !windowOwningControl->OwnsWindow() )
            {
            windowOwningControl = windowOwningControl->Parent();
            }

        if ( windowOwningControl )
            {
            windowOwningControl->IgnoreEventsUntilNextPointerUp();
            return;
            }
        }
    
    // Down
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
        //save start point
        iStartPoint = aPointerEvent.iParentPosition;
        iDragPoint = iStartPoint;
        iPhysics->InitPhysicsL( iWorldSize, iViewSize, EFalse );
        // Save start time
        iStartTime.HomeTime();
        }
    
    // Drag
    else if ( aPointerEvent.iType == TPointerEvent::EDrag )
        {
        // Check how position was changed and report to physics
        TPoint deltaPoint( iDragPoint - aPointerEvent.iParentPosition );
        if ( Abs(deltaPoint.iY) >= iPhysics->DragThreshold() )
            {
            iDragPoint = aPointerEvent.iParentPosition;
            iPhysics->RegisterPanningPosition( deltaPoint );
            }
        }
    
    // Up
    else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        // Calculate dragging distance
        TPoint drag( iStartPoint - aPointerEvent.iParentPosition );
        // Start physics
        iPhysics->StartPhysics( drag, iStartTime );        
        }
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::InitPhysics
// ---------------------------------------------------------------------------
//
void CBCDayPhysicsHandler::InitPhysicsL( TSize aWorldSize, TSize aViewSize )
    {
    if ( !iFeatureEnabled )    
        return;        
    
    iPhysics->InitPhysicsL( aWorldSize, aViewSize, EFalse );
    SetWorldSize( aWorldSize );
    SetViewSize( aViewSize );
    //iPhysics->UpdateViewWindowControl( &iControl );
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::SetWorldSize
// ---------------------------------------------------------------------------
//
void CBCDayPhysicsHandler::SetWorldSize( TSize aWorldSize )
    {
    if ( !iFeatureEnabled )        
        return;
        
    iWorldSize = aWorldSize;    
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::SetViewSize
// ---------------------------------------------------------------------------
//
void CBCDayPhysicsHandler::SetViewSize( TSize aViewSize )
    {
    if ( !iFeatureEnabled )        
            return;
            
    iViewSize = aViewSize;
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::ViewPositionChanged
// ---------------------------------------------------------------------------
//
void CBCDayPhysicsHandler::ViewPositionChanged( const TPoint& aNewPosition,
                                                TBool aDrawNow,
                                                TUint aFlags )
    {
    if ( !iFeatureEnabled )        
        return;    

    iHorScrollPos = aNewPosition.iX - iViewSize.iWidth / 2;
    
    // No horizontal scrolling
    iHorScrollPos = 0;
    iVerScrollPos = aNewPosition.iY - iViewSize.iHeight / 2;        
    iPhysicsObserver.ViewPositionChanged( TPoint( 0, iVerScrollPos ), aDrawNow, aFlags );
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::PhysicEmulationEnded
// ---------------------------------------------------------------------------
//
void CBCDayPhysicsHandler::PhysicEmulationEnded()
    {
    iPhysicsObserver.PhysicEmulationEnded();
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::ViewPosition
// ---------------------------------------------------------------------------
//
TPoint CBCDayPhysicsHandler::ViewPosition() const
    {
    if ( !iFeatureEnabled )        
        return TPoint( 0, 0 );
        
    return TPoint( iViewSize.iWidth / 2 + iHorScrollPos, iViewSize.iHeight / 2 + iVerScrollPos );
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::SetHScrollPos
// ---------------------------------------------------------------------------
//
void CBCDayPhysicsHandler::SetHScrollPos( TInt aHScrollPos )
    {
    iHorScrollPos = aHScrollPos;
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::SetVScrollPos
// ---------------------------------------------------------------------------
//
void CBCDayPhysicsHandler::SetVScrollPos( TInt aVScrollPos )
    {
    iVerScrollPos = aVScrollPos;
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::SetVScrollPos
// Returns TDayPhysicsHandlerAction action code i.e. which action CAknPhysics 
// is currently performing.
// ---------------------------------------------------------------------------
//
TInt CBCDayPhysicsHandler::OngoingPhysicsAction() const
    {
    if ( !iFeatureEnabled )        
        return KErrNotSupported;
    
    return iPhysics->OngoingPhysicsAction();
    }

// ---------------------------------------------------------------------------
// CBCDayPhysicsHandler::DragThreshold()
// Returns drag threshold value
// ---------------------------------------------------------------------------
//
TInt CBCDayPhysicsHandler::DragThreshold() const 
	{
	return iPhysics->DragThreshold();
	}
// End of file
