/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container control for Week View .
 *
*/


#ifndef CALENWEEKCONT_H
#define CALENWEEKCONT_H


#include <calsession.h>
#include <calenconstants.h>
#include <AknUtils.h>
#include <calcommon.h>  // CalCommon::TCalViewFilter
#include <caleninstanceid.h>            // TCalenInstanceId
#include <gesturehelper.h>

//  INCLUDES
#include "calencontainer.h"
#include "calenweekcallback.h"
#include "calenweekdata.h"
#include "calenglobaldata.h"

// FORWARD DECLARATIONS
class CAknsFrameBackgroundControlContext;
class CCalenWeekListbox;
class CCalenWeekView;
class CCalInstance;
class CCalHourItem;
class CGulIcon;
class TAknWindowLineLayout;
class TCalenInstanceId;
class CCalInstanceView;
class MCalenPreview;

namespace GestureHelper
    {
    class CGestureHelper;
    }

typedef CArrayFixFlat<CCalHourItem*>* CWeekSlotArrayPtr;

NONSHARABLE_CLASS (CCalHourItem):public CBase
    {
public:
    ~CCalHourItem();
    static CCalHourItem* NewL();
    static CCalHourItem* NewL(CCalInstance& aInstance);

    // FIXME: can be removed:
    void SetDataL(  CCalInstance& aInstance,
                    TTimeIntervalMinutes aStartTime,
                    TBool aTimedNote,
                    TCalenWeekHour aHourData);

    TBool HasInstance() const;
    TBool IsTimed() const;
private:
    CCalHourItem();
    void ConstructL(CCalInstance& aInstance);
    void ConstructL();

public:
    TTimeIntervalMinutes iStartTime;
    CCalInstance* iInstance;
    TBool iTimedNote;
    TCalenWeekHour iHourData;
    
    
    };


/**
 *  Container control for Week View.
 */
NONSHARABLE_CLASS( CCalenWeekContainer ) : public CCalenContainer,
                                           public MCalenWeekCursorObserver,
                                           public MEikListBoxObserver,
                                           public GestureHelper::MGestureObserver
    {
public:
    enum TLocaleChangeRedraw
        {
        ERedrawAll,
        ERedrawWeek
        };

public:  // Constructors and destructor
    /**
     * C++ constructor.
     */
    CCalenWeekContainer( CCalenNativeView* aView,
                         TTime& aTime,
                         TInt& aSelectedRowNumber,
                         TTimeIntervalMinutes& aFirstRowTime,
                         MCalenServices& aServices );

    /**
     * Destructor.
     */
    virtual ~CCalenWeekContainer();

public:
    /**
     * Create icon indices
     */
    void CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray );
    
public:  // New Function
    /**
     * Check with the layoutmanager for any layout chnanges
     */
    void CheckLayoutAndExtensionL();

    /**
     * Sets cursor in Active day
     */
    void SetCursorToActiveDayL();

    /**
     * Redrawing when locale change
     * @param aFlag Redraw flag: ERedrawAll | ERedrawWeek
     */
    void RedrawLocaleChangeL(TLocaleChangeRedraw aFlag);

    /**
     * Set Active context based on currently focused cell.
     * @param aInstAvailable Flag to find instance available
     */
    void SetActiveContextFromHighlightL(TBool aInstAvailable = ETrue);

    /**
     * Step 1 of construction of grid data.
     */
    void BeginPopulationWithInstanceViewL();
    
    /**
     * Step 2 of construction of grid data.
     */
    void FirstPopulateOfSlotTableL();

    /**
     * Step 3 of construction of grid data.
     */
    void SecondPopulateOfSlotTableL();
    
    /**
     * Step 4 of construction of grid data.
     */
    void CompletePopulationL();

    /**
     * Hides preview popup/preview pane
     */
    void HidePopup();
    /**
     * Search for top row.
     */	
    TInt SearchTopIndexL(TInt aColumn);  

    /**
     * Updates preview popup/preview pane
     */
    void UpdatePreviewPaneL();
    
    /*
     * @brief Find calendarinfo object based on calendar name
     * 
     * @param aName calendar filename
     * @param aCalendarInfo referance for calendarinfo
     * @return TBool find status
     */
    static TBool CalendarInfoIdentifierL(const HBufC* aName,
            const CCalCalendarInfo& aCalendarInfo);
    
	/*
	* Cleanup instances
	*/
    void CleanupInstances();
    
    /**
     * returns previewpane pointer
     */
    const MCalenPreview* PreviewPane();
        
public:
    /**
     * Handles navi decorator event in week view
     * @param aDirection left or right
     */
    void HandleNaviDecoratorEventL(TInt aDirection);
    void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                              const TPoint& aPenEventScreenLocation );
    
    /**
     * Handle the gesture event
     * @param aEvent event describing the gesture 
     */
    virtual void HandleGestureL( const GestureHelper::MGestureEvent& aEvent );
    
private:    // New Function
    TInt HighlightRowFromActiveContextL(TInt aColumn);

    /**
     * Sets highlight item in ListBox at active time
     */
    void SelectHighlightedCellAndVisibleRangeL();

    /**
     * Get the available info bar rect for this container
     */
    TRect InfoBarRectL( TBool aToolbarAvailable );   
    
    /**
     * Calculates and Sets day from ListBox column
     */
    void CalcDayFromColumn();

    /**
     * Populate whole week daylist from Agenda server
     */
    void PopulatesDayListsL();

    /**
     * Fills slot table with timed notes.
     */
    void InsertTimedNotesToSlotTableL();

    /**
     * Fills slot table with non-timed notes
     */
    void InsertNonTimedNotesToSlotTableL();

    /**
     * Sets Week data to ListBox
     */
    void SetListBoxDataL();

    /**
     * The note contained in within a time is searched.
     * ex)
     * aDayList (Note A 9:00-, Note B 9:30-, Note C 10:00)
     * aTime 9:00
     * Return list (Note A 9:00-, Note B 9:30-)
     * @param aDayList Data list of one day
     * @param aTime within a time
     */
    void SearchMatchedNoteL(RPointerArray <CCalInstance>& aDayList, TTime aTime);

    /**
     * Check specified entry whether Timed Note.
     * @param entry Check a note
     * @return ETrue : Timed note
     */
    TBool IsTimedNoteL(CCalEntry& aEntry);

    /**
     * Reset slot time table.
     */
    void ResetSlotTable();

    /**
     * Create slot table
     */ 
    void InitSlotTableL();

    TInt FindFirstItemOfColumn(TInt aColumn);
    TInt FindItemFromColumnL(const TCalenInstanceId& aId, TInt aColumn);
    //TInt FindFirstRowOfInstanceL(const TCalenInstanceId& aId, TInt aColumn);
    TInt FindRowForTime(TTimeIntervalMinutes aTime);

    /**
     * Sets highlight item in ListBox
     */
    void SetHighlightAndVisibleRange(TInt aRow, TInt aColumn, TInt aTopLine);

    /**
     * Adjust active time when item is updated
     * @param aId Note Id
     */
    //void AdjustActivetimeL(const CCalEntry& aEntry,const TCalTime& aInstanceTime);

   
    /**
     * Clear entry of array for cash
     */
    void ClearOneDayEntryCache();

    //TInt SearchTopIndexL( TInt aColumn );

    /**
     * Gets time of top item in ListBox
     * @return time of top item in ListBox
     */
    TTimeIntervalMinutes TopTime() const;

    /**
     * Arrow key pressed
     */
    TKeyResponse HorizontalMoveL(TInt aDir);


    void PointerMovedL(TInt aNewColumn);
    void HorizontalWeekMoveL(TInt aDir);


    void FocusChangeForPopupL();

    /**
     * Draw day names
     * @param aGc Graphic context
     */
    void DrawDayNamesL(CWindowGc& aGc) const;
    void DrawBackground(CWindowGc& aGc) const;
    void DrawGridLines(CWindowGc& aGc) const;

    CCalenWeekView& WeekView() const;    

    /**
     * Called when calendar settings changed. This methods deletes or create
     * preview pane depends on settings.
     */
    void HandleDisplayPreviewPaneSettingsChangeL();     
    
private:    // From CCalenContainer
    /**
     * From CCalenContainer.
     * (no implementation)
     * This methods is called after calendar settings have changed.
     */
    void UpdateSize();

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
     * From CCalenContainer Date cahnge operation notification handler.
     */
    void NotifyChangeDateL();

    /**
     * Notify that item is added or changed.
     * @param aEntry is the entry added or modified.
     * @param aInstanceTime is time of the modified instance.
     */ 
    //void NotifyEntryChangeL(const CCalEntry& aEntry,const TCalTime& aInstanceTime);

    /**
    * Calling this will force to resource change to happen
    * We will use the last requested resource change type.
    */
    void ForceResourceChange();

private:    // From CCoeControl
    /**
     * From CCoeControl Resizes child controls
     */
    void SizeChanged();
    void FocusChanged( TDrawNow aDrawNow );

    /**
     * React to skin, dynamic layout and other resource changes 
     */
    void HandleResourceChange(TInt aType);

    /**
     * From CCoeControl Return components count
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl Retrun the component specified by aIndex
     */
    CCoeControl* ComponentControl(TInt aIndex) const;

    /**
     * From CCoeControl processing of a key event
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);


    /**
     * From CCoeControl Handle pointer event
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    /**
     * From MEikListBoxObserver handle listbox event
     */
    void HandleListBoxEventL(
            CEikListBox* aListBox, TListBoxEvent aEventType );

    /**
     * From CCoeControl Drawing day name
     */
    void Draw(const TRect& aRect) const;

    /**
     * Pass skin information if needed.
     */
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    /**
     * From CCoeControl Gets help context
     * @param aContext Help context
     */
    void GetHelpContext(TCoeHelpContext& aContext) const;

    
private:    // From MActiveStep
    /**
     * From MActiveStep. Updates WeekView data
     */
    //TInt DoStepL();

private:    // From MCalenWeekCursorCallBack
    TKeyResponse CursorMovedL(MCalenWeekCursorObserver::TCursorMove aDir);

private:  // New variables
    CCalenWeekListbox* iListBox;
    CDesCArrayFlat* iDesArray;  // ListBox data
    RPointerArray<CCalInstance> iIdList[KCalenDaysInWeek];
    CWeekSlotArrayPtr iSlotTable[KCalenDaysInWeek];
    RPointerArray<CCalInstance> iEntryCache;
    RPointerArray<CCalInstance>    iOneDayEntryCache; 
    MCalenPreview* iPreview;

  
    TTime   iStartDay;          // first day of week of iCurrentDay
    TInt    iColumn;

    // In current week view, number of rows reserved for non-timed 
    // notes
    TInt iNonTimedAreaRowCount;

    CAknsFrameBackgroundControlContext* iBackgroundSkinContext;

    // Cached layout values (used in many places)
    TAknLayoutRect i_listscroll_cale_week_pane;
    TBool iViewPopulationComplete;
    
    // View specific data, references to the view
    TTime& iTime;
    TInt& iSelectedRowNumber;      // The row number of the highlighted cell.
    TTimeIntervalMinutes& iFirstRowTime;  // The offset of the topmost row on screen to the first row.
    
    TBool iTopRowDefault;
    TBool iRow;
    TBool iValidDay;
    TBool iHourChange;
   
    /// Own: Gesture helper
	GestureHelper::CGestureHelper* iGestureControl;
	/// Own: this variable indicates that gesture event was handled.
	TBool iGestureHandled;
	/// Own: indicates that multi touch has started and is in progress.
	TBool iMultiTouchStarted;
	
	// When doing pinch zoom on week view, focus is changed to a random 
	// location. There is need to restore start day and selected row and column index
	// that was previously selected(focused).	
	/// Own: previously selected(focused) time. 
	TTime iPreviousStartDay;
	/// Own: previously selected row index on week.
	TInt iPreviousRow;
	/// Own: previously selected column index on week.
	TInt iPreviousColumn;
	/// Own: previously selected item index on week view.
	TInt iPreviousItemIndex;
    };

#endif //CALENWEEKCONT_H


// End of File
