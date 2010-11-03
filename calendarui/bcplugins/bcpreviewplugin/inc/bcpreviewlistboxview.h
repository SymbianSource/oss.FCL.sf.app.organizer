/*
* Copyright (c) 2010-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Class implementing the RECAL customisation interface.
*
*/

#ifndef BCPREVIEWLISTBOXVIEW_H_
#define BCPREVIEWLISTBOXVIEW_H_

#include <eikfrlb.h>

// CLASS DECLARATION
/**
 * CBCPreviewListboxView class is responsible for scrolling view vertically.
 */
class CBCPreviewListboxView : public CFormattedCellListBoxView
	{
public:
	CBCPreviewListboxView();
	virtual ~CBCPreviewListboxView();

public:
	/**
	 * Scrolls vertically by the specified number of pixels.
	 * 
	 * @param aVScrollAmount The distance to scroll by in pixels. A negative 
     *        value scrolls to the top, a positive value scrolls to the bottom.
	 */
	void VScroll(TInt aVScrollAmount);
	/**
	 * Resets vertical scrolling. Next time VScroll method starts from 
	 * the top of the list.
	 */
	void ResetVScroll();
	/**
	 * Gets the current vertical offset of the view from the top of the list 
	 * in pixels.
	 * 
	 * @return current vertical offset of the view.
	 */
	TInt VerticalTopOffset();
	/**
	 * Set the bottom item index 
	 * 
	 * @param aBottomItemIndex current bottom item index
	 */
	void SetBottomItemIndex(TInt aBottomItemIndex);
	
protected: // From CListBoxView
	/**
	 * This method is reimplemented to fix avkon error: items are not scrolling 
	 * continuously when list box has a specific height. This is temporary 
	 * solution and must be removed when avkon release their own fix.
	 *  
	 * Gets the number of items that will fit into a given rectangle.
     *
     * @param aRect The rectangle.
     * @return The number of items that will fit into the given rectangle.
	 */
	TInt NumberOfItemsThatFitInRect(const TRect& aRect) const;
	

	/**
	 * This method is reimplemented to replace empty list default text color
	 * with our own color
	 */    
	void DrawEmptyList(const TRect &aClientRect) const;
    
	
protected:
	/**
     * Current vertical offset of the view from the top of the list in pixels.
	 */
	TInt iVerticalTopOffset;
	/**
	 * Previous item top index.
	 */
	TInt iPreviousItemTopIndex;
	};

#endif /* CBCPreviewListboxView_H_ */

// End of File
