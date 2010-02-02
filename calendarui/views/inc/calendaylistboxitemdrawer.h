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
* Description:   Customized drawing of CalenDayListBoxData.
 *                DrawItemText() passes extended parameters to CCalenDayListBoxData.
 *
*/



#ifndef CALENDAYLISTBOXITEMDRAWER_H
#define CALENDAYLISTBOXITEMDRAWER_H

//  INCLUDES
#include <eikfrlb.h>


// FORWARD DECLARATIONS
class CEikListBox;
struct SAknLayoutGfx;

//  CLASS DEFINITIONS

/**
 *  Customized drawing of CCalenDayListBoxData.
 *  DrawItemText() passes extended parameter to CCalenDayListBoxData
 */
NONSHARABLE_CLASS( CCalenDayListBoxItemDrawer ) : public CFormattedCellListBoxItemDrawer
    {
public:     // New function
    /**
     * C++ constructor.
     * @param aTextListBoxModel Listbox data Listbox
     * @param aFont Drawing font
     * @param aFormattedCellData Data drawing class
     * @param aListBox Listbox class
     */
    CCalenDayListBoxItemDrawer(MTextListBoxModel* aTextListBoxModel,
                                        const CFont* aFont,
                                        CFormattedCellListBoxData* aFormattedCellData,
                                        CEikListBox* aListBox);
    /**
     * Destructor.
     */
    virtual ~CCalenDayListBoxItemDrawer();

private:   // from CTextListItemDrawer
    /**
     * From CTextListItemDrawer
     * Passes a parameter to CFormattedCellListBoxData.
     */
    void DrawItemText(TInt aItemIndex, const TRect& aItemTextRect, TBool aItemIsCurrent, TBool aViewIsEmphasized, TBool aItemIsSelected) const;

private:    // data
    CEikListBox* iListBox;
    mutable TUint32 iColor;
    };

#endif // CALENDAYLISTBOXITEMDRAWER_H

// End of File
