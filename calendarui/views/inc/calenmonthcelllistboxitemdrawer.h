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
* Description:   Prepareing to draw a cell(setting color and underline).
 *                Moreover, drawing cell's bottom and right lines.
 *
*/



#ifndef CALENMONTHCELLLISTBOXITEMDRAWER_H
#define CALENMONTHCELLLISTBOXITEMDRAWER_H

//  INCLUDES
#include <eikfrlb.h>

// FORWARD DECLARATIONS
class CCalenMonthGrid;

// CLASS DECLARATION

/**
 *  Prepareing to draw a cell(setting color and underline).
 *  And drawing cell's bottom and right lines.
 */
NONSHARABLE_CLASS( CCalenMonthCellListBoxItemDrawer )
    : public CFormattedCellListBoxItemDrawer
    {
public:  // Constructors and destructor

    /**
     * C++ constructor.
     */
    CCalenMonthCellListBoxItemDrawer(
        MTextListBoxModel* aTextListBoxModel,
        CCalenMonthGrid* aCalendarMonthGrid,
        const CFont* aFont,
        CFormattedCellListBoxData* aFormattedCellData);
    /**
     * Destructor.
     */
    virtual ~CCalenMonthCellListBoxItemDrawer();

private: // from CTextListItemDrawer
    /**
     * From CTextListItemDrawer
     * Prepareing to draw a cell(setting color and underline).
     * And drawing cell's bottom and right lines.
     */
    void DrawItemText(TInt aItemIndex,
                      const TRect& aItemTextRect,
                      TBool aItemIsCurrent,
                      TBool aViewIsEmphasized,
                      TBool aItemIsSelected) const;

private: // new data
    CCalenMonthGrid* iCalendarMonthGrid;
    };

#endif // CALENMONTHCELLLISTBOXITEMDRAWER_H


// End of File
