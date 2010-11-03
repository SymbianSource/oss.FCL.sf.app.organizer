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
* Description:  Preview popup for calendar events showing more detailed
*                information about the events.
*
*/

#ifndef CBCPREVIEWPOPUP_H
#define CBCPREVIEWPOPUP_H

//Includes
#include <e32base.h>
#include <calentry.h>
#include "cbctextstylemanager.h"
#include <alf/alfeventhandler.h>
#include <alf/alftimedvalue.h>

// Forward declarations
class CAlfVisual;
class CCalenAppUi;
class CAlfControl;
class CAlfTextVisual;
class CCalInstance;
class CAlfDeckLayout;
class CAlfAnchorLayout;
class CAlfFlowLayout;
class MBCResourceManager;

/**
 *  Interface that all preview popup clients need to implement.
 */
class MBCPreviewPopupClient
    {
public:
    /**
     * Makes a scroll down request for the client of the popup.
     * Returns a boolean value whether or not it is possible.
     * @param aPixels Amount of additional vertical pixels needed
     *                for preview popup.
     * @return ETrue if client is able to complete the request,
     *         EFalse if it is not.
     */
    virtual TBool MakeScrollDownRequestL( const TInt aPixels ) = 0;
    };


/**
 *  Preview popup for calendar events
 *  Preview popup shows more detailed information about calendar events.
 *  It resizes itself automatically due to dynamic content and positions
 *  itself also automatically depending on the position of the visual that
 *  represents the calendar event.
 *
 *  @code
 *
 *  iPreviewPopup = CBCPreviewPopup::NewL(this, iParentLayout,
 *                                             iAppui, this);
 *  iPreviewPopup->ShowPreviewPopupL( calInstance, visual );
 *
 *  iPreviewPopup->ClosePreviewPopupL( animation );
 *  delete iPreviewPopup; // delete when preview popup is no longer needed
 *
 *  @endcode
 *
 */
class CBCPreviewPopup : public CBase,
                             public MAlfEventHandler
    {
public:

    /**
     * Construct the calendar event preview popup.
     * @param aOwner Future owner of the preview popup
     * @param aParentLayout Parent layout in which the preview popup is added
     * @param aViewportLayout Viewport layout for the popup
     * @param aResManager Reference of ResourceManager.
     * @param aClient Pointer to the preview popup client.
     * @return Pointer to the preview popup.
     */
    static CBCPreviewPopup* NewL( CAlfControl* aOwner,
                                       CAlfLayout* aParentLayout,
                                       CAlfLayout* aViewportLayout,
                                       MBCResourceManager& aResManager,
                                       MBCPreviewPopupClient* aClient );
    /**
     * Construct the calendar event preview popup. Leaves a pointer
     * to the cleanupstack.
     * @param aOwner Future owner of the preview popup
     * @param aParentLayout Parent layout in which the preview popup is added
     * @param aViewportLayout Viewport layout for the popup
     * @param aResManager Reference of ResourceManager.
     * @param aClient Pointer to the preview popup client.
     * @return Pointer to the preview popup.
     */
    static CBCPreviewPopup* NewLC( CAlfControl* aOwner,
                                        CAlfLayout* aParentLayout,
                                        CAlfLayout* aViewportLayout,
                                        MBCResourceManager& aResManager,
                                        MBCPreviewPopupClient* aClient );

    /**
     * Destructor
     */
    ~CBCPreviewPopup();


    /**
     * Shows preview popup. Shows details information about
     * calendar event.
     * @param aCalInstance Pointer to the calendar instance. Owenership
     *        is transfered to preview popup.
     * @param aFocusedVisual Pointer to visual that represents the
     *        calendar event in focus.
     * @param aFocusedSlot Pointer to visual that represents the
     *        row where the focus currently is.
     * @param aIsAllDayEventInUI Boolean value that defines whether or not
     *        UI view considers event as all day event or not. Example:
     *        day X 00:00 - day X+1 15:00 is not an all day event, but view
     *        considers it as an all day event when day X is
     *        focused in the view.
     */
    void ShowPreviewPopupL( CCalInstance* aCalInstance,
                            CAlfVisual* aFocusedVisual,
                            CAlfVisual* aFocusedSlot,
                            const TBool aIsAllDayEventInUI );

    /**
     * Notify the preview popup that a different slot has been focused
     * within the currently selected event.
     * @param aFocusedSlot Pointer to visual that represents the
     *        row where the focus currently is.
     */
    void UpdateFocusedSlotL( CAlfVisual* aFocusedSlot );

    /**
     * Closes preview popup.
     * @param aAnimate With ETrue popup is closed with animation.
     *        EFalse closes popup immediately with animation. This
     *        is to be used e.g. when day is changed or day view is closed.
     */
    void ClosePreviewPopup( const TBool aAnimate );

public: // From MAlfEventHandler

    TBool OfferEventL(const TAlfEvent& aEvent);

private:

    /**
     * C++ default constructor.
     */
    CBCPreviewPopup( CAlfControl* aOwner,
                          CAlfLayout* aParentLayout,
                          CAlfLayout* aViewportLayout,
                          MBCResourceManager& aResManager,
                          MBCPreviewPopupClient* aClient );

    /**
     * 2nd phase constructor.
     */
    void ConstructL();


    /**
     * Construct the calendar event preview popup.
     * @param aEntry Calendar entry
     */
    void ConstructPreviewPopupL( const CCalEntry& aEntry );

    /**
     * Creates visuals for the popup using calendar entry
     * as input information.
     * @param aEntry Reference to Calendar entry.
     */

    void CreateVisualsL( const CCalEntry& aEntry );

    /**
     * Set the background for preview popup.
     * @param aEntry Calendar entry
     */
    void SetBackgroundL( const CCalEntry& aEntry );

    /**
     * Constructs a visual showing the title of meeting/event
     * @param aEntry Reference to Calendar entry
     * @param aParentLayout Parent layout in which the visual is added
     */
    void CreateSummaryVisualL( const CCalEntry& aEntry,
                               CAlfLayout* aLayout );

    /**
     * Constructs a visual showing start and end time of meeting/event
     * @param aEntry Calendar entry
     * @param aParentLayout Parent layout in which the visual is added
     */
    void CreateTimeVisualL( const CCalEntry& aEntry,
                            CAlfLayout* aLayout );

    /**
     * Creates a visual for showing a period of dates.
     * @param aEntry Reference to Calendar entry
     * @param aParentLayout Parent layout in which the visual is added
     */
    void CreateDateVisualL( const CCalEntry& aEntry,
                            CAlfLayout* aLayout );

    /**
     * Constructs a visual showing the location of meeting/event
     * @param aEntry Reference to Calendar entry
     * @param aParentLayout Parent layout in which the visual is added
     */
    void CreateLocationVisualL( const CCalEntry& aEntry,
                                CAlfLayout* aLayout );

    /**
     * Constructs a visual showing the description of meeting/event
     * @param aEntry Reference to Calendar entry
     * @param aParentLayout Parent layout in which the visual is added
     */
    void CreateDescriptionVisualL( const CCalEntry& aEntry,
                                   CAlfLayout* aLayout );


    /**
     * Frees memory allocated for preview popup. Used when popup is closed
     * and animated out.
     */
    void DeletePreviewPopup();

    /**
     * Initialise animation data.
     */
    void InitAnimData();

    /**
     * Animates the popup in. Used when popup is opened.
     */
    void AnimateIn();

    /**
     * Animates the popup out. Used when popup is closed.
     */
    void AnimateOut();

    /**
     * Calculates and lays out visual layout. Returns the size of preview
     * popup
     * @return Size of the whole preview popup.
     */
    TSize CalculateLayout();

    /**
     * Checks if specified time period may be considered as an all-day event.
     * @param aStartTime Start time&date of the event.
     * @param aEndTime End time&date of the event.
     */
    TBool IsAllDayEvent( const TTime& aStartTime, const TTime& aEndTime ) const;

    /**
     * Sets common style for text visuals in preview popup.
     * @param aVisual Pointer to the visual.
     * @return void
     */
    TSize SetTextVisualStyleL( CAlfTextVisual* aVisual );

    /**
     * Checks both RRULE and RDATE information in case of occurence.
     * @param aEntry Reference to the calendar enry.
     * @return Returns ETrue if entry is a recurring event.
     */
    TBool IsRecurringEventL( const CCalEntry& aEntry );

    /**
     * Checks whether the given entry is an exception to a recurring event
     * @param aEntry Reference to the calendar entry.
     * @return Returns ETrue if entry is an exception to recurring event.
     */
    TBool IsRecurringExceptionEventL( const CCalEntry& aEntry );
    
private: // Enumerations

    /**
     * Custom event types for preview popup.
     */
    enum TCustomEventType
        {
        EOpenPopup,
        EAnimateInPopup,
        EClosePopup,
        EDeletePopup,
        EAnimateOutPopup
        };

    /**
     * Enumerations representing popup states.
     */
    enum TPopupState
        {
        EPopupDeleted,
        EPopupClosing,
        EPopupOpen,
        EPopupOpening
        };

    /**
     * Animation information.
     * Coordinates in this class are in local coordinates.
     */
    class TAnimationInfo
        {
    public:
        CAlfVisual* iFocusedVisual;
        TSize iTargetSize;
        TAlfRealPoint iSourceRealPos;
        TAlfRealPoint iTargetRealPos;
        TBool iIsAllDayEvent;
        };

private: // Data

    /**
     * Owner of preview popup visuals
     * Not own.
     */
    CAlfControl* iOwner;

    /**
     * Parent layout for preview popup visuals
     * Not own.
     */
    CAlfLayout* iParentLayout;

    /**
     * The viewport layout which determines the vertical limits for
     * where the popup is placed. Not own.
     */
    CAlfLayout* iViewportLayout;
    
    /**
     * Reference MBCResouceManager.
     * Not own.
     */
    MBCResourceManager& iResManager;

    /**
     * Pointer to the client of the popup. Needed for scrolling.
     * Not own.
     */
    MBCPreviewPopupClient* iClient;

    /**
     * Pointer to preview popup
     * Not own.
     */
    CAlfDeckLayout* iPreviewPopup;

    /**
     * Pointer to layout containing each line of the preview popup.
     * Not own.
     */
    CAlfFlowLayout* iPopupLines;

    /**
     * Flow layout for time text visual and icons.
     */
    CAlfFlowLayout* iTimeAndIconsLayout;

    /**
     * Pointer to text visual showing event summary
     * Not own.
     */
    CAlfTextVisual* iSummaryVisual;
    TSize iSummaryVisualSize;

    /**
     * Pointer to text visual showing event time information
     * Not own.
     */
    CAlfTextVisual* iTimeVisual;
    TSize iTimeVisualSize;

    /**
     * Pointer to a text visual showing date information for the event.
     * Not own.
     */
    CAlfTextVisual* iDateVisual;
    TSize iDateVisualSize;

    /**
     * Pointer to text visual showing event location information
     * Not own.
     */
    CAlfTextVisual* iLocationVisual;
    TSize iLocationVisualSize;

    /**
     * Pointer to text visual showing event description
     * Not own.
     */
    CAlfTextVisual* iDescriptionVisual;
    TSize iDescriptionVisualSize;

    /**
     * Pointer to focused visual to provide animation information
     * i.e. target and source sizes.
     * Not own.
     */
    CAlfVisual* iNextFocusedVisual;

    /**
     * Pointer to the slot visual, to provide y-coordinate for the
     * source position.
     * Not own.
     */
    CAlfVisual* iNextFocusedSlot;

    /**
     * Pointer to the calendar event instance to provide
     * event details.
     * Own.
     */
    CCalInstance* iCalInstance;

    /**
     * Own:
     * Animation info. See class definition.
     */
    TAnimationInfo iAnimInfo;

    /**
     * Own:
     * Contains the state of the popup.
     */
    TPopupState iPopupState;

    /**
     * This value may be used to determine whether or not
     * the next focused event is an all day event from
     * UI view point of view.
     */
    TBool iNextFocusedIsAllDayEvent;

    /**
     * Ref:
     * Visual for shadow border brush.
     */
    CAlfVisual* iPopupShadow;

    /**
     * Ref:
     * parent frame for shadow
     */
    CAlfVisual* iPopup;
    };


#endif // BCPREVIEWPOPUP_H

// End of file
