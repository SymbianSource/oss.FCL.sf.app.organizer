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
* Description:   The ListBox class's virtual functions.
 *                MakeViewClassInstanceL() are overridden by the
 *                Day View's ListBox.
 *
*/



#ifndef CALENDAYLISTBOX_H
#define CALENDAYLISTBOX_H

//  INCLUDES
#include <coecntrl.h>
#include <eikfrlb.h>
#include "CalendarVariant.hrh"

// FORWARD DECLARATIONS
class TAknWindowLineLayout;
class TAknTextLineLayout;
class TAknLayoutRect;
class TAknLayoutText;
class MCalenServices;
class CCalenDayContainer;
//  CLASS DEFINITIONS

/**
 *  DayView's ListBox class
 *  MakeViewClassInstanceL() is overriding.
 *  Each relative ListBox class is derived
 */
NONSHARABLE_CLASS( CCalenDayListBox ) : public CEikFormattedCellListBox
    {
public:  // Factory method 
    static CCalenDayListBox* NewL(
        const CCalenDayContainer* aDayContainer,
        MCalenServices& aServices );

public: // static methods 
    static void ReadLayout(TInt aLayoutVariant,
                           TLocale& aLocale,
                           TAknLayoutRect& aGfxA,
                           TAknLayoutText& aTextA,
                           TAknLayoutText& aTextB,
                           TAknLayoutText& aTextC,
                           TAknLayoutRect& aIconA,
                           TAknLayoutRect& aIconB,
                           TBool aMidnightVisualization = EFalse);

    static void ReadLayout(TLocale& aLocale,
                           TAknWindowLineLayout& aGfxA,
                           TAknTextLineLayout& aTextA,
                           TAknTextLineLayout& aTextB,
                           TAknTextLineLayout& aTextC,
                           TAknWindowLineLayout& aIconA,
                           TAknWindowLineLayout& aIconB,
                           TBool aMidnightVisualization = EFalse);

public: // from CEikListBox
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);


    /**
     * From CCoeControl
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);


private:    // From CEikFormattedCellListBox
    /**
     * From CEikFormattedCellListBox
     * Create ListBoxData and ListBoxItemDrawer.
     */
    void CreateItemDrawerL();

private:    // From CCoeControl
    /**
     * From CCoeControl Calls when this object size was changed
     */
    void SizeChanged();
    /**
     * From CCoeControl Returns minimum size of control
     */
    TSize MinimumSize();
    /**
     * From CCoeControl Draws itself
     */
    void Draw(const TRect& aRect) const;
   
    /**
     * From CCoeControl Handle global resource change
     */
    void HandleResourceChange(TInt aType);

    /**
     * From CCoeControl Supplies skin backround context
     */
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

private:    // From CEikColumnListBox
    /**
     * From ?base_class Creates CListBoxView class
     * @return CListBoxView pointer
     */
    CListBoxView* MakeViewClassInstanceL();

private:    // New function

#ifdef RD_CALEN_MIDNIGHT_VISUALIZATION    

    /* read _t2, _t4, _t1 fields for list_cale_pane */
    static void ReadTextLayoutForMidnight(TLocale& aLocale,
                                          TAknTextLineLayout& aTextA,
                                          TAknTextLineLayout& aTextB,
                                          TAknTextLineLayout& aTextC);

#endif // RD_CALEN_MIDNIGHT_VISUALIZATION    

    /* read _t2, _t4, _t1 fields for list_cale_pane */
    static void ReadTextLayoutNoMidnight(TLocale& aLocale,    
                                           TAknTextLineLayout& aTextA,
                                           TAknTextLineLayout& aTextB,
                                           TAknTextLineLayout& aTextC);                               
    /**
     * Handles size change event
     */
    void HandleSizeChangedL();

    /**
     * Handles list looping from bottom to top.
     */ 
    TKeyResponse HandleDownwardLoopingL(const TKeyEvent& aKeyEvent);

    /**
     * C++ default constructor.
     */
    CCalenDayListBox( MCalenServices& aServices );

    void ConstructL(const CCalenDayContainer* aDayContainer);
private:    // Data
    // not own
    MCalenServices& iServices;

public:
    // not own.
    const CCalenDayContainer* iDayContainer;
    };

#endif // CALENDAYLISTBOX_H

// End of File
