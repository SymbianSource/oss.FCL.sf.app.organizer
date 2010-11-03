/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Customized drawing of CBCPreviewListBoxData.
*                DrawItemText() passes extended parameters to CBCPreviewListBoxData.
*
*/


#ifndef CALENDAYLISTBOXITEMDRAWER_H
#define CALENDAYLISTBOXITEMDRAWER_H

//  INCLUDES
#include <eikfrlb.h>
#include <aknlists.h>
#include <calentry.h>


// FORWARD DECLARATIONS
class CEikListBox;
class CBCPreviewListbox;
struct SAknLayoutGfx;

//  CLASS DEFINITIONS 

/**
 *  Customized drawing of CCalenDayListBoxData.
 *  DrawItemText() passes extended parameter to CCalenDayListBoxData
 */
NONSHARABLE_CLASS( CBCPreviewListBoxItemDrawer ) : public CFormattedCellListBoxItemDrawer 
    {
public:     // New function
    /**
     * C++ constructor.
     * @param aTextListBoxModel Listbox data Listbox
     * @param aFont Drawing font
     * @param aFormattedCellData Data drawing class
     * @param aListBox Listbox class
     */
    CBCPreviewListBoxItemDrawer(MTextListBoxModel* aTextListBoxModel,
                                        const CFont* aFont,
                                        CFormattedCellListBoxData* aFormattedCellData,
                                        CBCPreviewListbox* aListBox);
    /**
     * Destructor.
     */
    virtual ~CBCPreviewListBoxItemDrawer();
    
public:
    /**
    * Set colors to be used instead of defaults
    */
    inline void SetSkinColors(TRgb aTextColor, TRgb aHighlightedTextColor )
    	{
    	iTColor = aTextColor;
    	iHTColor = aHighlightedTextColor;
    	}
    
private:   // from CTextListItemDrawer
    /**
     * From CTextListItemDrawer
     * Passes a parameter to CFormattedCellListBoxData.
     */
    void DrawItemText(TInt aItemIndex, const TRect& aItemTextRect, TBool aItemIsCurrent, TBool aViewIsEmphasized, TBool aItemIsSelected) const;
    
    /**
     * From CFormattedCellListBoxItemDrawer
     * Sets the item cell size. Used to increase cell size when scrollbar is not visible.
     *
     * The cell size is the on-screen size of the entire item, including its 
     * text and its item mark.
     * 
     * @param aSizeInPixels New size for the item cell.
     */
    void SetItemCellSize(const TSize& aSizeInPixels);
    
    
    /**
    * Use colors from skin
    */
	void UseSkinColors()const;
	
	/**
	 * Draw color stripe for current item.
	 * Stripe indicates in which calendar entry is saved.
	 * For MR events stripe additionaly indicates status of MR. 
	 */
	void DrawItemStripe( const TRect& aRect, TRgb aColor, 
			CCalEntry::TStatus aStatus ) const;
	

private:    // data
    CBCPreviewListbox* iListBox;
    TRgb iCurCalendarColor;
    
    TRgb iTColor;
    TRgb iHTColor;
    };

#endif // CALENDAYLISTBOXITEMDRAWER_H

// End of File
