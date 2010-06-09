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
* Description:  For Day view container of calendar application.
 *                The class derived from CCalenContainer.
 *
*/


#ifndef CALENDAYCONTAINER_H
#define CALENDAYCONTAINER_H

//  INCLUDES
#include <eiklbo.h>
#include <caleninstanceid.h>            // TCalenInstanceId


#include "calencontainer.h"
#include "CalendarVariant.hrh"

// CONSTANTS
const TInt KIndexError = -1;

// FORWARD DECLARATIONS
class CCalenDayListBoxModel;
class CCalenDayListBox;
class CCalenDayListBoxView;
class CCalenDayView;
class CCalInstance;
class CAknsFrameBackgroundControlContext;
class TCalenInstanceId;
//  CLASS DEFINITIONS
/**
 * CCalenDayContainer container control class for DayView.
 */
NONSHARABLE_CLASS(  CCalenDayContainer ) : public CCalenContainer,
                                                   public MEikListBoxObserver
    {
public: // Data

    NONSHARABLE_STRUCT( SItemInfo )
        {
        TTimeIntervalMinutes iStartTime;
        TCalenInstanceId iId;
        TBool iTimedNote;
        TInt iTopLine;
        TInt iBottomLine;

        // Fetched instance is temporarily stored to iTmpInstance 
        // This is destroyed after construction phase is over.
        CCalInstance* iTmpInstance;
        TUint32 iColor;
        };

public:  // Constructors and destructor
    /**
     * C++ default constructor.
     */
    CCalenDayContainer( CCalenNativeView* aView,
                        TTime& aDate,
                        TInt& aHighlightedRowNumber,
                        TInt& aFirstEntryOnScreenIndex,
                        MCalenServices& aServices );
    /**
     * Destructor.
     */
    virtual ~CCalenDayContainer();

public: // From CalenContainer
    /**
     * Create icon index
     */
    void CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray );
	
    /**
     * Handles list box events.
     * @param aListBox   The originating list box.
     * @param aEventType A code for the event.
     */
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
    
public:     // New function
    /**
     * Refresh Day view data and draws
     */
    void UpdateContainerL();

    /**
     * Update time format (12/24)
     */
    void UpdateTimeFormat();

    /**
     * @return ETrue, if day view doesn't have any instances to show.
     */ 
    TBool IsEmptyView() const;
    
    /**
     * Get the available preview pane rect for this container
     */
    TRect PreviewRectL();
    
    //void SetActiveContextFromHighlightL();

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION    
    TBool IsMidnightVisualization() const;
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION
    
    /**
     * Get instances for day. List of instances is processed during construction 
     * of list box data and destroyed once it's done.
     * Step 1 of construction of listbox data.
     */
    void GetInstanceListL();
    
    /**
     * Setup layout table.
     * Step 2 of construction of listbox data
     */
    void CreateSlotTableL();
    
    /**
     * Completes population.
     * Step 3 of construction of listbox data.
     */
    void CompletePopulationL();

    /**
     * Get the available info bar rect for this container
     */
    TRect InfoBarRectL( TBool aToolbarAvailable );    

    /**
     * Sets the context based on the highlighted item.
     */
    void SetContextFromHighlightL();

    /**
     * Sets the highlighted item based on the context.
     */
    void SetHighlightFromContextL();

    /**
     * Delete note command notification handler
     */
    void NotifyDeleteItemL();
    
    /**
     * Returns ETrue if event has geo coordinates else EFalse
     */
    TBool IsEventHasMapLocationL();
	
	/**
     * Returns ETrue if event has location text else EFalse
     */
    TBool IsEventHasNoLocationTextL();
  
    /**
     * This method is called after pointer event in Navi
     * decoration object.
     * @param aDirection right or left
     */
    void HandleNaviDecoratorEventL(TInt aEventID);
    
    /**
     * Handles long tap events
     */
    void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                              const TPoint& aPenEventScreenLocation );

    /**
     * Check whether entry exists or not
     */
    TBool IsValidEntryL();
private:    // New function

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION    
    void GetMidnightVisualizationL();
#endif // RD_CALEN_MIDNIGHT_VISUALIZATION
    /** 
     * Destroy all CCalInstances. This is called during construction of view data
     * once we have constructed actual listbox data.
     */
    void DestroyInstanceListL();

    /**
     * Set Listbox data to ListBox
     * Part of step 3 of construction of listbox data
     */
    void CreateListBoxDataL();

    /**
     * Called in the final stage of population. If we've just deleted an entry,
     * update which row is highlighted
     */
    void UpdateHighlightedRowIfDeletedL();

    // FIXME document these
    //void SetActiveContextForFreshDayL(TTime& aDay);
    //void SetActiveContextFromEntryL(const CCalEntry& aEntry,const TCalTime& aInstanceTime);
    
    /**
     * Handles pressing left or right.
     */
    void HandleHorizontalScrollL( TTimeIntervalDays aDirection );

    /**
     * Handles pressing up or down.
     */
    void HandleVerticalScrollL( TInt aNewCursor );

    /**
     * Sets highlight item in ListBox
     * @param aIndex Index of ListBox
     * @param aTopItem Index of top item
     */
    void SetHighlightAndVisibleItem(TInt aIndex, TInt aTopItem = KIndexError);

    TInt FindItemIndex(const TCalenInstanceId& aId) const;

    TInt FindItemIndexForRow(TInt aListIndex) const;
    
public:
    SItemInfo& FindItem(TInt aListIndex) const;
    
    MCalenServices& Services();
    
private:

    
    /**
     * Searches list item that is nearest to aTime
     * @param aTime 
     * @return if there is such item, index in ListBox is returned
     *         othwerise returns KIndexError.
     */
    TInt SearchNearestItem(TTimeIntervalMinutes aTime) const;

    void SetVisibleLines(TInt aHighlightFirstLine,
                         TInt aPreferredTopLine);

    

    CCalenDayListBoxView* View();

    CCalenDayView& DayView() const;
    public:

    enum TDayLayoutVariant
        {
        ENoLayoutVariantSet = 0,
        EListScrollCaleDayPane,
        EPopupToolbarWindowCp02,
        EScrollPaneCp09,
        EBgCalePane,
        EListCalePane,
        EListScrollCaleDayPaneT1
        };
    TInt LayoutVariantIndex(TDayLayoutVariant aLayout) const;
    
    /**
     * From CCalenContainer.
     * (no implementation)
     * This methods is called after calendar settings have changed.
     */
    void UpdateSize();

private:    // From CCoeControl
    /**
     * From CCoeControl Resizes child controls
     */
    void SizeChanged();
    void FocusChanged( TDrawNow aDrawNow );
    void HandleResourceChange(TInt aType);

    /**
     * From CCoeControl Gets child control count
     * @return Child control count
     */
    TInt CountComponentControls() const;
    /**
     * From CCoeControl Gets child control pointer
     * @param aIndex Child control index
     * @return child control pointer
     */
    CCoeControl* ComponentControl(TInt aIndex) const;
    /**
     * From CCoeControl Process key event
     * @param aKeyEvent The key event.
     * @param aType EEventKey | EEventKeyUp | EEventKeyDown.
     * @return EKeyWasNotConsumed: Key event was not consumed.
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * From CCoeControl Handle pointer event
     * @param aPointerEvent Event to be handled.
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    /**
     * From CCoeControl Drawing day name
     * @param aRect Clipping rectangle
     */
    void Draw(const TRect& aRect) const;

    /**
     * Pass skin information if needed
     */
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    /**
     * From CCoeControl Gets help context
     * @param aContext Help context
     */
    void GetHelpContext(TCoeHelpContext& aContext) const;

private:    // From CCalenContainer
    /**
     * From CCalenContainer Third phase constructor.
     * This function was called CCalenView::ConstructL().
     */
    void ConstructImplL();


    
    /**
     * From CCalenContainer Date cahnge operation notification handler.
     * (i.e. GotoDate).
     */
    void NotifyChangeDateL();

    /**
     * Notify that item is added or changed.
     * @param aEntry is the entry added or modified.
     * @param aInstanceTime is time of the modified instance.
     */ 
    //void NotifyEntryChangeL(const CCalEntry& aEntry,const TCalTime& aInstanceTime);

    
private: // New Functions
    /** 
     * Updates icons after e.g. skin change
     */ 
    void UpdateIconsL();
    
private:    // From MActiveStep
    /**
     * From MActiveStep Updates DayView data
     * @return 0 is DoStepL() exit code
     */
    //TInt DoStepL();

private:    // Data
    CCalenDayListBox* iListBox;
    CCalenDayListBoxModel* iListBoxModel;

    RPointerArray<CCalInstance> iInstanceList;
    CArrayFixFlat<SItemInfo>* iLayoutTable;

    /*enum TStepPopulate
        {
        ENothingDone,
        EPopulationDone,
        ESlotTableDone,
        EDone
        };
    TStepPopulate iStepsCompleted;*/
    TInt iDeletedIndex;
    TBool iMidnight;
    CAknsFrameBackgroundControlContext* iBackgroundSkinContext;

    TBool iDateWasNull;
// View specific data references
    TTime& iDate;                   // Current day
    TInt& iHighlightedRowNumber;
    TInt& iFirstEntryOnScreenIndex;
    };

#endif // CALENDAYCONTAINER_H

// End of File
