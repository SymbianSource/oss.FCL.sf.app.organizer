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
* Description:  Container for To-Do view's controls.
*
*/


#ifndef TODOCONTAINER_H
#define TODOCONTAINER_H

//  INCLUDES
#include <aknlists.h>
#include "calencontainer.h"

//  CONSTANTS

// FORWARD DECLARATIONS
class CAgnTodo;
class CCalenTodoController;
class CEikColumnListBox;
class TAknsItemID;
class CAknIconArray;
class CAknSingleGraphicHeadingStyleListBox;
class CCalInstance;

// CLASS DECLARATION

/**
*  Extended class for itemDrawer from CSingleHeadingStyleItemDrawer
*  We need this since itemDrawer extends the DrawItem function.
*  some extra functinalities are done in this DrawItem.
*/
NONSHARABLE_CLASS( CCalenTodoListBoxItemDrawer ) : public CSingleHeadingStyleItemDrawer
    {
    
    public:
    CCalenTodoListBoxItemDrawer( MTextListBoxModel* aTextListBoxModel,
                                     const CFont* aFont,
                                     CColumnListBoxData* aFormattedCellData,
                                     CEikListBox* aListBox ,
                                     const RArray<TInt>& aColorUidArray);
    
    ~CCalenTodoListBoxItemDrawer();

    private: // from CSingleHeadingStyleItemDrawer
    void DrawItem(TInt aItemIndex, 
                  TPoint aItemRectPos, 
                  TBool aItemIsSelected, 
                  TBool aItemIsCurrent, 
                  TBool aViewIsEmphasized,
                  TBool aViewIsDimmed) const;
    
    private:
    CEikListBox* iListBox; //Not owns
    const RArray<TInt>& iColorUidArray; //Not owns
    };

/**
*  Extension class for todo listbox  ( CAknSingleGraphicHeadingStyleListBox )
*  Only item drawer , column data and listbox view is created in this class 
*/
NONSHARABLE_CLASS( CCalenTodoListBox ) : public CAknSingleGraphicHeadingStyleListBox
    {   
    public:
        CCalenTodoListBox(const RArray<TInt>& aColorUidArray);
    
    private: //from CAknSingleGraphicHeadingStyleListBox
        CListBoxView* MakeViewClassInstanceL();
        void CreateItemDrawerL();
        void SizeChanged();
        
    private:
        void SizeChangedL();
        
    private:
    const RArray<TInt>& iColorUidArray;//Not owns
    };

/**
*  Container base class of ToDo application.
*/
NONSHARABLE_CLASS( CCalenTodoContainer ) : public CCalenContainer,
                                      public MEikListBoxObserver
    {
    public:     // New function
        /**
        * C++ constructor.
        */
        CCalenTodoContainer( CCalenNativeView* aView,
                             TInt& aHighlightedRowNumber,
                             TInt& aFirstEntryOnScreenIndex,
                             MCalenServices& aServices );

        /**
        * Destructor.
        */
        virtual ~CCalenTodoContainer();
    public:
        /**
         * Handles list box events.
         * @param aListBox   The originating list box.
         * @param aEventType A code for the event.
         */
        void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

    public: // New functions
        /**
        * Third phase constructor.
        */
        void ConstructImplL();
        
        /**
         * Step 1 of construction of view data.
         */
        void BeginPopulationWithInstanceViewL();

        /**
         * Step 2 of construction of view data.
         */
        void CompletePopulationL();

        /**
        * Return a focused item index.
        * @return Item index.       (>= 0)
        *         Item is empty.    (<  0)
        */
        TInt CurrentItemIndex() const;

        /**
        * Return pointer of controler for Agenda server.
        * @return CCalenTodoController pointer
        */
        CCalenTodoController* Controller();

        /**
        * Set current item index.
        * @param aIndex Item index
        */
        void SetCurrentItemIndex(TInt aIndex);

        /**
        * Return list box pointer.
        * @return List box pointer.
        */
        CEikColumnListBox* ListBox() const;

        /**
        * Return marked count of list box.
        * @return Marked count.
        */
        TInt MarkedCount();

        /**
         * @param aMark Select to mark or unmark.
         **/
        void MarkAllL(TBool aMark);

        /**
         * @param aMark Select to mark or unmark.
         **/
        void MarkCurrentL(TBool aMark);

        /**
         * Checks if currently focused item is selected(marked).
         **/
        TBool IsCurrentItemSelected(void);
        
        /**
         * Checks if view is empty or not
         * @return ETrue if view is empty.
         **/
        TBool IsEmptyView() const;
        
        /**
        * Get the available preview pane rect for this container
        */
        TRect PreviewRectL();


        /**
         * From CCalenContainer
         */
        void HandleNaviDecoratorEventL(TInt aDirection);
        
        void HandleLongTapEventL( const TPoint& aPenEventLocation, 
                                  const TPoint& aPenEventScreenLocation );

    private: // New functions
        /**
        * Set current item index and draw list.
        */
        void SetCurrentItemIndexAndDraw();

        /**
        * Prepare setting index of current item.
        */
        //void PrepareSetCurrentItemIndexL();

        /**
        * Create all item data and draw for list box.
        */
        void CreateEntryItertorL();

        /**
        * Make a list data of ToDo list.
        * @param aInstance Agenda instance pointer
        * @param aPtr List data pointer
        */
        void MakeListDataL(CCalInstance* aInstance, TPtr& aPtr);

        /**
        * Create skinable icons.
        * 
        */
        void CreateAndAppendIconL( const TAknsItemID& aID,
                                   const TDesC& aFileName,
                                   const TInt aBitmapId,
                                   const TInt aMaskId,
                                   CAknIconArray* aIconList);

        /**
        * Replace a white space character in text buffer
        * by blank character (" ").
        * @param aBuf Text buffer
        */
        void ReplaceToWhiteSpace(TDes& aBuf);

        /**
         * Sets the correct highlight, based on the context.
         */
        void SetHighlightingL();

    private: // Functions from base classes
        /**
        * From CCoeControl, child control was resized.
        */
        void SizeChanged();

        /**
        * From CCoeControl, return child control count.
        * @return Control count
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl, return child control pointer.
        * @param aIndex Child control index
        * @return Child control pointer.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl, process key event.
        * @param aKeyEvent The key event.
        * @param aType EEventKey | EEventKeyUp | EEventKeyDown.
        * @return EKeyWasNotConsumed : Key event was not consumed.
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                    TEventCode aType);


        /**
         * From CCoeControl, process pointer event.
         */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

        void Draw(const TRect& /*aRect*/) const;

        /**
         * Pass skin information if needed
         */
        TTypeUid::Ptr MopSupplyObject(TTypeUid aId);



        /**
        * From CCoeControl, Get help context.
        * @param aContext Help context
        */
        void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
        * From MActiveStep, reflesh ToDo data.
        */
        TInt DoStepL();

        /**
         * From CCoeControl Handles a resource relative event
         * 
         * @param aType The type of change.
         */
        void HandleResourceChange(TInt aType);

    public: // From CCalenContainer
        /**
         * This methods is called after calendar settings have changed.
         * See day, week and month container for implementation.
         */
        void UpdateSize();

        /**
         * Handle focus events 
         */
        void FocusChanged(TDrawNow aDrawNow);

        /**
        * Calling this will force to resource change to happen
        * We will use the last requested resource change type.
        */
        void ForceResourceChange();
        
        /**
         * Create icon index
         */
        void CreateIconIndicesL( RArray<MCalenServices::TCalenIcons>& aIndexArray );
        
        void CleanupInstances();
        
    private:
        
        /*
         * @brief Find calendarinfo object based on calendar name
         * 
         * @param aName calendar filename
         * @param aCalendarInfo referance for calendarinfo
         * @return TBool find status
         */
        static TBool CalendarInfoIdentifierL(const HBufC* aName,
                const CCalCalendarInfo& aCalendarInfo);

    protected:  // Data
        CCalenTodoController* iController;

    private:    // Data
        CAknSingleGraphicHeadingStyleListBox* iListBox;
        //CCalenTodoListBox* iListBox;
//        TInt iForceCurrentIndex;
        CDesCArrayFlat* iDesArray;
        
        RArray<TInt> iColorUidArray;

        enum TStepPopulate
            {
            ENothingDone,
            EPopulationDone,
            EDone
            };

        TStepPopulate iStepState;
        //HBufC* iEmptyListText;
        // view specific data, references to view data
        TInt& iHighlightedRowNumber;     // The row number highlighted.
        TInt& iFirstEntryOnScreenIndex;  // The offset of the topmost item on screen to the first entry.
    };

#endif // TODOCONTAINER_H


// End of File
