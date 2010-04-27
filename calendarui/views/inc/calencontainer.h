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
* Description:  Container base class of Calendar application.
 *                This class is the derivation origin of Calendar container.
 *
*/


#ifndef CALENCONTAINER_H
#define CALENCONTAINER_H

//  INCLUDES
#include <babitflags.h>
#include <coecntrl.h>
#include <coemain.h>
#include <caltime.h>

#include <aknlongtapdetector.h>
#include <AknNaviDecoratorObserver.h>
#include <calennavilabel.h>

#include <calenservices.h>
#include "CalendarVariant.hrh"

// FORWARD DECLARATIONS
class CCalInstance;
class CCalEntry;
class CCalenNativeView;
class CAknIconArray;
class CAknsBasicBackgroundControlContext;
class CAknsFrameBackgroundControlContext;
class MCalenViewUiExtension;
class CEikLabel;
class MCalenContext;
class CCalenContainerLayoutManager;
class MCalenPreview;


class MAknNaviDecoratorObserver;
class MCCalenNaviLabelObserver;


class CCalenPreview;

//  CLASS DEFINITIONS

/**
 * Container base class of calendar application.
 */
NONSHARABLE_CLASS( CCalenContainer ) : public CCoeControl,
                                       public MAknNaviDecoratorObserver,
                                       public MCCalenNaviLabelObserver,
                                       public MAknLongTapDetectorCallBack

    {
public:  // Constructors and destructor
    /**
     * C++ constructor.
     */
    CCalenContainer( CCalenNativeView* aView, MCalenServices& aServices );

    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();

    /**
     * Destructor.
     */
    virtual ~CCalenContainer();

public:     // New function
    /**
     * Retrieves the date from the context. If the context is a todo in the past,
     * today is returned.
     */
    static TTime DateFromContextL( const MCalenContext& aContext );

    /**
     * Ignore Agenda server notifier.
     * iIgnoreDocChange is setted ETrue this function.
     * Then, iIgnoreDocChange is setted EFalse by last code of DoStepL()
     */
    void SetIgnoreDocuChange();

    

    /**
     * Called when an item is changed.
     * @param aId It is changed item's ID
     */
    //void DoItemChangedL(const CCalEntry& aEntry,const TCalTime& aInstanceTime,TBool aForceRefresh);

    /**
     * Third phase constructor.
     * Called from CCalendarView::ConstructL().
     */
    virtual void ConstructImplL() = 0;

    /**
     * Notify that Active date is changed. Each contianer will redraw.
     */
    virtual void NotifyChangeDateL();

   
    /**
     * Get the available info bar rect for this container
     * Default implementation returns empty rect
     */
    virtual TRect InfoBarRectL( TBool aToolbarAvailable );
    
    /**
     * Get the available preview pane rect for this container
     */
    virtual TRect PreviewRectL();
    
    /**
     * Get the view for this container
     */
    CCalenNativeView* View() { return iView; }
    
    TBool UseToolbar() const;
    TBool UseInfobar() const;
    TBool UsePreview() const;


    // From MAknLongTapDetectorCallBack
    virtual void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                                      const TPoint& aPenEventScreenLocation ) = 0;


    /**
     * Remove from Control stack and Make in-visible
     * @param aMakeInvisible if ETrue is set, container becomes invisible
     */
    void RemoveFromStackAndMakeInvisible(TBool aMakeInvisible = ETrue);

    /**
     * Add to Control stack and Make visible
     */
    void AddToStackAndMakeVisibleL();

    /**
     * Is this control exist on stack?
     * @return If value is ETrue, this control is exist on stack
     */
    TBool IsOnStack();

    /**
     * This methods is called after calendar settings have changed.
     * See day, week and month container for implementation.
     */
    virtual void UpdateSize() = 0;

    /**
    * Creates icon array according to icon index array passed in
    */
    CAknIconArray* CreateIconsL( const RArray<MCalenServices::TCalenIcons>& aIndexArray, const TInt aViewId = 0 ); 
    
    /**
     * Creates icon index array
     */
    virtual void CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray) = 0;
    
    /**
     * Get icon index in the icons array
     */
    TInt IconIndex( MCalenServices::TCalenIcons aType ) const;

    /**
     * Get icon index according to entry type
     * aAllDayEvent - allday will havew seprate icon,
     * by defalut value is EFalse if not passed
     * 
     */
    TInt IconIndexFromEntryL( CCalEntry& aEntry, TBool aAllDayEvent = EFalse ) const;
    
    

    /**
     * Determines if aEntry requires (P)riority, (A)larm, (R)ecurrence or (E)xception icons.
     * If so, it returns aDes, containing those icons indexes, as created by CalenIcon.
     * @param aEntry: A Calendar entry
     * @param aIconFormatDes: i.e. " \t%d" for ToDo view, "%d\t" for Day view
     * @param aTargetDes: A descriptor, holding icons indexes. Untouched if no Priority, Alarm,
     *                    Recurrence or Exception icons needed.
     * @param aPushSingleIconToRight: When ETrue, (aIconFormatDes - "%d") is appended to
     *          aTargetDes, when there is only one icon to display (i.e. alarm)
     *
     *  example:    init  value of aIconFormatDes = "%d\t" (Day view)
     *              init  value of aTargetDes: "3\t\t\t\ToDoWithAlarm\t"     
     *  example:    final value of aTargetDes: "3\t\t\t\ToDoWithAlarm\t\t4\t"
     *                                          (aPushSingleIconToRight = ETrue)
     *  example:    final value of aTargetDes: "3\t\t\t\ToDoWithAlarm\t4\t"
     *                                          (aPushSingleIconToRight = EFalse)
     *
     * @param aRevertOrder: When ETrue, A,R,P/E appended to aTargetDes.
     *                      When EFalse, P/E,R,A appended to aTargetDes.
     *
     * @leave with KErrArgument if aIconFormatDes does not have "%d" in it
     * @leave with KErrArgument if aIconFormatDes length is greater than 50
     * @leave with KErrBadDescriptor if there is not enough free space in aTargetDes
     *             to hold 2 icon strings with aIconFormatDes
     */
    void InsertAdditionalIconsL( const CCalEntry& aEntry,
                                         const TDesC& aIconFormatDes,
                                         TPtr& aTargetDes,
                                         TBool aPushSingleIconToRight = ETrue,
                                         TBool aRevertOrder = EFalse ) const;
                         
     /**
     * Returns the reduced size of the main pane if the
     * preview pane is displayed.
     */                                     
    TRect ReducePreview(TRect aRect) const;

    // Methods for view ui extensions
    /**
     * When focus has changed, UI container should call
     * this method. 
     */ 
    void UpdateStatusPaneAndExtensionsL();
    
	/*
	* Clean instances
	*/
    virtual void CleanupInstances(){ };
    
    /**
     * Hide/Unhide "today" toolbar item based on the focused day 
     */
    void UpdateTodayToolbarItemL();
    
	/**
     * returns previewpane pointer
     */
    virtual const MCalenPreview* PreviewPane(){return NULL; };
private: // New functions
    /**
     * Return number of icons needed to be displayed
     * @return number of icons (between 0 and 3)
     */    
    TInt NumberOfIcons( const TBool aException,
                               const TBool aRepeat,
                               const TBool aAlarm, 
                               const TBool aHasPriority ) const;
    /**
     * Check if the entry has priority (only ToDos)
     * @param aEntry : A Calendar Entry
     * @param aPriHigh : Set aPriHigh ETrue, if aEntry is high priority ToDo
     * @param aPriLow : Set aPriLow ETrue, if aEntry is low priority ToDo
     * @return ETrue if high or low priority
     * @return EFalse otherwise
     */    
    TBool NeedsPriorityIconL( const CCalEntry& aEntry,
                                     TBool& aPriHigh,
                                     TBool& aPriLow ) const;
                                     
protected:  // New function


    /**
     * Long tap animation is handled by calling this method
     */
    void NotifyLongTapDetectorL(const TPointerEvent& aPointerEvent);


protected:  // Functions from base classes
    /**
     * From CCoeControl Handle key event
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * Called when control size is changed 
     */
    void SizeChanged(const TRect& aParent);    


    // From MAknNaviDecoratorObserver
    virtual void HandleNaviDecoratorEventL( TInt aEventID ) = 0;
    // From MCCalenNaviLabelObserver
    void HandleNaviLabelEventL( TInt aEventID );


protected:    // CCoeControl
    /**
     * Handler of changing focus
     * @param aDrawNow ENoDrawNow | EDrawNow
     */
    void FocusChanged(TDrawNow aDrawNow);

public:    // CCoeControl
    /**
     * Handles resource changes.
     * @param aType Resource change type.
     **/
    virtual void HandleResourceChange(TInt aType);
    
    /**
     * Handles command to stop the alarm ,in case of autosnooze for MSK case.
     **/
    virtual void HandleStopCommandL(){};

protected:  // Data
    CCalenNativeView* iView;

    // main area background skin for all views
    CAknsBasicBackgroundControlContext* iBgContext;
    // week and month view heading pane skin
    CAknsFrameBackgroundControlContext* iHeadingBgContext;
    // week and month view side pane skin
    CAknsFrameBackgroundControlContext* iSideBgContext;

    // flag for touch ui long tap functionality
    TBool    iFirstTap;
    CAknLongTapDetector* iLongTapDetector; 



protected:
    // Control to show UI extension data. May be either a preview pane or an info bar, or NULL
    CCoeControl* iUiExtension;
    
    MCalenServices& iServices;
    RArray<MCalenServices::TCalenIcons> iIconIndices;
    CCalenContainerLayoutManager* iLayoutManager;
    };

#endif // CALENCONTAINER_H


// End of File
