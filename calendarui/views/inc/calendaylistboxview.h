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
* Description:   ListBox View class. The class has a special drawing and
 *                special cusor moving.
 *
*/



#ifndef CALENDAYLISTBOXVIEW_H
#define CALENDAYLISTBOXVIEW_H

// INCLUDE
#include <eikfrlb.h>

// CLASS DECLARATION

/**
 *  ListBox View class
 *  The class has a special drawing and special cusor moving.
 */

NONSHARABLE_CLASS( CCalenDayListBoxView ) : public CFormattedCellListBoxView
    {
public:  // Constructors and destructor

    /**
     * C++ constructor.
     * @param aParent is a parent control (CEikListbox) 
     */
    CCalenDayListBoxView(CCoeControl* aParent);

    ~CCalenDayListBoxView();

public:   // New functions
    /**
     * Sets a array for dividing a cell
     * @param aArray has a information for dividing a cell
     */
    void SetCellArray(CArrayFix<TInt>* aArray);

    /**
     * @param line of cell 
     * @returns first line of cell that given line belongs
     */
    TInt FirstLineOfCell(TInt aLine) const;

    TInt LastLineOfCell(TInt aLine) const;


    /**
     * Moves focus to new position.
     * @param aItemIndex Index of new position
     */
    void MoveToItemL(TInt aItemIndex);


    /**
     * Size of possibly multiline entry
     * @param aItemIndex Index of an item
     * @return Size of day view item
     */
    TSize ItemSize(TInt aItemIndex) const;

public:  // New functions
    /**
     * Checks the line whether the ternimation in a cell.
     * @param aItemIndex is checked item index
     */
    TBool IsLastLineOfCell(TInt aItemIndex) const;
    /**
     * Checks the line whether the top in a cell.
     * @param aItemIndex is checked item index
     */
    TBool IsFirstLineOfCell(TInt aItemIndex) const;

private:  
    /**
     * Draws a whole cell
     * @param aIndex is a top line in a cell
     */
    void DrawCell(TInt aIndex);

    /**
     * Calculates  rectangle for visible calendar event (1-4 lines)
     */
    TRect DayViewItemRect(TInt aItemIndex) const;

    /** 
     * Moves focus to item and ensures that whole item is visible
     */
    void ScrollToItemL(TInt aItemIndex, TSelectionMode aSelectionMode);

    /* Just a wrapper for leaving functions */
    void DrawItemL(TInt aItemIndex) const;

private:  // from CEikListBoxView
    /**
     * From CEikListBoxView
     * Draws a Listbox's item
     * @param aItemIndex is drawing item index
     */
    void DrawItem(TInt aItemIndex) const;
    /**
     * From CEikListBoxView
     * Moves the cursor
     * @param aCursorMovement is a cursor movement
     * @param aSelectionMode is a selection mode
     */
    void MoveCursorL(TCursorMovement aCursorMovement,
                     TSelectionMode aSelectionMode);

private: // Data
    CArrayFix<TInt>* iCellArray; // cache
    CCoeControl* iParent;
    };

#endif // CALENDAYLISTBOXVIEW_H

// End of File
