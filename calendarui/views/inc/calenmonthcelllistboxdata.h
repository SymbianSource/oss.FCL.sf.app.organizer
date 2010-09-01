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
* Description:   Drawing Cell of Month View of calendar application.
 *
*/



#ifndef CALENMONTHCELLLISTBOXDATA_H
#define CALENMONTHCELLLISTBOXDATA_H

//  INCLUDES
#include <eikfrlbd.h>

// CLASS DECLARATION

/**
 *  Drawing Cell of Month View of calendar application
 */
NONSHARABLE_CLASS( CCalenMonthCellListBoxData ) : public CFormattedCellListBoxData
    {
public:     // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CCalenMonthCellListBoxData* NewL();

    /**
     * Destructor.
     */
    virtual ~CCalenMonthCellListBoxData();

private:    // Constructors and destructor
    /**
     * C++ default constructor.
     */
    CCalenMonthCellListBoxData();

private:    // new function
    /**
     * Draw date and bitmap of Month Grid .
     * @param aProperties Item margin, etc...
     * @param aGc Graphic context
     * @param aText Drawing text
     * @param aItemRect item rect
     * @param aColors Drawing colors
     */
    void DrawFormatted(TListItemProperties aProperties,
                       CWindowGc& aGc, const TDesC* aText,
                       const TRect& aItemRect, TBool aHighlight,
                       const TColors& aColors) const;
    /**
     * an ordinal number of last subcell of cell
     * @return 2(constant)
     */
    TInt LastSubCell() const;

private:     // From CFormattedCellListBoxData
    /**
     * From CFormattedCellListBoxData Draw a cell of Month Grid
     * @param aProperties Item margin, etc...
     * @param aGc Graphic context
     * @param aText Drawing text
     * @param aItemRect item rect
     * @param aColors Drawing colors
     */
    void Draw(TListItemProperties aProperties,
              CWindowGc& aGc, const TDesC* aText,
              const TRect& aRect, TBool aHighlight,
              const TColors& aColors) const;

private: // new data

    TBool iSkinHighlightDrawn;

    };

#endif  // CALENMONTHCELLLISTBOXDATA_H


// End of File
