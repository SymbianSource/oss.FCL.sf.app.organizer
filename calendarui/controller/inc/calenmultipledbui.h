/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CALENMULTIDBUI_H_
#define CALENMULTIDBUI_H_

// System includes.
#include <AknDialog.h>
#include <coemain.h>
#include <eiklbo.h>
#include <aknlists.h>
#include <calenservices.h>

#include <calennotificationhandler.h>

// User includes. 
#include "calencontroller.h" 

// Forward declarations.
class CEikColumnListBox;
class TAknsItemID;
class CAknIconArray;
class CAknSingleGraphicHeadingStyleListBox;
class CAknsBasicBackgroundControlContext;
class CAknNavigationControlContainer;
class CAknToolbar;
class CAknButton;
class CCalenMultipleDbListbox;
class CCalenMultiDBEditor;

class CCalenMultipleDbUi : public CAknDialog,
                           public MEikListBoxObserver,
                           public MCalenNotificationHandler
    {
private:
    
    /**
     * @brief Default constructor
     * 
     * @param aController reference to the controller
     */
    CCalenMultipleDbUi(CCalenController& aController );
    
    /**
     * @brief second phase constructor
     */
    void ConstructL();
    
public:
    
    /**
     * @brief first phase construction
     * 
     * @aController reference to the controller 
     */
    static CCalenMultipleDbUi* NewL( CCalenController& aController );
    
    /**
     * @brief destructor
     */
    ~CCalenMultipleDbUi();
    
    /**
    * @brief From CAknDialog.Handles focus change
    * 
    * @param aDrawNow draw status from CAknDialog.
    */ 
    void FocusChanged( TDrawNow aDrawNow );
    
protected:
    /**
    * @brief From CEikDialog.Dynamic initialization of the menu pane
    *
    * @param aResourceId menu pane resource
    * @param aMenuPane pointer to the menupane
    */ 
    void DynInitMenuPaneL( TInt aResourceId,CEikMenuPane* aMenuPane ) ;

   /**
    * @brief From MEikMenuObserver.Handles the commands in dialog
    *
    * @param aCommandId Not used.
    */ 
    void ProcessCommandL( TInt aCommandId ) ;
   
    /** 
    * @brief From CCoeControl.Handles key events.
    *
    * @aKeyEvent key event generated
    * @aType type of the event code 
    */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType) ;
   
    /** 
     * @brief From CCoeControl.Handles pointer events
     *
     * aPointerEvent reference to the pointer event
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    
    /**
     * @brief From CEikDialog.Handles a dialog button press 
     * for the specified button. 
     *
     * @aButtonId soft key pressed 
     * @return  
     */
    TBool OkToExitL( TInt aButtonId ) ;
    
    /** 
    * brief From MObjectProvider. 
    * Gets Mop supply object of the given type.  
    *
    * @param aId Identifier for the supply object.
    * @return Pointer to the supply object type ID.
    */
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    
    /**
    * brief From CEikDialog.Prepare of controls
    */
    void PreLayoutDynInitL();
    
    /**
    * From @c CEikDialog.
    * 
    **/
    void PostLayoutDynInitL();
    
    /**
    * From @c CCoeControl.
    * Called when controls extent is set.
    **/
    void SizeChanged();
    
    /**
    * From @c CCoeControl.
    **/ 
    void HandleResourceChange(TInt aType);
    
    /**
    * From @c CCoeControl.
    * 
    * @param aIndex index of the control
    * @return Pointer to the Control.
    **/
    CCoeControl* ComponentControl( TInt aIndex) const;
    
    /**
    * From @c CCoeControl.
    * 
    * @return Pointer to the Control.
    **/
    TInt CountComponentControls() const;
    
    
    /**
    * From @c CCoeControl.
    * 
    * @param aRect
    **/
    void Draw(const TRect &aRect) const;
    
    
    
private: //Internal
    
    /*
     * Construction of listbox ui
     * @return void
     */
    void ConstructListL();
    
    /* 
     * Update listbox contents 
     * @return void
     */
    void UpdateListboxL();
    
    /*
     * setup title pane for the status pane
     * @return void 
     */
    void SetTitlePaneL();  
    
    /*
     * Create icons based on MCalenServices::TCalenIcon indices
     * @param aIndexArray MCalenServices::TCalenIcons array for services
     * @return CAknIconArray* pointer to array of icons
     */
    CAknIconArray*  CreateIconsL(RArray<MCalenServices::TCalenIcons>& aIndexArray );
    
    /*
     * Used for marking list item as visible/hidden
     * @param aMark TBool based on bool value item is made visible/hidden
     * @return void
     */
    void DoSelectionL( TBool aMark );
    
    /*
     * Add new listbox item (calendar)
     * @return TInt return value of editor 
     */
    TInt AddItemL();
    
    /*
     * Editing listbox item (calendar)
     * @return TInt return value of editor
     */
    TInt EditItemL();
    
    /*
     * Delete an listbox item (calendar)
     * @return void
     */
    TInt DeleteItemL();
    /*
     * Load icons into memory
     * @return void
     */
    void PrepareIconsL();
    
    /*
     * Check if any chnages has occured before modifying an item(calendar)
     * @param aDbInfoOriginal CCalCalendarInfo object before editing
     * @param aDbInfoModified CCalCalendarInfo object after editing
     */
    TBool CheckForChangesL( CCalCalendarInfo& aCalendarInfoOriginal,
                CCalCalendarInfo& aCalendarInfoModified);
    
    /*
     * Async exit for the dialog
     */
    static TInt DoAsyncExit(TAny* aPtr);


    /*
     * @brief Creates button for the toolbar.
	 * @param aIcon Icon to be displayed for a button.
	 * @param aDisplayTextID text to be displayed for a button.
	 * @param aToolbar toolbar reference.
     */    
    CAknButton* CreateButtonL( CGulIcon* aIcon, TInt aDisplayTextID,
                               TInt aTooltipID, CAknToolbar& aToolbar );
    
    /*
     * @brief set the calendar properties for newly added calendar
     * @param aCalendarinfo a reference to CCalCalendarInfo.
     */
    void SetCalendarAddPropertiesL(CCalCalendarInfo& aCalendarInfo);

    /*
     * @brief update the calendar properties for calendar being edited.
     * @param aCalendarinfo a reference to CCalCalendarInfo.
     */
    void SetCalendarUpdatePropertiesL(CCalCalendarInfo& aCalendarInfo);
    
    /*
     * Refocus the items in list when delete action happened. 
     */
    void ReAdjustListItemFocusL(const TInt aCurrentIndex);

public:  // from MCalenNotificationHandler
	/** 
	* @brief Handle notifications
	* @param aNotification Nofication to be handled
	*/
	void HandleNotification(const TCalenNotification aNotification );
	
public:   // from MEikListBoxObserver
    /**
    * Handles list box events.
    *
    * @param aListBox   The originating list box.
    * @param aEventType A code for the event. Further information 
    *                   may be obtained by accessing the list box itself.
    */
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

private:
    CCalenMultipleDbListbox* iListBox;
    CAknsBasicBackgroundControlContext* iBgContext; //Owns
    CAknNavigationControlContainer*   iNaviContainer; //Not own
    CCalenController& iController;
    CDesCArrayFlat* iDesArray; //Owns
    TRect iRect;
    TBool iFirstTap;
    TBool iDialogLaunched;
    RArray<MCalenServices::TCalenIcons> iIconIndices;
    CAknIconArray* iIconList;
    RArray<TInt32> iColorUidArray;
    CAsyncCallBack* iAsyncExit;
    TInt iAsyncExitCmd;
    CCalenMultiDBEditor* iDbEditor;//Not not owned
    TBool iConflictOccured;
    };


NONSHARABLE_CLASS( CCalenMultipleDbListboxItemDrawer ) : public CFormattedCellListBoxItemDrawer
    {
    
    public:
        CCalenMultipleDbListboxItemDrawer( MTextListBoxModel* aTextListBoxModel,
                                               const CFont* aFont,
                                               CFormattedCellListBoxData* aFormattedCellData,
                                               CEikListBox* aListBox ,
                                               const RArray<TInt32>& aColorUidArray);
    
    ~CCalenMultipleDbListboxItemDrawer();

    private: // from CWordWrappedFormattedCellItemDrawer
    void DrawItem(TInt aItemIndex, 
                  TPoint aItemRectPos, 
                  TBool aItemIsSelected, 
                  TBool aItemIsCurrent, 
                  TBool aViewIsEmphasized,
                  TBool aViewIsDimmed) const;
    
    private:
    CEikListBox* iListBox; //Not owns
    const RArray<TInt32>& iColorUidArray; //Not owns
    CGulIcon* iIcon;
    };

NONSHARABLE_CLASS( CCalenMultipleDbListbox ): public CAknDoubleGraphicStyleListBox
    {
    public:
        CCalenMultipleDbListbox(const RArray<TInt32>& aColorUidArray);
        
    private: //from CAknDouble2GraphicStyleListBox
        //CListBoxView* MakeViewClassInstanceL();
        void CreateItemDrawerL();
        void SizeChanged();
        void SizeChangedL();  
        
    private:
        const RArray<TInt32>& iColorUidArray;//Not owns
    };

#endif /*CALENMULTIDBUI_H_*/
