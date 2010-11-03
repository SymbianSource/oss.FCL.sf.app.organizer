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

#ifndef CBCDAYPHYSICSHANDLER_H_
#define CBCDAYPHYSICSHANDLER_H_

// SYSTEM INCLUDES
#include <e32base.h>
#include <w32std.h>
#include <aknphysicsobserveriface.h>

// FORWARD DECLARATIONS
class CAknPhysics;
class CCoeControl;

// CLASS DEFINITION

/**
 *  Handler class for physics used by kinetic scrolling.
 */
NONSHARABLE_CLASS( CBCDayPhysicsHandler ): public CBase, 
                                           public MAknPhysicsObserver
    {
    public:
        
        /**  CBCDayPhysicsHandler action definition
         *   Forwarded from CAknPhysics 
         */
        enum TDayPhysicsHandlerAction
            {
            EDayPhysicsHandlerActionNone = 1,
            EDayPhysicsHandlerActionFlicking,
            EDayPhysicsHandlerActionBouncing,
            EDayPhysicsHandlerActionDragging
            };
            
    public: // C'tor and d'tor
       
        /**
         * Two-phased constructor.
         */
       static CBCDayPhysicsHandler* NewL( CCoeControl& aControl, MAknPhysicsObserver& aPhysicsObserver );
       
       /**
        * Destructor
        */
       ~CBCDayPhysicsHandler();
       
    public: // Interface
        
        /**
         * Handles the pointer event received by AknStylusPopUpMenuContent
         *
         * @param aPointerEvent Pointer event to handle.         
         */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

        /**
         * Initializes physics.
         */
        void InitPhysicsL( TSize aWorldSize, TSize aViewSize );

        /**
         * Sets world size.
         *
         * @param aWorldSize New world size.
         */
        void SetWorldSize( TSize aWorldSize );
        
        /**
         * Sets view size.
         *
         * @param aViewSize New view size.
         */
        void SetViewSize( TSize aViewSize );     
        
        /**
         * Sets HScrollPos.
         *
         * @param aHScrollPos view HScroll position.
         */
        void SetHScrollPos( TInt aHScrollPos );
        
        /**
         * Sets VScrollPos.
         *
         * @param aVScrollPos view VScroll position.
         */
        void SetVScrollPos( TInt aVScrollPos );
        
        /**
         * Returns TDayPhysicsHandlerAction action code i.e. which action CAknPhysics 
         * is currently performing.
         *
         * @return TDayPhysicsHandlerAction type action code.
         */
        TInt OngoingPhysicsAction() const;
		
		/**
         * Returns drag threshold value
         *
         * @return drag threshold value
         */
        TInt DragThreshold() const;

    public: // from base class MAknPhysicsObserver
        void ViewPositionChanged( const TPoint& aNewPosition,
                                  TBool aDrawNow,
                                  TUint aFlags );            
        void PhysicEmulationEnded();    
        TPoint ViewPosition() const;

    private: // Implementation
        CBCDayPhysicsHandler( CCoeControl& aControl, MAknPhysicsObserver& aPhysicsObserver );
        void ConstructL();

    private:
        /**
         * Own: Physics engine handle
         */        
        CAknPhysics* iPhysics;        
        
        /**
         * Ref: Reference to CCoeControl that should be kinetic
         */
        CCoeControl& iControl;
        
        /**
         * Ref: Reference to one object implemented MAknPhysicsObserver
         * While need to move view, use this interface to notify 
         */
        MAknPhysicsObserver& iPhysicsObserver;
        
        /**
         * Feature enabled
         */
        TBool iFeatureEnabled;
                        
        /**
         * Physics world size
         */
        TSize iWorldSize;
                
        /**
         * Physics view size.
         */
        TSize iViewSize;

        /**
         * Horizontal scroll distance.
         */
        TInt iHorScrollPos;

        /**
         * Vertical scroll distance.
         */
        TInt iVerScrollPos;

        /**
         * Pointer event start time.
         */
        TTime iStartTime;

        /**
         * Pointer event start position.
         */
        TPoint iStartPoint;

        /**
         * Pointer event drag position.
         */
        TPoint iDragPoint;            
    };

#endif // CBCDAYPHYSICSHANDLER_H_

// End of file
