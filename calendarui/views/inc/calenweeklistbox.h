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
* Description:   CCalenWeekListbox is the class that customizes drawing
 *                part of CEikFormattedCellListBox.
 *
*/



#ifndef CALENWEEKLISTBOX_H
#define CALENWEEKLISTBOX_H

//  INCLUDES
#include <eikfrlb.h>
#include <gdi.h>

// FORWARD DECLARATIONS
class CAknsFrameBackgroundControlContext;
class CCalenWeekContainer;
class CGulIcon; 
class MCalenWeekCursorObserver;


//  CLASS DEFINITIONS

/**
 *  CCalenWeekListbox is the class that customizes drawing
 *  part of CEikFormattedCellListBox.
 */
NONSHARABLE_CLASS( CCalenWeekListbox ) : public CEikFormattedCellListBox
    {
public:  // Constructors and destructor

    /**
     * C++ default constructor.
     */
    CCalenWeekListbox(CCalenWeekContainer& aWeekContainer);

    /**
     * Symbian OS constructor
     * @param aParent Parent container control
     */
    //lint -e{1411} Upperlevel ConstructL can be hidden as it is never called
    void ConstructL(const CCoeControl* aParent);

    /**
     * Destructor.
     */
    virtual ~CCalenWeekListbox();
public:     // New function
    /**
     * Set cursor handling class
     * @param aCursorObsever Cursor handling observer
     */
    void SetCursorObserver(MCalenWeekCursorObserver* aCursorObsever);
    /**
     * Set the start of a week
     * @param aStartDay start of a week
     */
    void SetStartDay(TTime aStartDay);

    /**
     * Set background control context for skinning.
     */
    void SetBgContext(MAknsControlContext* aCc);

    TRect LayoutRect() const;
public:     // From CCoeControl
    /**
     * From CCoeControl, handles key events.
     * @param aKeyEvent Event to handled.
     * @param aType Type of the key event.
     * @return Reponse code (EKeyWasConsumed, EKeyWasNotConsumed).
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                TEventCode aType);


    /**
     * checks if pointer event was done in allowed area
     */
    TBool IsValidPointer(TPoint aPos);
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    /**
     * Calculates new column from pointer event
     */
    TInt PointerEventColumn(TPoint aPos);


protected: // from MObjectProvider
    virtual TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

private:    // New function
    /**
     * Set all cell data
     */
    void SetupLayoutL();

    /**
     * Set a Background color for all subcells
     */
    void SetSubcellColorsL() const;

private:    // From CEikFormattedCellListBox
    /**
     * From CEikFormattedCellListBox
     * Create CColumnListBoxData and CColumnListBoxItemDrawer.
     */
    void CreateItemDrawerL();

private:    // from CEikListBox
    /**
     * From CEikListBox Create ListBoxView class
     */
    CListBoxView* MakeViewClassInstanceL();
    /**
     * From CEikListBox Called when Left arrow key was pressed
     */
    void HandleLeftArrowKeyL(CListBoxView::TSelectionMode aSelectionMode);
    /**
     * From CEikListBox Called when Right arrow key was pressed
     */
    void HandleRightArrowKeyL(CListBoxView::TSelectionMode aSelectionMode);

private:    // From CCoeControl
    /**
     * From CCoeControl Initilizes ListBox cell
     */
    void SizeChanged();

private:    // Data
    CCalenWeekContainer& iWeekContainer;

    MCalenWeekCursorObserver* iCursorObsever;
    TTime iStartDay;
    MAknsControlContext* iBgContext;  // not owned
    };

#endif // CALENWEEKLISTBOX_H


// End of File
