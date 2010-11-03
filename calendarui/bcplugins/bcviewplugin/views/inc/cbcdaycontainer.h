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
* Description:  Day view container class
*
*/


#ifndef CBCDAYCONTAINER_H
#define CBCDAYCONTAINER_H


// SYSTEM INCLUDES
#include <aknphysicsobserveriface.h>  //MAknPhysicsObserver
#include <akncollection.h>  //MAknCollection
#include <gestureobserver.h>  //MGestureObserver
#include <touchfeedback.h> //MTouchFeedback

// PROJECT INCLUDES
#include "cbccontainerbase.h"
#include "cbcslotinfostorage.h"
#include "cbcpreviewpopup.h"
#include "cbcalendefs.h"

// Forward declarations
class CBCPreviewPopup;
class CBCViewBase;
class CAlfDeckLayout;
class CAlfAnchorLayout;
class CAlfGridLayout;
class CAlfViewportLayout;
class CAlfImageVisual;
class CAknPhysics;
class CAknItemActionMenu;
class CBCDayPhysicsHandler;
class CAlfCanvasVisual;
namespace GestureHelper
    {
    class CGestureHelper;
    }

// CLASS DEFINITION

/**
 *  Calendar day view control
 *  The control inherit from CBCContainerBase,  
 *  handling special events in the day view.
 */
NONSHARABLE_CLASS( CBCDayContainer ) : public CBCContainerBase,
                                       public MBCPreviewPopupClient,
                                       public MAknPhysicsObserver,
                                       public MAknCollection,
                                       public GestureHelper::MGestureObserver
    {

public:

    /**
     * C++ constructor.
     * @param aEnv Reference to Hitchcock environment
     * @param aView Pointer to day view.
     */
    CBCDayContainer( CAlfEnv& aEnv, CBCViewBase* aView, 
            MCalenServices* aServices, MBCResourceManager& aResManager );

    /**
     * Constructor.
     */
    void ConstructImplL();
    
    /**
     * Destructor
     */
    virtual ~CBCDayContainer();

public:
    
    // from base class MAknPhysicsObserver

    /**
     * Physics emulation has moved the view
     * 
     * @param aNewPosition The new position of the view.
     * @param  aDrawNow    ETrue if client should redraw the screen,
     *                     otherwise EFalse.
     * @param  aFlags      Additional flags, not used currently.
     */
    void ViewPositionChanged( const TPoint& aNewPosition,
    		TBool aDrawNow,
    		TUint aFlags );
    
    /**
     * Physics emulation has stopped moving the view
     */
    void PhysicEmulationEnded();

    /**
     * Returns the observer view position.
     *
     * @return Physics observer view position.
     */
    TPoint ViewPosition() const;   
    
    //From MAknCollection
    
    /**
     * Returns the collection state. The state is combination of
     * flags defined in @c TStateFlag. 
     *
     * @return  Collection state.
     */
    virtual TUint CollectionState() const;

    /**
     * Notifies that item action menu (@c CAknItemActionMenu)
     * was closed. 
     */
    virtual void ItemActionMenuClosed();

    /** 
     * Extension function.
     *
     * @param  aExtensionId  Extension id. 
     * @param  a0            First extension method parameter.
     * @param  a1            Second extension method parameter.
     */    
    virtual TInt CollectionExtension(
            TUint aExtensionId, TAny*& a0, TAny* a1 );
    
    //From MGestureObserver
    
    /**
     * Handle the gesture event
     * @param aEvent event describing the gesture 
     */
    virtual void HandleGestureL( const GestureHelper::MGestureEvent& aEvent );
    
public: // New functions

    /**
     * Update the container with new content.
     */
    void UpdateContainer();

    /**
     * Update control bar.
     */
    void UpdateControlBarL();

    /**
     * Update CCalenState according to highlight in the view
     */
    void SetActiveContextFromHighlightL();

    /**
     * Is this view empty.
     * @return ETrue if the view is empty, EFalse otherwise.
     */
    TBool IsEmptyView() const;

    /**
     * Is single event selected in this view.
     * @return ETrue if single event is selected, EFalse otherwise.
     */
    TBool IsSingleEventSelected();

    /**
     * Are multiple events selected in this view.
     * @return ETrue if multiple events are selected, EFalse otherwise.
     */
    TBool IsMultipleEventsSelected();
    
    /**
     * Is event selected. Can be single or multiple event.
     * @return ETrue if event is selected, EFalse otherwise.
     */
    TBool IsEventSelected();

    /**
     * Implementation of Select command.
     */
    void SelectImplL();

    /**
     * Implementation of Back command.
     */
    void BackImplL();

    /**
     * Update time format (after locale change)
     */
    void UpdateTimeFormatL();

    /**
     * @returns ETrue if in Visual Selection mode.
     */
    TBool IsVisualSelectionMode();

    /**
     * @returns the number of selected slots in Visual Selection mode
     */
    TInt SelectedSlotsNumber();

    /**
     * Releases slots visual selection.
     * 
     * @param aFocusSlot ETrue if the view should go back to the VS starting slot.
     */
    void ReleaseVisualSelectionL( TBool aFocusStartSlot = ETrue );

    /**
     * Gets visual selection start and end times.
     * @param aInitialCalTime - default start time of event
     * @param aInitialCalEndTime- default End time of event
     */
    void GetStartEndTimeL(
            TCalTime& aInitialCalTime,
            TCalTime& aInitialCalEndTime);
    
    /**
     * Returns ETrue if event has geo coordinates else EFalse
     */
    TBool EventHasMapLocationL();
	
	/**
     * Returns ETrue if event has location text else EFalse
     */
    TBool EventHasLocationTextL();
    
    
    //view population
    
    /**
     * Get instance list
     */
    void GetInstanceListL();

    /**
     * Create layout table
     */
    void CreateLayoutTableL();
    
    /**
     * Completes view population.     
     */
    void CompletePopulationL();

// from base class CAlfControl

    /**
     * From CAlfControl.
     * Captures keypresses and other messages for this control.
     * @param aEvent The received event.
     * @return ETrue if the event was consumed, EFalse otherwise.
     */
    TBool OfferEventL(const TAlfEvent& aEvent);
    
    /**
     * Handle pointer event.
     * 
     * @param aPointerEvent Pointer event object
     * 
     * @return ETrue if the event was handled.
     */
    TBool HandlePointerEventL( const TAlfEvent& aPointerEvent );
    
    
// from base class CBCContainerBase

    /**
     * From CBCContainerBase.
     * Handle view cycling.
     */

    void HandleViewCyclingL();

    /**
     * From CFSCalenAlfContainer.
     * Refresh the layout. This is called when display size is changed.
     * The content of the view is not changed.
     */
    void RefreshLayoutL();
    

// from CBCContainerBase
    /**
     * From CBCContainerBase.
     * Called when the date is changed.
     */
    void NotifyChangeDateL();
    
    /**
     * Handles "Editor closed" (canceled) notification.
     */
    void OnEditorClosedL();


// From MFSCalenPreviewPopupClient

    /**
     * Makes a scroll down request for the view.
     * Returns a boolean value whether or not it is possible.
     * @param aPixels Amount of additional vertical pixels needed
     *                for preview popup.
     * @return ETrue if view is able to complete the request,
     *         EFalse if it is not.
     */
    TBool MakeScrollDownRequestL( const TInt aPixels );

protected:
    /**
     * From CAlfControl
     * Called when the layout has changed.
     */
    void VisualLayoutUpdated( CAlfVisual& aVisual );

private:

    // New

    /**
     * Fetch new layout values from the layout manager.
     */
    void FetchLayoutValues();

    /**
     * Consruct control bar.
     */
    void ConstructControlBarL();

    /**
     * Remove control bar.
     */
    void RemoveControlBar();

    /**
     * Construct the scrollable grid.
     */
    void ConstructScrollableGridL();

    /**
     * Construct hour selector.
     */
    void ConstructSelectorL();
    
    /**
     * Sets correct brush to selector.
     * Selector's brush depands on its state.
     */
    void SetSelectorBrushL( );
    
    /**
     * Constructs arrows for indicating events outside the visible area.
     */
    void ConstructHiddenEventsArrowsL();

    /**
     * Position the layouts on the screen.
     */
    void PositionLayouts();


    // EVENT VISUAL HANDLING

    /**
     * Create event visuals. Remove old ones if they exist.
     */
    void CreateTimedEventsL();

    /**
     * Remove event visuals.
     */
    void RemoveTimedEvents();

    /**
     * Update event visuals. Create visuals if they don't exist.
     */
    void UpdateTimedEventsL();

    /**
     * Creates event visual
     * @param aEvent Event information
     * @return Created visual
     */
    CAlfVisual* CreateTimedEventL( SCalenApptInfo& aEvent );
    
    /**
     * Adds line visual for indicating Calendar with color.
     * 
     * @param aParenLayout Parent layout for line with color.
     */
    void CreateEventColorLineL( CAlfLayout* aParenLayout );

    /**
     * Updates event visual.
     * @param aVisual The event visual.
     * @param aEvent The event information
     */
    void UpdateTimedEventL( CAlfVisual* aVisual, SCalenApptInfo& aEvent );
    
    /**
     * Updates color line.
     */
    void UpdateEventColorLine( CAlfCanvasVisual& aCanvas, SCalenApptInfo& aEvent,
    		                  TRect& aInnerRect );

    // UNTIMED EVENTS HANDLING

    /**
     * Construct untimed event rows
     */
    void ConstructUntimedEventsL();

    /**
     * Update untimed event rows
     */
    void UpdateUntimedEventsL();

    /**
     * Add extra rows to the day grid
     */
    void AddExtraLineToGridL();

    /**
     * Remove extra rows from the day grid
     */
    void RemoveExtraSlotsL();


    // HOUR SELECTION HANDLING

    /**
     * Update view highlight according to CCalenState.
     */
    void SetHighlightFromActiveContextL();

    /**
     * Update hour selector size and position to correct time slot or event.
     */
    void UpdateSelectorL( TInt aLayoutTime = KLayoutTime,
                          TBool aAllowScroll = ETrue,
                          TBool aRefreshCurrent = EFalse );

    /**
     * Focus event
     * 
     * @aLayoutTime Time for selector update
     * @aRefreshCurrent ETrue to refresh currently focused event.
     */
    TBool FocusEventL( TInt aLayoutTime = KLayoutTime,
                       TBool aRefreshCurrent = EFalse );

    /**
     * Focus timeslot
     */
    void FocusSlotL( TInt aLayoutTime = KLayoutTime );

    /**
     * Update event background according to focus
     * @param aEventVisual Event visual to be updated
     * @param aFocused ETrue if event is focused.
     */
    void UpdateEventBackgroundL( CAlfVisual* eventVisual, TBool aFocused );

    /**
     * Update text color of untimed events, according to focus.
     * @param aSlotInd Slot index
     * @param aFocused ETrue if this slot is focused.
     */
    void UpdateUntimedTextColorL( const TInt& aSlotInd, TBool aFocused );

    /**
     * Return the initial text type for selected elements.
     */
    void ResetFocusTextsL();

    /**
     * Closes the preview window with or without animation.
     * @param aAnimate Animate if Etrue, no animation if EFalse.
     * @return void.
     */
    void ClosePreviewWindow( const TBool& aAnimate );

    /**
     * Select hour.
     * @param aHour Hour to be selected.
     */
    void SelectHour( TInt aHour );

    /**
     * Scroll the view up or down by one hour.
     * @param aDirection - Direction of movement
     * @param aDelta slots to move
     */
    void ScrollViewL( TScrollDirection aDirection, TInt aDelta );

    /**
     * Update the scrolling so that the view displays correct elements,
     * according to the selection. For initial opening the rule is different.
     * @param aInitialUpdate ETrue if this is the initial update.
     */
    void UpdateScrollingL( TBool aInitialUpdate = EFalse );

    /**
     * Update viewport layout values, to scroll the grid.
     */
    void UpdateViewportValues();

    /**
     * Get the slot visual for the given slot index.
     * @param aSlotIndex Index of the slot
     * @return The slot visual.
     */
    CAlfVisual& GetSlotVisual( TInt aSlot );


    // DAY SELECTION HANDLING

    /**
     * Change the date.
     * @param aDayChange Day change counted from the current date.
     */
    void ChangeDateL( const TTimeIntervalDays& aDayChange );

    /**
     * Set CalenState before changing the date.
     * @param aDay New date.
     */
    void SetActiveContextForFreshDayL( TTime& aDay );

    /**
     * Handles pressing up or down.
     */
    virtual void HandleNaviDecoratorEventL( TInt aEventID );

    // EVENT HANDLING

    /**
     * Refresh
     */
    void Refresh();

    // ARROWS

    /**
     * Update the arrows indicating events that are outside
     * the visible screen.
     * @param aForceUpdate ETrue to force update, for example if screen size changed.
     */
    void UpdateUnvisibleEventArrowsL( const TBool& aForceUpdate = EFalse );

    /**
     * Display the arrows indicating events that are outside
     * the visible screen.
     * @param aDisplayUp ETrue to display up arrow, EFalse to hide it.
     * @param aDisplayDown ETrue to display down arrow, EFalse to hide it.
     * @param aForceUpdate ETrue to force update, for example if screen size changed.
     */
    void DisplayScrollArrowsL( const TBool& aDisplayUp,
                               const TBool& aDisplayDown,
                               const TBool& aForceUpdate );

    // VISUAL SELECTION

    /**
     * Selects the focused empty timeslot.
     */
    void SelectFocusedTimeSlotL();

    /**
     * Constructs slots visual selection.
     */
    void ConstructVisualSelectionL();

    /**
     * Update Visual Selection when display size is changed.
     */
    void UpdateVisualSelectionL();
    
    /**
     * Updates Visual Selection area in a given time.
     * 
     * @param aTransitionTime Time for VS update.
     */
    void UpdateVisualSelectionL( TInt aTransitionTime );
    
    /**
     * Handle key in Visual Selection mode - e.g. expand the VS.
     * 
     * @param aCode Key code.
     */
    void HandleKeyVisualSelectionL( TKeyCode aCode );
    
    /**
     * Expand Visual Selection area.
     * 
     * @param aDelta Distance to move focused VS' edge.
     * @param aMoveTime Duration of the movement.
     */
    TBool ExpandVisualSelectionL( TInt aDelta, TInt aMoveTime = KLayoutTime );
    
    /**
     * Align VS to the nearest slot.
     */
    void AlignVisualSelectionToFullSlotL();
    
    //TOUCH
    
    /*
     * Get index of a slot pointed by aPosition
     * 
     * @param aPosition
     * 
     * @return Index of a slot; KErrNotFound if the there's no slot at aPosition.
     */
    TInt GetSlotIndex( TPoint aPosition );
    
    /**
     * Returns whether a point is within event area.
     * 
     * @param aPoint Point to test.
     * @return ETrue Point is withing event area; EFalse otherwise.
     */
    TBool IsInEventArea( const TPoint& aPoint );
    
    /**
     * Sets selector's state.
     * Selector can be in Normal, Pressed or in Off state.
     * 
     * @param aState
     */
    void SetSelectorStateL( TSelectorState aState );
    
    /**
     * The function checkes whether the selector is visible.
     * 
     * @return ETrue if selector is visible, EFalse otherwise
     */
    TBool IsSelectorEnabled() const;
    
    /**
     * Returns selector's state.
     */
    TSelectorState SelectorState();
    
    /**
     * Enables focus with key event.
     * Focus appears when navikeys or selection key is pressed. 
     * 
     * @return ETrue if focus was not visible and was turned on.
     *         EFalse focus was visible, event was not a key event
     *                or pressed key does not turn the focus on.
     */
    TBool EnableSelectorWithKeyEventL( const TAlfEvent& aEvent );
    
    /**
     * Handle start of a gesture.
     * 
     * @param aEvent Gesture start event
     */
    void HandleGestureStartL( const GestureHelper::MGestureEvent& aEvent );
    
    /**
     * Hanlde drag event.
     * 
     * @param aEvent Drag event.
     */
    void HandleDragEventL( const GestureHelper::MGestureEvent& aEvent );
    
    /**
     * Handle release of a gesture.
     * 
     * @param aEvent Release event
     */
    void HandleGestureReleasedL( const GestureHelper::MGestureEvent& aEvent );
    
    /**
     * Handle single tap event.
     * 
     * @param aEvent Single tap event
     */
    void HandleSingleTapL( const GestureHelper::MGestureEvent& aEvent );
    
    /*
     * Handles long tap event.
     * 
     * @param aPointerEvent Pointer event to be handled.
     */
    void HandleLongTapL( const TPointerEvent& aPointerEvent );
    
    /**
     * Handle short/long tap event.
     * 
     * @param aLongTap ETrue if long tap is handled
     * @param aCurrentPoint Point where tap appeared.
     * @param aPointedVisual Visual pointed with tap.
     */
    void HandleTapL( TBool aLongTap, TPoint aCurrentPoint, CAlfVisual* aPointedVisual );
    
    /**
     * Selects pointed slot.
     * 
     * @param aCurrentPoint Position pointed by a tap.
     */
    void SetHighlightFromTapL( const TPoint& aCurrentPoint );
    
    /**
     * Handle swipe left gesture.
     * @param aEvent Swipe left gesture event
     */
    void HandleSwipeLeftL( const GestureHelper::MGestureEvent& aEvent );
    
    /**
     * Handle swipe right gesture.
     * @param aEvent Swipe right gesture event
     */
    void HandleSwipeRightL( const GestureHelper::MGestureEvent& aEvent );
    
    /**
     * Sets a focus to specified arrow.
     * If the arrow is outside the visible area then the view is updated to show it.
     * The focus is displayed or not depanding on current focus visibility in view.
     * 
     * @param aFocusDownArrow ETrue - focus Down arrow. EFalse - focus Up arrow.
     */
    void FocusVSArrowL( TBool aFocusDownArrow );
    
    /**
     * Sets whether keyboard/touch is in use.
     * @aTouchActive ETrue if keyboard is in use.
     */
    void SetKeyboardActive( TBool aActive );

    /**
     * Returns whether keyboard is in use.
     */
    TBool IsKeyboardActive();
    
    /**
     * Opens event. Draws pressed selector, gives haptic feedback.
     * 
     * @return ETrue if event was selected. EFalse otherwise(no event focused).
     */
    TBool DoSelectEventL();

    /**
     * Handles long-tap event selection. Displays stylus popup; draws selector.
     * 
     * @param aPoint A point where long tap occured. Relative to visible area.
     * @return ETrue if event was selected.
     *         EFalse if no event is Focused or there are no item-specific option menu items.
     */
    TBool DoLongSelectEventL( TPoint& aPoint );
    
    /**
     * Selects Visual Selection area. 
     * Starts editor; gives haptic feedback.
     * 
     * @return ETrue if VS was selected.
     *         EFalse if VS is not active.
     */
    TBool DoSelectVisualSelectionL();
    
    //HAPTIC FEEDBACK
    
    /**
     * Gives direct haptic feedback.
     * 
     * @param aType The logical feedback type.
     */
    void InstantFeedback( TTouchLogicalFeedback aType );
    
    /**
     * 
     */
//    void InstantFeedback( const CCoeControl* aControl, 
//                          TTouchLogicalFeedback aType );
    
private:

    void RefreshL();
    
    void FreezeLayout( TBool aFreeze );
    
    /**
     * @brief Find calendarinfo object based on calendar name
     * 
     * @param aName calendar filename
     * @param aCalendarInfo referance for calendarinfo
     * @return TBool find status
     */
    static TBool CalendarInfoIdentifierL(const HBufC* aName,
            const CCalCalendarInfo& aCalendarInfo);
    
    /**
     * Set view to new position(move it)
     * @param aPos new pos
     * @param aTransitionTime duration time in moving
     */
    void SetViewPos( const TAlfRealPoint& aPos, TInt aTransitionTime );
    
    /**
     * The function retrieves summary + location from cal entry.
     */
    HBufC* SummaryLocationCustomL( const CCalEntry& aEntry, const TInt aResId );
    
    /**
     * The function cleans text from special chars.
     */
    void TrimAndCleanText( TDes& aTarget );
    
    /**
     * 
     */
    void ConvertToDate( TTime& aTime );
    
private: // data

    /**
     * Current date
     */
    TTime iDate;

    // Layouts:

    /**
     * Root layout of all layouts and visuals
     * Own. Base class takes care of destroying.
     */
    CAlfDeckLayout* iParentLayout;

    /**
     * Base layout for positioning the static elements of the view
     * Own. Base class takes care of destroying.
     */
    CAlfAnchorLayout* iBaseLayout;

    /**
     * Layout for control bar (lunar data)
     * Own. Base class takes care of destroying.
     */
    CAlfGridLayout* iControlBar;

    /**
     * Layout containing the scrollable area (grid).
     * Own. Base class takes care of destroying.
     */
    CAlfViewportLayout* iScrollableLayout;

    /**
     * Parent layout for event visuals.
     * Own. Base class takes care of destroying.
     */
    CAlfDeckLayout* iEventParentLayout;

    /**
     * Parent layout for hour selector. Needed for correct
     * positioning of the selector.
     * Own. Base class takes care of destroying.
     */
    CAlfDeckLayout* iSelectorParentLayout;
    
    /**
     * Layout for Visual Selection.
     * Own.
     */
    CAlfDeckLayout* iVisualSelectionLayout;
    
    /**
     * Layout for positioning the grids inside scroll area
     * Own. Base class takes care of destroying.
     */
    CAlfAnchorLayout* iGridParentLayout;

    /**
     * Calendar day grid layout
     * Own. Base class takes care of destroying.
     */
    CAlfGridLayout* iTimeGridLayout;

    /**
     * Calendar day grid layout
     * Own. Base class takes care of destroying.
     */
    CAlfGridLayout* iGridLayout;

    /**
     * Layout for extra slots
     * Own. Base class takes care of destroying.
     */
    CAlfGridLayout* iExtraSlotParent;
    CAlfGridLayout* iExtraTimeSlot;
    CAlfGridLayout* iExtraSlotGrid;
    TInt iExtraSlotBorderIndex;

    /**
     * Selector.
     * Own. Base class takes care of destroying.
     */
    CAlfImageVisual* iSelectorVisual;

    /**
     * Values needed for positioning the elements on the screen
     */
    TInt iControlBarHeight;
    TInt iTimeGridWidth;
    TInt iAlldayEventWidth;
    TInt iSelectionSpaceOnRight;
    TInt iEventMargin;
    TSize iArrowSize;
    TInt iSlotHeight; // height of one slot + one pixel for the line in between
    TInt iEventColorLineWidth;

    /**
     * Amount of visible hours, depends on screen resolution
     */
    TInt iVisibleSlots;

    /**
     * Amount of extra slots in the grid, and their position in the grid.
     */
    TInt iExtraSlots;
    TInt iExtraSlotGridPos;

    /**
     * Topmost visible slot.
     */
    TInt iTopVisibleSlot;

    /**
     * Calendar instance array for current day.
     * Destroyed after internal data structures created.
     */
    RPointerArray<CCalInstance> iInstanceList;

    /**
     * Event info array for current day.
     * OWN
     */
    CArrayFixFlat<SCalenApptInfo>* iLayoutTable;

    /**
     * Event storage. OWN
     */
    CBCSlotInfoStorage* iSlotStorage;

    /**
     * List of event visuals.
     */
    RPointerArray<CAlfVisual> iApptVisualList;

    /**
     * Preview popup window for focused event.
     * Own.
     */
    CBCPreviewPopup* iPreviewPopup;

    /**
     * Arrow visuals for indicating events outside the screen.
     * Own.
     */
    CAlfVisual* iScrollArrowUp;
    CAlfVisual* iScrollArrowDown;

    /**
     * Pointer to event that was previously focused.
     * Ref
     */
    CAlfVisual* iPreviouslyFocusedEvent;

    /**
     * Untimed event that was previously focused.
     */
    TInt iPreviouslyFocusedUntimedSlot;

    /**
     * ETrue if the view is just opening.
     */
    TBool iViewOpening;

    /**
     * Selected slot. Needed when screen size is changed.
     */
    TInt iSelectedSlot;

    /**
     * To save the previus gridIndex value
     */
    TInt iPreviousGridIndex;
    
    //touch stuff

    /**
     * Pointer to physics.
     * Own.
     */
    CBCDayPhysicsHandler* iPhysics;
    TSize iWorldSize;

    /**
     * Physics view size.
     */
    TSize iViewSize;

    /**
     * Horizontal scroll pos.
     */
    TInt iHorScrollPos;

    /**
     * Vertical scroll index.
     */
    TInt iVerScrollPos;

    /**
     * Pointer event start position.
     */
    TPoint iStartPoint;

    /**
     * Pointer event drag position.
     */
    TPoint iDragPoint;
    
    /**
     * Pointer to item action menu.
     * Not own.
     */
    CAknItemActionMenu* iItemActionMenu;
    
    /**
     * keyboard/touch in use.
     */
    TBool iKeyboardActive;
    
    /**
     * Selector's state
     */
    TSelectorState iSelectorState;
    
    /**
     * Gesture helper provides functionality to convert a stream of pointer 
     * events into a logical gesture.
     * Own.
     */
    GestureHelper::CGestureHelper* iGestureHelper;
    
    /**
     * Pointer to the feedback object. 
     * Not owned.
     */    
    MTouchFeedback* iFeedback;
    
    ////////////////////////////Visual Selection////////////////////////////////
    /**
     * Visual Selection.
     */
    TBool iVSMode;
    
    /**
     * Drag time for Visual Selection.
     */
    TTime iVSDragTime;

    TInt iVSSelectedSlot;
    TInt iVSSelectedRegion;
    TInt iVSSelectedColumnIdx;
    TInt iVSStartSlot;
    TInt iVSStartSlotY;
    TInt iVSEndSlot;
    TInt iVSEndSlotY;
    TInt iVSSelectedSlotsNumber;

    /*
     * Visual Selection area.
     * Own. Base class takes care of destroying.
     */
    CAlfImageVisual* iVSVisual;
    
    /**
     * Visual Selection text - "New entry"
     * Own.
     */
    CAlfTextVisual* iVSText;

    /**
     * Width of Visual Selection area;
     */
    TInt iVSAreaWidth;
    
    /**
     * Visual Selection's Upper handle - 'up' visual.
     * Own.
     */
    CAlfVisual* iVSUpperArrow;
    
    /**
     * Touch area for Visual Selection's Upper handle.
     * Own.
     */
    CAlfVisual* iVSUpperArrowTouchArea;
    
    /**
     * VS Up arrow's size.
     */
    TSize iVSUpArrowsSize;
    
    /**
     * VS Up arrow x position.
     */
    TInt iVSUpArrowXPosition;
        
    /**
     * Visual Selection's Lower handle - 'down' visual.
     * Own.
     */
    CAlfVisual* iVSLowerArrow;
    
    /**
     * Touch area for Visual Selection's Lower handle.
     * Own.
     */
    CAlfVisual* iVSLowerArrowTouchArea;
    
    /**
     * VS Down arrow's size.
     */
    TSize iVSDownArrowSize;
    
    /**
     * VS Down arrow x position.
     */
    TInt iVSDownArrowXPosition;
    
    /**
     * Focus is on lower arrow.
     */
    TBool iVSFocusOnLowerArrow;
    
	/// Ref: Lunar label provided and owned by regional plugin
	CCoeControl* iLabelControl;
    
    /////////////////////////end of Visual Selection////////////////////////////
    };

#endif // CBCDAYCONTAINER_H
