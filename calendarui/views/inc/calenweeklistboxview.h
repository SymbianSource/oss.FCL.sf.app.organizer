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
* Description:   CCalenWeekLBView customized key action of
 *                CFormattedCellListBoxView, and customized scrolling
 *                for event indicator.
 *
*/



#ifndef CALENWEEKLBVIEW_H
#define CALENWEEKLBVIEW_H

//  INCLUDES
#include <eikfrlb.h>
#include <eikfrlbd.h> // For MListBoxAnimBackgroundDrawer

//  CLASS DEFINITIONS

/**
 *  Customized key action of CFormattedCellListBoxView.
 *  Customized scrolling for event indicator
 */
NONSHARABLE_CLASS( CCalenWeekLBView ) :
    public CFormattedCellListBoxView,
    public MListBoxAnimBackgroundDrawer
    {
public:  // Constructors and destructor

    /**
     * C++ default constructor.
     */
    CCalenWeekLBView();
    /**
     * Destructor.
     */
    virtual ~CCalenWeekLBView();

public:     // New function
    /**
     * Sets current Column
     * @param aColumn New current colunm
     * @param aRedraw ETrue : Draw now
     */
    void SetColumn(TInt aColumn, TBool aRedraw = EFalse);
    /**
     * Gets current Column
     * @return current Column
     */
    TInt Column() const;

    // Implementation of MListBoxAnimBackgroundDrawer
    TBool DrawHighlightAnimBackground( CFbsBitGc& aGc ) const;


    /**
     * Handle move from pointer event (touch ui)
     */
    void PointerMoveToItemL(TInt aNewColumn);


private:    // New function
    /**
     * Calls form MoveCursorL().
     * Left/Right arrow key handing function
     * @param aTargetSubCellIndex New current colunm
     * @param TSelectionMode ListBox selection mode
     */
    void HorizontalMoveToItemL(TInt aTargetSubCellIndex,
                               TSelectionMode aSelectionMode);
    /**
     * Check column existence
     * @param aColumn Column index
     * @return ETrue : A column exists
     */
    TBool ColumnExist(TInt aColumn);

private:    // from CEikListBoxView
    /**
     * From CEikListBoxView Handling cursor move
     */
    void MoveCursorL(TCursorMovement aCursorMovement,
                     TSelectionMode aSelectionMode);
    /**
     * From CEikListBoxView Drawing all visible item
     */
    //lint -e{1735} suppressed: default parameter is in original definition 
    void Draw(const TRect* aClipRect = NULL) const;
    /**
     * From CEikListBoxView Drawing item
     */
    void DrawItem(TInt aItemIndex) const;

private:    // data
    TInt iColumn;
    };

#endif // CALENWEEKLBVIEW_H


// End of File
