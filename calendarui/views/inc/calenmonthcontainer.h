/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Month View container control.
 *
*/



#ifndef CALENMONTHCONT_H
#define CALENMONTHCONT_H

//  INCLUDES

#include <eiklbo.h>
#include "calencontainer.h"
#include "calenmonthdata.h" // CMonthDataArray is typedef

#include <e32std.h>

// FORWARD DECLARATIONS
class CAknGrid;
class CAknIconArray;
class CAknsFrameBackgroundControlContext;
class CCalInstance;
class CCalenMonthGrid;
class CCalenMonthView;
class CEikLabel;
class TAknLayoutRect;
class CCalenPreview;


//  CLASS DEFINITIONS

/**
 *  Month View container control
 */

NONSHARABLE_CLASS( CCalenMonthContainer ) : public CCalenContainer,
                                            public MEikListBoxObserver
    {
public:  // Constructors and destructor
    /**
     * C++ constructor.
     */
    CCalenMonthContainer( CCalenNativeView* aView,
                          TTime& aDate,
                          MCalenServices& aServices );

    /**
     * Destructor.
     */
    virtual ~CCalenMonthContainer();

public:
    /**
     * Create icon indices for month view
     */
    void CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray );
    
public:     // New Function
    /**
     * Called when Midnight cross over
     * Redraw today's indicator
     */
    void ChangesMidnightCrossover();

    /**
     * Initilizes grid data.
     */
    void InitDataForMonthL();//const TTime& aNewActiveDay);

    /**
     * Population step with no instance view.
     * Step 1 of construction of grid data.
     */
    void PrepareForPopulationL();
    
    /**
     * Populates data from agenda server and sets number of notes.
     * Step 2 of construction of grid data.
     */
    void PopulateWithInstanceViewL();
    
    /**
     * Set event indicator to each day.
     * Step 3 of construction of grid data.
     */
    void SetIndicatorL();

    /**
     * Sets the current focus.
     * Step 4 of construction of grid data.
     */
    void SetFocusL();

    /**
     * Sets the current focus.
     * Step 5 of construction of grid data.
     */
    void CompletePopulationL();
    
    /**
     * Get the available info bar rect for this container
     */
    TRect InfoBarRectL( TBool aToolbarAvailable );


    /**
     * Handle navigation decorator event
     * @param left or right
     */
    void HandleNaviDecoratorEventL(TInt aDirection);
    
    void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                              const TPoint& aPenEventScreenLocation );
    void HidePopup();
    
    void RedrawPreviewPane();
    
    /**
     * returns previewpane pointer
     */    
    const MCalenPreview* PreviewPane();


private:    // New function
    /**
     * Set iActiveDay to argument's aDay,
     * and also set context's active day too.
     * And re-calculate first day of grid.
     * @param aDay New active day
     */
    void SetActiveDayL( TTime aDay );

    /**
     * Return a day from Grid index
     * @param aIndex Grid index
     * @return Real day
     */
    TTime DayOfIndex( TInt aIndex );

    /**
     * Sets cursor to Active day
     */
    void SetCursorToActiveDay();

    /**
     * Month grid cells setup method
     * @param aListBox CAknGrid
     */
    void ApplySelGridGraphicStyleL( CAknGrid& aListBox );

    /**
     * Change Active Day and Data.
     */
    void ChangeMonthL();

    void HandleDayChangeL(TTime aNewActiveDay);

    /**
     * Icon column setting for CAknGrid
     * @param aListBox Set a Grid
     * @param aIndex Cell index
     * @param aP1 Cell top-left
     * @param aSize Cell size
     */
    void SetupSelgridFormBitmapCellL(CAknGrid& aListBox, TInt aIndex,
                                     TPoint aP1, TSize aSize);
    /**
     * Drawing days of the week.
     * @param aGc Graphic context
     */
    void DrawColLabel(CWindowGc& aGc, TBool aUseWeeks = ETrue) const;
    /**
     * Drawing week numbers.
     * @param aGc Graphic context
     */
    void DrawRowLabel(CWindowGc& aGc) const;
    /**
     * Set Date(1-28,29,30,31) to iData.
     * Part of step 1 of construction of grid data
     */
    void SetDateL();

    TInt GridIndex() const;

    CCalenMonthView& MonthView() const;

    static TInt EntryTimeCompare(const CCalInstance& aInstance_1, 
                                 const CCalInstance& aInstance_2);

                                
    void HandleWeekNumberTapL(const TPoint &aPosition);  


    TBool UseWeeks() const;
    
    TBool UseWeeksL() const; 
public:

    enum TMonthLayoutVariant
        {
        ENoLayoutVariantSet = 0,
        EMainCaleMonthPane,
        EBgCalePaneCp02,
        ECaleMonthDayHeadingPane,
        ECaleMonthWeekPane,
        EGridCaleMonthPane,
        ECaleMonthPaneG,
        ECaleMonthPaneG1,
        ECaleMonthPaneG2,
        ECaleMonthPaneG3,
        ECaleMonthPaneG9,
        EBgCaleHeadingPaneCp01,
        ECaleMonthPaneT1,
        ECaleMonthDayHeadingPaneT,
        ECaleMonthWeekPaneT,
        EBgCaleSidePaneCp01,
        ECellCaleMonthPane,
        ECellCaleMonthPaneG1,
        ECellCaleMonthPaneT1
        };

    /**
     * Container can request current variant layout index
     */
    TInt LayoutVariantIndex(TMonthLayoutVariant aLayout) const;

private:    // From CCalenContainer
    /**
     * From CCalenContainer Date cahnge operation notification handler.
     * (i.e. GotoDate).
     */
    void NotifyChangeDateL();

private:    // From CCoeControl
    /**
     * From CCoeControl return number of components
     */
    TInt CountComponentControls() const;
    /**
     * From CCoeControl retrun the component specified by aIndex
     */
    CCoeControl* ComponentControl(TInt aIndex) const;
    /**
     * From CCoeControl processing of a key event
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                TEventCode aType);

    /**
     * From CCoeControl Handle pointer event
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);


    /**
     * From CCoeControl drawing month view
     */
    void Draw(const TRect& /*aRect*/) const;

    /**
     * Pass skin information if needed.
     */
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    /**
     * From CCoeControl Gets help context
     * @param aContext Help context
     */
    void GetHelpContext(TCoeHelpContext& aContext) const;

    /**
     * From CCoeControl Resizes child controls
     */
    void SizeChanged();
    void FocusChanged( TDrawNow aDrawNow );

    /**
     * From CCoeControl Handles a resource relative event
     */
    void HandleResourceChange(TInt aType);

private:    // From CCalenContainer
    /**
     * From CCalenContainer Third phase constructor.
     * This function was called CCalenView::ConstructL().
     */
    void ConstructImplL();

    /*
      Constructs heading/side/background skin context if enabled   
    */    
    void ConstructBackgroundContextL();

    /**
     * From CCalenContainer.
     * This methods is called after calendar settings have changed.
     * Implementation for month view without week numbers.
     */
    void UpdateSize();    

    /**
     * Handles list box events.
     *
     * This function is invoked by CEikListBox to
     * notify the observer of list box events.
     *
     * @param aListBox   The originating list box.
     * @param aEventType A code for the event. Further information 
     *                   may be obtained by accessing the list box itself.
     */
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType );


private:    // own methods

    // just a wrapper for leaves
    void DoSizeChangedL();

    // just a wrapper for leaves
    static TInt DoEntryTimeCompareL( const CCalInstance& aInstance_1, 
                                     const CCalInstance& aInstance_2 );

    TKeyResponse HandleAttemptToMoveL( TUint aKeyCode );
    
private:  // New variables

    /* 
     * This enumeration index should match
     */ 
    enum TIconIndex
        {
        EIconNull = -1, // ENoEvent
        EIconMany, // EManyEvents 
        EIconGrayed
        };

    enum TCompareResult
        {
        EEqual       =  0, // 1. == 2.
        ELessThan    = -1, // 1. < 2.
        EGreaterThan =  1  // 1. > 2.
        };


    CCalenMonthGrid* iGrid; // date's grid

    CMonthDataArray* iMonthDataArray;
    TTime   iFirstDayOfGrid;
    TInt    iTempMonth; // for short cut to go to a month
    TBuf<16> iDayFormat;

    CAknsFrameBackgroundControlContext* iBackgroundSkinContext;
    
    MCalenPreview* iPreview;

    // View specific data, references to views data
    TTime& iDate;
    TTime iOldFirstDayOfGrid;
	TBool iChangeMonth;
	TBool iChangeDay;
    };

#endif //CALENMONTHCONT_H

// End of File
