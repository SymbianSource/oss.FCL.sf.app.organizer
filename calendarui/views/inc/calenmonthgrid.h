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
* Description:   Grid of month view.
 *
*/



#ifndef CALENMONTHGRID_H
#define CALENMONTHGRID_H

//  INCLUDES
#include <AknGrid.h>

class CCalenMonthContainer;

// CLASS DECLARATION

/**
 *  Grid of month view
 */
NONSHARABLE_CLASS( CCalenMonthGrid ) : public CAknGrid 
    {
public:  // Constructors and destructor

    /**
     * C++ constructor.
     * @param aFirstDayOfGrid first day of grid
     */
    CCalenMonthGrid(TTime aFirstDayOfGrid, CCalenMonthContainer* aMonthCont);
    /**
     * Destructor.
     */
    virtual ~CCalenMonthGrid();
public:  // new functions
    /**
     * Return first day of grid
     * @return first day of grid
     */
    TTime FirstDayOfGrid();
    /**
     * Set argument aDay to first day of Grid
     * @param aDay New first day of grid
     */
    void SetFirstDayOfGrid(TTime aDay);

protected: // From CAknGrid
    /**
     * From CAknGrid Creates CFormattedCellListBoxItemDrawer,
     * actually CCalenMonthCellListBoxItemDrawer.
     */
    void CreateItemDrawerL();

    /**
     * Override default scrollbar implementation to prevent scrollbar 
     * to be drawn.
     */
    void UpdateScrollBarsL();

private: // new data
    
    /**
      * From CCoeControl drawing month view
      */
     void Draw(const TRect& /*aRect*/) const;
     
     /**
       * Draw secondary grid lines.
       */   
     void DrawGridLines() const;
     
     TTime iFirstDayOfGrid;
     
     CCalenMonthContainer* iMonthContainer;
     

    };

#endif // CALENMONTHGRID_H


// End of File
