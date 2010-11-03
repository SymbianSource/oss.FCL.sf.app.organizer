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
* Description:  Description:   The model part of preview plugin.
*
*/



#ifndef BCPREVIEWPANECONTAINER_H
#define BCPREVIEWPANECONTAINER_H

//  INCLUDES
#include <coecntrl.h>
#include <mcalenpreview.h>
#include <gestureobserver.h>
#include <gesturehelper.h>
#include <calennotificationhandler.h>

// FORWARD DECLARATIONS
class CEikLabel;
class MCalenServices;
class CCalInstance;
class CBCPreviewPaneData;
class CAknsBasicBackgroundControlContext;
class CBCPreviewListbox;
class CAknsFrameBackgroundControlContext;
class CAknToolbar;

using namespace GestureHelper;

NONSHARABLE_CLASS( CBCPreviewPaneContainer ) :
    public CCoeControl, public MCalenPreview, public MGestureObserver, 
    public MCalenNotificationHandler
    {
public:

   static CBCPreviewPaneContainer* NewL( MCalenServices& aServices, const TRect& aRect );

   /**
    * Destructor.
    */
    ~CBCPreviewPaneContainer();

public:
    // new functions
    void Hide();
    void FocusChangedL(const TTime& aTime);
    void FocusChangedL(CCalInstance* aInstance);
    void HandleResourceChange( TInt aType );
    /**
     * Starts the auto-scroll running. If it was already started this methods do
     * nothing. Auto-scroll will not be started if scroll distance is less or
     * equal to zero.
     */
    void StartAutoScroll();
    /**
     * Stops the auto-scroll running. If it was already stopped this methods do
     * nothing.
     */
    void StopAutoScroll();
    /**
     * Tests if auto-scroll is running.
     *
     * @return @c ETrue if auto-scroll is running. @c EFalse if auto-scroll 
     *         is not running.
     */
    TBool IsAutoScrollRunning() const;

public: //From MGestureObserver
	
    void HandleGestureL( const MGestureEvent& aEvent );
    
public: // From CCoeCOntrol
	
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
protected: // From CCoeCOntrol
    void SizeChanged();

public: // From notifcationhandler
    void HandleNotification( const TCalenNotification aNotification );

private:

    void ConstructL();
    /**
     * C++ default constructor.
     */
    CBCPreviewPaneContainer( MCalenServices& aServices, const TRect& aRect );

    void Draw(const TRect& aRect) const;
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
    /**
     * reset the components of current container.
     */
    void ResetComponents();
    /**
     * init the components of current container.
     */
    void InitComponentsL();
    /**
     * create the listbox for the preview.
     */
    void CreateListboxL();
    /**
     * create the lunar data label for the preview (only for chinese variants)
     */
    void CreateLabelL();
    /**
     * Check if the preview pane needs to scroll and if so do the scroll.
     */
    void DoPreviewAutoScroll();
    /**
	 * The call back function for auto-scrolling.
	 * @param aAny A pointer to this class.
	 */
	static TInt AutoScrollTimerCallBack(TAny* aAny);
	/**
	 * Called when focus changed in month/week view on timed or non-timed 
	 * cells.
	 */
	void DoFocusChangedL();
	
	/**
	 * Gets preview pane rect. from LAF data 
	 */
	TRect PreviewPaneRect();
	
	/**
	 * Calculates label rect.  
	 */
	TRect LabelRect();
	/**
	 * Gets the value indicating how much content is clipped outside of the 
	 * viewport. This is the distance that the viewport will move during 
	 * scrolling.
	 * 
	 * @return distance that the viewport will move during scrolling. Value less
	 *         or equal to zero means that scroll is not needed.
	 */
	TInt ScrollDistance() const;
	
	/**
	 * Gets colors from skin and sets as current for item drawer
	 */
	void SetSkinColors();
	
	/**
	 * Used to change font size for VGA screens
	 */	
	void OverrideListBoxFontsForSmallScreen();
	
private: // data

	/** Scrolling directions **/
	enum TScrollDirection
	    {
	    EScrollUp,
	    EScrollDown
	    };

    // Define the rect of preview pane
    // Get the rect area from Recal.
    TRect iPreviewRect;

    // Get the reference from Recal
    MCalenServices& iServices;

    CBCPreviewListbox* iListbox;

    CBCPreviewPaneData* iPreviewData;

    CAknsBasicBackgroundControlContext* iBgContext; // for skins support

    CAknsFrameBackgroundControlContext* iBackgroundSkinContext;

    /// Own: Gesture helper
    CGestureHelper* iGestureControl;
    /**
	 * The direction in which the preview pane should scroll.
	 */
    TScrollDirection iPreviewScrollDirection;
    /**
	 * OWN: The periodic timer used to do auto-scrolling.
	 */
	CPeriodic* iAutoScrollTimer;
	/// Own: Indicates whether the autoscroll is running or not.
	TBool iIsAutoScrollRunning;
	/// Own: used for gesture handling
	TBool iGestureHandled;
	/// Ref: Lunar label provided and owned by plugin
	CCoeControl* iLabelControl;
	/// Own: true for VGA screen, false otherwise
	TBool iIsVGA;
    };

#endif // BCPREVIEWPANECONTAINER_H

// End of File
