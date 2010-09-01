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
* Description:   Drawing WeekView Listbox data.
 *                Event bar and Event icon, event indicator
 *
*/



#ifndef CALENWEEKLBDATA_H
#define CALENWEEKLBDATA_H

//  INCLUDES
#include <eikfrlbd.h>

// FORWARD DECLARATIONS
class TCalenWeekHour;
class MAknsSkinInstance;
class MAknsControlContext;

// CLASS DECLARATION

/**
 *  Drawing WeekView Listbox data.
 *  Event bar and Event icon, event indicator
 */
NONSHARABLE_CLASS( CCalenWeekLBData )
    : public CFormattedCellListBoxData
    {
public:  // Constructors and destructor
    /**
     * Destructor.
     */
    virtual ~CCalenWeekLBData();

    /**
     * Two-phased constructor.
     */
    static CCalenWeekLBData* NewL();

public:     // New function
    /**
     * Set current sub cell position.
     * @param aCurrentSubCellRect  Rect of the cell.
     * @param aCurrentItemPos      Position of the cell.
     */
    void CurrentSubCellPos( TRect& aCurrentSubCellRect, const TPoint& aCurrentItemPos );

    /**
     * Set highlight of row.
     * @param aHilightedSubCell cell to highlight
     */
    void SetHilightedSubCell(TInt aHilightedSubCell);
    /**
     * Return highlight of row.
     * @return highlighted cell
     */
    TInt HilightedSubCell();

    /**
     * Set count of row
     * @param aCount count of row
     */
    void SetCelCount(TInt aCount);
    
    /**
     * Set layout variant index
     * @param aLayout used layout
     */
    void SetVariantIndex(TInt aLayout);
    
    /**
     * Return count of row
     * @return count of row
     */
    TInt CellCount() const;

    /**
     * Drawing listbox item and highlight
     * @param aProperties Item margin, etc...
     * @param aGc Graphic context
     * @param aText Drawing text
     * @param aItemRect item rect
     * @param aColors Drawing colors
     */
    void Draw(TListItemProperties aProperties, CWindowGc& aGc,
              const TDesC* aText, const TRect& aRect, TBool aHighlight,
              const TColors& aColors) const;

private:    // Constructors and destructor
    /**
     * C++ Constructor.
     */
    CCalenWeekLBData();

private:    // New function
    /**
     * Drawing listbox item
     * @param aProperties Item margin, etc...
     * @param aGc Graphic context
     * @param aText Drawing text
     * @param aItemRect item rect
     * @param aColors Drawing colors
     */
    void DrawFormatted(TListItemProperties aProperties, CWindowGc& aGc,
                       const TDesC* aText, const TRect& aItemRect,
                       const TColors& aColors, TBool aHighlight) const;
    /**
     * Drawing cell text
     * @param aProperties Item margin, etc...
     * @param aSubCell Row index
     * @param aGc Graphic context
     * @param aText Drawing cell text
     * @param aCellRect Cell rect
     * @param aTextRect Text rect
     */
    void DrawCellText(TListItemProperties aProperties, TInt aSubCell,
                      CWindowGc& aGc, const TPtrC aText, const TRect& aCellRect,
                      const TRect& aTextRect, const TColors& aColors) const;
    /**
     * Drawing meeting note bar
     * @param aGc Graphic context
     * @param aHourData Hour schedule
     * @param aCellRect Cell rect
     */
    void DrawDurationBar(CWindowGc& aGc,
                         const TCalenWeekHour& aHourData,
                         const TRect& aCellRect) const;

    /**
     * Drawing non-timed note type icon
     * @param aGc Graphic context
     * @param aHourData Hour schedule
     * @param aCellRect Cell rect
     */
    void DrawIcon(CWindowGc& aGc,
                  const TCalenWeekHour& aHourData, 
                  const TRect& aCellRect) const;

    TBool DrawHighlight(const TRect aItemRect, CWindowGc& aGc, const TBool aDrawSkinned) const;
    
    /**
     *  Initialise Pattern at the construction period
     */
    void InitialisePatternL();
private:    // data
    TInt iLayoutVariant;
    TInt iHilightedSubCell;
    TInt iCellCount;
    CFbsBitmap* iGradientBitmap;
    };

#endif // CALENWEEKLBDATA_H


// End of File
