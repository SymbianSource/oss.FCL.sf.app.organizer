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
* Description:   Customized drawing of CCalenWeekLBData.
 *                DrawItemText() passes extended parameters to CCalenWeekLBData.
 *
*/



#ifndef CALENWEEKLBITEMDRAWER_H
#define CALENWEEKLBITEMDRAWER_H

//  INCLUDES
#include <eikfrlb.h>

// FORWARD DECLARATIONS
class CEikListBox;
struct SAknLayoutGfx;
class CFbsBitmap;
class TPoint;
class CCalenWeekListbox;

//  CLASS DEFINITIONS

/**
 *  Customized drawing of CCalenWeekLBData.
 *  DrawItemText() passes extended parameter to CCalenWeekLBData
 */
NONSHARABLE_CLASS( CCalenWeekListBoxItemDrawer )
    : public CFormattedCellListBoxItemDrawer
    {
public:     // New function
    /**
     * C++ constructor.
     * @param aTextListBoxModel Listbox data Listbox
     * @param aFont Drawing font
     * @param aFormattedCellData Data drawing class
     * @param aListBox Listbox class
     */
    CCalenWeekListBoxItemDrawer(MTextListBoxModel* aTextListBoxModel,
                                const CFont* aFont,
                                CFormattedCellListBoxData* aFormattedCellData);
    /**
     * Destructor.
     */
    virtual ~CCalenWeekListBoxItemDrawer();

private:   // from CTextListItemDrawer
    /**
     * From CTextListItemDrawer
     * Passes a parameter to CFormattedCellListBoxData.
     */
    void DrawItemText(TInt aItemIndex, const TRect& aItemTextRect,
                      TBool aItemIsCurrent, TBool aViewIsEmphasized,
                      TBool aItemIsSelected) const;
    };

#endif // CALENWEEKLBITEMDRAWER_H


// End of File
